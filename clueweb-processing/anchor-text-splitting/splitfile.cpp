#include <unistd.h>
#include <ios>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <tr1/unordered_map>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;
using namespace tr1;

////////////////////////////////////////////////////////////////////////////////

const int MAX_LINE_SIZE = 12345678;
const double FREE_MEM_THRESHOLD_KB = 2000;
const int CHECK_MEMORY_PERIOD = 1000;

// The checking memory code doesn't seem to work very welll... here's just a
// rough estimate based on observations of top that should keep us from thrashing
const int FLUSH_LINES_THRESHOLD = 4000000;

typedef unordered_map<string, string> FileToLines;
typedef unordered_map<string, FileToLines> DirToFiles;



// From http://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-run-time-in-c
//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   static string pid, comm, state, ppid, pgrp, session, tty_nr;
   static string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   static string utime, stime, cutime, cstime, priority, nice;
   static string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}



////////////////////////////////////////////////////////////////////////////////

// Flushes string to end of a file, and CLEARS the string. DOES NOT CHECK if
// file or directory exists.
void flushFile(const char* fileName, string& contents) {
  FILE* outFile = fopen(fileName, "a");  // Open file to append to
  fprintf(outFile, "%s", contents.c_str());
  fclose(outFile);
  contents.resize(0);
}

// Flush all contents of the directory hash map to files
void flushDir(DirToFiles& dir) {
  string dirName, fileName;
  for (DirToFiles::iterator i = dir.begin(); i != dir.end(); ++i) {
    dirName = i->first;
    mkdir(dirName.c_str(), 0777);  // Create the directory
    FileToLines& file = i->second;
    for (FileToLines::iterator j = file.begin(); j != file.end(); ++j) {
      fileName = dirName + "/" + j->first;
      flushFile(fileName.c_str(), j->second);
    }
  }
}

void replaceTabsWithSpaces(char* str) {
  for (;;) {
    char* pos = strchr(str, '\t');
    if (pos == NULL) return;
    *pos = ' ';
    str = pos + 1;
  }
}

bool shouldFlush() {
  static double vmUsage, residentSet;
  process_mem_usage(vmUsage, residentSet);
  assert(residentSet != 0 || vmUsage != 0);
  return (vmUsage - residentSet < FREE_MEM_THRESHOLD_KB);
}

////////////////////////////////////////////////////////////////////////////////

int main() {
  char docno[500];
  char* anchorText = (char*)malloc(MAX_LINE_SIZE);
  char* line = (char*)malloc(MAX_LINE_SIZE);
  char dirName[100], fileName[100];

  DirToFiles directory;
  string rejects;

  long long int linesProcessed = 0;
  while (scanf("%12345678[^\n]\n", line) != EOF) {
    int numMatched = sscanf(line, "%s\t%*s\t%12345678[^\n]\n", docno, anchorText);

    if (numMatched < 2) {
      rejects.append(docno).append("\n");
    } else {
      sscanf(docno, "%*[^-]-%[^-]-%[^-]", dirName, fileName);
      replaceTabsWithSpaces(anchorText);
      directory[dirName][fileName].append(docno)
                                  .append("\t")
                                  .append(anchorText)
                                  .append("\n");
    }

    // TODO: Figure out why checking memory does not work as expected.
    //if (counter % CHECK_MEMORY_PERIOD == 0 && shouldFlush()) {
    if (linesProcessed % FLUSH_LINES_THRESHOLD == 0) {
      cout << "Flushing at " << linesProcessed << endl;
      flushDir(directory);
      flushFile("rejects.txt", rejects);
      rejects.clear();
      directory.clear();
      cout << "Flushed to file!" << endl;
    }
    linesProcessed++;
  }

  flushDir(directory);
  flushFile("rejects.txt", rejects);

  return 0;
}
