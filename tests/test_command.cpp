//
// Created by Troy Spradling on 10/26/24.
//

#include <gtest/gtest.h>
// #include "cache_server.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}