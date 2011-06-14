/*==========================================================================
* Copyright (c) 2004 University of Massachusetts.  All Rights Reserved.
*
* Use of the Lemur Toolkit for Language Modeling and Information Retrieval
* is subject to the terms of the software license set forth in the LICENSE
* file included with this software, and also available at
* http://www.lemurproject.org/license.html
*
*==========================================================================
*/

//
// runquery
//
// 24 February 2004 -- tds
//
// 18 August 2004 -- dam
// incorporated multiple query, query expansion, and TREC output support 
//
//
// Indri local machine query application
//
/*! \page runquery Indri Query Retrieval
<H3>QueryEnvironment Parameters</H3>
<H4>Retrieval Parameters</H4>
<dl>
<dt>memory</dt>
<dd> an integer value specifying the number of bytes to use for the
query retrieval process. The value can include a scaling factor by
adding a suffix. Valid values are (case insensitive) K = 1000, M =
1000000, G = 1000000000. So 100M would be equivalent to 100000000. The
value should contain only decimal digits and the optional
suffix. Specified as &lt;memory&gt;100M&lt;/memory&gt; in the parameter
file and as <tt>-memory=100M</tt> on the command line. </dd> 
<dt>index</dt>
<dd> path to an Indri Repository. Specified as
&lt;index&gt;/path/to/repository&lt;/index&gt; in the parameter file and
as <tt>-index=/path/to/repository</tt> on the command line. This element
can be specified multiple times to combine Repositories.
</dd>
<dt>server</dt>
<dd> hostname of a host running an Indri server (Indrid). Specified as
&lt;server&gt;hostname&lt;/server&gt; in the parameter file and
as <tt>-server=hostname</tt> on the command line. The hostname can
include an optional port number to connect to, using the form
<tt>hostname:portnum</tt>. This element
can be specified multiple times to combine servers.
</dd>
<dt>count</dt>
<dd>an integer value specifying the maximum number of results to
return for a given query. Specified as
&lt;count&gt;number&lt;/count&gt; in the parameter file and
as <tt>-count=number</tt> on the command line. </dd>
<dt>query</dt>
<dd>An indri query language query to run. This element can be specified
multiple times.
</dd>
<dt>rule</dt>
<dd>specifies the smoothing rule (TermScoreFunction) to apply. Format of
the rule is:<br> 

<tt>   ( key ":" value ) [ "," key ":" value ]* </tt>
<p>
Here's an example rule in command line format:<br>

<tt>-rule=method:linear,collectionLambda:0.2,field:title</tt>
<p> and in parameter file format:<br>
<tt>
&lt;rule&gt;method:linear,collectionLambda:0.2,field:title&lt;/rule&gt;
</tt>

<p>This corresponds to Jelinek-Mercer smoothing with background lambda
equal to 0.2, only for items in a title field. 

<p>If nothing is listed for a key, all values are assumed.
So, a rule that does not specify a field matches all fields.  This makes
<tt>-rule=method:linear,collectionLambda:0.2</tt> a valid rule. 

<p>Valid keys:
<dl>
<dt>   method</dt><dd> smoothing method (text)</dd>
<dt>   field</dt><dd> field to apply this rule to</dd>
<dt>   operator
<dd> type of item in query to apply to { term, window }</dd>
</dl>

<p>Valid methods:
<dl>
<dt>   dirichlet</dt><dd> (also 'd', 'dir') (default mu=2500)</dd>
<dt>   jelinek-mercer</dt><dd> (also 'jm', 'linear') (default
collectionLambda=0.4, documentLambda=0.0),  collectionLambda is also
known as just "lambda", either will work </dt>
<dt>   twostage</dt><dd> (also 'two-stage', 'two') (default mu=2500,
lambda=0.4)</dd> 
</dl>
If the rule doesn't parse correctly, the default is Dirichlet, mu=2500.
</dd>
<dt>stopper</dt>
<dd>a complex element containing one or more subelements named word,
specifying the stopword list to use. Specified as
&lt;stopper&gt;&lt;word&gt;stopword&lt;/word&gt;&lt;/stopper&gt; and
as <tt>-stopper.word=stopword</tt> on the command line. This is an
optional parameter with the default of no stopping.</dd>
<dt>maxWildcardTerms</dt>
<dd>
<i>(optional)</i> An integer specifying the maximum number of wildcard terms that can 
be generated for a synonym list for this query or set of queries. If this limit
is reached for a wildcard term, an exception will be thrown. If this parameter
is not specified, a default of 100 will be used.
</dd>
</dl>
<H4>Formatting Parameters</H4>
<dl>
<dt>queryOffset</dt>
<dd>an integer value specifying one less than the starting query number, eg 150 for
TREC formatted output. Specified as
&lt;queryOffset&gt;number&lt;/queryOffset&gt; in the parameter file and
as <tt>-queryOffset=number</tt> on the command line.</dd>
<dt>runID</dt>
<dd>a string specifying the id for a query run, used in TREC scorable
output. Specified as
&lt;runID&gt;someID&lt;/runID&gt; in the parameter file and
as <tt>-runID=someID</tt> on the command line.</dd>
<dt>trecFormat</dt>
<dd>the symbol <tt>true</tt> to produce TREC scorable output, otherwise
the symbol <tt>false</tt>. Specified as
&lt;trecFormat&gt;true&lt;/trecFormat&gt; in the parameter file and
as <tt>-trecFormat=true</tt> on the command line.  Note that <tt>0</tt>
can be used for false, and <tt>1</tt> can be used for true.</dd>
</dl>
<H4>Pseudo-Relevance Feedback Parameters</H4>
<dl>
<dt>fbDocs</dt>
<dd>an integer specifying the number of documents to use for
feedback. Specified as 
&lt;fbDocs&gt;number&lt;/fbDocs&gt; in the parameter file and
as <tt>-fbDocs=number</tt> on the command line.</dd>
<dt>fbTerms</dt>
<dd>an integer specifying the number of terms to use for
feedback. Specified as 
&lt;fbTerms&gt;number&lt;/fbTerms&gt; in the parameter file and
as <tt>-fbTerms=number</tt> on the command line.</dd>
<dt>fbMu</dt>
<dd>a floating point value specifying the value of mu to use for
feedback. Specified as
&lt;fbMu&gt;number&lt;/fbMu&gt; in the parameter file and
as <tt>-fbMu=number</tt> on the command line.</dd>
<dt>fbOrigWeight</dt>
<dd>a floating point value in the range [0.0..1.0] specifying the weight
for the original query in the expanded query. Specified as
&lt;fbOrigWeight&gt;number&lt;/fbOrigWeight&gt; in the parameter file and
as <tt>-fbOrigWeight=number</tt> on the command line.</dd>
</dl>
*/
#include <time.h>
#include "indri/QueryEnvironment.hpp"
#include "indri/LocalQueryServer.hpp"
#include "indri/delete_range.hpp"
#include "indri/NetworkStream.hpp"
#include "indri/NetworkMessageStream.hpp"
#include "indri/NetworkServerProxy.hpp"

