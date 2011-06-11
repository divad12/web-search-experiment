// tracking with jQuery
$(function() {

	// add page loaded action
	$(window).bind('load', function(e) {
		if ($.cookies.get('last-url') === window.location.href) {
			return;
		}

		addAction({
			type: 'load',
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
			type: 'unload',
		},
		{
			force: true,
			sync: true
		});
	});

	$('form').bind('submit', function(e) {
		var query = $(this).find('#search_box').val();
		addAction({
			type: 'submit',
			query: query
		});
	});

	$(document.documentElement).bind('keydown', function(e) {
		addAction({
			type: $(e.target).attr('id') === 'search_box' ? 'search': 'keydown',
			altKey: e.altKey,
			ctrlKey: e.ctrlKey,
			shiftKey: e.shiftKey,
			which: e.which
		});
	});

	// track region change
	$('.region').bind('mouseenter', function(e) {
		addAction({
			type: 'mouseenter',
			region: $(this).attr('id')
		});
	});

	// track clicking link
	$('[id^=result], #prev-next').find('a').bind('click', function(e) {
		addAction({
			type: 'click',
			url: $(this).attr('href')
		});
	});

  // track clicking link
  $('#completed a').bind('click', function(e) {
    addAction({
      type: 'completed',
      url: $(this).attr('href')
    });
  });

	// add action and send if buffer reaches certain size or if force is true
	var addAction = (function() {
		var actions = [];
		var freq = 10;
		var url = 'store.php';

		return function(action, options) {

			action.seq = $.cookies.get('seq');
			$.cookies.set('seq', action.seq + 1);
			action.timeStamp = (new Date()).getTime();
			actions.push(action);

			var force = options && options.force;
			var sync = options && options.sync;

			if (actions.length % freq === 0 || force) {

        var postData = actions;
        actions = [];

				$.ajax(url, {
					type: 'POST',
					async: !sync,
					data: {
						sid: $.cookies.get('sid'),
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

	// make sure cookies are set up
	(function() {
		if (!$.cookies.get('sid')) {
			$.cookies.set('sid', Math.floor(Math.random() * 1000000));
		}

		if (!$.cookies.get('seq')) {
			$.cookies.set('seq', 0);
		}
	})();
});

