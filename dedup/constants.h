#ifndef _DEDUP_CONSTANTS_
#define _DEDUP_CONSTANTS_

#include <cstdlib>
#include <cstring>

namespace dedup {

// 1 less than 100 to save space in output
const size_t SKETCH_SIZE = 99;
const size_t WORDS_PER_SHINGLE = 10;

const char* DOC_ID_SEP = "|";

const char* DOCNO_START_TAG = "<DOCNO>";
const char* DOCNO_END_TAG = "</DOCNO>";
const size_t DOCNO_START_LEN = strlen(DOCNO_START_TAG);

const char* DOC_START_TAG = "<DOC>";
const char* DOC_END_TAG = "</DOC>";

const char* TITLE_START_TAG = "<title>";
const char* TITLE_END_TAG = "</title>";
const size_t TITLE_START_LEN = strlen(TITLE_START_TAG);

}  // namespace dedup

#endif
