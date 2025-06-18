#include <bread/cache_server.h>
#include <bread/commands/get_command.h>

std::expected<std::string, std::string> bread::commands::GetCommand::execute(
    ::cache_server& server, sockpp::tcp_socket& client) {
  std::ostringstream response;
  for (const auto& key : keys_) {
    if (auto value = server.get_value(key)) {
      response << "VALUE " << key << " 0 " << value->size() << "\r\n";
      response << *value << "\r\n";
    }
  }
  response << "GET\r\n";
  client.write(response.str());
  return std::string("");
}