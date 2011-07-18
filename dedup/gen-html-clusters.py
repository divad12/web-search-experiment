#!/usr/bin/python

# Read from stdin clusters of similar clueweb documents, and output a static html page of clusters with links

# TODO: Paginate so as not to load a huge static html page

import re;
import fileinput
import urllib

mansciBasename = 'http://mansci-mark-2.uwaterloo.ca:3624/smucker/websearchapi/showdoc.php?docno='
linesPerFile = 2000

def fileNameForPart(part):
  return 'clueweb-clusters-%d.html' % part

def pageNavLinks(part):
  return '<p><a href="%s">&lt; Prev</a> | Part %d | <a href="%s">Next &gt;</a></p>' % (fileNameForPart(part - 1), part, fileNameForPart(part + 1))

# TODO: This is a bad hack to get the url name by fetching from Msci server.
# should run this directly on msci server.
urlRegex = re.compile('&lt;url&gt;([^<]+)&lt;/url&gt;')

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

  outFile.write('<li>\n')
  for doc in line.split(','):
    doc = doc.strip()
    docUrl = mansciBasename + doc
    #contents = urllib.urlopen(docUrl).read()
    #url = urlRegex.search(contents).expand('\\1')
    outFile.write('<a href="%s">%s</a>, ' % (docUrl, doc))
  outFile.write('</li>\n')
  clustersPrinted += 1

if outFile:
  outFile.write('</ol>\n')
  outFile.close()
