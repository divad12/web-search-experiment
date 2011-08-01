#ifndef _DEDUP_CONSTANTS_
#define _DEDUP_CONSTANTS_

#include <cstdlib>

namespace dedup {

// 1 less than 100 to save space in output
const size_t SKETCH_SIZE = 99;
const size_t WORDS_PER_SHINGLE = 10;

}  // namespace dedup

#endif
