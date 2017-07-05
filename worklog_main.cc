#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "atl/colors.h"
#include "atl/file.h"
#include "atl/status.h"
#include "atl/statusor.h"
#include "atl/string.h"
#include "atl/time.h"

#include "command.h"
#include "filter.h"
#include "serializer.h"
#include "utils.h"
#include "worklog.h"

#include "storage.h"

int CommandNewWorklog(const worklog::CommandContext& ctx) {
  atl::TempFile tmp_file;
  if (!tmp_file) {
    std::cerr << "Error: Failed to create temp file at: " << tmp_file.path()
              << "\n";
    return -1;
  }

  tmp_file.stream() << Template();
  tmp_file.stream().flush();

  auto content = worklog::ContentFromEditor(tmp_file.path());
  if (!content) {
    std::cerr << "Error: Failed to obtain content from the editor "
              << "which is stored in the file: " << tmp_file.path() << "\n";

    return -1;
  }

  worklog::HumanSerializer hs;
  worklog::Log log = hs.Unserialize(content.value());

  worklog::Storage store(ctx.config);
  atl::Status status = store.Save(log);
  if (!status.ok()) {
    std::cerr << "Failed to save the work log. Reason: "
              << status.error_message()
              << ". Your work log is backed up here: " << tmp_file.path()
              << "\n";

    return -1;
  }

  return 0;
}

int CommandEditWorklog(const worklog::CommandContext& ctx) {
  if (ctx.args.size() < 3) {
    std::cerr << "Error: Please specify a work log id\n";
    return -1;
  }

  const std::string& worklog_id = ctx.args[2];
  worklog::Storage store(ctx.config);

  atl::Optional<int> id = NumberFromString(worklog_id);
  if (!id) {
    std::cerr << "Error: Failed to convert worklog id to numeric value: "
              << worklog_id << "\n";

    return -1;
  }

  atl::StatusOr<worklog::Log> status = store.LoadById(id.value());
  if (!status.ok()) {
    std::cerr << "Error: Failed to load worklog by id " << worklog_id << ". "
              << status.status().error_message() << "\n";
    return -1;
  }

  auto log = status.ValueOrDie();
  worklog::HumanSerializer hs;

  atl::TempFile tmp_file;
  if (!tmp_file) {
    std::cerr << "Error: Failed to create temp file at: " << tmp_file.path()
              << "\n";
    return -1;
  }

  tmp_file.stream() << hs.Serialize(log);
  tmp_file.stream().flush();

  auto content = worklog::ContentFromEditor(tmp_file.path());
  if (!content) {
    std::cerr << "Error: Failed to obtain content from the editor "
              << "which is stored in the file: " << tmp_file.path() << "\n";

    return -1;
  }

  worklog::Log updatedLog = hs.Unserialize(content.value());
  updatedLog.id = log.id;

  atl::Status saveStatus = store.Update(updatedLog);
  if (!saveStatus.ok()) {
    std::cerr << "Failed to save the work log. Reason: "
              << saveStatus.error_message()
              << ". Your work log is backed up here: " << tmp_file.path()
              << "\n";

    return -1;
  }

  return 0;
}

int CommandViewWorklog(const worklog::CommandContext& ctx) {
  if (ctx.args.size() < 3) {
    std::cerr << "Error: Please specify a work log id\n";
    return -1;
  }

  const std::string& worklog_id = ctx.args[2];
  worklog::Storage store(ctx.config);

  atl::Optional<int> id = NumberFromString(worklog_id);
  if (!id) {
    std::cerr << "Error: Failed to convert worklog id to numeric value: "
              << worklog_id << "\n";

    return -1;
  }

  atl::StatusOr<worklog::Log> status = store.LoadById(id.value());
  if (!status.ok()) {
    std::cerr << "Error: Failed to load worklog by id " << worklog_id << ". "
              << status.status().error_message() << "\n";
    return -1;
  }

  auto log = status.ValueOrDie();
  worklog::HumanSerializer hs;

  std::cout << hs.Serialize(log) << "\n";

  return 0;
}

