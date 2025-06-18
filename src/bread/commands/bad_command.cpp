#include <bread/commands/bad_command.h>

bread::commands::BadCommand::~BadCommand() = default;

std::expected<std::string, std::string> bread::commands::BadCommand::execute(
    cache_server& /*server*/, sockpp::tcp_socket& client) {
  client.write(msg_ + "\r\n");
  return std::unexpected(msg_);
}