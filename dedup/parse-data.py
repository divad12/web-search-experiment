#!/usr/bin/python

# Read from stdin JSON returned by elastic-mapreduce --describe, and output
# elapsed times for each step

import sys
import json
from datetime import datetime

def readableTimestamp(timestamp):
  return datetime.fromtimestamp(int(timestamp)).strftime('%Y-%m-%d %H:%M:%S')

data = json.loads(''.join(sys.stdin.readlines()))

for jobflow in data['JobFlows']:
  print 'JobFlow', jobflow['JobFlowId'], 'named', jobflow['Name']
  print '- Ready at', readableTimestamp(jobflow['ExecutionStatusDetail']['ReadyDateTime'])
  instances = jobflow['Instances']
  print '-', instances['InstanceCount'], 'instances,', instances['MasterInstanceType'], 'master instance,', instances['SlaveInstanceType'], 'slave instances'
  print '- Steps:'

  for step in jobflow['Steps']:
    detail = step['ExecutionStatusDetail']
    if not (detail['EndDateTime'] and detail['StartDateTime'] and detail['State'] == 'COMPLETED'):
      continue

    elapsed = str(int(detail['EndDateTime']) - int(detail['StartDateTime']))
    inputLocation = step['StepConfig']['HadoopJarStep']['Args'][1]
    # HACK HACK HACK hardcoded
    if inputLocation.find('output') != -1:
      inputLocation = ''
    else:
      print

    print '  -', inputLocation, 'on step', step['StepConfig']['Name']
    print '     -', elapsed, 'seconds: started', readableTimestamp(detail['StartDateTime']), ', ended', readableTimestamp(detail['EndDateTime'])

  print
