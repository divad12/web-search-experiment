// TODO: comment and clean up

#include <cstdio>
#include <string>

void emitJaccard(const std::string& key, int intersection) {
    static char doc1[1000], doc2[1000];
    static int size1, size2;
    sscanf(key.c_str(), "%[^|]|%d-%[^|]|%d", doc1, &size1, doc2, &size2);
    double jaccard = static_cast<double>(intersection) / (size1 + size2 - intersection);
    printf("%s-%s\t%g\n", doc1, doc2, jaccard);
}

int main() {
    char key[1000];
    int value;
    std::string prevKey;
    int inCommon = 0;
    while (scanf("%s\t%d\n", key, &value) != EOF) {
        inCommon++;
        if (prevKey != key) {
            emitJaccard(prevKey, inCommon);
            inCommon = 0;
        }
        prevKey = key;
    }

    emitJaccard(prevKey, inCommon);

    return 0;
}
