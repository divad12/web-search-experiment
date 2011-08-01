#ifndef _DEDUP_UTILS_
#define _DEDUP_UTILS_

#include <string>

namespace dedup {

std::string base64Encode(unsigned long long num);
void trim(std::string& str);
std::string numToStr(int num);

}  // namespace dedup

#endif
