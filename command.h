#ifndef COMMAND_H_
#define COMMAND_H_

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "atl/status.h"
#include "atl/statusor.h"

#include "worklog.h"

namespace worklog {
struct CommandContext {
  std::vector<std::string> args;
  worklog::Config config;
};

class Command {
 public:
  using Action = std::function<int(const CommandContext&)>;

  Command(const std::string& name, const std::string& description,
          Action action)
      : name_(name), description_(description), action_(action) {}

  std::string name() const;
  std::string description() const;
  Action action() const;

 private:
  const std::string name_;
  const std::string description_;
  const Action action_;
};

class CommandParser {
 public:
  void Add(Command cmd);
  atl::StatusOr<Command::Action> Parse(const std::vector<std::string>& args);
  void PrintHelp() const;

 private:
  std::map<std::string, Command> commands_;
};


} // namespace worklog
#endif  // COMMAND_H_
