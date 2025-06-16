#include <bread/cache_server.h>
#include <gtest/gtest.h>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_connector.h>
#include <sockpp/tcp_socket.h>

#include <chrono>
#include <thread>

class CacheServerFixture : public ::testing::Test {
 protected:
  std::string host{"127.0.0.1"};
  cache_server server{host, 0};
  sockpp::tcp_acceptor acceptor;
  sockpp::tcp_socket server_sock;
  sockpp::tcp_socket client_sock;

  static void SetUpTestSuite() { sockpp::initialize(); }

  void SetUp() override {
    acceptor = sockpp::tcp_acceptor(0);
    ASSERT_TRUE(acceptor);
    sockpp::inet_address addr(host, acceptor.address().port());
    sockpp::tcp_connector conn;
    ASSERT_TRUE(conn.connect(addr));
    client_sock = sockpp::tcp_socket(conn.release());
    auto res = acceptor.accept();
    ASSERT_TRUE(res);
    server_sock = std::move(res.release());
  }

  void TearDown() override {
    acceptor.close();
    server_sock.close();
    client_sock.close();
  }
};

TEST_F(CacheServerFixture, ReadCommand) {
  const std::string cmd = "get foo\r\n";
  client_sock.write(cmd);
  auto result = server.read_command(server_sock);
  ASSERT_TRUE(result);
  EXPECT_EQ(result.value(), "get foo");
}

TEST_F(CacheServerFixture, ReadDataBlock) {
  const std::string data = "hello\r\n";
  client_sock.write(data.substr(0, 2));
  client_sock.write(data.substr(2));
  auto result = server.read_data_block(server_sock, data.size());
  ASSERT_TRUE(result);
  EXPECT_EQ(result.value(), data);
}

TEST_F(CacheServerFixture, ProcessSetGetDelete) {
  // Send data block for set command
  const std::string data = "world\r\n";
  client_sock.write(data);
  auto res_set = server.process_command("set key 0 0 5", server_sock);
  ASSERT_TRUE(res_set);
  char buf[64];
  auto n = client_sock.read(buf, sizeof(buf));
  std::string response(buf, buf + n.value());
  EXPECT_EQ(response, "SET\r\n");

  // get existing key
  auto res_get = server.process_command("get key", server_sock);
  ASSERT_TRUE(res_get);
  n = client_sock.read(buf, sizeof(buf));
  response.assign(buf, buf + n.value());
  EXPECT_EQ(response, "VALUE key 0 5\r\nworld\r\nGET\r\n");

  // delete key
  auto res_del = server.process_command("delete key", server_sock);
  ASSERT_TRUE(res_del);
  n = client_sock.read(buf, sizeof(buf));
  response.assign(buf, buf + n.value());
  EXPECT_EQ(response, "DELETED\r\n");

  // get after delete
  auto res_get2 = server.process_command("get key", server_sock);
  ASSERT_TRUE(res_get2);
  n = client_sock.read(buf, sizeof(buf));
  response.assign(buf, buf + n.value());
  EXPECT_EQ(response, "GET\r\n");

  // quit
  auto res_quit = server.process_command("quit", server_sock);
  EXPECT_FALSE(res_quit.has_value());
  EXPECT_EQ(res_quit.error(), "Client requested to quit");
}
