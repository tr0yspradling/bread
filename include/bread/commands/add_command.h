#pragma once

#include <bread/command.h>

#include <string>

namespace bread {
namespace commands {

/// "add" stores data only if the key does not already exist.
class AddCommand : public Command {
 public:
  AddCommand(std::string key, int flags, int exptime, size_t bytes)
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