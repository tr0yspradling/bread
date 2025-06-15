#include <bread/cache_server.h>

#include <iostream>

#ifndef BREAD_VERSION
#  define BREAD_VERSION "dev"
#endif

int main() {
  try {
    // TODO: Set these from 1) program arguments 2) env vars 3) yaml
    std::string host = "127.0.0.1";
    in_port_t port = 2929;

    sockpp::initialize();

    cache_server cache_server{host, port};  // Default memcached port
    std::thread cache_server_thread(&cache_server::start, &cache_server);

    // Wait for user input to stop the cache_server
    std::cout << "Memcached clone cache_server " << BREAD_VERSION
              << " is running. Press Enter to stop.\n";
    std::cin.get();

    cache_server.stop();
    if (cache_server_thread.joinable()) {
      cache_server_thread.join();
    }

  } catch (const std::exception &ex) {
    std::cerr << "cache_server error: " << ex.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
