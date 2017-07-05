#ifndef ATL_STRING_H_
#define ATL_STRING_H_

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace atl {

std::string CreateSnippet(const std::string& str, unsigned int num_chars, const std::string& filler = "...");
std::string ToUpper(const std::string& str);
std::string ToLower(const std::string& str);

template <typename Container>
std::string Join(const Container& container, const std::string& delim) {
  std::ostringstream text;

  int length = container.size();
  for (auto it = container.cbegin(); it != container.cend(); ++it) {
    int i = std::distance(container.cbegin(), it);

    if (i < length - 1) {
      text << *it << delim;
      continue;
    }

    text << *it;
  }

  return text.str();
}

template <typename... Args>
std::string JoinStr(std::string delim, Args... args) {
  // FIXME(an): Make JoinStr mit Join consistent then ^^ should work hopefully.
  std::vector<std::string> vec = {args...};
  return Join(vec, delim);
}

std::vector<std::string> Split(const std::string& text,
                               const std::string& delim,
                               bool ignore_empty = false);

std::string TrimLeft(const std::string& text, const std::string& cutset);
std::string TrimRight(const std::string& text, const std::string& cutset);
std::string Trim(const std::string& text, const std::string& cutset);
std::string TrimSpace(const std::string& text);

}  // namespace atl

#endif  // ATL_STRING_H_
