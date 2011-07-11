<?php

require_once('domxml-php4-to-php5.php');

// From: http://www.phpedit.net/snippet/Remove-Invalid-XML-Characters
/**
* Removes invalid XML
*
* @access public
* @param string $value
* @return string
*/
function stripInvalidXml($value)
{
    $ret = "";
    $current;
    if (empty($value)) 
    {
        return $ret;
    }
 
    $length = strlen($value);
    for ($i=0; $i < $length; $i++)
    {
        $current = ord($value{$i});
        if (($current == 0x9) ||
            ($current == 0xA) ||
            ($current == 0xD) ||
            (($current >= 0x20) && ($current <= 0xD7FF)) ||
            (($current >= 0xE000) && ($current <= 0xFFFD)) ||
            (($current >= 0x10000) && ($current <= 0x10FFFF)))
        {
            $ret .= chr($current);
        }
        else
        {
            $ret .= " ";
        }
    }
    return $ret;
 }


function GetFromRequest( $name, $default = "" ) 
{
  $value = $default ;
  if ( isset( $_REQUEST[$name] ) )
  {
    $value = $_REQUEST[$name] ;
  }
  return $value ;
}


function CreateAndAppendNode( $nodeName, $nodeText, &$toNode )
{
  $doc = $toNode->owner_document() ;
  $newNode = $doc->create_element( $nodeName ) ;
  $toNode->append_child( $newNode ) ;
  $textNode = $doc->create_text_node( $nodeText ) ;
  $newNode->append_child( $textNode ) ;
}

function ImportSubtree( $sourceDomNode, & $destDomNode )
{
  $destDomDoc = $destDomNode->owner_document() ;
  $name = $sourceDomNode->node_name() ;
  $value = $sourceDomNode->node_value() ;
  $type = $sourceDomNode->node_type() ;
  switch ( $type )
  {
  case XML_ELEMENT_NODE:
    $destNewNode = $destDomDoc->create_element( $name ) ;
    break ;

  case XML_TEXT_NODE:
    $destNewNode = $destDomDoc->create_text_node( $value ) ;
    break ;

  case XML_COMMENT_NODE:
    $destNewNode = $destDomDoc->create_comment( $value ) ;
    break ;

  default:
    ErrorQuit( "ImportSubTree: I need to clone a node of type=$type, but I can't." ) ;
  } ;

  if ( $sourceDomNode->has_attributes() )
  {
    $attributes = $sourceDomNode->attributes() ;
    foreach ( $attributes as $attributeNode )
    {
      $atName = $attributeNode->node_name() ;
      $atValue = $attributeNode->node_value() ;
      $destNewNode->set_attribute( $atName, $atValue ) ;
    }
  }

  $destDomNode->append_child( $destNewNode ) ;
  foreach ( $sourceDomNode->child_nodes() as $childNode )
  {
    ImportSubTree( $childNode, $destNewNode ) ;
  }
}

function ErrorQuit( $message )
{
  // setup the results xml
  $pxmlDoc = domxml_new_doc('1.0') ; // results xml
  $pRoot = $pxmlDoc->append_child( $pxmlDoc->create_element('search-response') );  

  CreateAndAppendNode( "error", $message, $pRoot ) ;

  $responseXML = $pxmlDoc->dump_mem() ;

  header('Content-type: text/xml');
  echo ( $responseXML ) ; 

  exit;
}

// Take the text and simply mark it up with <strong> around the matches
// returns the marked up text
// for now, passages are not handled correctly...
//
// The idea behind textOffset is that you've passed in a piece of text 
// that actually starts textOffset bytes beyond the beginning of the doc
// text that matches and positions are based on.
function highlightMatches( $text, $matches, $positions, $textOffset=0 ) 
{
  $output = "" ;
  $currByte = 0 ;
  $textLen = strlen( $text ) ;
  for ( $i = 0 ; $i < count($matches) ; ++$i ) 
  {
    $match = $matches[$i] ;

    $begin = $match->begin ;
    $end = $match->end ;

    $beginByte = $positions[$begin]->begin - $textOffset ;
    $endByte = $positions[$end-1]->end - $textOffset ;

    if ( $beginByte >= $textLen || $endByte >= $textLen )
    {
      break ;
    }
    $output = $output . substr( $text, $currByte, $beginByte - $currByte ) ;
    $output = $output . "<strong>" . substr( $text, $beginByte, $endByte - $beginByte ) . "</strong>" ;
    $currByte = $endByte ;
  }
  $output = $output . substr( $text, $currByte ) ;

  return $output ;
}                         

// this function from: http://us2.php.net/count

