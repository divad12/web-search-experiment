#!/usr/bin/python

# Read from stdin clusters of similar clueweb documents, and output static html
# pages in the current directory of clusters with links

import re
import fileinput
import urllib

mansciBasename = 'http://mansci-mark-2.uwaterloo.ca:3624/smucker/websearchapi/showdoc.php?docno='
linesPerFile = 2000

def fileNameForPart(part):
  return 'clueweb-clusters-%d.html' % part

def pageNavLinks(part):
  return '<p><a href="%s">&lt; Prev</a> | Part %d | <a href="%s">Next &gt;</a></p>' % (fileNameForPart(part - 1), part, fileNameForPart(part + 1))

clustersPrinted = 0
part = 0
outFile = None
for line in fileinput.input():
  if clustersPrinted % linesPerFile == 0:
    if outFile:
      outFile.write('</ol>\n')
      outFile.write(pageNavLinks(part))
      outFile.close()
    outFile = open(fileNameForPart(part), 'w')
    outFile.write(pageNavLinks(part))
    outFile.write('<ol start="%d">\n' % clustersPrinted)
    part += 1

  # TODO: use map and join
  outFile.write('<li>\n')
  lineSplit1 = line.split('\t')
  similarity = float(lineSplit1[0])
  outFile.write("%.4f - " % similarity)
  for doc in lineSplit1[1].split(','):
    doc = doc.strip()
    docUrl = mansciBasename + doc
    outFile.write('<a href="%s">%s</a>, ' % (docUrl, doc))
  outFile.write('</li>\n')
  clustersPrinted += 1

if outFile:
  outFile.write('</ol>\n')
  outFile.close()
