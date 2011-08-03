// Reducer for clustering (finding disjoint sets of similar docs)

#include <cstdio>
#include <string>
#include <tr1/unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// A document in the union-find data structure.
class Doc {
 public:
  explicit Doc(Doc* parent) : parent_(parent), size_(1), accumJaccard_(0), numEdges_(0) {}

  void addEdge(float jaccard);
  void mergeWith(Doc* rep, float jaccard);
  Doc* findRep();  // non-const because we flatten tree structure on find
  int size() const { return size_; }
  float avgJaccard() const { return accumJaccard_ / numEdges_; }

 private:
  Doc* parent_;
  int size_;
  float accumJaccard_;
  int numEdges_;
};

void Doc::addEdge(float jaccard) {
  accumJaccard_ += jaccard;
  numEdges_++;
}

void Doc::mergeWith(Doc* rep, float jaccard) {
  parent_ = rep;
  rep->size_ += size_;
  rep->accumJaccard_ += accumJaccard_ + jaccard;
  rep->numEdges_ += numEdges_ + 1;
}

Doc* Doc::findRep() {
  if (!parent_) {
    return this;
  } else {
    // Flatten the tree structure: important optimization to attain inverse ackermann running time
    parent_ = parent_->findRep();
    return parent_;
  }
}

class DocSet {
 public:
  void addEdge(const std::string& doc1, const std::string& doc2, float jaccard);
  typedef std::vector<std::string> Cluster;
  typedef std::tr1::unordered_map<Doc*,Cluster> ClusterMap;
  // Outputs the clusters into an iterable data structure.
  void produceClusters(ClusterMap& outClusters);
 private:
  Doc* findOrMakeDoc(const std::string& doc);

  typedef std::tr1::unordered_map<std::string,Doc*> DocMap;
  DocMap docMap_;
};

Doc* DocSet::findOrMakeDoc(const std::string& doc) {
  DocMap::iterator it = docMap_.find(doc);
  if (it != docMap_.end()) {
    return it->second;
  }

  Doc* newDoc = new Doc(NULL);
  docMap_[doc] = newDoc;
  return newDoc;
}

void DocSet::addEdge(const std::string& doc1, const std::string& doc2, float jaccard) {
  Doc* rep1 = findOrMakeDoc(doc1)->findRep();
  Doc* rep2 = findOrMakeDoc(doc2)->findRep();

  if (rep1 == rep2) {
    rep1->addEdge(jaccard);
    return;
  }

  if (rep1->size() < rep2->size()) {
    rep1->mergeWith(rep2, jaccard);
  } else {
    rep2->mergeWith(rep1, jaccard);
  }
}

void DocSet::produceClusters(DocSet::ClusterMap& outClusters) {
  for (DocMap::iterator it = docMap_.begin(); it != docMap_.end(); ++it) {
    Doc* rep = it->second->findRep();
    outClusters[rep].push_back(it->first);
  }
}

////////////////////////////////////////////////////////////////////////////////
void emitClusters(const DocSet::ClusterMap& clusters) {
  for (DocSet::ClusterMap::const_iterator it = clusters.begin(); it != clusters.end(); ++it) {
    printf("%.4f\t", it->first->avgJaccard());
    const DocSet::Cluster& cluster = it->second;
    for (DocSet::Cluster::const_iterator it2 = cluster.begin(); it2 != cluster.end(); ++it2) {
      if (it2 != cluster.begin()) {
        printf(", ");
      }
      printf("%s", it2->c_str());
    }
    printf("\n");
  }
}

////////////////////////////////////////////////////////////////////////////////
int main() {
  char doc1[1000], doc2[1000];
  float jaccard;
  DocSet docSet;

  while (scanf("1\t%s\t%s\t%f\n", doc1, doc2, &jaccard) != EOF) {
    docSet.addEdge(doc1, doc2, jaccard);
  }

  DocSet::ClusterMap clusters;
  docSet.produceClusters(clusters);
  emitClusters(clusters);

  return 0;
}
