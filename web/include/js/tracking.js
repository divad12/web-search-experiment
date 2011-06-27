// tracking with jQuery
$(function() {

	// add page loaded action
	$(window).bind('load', function(e) {
		if ($.cookies.get('last-url') === window.location.href) {
			return;
		}

		addAction({
			name: 'load',
			url: window.location.href,
			results: $.makeArray($('[id^=result-]').map(function() {
				return {
					title: $(this).find('.title a').text(),
					snippet: $(this).find('.snippet').text(),
					link: $(this).find('.link').text()
				};
			}))
		});
	});

	// when leaving the current page, send actions in buffer
	$(window).bind('unload', function(e) {
		$.cookies.set('last-url', window.location.href);
		addAction({
			name: 'unload',
		},
		{
			force: true,
			sync: true
		});
	});

  // track search submit
	$('form').bind('submit', function(e) {
		var query = $(this).find('#search_box').val();
		addAction({
			name: 'submit',
			query: query
		});
	});

  // track key press
	//$(document.documentElement).bind('keydown', function(e) {
	//	addAction({
	//		name: $(e.target).attr('id') === 'search_box' ? 'search': 'keydown',
	//		altKey: e.altKey,
	//		ctrlKey: e.ctrlKey,
	//		shiftKey: e.shiftKey,
	//		which: e.which
	//	});
	//});

	// track region change
	//$('.region').bind('mouseenter', function(e) {
	//	addAction({
	//		name: 'mouseenter',
	//		region: $(this).attr('id')
	//	});
	//});

	// track clicking result links
  $('[id^=result]').find('a').bind('click', function(e) {
		addAction({
			name: 'click',
			url: $(this).attr('href')
		});
	});

  // track clicking changing page links
  $('#prev-next').find('a').bind('click', function(e) {
    addAction({
      name: 'click',
      url: $(this).attr('href')
    });
  });

  // track clicking completed link
  $('#completed a').bind('click', function(e) {
    addAction({
      name: 'completed',
      url: $(this).attr('href')
    });
  });

	// add action and send if buffer reaches certain size or if force is true
	var addAction = (function() {
		var actions = [];
		var freq = 1;
		var url = 'store.php';

		return function(action, options) {
      // make sure cookies are set up
      //if (!$.cookies.get('sid')) {
      //  $.cookies.set('sid', Math.floor(Math.random() * 1000000));
      //}
      if (!$.cookies.get('seq')) {
        $.cookies.set('seq', 0);
      }
      if (!$.cookies.get('startTime')) {
        $.cookies.set('startTime', (new Date()).getTime());
      }

			action.seq = $.cookies.get('seq');
			$.cookies.set('seq', action.seq + 1);
      action.timeStamp = (new Date()).getTime() - $.cookies.get('startTime');
			actions.push(action);

			var force = options && options.force;
			var sync = options && options.sync;

      // TODO: remove short circuit of batch sending
			if (true || /*actions.length % freq === 0 ||*/ force) {

        var postData = actions;
        actions = [];

        console.log('sending data:', postData);

				$.ajax(url, {
					type: 'POST',
					async: !sync,
					data: {
						//sid: $.cookies.get('sid'),
						actions: postData
					},
					success: function(data) {
						console.log('received data:', data);
					}
				});
			}
		}
	})();

	// logging via firebug/chrome dev; handle case not found
	(function() {
		console.log = console.log || function() {};
		console.warn = console.log || function() {};
		console.error = console.log || function() {};
		console.info = console.log || function() {};
	})();

  addAction({
    name: 'return'
  });
});
