#include <bread/cache_server.h>
#include <gtest/gtest.h>
#include <sockpp/error.h>
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
    sockpp::error_code ec1;
    sockpp::inet_address listen_addr(host, 0, ec1);
    ASSERT_FALSE(ec1);
    acceptor = sockpp::tcp_acceptor(listen_addr,
                                    sockpp::tcp_acceptor::DFLT_QUE_SIZE, ec1);
    ASSERT_FALSE(ec1);
    sockpp::error_code ec;
    sockpp::inet_address addr(host, acceptor.address().port(), ec);
    ASSERT_FALSE(ec);
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

TEST_F(CacheServerFixture, ProcessAdd) {
  const std::string data = "hello\r\n";
  client_sock.write(data);
  auto res = server.process_command("add key 0 0 5", server_sock);
  ASSERT_TRUE(res);
  char buf[64];
  auto n = client_sock.read(buf, sizeof(buf));
  std::string response(buf, buf + n.value());
  EXPECT_EQ(response, "STORED\r\n");
}

TEST_F(CacheServerFixture, ProcessReplace) {
  // replace should fail if key missing
  auto res1 = server.process_command("replace missing 0 0 5", server_sock);
  ASSERT_TRUE(res1);
  char buf[64];
  auto n1 = client_sock.read(buf, sizeof(buf));
  std::string resp1(buf, buf + n1.value());
  EXPECT_EQ(resp1, "NOT_STORED\r\n");

  // set then replace
  const std::string init = "world\r\n";
  client_sock.write(init);
  server.process_command("set key 0 0 5", server_sock);
  // consume SET response
  auto n_set = client_sock.read(buf, sizeof(buf));
  (void)n_set;
  const std::string repl = "abcde\r\n";
  client_sock.write(repl);
  auto res2 = server.process_command("replace key 0 0 5", server_sock);
  ASSERT_TRUE(res2);
  auto n2 = client_sock.read(buf, sizeof(buf));
  std::string resp2(buf, buf + n2.value());
  EXPECT_EQ(resp2, "STORED\r\n");
}

TEST_F(CacheServerFixture, ProcessAppendPrepend) {
  char buf[128];
  // append/prepend should fail if missing
  auto ra = server.process_command("append m 0 0 3", server_sock);
  ASSERT_TRUE(ra);
  auto na = client_sock.read(buf, sizeof(buf));
  EXPECT_EQ(std::string(buf, buf + na.value()), "NOT_STORED\r\n");
  auto rp = server.process_command("prepend m 0 0 3", server_sock);
  ASSERT_TRUE(rp);
  auto np = client_sock.read(buf, sizeof(buf));
  EXPECT_EQ(std::string(buf, buf + np.value()), "NOT_STORED\r\n");

  // set base value
  const std::string base = "mid\r\n";
  client_sock.write(base);
  server.process_command("set k 0 0 3", server_sock);
  // consume SET response
  auto n_set2 = client_sock.read(buf, sizeof(buf));
  (void)n_set2;
  // append
  const std::string tail = "end\r\n";
  client_sock.write(tail);
  auto ra2 = server.process_command("append k 0 0 3", server_sock);
  ASSERT_TRUE(ra2);
  auto n3 = client_sock.read(buf, sizeof(buf));
  EXPECT_EQ(std::string(buf, buf + n3.value()), "STORED\r\n");
  // prepend
  const std::string head = "sta\r\n";
  client_sock.write(head);
  auto rp2 = server.process_command("prepend k 0 0 3", server_sock);
  ASSERT_TRUE(rp2);
  auto n4 = client_sock.read(buf, sizeof(buf));
  EXPECT_EQ(std::string(buf, buf + n4.value()), "STORED\r\n");
}
