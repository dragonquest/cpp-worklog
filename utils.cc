#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>

#include "atl/status.h"
#include "atl/optional.h"
#include "atl/time.h"
#include "atl/file.h"
#include "atl/colors.h"
#include "atl/string.h"

#include "serializer.h"
#include "worklog.h"
#include "utils.h"

std::string Template() {
  worklog::HumanSerializer hs;

  worklog::Log log;
  log.subject = "Title here";
  log.description = "Description here";
  log.tags = {"tag1", "tag2"};
  log.created_at = atl::UnixTimestamp(atl::CurrentTime());

  return hs.Serialize(log);
}

int PostEditValidation(const std::string& content) {
  worklog::HumanSerializer hs;
  worklog::Log log = hs.Unserialize(content);
  atl::Status valid_check = worklog::Validate(log);
  if (!valid_check.ok()) {
    std::cerr << "Error: The work log is invalid. Reason: "
              << valid_check.error_message()
              << ". Please fix!\n";
    return -1;
  }

  return 0;
}

atl::Optional<int> ExtractWorklogIdFromPath(const std::string& path) {
  auto pos = path.rfind("/");
  if (pos == std::string::npos) {
    return {};
  }

  return std::stoi(path.substr(pos + 1));
}

std::vector<worklog::Log> IndexFromDir(const std::string& path) {
  std::vector<worklog::Log> index;
  worklog::HumanSerializer hs;

  atl::WalkDir(path, [&index, &hs](const std::string& file) -> bool {
    atl::Optional<int> worklog_id = ExtractWorklogIdFromPath(file);
    if (!worklog_id) {
      std::cerr << "Failed to extract work log from path: " << file << "\n";
      return true;
    }

    auto content = atl::FileReadContent(file);
    if (!content) {
      std::cerr << "Error: Failed to read log: " << file << "\n";
      return true;
    }

    auto log = hs.Unserialize(content.value());
    log.id = worklog_id.value();

    index.push_back(log);
    return true;
  });

  // Sort by created_at DESC - newer entries are displayed first:
  std::sort(index.begin(), index.end(), [](const worklog::Log& a, const worklog::Log& b) {
    return a.created_at > b.created_at;
  });

  return index;
}

atl::Optional<int> NumberFromString(const std::string& number) {
  try {
    return std::stoi(number);
  } catch (const std::exception& e) {
    return {};
  }
}

void PrintWorklog(const worklog::Log& log) {
  if (log.id > 0) {
    std::cout << std::setw(10) << std::left << log.id;
  } else {
    std::cout << std::setw(10) << std::left << "???";
  }

  std::cout << atl::FormatTime(log.created_at) << "  " << atl::console::fg::yellow
            << std::setw(30) << std::left << atl::CreateSnippet(log.subject, 30)
            << atl::console::fg::reset;
  std::cout << "  [" << atl::Join(log.tags, ", ") << "]\n";
}

worklog::Command::Action MustBeInWorkspace(worklog::Command::Action action) {
  return [action](const worklog::CommandContext& ctx) -> int {
    if (!worklog::IsInWorklogSpace(ctx.config)) {
      std::cerr << "Fatal: Not in a worklog space. Please initialize a worklog first (see 'help')\n";
      return -1;
    }

    return action(ctx);
  };
}
