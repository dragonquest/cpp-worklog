#include <algorithm>
#include <string>

#include "atl/string.h"
#include "worklog.h"

#include "filter.h"

namespace worklog {

Filter ParseFilter(const std::string& text) {
  Filter filter;

  bool ignore_empty = true;

  auto queries = atl::Split(text, " ", ignore_empty);
  for (const auto& query : queries) {
    std::vector<std::string> key_value = atl::Split(query, ":", ignore_empty);
    if (key_value.size() != 2) {
      // TODO(an): Display some better error message
      continue;
    }

    bool is_negated = false;
    if (key_value[0].find("-") == 0) {
      key_value[0] = key_value[0].substr(1);
      is_negated = true;
    }

    const std::string& key = key_value[0];
    const std::string& value = key_value[1];

    if (key == "tag") {
      if (is_negated) {
        filter.tags_negative.insert(value);
        continue;
      }

      filter.tags.insert(value);
    } else if (key == "subject") {
      filter.subject = value;
    }
  }

  return filter;
}

void ApplyFilter(std::function<bool(const worklog::Log&)> apply_func, std::vector<worklog::Log>* logs) {
  logs->erase(std::remove_if(logs->begin(), logs->end(), std::not1(apply_func)),
              logs->end());
}

bool Contains(const std::set<std::string>& haystack,
              const std::string& needle) {
  auto res = haystack.find(needle);
  return res != haystack.end();
}

std::function<bool(const worklog::Log&)> SubjectFilter(const Filter& filter) {
  return [&filter](const worklog::Log& log) -> bool {
    auto found = log.subject.find(filter.subject);
    return found != std::string::npos;
  };
}

std::function<bool(const worklog::Log&)> TagsFilter(const Filter& filter) {
  return [&filter](const worklog::Log& log) -> bool {
    int should_keep_score = 0;
    for (const auto& tag : log.tags) {
      if (Contains(filter.tags_negative, tag)) {
        should_keep_score--;
      }

      if (Contains(filter.tags, tag)) {
        should_keep_score++;
      }
    }

    return should_keep_score > 0;
  };
}

std::function<bool(const worklog::Log&)> OnlyValidFilter() {
  return [](const worklog::Log& log) -> bool {
    atl::Status valid_check = worklog::Validate(log);
    return valid_check.ok();
  };
}

std::function<bool(const worklog::Log&)> OnlyInvalidFilter() {
  return [](const worklog::Log& log) -> bool {
    atl::Status valid_check = worklog::Validate(log);
    return !valid_check.ok();
  };
}
} // namespace worklog

