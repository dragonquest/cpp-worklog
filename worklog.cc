#include <algorithm>
#include <ctime>
#include <set>
#include <string>

#include "atl/file.h"
#include "atl/optional.h"
#include "atl/string.h"
#include "atl/time.h"

#include "process.h"
#include "worklog.h"

namespace worklog {

std::string Config::NextIdPath() const {
  return atl::JoinStr("/", meta_dir, next_id);
}

atl::Status Validate(const Log& log) {
  if (log.subject == "" || log.description == "") {
    return atl::Status(atl::error::INTERNAL, "Subject or description is empty.");
  }

  if (!atl::IsValidTimestamp(log.created_at)) {
    return atl::Status(atl::error::INTERNAL, "Invalid created_at date value. Expected format: 0000-00-00");
  }

  return atl::Status();
}

bool LaunchEditor(const std::string& file) {
  std::vector<std::string> args = {getenv("EDITOR"), file};
  Process ps;

  auto result = ps.Run(args);
  if (!result.is_ok()) {
    return false;
  }

  return true;
}

atl::Optional<std::string> ContentFromEditor(const std::string& file) {
  bool successful = LaunchEditor(file);
  if (!successful) {
    return {};
  }

  return atl::FileReadContent(file);
}

atl::Status MaybeSetupWorklogSpace(const Config& conf) {
  if (!atl::MkDir(conf.meta_dir)) {
    return atl::Status(atl::error::INTERNAL,
                       "Failed to create meta dir: " + conf.meta_dir);
  }

  if (!atl::MkDir(conf.logs_dir)) {
    return gtl::Status(atl::error::INTERNAL,
                       "Failed to create work log dir: " + conf.logs_dir);
  }

  atl::FileWriteContent(conf.NextIdPath(), "1");

  return atl::Status();
}

bool IsInWorklogSpace(const Config& conf) {
  return atl::FileExists(conf.meta_dir) &&
         atl::FileExists(conf.logs_dir) &&
         atl::FileExists(conf.NextIdPath());
}

atl::Optional<int> NextId(const Config& conf) {
  // FIXME(an): Better rrror handling?
  std::string next_id_file = conf.NextIdPath();

  if (!atl::FileExists(next_id_file)) {
    // When no next_id file has been found then we return '1' and save
    // '2' to next_id file, so it will work properly for the next time.
    atl::FileWriteContent(next_id_file, "2");
    return 1; // id
  }

  auto content = atl::FileReadContent(next_id_file);
  if (!content) {
    return {};
  }

  int id = std::atoi(content.value().c_str());
  atl::FileWriteContent(next_id_file, std::to_string(id + 1));

  return id;
}


}  // namespace worklog
