#include <bread/cache_client.h>
#include <bread/cache_server.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>
#include <iostream>

class ClientIntegration : public ::testing::Test {
 protected:
  std::string host{"127.0.0.1"};
  in_port_t port{0};
  cache_server server{host, port};
  std::thread srv_thread;

  static void SetUpTestSuite() { sockpp::initialize(); }

  void SetUp() override {
    srv_thread = std::thread(&cache_server::start, &server);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    port = server.port();
    std::cout << "server on port " << port << "\n";
  }

  void TearDown() override {
    server.stop();
    if (srv_thread.joinable()) srv_thread.join();
  }
};

TEST_F(ClientIntegration, SetGetDelete) {
  cache_client client;
  ASSERT_TRUE(client.connect(host, port));
  std::cout << "connected\n";

  ASSERT_TRUE(client.set("key", "value"));
  std::cout << "set done\n";
  auto val = client.get("key");
  std::cout << "get returned\n";
  ASSERT_TRUE(val.has_value());
  EXPECT_EQ(val.value(), "value");

  ASSERT_TRUE(client.del("key"));
  std::cout << "delete done\n";
  auto none = client.get("key");
  ASSERT_TRUE(none.has_value());
  EXPECT_TRUE(none->empty());

  client.close();
}