#include "indri/ListIteratorNode.hpp"
#include "indri/ExtentInsideNode.hpp"
#include "indri/DocListIteratorNode.hpp"
#include "indri/FieldIteratorNode.hpp"

#include "indri/Parameters.hpp"

#include "indri/ParsedDocument.hpp"
#include "indri/Collection.hpp"
#include "indri/CompressedCollection.hpp"
#include "indri/TaggedDocumentIterator.hpp"
#include "indri/XMLNode.hpp"

#include "indri/QueryExpander.hpp"
#include "indri/RMExpander.hpp"
#include "indri/PonteExpander.hpp"

#include "indri/IndriTimer.hpp"
#include "indri/UtilityThread.hpp"
#include "indri/ScopedLock.hpp"
#include "indri/delete_range.hpp"

#include "SnippetBuilderCIQA.h"

#include <queue>
#include <cctype>
#include <string>
#include <sstream>

#include <libxml/parser.h>

// from http://techjude.blogspot.com/2006/11/neat-tostring-for-c.html
template <class T>
std::string ToString(T toBeConverted)
{ 
    std::ostringstream buffer; 
    buffer << toBeConverted; 
    return buffer.str();
}

static bool copy_parameters_to_string_vector( std::vector<std::string>& vec, 
                                             indri::api::Parameters p, 
                                             const std::string& parameterName ) 
{
    if( !p.exists(parameterName) )
        return false;

    indri::api::Parameters slice = p[parameterName];

    for( int i=0; i<slice.size(); i++ ) 
    {
        vec.push_back( slice[i] );
    }

    return true;
}

