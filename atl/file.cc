#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

// Using boost filesystem because it will be soon in C++17 and then
// it's possible to remove the boost dependency.
#include <boost/filesystem.hpp>

#include "optional.h"
#include "file.h"

// File contains simple and quick file read & write helpers

namespace atl {

void WalkDir(const std::string& path, std::function<bool(const std::string&)> callback) {
  if (path.empty()) return;

  namespace fs = boost::filesystem;
  fs::path dir_path(path);
  fs::recursive_directory_iterator end;

  for (fs::recursive_directory_iterator i(dir_path); i != end; ++i) {
    const fs::path& current_file = *i;
    if (!callback(current_file.string())) {
      return;
    }
  }
  return;
}

std::string TempName() {
  boost::filesystem::path temp = boost::filesystem::unique_path();
  return temp.native();
}

std::string TempFileName() {
  boost::filesystem::path temp = boost::filesystem::unique_path();
  return boost::filesystem::temp_directory_path().native() + "/" + temp.native();
}

std::ofstream& TempFile::stream() {
	return stream_;
}

const std::string& TempFile::path() const {
	return path_;
}

TempFile::~TempFile() {
	atl::Remove(path_);
}

bool MkDir(const std::string& path) {
  return boost::filesystem::create_directory(path);
}

// Wrapper for: http://www.boost.org/doc/libs/1_55_0/libs/filesystem/doc/reference.html#rename
bool Rename(const std::string& old_path, const std::string& new_path) {
  boost::filesystem::rename(old_path, new_path);
  return true;
}

bool Remove(const std::string& path) { return boost::filesystem::remove(path); }

bool FileExists(const std::string& filename) {
  boost::filesystem::path file(filename);
  return boost::filesystem::exists(file);
}

bool FileWriteContent(const std::string& filename, const std::string& content) {
  std::ofstream file(filename);

  if (!file) {
    return false;
  }

  file << content;

  // file.close() is not needed because the destructor takes care of it.
  return true;
}

atl::Optional<std::string> FileReadContent(const std::string& filename) {
  std::ifstream file(filename);

  if (!file.is_open()) {
    return atl::Optional<std::string>();
  }

  std::string content;

  content.assign(std::istreambuf_iterator<char>(file),
                 std::istreambuf_iterator<char>());

  // file.close() is not needed because the destructor takes care of it.
  return std::move(content);
}

}  // namespace atl
