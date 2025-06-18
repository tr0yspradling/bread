#pragma once

#include <bread/command.h>

namespace bread {
namespace commands {

class QuitCommand : public Command {
 public:
  std::expected<std::string, std::string> execute(
      ::cache_server& server, sockpp::tcp_socket& client) override;
};

}  // namespace commands
}  // namespace bread