// tracking with jQuery
$(function() {

  // track search submit
  $('form').bind('submit', function(e) {
    var query = $(this).find('#search_box').val();
    addAction({
      event_name: 'search',
      query: query
    });
  });

	// add page loaded action
	$(window).bind('load', function(e) {
    addAction({
      event_name: 'load',
      results: $.makeArray($('[id^="result_"]').map(function(value) {
        return getResult(this);
      }))
    });
	});

	// when leaving the current page, send actions in buffer
	$(window).bind('unload', function(e) {
		addAction({
			event_name: 'unload',
		},
		{
			force: true,
			sync: true
		});
	});

	// track clicking result links
  $('[id^="result_"]').bind('click', function(e) {
    var result = getResult(this);
    result['event_name'] = 'click_result';
		addAction(result);
	});

  // track clicking previous page links
  $('a#previous').bind('click', function(e) {
    addAction({
      event_name: 'click_prev',
      page: parseInt(getParameterByKey('page')) - 1
    });
  });

  // track clicking next page links
  $('a#next').bind('click', function(e) {
    addAction({
      event_name: 'click_next',
      page: parseInt(getParameterByKey('page')) + 1
    });
  });

  // track clicking completed link
  $('#completed a').bind('click', function(e) {
    addAction({
      event_name: 'click_completed',
    });
  });

  (function() {
    var timer;
    // track region change
    $('.region').bind('mouseenter', function(e) {
      if (timer) {
        clearTimeout(timer);
      }

      var that = this;
      timer = setTimeout(function() {
        var event;
        region_name = $(that).attr('id')
        if (region_name.indexOf('result_') === 0) {
          event = getResult(that);
          event['event_name'] = 'region_result';
        } else {
          event = {
            event_name: 'region_non_result'
          };
        }
        event['region_name'] = region_name;
        addAction(event);
      }, 1000);
    });
  })();

  //// track key press
  //$(document.documentElement).bind('keydown', function(e) {
  //	addAction({
  //		event_name: $(e.target).attr('id') === 'search_box' ? 'search': 'keydown',
  //		altKey: e.altKey,
  //		ctrlKey: e.ctrlKey,
  //		shiftKey: e.shiftKey,
  //		which: e.which
  //	});
  //});

	// add action and send if buffer reaches certain size or if force is true
	var addAction = (function() {
		var actions = [];
		var freq = 1;
		var url = 'log.php';

		return function(action, options) {
			//action.event_num = $.cookies.get('event_num');
			//$.cookies.set('event_num', action.event_num + 1);
      var client_time = (new Date()).getTime();
      //action.elapsed_client_time = client_time - $.cookies.get('client_start_time');
      action.client_time = client_time;
      action.current_url = window.location.href;
      action.current_query = getParameterByKey('query');
      current_page = parseInt(getParameterByKey('page'));
      action.current_page = isNaN(current_page) ? -1 : current_page;
			actions.push(action);

			var force = options && options.force;
			var sync = options && options.sync;

      // TODO: remove true
			if (true || actions.length % freq === 0 || force) {
        var postData = actions;
        actions = [];

        console.log('sending data:', postData);

				$.ajax(url, {
					type: 'POST',
					async: !sync,
					data: {
						actions: postData
					},
					success: function(data) {
						console.log('received data:', data);
					}
				});
			}
		}
	})();

  // helper function for retrieving url query string value from key
  var getParameterByKey = function(key) {
    key = key.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
    var regexS = "[\\?&]" + key + "=([^&#]*)";
    var regex = new RegExp(regexS);
    var results = regex.exec(window.location.href);
    if(results == null) {
      return "";
    } else {
      return decodeURIComponent(results[1].replace(/\+/g, " "));
    }
  };

  var getResult = function(that) {
    result_prefix = 'result_';
    result = $(that).attr('id');
    rank = parseInt(result.substring(result_prefix.length, result.length));
    resultObj = {
      rank: rank,
      title: $(that).find('.title a').text(),
      snippet: $(that).find('.snippet').text(),
      link: $(that).find('.link').text()
    };
    docno = $(that).find('.docno');
    console.log('docno is', docno);
    if (docno.length) {
      resultObj['docno'] = docno.text()
    }
    document_id = $(that).find('.document_id');
    if (document_id.length) {
      resultObj['document_id'] = document_id.text()
    }
    return resultObj;
  };

	// logging via firebug/chrome dev; handle case not found
	(function() {
		console.log = console.log || function() {};
		console.warn = console.log || function() {};
		console.error = console.log || function() {};
		console.info = console.log || function() {};
	})();
});
