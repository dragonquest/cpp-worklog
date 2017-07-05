#include <string>

#include "atl/time.h"
#include "atl/string.h"

#include "worklog.h"
#include "serializer.h"

namespace worklog {

std::string HumanSerializer::Serialize(const Log& log) {
  std::ostringstream text;

  text << "date=" << atl::FormatTime(log.created_at) << "\n";
  text << "tags=" << atl::Join(log.tags, ", ") << "\n\n";
  text << atl::TrimSpace(log.subject) << "\n\n";
  text << atl::TrimSpace(log.description) << "\n\n";

  return text.str();
}

Log HumanSerializer::Unserialize(const std::string& text) {
  Log log;

  log.created_at = 0;
  log.id = 0;

  bool ignore_empty = false;

  auto res = atl::Split(text, "\n", ignore_empty);
  for (const auto& bit : res) {
    if (IsTag(bit)) {
      std::vector<std::string> tag = atl::Split(bit, "=");

      if (tag.size() != 2) {
        continue;
      }

      // The tag's can come along like 'tag= xxx', '   tag   =    xxx'
      // thats why we TrimSpace it here:
      const std::string& tag_name = atl::TrimSpace(tag[0]);
      const std::string& tag_value = atl::TrimSpace(tag[1]);

      if (tag_name == "" || tag_value == "") {
        continue;
      }

      if (tag_name == "tags") {
        // the tag is of type 'tags' which will contain multiple
        // comma separated values:
        auto tags = atl::Split(tag_value, ",");
        for (auto v : tags) {
          log.tags.insert(atl::TrimSpace(v));
        }

      } else if (tag_name == "date") {
        // the tag is of type 'date' which will contain a date
        // in the format: 2017-12-30
        auto time = atl::ParseTime(tag_value);
        if (!time.second) {
          continue;
        }

        log.created_at = atl::UnixTimestamp(time.first);
      } else {
        // TODO(an): Unknkown tag - handle here
      }
    } else if (log.subject == "") {
      log.subject = bit;
    } else {
      log.description += bit + "\n";
    }
  }

  return log;
}

} // namespace worklog
