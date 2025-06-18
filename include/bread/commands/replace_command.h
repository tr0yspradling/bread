#pragma once

#include <bread/command.h>

#include <string>

namespace bread {
namespace commands {

/// "replace" stores data only if the key already exists.
class ReplaceCommand : public Command {
 public:
  ReplaceCommand(std::string key, int flags, int exptime, size_t bytes)
      : key_(std::move(key)), flags_(flags), exptime_(exptime), bytes_(bytes) {}
  std::expected<std::string, std::string> execute(
      ::cache_server& server, sockpp::tcp_socket& client) override;

 private:
  std::string key_;
  int flags_;
  int exptime_;
  size_t bytes_;
};

}  // namespace commands
}  // namespace bread