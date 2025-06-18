// memcached_clone.cpp

#include <bread/cache_server.h>
#include <bread/commands/command_factory.h>
#include <bread/storage.h>

void cache_server::start() {
  sockpp::inet_address addr(host_, port_);
  acceptor = sockpp::tcp_acceptor(addr);
  is_running.store(true, std::memory_order_relaxed);
  while (is_running.load(std::memory_order_relaxed)) {
    sockpp::inet_address peer;
    auto socket = acceptor.accept(&peer);
    if (socket) {
      sockpp::tcp_socket sock = socket.release();
      std::thread thr(&cache_server::handle_client, this, std::move(sock));
      worker_threads.push_back(std::move(thr));
    } else {
      std::cerr << "Error accepting connection.";
    }
  }
}

void cache_server::stop() {
  is_running.store(false, std::memory_order_relaxed);
  acceptor.close();
  for (auto &thr : worker_threads) {
    if (thr.joinable()) {
      thr.join();
    }
  }
}

void cache_server::handle_client(sockpp::tcp_socket &&client_sock) {
  while (is_running.load(std::memory_order_relaxed)) {
    auto command_expected = read_command(client_sock);
    if (!command_expected) {
      std::cerr << "Read error: " << command_expected.error() << '\n';
      break;
    }

    auto result = process_command(command_expected.value(), client_sock);
    if (!result) {
      if (result.error() == "Client requested to quit") {
        break;
      } else {
        std::cerr << "Processing error: " << result.error() << '\n';
      }
    }
  }
  client_sock.close();
}

std::expected<std::string, std::string> cache_server::read_command(
    sockpp::tcp_socket &client_sock) {
  std::string command;
  char buffer[1024];

  while (true) {
    auto bytes_read = client_sock.read(buffer, sizeof(buffer));
    if (bytes_read.value() > 0) {
      command.append(buffer, bytes_read.value());

      // Look for '\r\n' indicating end of command
      if (auto pos = command.find("\r\n"); pos != std::string::npos) {
        command.erase(pos);  // Remove '\r\n'
        return command;
      }
    } else if (bytes_read.value() == 0) {
      // Client disconnected
      return std::unexpected(std::string("Client disconnected"));
    } else {
      // Error occurred
      return std::unexpected(std::string("Error reading from socket"));
    }
  }
}

std::expected<std::string, std::string> cache_server::process_command(
    const std::string &command, sockpp::tcp_socket &client_sock) {
  auto cmd = bread::commands::parse_command(command);
  return cmd->execute(*this, client_sock);
}

std::expected<std::string, std::string> cache_server::read_data_block(
    sockpp::tcp_socket &client_sock, size_t total_bytes) {
  std::string data_block(total_bytes, '\0');
  size_t bytes_received = 0;

  while (bytes_received < total_bytes) {
    auto n = client_sock.read(&data_block[bytes_received],
                              total_bytes - bytes_received);

    if (n.value() > 0) {
      bytes_received += n.value();
    } else if (n.value() == 0) {
      return std::unexpected(std::string("Client disconnected"));
    } else {
      return std::unexpected(std::string("Error reading data block."));
    }
  }

  return data_block;
}

void cache_server::handle_set(const std::string &key, std::string value) {
  storage_->set(key, std::move(value));
}

void cache_server::handle_get(const std::string &key,
                              sockpp::tcp_socket &client_sock) {
  if (auto value = storage_->get(key)) {
    std::ostringstream response;
    response << "VALUE " << key << " 0 " << value->size() << "\r\n";
    response << *value << "\r\n";
    client_sock.write(response.str());
  }
}

void cache_server::handle_delete(const std::string &key,
                                 sockpp::tcp_socket &client_sock) {
  if (storage_->del(key)) {
    client_sock.write("DELETED\r\n");
  } else {
    client_sock.write("NOT_FOUND\r\n");
  }
}
