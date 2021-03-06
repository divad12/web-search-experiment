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
// TODO: Global variables used for efficiency (so we don't pass things into and
// out of functions. Refactor to be a class and make things member functions.

// The node containing the text to be shingled
static std::string contentStartTag;
static std::string contentEndTag;
static int contentStartLen;

static std::string commonDocIdPrefix = "";
static size_t commonDocIdPrefixLen = 0;

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

// Removes a common prefix from a docID, but first ensures that the given
// string has that prefix.
std::string removeCommonPrefix(const std::string& str) {
  assert(str.compare(0, commonDocIdPrefixLen, commonDocIdPrefix) == 0);
  return str.substr(commonDocIdPrefixLen);
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

  // Now emit an unbiased deterministic sample of <shingle, docId> pairs.
  std::string docId = removeCommonPrefix(getDocId(doc));
  size_t emitted = 0;
  for (ShingleSet::iterator it = shingleSet.begin();
      it != shingleSet.end() && emitted < SKETCH_SIZE; ++it, ++emitted) {
    // Base64-encode the shingle hash value to save space.
    printf("%s\t%s\n", base64Encode(*it).c_str(), docId.c_str());
  }
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
  // Check for the cmd line args
  if (argc < 2 || argc > 3) {
    std::cerr << "Usage: ./mapper1 contentTagName [commonPrefix]\n"
              << "\ncontentTagName\tThe node containing the doc's text.\n"
              << "\ncommonPrefix\tA common prefix of all docIDs (for optimization purposes."
              << "\nExample:\n"
              << "zcat 00.trectext.gz | ./mapper1 body clueweb09-en0000-00 | sort > mapper1.out\n";
    exit(1);
  } else {
    contentStartTag = std::string("<") + argv[1] + ">";
    contentEndTag = std::string("</") + argv[1] + ">";
    contentStartLen = contentStartTag.length();
    if (argc >= 3) {
      commonDocIdPrefix = argv[2];
      commonDocIdPrefixLen = commonDocIdPrefix.length();
    }
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
