#pragma once

#include <bread/expected_compat.h>
#include <bread/storage.h>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_socket.h>

#include <atomic>
#include <charconv>  // For std::from_chars
#include <iostream>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

class cache_server {
  /*
   *
   */
  std::atomic_bool is_running{false};
  std::vector<std::thread> worker_threads;
  cache_storage storage_;
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
  void handle_client(sockpp::tcp_socket &&client_sock);

  [[nodiscard]] std::expected<std::string, std::string> read_data_block(
      sockpp::tcp_socket &client_sock, size_t total_bytes);

  [[nodiscard]] std::expected<std::string, std::string> read_command(
      sockpp::tcp_socket &client_sock);

  [[nodiscard]] std::expected<std::string, std::string> process_command(
      const std::string &command, sockpp::tcp_socket &client_sock);

  [[nodiscard]] in_port_t port() const { return acceptor.address().port(); }

 private:
  void handle_set(const std::string &key, std::string value);

  void handle_get(const std::string &key, sockpp::tcp_socket &client_sock);

  void handle_delete(const std::string &key, sockpp::tcp_socket &client_sock);

  [[nodiscard]] in_port_t port() const { return acceptor.address().port(); }
};