std::string ToUpper( std::string const & str )
{
    std::string result ;
    for (int i = 0 ; i < str.length() ; ++i )
    {
        result += std::toupper( str[i] ) ;
    }
    return result ;
}

struct query_t 
{
    struct greater 
    {
        bool operator() ( query_t* one, query_t* two ) 
        {
            return one->index > two->index;
        }
    };

    query_t( int _index, int _number, const std::string& _text ) :
    index( _index ), number( _number ), text( _text )
    {
        // empty
    }

    int number;
    int index;
    std::string text;
};

class QueryThread : public indri::thread::UtilityThread 
{
private:
    indri::thread::Lockable& _queueLock;
    indri::thread::ConditionVariable& _queueEvent;
    std::queue< query_t* >& _queries;
    std::priority_queue< query_t*, std::vector< query_t* >, query_t::greater >& _output;

    indri::api::QueryEnvironment _environment;
    indri::api::Parameters& _parameters;
    int _requested;
    int _initialRequested;
    int _start ; // offset into the count of results to print

    bool _printDocuments;
    bool _printPassages;
    bool _printSnippets;
    bool _printQuery;

    std::string _runID;
    bool _trecFormat;

    indri::query::QueryExpander* _expander;
    std::vector<indri::api::ScoredExtentResult> _results;
    indri::api::QueryAnnotation* _annotation;

    // for the xml output
    xmlNode * pRoot; // root element
    xmlDoc * pDoc;   // dom doc

    // okay, this is bogus, but it is the expeditious way to 
    // get this functionality into here.  This program was originally 
    // designed to handle a bunch of queries.  I use it to produce 
    // results for one query.  Well, I also want to pass into the 
    // program a "sentence query" that ranks sentence fields 
    // in a document.  This is a variant of the passed in query that 
    // will not have any filreqs.  Well, so I make this a "global"
    // variable even though it should be different for each query.  Since
    // there will only be one query, there will not be a problem....
    // -- smucker, 8/3/2007
    std::string _sentenceQuery ;
    int _numSentences ; // number of sentences to put into the snippet

    xmlNode * _appendElement( xmlNode * pNode,  
        std::string const & name, std::string const & val )
    {
        xmlNode * pNewNode = 
            xmlNewTextChild( pNode, NULL, 
            BAD_CAST name.c_str(), BAD_CAST val.c_str() ) ;
        return pNewNode ;
    }

    // Runs the query, expanding it if necessary.  Will print output as well if verbose is on.
    void _runQuery( const std::string& query ) 
    {
        try 
        {
            if( _printQuery ) 
            {
                _appendElement( pRoot, "indri-query", query ) ;
                _appendElement( pRoot, "indri-sentence-query", _sentenceQuery ) ;
            }

            _results = _environment.runQuery( query, _initialRequested );

            if( _expander ) 
            {
                std::string expandedQuery = _expander->expand( query, _results );
                if( _printQuery ) _appendElement( pRoot, "expanded-query", expandedQuery ) ;
                _results = _environment.runQuery( expandedQuery, _requested );
            }
        }
        catch( lemur::api::Exception& e )
        {
            _results.clear();
            LEMUR_RETHROW(e, "QueryThread::_runQuery Exception");
        }
    }

