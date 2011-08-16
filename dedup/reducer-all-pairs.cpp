// For every shingle, emit all pairs of documents that share that shingle.

#include <cassert>
#include <cstdio>
#include <iostream>
#include <set>

#include "constants.h"

////////////////////////////////////////////////////////////////////////////////
typedef std::set<std::string> StringSet;

////////////////////////////////////////////////////////////////////////////////
void emitAllPairs(const StringSet& docSet) {
    if (docSet.size() < 2) return;  // No pairs to emit
    assert(docSet.size() <= dedup::COMMON_SHINGLE_THRESHOLD);

    for (StringSet::iterator it1 = docSet.begin();
            it1 != docSet.end(); ++it1) {

        StringSet::iterator it2 = it1;
        for (++it2; it2 != docSet.end(); ++it2) {
            printf("%s%s%s\t1\n", it1->c_str(), dedup::DOC_ID_SEP, it2->c_str());
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
int main() {
    // Just generate n(n+1)/2 pairs of the input
    char key[1000], value[1000];
    std::set<std::string> docSet;
    std::string prevKey;
    while (scanf("%s\t%s\n", key, value) != EOF) {
        if (prevKey != key) {
            emitAllPairs(docSet);
            docSet.clear();
        }
        docSet.insert(value);
        prevKey = key;
    }

    emitAllPairs(docSet);

    // TODO: do a preliminary reduce2 (sum) on this machine before this data is
    // uploaded to S3. This does not seem possible with Hadoop streaming.

    return 0;
}
