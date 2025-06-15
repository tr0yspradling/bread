#pragma once

#include <atomic>
#include <charconv> // For std::from_chars
#include <expected>
#include <iostream>
#include <sstream>
#include <memory>
#include <mutex>
#include <ranges>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_socket.h>

class cache_server {
   /*
   *
   */
   std::atomic_bool is_running{false};
   std::shared_mutex datastore_mutex;
   std::vector<std::thread> worker_threads;
   std::unordered_map<std::string, std::string> datastore;
   sockpp::tcp_acceptor acceptor;

public:
   explicit cache_server(std::string &host, in_port_t port) : acceptor(port) {
      if (!acceptor) {
         throw std::runtime_error("Failed to create acceptor.");
      }
   }

   ~cache_server() { stop(); }

   void start();

   void stop();

   // for some reason we need && here, otherwise the call to std::thread in
   // cache_server::start will break.
   void handle_client(sockpp::tcp_socket&& client_sock);

   [[nodiscard]] std::expected<std::string, std::string>
   read_data_block(sockpp::tcp_socket &client_sock, size_t total_bytes);

   [[nodiscard]] std::expected<std::string, std::string>
   read_command(sockpp::tcp_socket &client_sock);

   [[nodiscard]] std::expected<std::string, std::string>
   process_command(const std::string &command, sockpp::tcp_socket &client_sock);

   void handle_set(const std::string &key, std::string value);

   void handle_get(const std::string &key, sockpp::tcp_socket &client_sock);

   void handle_delete(const std::string &key, sockpp::tcp_socket &client_sock);
};
