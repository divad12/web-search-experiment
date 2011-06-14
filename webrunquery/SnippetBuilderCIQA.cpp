

//
// SnippetBuilderCIQA
//
// This code is based largely on the code I wrote for the PHP and Java
// interfaces.
//
// 17 July 2006 -- tds
//            

#include "SnippetBuilderCIQA.h"
#include <algorithm>
#include <set>

//
// _getRawNodes
//

void indri::api::SnippetBuilderCIQA::GetRawNodes( 
       std::vector<std::string>& nodeNames, 
       const indri::api::QueryAnnotationNode* node ) 
{
  if( node->type == "IndexTerm" || node->type == "OrderedWindowNode" ) {
    nodeNames.push_back( node->name );
  } else {
    for( size_t i=0; i<node->children.size(); i++ ) {
      GetRawNodes( nodeNames, node->children[i] );
    }
  }
}

//
// sort_extent_pairs
//

// Already defined in the indri library's SnippetBuilder.cpp

namespace NS_SnippetBuilderCIQA
{
bool sort_extent_pairs( const std::pair< indri::index::Extent, int >& one,
                        const std::pair< indri::index::Extent, int >& two ) {
  return one.first.beginsBefore( two.first );
}
}


//
// _documentMatches
//

std::vector< std::pair<indri::index::Extent, int> > 
indri::api::SnippetBuilderCIQA::DocumentMatches( int document, 
						  const std::map< std::string,
						  std::vector<indri::api::ScoredExtentResult> >& annotations,
						  const std::vector<std::string>& nodeNames,
						 int beginPos, int endPos) // range of desired matches
{
  std::vector< std::pair<indri::index::Extent, int> > extents;
  
  for( size_t i=0; i<nodeNames.size(); i++ ) {
    std::map< std::string, std::vector<indri::api::ScoredExtentResult> >::const_iterator iter;
    const std::string& nodeName = nodeNames[i];
    
    // are there annotations for this node?  if not, keep going
    iter = annotations.find( nodeName );
    if( iter == annotations.end() ) {
      continue;
    }
    
    // there are annotations, so get just the ones for this document
    const std::vector<indri::api::ScoredExtentResult>& matches = iter->second;
    for( size_t j=0; j<matches.size(); j++ ) {

      if( matches[j].document == document ) {
        indri::index::Extent e;
        e.begin = matches[j].begin;
        e.end = matches[j].end;

        if ( beginPos <= e.begin && endPos >= e.end )
	  extents.push_back( std::make_pair( e, i ) );
      }
    }
  }
  
  // now we have some extents; we'll sort them by start position
  std::sort( extents.begin(), extents.end(), NS_SnippetBuilderCIQA::sort_extent_pairs );
  return extents;
}

