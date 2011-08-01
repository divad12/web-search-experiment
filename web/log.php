<?php

function stringToAscii($str) {
  $ascii = '';
  for ($i = 0; $i < strlen($str); $i++) {
    $ascii .= ord($str[$i]).', ';
  }
  return $ascii;
}

session_start();

if (empty($_SESSION['user_id'])) {
  error_log("'user_id' could not be found in session");
  return;
} else {
  $user_id = $_SESSION['user_id'];
}

if (empty($_SESSION['topic_id'])) {
  error_log("'topic_id' could not be found in session");
  return;
} else {
  $topic_id = $_SESSION['topic_id'];
}

$actions = $_POST['actions'];
$source = $_SESSION['source'];
$path = "data/$user_id"."_$topic_id"."_$source.txt";
$message = '';
foreach ($actions as $action) {
  $action['server_time'] = time();
  $action['elapsed_client_time'] = $action['client_time'] - $_SESSION['client_start_time'];
  $milliseconds = (int) (microtime(true) * 1000);
  $action['elapsed_server_time'] = $milliseconds - $_SESSION['server_start_time'];
  $line = json_encode($action);
  $message .= $line."\n";
}

require_once('lib/sync_write.php');
sync_write($path, $message);

// XXX remove in production
print $line;

// TODO get php doctrine orm working
//require_once('orm/bootstrap.php');
//
//$conn = Doctrine_Manager::connection(DSN);
//$_COOKIE['user'] = 'mduan';
//if (empty($_COOKIE['user'])) {
//  return;
//} else {
//  $username = $_COOKIE['user'];
//  $q = Doctrine_Query::create()
//          ->select("u.*")
//          ->from("User u")
//          ->where("u.name = ?", $username);
//  print "print query: ".$q->getSqlQuery()."\n";
//  $user_collection = $q->execute();
//  if ($user_collection->count() == 0) {
//    $user = new User();
//    $user['name'] = $username;
//    $user->save();
//  } else {
//    $user = $user_collection->getFirst();
//  }
//}
//if (empty($_COOKIE['sid'])) {
//  $session = new Session();
//  $session->link('user', array($user['ID']));
//  $session->save();
//  $sid = $session['ID'];
//  setcookie('sid', $sid);
//} else {
//  $sid = $_COOKIE['sid'];
//}
//
//$path = "data/$sid.txt";
//$actions = $_POST['actions'];
//foreach ($actions as $action) {
//  file_put_contents($path, json_encode($action)."\n", FILE_APPEND);
//}
////print_r($actions);
//
//print_r($actions);
//
//foreach ($actions as $action) {
//  $event = new Event();
//  $event->name = $action['name'];
//  $event->seq = $action['seq'];
//  $event->time = $action['time'];
//  $event->save();
//  //$user = new User();
//  //$user->save();
//}
//$conn->close();
