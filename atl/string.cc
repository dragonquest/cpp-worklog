#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

#include <boost/algorithm/string/case_conv.hpp>

#include "string.h"

namespace atl {

std::string CreateSnippet(const std::string& str, unsigned int num_chars, const std::string& filler) {
  assert(num_chars - filler.length() > 0);

  unsigned int cut_after_num = num_chars - filler.length();

  if (str.length() > cut_after_num) {
    return str.substr(0, cut_after_num) + filler;
  }

  return str;
}

std::string ToUpper(const std::string& str) {
  return boost::to_upper_copy<std::string>(str);
}

std::string ToLower(const std::string& str) {
  return boost::to_lower_copy<std::string>(str);
}

std::vector<std::string> Split(const std::string& text,
                               const std::string& delim,
                               bool ignore_empty) {
  std::vector<std::string> res;

  std::size_t pos = 0;
  std::size_t len = text.length();

  while (pos < len) {
    auto n = text.find(delim, pos);
    if (n == std::string::npos) {
      break;
    }

    if (ignore_empty && (n - pos) == 0) {
      pos += delim.length();
      continue;
    }

    res.push_back(text.substr(pos, n - pos));
    pos = n + delim.length();
  }

  if (pos < len) {
    res.push_back(text.substr(pos));
  }

  return res;
}

std::string TrimLeft(const std::string& text, const std::string& cutset) {
  if (text.length() == 0) {
    return text;
  }

  auto n = text.find_first_not_of(cutset);
  if (n == std::string::npos) {
    return text;
  }

  return text.substr(n);
}

std::string TrimRight(const std::string& text, const std::string& cutset) {
  if (text.length() == 0) {
    return text;
  }

  auto n = text.find_last_not_of(cutset);
  if (n == std::string::npos) {
    return text;
  }

  return text.substr(0, n + 1);
}

std::string Trim(const std::string& text, const std::string& cutset) {
  if (text.length() == 0) {
    return text;
  }

  return TrimRight(TrimLeft(text, cutset), cutset);
}

std::string TrimSpace(const std::string& text) {
  std::string cutset(" \t\r\n");

  return Trim(text, cutset);
}

}  // namespace atl
