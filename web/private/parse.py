#!/usr/bin/python

import json

# TODO dynamic file path
f = open('../data/user1-topic1.txt')

# session information
line = f.readline()
session = json.loads(line)
print 'Session information:'
print 'User Id: %s' % session['user_id']
print 'Topic Id: %s' % session['topic_id']
print 'Source: %s' % session['source']
print 'Client Start Time: %d' % int(session['client_start_time'])
print 'Server Start Time: %d' % int(session['server_start_time'])
print ''

line = f.readline()
event_num = 0
while line:
  event = json.loads(line)

  event_name = event['event_name']
  client_time = int(event['client_time'])
  server_time = int(event['server_time'])
  elapsed_client_time = int(event['elapsed_client_time'])
  elapsed_server_time = int(event['elapsed_server_time'])
  current_url = event['current_url']
  current_query = event['current_query']
  current_page = int(event['current_page'])

  event_num += 1
  print 'Event %d: Name=%s' % (event_num, event_name)
  print 'Times: Client Time=%d, Server Time=%d, Elapsed Client Time=%d, Elapsed Server Time=%d' % (client_time, server_time, elapsed_client_time, elapsed_server_time)
  print 'Context: Url=%s, Query=%s, Page=%d' % (current_url, current_query, current_page)

  # 1
  if event_name == 'search':
    print 'Details: Searching for %s' % event['query']
  # 2
  elif event_name == 'load':
    if 'results' in event:
      print 'Details: Loading results:'
      for result in event['results']:
        print '  %d: %s [%s]' % (int(result['rank']), result['title'], result['link'])
    else:
      print 'Details: Loading initial search page'
  # 3
  elif event_name == 'unload':
    print 'Details: Unloading page'
  # 4
  elif event_name == 'click_result':
    print 'Details: Clicking result:'
    print ' Rank: %d' % (event['rank'])
    print ' Title: %s' % event['title']
    print ' Snippet: %s' % event['snippet']
    print ' Link: %s' % event['link']
  # 5
  elif event_name == 'click_prev':
    print 'Details: Clicking previous page to page %d' % int(event['page'])
  # 6
  elif event_name == 'click_next':
    print 'Details: Clicking next page to page %d' % int(event['page'])
  # 7
  elif event_name == 'click_completed':
    print 'Details: Clicking completed'
  # 8
  elif event_name == 'region_non_result':
    print 'Details: In non result region %s' % event['region_name']
  # 9
  elif event_name == 'region_result':
    print 'Details: In result region %s with result:' % event['region_name']
    print ' Rank: %d' % int(event['rank'])
    print ' Title: %s' % event['title']
    print ' Snippet: %s' % event['snippet']
    print ' Link: %s' % event['link']

  print ''

  line = f.readline()


