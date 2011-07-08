<?php
  /** Indri settings
   *
   */
   
  //
  // index: Complete path to the Indri index to be queried.
  //        Make sure that the web server has permission to read
  //        this index, and that there are no processes writing to
  //        this index.
  //

  //$indri_param['index'] = "/work/data/indexes/indri-5.0/aquaint-krovetz";
  $indri_param['index'] = "/work/data/websearch-tmp/cw";

  //
  // document_format: Use 'text' if the document is a text file (or 
  //        an XML file, but you want to show the tags in the browser).
  //        Use 'html' to attempt to render the document in the browser.
  //

  $indri_param['document_format'] = "html";

  //
  // library: Location and name of the Indri PHP shared library,
  //          probably called 'libindri.so'.  Check your PHP
  //          installation to see where this library needs to be stored.
  //

  $indri_param['library'] = "libindri_php.so";

  //
  // snippet_length: Maximum length, in words, of the snippet that
  //          appears below each search result.
  //

  $indri_param['snippet_length'] = 50;

  //
  // memory_limit: Maximum number of bytes that the PHP process should
  //          use while processing query requests.
  //

  $indri_param['memory_limit'] = 1024*1024*1024;

  //
  // page_docs: Number of documents to display on each query result page.
  //

  $indri_param['page_docs'] = 10;
?>