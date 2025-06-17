#pragma once

#include <sockpp/tcp_connector.h>
#include <sockpp/tcp_socket.h>

#include <optional>
#include <string>

class cache_client {
  sockpp::tcp_connector conn_;

 public:
  cache_client() = default;
  cache_client(const std::string& host, int port) { connect(host, port); }

  bool connect(const std::string& host, int port) {
    sockpp::inet_address addr(host, port);
    conn_ = sockpp::tcp_connector(addr);
    return bool(conn_);
  }

  bool is_connected() const { return bool(conn_); }
  void close() { conn_.close(); }

  bool set(const std::string& key, const std::string& value);
  std::optional<std::string> get(const std::string& key);
  bool del(const std::string& key);
};
