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
    // to handle user clicking on link and then clicking back button;
    // prevent triggering load event in this case
		if ($.cookies.get('last_url') === window.location.href) {
			return;
		}

    result_prefix = 'result_';
		addAction({
			event_name: 'load',
			results: $.makeArray($('div[id^="result_prefix"]').map(function(value) {
        result = $(this).attr('id');
        rank = parseInt(result.substring(result_prefix.length, result.length));
				return {
          rank: rank,
					title: $(this).find('.title a').text(),
					snippet: $(this).find('.snippet').text(),
					link: $(this).find('.link').text()
				};
			}))
		});
	});

	// when leaving the current page, send actions in buffer
	$(window).bind('unload', function(e) {
		$.cookies.set('last_url', window.location.href);
		addAction({
			event_name: 'unload',
		},
		{
			force: true,
			sync: true
		});
	});

	// track clicking result links
  $('[id^="link_result_"]').bind('click', function(e) {
    result_prefix = 'link_result_';
    result = $(e.target).attr('id');
    rank = parseInt(result.substring(result_prefix.length, result.length - 1));
		addAction({
			event_name: 'click_result',
      rank: rank,
      title: $(this).find('.title a').text(),
      snippet: $(this).find('.snippet').text(),
      link: $(this).find('.link').text()
		});
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

  // track region change
  //$('.region').bind('mouseenter', function(e) {
  //  addAction({
  //    event_name: 'mouse_region',
  //    region_name: $(this).attr('id')
  //  });
  //});

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
			action.event_num = $.cookies.get('event_num');
			$.cookies.set('event_num', action.event_num + 1);
      action.elapsed_time = (new Date()).getTime() - $.cookies.get('start_time');
      action.current_url = window.location.href;
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

	// logging via firebug/chrome dev; handle case not found
	(function() {
		console.log = console.log || function() {};
		console.warn = console.log || function() {};
		console.error = console.log || function() {};
		console.info = console.log || function() {};
	})();
});