    void _printResultRegion( int start, int end ) 
    {
        std::vector<indri::api::ParsedDocument*> documents;
        std::vector<indri::api::ScoredExtentResult> resultSubset;

        resultSubset.assign( _results.begin() + start, _results.begin() + end );

        // Fetch document data for printing
        // Need document text, so we'll fetch the whole document
        documents = _environment.documents( resultSubset );

        // Print results
        xmlNode * pResultsNode = xmlNewChild( pRoot, NULL, BAD_CAST "results", NULL ) ;
        for( unsigned int i=0; i < resultSubset.size(); i++ ) 
        {
            int rank = start+i+1;
            int docID = resultSubset[i].document ;
            std::string docText = documents[i]->text ;
            indri::utility::greedy_vector<indri::parse::TermExtent> & docPositions = 
                documents[i]->positions  ;

            // snippet
	    int maxWords = 35 ;
	    int totalWords = 0 ;
            std::string snippet = "" ;
	    std::string snippetNoHtml = "" ; // version of snippet without highlighting
            std::vector<lemur::api::DOCID_T> docSet ;
            docSet.push_back( docID ) ;
            indri::api::QueryAnnotation * pSentAnts = 
                _environment.runAnnotatedQuery( _sentenceQuery, docSet, _numSentences ) ;
	    indri::api::SnippetBuilderCIQA snippetBuilder( true ) ;
	    std::vector<std::string>  nodeNames ;
	    snippetBuilder.GetRawNodes( nodeNames, pSentAnts->getQueryTree() ) ;
	    
            std::vector<indri::api::ScoredExtentResult> sentResults ;
            sentResults = pSentAnts->getResults() ;
            for ( size_t s = 0 ; s < sentResults.size() ; ++s )
            {
	        bool addElipse = false ;
                int beginPos = sentResults[s].begin ;
                int endPos = sentResults[s].end ;
		int numWords = endPos - beginPos ;
		if ( totalWords + numWords > maxWords )
		{
		  endPos = beginPos + maxWords - totalWords ;
		  numWords = endPos - beginPos ;
		  addElipse = true ;
		  if ( numWords == 0 )
		    break ;
		}
		totalWords += numWords ;
                int beginByte = docPositions[beginPos].begin ;
                int endByte = docPositions[endPos-1].end ;
                if ( docText[endByte] != '0' && docText[endByte] != '<' )
                {
                    ++endByte ;
                }
                std::string sentenceText = docText.substr( beginByte, endByte - beginByte ) ;
		snippetNoHtml += sentenceText + " " ;

		std::vector< std::pair<indri::index::Extent, int> > matches = 
		  snippetBuilder.DocumentMatches( docID, pSentAnts->getAnnotations(), nodeNames,
						  beginPos, endPos ) ;
		std::string hiSent =
		  snippetBuilder.HighLightMatches( sentenceText, 
						   matches, 
						   docPositions,
						   beginByte ) ;
                snippet += hiSent + " " ;
		if ( addElipse )
		  snippet += "..." ;
            }

            // metadata
            std::string docno ;
            std::string title ;
            std::string headline ;
            std::string dateline ;
            indri::utility::greedy_vector<indri::parse::MetadataPair>::iterator iter = 
                documents[i]->metadata.begin() ;
            for ( ; iter != documents[i]->metadata.end() ; ++iter )
            {
                std::string key = iter->key ;
                if ( key == "docno" )
                    docno = (char const *)iter->value ;
                else if ( key == "title" )
                    title = (char const *)iter->value ; 
                else if ( key == "headline" )
                    headline = (char const *)iter->value ; 
                else if ( key == "dateline" )
                    dateline = (char const *)iter->value ; 
            }

            if ( title.length() == 0 ) 
            {
                if ( headline.length() > 0 )
                    title = headline ;
                else if ( snippetNoHtml.length() > 0 )
                    title = snippetNoHtml.substr(0,50) + "..." ;
                else 
                    title = docno.substr(0,50) + "..." ;
            }

	    std::string date ;
	    if ( docno.length() > 0 )
	    {
	      // <DOCNO>LTW_ENG_20041001.0001</DOCNO> for AQUAINT2
	      //                012345678
	      int idxPeriod = -1 ;
	      for ( int i = 0 ; i < docno.length() ; ++i )
		{
		  if ( docno[i] == '.' )
		    {
		      idxPeriod = i ;
		    }
		}
	      if ( idxPeriod >= 8 )
		{
		  std::string packedDate = docno.substr( idxPeriod - 8, 8 ) ;
		  std::string year = packedDate.substr(  0, 4 ) ;
		  std::string month = packedDate.substr( 4, 2 ) ;
		  std::string day = packedDate.substr( 6, 2 ) ;
		  date = year + "-" + month + "-" + day ;
		}
	    }


            xmlNode * pResultNode = xmlNewChild( pResultsNode, NULL, BAD_CAST "result", NULL ) ;
            _appendElement( pResultNode, "rank", ToString(rank) ) ;
            _appendElement( pResultNode, "score", ToString(resultSubset[i].score) ) ;
            _appendElement( pResultNode, "url", docno ) ;
            _appendElement( pResultNode, "docno", docno ) ;
            _appendElement( pResultNode, "document-id", ToString(docID) ) ;
            _appendElement( pResultNode, "title", title ) ;
            //if ( dateline.length() > 0 )
            //    _appendElement( pResultNode, "dateline", dateline ) ;
	    //if ( date.length() > 0 )
            //    _appendElement( pResultNode, "date", date ) ;

            _appendElement( pResultNode, "snippet", snippet ) ;

            if( documents.size() )
                delete documents[i];
        }
    }

