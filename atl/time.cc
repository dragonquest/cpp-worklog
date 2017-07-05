#include <chrono>
#include <ctime>
#include <iomanip>  // put_time
#include <sstream>
#include <string>

#include "time.h"

namespace atl {

uint64_t UnixTimestamp(
    const std::chrono::time_point<std::chrono::system_clock>& tp) {
  return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch())
      .count();
}

std::chrono::time_point<std::chrono::system_clock> CurrentTime() {
  return std::chrono::system_clock::now();
}

std::pair<std::chrono::time_point<std::chrono::system_clock>, bool> ParseTime(
    const std::string& time, const std::string& format) {
  std::tm t = {};
  std::istringstream ss(time);

  // ss.imbue(std::locale::classic());
  ss >> std::get_time(&t, format.c_str());
  if (ss.fail()) {
    return std::make_pair<std::chrono::time_point<std::chrono::system_clock>,
                          bool>(CurrentTime(), false);
  }

  t.tm_isdst = 1;
  auto tp = std::mktime(&t);
  return std::make_pair<std::chrono::time_point<std::chrono::system_clock>,
                        bool>(std::chrono::system_clock::from_time_t(tp), true);
}

std::string FormatTime(uint64_t time_date_stamp,
                       const std::string& format) {
  std::time_t temp = time_date_stamp;
  std::tm* t = std::localtime(&temp);
  t->tm_isdst = 1;
  std::stringstream ss;
  ss.imbue(std::locale::classic());
  ss << std::put_time(t, format.c_str());

  return ss.str();
}

bool IsValidTimestamp(uint64_t timestamp) {
  if (timestamp <= 0) {
    return false;
  }

  std::string year = atl::FormatTime(timestamp, "%Y");
  if (year.length() != 4) {
    return false;
  }

  std::string month = atl::FormatTime(timestamp, "%m");
  if (month.length() != 2) {
    return false;
  }

  std::string day = atl::FormatTime(timestamp, "%d");
  if (day.length() != 2) {
    return false;
  }

  return true;
}

}  // namespace atl
