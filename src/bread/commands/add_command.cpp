#include <bread/cache_server.h>
#include <bread/commands/add_command.h>

std::expected<std::string, std::string> bread::commands::AddCommand::execute(
    cache_server& server, sockpp::tcp_socket& client) {
  if (server.get_value(key_)) {
    client.write("NOT_STORED\r\n");
    return std::string("");
  }
  auto data_block_expected = server.read_data_block(client, bytes_ + 2);
  if (!data_block_expected) {
    client.write("CLIENT_ERROR " + data_block_expected.error() + "\r\n");
    return std::unexpected(std::string("CLIENT_ERROR"));
  }
  auto data_block = data_block_expected.value();
  if (!data_block.ends_with("\r\n")) {
    client.write("CLIENT_ERROR bad data termination\r\n");
    return std::unexpected(std::string("CLIENT_ERROR bad data termination"));
  }
  data_block.resize(bytes_);
  server.handle_set(key_, std::move(data_block));
  client.write("STORED\r\n");
  return std::string("");
}