int CommandInitWorklog(const worklog::CommandContext& ctx) {
  atl::Status status = worklog::MaybeSetupWorklogSpace(ctx.config);
  if (!status.ok()) {
    std::cerr << "Failed to setup worklog space: " << status.error_message()
              << "\n";

    return -1;
  }

  return 0;
}

int CommandDeleteWorklog(const worklog::CommandContext& ctx) {
  if (ctx.args.size() < 3) {
    std::cerr << "Error: Please specify a work log id\n";
    return -1;
  }

  int id = 0;

  try {
    id = std::stoi(ctx.args[2]);
  } catch (const std::exception& e) {
    std::cerr << "Error: cannot extract id from parameter (not a number?): "
              << e.what() << "\n";
    return -1;
  }

  std::string log_path =
      atl::JoinStr("/", ctx.config.logs_dir, std::to_string(id));

  if (!atl::FileExists(log_path)) {
    return 0;
  }

  atl::Remove(log_path);

  return 0;
}

int CommandListBroken(const worklog::CommandContext& ctx) {
  auto index = IndexFromDir(ctx.config.logs_dir);
  worklog::ApplyFilter(worklog::OnlyInvalidFilter(), &index);

  for (const auto& log : index) {
    PrintWorklog(log);
  }

  return 0;
}

int CommandListAll(const worklog::CommandContext& ctx) {
  auto index = IndexFromDir(ctx.config.logs_dir);
  worklog::ApplyFilter(worklog::OnlyValidFilter(), &index);

  for (const auto& log : index) {
    PrintWorklog(log);
  }

  return 0;
}

int SubCommandTagsListAll(const worklog::CommandContext& ctx) {
  auto index = IndexFromDir(ctx.config.logs_dir);
  worklog::ApplyFilter(worklog::OnlyValidFilter(), &index);

  std::vector<std::string> tags;
  std::unordered_map<std::string, int> counts;

  for (const auto& log : index) {
    for (const auto& tag : log.tags) {
      auto found = counts.find(tag);
      if (found == counts.end()) {
        tags.push_back(tag);
      }

      counts[tag]++;
    }
  }

  std::sort(tags.begin(), tags.end(),
            [&counts](const std::string& a, const std::string& b) {
              return counts[a] > counts[b];
            });

  for (const auto& tag : tags) {
    std::cout << counts[tag] << " " << tag << "\n";
  }

  return 0;
}

int SubCommandTagsRemove(const worklog::CommandContext& ctx) {
  const std::string& tag = ctx.args[3];
  const std::string& worklog_id = ctx.args[4];

  worklog::Storage store(ctx.config);

  atl::Optional<int> id = NumberFromString(worklog_id);
  if (!id) {
    std::cerr << "Error: Failed to convert worklog id to numeric value: "
              << worklog_id << "\n";

    return -1;
  }

  atl::StatusOr<worklog::Log> status = store.LoadById(id.value());
  if (!status.ok()) {
    std::cerr << "Error: Failed to load worklog by id " << worklog_id << ". "
              << status.status().error_message() << "\n";
    return -1;
  }

  auto log = status.ValueOrDie();

  log.tags.erase(tag);

  atl::Status updateStatus = store.Update(log);
  if (!updateStatus.ok()) {
    std::cerr << "Error: Failed to update log with id: " << worklog_id << ". "
              << updateStatus.error_message() << "\n";
    return -1;
  }

  return 0;
}

int SubCommandTagsAdd(const worklog::CommandContext& ctx) {
  const std::string& tag = ctx.args[3];
  const std::string& worklog_id = ctx.args[4];

  worklog::Storage store(ctx.config);

  atl::Optional<int> id = NumberFromString(worklog_id);
  if (!id) {
    std::cerr << "Error: Failed to convert worklog id to numeric value: "
              << worklog_id << "\n";

    return -1;
  }

  atl::StatusOr<worklog::Log> status = store.LoadById(id.value());
  if (!status.ok()) {
    std::cerr << "Error: Failed to load worklog by id " << worklog_id << ". "
              << status.status().error_message() << "\n";
    return -1;
  }

  auto log = status.ValueOrDie();

  log.tags.insert(tag);

  atl::Status updateStatus = store.Update(log);
  if (!updateStatus.ok()) {
    std::cerr << "Error: Failed to update log with id: " << worklog_id << ". "
              << updateStatus.error_message() << "\n";
    return -1;
  }

  return 0;
}

