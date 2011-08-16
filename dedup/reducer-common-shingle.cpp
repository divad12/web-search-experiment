// Applies the common shingles optimization: Filter out those shingles shared
// by too many documents
// <shingle, docId> --> <docId, shingle>

#include <cstdio>
#include <iostream>
#include <set>

#include "constants.h"

////////////////////////////////////////////////////////////////////////////////
typedef std::set<std::string> StringSet;

////////////////////////////////////////////////////////////////////////////////
void emitFilteredShingles(const StringSet& docSet, const char* shingle) {
  // Shingle too common usually means mechanically generated and little
  // effect on RR. Since they'll explode quadratically, we can't afford to
  // emit them.
  if (docSet.size() > dedup::COMMON_SHINGLE_THRESHOLD) return;

  for (StringSet::iterator it = docSet.begin(); it != docSet.end(); ++it) {
    printf("%s\t%s\n", it->c_str(), shingle);
  }
}

////////////////////////////////////////////////////////////////////////////////
int main() {
  char value[1000], key[1000];
  StringSet docSet;
  std::string prevKey;
  while (scanf("%s\t%s\n", key, value) != EOF) {
    if (prevKey != key) {
      emitFilteredShingles(docSet, prevKey.c_str());
      docSet.clear();
    }
    docSet.insert(value);
    prevKey = key;
  }

  emitFilteredShingles(docSet, prevKey.c_str());

  return 0;
}
