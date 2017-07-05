#include <string>
#include <vector>

#include "atl/file.h"
#include "atl/optional.h"
#include "atl/status.h"
#include "atl/statusor.h"
#include "atl/string.h"

#include "serializer.h"
#include "storage.h"
#include "worklog.h"

namespace worklog {
atl::StatusOr<Log> Storage::LoadById(int id) {
  std::string log_path =
      atl::JoinStr("/", config_.logs_dir, std::to_string(id));

  if (!atl::FileExists(log_path)) {
    return atl::Status(atl::error::NOT_FOUND,
                       "The work log does not exist under: " + log_path);
  }

  atl::Optional<std::string> content = atl::FileReadContent(log_path);
  if (!content) {
    return atl::Status(atl::error::INTERNAL,
                       "Failed to read content from: " + log_path);
  }

  Log log = hs_.Unserialize(content.value());
  log.id = id;
  return log;
}

atl::Status Storage::Save(Log& log) {
  if (log.id > 0) {
    return atl::Status(atl::error::INVALID_ARGUMENT,
                       "Worklog has an id, please use Update");
  }

  atl::Optional<int> next_id_value = NextId(config_);
  if (!next_id_value) {
    return atl::Status(atl::error::INTERNAL, "Failed to generate a new id");
  }

  int next_id = next_id_value.value();
  log.id = next_id;

  std::string log_path =
      atl::JoinStr("/", config_.logs_dir, std::to_string(log.id));

  if (atl::FileExists(log_path)) {
    return atl::Status(atl::error::INTERNAL,
                       "A worklog does already exist under: " + log_path);
  }

  atl::FileWriteContent(log_path, hs_.Serialize(log));

  return atl::Status();
}

atl::Status Storage::Update(const Log& log) {
  if (log.id <= 0) {
    return atl::Status(atl::error::INVALID_ARGUMENT,
                       "Worklog has no id, please use Save");
  }

  std::string log_path =
      atl::JoinStr("/", config_.logs_dir, std::to_string(log.id));
  if (!atl::FileExists(log_path)) {
    return atl::Status(atl::error::INTERNAL,
                       "A worklog does not yet exist under: " + log_path);
  }

  atl::FileWriteContent(log_path, hs_.Serialize(log));

  return atl::Status();
}
}  // namespace worklog