int CommandTags(const worklog::CommandContext& ctx) {
  if (ctx.args.size() < 3) {
    std::cerr << "Missing arguments. Please specify if you want to "
              << "add, remove or list tags.\n"
              << "Examples: \n\n"
              << ctx.args[0] << " tag add php <id>        "
              << "adds php tag to worklog\n"
              << ctx.args[0] << " tag remove php <id>     "
              << "removes php tag from worklog\n"
              << ctx.args[0] << " tag list                "
              << "lists all available tags\n";

    return -1;
  }

  if (ctx.args[2] == "list" || ctx.args[2] == "all") {
    return SubCommandTagsListAll(ctx);
  } else if (ctx.args[2] == "add") {
    if (ctx.args.size() < 5) {
      std::cerr << "Please specify a tag and worklog id.\n"
                << "Enter: " << ctx.args[0] << " tag for further help\n";

      return -1;
    }

    return SubCommandTagsAdd(ctx);
  } else if (ctx.args[2] == "del" || ctx.args[2] == "remove" ||
             ctx.args[2] == "rm") {
    if (ctx.args.size() < 5) {
      std::cerr << "Please specify a tag and worklog id.\n"
                << "Enter: " << ctx.args[0] << " tag for further help\n";

      return -1;
    }

    return SubCommandTagsRemove(ctx);
  }

  return 0;
  auto index = IndexFromDir(ctx.config.logs_dir);
  worklog::ApplyFilter(worklog::OnlyValidFilter(), &index);
}

int CommandYearly(const worklog::CommandContext& ctx) {
  auto index = IndexFromDir(ctx.config.logs_dir);
  worklog::ApplyFilter(worklog::OnlyValidFilter(), &index);

  int prev_year = 0;
  for (const auto& log : index) {
    int year = std::stoi(atl::FormatTime(log.created_at, "%Y"));
    if (prev_year != year) {
      if (prev_year != 0) {
        std::cout << "\n";
      }

      std::cout << atl::console::style::bold << atl::console::fg::cyan << year
                << ":" << atl::console::fg::reset << atl::console::style::reset
                << "\n";

      prev_year = year;
    }

    PrintWorklog(log);
  }

  return 0;
}

int CommandSearch(const worklog::CommandContext& ctx) {
  if (ctx.args.size() < 3) {
    std::cerr << "Error: Please specify a search query or use the 'list' "
                 "command if you want to list all entries\n";
    return 1;
  }

  // Concat the args so we have a string for the ParseFilter
  int first_tag_idx = 2;
  std::ostringstream text;
  for (unsigned int i = first_tag_idx; i < ctx.args.size(); i++) {
    text << ctx.args[i] << " ";
  }

  const worklog::Filter& filter = worklog::ParseFilter(text.str());
  std::vector<worklog::Log> index = IndexFromDir(ctx.config.logs_dir);

  worklog::ApplyFilter(worklog::OnlyValidFilter(), &index);

  if (filter.tags.size() > 0 || filter.tags_negative.size() > 0) {
    worklog::ApplyFilter(worklog::TagsFilter(filter), &index);
  }

  if (filter.subject.length() > 0) {
    worklog::ApplyFilter(worklog::SubjectFilter(filter), &index);
  }

  for (const auto& log : index) {
    PrintWorklog(log);
  }

  return 0;
}

// Forward declaring this function because we need it only in CommandRepeat but
// it is defined below:
int ParseAndExecute(const std::vector<std::string>& args);

