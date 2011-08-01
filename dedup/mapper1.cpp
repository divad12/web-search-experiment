// Mapper for Amazon Elastic Map Reduce using Hadoop streaming

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <tr1/unordered_set>

// TODO: Test this rabin-hash implementation
#include "rabin-hash-64.h"

////////////////////////////////////////////////////////////////////////////////
const char* DOCNO_START_TAG = "<DOCNO>";
const char* DOCNO_END_TAG = "</DOCNO>";
const size_t DOCNO_START_LEN = strlen(DOCNO_START_TAG);

const char* DOC_START_TAG = "<DOC>";
const char* DOC_END_TAG = "</DOC>";

// TODO: Annoyance: Aquaint uses <BODY>, clueweb uses <body>
const char* BODY_START_TAG = "<body>";
const char* BODY_END_TAG = "</body>";
const size_t BODY_START_LEN = strlen(BODY_START_TAG);

const char* TITLE_START_TAG = "<title>";
const char* TITLE_END_TAG = "</title>";
const size_t TITLE_START_LEN = strlen(TITLE_START_TAG);

const char* DOC_ID_SEP = "|";

// TODO: Should not declare static variables of class type.
RabinHashFunction64 rabinHash;

const size_t WORDS_PER_SHINGLE = 10;
// TODO: Make sketch_size a function of the size of the document?
const size_t SKETCH_SIZE = 100;

////////////////////////////////////////////////////////////////////////////////
void trim(std::string& str) {
  str.erase(str.find_last_not_of(' ') + 1);
  str.erase(0, str.find_first_not_of(' '));
}

std::string numToStr(long long num) {
  char str[20];
  sprintf(str, "%lld", num);
  return std::string(str);
}

std::string padStr(const std::string& str, size_t size, char paddingChar) {
  std::stringstream ss;
  ss << std::setfill(paddingChar) << std::setw(size) << str;
  return ss.str();
}

// THis is a character from the input that we want to keep
bool isGoodChar(char c) {
  return isalnum(c) || isspace(c);
}

// Normalize the input string: lowercase all letters, only keep alphanumerics,
// remove all html tags, collapse whitespace into ' '
void processInput(std::string& str) {
  std::string::iterator newEnd = str.begin();
  bool inTag = false;
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    if (inTag) {
      if (*it == '>') {
        inTag = false;
      }
    } else if (isGoodChar(*it)) {
      *(newEnd++) = tolower(*it);
    } else if (*it == '<') {  // ASSUMPTION: '<' is not a good char
      inTag = true;
    }
  }
  str.erase(newEnd, str.end());

  // Make a second pass to collapse all contiguous whitespace into ' '
  // TODO: Optimization: Make this into a single pass, or split this out into a
  //     different func
  bool isPrevCharWhitespace = false;
  newEnd = str.begin();
  for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
    if (!(isPrevCharWhitespace && isspace(*it))) {
      if (isspace(*it)) {
        *it = ' ';
        isPrevCharWhitespace = true;
      } else {
        isPrevCharWhitespace = false;
      }
      *(newEnd++) = *it;
    }
  }
  str.erase(newEnd, str.end());
}

// From a document, get only the parts that we want to shingle
std::string getDedupInput(const std::string& doc) {
  // ASSUMPTION: Title and body nodes are not nested within each other
  // ASSUMPTION: TItle node may or may not exist; body node MUST exist
  std::string ret;

  size_t titleStart = doc.find(TITLE_START_TAG);
  if (titleStart != std::string::npos) {
    titleStart += TITLE_START_LEN;
    size_t titleEnd = doc.find(TITLE_END_TAG, titleStart);
    ret = doc.substr(titleStart, titleEnd - titleStart);
  }

  size_t bodyStart = doc.find(BODY_START_TAG) + BODY_START_LEN;
  size_t bodyEnd = doc.find(BODY_END_TAG, bodyStart);
  return ret + "\n" + doc.substr(bodyStart, bodyEnd - bodyStart);
}

// Get the document id string from the doc contents
std::string getDocId(const std::string& doc) {
  size_t docnoStart = doc.find(DOCNO_START_TAG) + DOCNO_START_LEN;
  size_t docnoEnd = doc.find(DOCNO_END_TAG, docnoStart);
  std::string docId = doc.substr(docnoStart, docnoEnd - docnoStart);
  trim(docId);
  return docId;
}

// NOTE: This destroys the original value in string doc
void emitKeyValuePairs(const std::string& doc) {
  // TODO: need to worry about unicode?
  std::string toShingleStr = getDedupInput(doc);
  processInput(toShingleStr);

  typedef std::set<long long> ShingleSet;
  ShingleSet shingleSet;

  // Get the initial shingles
  size_t toShingleStrLen = toShingleStr.length();
  size_t begin = toShingleStr.find_first_not_of(' ', 0);
  size_t end = begin;
  for (size_t i = 0; i < WORDS_PER_SHINGLE; ++i) {
    end = toShingleStr.find_first_of(' ', end + 1);
    if (end == std::string::npos) {
      end = toShingleStrLen;
      break;
    }
  }

  // Now shift that window of words and hash each window
  while (true) {
    long long shingle = rabinHash.hash(toShingleStr.c_str() + begin, end - begin);
    shingleSet.insert(shingle);

    if (end == toShingleStrLen) break;

    begin = toShingleStr.find_first_of(' ', begin + 1) + 1;
    end = toShingleStr.find_first_of(' ', end + 1);
    if (end == std::string::npos) {
      end = toShingleStrLen;
    }
  }

  // Get the document ID string concatenated with the # of unique shingles
  std::string docId = getDocId(doc);
  docId += DOC_ID_SEP;
  docId += numToStr(std::min(shingleSet.size(), SKETCH_SIZE));
  const char* docIdCstr = docId.c_str();

  // Now emit the <shingle, docId + size> pairs.
  size_t emitted = 0;
  for (ShingleSet::iterator it = shingleSet.begin();
      it != shingleSet.end() && emitted < SKETCH_SIZE; ++it, ++emitted) {
    printf("%lld\t%s\n", *it, docIdCstr);
  }
}

////////////////////////////////////////////////////////////////////////////////
int main() {
  static const int MAX_LINE_LEN = 100000;
  char line[MAX_LINE_LEN];

  std::string doc;
  bool inDoc = false;

  while (fgets(line, MAX_LINE_LEN, stdin)) {
    // Assumptions: no nested docs; doc tags are uppercase
    if (inDoc) {
      doc += line;
      if (strstr(line, DOC_END_TAG)) {
        inDoc = false;
        emitKeyValuePairs(doc);
      }
    } else if (strstr(line, DOC_START_TAG)) {
      inDoc = true;
      doc = line;
    }
  }

  return 0;
}