/*
bool should_skip( const std::vector< indri::api::SnippetBuilderCIQA::Region >& skips, int begin, int end ) {
  for( size_t i=0; i<skips.size(); i++ ) {
    if( skips[i].begin <= begin && skips[i].end >= end )
      return true;
  }

  return false;
}
*/
/*

//
// _bestRegion
//

indri::api::SnippetBuilderCIQA::Region indri::api::SnippetBuilderCIQA::_bestRegion(
  const std::vector< std::pair<indri::index::Extent, int> >& extents,
  const std::vector< indri::api::SnippetBuilderCIQA::Region >& skipRegions,
  int positionCount, int windowWidth ) {
  // try to find as many unique occurrences as possible
  Region best;
  size_t bestUnique = 0;
  std::set<int> bestSet;
  best.begin = 0;
  best.end = 0;

  std::vector< Region >::const_iterator skipIter = skipRegions.begin();

  for( size_t i=0; i<extents.size(); i++ ) {
    if( should_skip( skipRegions, extents[i].first.begin, extents[i].first.end ) )
      continue;

    // if this extent is past the end, it doesn't count
    if( extents[i].first.begin >= positionCount )
      break;

    // okay, now let's really look for a nice extent
    Region region;
    region.begin = extents[i].first.begin;
    region.end = extents[i].first.end;
    region.extents.push_back( extents[i].first );

    std::set<int> nodes;
    nodes.insert( extents[i].second );
    size_t j;

    for( j=i; j<extents.size(); j++ ) {
      int newEnd = std::max( extents[j].first.end, region.end );

      if( newEnd - region.begin > windowWidth || should_skip( skipRegions, extents[j].first.begin, extents[j].first.end ) )
        break;

      // remove duplicate and/or overlapping extents
      if( region.extents.back().end < extents[j].first.begin ) {
        region.extents.push_back( extents[j].first );
      } else {
        region.extents.back().end = extents[j].first.end;
      }

      nodes.insert( extents[j].second );
      region.end = newEnd;
    }

    if( bestUnique < nodes.size() ) {
      best = region;
      bestUnique = nodes.size();
      bestSet = nodes;
    }
  }

  return best;
}
*/
/*

//
// _buildRegions
//


std::vector<indri::api::SnippetBuilderCIQA::Region> indri::api::SnippetBuilderCIQA::_buildRegions(
  std::vector< std::pair<indri::index::Extent, int> >& extents,
  int positionCount, int matchWidth, int windowWidth )
{
  std::vector<indri::api::SnippetBuilderCIQA::Region> regions;
  int wordCount = 0;
  
  if( extents.size() == 0 ) 
    return regions;

  std::vector<Region> matchRegions;
  int wordsUsed = 0;

  // find the best possible extents (in terms of coverage) that we possibly can
  // bias toward the document beginning
  while( wordsUsed < windowWidth ) {
    Region matchRegion = _bestRegion( extents, matchRegions, positionCount, windowWidth - wordsUsed );
    wordsUsed += matchRegion.end - matchRegion.begin;

    if( matchRegion.end - matchRegion.begin == 0 )
      break;

    matchRegions.push_back( matchRegion );
    std::sort( matchRegions.begin(), matchRegions.end(), sort_regions );
  }

  // now we have some match regions, so put together some reasonable context for them
  // BUGBUG: need additional logic here to ensure we don't get overlap between the regions.
  for( size_t i=0; i<matchRegions.size(); i++ ) {
    matchRegions[i].begin = std::max( 0, matchRegions[i].begin - matchWidth / 2 );
    matchRegions[i].end = std::min( positionCount, matchRegions[i].end + matchWidth / 2 );
  }

  return matchRegions;
}

*/


//
// _sanitizeText
//

char* indri::api::SnippetBuilderCIQA::_sanitizeText( const char* text, int begin, int length ) {
  char* result = (char*) malloc( length+1 );
  memset( result, 0, length+1 );
  
  bool wasSpace = false;
  int end = begin+length;
  int j=0;
  
  for( int i=begin; i<end; i++ ) {
    char c = text[i];
    
    if( c == '<' ) {
      i++;

      // skip past any whitespace
      i += strspn( text + i, " \t\n\r" );
      
      if( !::strncmp( "!--", text + i, 3 ) ) {
        // in comment, search for end of it:
        i += 3;
        const char* endp = strstr( "-->", text + i );
        if( endp )
          i = endp - text + 2;
        else
          i = end;
      } else if( i < length-5 && !lemur_compat::strncasecmp( "style", text + i, 5 ) ) {
        // style tag
        const char* endp = lemur_compat::strcasestr( "</style", text + i );
        if( endp )
          endp = strchr( endp, '>' );
        if( endp )
          i = endp - text;
        else 
          i = end;
      } else if( i < length-6 && !lemur_compat::strncasecmp( "script", text + i, 6 ) ) {
        // script tag
        const char* endp = lemur_compat::strcasestr( "</script", text + i );
        if( endp )
          endp = strchr( endp, '>' );
        if( endp )
          i = endp - text;
        else
          i = end;
      } else {
        // regular old everyday tag, skip it
        const char* endp = strchr( text + i, '>' );
        if( endp )
          i = endp - text;
        else
          i = end;
      }
    } else if( isspace(c) ) {
      // allow one space between words
      if( !wasSpace ) {
        wasSpace = true;
        result[j++] = ' ';
      }
    } else {
      result[j++] = c;
      wasSpace = false;
    }
  }
  
  result[j] = 0;
  return result;
}

//
// _addHighlightedText
//

