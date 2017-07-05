#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <string>

#include "worklog.h"

namespace worklog {
class HumanSerializer {
 public:
  // Serializes the Log in human text form
  // (which is similar like a git commit message)
  std::string Serialize(const Log& log);

  // Unserializes the Log in human text form
  // FIXME(an): Reimplement into a state machine
  //            because this will fail badly if there is no tag and the
  //            description or subject contains a '='.
  Log Unserialize(const std::string& text);

 private:
  bool IsTag(const std::string& text) {
    return text.find("=") != std::string::npos;
  }
};
} // namespace worklog
#endif  // SERIALIZER_H_
