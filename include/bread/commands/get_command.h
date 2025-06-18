#pragma once

#include <bread/command.h>

#include <string>
#include <vector>

namespace bread {
namespace commands {

class GetCommand : public Command {
 public:
  explicit GetCommand(std::vector<std::string> keys) : keys_(std::move(keys)) {}
  std::expected<std::string, std::string> execute(
      ::cache_server& server, sockpp::tcp_socket& client) override;

 private:
  std::vector<std::string> keys_;
};

}  // namespace commands
}  // namespace bread