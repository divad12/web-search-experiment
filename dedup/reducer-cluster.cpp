// Reducer for clustering (finding disjoint sets of similar docs)

#include <cstdio>
#include <set>
#include <string>

typedef std::set<std::string> StringSet;

void emitCluster(const std::string doc, const StringSet& cluster, double accumJaccard) {

    double avgJaccard = accumJaccard / cluster.size();
    // Only emit the cluster if the current key is the lowest in the cluster.
    // This prevents us from emitting the same cluster multiple times.
    if (doc <= *(cluster.begin())) {
        printf("%.4f\t%s", avgJaccard, doc.c_str());
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
    double jaccard = 0.0, accumJaccard = 0.0;

    while (scanf("%s\t%[^|]|%lf\n", doc1, doc2, &jaccard) != EOF) {
        if (prevKey != doc1 && !prevKey.empty()) {
            emitCluster(prevKey, cluster, accumJaccard);
            cluster.clear();
            accumJaccard = 0.0;
        }

        cluster.insert(doc2);
        accumJaccard += jaccard;
        prevKey = doc1;
    }

    if (!cluster.empty()) {
      emitCluster(prevKey, cluster, accumJaccard);
    }

    return 0;
}
