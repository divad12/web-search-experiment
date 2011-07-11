<?php 
include("include/libindri.php") ;
include("include/libciqa.php" ) ;

// make a query environment.
$env = new QueryEnvironment();
indri_setupenvironment( $indri_param, $env, $_REQUEST );
if ( isset( $_REQUEST['documentID'] ) )
{
  $numericDocID = (int) $_REQUEST['documentID'];
  $reqdocs = array( $numericDocID );
}
if ( isset( $_REQUEST['docno'] ) )
{
  $docno = $_REQUEST['docno'];
  $docnos = array($docno);
  $reqdocs = $env->documentIDsFromMetadata("docno", $docnos);
}

$docText = "Error, docText not set." ;
$docs = $env->documentsdocids( $reqdocs );
$docText = $docs[0]->text ;
    echo "<pre>\n";
    echo indri_escapetags( $docText );
    echo "</pre>\n";

?>
