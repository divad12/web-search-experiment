<?php

if (!empty($_GET['docno'])) {
  $docno = urldecode($_GET['docno']);
  // TODO: plug in proper endpoint
  $url = 'http://mansci-mark-2.uwaterloo.ca/smucker/websearchapi/showdoc.php';
  $url .= "?docno=$docno";
  $text = file_get_contents($url);
  print "<pre>$text</pre>";
} else if (!empty($_GET['url'])) {
  $url = urldecode($_GET['url']);
  $html = file_get_contents($url);
  $cmd = 'java -jar private/dist/Clueweb2trectext.jar';
  $descriptor_spec = array(0 => array('pipe', 'r'),
                          1 => array('pipe', 'w'));
  $process = proc_open($cmd, $descriptor_spec, $pipes);
  if (is_resource($process)) {
    fwrite($pipes[0], $html);
    fclose($pipes[0]);
    $text = '';
    while ($line = fgets($pipes[1])) {
      $text .= $line;
    }
    fclose($pipes[1]);
    proc_close($process);
    print "<pre>$text</pre>";
  }
}
