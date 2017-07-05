#ifndef ATL_FILE_H_
#define ATL_FILE_H_

#include <fstream>
#include <functional>
#include <string>

#include "file.h"
#include "optional.h"
#include "status.h"
#include "statusor.h"

namespace atl {
void WalkDir(const std::string& path,
             std::function<bool(const std::string&)> callback);
std::string TempName();
std::string TempFileName();

class TempFile {
 public:
	TempFile() : path_(TempFileName()), stream_(path_) {}
	explicit TempFile(const std::string& path) : path_(path), stream_(path) {}

	operator bool() const {
		return stream_.good();
	}
	std::ofstream& stream();
	const std::string& path() const;
	~TempFile();

	private:
		const std::string path_;
		std::ofstream stream_;
};

bool MkDir(const std::string& path);
bool Rename(const std::string& old_path, const std::string& new_path);
bool Remove(const std::string& path);
bool FileExists(const std::string& filename);
bool FileWriteContent(const std::string& filename, const std::string& content);
atl::Optional<std::string> FileReadContent(const std::string& filename);
}  // namespace atl

#endif  // ATL_FILE_H_
