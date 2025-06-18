#include <bread/commands/quit_command.h>

std::expected<std::string, std::string> bread::commands::QuitCommand::execute(
    cache_server& /*server*/, sockpp::tcp_socket& /*client*/) {
  return std::unexpected(std::string("Client requested to quit"));
}