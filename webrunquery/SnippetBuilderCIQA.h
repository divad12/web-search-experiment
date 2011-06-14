

//
// SnippetBuilderCIQA
//
// This code is based largely on the code I wrote for the PHP and Java
// interfaces.
//
// 17 July 2006 -- tds
//            

#ifndef INDRI_SNIPPETBUILDER_HPP
#define INDRI_SNIPPETBUILDER_HPP

#include <vector>
#include <string>
#include "indri/QueryAnnotation.hpp"
#include "indri/ParsedDocument.hpp"

namespace indri {
  namespace api {
    class SnippetBuilderCIQA {
    private:
      bool _HTMLOutput;

    private:  
      char* _sanitizeText( const char* text, int begin, int length );

      void _addEllipsis( std::string& snippet );
      void _addHighlightedRegion( std::string& snippet, char* region );
      void _addUnhighlightedRegion( std::string& snippet, char* region );
      void _completeSnippet( std::string& snippet );
      
    public:  
      SnippetBuilderCIQA( bool html ) : _HTMLOutput(html) {}

      void GetRawNodes( std::vector<std::string>& nodeNames, const indri::api::QueryAnnotationNode* node );
     
      std::vector< std::pair<indri::index::Extent, int> > 
	DocumentMatches( int document, 
			 const std::map< std::string, 
			 std::vector<indri::api::ScoredExtentResult> >& annotations,
			 const std::vector<std::string>& nodeNames,
			 int beginPos, int endPos); // range of desired matches

      // The idea behind textOffset is that you've passed in a piece of text 
      // that actually starts textOffset bytes beyond the beginning of the doc
      // that that matches and positions are based on
      std::string HighLightMatches( std::string const & text, 
				    std::vector< std::pair<indri::index::Extent, int> > const & matches,
				    indri::utility::greedy_vector<indri::parse::TermExtent> const & positions,
				    int textOffset = 0 ) ;
      

    };
  }
}

#endif // INDRI_SNIPPETBUILDER_HPP
