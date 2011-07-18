// This essentially implements the LongValueSum reducer in Hadoop's aggregate
// class, but with a few changes.

#include <cstdio>
#include <string>

void emitSumOfKeys(const char* key, int value) {
    // TODO: As an optimization, don't print doc pairs sharing less than two
    //     shingles in common. However, this nixes all docs < SHINGLE_SIZE
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
