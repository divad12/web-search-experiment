<?php

function validParam($key, $default) {
  if (!empty($_GET[$key])) {
    return $_GET[$key];
  } else {
    return $default;
  }
}

function validIntParam($key, $default) {
  if (!empty($_GET[$key]) && is_numeric($_GET[$key]) && $_GET[$key] >= 0) {
    return intval($_GET[$key]);
  } else {
    return $default;
  }
}

print "<html>";
print "<head>";
print "<script type='text/javascript' src='include/js/jquery-1.6.js'></script>";
print "<script type='text/javascript' src='include/js/jquery.cookies.2.2.0.js'></script>";
print "<script type='text/javascript' src='include/js/tracking.js'></script>";
print "<link rel='stylesheet' type='text/css' href='include/css/style.css' />";
print "<title>Yahoo Search</title>";
print "</head>";
print "<body>";

$query = validParam('query', '');

print "<div class='doc-body'>";

print "<div class='region' id='completed'>";
print "<a href='completed.php'>Completed</a>";
print "<br />";
print "<br />";
print "</div>";

print "<div class='region' id='search'>";
print "<form action='' method='get'>";
print "<input type='hidden' name='count' value='10' />";
print "<input type='hidden' name='offset' value='0' />";
print "<input id='search_box' type='text' name='query' value='$query' />";
print "<input id='search_button' type='submit' value='Search' />";
print "</form>";
print "</div>";

if ($query) {
  require("lib/OAuth.php");

  $count = validIntParam('count', 10);
  $offset = validIntParam('offset', 0);

  $data = explode("\n", file_get_contents('lib/yahoo-boss-keys.txt'));
  $cc_key  = $data[0];
  $cc_secret = $data[1];

  $url = 'http://yboss.yahooapis.com/ysearch/web';
  //$url = "http://yboss.yahooapis.com/ysearch/news,web,images,limitedweb";
  $args = array(
      'format' => 'xml',
      'count' => $count,
      'start' => $offset,
      'q' => $query
  );

  $consumer = new OAuthConsumer($cc_key, $cc_secret);
  $request = OAuthRequest::from_consumer_and_token($consumer, null, 'GET', $url, $args);
  $request->sign_request(new OAuthSignatureMethod_HMAC_SHA1(), $consumer, NULL);
  $url = sprintf("%s?%s", $url, OAuthUtil::build_http_query($args));
  $ch = curl_init();
  $headers = array($request->to_header());
  curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
  curl_setopt($ch, CURLOPT_HEADER, 0);
  curl_setopt($ch, CURLOPT_URL, $url);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
  $response = curl_exec($ch);

  $xml = new SimpleXMLElement($response);

  //print_r($xml);

  $attributes = $xml->web->attributes();
  $offset = $attributes['start'];
  $count = $attributes['count'];
  $total_result = $attributes['totalresults'];
  $last = $offset + $count - 1;
  print "<div class='region' id='results-info'>";
  print "<p>Results $offset to $last of about $total_result results</p>";
  print "</div>";

  $results = $xml->web->results->result;
  if (!empty($results)) {
    $idx = $offset;
    foreach ($results as $result) {
      //print_r($result);
      print "<div class='region' id='result-$idx'>";
      $link_title = utf8_decode($result->title);
      print "<span class='title'><a href='$result->clickurl'>$link_title</a></span>";
      print "<br/ >";
      if (trim($result->abstract)) {
        $abstract = utf8_decode($result->abstract);
        print "<span class='snippet'>$abstract</span>";
        print "<br />";
      } else {
        print "<span class='snippet'></span>";
      }
      print "<span class='link'>$result->dispurl</span>";
      print "<br />";
      print "<br />";
      print "</div>";
      $idx++;
    }
  }

  print "<div class='region' id='prev-next'>";
  if ($offset > 0) {
    $previous_offset = $offset - $count;
    if ($previous_offset < 0) {
      $previous_offset = 0;
    }
    print "<a id='previous' href='?count=$count&offset=$previous_offset&query=$query'>Previous</a>";
    print "<span id='prev-next-gap'></span>";
  }
  $next_offset = $offset + $count;
  print "<a id='next' href='?count=$count&offset=$next_offset&query=$query'>Next</a>";
  print "</div>";
}

print "</div>";

print "</body>";
print "</html>";
