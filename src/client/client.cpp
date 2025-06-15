#include <client/client.h>
#include <sockpp/inet_address.h>
#include <sockpp/tcp_connector.h>

#include <iostream>
#include <sstream>

bool cache_client::connect(const std::string &host, in_port_t port) {
  sockpp::inet_address addr(host, port);
  conn_ = sockpp::tcp_connector(addr);
  return conn_.is_open();
}

bool cache_client::set(const std::string &key, const std::string &value) {
  std::ostringstream oss;
  oss << "set " << key << " 0 0 " << value.size() << "\r\n" << value << "\r\n";
  auto data = oss.str();
  auto n = conn_.write(data);
  if (n.value() != static_cast<ssize_t>(data.size())) {
    return false;
  }
  char buf[256];
  auto r = conn_.read(buf, sizeof(buf));
  if (r.value() <= 0) {
    return false;
  }
  std::string resp(buf, r.value());
  return resp.rfind("SET", 0) == 0;
}

std::optional<std::string> cache_client::get(const std::string &key) {
  std::string cmd = "get " + key + "\r\n";
  auto n = conn_.write(cmd);
  if (n.value() != static_cast<ssize_t>(cmd.size())) {
    return std::nullopt;
  }
  std::string response;
  char buf[256];
  while (true) {
    auto r = conn_.read(buf, sizeof(buf));
    if (r.value() <= 0) {
      return std::nullopt;
    }
    response.append(buf, r.value());
    if (response.ends_with("GET\r\n")) {
      break;
    }
  }
  if (!response.starts_with("VALUE")) {
    return std::nullopt;
  }
  auto first = response.find("\r\n");
  if (first == std::string::npos) {
    return std::nullopt;
  }
  auto second = response.find("\r\n", first + 2);
  if (second == std::string::npos) {
    return std::nullopt;
  }
  return response.substr(first + 2, second - (first + 2));
}

bool cache_client::delete_key(const std::string &key) {
  std::string cmd = "delete " + key + "\r\n";
  auto n = conn_.write(cmd);
  if (n.value() != static_cast<ssize_t>(cmd.size())) {
    return false;
  }
  char buf[256];
  auto r = conn_.read(buf, sizeof(buf));
  if (r.value() <= 0) {
    return false;
  }
  std::string resp(buf, r.value());
  return resp.rfind("DELETED", 0) == 0;
}

void cache_client::quit() {
  std::string cmd = "quit\r\n";
  conn_.write(cmd);
  conn_.close();
}
