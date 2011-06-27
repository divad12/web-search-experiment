// Reducer for clustering (finding disjoint sets of similar docs)

#include <cstdio>
#include <set>
#include <string>

typedef std::set<std::string> StringSet;

void emitCluster(const std::string doc, const StringSet& cluster) {
    // Only emit the cluster if the current key is the lowest in the cluster.
    // This prevents us from emitting the same cluster multiple times.
    if (doc <= *(cluster.begin())) {
        printf("%s", doc.c_str());
        for (StringSet::iterator it = cluster.begin(); it != cluster.end(); ++it) {
            printf(", %s", it->c_str());
        }
        printf("\n");
    }
}

int main() {
    char doc1[1000], doc2[1000];
    StringSet cluster;
    std::string prevKey;

    while (scanf("%s\t%s\n", doc1, doc2) != EOF) {
        if (prevKey != doc1 && !prevKey.empty()) {
            emitCluster(prevKey, cluster);
            cluster.clear();
        }

        cluster.insert(doc2);
        prevKey = doc1;
    }

    if (!cluster.empty()) {
      emitCluster(prevKey, cluster);
    }

    return 0;
}