    void _printResults( std::stringstream& output, int start ) 
    {
        int end = _results.size() ;
        _printResultRegion( start, end );

        xmlChar * xmlbuff; 
        int buffersize;
        xmlDocDumpFormatMemory(pDoc, & xmlbuff, & buffersize, 1 );

        output << xmlbuff ; 
        xmlFree( xmlbuff );
        xmlbuff = 0 ;

        delete _annotation;
        _annotation = 0;
    }

public:
    QueryThread( std::queue< query_t* >& queries,
        std::priority_queue< query_t*, std::vector< query_t* >, 
        query_t::greater >& output,
        indri::thread::Lockable& queueLock,
        indri::thread::ConditionVariable& queueEvent,
        indri::api::Parameters& params ) :
    _queries(queries),
        _output(output),
        _queueLock(queueLock),
        _queueEvent(queueEvent),
        _parameters(params),
        _expander(0),
        _annotation(0)
    {
        //empty
    }

    ~QueryThread() 
    { 
        // empty
    }

    UINT64 initialize() 
    {
        _environment.setMemory( _parameters.get("memory", 100*1024*1024) );

        std::vector<std::string> stopwords;
        if( copy_parameters_to_string_vector( stopwords, _parameters, "stopper.word" ) )
            _environment.setStopwords(stopwords);

        std::vector<std::string> smoothingRules;
        if( copy_parameters_to_string_vector( smoothingRules, _parameters, "rule" ) )
            _environment.setScoringRules( smoothingRules );

        if( _parameters.exists( "index" ) ) 
        {
            indri::api::Parameters indexes = _parameters["index"];

            for( unsigned int i=0; i < indexes.size(); i++ ) 
            {
                _environment.addIndex( std::string(indexes[i]) );
            }
        }

        if( _parameters.exists( "server" ) ) 
        {
            indri::api::Parameters servers = _parameters["server"];

            for( unsigned int i=0; i < servers.size(); i++ ) 
            {
                _environment.addServer( std::string(servers[i]) );
            }
        }

        _requested = _parameters.get( "count", 1000 );
        _start = _parameters.get( "start", 0 );
        _initialRequested = _parameters.get( "fbDocs", _requested );
        _runID = _parameters.get( "runID", "indri" );
        _trecFormat = _parameters.get( "trecFormat" , false );
        _printQuery = _parameters.get( "printQuery", true );
        _printDocuments = _parameters.get( "printDocuments", false );
        _printPassages = _parameters.get( "printPassages", false );
        _printSnippets = _parameters.get( "printSnippets", true );        
        _sentenceQuery = _parameters.get( "sentenceQuery", "" ) ;
        _numSentences = _parameters.get( "numSentences", 2 ) ;

        if( _parameters.get( "fbDocs", 0 ) != 0 ) 
        {
            _expander = new indri::query::RMExpander( &_environment, _parameters );
        }

        return 0;
    }

    void deinitialize() 
    {
        delete _expander;
        _environment.close();
    }

    bool hasWork() 
    {
        indri::thread::ScopedLock sl( &_queueLock );
        return _queries.size() > 0;
    }

