#ifndef UTILS_H_
#define UTILS_H_
#include <string>
#include <vector>

#include "command.h"

std::string Template();
int PostEditValidation(const std::string& content);
atl::Optional<int> ExtractWorklogIdFromPath(const std::string& path);
std::vector<worklog::Log> IndexFromDir(const std::string& path);
atl::Optional<int> NumberFromString(const std::string& number);
void PrintWorklog(const worklog::Log& log);
worklog::Command::Action MustBeInWorkspace(worklog::Command::Action action);
#endif  // UTILS_H_
