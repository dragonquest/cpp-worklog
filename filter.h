#ifndef FILTER_H_
#define FILTER_H_
#include <set>
#include <functional>

namespace worklog {
struct Filter {
  std::set<std::string> tags;
  std::set<std::string> tags_negative;
  std::string subject;
};

Filter ParseFilter(const std::string& text);
void ApplyFilter(std::function<bool(const worklog::Log&)> apply_func, std::vector<worklog::Log>* logs);

std::function<bool(const worklog::Log&)> SubjectFilter(const Filter& filter);
std::function<bool(const worklog::Log&)> TagsFilter(const Filter& filter);
std::function<bool(const worklog::Log&)> OnlyValidFilter();
std::function<bool(const worklog::Log&)> OnlyInvalidFilter();

} // namespace worklog
#endif  // FILTER_H_
