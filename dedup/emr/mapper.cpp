// Mapper for Amazon Elastic Map Reduce using Hadoop streaming

#include <cstdio>
#include <cstring>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
const char* DOCNO_START_TAG = "<DOCNO>";
const char* DOCNO_END_TAG = "</DOCNO>";
const size_t DOCNO_START_LEN = strlen(DOCNO_START_TAG);

const char* DOC_START_TAG = "<DOC>";
const char* DOC_END_TAG = "</DOC>";

////////////////////////////////////////////////////////////////////////////////
// Work in progress. This just emits the pair <docId,1> for now.
void emitKeyValuePairs(const std::string& doc) {
    size_t docnoStart = doc.find(DOCNO_START_TAG);
    size_t docnoEnd = doc.find(DOCNO_END_TAG, 0);

    // TODO: Use boost to trim id string.
    std::string docId = doc.substr(docnoStart + DOCNO_START_LEN, docnoEnd - docnoStart - DOCNO_START_LEN);
    printf("LongValueSum:%s\t1\n", docId.c_str());
}

////////////////////////////////////////////////////////////////////////////////
int main() {
    char line[10000];

    std::string doc;
    bool inDoc = false;
    while (scanf("%[^\n]\n", line) != EOF) {
        // Assumptions: no nested docs; doc tags are uppercase
        if (inDoc) {
            doc += line;
            if (strstr(line, DOC_END_TAG)) {
                inDoc = false;
                emitKeyValuePairs(doc);
            }
        } else if (strstr(line, DOC_START_TAG)) {
            inDoc = true;
            doc = line;
        }
    }

    return 0;
}
