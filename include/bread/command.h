#pragma once

#include <bread/expected_compat.h>
#include <sockpp/tcp_socket.h>

#include <string>

class cache_server;

namespace bread {
namespace commands {

class Command {
 public:
  virtual ~Command() = default;
  virtual std::expected<std::string, std::string> execute(
      ::cache_server& server, sockpp::tcp_socket& client) = 0;
};

}  // namespace commands
}  // namespace bread