<?php

require_once('private/config.php');

$text = '';
if (!empty($_GET['docno'])) {
  $docno = urldecode($_GET['docno']);
  $url = "$indri_showdoc_endpoint?docno=$docno";
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, $url);
  curl_setopt($ch, CURLOPT_HEADER, 0);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  curl_setopt($ch, CURLOPT_PORT, $indri_endpoint_port);
  $text = curl_exec($ch);
} else if (!empty($_GET['url'])) {
  $url = urldecode($_GET['url']);
  $html = file_get_contents($url);
  $cmd = "$java_path -jar private/dist/Clueweb2trectext.jar";
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
  }
}
print "<pre>$text</pre>";