    UINT64 work() 
    {
        query_t* query;
        std::stringstream output;

        // create xml doc
        pDoc = xmlNewDoc(BAD_CAST "1.0");
        pRoot = xmlNewNode(NULL, BAD_CAST "search-response");
        xmlDocSetRootElement(pDoc, pRoot);

        // pop a query off the queue
        {
            indri::thread::ScopedLock sl( &_queueLock );
            if( _queries.size() ) 
            {
                query = _queries.front();
                _queries.pop();
            } 
            else 
            {
                return 0;
            }
        }

        // run the query
        try 
        {
            _runQuery( query->text );
        } 
        catch( lemur::api::Exception& e ) 
        {
            _appendElement( pRoot, "error", e.what() ) ;
        }

        // print the results to the output stream
        _printResults( output, _start );

        // push that data into an output queue...?
        {
            indri::thread::ScopedLock sl( &_queueLock );
            _output.push( new query_t( query->index, query->number, output.str() ) );
            _queueEvent.notifyAll();
        }

        xmlFreeDoc(pDoc);
        pDoc = 0 ;
        pRoot = 0 ;	

        delete query;
        return 0;
    }
};

void push_queue( std::queue< query_t* >& q, 
                indri::api::Parameters& queries,
                int queryOffset ) 
{
    for( int i=0; i<queries.size(); i++ ) 
    {
        int queryNumber;
        std::string queryText;

        if( queries[i].exists( "number" ) ) 
        {
            queryText = (std::string) queries[i]["text"];
            queryNumber = (int) queries[i]["number"];
        } 
        else 
        {
            queryText = (std::string) queries[i];
            queryNumber = queryOffset + i;
        }

        q.push( new query_t( i, queryNumber, queryText ) );
    }
}

int main(int argc, char * argv[]) 
{
    try 
    {
        indri::api::Parameters& param = indri::api::Parameters::instance();
        param.loadCommandLine( argc, argv );

        if( param.get( "version", 0 ) ) 
        {
            std::cout << INDRI_DISTRIBUTION << std::endl;
        }

        if( !param.exists( "query" ) )
            LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify a query." );

        if( !param.exists( "sentenceQuery" ) )
            LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify a sentenceQuery." );

        if( !param.exists("index") && !param.exists("server") ) 
            LEMUR_THROW( LEMUR_MISSING_PARAMETER_ERROR, "Must specify a server or index to query against." );

        int threadCount = 1 ; // param.get( "threads", 1 );, no need for multiple threads, only one query
        std::queue< query_t* > queries;
        std::priority_queue< query_t*, std::vector< query_t* >, query_t::greater > output;
        std::vector< QueryThread* > threads;
        indri::thread::Mutex queueLock;
        indri::thread::ConditionVariable queueEvent;

        // push all queries onto a queue
        indri::api::Parameters parameterQueries = param[ "query" ];
        int queryOffset = param.get( "queryOffset", 0 );
        push_queue( queries, parameterQueries, queryOffset );
        int queryCount = queries.size();

        if ( queryCount != 1 )
        {
            LEMUR_THROW( LEMUR_GENERIC_ERROR, "This program only supports one query." ) ;
        }

        // launch threads
        for( int i=0; i<threadCount; i++ ) 
        {
            threads.push_back( new QueryThread( queries, output, queueLock, queueEvent, param ) );
            threads.back()->start();
        }

        int query = 0;

        // acquire the lock.
        queueLock.lock();

        // process output as it appears on the queue
        while( query < queryCount ) 
        {
            query_t* result = NULL;

            // wait for something to happen
            queueEvent.wait( queueLock );

            while( output.size() && output.top()->index == query ) 
            {
                result = output.top();
                output.pop();

                queueLock.unlock();

                std::cout << result->text;
                delete result;
                query++;

                queueLock.lock();
            }
        }
        queueLock.unlock();

        // join all the threads
        for( int i=0; i<threads.size(); i++ )
            threads[i]->join();

        // we've seen all the query output now, so we can quit
        indri::utility::delete_vector_contents( threads );
    } 
    catch( lemur::api::Exception& e ) 
    {
        LEMUR_ABORT(e);
    } 
    catch( ... ) 
    {
        std::cout << "Caught unhandled exception" << std::endl;
        return -1;
    }

    return 0;
}

