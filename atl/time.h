#ifndef ATL_TIME_H_
#define ATL_TIME_H_

#include <chrono>
#include <string>

namespace atl {
uint64_t UnixTimestamp(
    const std::chrono::time_point<std::chrono::system_clock>& tp);

std::chrono::time_point<std::chrono::system_clock> CurrentTime();

std::pair<std::chrono::time_point<std::chrono::system_clock>, bool> ParseTime(
    const std::string& time, const std::string& format = "%Y-%m-%d");

std::string FormatTime(uint64_t time_date_stamp,
                       const std::string& format = "%Y-%m-%d");

bool IsValidTimestamp(uint64_t timestamp);
}  // namespace atl

#endif  // ATL_TIME_H_
