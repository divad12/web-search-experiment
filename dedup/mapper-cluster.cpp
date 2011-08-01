// Mapper for clustering (finding disjoint sets of similar documents)
// <docId pair, # common shingles> --> bi-directional edges of similiarity graph

#include <cstdio>
#include <string>

const double NEAR_DUPLICATE_THRESHOLD = 0.5;

// TODO: duplicate code with mapper3.cpp
// Emits pairs <doc1, doc2> and <doc2, doc1> if doc1 and doc2 are
// near-duplicates. This is an undirected edge in our similarity graph.
void emitEdge(const std::string& key, int intersection) {
    static char doc1[1000], doc2[1000];
    static int size1, size2;
    sscanf(key.c_str(), "%[^|]|%d=%[^|]|%d", doc1, &size1, doc2, &size2);
    double jaccard = static_cast<double>(intersection) / (size1 + size2 - intersection);

    if (jaccard >= NEAR_DUPLICATE_THRESHOLD) {
        printf("%s\t%s\n", doc1, doc2);
        printf("%s\t%s\n", doc2, doc1);
    }
}

int main() {
    char key[1000];
    int value;
    while (scanf("%s\t%d\n", key, &value) != EOF) {
        emitEdge(key, value);
    }

    return 0;
}