void indri::api::SnippetBuilderCIQA::_addHighlightedRegion( std::string& snippet, char* region ) {
  if( _HTMLOutput ) {
    snippet += "<strong>";
    snippet += region;
    snippet += "</strong>";
  } else {
    for( char* c = region; *c; c++ ) {
      *c = toupper(*c);
    }
    snippet += region;
  }
  free(region);
}

//
// _addUnhighlightedText
//

void indri::api::SnippetBuilderCIQA::_addUnhighlightedRegion( std::string& snippet, char* region ) {
  if( _HTMLOutput ) 
    snippet += region;
  else
    snippet += region;
  
  free(region);
}

//
// _addEllipsis
//

void indri::api::SnippetBuilderCIQA::_addEllipsis( std::string& snippet ) {
  if( _HTMLOutput )
    snippet += " <b>...</b> ";
  else
    snippet += " ... ";

}

//
// _completeSnippet
//

void indri::api::SnippetBuilderCIQA::_completeSnippet( std::string& snippet ) {
  if( _HTMLOutput )
    return;
  
  size_t i = 0;

  // add linebreaks
  while( i < snippet.size() ) {
    i += 50;
    while( i < snippet.size() && !isspace(snippet[i]) )
      i++;
    if( i < snippet.size() )
      snippet[i] = '\n';
  }
}

//
// build
//

/* 
std::string indri::api::SnippetBuilderCIQA::build( int documentID, 
						   const indri::api::ParsedDocument* document, 
						   indri::api::QueryAnnotation* annotation ) 
{
  // put together the match information we'll need later 
  int windowSize = 50;
  const char* text = document->text;
  std::vector<std::string> nodeNames;
  _getRawNodes( nodeNames, annotation->getQueryTree() );
  std::vector< std::pair<indri::index::Extent, int> > extents = 
    _documentMatches( documentID, annotation->getAnnotations(), nodeNames );
  
  if( extents.size() == 0 )
    return std::string();
  
  // now, we have to put all of these regions together into a snippet
  std::string snippet;
  int wordCount = 0;

  
  
  for( size_t i=0; i<regions.size() && windowSize > wordCount; i++ ) {
    Region& region = regions[i];
    
    if( region.begin != 0 && i == 0 ) {
      _addEllipsis( snippet );
    }
	
	  if( region.end > (int)document->positions.size() )
	    continue;
    
    int beginByte = document->positions[region.begin].begin;
    int endByte = document->positions[region.end-1].end;
    int current = beginByte;
    wordCount += region.end - region.begin;
    
    for( size_t j=0; j<region.extents.size(); j++ ) {
      int regionBegin = region.extents[j].begin;
      int regionEnd = region.extents[j].end;
	  
	  if( regionEnd > (int)document->positions.size() )
		continue;
      
      int beginMatch = document->positions[regionBegin].begin;
      int endMatch = document->positions[regionEnd-1].end;
      
      _addUnhighlightedRegion( snippet, _sanitizeText( text, current, beginMatch - current ) );
      _addHighlightedRegion( snippet, _sanitizeText( text, beginMatch, endMatch - beginMatch ) );
      
      current = endMatch;
    }
    
    _addUnhighlightedRegion( snippet, _sanitizeText( text, current, endByte - current ) );
    
    if( region.end != document->positions.size()-1 )
      _addEllipsis( snippet );
  }

  _completeSnippet( snippet );
  return snippet;
}                         

*/

std::string indri::api::SnippetBuilderCIQA::HighLightMatches( std::string const & text, 
				    std::vector< std::pair<indri::index::Extent, int> > const & matches,
				    indri::utility::greedy_vector<indri::parse::TermExtent> const & positions,
				    int textOffset ) 
{
  std::string output = "" ;
  int currByte = 0 ;
  for ( size_t i = 0 ; i < matches.size() ; ++i )
  {
    indri::index::Extent const & match = matches[i].first ;
    
    int begin = match.begin ;
    int end = match.end ;

    int beginByte = positions[begin].begin - textOffset ;
    int endByte = positions[end-1].end - textOffset ;

    output += text.substr( currByte, beginByte - currByte ) ;
    output += "<strong>" + text.substr( beginByte, endByte - beginByte ) + "</strong>" ;
    currByte = endByte ;
  }
  output += text.substr( currByte ) ;

  return output ;
}
