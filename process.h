#ifndef PROCESS_H_
#define PROCESS_H_

#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

namespace worklog {
struct ProcessInfo {
  long pid = 0;

  int exit_code = 0;

  bool fork_failed = false;

  bool exited = false;
  bool killed = false;
  bool stopped = false;
  bool continued = false;
};

struct ProcessStatus {
  ProcessInfo process;
  std::string error;

  bool is_ok() const {
    return process.exited && process.exit_code == 0;
  }
};

class Process {
 public:
  ProcessStatus Run(const std::vector<std::string>& args);
};
} // namespace worklog

#endif  // PROCESS_H_

