<?php

function sync_write($path, $message) {
  $file = fopen($path, 'a');
  flock($file, LOCK_EX);
  fwrite($file, $message);
  flock($file, LOCK_UN);
  fclose($file);
}

if (empty($_COOKIE['sid'])) {
  $sid = mt_rand();
  setcookie('sid', $sid);
} else {
  $sid = $_COOKIE['sid'];
}

$actions = $_POST['actions'];
file_put_contents("data/test.bak", "$actions\n", FILE_APPEND);
$message = '';
$path = "data/$sid.txt";
foreach ($actions as $action) {
  //file_put_contents("$path.bak", json_encode($action)."\n", FILE_APPEND);
  $message .= json_encode($action)."\n";
}
sync_write($path, $message);

// TODO: get php doctrine orm working
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
