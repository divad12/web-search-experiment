// Mapper for Amazon Elastic Map Reduce using Hadoop streaming

#include <cassert>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <tr1/unordered_set>

#include "third_party/rabinhash-64/rabinhash64.h"

////////////////////////////////////////////////////////////////////////////////
const char* DOCNO_START_TAG = "<DOCNO>";
const char* DOCNO_END_TAG = "</DOCNO>";
const size_t DOCNO_START_LEN = strlen(DOCNO_START_TAG);

const char* DOC_START_TAG = "<DOC>";
const char* DOC_END_TAG = "</DOC>";

// TODO: it seems like in some articles that the main article is within the
//     HEADLINE tags, not the TEXT tags.
const char* TEXT_START_TAG = "<TEXT>";
const char* TEXT_END_TAG = "</TEXT>";
const size_t TEXT_START_LEN = strlen(TEXT_START_TAG);

const char* WHITESPACE = " \t\n";
const char* DOC_ID_SEP = "|";

// TODO: Should not declare static variables of class type.
// FIXME: This 3rd-party rabin hash function does not actually generate 64-bit
//     hashes (just 32-bit).
RabinHashFunction64 rabinHash(1);

const size_t SHINGLE_SIZE = 64;

////////////////////////////////////////////////////////////////////////////////
void trim(std::string& str) {
    str.erase(str.find_last_not_of(' ') + 1);
    str.erase(0, str.find_first_not_of(' '));
}

std::string numToStr(long long num) {
    char str[20];
    sprintf(str, "%lld", num);
    return std::string(str);
}

std::string padStr(const std::string& str, size_t size, char paddingChar) {
    std::stringstream ss;
    ss << std::setfill(paddingChar) << std::setw(size) << str;
    return ss.str();
}

void emitKeyValuePairs(const std::string& doc) {
    // TODO: need to worry about unicode?

    std::tr1::unordered_set<long long> shingleSet;
    size_t textStart = doc.find(TEXT_START_TAG) + TEXT_START_LEN;
    size_t textEnd = doc.find(TEXT_END_TAG, textStart);

    // TODO: could clean up xml if this is not done for actual input i.e.
    //     remove tags, collapse whitespace

    // Get the set of all shingles in this document
    for (size_t i = textStart; i + SHINGLE_SIZE < textEnd; ) {
        long long shingle = rabinHash.hash(doc.c_str() + i, SHINGLE_SIZE);
        shingleSet.insert(shingle);

        i = doc.find_first_of(WHITESPACE, i + 1);
        if (i == std::string::npos) break;
        i = doc.find_first_not_of(WHITESPACE, i + 1);
        if (i == std::string::npos) break;
    }

    if (textEnd - textStart < SHINGLE_SIZE) {
        std::string padded = padStr(doc.substr(textStart, textEnd - textStart), SHINGLE_SIZE, 'x');
        long long shingle = rabinHash.hash(padded.c_str(), SHINGLE_SIZE);
        shingleSet.insert(shingle);
    }

    // Get the document ID string concatenated with the # of unique shingles
    size_t docnoStart = doc.find(DOCNO_START_TAG) + DOCNO_START_LEN;
    size_t docnoEnd = doc.find(DOCNO_END_TAG, docnoStart);
    std::string docId = doc.substr(docnoStart, docnoEnd - docnoStart);
    trim(docId);
    docId += DOC_ID_SEP;
    docId += numToStr(shingleSet.size());
    const char* docIdCstr = docId.c_str();

    // Now emit the <shingle, docId + size> pairs.
    for (std::tr1::unordered_set<long long>::iterator it = shingleSet.begin();  // Wow what a mouthful... want C++0x auto
            it != shingleSet.end(); ++it) {
        printf("%lld\t%s\n", *it, docIdCstr);
    }
}

////////////////////////////////////////////////////////////////////////////////
int main() {
    char line[10000];

    std::string doc;
    bool inDoc = false;

    // Using scanf instead of getline because it is much more efficient. This
    // is important since we're streaming in large amount of data.
    while (scanf("%9999[^\n]\n", line) != EOF) {
        // Assumptions: no nested docs; doc tags are uppercase
        if (inDoc) {
            doc += line;
            doc += '\n';
            if (strstr(line, DOC_END_TAG)) {
                inDoc = false;
                emitKeyValuePairs(doc);
            }
        } else if (strstr(line, DOC_START_TAG)) {
            inDoc = true;
            doc = line;
            doc += '\n';
        }
    }

    return 0;
}
