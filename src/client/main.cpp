#include <client/client.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: cache_client <host> <port> <command> [args...]\n";
    return 1;
  }

  std::string host = argv[1];
  in_port_t port = static_cast<in_port_t>(std::stoi(argv[2]));

  sockpp::initialize();

  cache_client client;
  if (!client.connect(host, port)) {
    std::cerr << "Failed to connect" << std::endl;
    return 1;
  }

  std::string cmd = argv[3];
  if (cmd == "set") {
    if (argc < 6) {
      std::cerr << "Usage: cache_client <host> <port> set <key> <value>\n";
      return 1;
    }
    std::string key = argv[4];
    std::string value = argv[5];
    if (!client.set(key, value)) {
      std::cerr << "SET failed\n";
      return 1;
    }
  } else if (cmd == "get") {
    if (argc < 5) {
      std::cerr << "Usage: cache_client <host> <port> get <key>\n";
      return 1;
    }
    std::string key = argv[4];
    auto val = client.get(key);
    if (val) {
      std::cout << *val << std::endl;
    } else {
      std::cout << "NOT_FOUND" << std::endl;
    }
  } else if (cmd == "delete") {
    if (argc < 5) {
      std::cerr << "Usage: cache_client <host> <port> delete <key>\n";
      return 1;
    }
    std::string key = argv[4];
    if (!client.delete_key(key)) {
      std::cerr << "Delete failed" << std::endl;
      return 1;
    }
  } else if (cmd == "quit") {
    client.quit();
  } else {
    std::cerr << "Unknown command\n";
    return 1;
  }

  return 0;
}