function trim_text_elipse($text, $count)
{
  //Create variable
  $trimmed="";

  //Remove double white space
  $text = str_replace("  ", " ", $text);

  //Turn the text into an array
  $string = explode(" ", $text);

  //Check to see how many words there are
  $wordTotal = count($string);    
  //Check to see if there are more words than the $count variable
  if($wordTotal > $count){
    //Loop through adding words until the $count variable is reached
    for ( $wordCounter = 0; $wordCounter <= $count; $wordCounter++ ){
      $trimmed .= $string[$wordCounter];
      //Check to and add space or finish with elipse            
      if ( $wordCounter < $count ){ $trimmed .= " "; }
      else { $trimmed .= " &#8230;"; }
    }
  }else{
    //Set value returned to the existing value
    $trimmed =$text;
  }
  //Trim off any white space    
  $trimmed = trim($trimmed);
  return $trimmed;
}

function GetSingleNodeValue( $xpathQuery , $xpathContext )
{
  $result = $xpathContext->xpath_eval( $xpathQuery );
  $nodes = $result->nodeset ;
  if ( count( $nodes ) == 0 )
  {
 	return '' ;
  }
  $theNode = $nodes[0] ;
  return $theNode->get_content() ;
}

function GetStopWords()
{
  if ( !$doc = domxml_open_file("stopwords.xml") ) 
  {
    ErrorQuit( "Error while parsing stopwords.xml\n" ) ;
  }
  $xpath = $doc->xpath_init();
  $xpathContext = $doc->xpath_new_context();

  $result = $xpathContext->xpath_eval( "/parameters/stopper/word" );
  $nodes = $result->nodeset ;

  $indexes = array() ;
  foreach ($nodes as $node)
  {
    $stopwords[] = $node->get_content() ;
  }
  return $stopwords ;
}

function ExtractTagValueFromText( $documentText, $tagName, $cleanResult = false )
{
    $tagValue = "" ;
    $headStart = strpos( $documentText, "<$tagName>" ) ;
    if ( ! ( $headStart === false ) )
    {
       $headStart += strlen($tagName)+2 ;
       $headEnd = strpos( $documentText, "</$tagName>", $headStart ) ;
       $headLen = $headEnd - $headStart ;
       if ( ! ( $headEnd === false ) )
       {
          $tagValue = substr( $documentText, $headStart, $headLen ) ;
          // remove tags.
	  if ( $cleanResult )
	  {
	    $tagValue = preg_replace('@<[\/\!]*?[^<>]*?>@si', '', $tagValue);
	    $tagValue = preg_replace("/(\t|\r|\n)/", " ", $tagValue ) ;  
	    $tagValue = preg_replace('/(\\s+)/', ' ',$tagValue); // replace multi spaces with singles
	    $tagValue = ToAscii( trim( $tagValue ) ) ;
	  }
       }
    }
  return $tagValue ;
}

/**
 * Simple function to replicate PHP 5 behaviour
 */
function microtime_float()
{
  list($usec, $sec) = explode(" ", microtime());
  return ((float)$usec + (float)$sec);
}

function SyncWrite( $filename, $message )
{
  $file = fopen( $filename, "a" ) ;
  flock( $file, LOCK_EX ) ;
  fwrite( $file, $message ) ;
  flock( $file, LOCK_UN ) ;
  fclose( $file ) ;
}


function MakeXmlEvent( $eventName, $nameValuePairs )
{
  //  $pxmlDoc = new_xmldoc('1.0') ;
  $pxmlDoc = domxml_new_doc('1.0') ; // results xml
  $pRoot = $pxmlDoc->append_child( $pxmlDoc->create_element( "event" ) );  

  CreateAndAppendNode( "event-name", $eventName, $pRoot ) ;  
  CreateAndAppendNode( "date", date("r"), $pRoot ) ;  
  CreateAndAppendNode( "unixtime", microtime_float(), $pRoot ) ;  
  CreateAndAppendNode( "remote-address", gethostbyaddr( $_SERVER['REMOTE_ADDR'] ), $pRoot ) ;
  CreateAndAppendNode( "server-name", $_SERVER['SERVER_NAME'], $pRoot ) ;
  CreateAndAppendNode( "server-port", $_SERVER['SERVER_PORT'], $pRoot ) ;
  CreateAndAppendNode( "request-uri", $_SERVER['REQUEST_URI'], $pRoot ) ;

  foreach ( $nameValuePairs as $key => $value )
  {
    CreateAndAppendNode( $key, $value, $pRoot ) ;
  }

  $result = $pxmlDoc->dump_node($pRoot) ;
  $pxmlDoc->free() ;

  return $result ;
}

?>