// The first mapper: transforms docs in clueweb trectext XML format to
// <shingle_hash_value, docId> shingles. For use in Hadoop streaming.
// The first argument must be the name of the tag of the content to shingle.

#include <cassert>
#include <cctype>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <tr1/unordered_set>

#include "rabin-hash-64.h"
#include "constants.h"
#include "utils.h"

// TODO: encapsulate all dedup code in namespace
using namespace dedup;

////////////////////////////////////////////////////////////////////////////////
// The node containing the text to be shingled
static std::string contentStartTag;
static std::string contentEndTag;
static int contentStartLen;

////////////////////////////////////////////////////////////////////////////////
// This is a character from the input that we want to keep
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
  // ASSUMPTION: Title and content nodes are not nested within each other
  // ASSUMPTION: TItle node may or may not exist; content node MUST exist
  std::string ret;

  size_t titleStart = doc.find(TITLE_START_TAG);
  if (titleStart != std::string::npos) {
    titleStart += TITLE_START_LEN;
    size_t titleEnd = doc.find(TITLE_END_TAG, titleStart);
    ret = doc.substr(titleStart, titleEnd - titleStart);
  }

  size_t contentStart = doc.find(contentStartTag) + contentStartLen;
  size_t contentEnd = doc.find(contentEndTag, contentStart);
  return ret + "\n" + doc.substr(contentStart, contentEnd - contentStart);
}

// Get the document id string from the doc contents
std::string getDocId(const std::string& doc) {
  size_t docnoStart = doc.find(DOCNO_START_TAG) + DOCNO_START_LEN;
  size_t docnoEnd = doc.find(DOCNO_END_TAG, docnoStart);
  std::string docId = doc.substr(docnoStart, docnoEnd - docnoStart);
  trim(docId);
  return docId;
}

// Emit the shingles for this doc
void emitKeyValuePairs(const std::string& doc) {
  std::string toShingleStr = getDedupInput(doc);
  processInput(toShingleStr);

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
  static RabinHashFunction64 rabinHash;
  typedef unsigned long long ShingleHash;
  typedef std::set<ShingleHash> ShingleSet;
  ShingleSet shingleSet;

  while (true) {
    ShingleHash shingle = rabinHash.hash(toShingleStr.c_str() + begin, end - begin);
    shingleSet.insert(shingle);

    if (end == toShingleStrLen) break;

    begin = toShingleStr.find_first_of(' ', begin + 1) + 1;
    end = toShingleStr.find_first_of(' ', end + 1);
    if (end == std::string::npos) {
      end = toShingleStrLen;
    }
  }

  // Get the document ID string concatenated with the # of shingles we are
  // outputting.
  std::string docId = getDocId(doc);
  docId += DOC_SIZE_SEP;
  docId += numToStr(std::min(shingleSet.size(), SKETCH_SIZE));
  const char* docIdCstr = docId.c_str();

  // Now emit an unbiased deterministic sample of <shingle, docId + size> pairs.
  size_t emitted = 0;
  for (ShingleSet::iterator it = shingleSet.begin();
      it != shingleSet.end() && emitted < SKETCH_SIZE; ++it, ++emitted) {
    // Base64-encode the shingle hash value to save space.
    printf("%s\t%s\n", base64Encode(*it).c_str(), docIdCstr);
  }
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
  // Check for the cmd line arg that specifies the name of the content node
  if (argc != 2) {
    std::cerr << "Usage: ./mapper1 contentTagName\n"
              << "\ncontentTagName\tThe node containing the doc's text.\n"
              << "\nExample:\n"
              << "zcat 00.trectext.gz | ./mapper1 body | sort > mapper1.out\n";
    exit(1);
  } else {
    contentStartTag = std::string("<") + argv[1] + ">";
    contentEndTag = std::string("</") + argv[1] + ">";
    contentStartLen = contentStartTag.length();
  }

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
