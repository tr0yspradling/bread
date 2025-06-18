#include <bread/commands/unknown_command.h>

std::expected<std::string, std::string>
bread::commands::UnknownCommand::execute(cache_server& /*server*/,
                                         sockpp::tcp_socket& client) {
  client.write("NO_CMD\r\n");
  return std::string("");
}