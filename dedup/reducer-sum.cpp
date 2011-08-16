// This essentially implements the LongValueSum reducer in Hadoop's aggregate
// class, but possibly with a few changes. This can also be used as a combiner
// after reducer1
// <docId-size-docId-size,#> --> <docId-size-docId-size,#>

#include <cstdio>
#include <iostream>
#include <string>

void emitSumOfKeys(const char* key, int value) {
    // TODO: Optimization: Check jaccard similarity here and prevent output of
    //     those under threshold. If this is done then this program cannot be
    //     used as a local combiner.
    if (value > 0) {
        printf("%s\t%d\n", key, value);
    }
}

int main() {
    char key[1000];
    int value;
    std::string prevKey;
    int inCommon = 0;

    // TODO: Optimizations: Don't use std::strings
    while (scanf("%s\t%d\n", key, &value) != EOF) {
        if (prevKey != key && !prevKey.empty()) {
            emitSumOfKeys(prevKey.c_str(), inCommon);
            inCommon = 0;
        }
        inCommon += value;
        prevKey = key;
    }

    emitSumOfKeys(prevKey.c_str(), inCommon);

    return 0;
}
