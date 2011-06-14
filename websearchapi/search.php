<?php

include("include/libindri.php") ;
include("include/libciqa.php" ) ;

function main()
{
  global $indri_param ;

  if ( isset( $_REQUEST['query'] ) )
  {
    $orig_query = $_REQUEST['query'] ;
  }
  else if ( isset( $_REQUEST['search'] ) )
  {
    $orig_query = $_REQUEST['search'] ;
  }
  else
  {
    ErrorQuit( "no query" ) ;
  }

  if ( get_magic_quotes_gpc() )
  {
    $orig_query = stripslashes($orig_query) ;
  }


  // setup the results xml
  //  $pxmlDoc = new_xmldoc('1.0') ; // results xml
  $pxmlDoc = domxml_new_doc('1.0') ; // results xml
  $pRoot = $pxmlDoc->append_child( $pxmlDoc->create_element('search-response') );  

  CreateAndAppendNode( "raw-query", $orig_query, $pRoot ) ;

  $query = indri_cleanquery( $orig_query );
  $orig_queryMinus = str_replace( "+", " ", $orig_query ) ;
  $orig_queryMinus = str_replace( "\"", " ", $orig_queryMinus ) ;
  $queryMinus = indri_cleanquery_bow( $orig_queryMinus );
  $sentenceQuery = "#combine[sentence]( $queryMinus )" ;

  // run the query
  $start_time = indri_timer() ;

  $index = $indri_param[ 'index' ] ;
  $start = 0 ;
  if ( isset( $_REQUEST['startdoc'] ) )
  {
  	$start = $_REQUEST['startdoc'] ;
	if ( $start < 0 )
	{
	  $start = 0 ;
	}
	if ( $start > 1000 )
	{
	  $start = 1000 ;
	}
  }
  $numdisplay = 10 ;
  if ( isset( $_REQUEST['numdisplay'] ) )
  {
  	$numdisplay = $_REQUEST['numdisplay'] ;
	if ( $numdisplay < 1 )
	{
	  $numdisplay = 10 ;
	}
	if ( $numdisplay > 100 )
	{
	  $numdisplay = 100 ;
	}  
  }
  $count = $start + $numdisplay ;
  //  $path = "/usr/fit/tmp5/nanoadm/installs/apache/htdocs.8024/v1.5" ;
  $numSentences = 2 ;
  $shellCmd = "./webrunquery -numSentences=$numSentences -index=$index -count=$count -start=$start -query='$query' -sentenceQuery='$sentenceQuery'" ;
  $modrunqueryXML = shell_exec( $shellCmd ) ;
  $end_time = indri_timer() ;
  $total_time = $end_time - $start_time ;

  CreateAndAppendNode( "total-time", $total_time, $pRoot ) ;

  if ( ! ($pModDoc = domxml_open_mem( $modrunqueryXML )) )
  {
    ErrorQuit( "unable to parse xml from ciqarunquery. xml=$modrunqueryXML" ) ;
  }
  $pModRoot = $pModDoc->document_element() ;
  foreach ( $pModRoot->child_nodes() as $childNode )
  {
    ImportSubTree( $childNode, $pRoot ) ;
  }

  // Log stuff
  $loginfo = array() ;
  $loginfo['topicID'] = GetFromRequest("topicID", "noTopicID") ;
  //  $loginfo['sessionID'] = GetFromRequest("sessionID") ;
  $loginfo['user-query'] = $orig_query ;
  $loginfo['index'] = $index ;
  $loginfo['start'] = $start ;
  $loginfo['numdisplay'] = $numdisplay ;
  $loginfo['count'] = $count ;
  $loginfo['numSentences'] = $numSentences ;

  $eventXML = MakeXmlEvent( "search", $loginfo ) ;

  if ( ( $pEventDoc = domxml_open_mem( $eventXML ) ) )
  {
    $eventChildren = $pEventDoc->child_nodes() ;
    $pEventRoot = $eventChildren[0] ;
    ImportSubTree( $pRoot, $pEventRoot ) ; // put results into event
    $eventXML = $pEventDoc->dump_node($pEventRoot) ;
    $pEventDoc->free() ;
  }
  $topicID = $loginfo['topicID'] ;
  SyncWrite( "logs/topic-$topicID.log", "$eventXML\n" ) ;

  header("Content-type: text/xml");

  $resultXML = $pxmlDoc->dump_mem() ; 
  $pxmlDoc->free() ;
  
  print $resultXML ;

}

main() ;

?>
