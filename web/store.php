<?php

$sid = $_POST['sid'];
$path = "data/$sid.txt";
$actions = $_POST['actions'];
foreach ($actions as $action) {
  file_put_contents($path, serialize($action)."\n", FILE_APPEND);
}
print_r($actions);
