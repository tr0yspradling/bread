#include <bread/cache_client.h>
#include <sockpp/stream_socket.h>
#include <sockpp/tcp_connector.h>

#include <string>
#include <string_view>

bool cache_client::set(const std::string& key, const std::string& value) {
  if (!conn_) return false;
  std::string cmd = "set " + key + " 0 0 " + std::to_string(value.size()) +
                    "\r\n" + value + "\r\n";
  auto n = conn_.write_n(cmd.data(), cmd.size());
  if (!n || n.value() != (ssize_t)cmd.size()) return false;
  char buf[64];
  auto r = conn_.read(buf, sizeof(buf));
  if (!r) return false;
  std::string resp(buf, r.value());
  return resp == "SET\r\n";
}

std::optional<std::string> cache_client::get(const std::string& key) {
  if (!conn_) return std::nullopt;
  std::string cmd = "get " + key + "\r\n";
  auto n = conn_.write_n(cmd.data(), cmd.size());
  if (!n || n.value() != (ssize_t)cmd.size()) return std::nullopt;
  std::string resp;
  char buf[256];
  while (true) {
    auto r = conn_.read(buf, sizeof(buf));
    if (!r || r.value() <= 0) return std::nullopt;
    resp.append(buf, r.value());
    if (resp.size() >= 5 && resp.ends_with("GET\r\n")) break;
  }
  std::string_view view(resp);
  if (view.rfind("VALUE", 0) == 0) {
    auto pos1 = view.find("\r\n");
    auto pos2 = view.rfind("GET\r\n");
    if (pos1 != std::string_view::npos && pos2 != std::string_view::npos &&
        pos2 > pos1 + 2) {
      return std::string(view.substr(pos1 + 2, pos2 - (pos1 + 2)));
    }
  }
  return std::string();
}

bool cache_client::del(const std::string& key) {
  if (!conn_) return false;
  std::string cmd = "delete " + key + "\r\n";
  auto n = conn_.write_n(cmd.data(), cmd.size());
  if (!n || n.value() != (ssize_t)cmd.size()) return false;
  char buf[64];
  auto r = conn_.read(buf, sizeof(buf));
  if (!r) return false;
  std::string resp(buf, r.value());
  return resp == "DELETED\r\n";
}
