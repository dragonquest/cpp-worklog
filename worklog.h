#ifndef WORKLOG_H_
#define WORKLOG_H_

#include <set>
#include <string>

#include "atl/optional.h"
#include "atl/status.h"

namespace worklog {

struct Log {
  int id;
  std::string subject;
  std::string description;
  uint64_t created_at;
  std::set<std::string> tags;
};

struct Config {
  std::string meta_dir = ".worklog";
  std::string logs_dir = ".worklog/logs";

  std::string next_id = "next_id";
  std::string NextIdPath() const;
};

atl::Status Validate(const Log& log);
bool LaunchEditor(const std::string& file);
atl::Optional<std::string> ContentFromEditor(const std::string& file);
atl::Status MaybeSetupWorklogSpace(const Config& conf);
bool IsInWorklogSpace(const Config& conf);
atl::Optional<int> NextId(const Config& conf);

}  // namespace worklog

#endif  // WORKLOG_H_
