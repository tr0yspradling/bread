#pragma once

#include <bread/command.h>

#include <string>

namespace bread {
namespace commands {

class BadCommand : public Command {
 public:
  explicit BadCommand(std::string msg) : msg_(std::move(msg)) {}
  ~BadCommand() override;
  std::expected<std::string, std::string> execute(
      ::cache_server& server, sockpp::tcp_socket& client) override;

 private:
  std::string msg_;
};

}  // namespace commands
}  // namespace bread