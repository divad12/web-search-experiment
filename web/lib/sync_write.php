<?php

function sync_write($path, $message) {
  $file = fopen($path, 'a');
  flock($file, LOCK_EX);
  fwrite($file, $message);
  flock($file, LOCK_UN);
  fclose($file);
}

