#include <bread/storage.h>
#include <bread/storage_expiration.h>
#include <gtest/gtest.h>

#include <chrono>
#include <thread>

TEST(ExpirationStorageTest, KeyExpiresAfterTTL) {
  bread::expiration_engine engine(std::make_unique<bread::cache_storage>());
  engine.set("foo", "bar", 1);
  auto v1 = engine.get("foo");
  ASSERT_TRUE(v1.has_value());
  EXPECT_EQ(*v1, "bar");
  std::this_thread::sleep_for(std::chrono::seconds(2));
  auto v2 = engine.get("foo");
  EXPECT_FALSE(v2.has_value());
}

TEST(ExpirationStorageTest, KeyDoesNotExpireWithZeroTTL) {
  bread::expiration_engine engine(std::make_unique<bread::cache_storage>());
  engine.set("foo", "baz", 0);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto v = engine.get("foo");
  ASSERT_TRUE(v.has_value());
  EXPECT_EQ(*v, "baz");
}