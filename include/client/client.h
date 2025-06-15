#pragma once

#include <sockpp/tcp_connector.h>

#include <optional>
#include <string>

class cache_client {
  sockpp::tcp_connector conn_;

 public:
  cache_client() = default;

  bool connect(const std::string &host, in_port_t port);

  bool set(const std::string &key, const std::string &value);

  std::optional<std::string> get(const std::string &key);

  bool delete_key(const std::string &key);

  void quit();
};
