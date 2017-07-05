#ifndef STORAGE_H_
#define STORAGE_H_

#include "atl/status.h"
#include "atl/statusor.h"

#include "serializer.h"
#include "worklog.h"

namespace worklog {
class Storage {
 public:
  explicit Storage(const Config& config) : config_(config) {}

  atl::StatusOr<Log> LoadById(int id);
  atl::Status Save(Log& log);
  atl::Status Update(const Log& log);

 private:
  Config config_;
  HumanSerializer hs_;
};
}  // namespace worklog

#endif  // STORAGE_H
