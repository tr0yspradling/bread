// memcached_clone.cpp

#include "cache_server.h"

void cache_server::start() {
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
}

void cache_server::handle_client(sockpp::tcp_socket &&client_sock) {
   while (is_running.load(std::memory_order_relaxed)) {
      std::cout << "FOOBAR\n";
      auto command_expected = read_command(client_sock);
      if (!command_expected) {
         // Log the error and break
         std::cerr << "Read error: " << command_expected.error() << '\n';
         break;
      }

//      auto result = process_command(command_expected.value(), client_sock);
//      if (!result) {
//         // Handle special cases like client quitting
//         if (result.error() == "Client requested to quit") {
//            break;
//         } else {
//            std::cerr << "Processing error: " << result.error() << '\n';
//         }
//      }
   }
   client_sock.close();
}

std::expected<std::string, std::string>
cache_server::read_command(sockpp::tcp_socket &client_sock) {
   std::string command;
   char buffer[1024];

   while (true) {
      auto bytes_read = client_sock.read(buffer, sizeof(buffer));
      if (bytes_read.value() > 0) {
         command.append(buffer, bytes_read.value());

         // Look for '\r\n' indicating end of command
         if (auto pos = command.find("\r\n"); pos != std::string::npos) {
            command.erase(pos); // Remove '\r\n'
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

std::expected<std::string, std::string>
cache_server::process_command(const std::string &command, sockpp::tcp_socket &client_sock) {
   std::istringstream iss(command);
   std::string cmd;
   iss >> cmd;
   if (cmd == "set") {
      std::string key;
      int flags = 0, exptime = 0, bytes = 0;

      iss >> key >> flags >> exptime >> bytes;

      if (iss.fail() || key.empty()) {
         client_sock.write("CLIENT_ERROR bad command line format\r\n");
         return std::unexpected(std::string("CLIENT_ERROR bad command line format"));
      }

      // Read data block
      auto data_block_expected = read_data_block(client_sock, bytes + 2); // +2 for '\r\n'
      if (!data_block_expected.has_value()) {
         client_sock.write("CLIENT_ERROR " + data_block_expected.error() + "\r\n");
         return std::unexpected(std::string("CLIENT_ERROR"));
      }

      try {
         auto data_block = data_block_expected.value();
         // Verify data ends with '\r\n'
         if (!data_block.ends_with("\r\n")) {
            client_sock.write("CLIENT_ERROR bad data termination\r\n");
            return std::unexpected(std::string("CLIENT_ERROR bad data termination"));
         }
         // Remove trailing '\r\n'
         data_block.resize(bytes);
         handle_set(std::move(key), std::move(data_block));
         client_sock.write("SET\r\n");
      } catch (std::bad_expected_access<std::string> &ex) {
         return std::unexpected(ex.what());
      }
   } else if (cmd == "get") {
      std::vector<std::string> keys{std::istream_iterator<std::string>{iss}, {}};
      if (keys.empty()) {
         client_sock.write("CLIENT_ERROR bad command line format\r\n");
         return std::string("");
      }
      for (const auto &key: keys) {
         handle_get(key, client_sock);
      }
      client_sock.write("GET\r\n");
   } else if (cmd == "delete") {
      std::string key;
      iss >> key;
      if (iss.fail() || key.empty()) {
         client_sock.write("CLIENT_ERROR bad command line format\r\n");
         return std::unexpected(std::string("CLIENT_ERROR"));
      }
      handle_delete(key, client_sock);
   } else if (cmd == "quit") {
      return std::unexpected(std::string("Client requested to quit"));
   } else {
      client_sock.write("NO_CMD\r\n");
   }
   return std::string("");
}

std::expected<std::string, std::string>
cache_server::read_data_block(sockpp::tcp_socket &client_sock, size_t total_bytes) {
   std::string data_block(total_bytes, '\0');
   size_t bytes_received = 0;

   while (bytes_received < total_bytes) {
      auto n = client_sock.read(&data_block[bytes_received], total_bytes - bytes_received);

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

void cache_server::handle_set(const std::string key, std::string value) {
   std::unique_lock lock(datastore_mutex);
   datastore[std::move(key)] = std::move(value);
}

void cache_server::handle_get(const std::string key, sockpp::tcp_socket &client_sock) {
   std::shared_lock lock(datastore_mutex);
   if (auto it = datastore.find(key); it != datastore.end()) {
      std::ostringstream response;
      response << "VALUE " << key << " 0 " << it->second.size() << "\r\n"
               << it->second << "\r\n";
      client_sock.write(response.str());
   }
}

void cache_server::handle_delete(const std::string key, sockpp::tcp_socket &client_sock) {
   std::unique_lock lock(datastore_mutex);
   if (auto erased = datastore.erase(std::string(key)); erased > 0) {
      client_sock.write("DELETED\r\n");
   } else {
      client_sock.write("NOT_FOUND\r\n");
   }
}
