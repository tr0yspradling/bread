#pragma once

#include <bread/command.h>

#include <string>

namespace bread {
namespace commands {

class DeleteCommand : public Command {
 public:
  explicit DeleteCommand(std::string key) : key_(std::move(key)) {}
  std::expected<std::string, std::string> execute(
      ::cache_server& server, sockpp::tcp_socket& client) override;

 private:
  std::string key_;
};

}  // namespace commands
}  // namespace bread