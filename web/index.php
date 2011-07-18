<?php

print "<html>";
print "<head>";
//print "<script type='text/javascript' src='include/js/jquery-1.6.js'></script>";
//print "<script type='text/javascript' src='include/js/jquery.cookies.2.2.0.js'></script>";
print "<script type='text/javascript'>document.getElementById('client_start_time').value = (new Date()).getTime();</script>";
print "<title>Web Search</title>";
print "</head>";
print "<body>";

print "<form action='' method='post'>";
print "<input id='client_start_time' type='hidden' name='client_start_time' value='' />";
print "User Id: <input type='text' name='user_id' value='' /><br />";
print "Topic Id: <input type='text' name='topic_id' value='' /><br />";
print "Source: ";
$sources = array('indri' => 'Indri', 'yahoo' => 'Yahoo');
foreach ($sources as $key => $value) {
  print "$value <input type='radio' name='source' value='$key' />";
}
print "<br />";
print "<input id='submit' type='submit' value='Submit' />";
print "</form>";

print "</body>";
print "</html>";

error_log('$_POST contents: '.print_r($_POST, true));

$keys = array('user_id', 'topic_id', 'source');
foreach ($keys as $key) {
  if (empty($_POST[$key])) {
    error_log("'$key' could not be found in post (index.php)");
    return;
  }
}

if (!array_key_exists($source = $_POST['source'], $sources)) {
  error_log("'source' set to invalid value $source (index.php)");
  return;
}

$params = array();
foreach ($keys as $key) {
  $params[$key] = $_POST[$key];
  setcookie($key, $_POST[$key]);
}
$params['client_start_time'] = $_POST['client_start_time'];
$params['server_start_time'] = time();

$path = 'data/'.$params['user_id'].'-'.$params['topic_id'].'.txt';
// indicative of new session
if (!file_exists($path)) {
  require_once('lib/sync_write.php');
  $message = json_encode($params)."\n";
  sync_write($path, $message);
  // remove cookies to start new search session
  $past = time() - 3600;
  foreach ( $_COOKIE as $key => $value ) {
    setcookie($key, $value, $past, '/');
  }
  setcookie('client_start_time', $_POST['client_start_time']);
  setcookie('event_num', 0);
}

header('Location: search.php');
