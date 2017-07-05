#include <iomanip>
#include <map>
#include <string>
#include <iostream>

#include "command.h"

namespace worklog {
std::string Command::name() const { return name_; }
std::string Command::description() const { return description_; }
Command::Action Command::action() const { return action_; }

void CommandParser::Add(Command cmd) {
  commands_.emplace(cmd.name(), std::move(cmd));
}

atl::StatusOr<Command::Action> CommandParser::Parse(const std::vector<std::string>& args) {
  if (args.size() <= 1) {
    return atl::Status(atl::error::INVALID_ARGUMENT, "No command specified");
  }

  const std::string& name = args[1];

  auto entry = commands_.find(name);
  if (entry != commands_.end()) {
    return entry->second.action();
  }

  return atl::Status(atl::error::NOT_FOUND, "Command not found");
}

void CommandParser::PrintHelp() const {
  std::cout << "Available commands: \n";
  for (const auto& entry : commands_) {
    const Command& cmd = entry.second;

    std::cout << "  ";
    std::cout << std::setw(20) << std::left << cmd.name();
    std::cout << cmd.description() << "\n";
  }
  std::cout << "\n";
}

} // namespace worklog
