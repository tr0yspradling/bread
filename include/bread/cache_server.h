#pragma once

#include <bread/expected_compat.h>
#include <bread/storage.h>
#include <bread/storage_engine.h>
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
  std::unique_ptr<bread::storage_engine> storage_;
  sockpp::tcp_acceptor acceptor;

 public:
  /// @param storage Optional custom storage engine; defaults to in-memory
  /// cache_storage.
  explicit cache_server(std::string host, in_port_t port,
                        std::unique_ptr<bread::storage_engine> storage =
                            std::make_unique<bread::cache_storage>())
      : host_(std::move(host)), port_(port), storage_(std::move(storage)) {}

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

  [[nodiscard]] in_port_t port() const { return port_; }

 public:
  void handle_set(const std::string &key, std::string value);

  void handle_get(const std::string &key, sockpp::tcp_socket &client_sock);

  /// Retrieves the stored value for a key (no network I/O).
  [[nodiscard]] std::optional<std::string> get_value(
      const std::string &key) const {
    return storage_->get(key);
  }

  void handle_delete(const std::string &key, sockpp::tcp_socket &client_sock);

 private:
  std::string host_;
  in_port_t port_;
};
