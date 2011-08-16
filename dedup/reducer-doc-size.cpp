// Generate <shingle,doc-size> pairs, with size of sketch info appended to each
// docID
// <docId, shingle> --> <shingle, docId-size>

#include <cassert>
#include <cstdio>
#include <iostream>
#include <set>

#include "constants.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
typedef std::set<std::string> StringSet;

////////////////////////////////////////////////////////////////////////////////
void emitShinglesWithDocSizes(const StringSet& shingleSet,
    const std::string& docId) {
  assert(shingleSet.size() <= dedup::SKETCH_SIZE);

  std::string docIdWithSize = docId + dedup::DOC_SIZE_SEP +
      dedup::numToStr(shingleSet.size());
  for (StringSet::iterator it = shingleSet.begin();
      it != shingleSet.end(); ++it) {
    printf("%s\t%s\n", it->c_str(), docIdWithSize.c_str());
  }
}

////////////////////////////////////////////////////////////////////////////////
// TODO: Duplicate code with reducer-shingle-threshold.cpp. Refactor into class
// with virtual method call to call the emit method
int main() {
  char value[1000], key[1000];
  StringSet shingleSet;
  std::string prevKey;
  while (scanf("%s\t%s\n", key, value) != EOF) {
    if (prevKey != key) {
      emitShinglesWithDocSizes(shingleSet, prevKey);
      shingleSet.clear();
    }
    shingleSet.insert(value);
    prevKey = key;
  }

  emitShinglesWithDocSizes(shingleSet, prevKey);

  return 0;
}
