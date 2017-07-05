#include "process.h"

namespace worklog {
ProcessStatus Process::Run(const std::vector<std::string>& args) {
  ProcessStatus ps;
  int child_pid = fork();
  int child_status;

  if (child_pid == -1) {
    ps.process.fork_failed = true;
    ps.error = "Failed to create a new process";
    return ps;
  }

  // Parse arguments
  char* cargs[args.size() + 1];
  int i = 0;
  for (; i < args.size(); i++) {
    cargs[i] = (char*)args[i].c_str();
  }
  cargs[i] = NULL;

  if (child_pid == 0) {
    execvp(cargs[0], cargs);
  }

  pid_t pid;
  do {
    pid = wait(&child_status);

    ps.process.pid = pid;

    if (WIFEXITED(child_status)) {
      ps.process.exited = true;
      ps.process.exit_code = WEXITSTATUS(child_status);
    }

    if (WIFSIGNALED(child_status)) {
      ps.process.killed = true;
      ps.error = "Killed by signal: " + std::to_string(WTERMSIG(child_status));
    }

    if (WIFSTOPPED(child_status)) {
      ps.process.stopped = true;
      ps.error = "Stopped by signal: " + std::to_string(WSTOPSIG(child_status));
    }

    if (WIFSTOPPED(child_status)) {
      ps.process.continued = true;
      ps.error =
          "Continued by signal: " + std::to_string(WSTOPSIG(child_status));
    }

  } while (!WIFEXITED(child_status) && !WIFSIGNALED(child_status));

  return ps;
}
} // namespace worklog
