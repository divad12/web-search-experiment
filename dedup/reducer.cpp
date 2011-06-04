// TODO: comment

#include <cstdio>
#include <iostream>
#include <set>

////////////////////////////////////////////////////////////////////////////////
typedef std::set<std::string> StringSet;

////////////////////////////////////////////////////////////////////////////////
void emitAllPairs(const StringSet& docSet) {
    for (StringSet::iterator it1 = docSet.begin();
            it1 != docSet.end(); ++it1) {

        StringSet::iterator it2 = it1;
        for (++it2; it2 != docSet.end(); ++it2) {
            printf("%s-%s\t1\n", it1->c_str(), it2->c_str());
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
        docSet.insert(value);
        if (prevKey != key) {
            if (docSet.size() > 1) {
                emitAllPairs(docSet);
            }
            docSet.clear();
        }
        prevKey = key;
    }

    if (docSet.size() > 1) {
        emitAllPairs(docSet);
    }

    // TODO: do a preliminary reduce right here, before this data gets sent
    // over the network

    return 0;
}