int CommandRepeat(const worklog::CommandContext& ctx,
                  const worklog::CommandParser& cp) {
  if (ctx.args.size() < 4) {
    std::cerr << "Error: Invalid input for repetition. Format example: "
              << ctx.args[0] << " rep 1,3,4 view\n";
    return -1;
  }

  const std::string selector_part = ctx.args[2];
  const std::string rep_command = ctx.args[3];

  std::vector<std::string> ids = atl::Split(selector_part, ",", true);

  int batch_exit_code = 0;
  for (const auto& id : ids) {
    std::vector<std::string> args = {ctx.args[0], rep_command, id};

    std::cerr << "Executed: " << atl::Join(args, " ")
              << ". Potential output:\n";
    int exit_code = ParseAndExecute(args);
    std::cerr << "... returned with exit code: " << exit_code << "\n";

    // if it has an optional separator then we print it:
    if (ctx.args.size() == 5) {
      std::cout << ctx.args[4] << "\n";
    }

    if (exit_code != 0) {
      // Overwriting the exit code only if the code is non zero.
      batch_exit_code = exit_code;
    }
  }

  return batch_exit_code;
}

worklog::CommandParser MakeCommandParser() {
  using worklog::Command;
  using worklog::CommandParser;

  CommandParser cp;
  cp.Add(Command("init", "initializes a worklog space", &CommandInitWorklog));
  cp.Add(Command("new", "add a new work log",
                 MustBeInWorkspace(&CommandNewWorklog)));
  cp.Add(Command("edit",
                 "edit a work log. An additional id parameter is required.",
                 MustBeInWorkspace(&CommandEditWorklog)));
  cp.Add(Command("view",
                 "view a work log. An additional id parameter is required.",
                 MustBeInWorkspace(&CommandViewWorklog)));
  cp.Add(Command("rm",
                 "removes a work log. An additional id parameter is required.",
                 MustBeInWorkspace(&CommandDeleteWorklog)));
  cp.Add(Command("list", "lists all logs", MustBeInWorkspace(&CommandListAll)));
  cp.Add(Command("broken", "lists all invalid logs",
                 MustBeInWorkspace(&CommandListBroken)));

  cp.Add(Command("tag", "add, remove or list tags",
                 MustBeInWorkspace(&CommandTags)));

  cp.Add(Command("search",
                 "search the work logs by a filter: tag:php -tag:javascript",
                 MustBeInWorkspace(&CommandSearch)));

  // TODO(an): make it 'stats yearly':
  cp.Add(Command("yearly", "shows a breakdown report by year",
                 MustBeInWorkspace(&CommandYearly)));
  cp.Add(Command("rep",
                 "repeats a command. Example: ./tool rep 1,3,7 view "
                 "[\"separator string\"] (shows 1, 3 & 7 in a loop)",
                 [&cp](const worklog::CommandContext& ctx) -> int {
                   return CommandRepeat(ctx, cp);
                 }));

  cp.Add(Command("help", "shows this help",
                 [&cp](const worklog::CommandContext& ctx) -> int {
                   cp.PrintHelp();
                   return 0;
                 }));

  return cp;
}

int ParseAndExecute(const std::vector<std::string>& args) {
  worklog::CommandParser cp = MakeCommandParser();

  atl::StatusOr<worklog::Command::Action> parsing = cp.Parse(args);
  if (!parsing.ok()) {
    std::cout << parsing.status() << "\n\n";
    cp.PrintHelp();
    return -1;
  }

  worklog::CommandContext ctx;
  ctx.args = args;
  ctx.config = worklog::Config();

  auto cmd = parsing.ValueOrDie();
  return cmd(ctx);
}

int main(int argc, char** argv) {
  if (getenv("EDITOR") == nullptr) {
    std::cerr << "Error: Please set the environment variable "
              << "$EDITOR to your favourite editor (ie. vim). "
              << "(the value is currently empty!)\n";

    return -1;
  }
  
  std::vector<std::string> args(argv, argv + argc);
  return ParseAndExecute(args);
}
