#include "utils.h"

namespace dedup {

// Base-64 encode an unsigned long long, to save space on hash values.
std::string base64Encode(unsigned long long num) {
  static const std::string base64Chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string encoded;
  while (num) {
    encoded += base64Chars[num & 077];
    num >>= 6;
  }
  return encoded;
}

void trim(std::string& str) {
  str.erase(str.find_last_not_of(' ') + 1);
  str.erase(0, str.find_first_not_of(' '));
}

std::string numToStr(int num) {
  char str[20];
  sprintf(str, "%d", num);
  return std::string(str);
}

}  // namespace dedup
