<?php

require_once('private/config.php');

session_start();

// expire hourly
$expires = 60 * 60;
header("Pragma: public");
header("Cache-Control: maxage=$expires");
header("Expires: ".gmdate('D, d M Y H:i:s', time() + $expires)." GMT");

$keys = array('topic_id', 'user_id', 'source', 'use_cached');
$params = array();
$is_setup = true;
foreach ($keys as $key) {
  if (!isset($_SESSION[$key])) {
    error_log("'$key' could not be found in cookie (search.php)");
    $is_setup = false;
  }
}
$use_cached = $_SESSION['use_cached'];

if ($is_setup) {
  $source = $_SESSION['source'];
  if ($source !== 'yahoo' && $source !== 'indri') {
    error_log("'source' set to invalid value $source (search.php)");
    $is_setup = false;
  }
}

if ($is_setup) {

  function validParam($key, $default) {
    if (isset($_GET[$key])) {
      return $_GET[$key];
    } else {
      return $default;
    }
  }

  function validIntParam($key, $default) {
    if (isset($_GET[$key]) && is_numeric($_GET[$key]) && $_GET[$key] >= 0) {
      return intval($_GET[$key]);
    } else {
      return $default;
    }
  }

  class Result {
    public $title;
    public $snippet;
    public $link;
    public $disp_link;
    public $rank;
    public $docno;
    public $documentId;

    public function __construct($title, $snippet, $link, $disp_link, $rank,
                                $docno=null, $document_id=null) {
      $this->title = $title;
      $this->snippet = $snippet;
      $this->link = $link;
      $this->disp_link = $disp_link;
      $this->rank = $rank;
      $this->docno = $docno;
      $this->documentId = $document_id;
    }
  }

  print "<html>";
  print "<head>";
  print "<script type='text/javascript' src='include/js/jquery-1.6.js'></script>";
  print "<script type='text/javascript' src='include/js/jquery.cookies.2.2.0.js'></script>";
  print "<script type='text/javascript' src='include/js/log.js'></script>";
  print "<link rel='stylesheet' type='text/css' href='include/css/style.css' />";
  print "<title>Web Search</title>";
  print "</head>";
  print "<body>";

  $query = trim(validParam('query', ''));
  $query_encoded = urlencode($query);
  $query_html = htmlentities($query, ENT_QUOTES);

  print "<div class='doc_body'>";

  print "<div class='region' id='completed'>";
  print "<a href='completed.php'>Completed</a>";
  print "<br />";
  print "<br />";
  print "</div>";

  print "<div class='region' id='search'>";
  print "<form action='' method='get'>";
  print "<input type='hidden' name='page' value='0' />";
  print "<input id='search_box' type='text' name='query' value='$query_html' />";
  print "<input id='search_button' type='submit' value='Search' />";
  print "</form>";
  print "</div>";

  if ($query) {

    $count = 10;
    $page = validIntParam('page', 0);
    $offset = $page * $count;

    $results = array();
    if ($source === 'yahoo') {
      require_once('lib/OAuth.php');

      $args = array(
          'format' => 'xml',
          'count' => $count,
          'start' => $offset,
          'q' => $query_encoded
      );

      $consumer = new OAuthConsumer($yahoo_key, $yahoo_secret);
      $request = OAuthRequest::from_consumer_and_token($consumer, null, 'GET', $yahoo_search_endpoint, $args);
      $request->sign_request(new OAuthSignatureMethod_HMAC_SHA1(), $consumer, NULL);
      $url = sprintf('%s?%s', $yahoo_search_endpoint, OAuthUtil::build_http_query($args));
      $ch = curl_init();
      $headers = array($request->to_header());
      curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
      curl_setopt($ch, CURLOPT_HEADER, 0);
      curl_setopt($ch, CURLOPT_URL, $url);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
      $response = curl_exec($ch);

      $xml = new SimpleXMLElement($response);

      $attributes = $xml->web->attributes();
      //$offset = $attributes['start'];
      //$count = $attributes['count'];

      $search_results = $xml->web->results->result;
      $rank = $offset;
      foreach ($search_results as $search_result) {
        $title = utf8_decode($search_result->title);
        $snippet = utf8_decode($search_result->abstract);
        $link = $search_result->url;
        $disp_link = $search_result->dispurl;
        $result = new Result($title, $snippet, $link, $disp_link, $rank);
        $results[] = $result;
        $rank++;
      }

    } else if ($source === 'indri') {
      // TODO: use query parameter function
      $url = sprintf('%s?query=%s&startdoc=%d&numdisplay=%d', $indri_search_endpoint, $query_encoded, $offset, $count);
      $ch = curl_init();
      curl_setopt($ch, CURLOPT_URL, $url);
      curl_setopt($ch, CURLOPT_HEADER, 0);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
      curl_setopt($ch, CURLOPT_PORT, $indri_endpoint_port);
      $response = curl_exec($ch);

      // TODO: proper detection of last page

      $xml = new SimpleXMLElement($response);
      $search_results = $xml->results->result;
      foreach ($search_results as $search_result) {
        $title = utf8_decode($search_result->title);
        $snippet = utf8_decode($search_result->snippet);
        $link = $search_result->url;
        $rank = $search_result->rank;
        $docno = $search_result->docno;
        $document_id = $search_result->{'document-id'};
        //error_log(print_r($document_id, true));
        $result = new Result($title, $snippet, $link, $link, $rank, $docno, $document_id);
        $results[] = $result;
      }
    } else {
      print "Unexpected pathway\n";
      return;
    }

    $last_result = $offset + $count - 1;
    print "<div class='region' id='results_info'>";
    print "<p>Showing results $offset to $last_result</p>";
    print "</div>";

    foreach ($results as $result) {
      $cached_url = "cached.php?url=".urlencode($result->link)."&docno=";
      if (!empty($result->docno)) {
        $cached_url .= urlencode($result->docno);
      }
      if ($use_cached) {
        $link = $cached_url;
      } else {
        $link = $result->link;
      }
      print "<div class='region' id='result_$result->rank'>";
      print "<span class='title'><a href='$link'>$result->title</a></span>";
      print "<br/ >";
      if (trim($result->snippet)) {
        print "<span class='snippet'>$result->snippet</span>";
        print "<br />";
      } else {
        print "<span class='snippet'></span>";
      }
      print "<span class='link'>$result->disp_link</span>";
      if (!$use_cached) {
        print " - <span><a class='cached' href='$cached_url'>Cached</a></span>";
      }
      if ($result->docno) {
        print "<span class='docno' style='display:none'>$result->docno</span>";
      }
      if ($result->documentId) {
        print "<span class='document_id' style='display:none'>$result->documentId</span>";
      }
      print "<br />";
      print "<br />";
      print "</div>";
    }

    print "<div class='region' id='prev_next'>";
    if ($page > 0) {
      print "<a id='previous' href='?page=".($page - 1)."&query=$query_html'>Previous</a>";
      print "<span id='prev_next_gap'></span>";
    }
    print "<a id='next' href='?page=".($page + 1)."&query=$query_html'>Next</a>";
    print "</div>";
  }

  print "</div>";

  print "</body>";
  print "</html>";
} else {
  header('Location: index.php');
}
