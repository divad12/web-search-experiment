<?php

ini_set('auto_detect_line_endings', false);

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

//if (empty($_COOKIE['user'])) {
//  print "'user' cookie must be set to view this page\n";
//  return;
//}

$src = validParam('src', '');
if ($src !== 'yahoo' && $src !== 'indri') {
  print "'src' must be set to yahoo or indri to view this page\n";
  return;
}

class Result {
  public $title;
  public $snippet;
  public $link;
  public $rank;

  public function __construct($title, $snippet, $link, $rank) {
    $this->title = $title;
    $this->snippet = $snippet;
    $this->link = $link;
    $this->rank = $rank;
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
print "<input type='hidden' name='src' value='$src' />";
print "<input id='search_box' type='text' name='query' value='$query' />";
print "<input id='search_button' type='submit' value='Search' />";
print "</form>";
print "</div>";

if ($query) {
  require_once("lib/OAuth.php");

  $count = validIntParam('count', 10);
  $offset = validIntParam('offset', 0);

  $results = array();
  if ($src === 'yahoo') {
    $json = json_decode(file_get_contents('lib/yahoo-boss-keys.txt'));
    $cc_key = $json->key;
    $cc_secret = $json->secret;

    $url = 'http://yboss.yahooapis.com/ysearch/web';
    $args = array(
        'format' => 'xml',
        'count' => $count,
        'start' => $offset,
        'q' => $query
    );

    $consumer = new OAuthConsumer($cc_key, $cc_secret);
    $request = OAuthRequest::from_consumer_and_token($consumer, null, 'GET', $url, $args);
    $request->sign_request(new OAuthSignatureMethod_HMAC_SHA1(), $consumer, NULL);
    $url = sprintf('%s?%s', $url, OAuthUtil::build_http_query($args));
    $ch = curl_init();
    $headers = array($request->to_header());
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    $response = curl_exec($ch);

    $xml = new SimpleXMLElement($response);

    $attributes = $xml->web->attributes();
    $offset = $attributes['start'];
    $count = $attributes['count'];
    $total_result = $attributes['totalresults'];
    $last = $offset + $count - 1;

    $search_results = $xml->web->results->result;
    $rank = $offset;
    foreach ($search_results as $search_result) {
      $title = utf8_decode($search_result->title);
      $snippet = utf8_decode($search_result->abstract);
      $link = $search_result->dispurl;
      $result = new Result($title, $snippet, $link, $rank);
      $results[] = $result;
      $rank++;
    }

  } else if ($src === 'indri') {
    $url = 'http://mansci-mark-2.uwaterloo.ca/smucker/websearchapi/search.php';
    // TODO: use query parameter function
    $url = sprintf('%s?query=%s&startdoc=%d&numdisplay=%d', $url, urlencode($query), $offset, $count);

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_PORT, 3624);
    $response = curl_exec($ch);

    // TODO: proper detection of last page
    $last = $offset + $count - 1;

    $xml = new SimpleXMLElement($response);
    $search_results = $xml->results->result;
    foreach ($search_results as $search_result) {
      $title = utf8_decode($search_result->title);
      $snippet = utf8_decode($search_result->snippet);
      $link = $search_result->url;
      $rank = $search_result->rank;
      $result = new Result($title, $snippet, $link, $rank);
      $results[] = $result;
    }
  } else {
    print "Unexpected pathway\n";
    return;
  }

  print "<div class='region' id='results-info'>";
  print "<p>Results $offset to $last of about $total_result results</p>";
  print "</div>";

  foreach ($results as $result) {
    print "<div class='region' id='result-$result->rank'>";
    print "<span class='title'><a href='$result->clickurl'>$result->title</a></span>";
    print "<br/ >";
    if (trim($result->snippet)) {
      print "<span class='snippet'>$result->snippet</span>";
      print "<br />";
    } else {
      print "<span class='snippet'></span>";
    }
    print "<span class='link'>$result->link</span>";
    print "<br />";
    print "<br />";
    print "</div>";
  }

  print "<div class='region' id='prev-next'>";
  if ($offset > 0) {
    $previous_offset = $offset - $count;
    if ($previous_offset < 0) {
      $previous_offset = 0;
    }
    print "<a id='previous' href='?count=$count&offset=$previous_offset&src=$src&query=$query'>Previous</a>";
    print "<span id='prev-next-gap'></span>";
  }
  $next_offset = $offset + $count;
  print "<a id='next' href='?count=$count&offset=$next_offset&src=$src&query=$query'>Next</a>";
  print "</div>";
}

print "</div>";

print "</body>";
print "</html>";
