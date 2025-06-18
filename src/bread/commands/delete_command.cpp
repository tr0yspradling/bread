#include <bread/cache_server.h>
#include <bread/commands/delete_command.h>

std::expected<std::string, std::string> bread::commands::DeleteCommand::execute(
    cache_server& server, sockpp::tcp_socket& client) {
  server.handle_delete(key_, client);
  return std::string("");
}