// SPDX-License-Identifier: MIT

#include "bulk.h"
#include "execution.h"
#include "sync_wait.h"
#include <gtest/gtest.h>

namespace execution {
namespace {

TEST(bulk, thread_pool) {
  std::vector<int> v(23, 42);
  for (int r : v) {
    EXPECT_EQ(42, r);
  }

  detail::thread_pool pool{8};
  auto s1 = schedule(pool) | []() { return 23; };
  auto s2 = bulk(s1, v.size(), [&v](const int i, const int m) { v[i] = m; });
  this_thread::sync_wait<int>(s2);

  for (int r : v) {
    EXPECT_EQ(23, r);
  }
}

TEST(bulk, inline_task) {
  std::vector<int> v(23, 42);
  for (int r : v) {
    EXPECT_EQ(42, r);
  }

  auto s1 = schedule() | []() { return 23; };
  auto s2 = bulk(s1, v.size(), [&v](const int i, const int m) { v[i] = m; });
  this_thread::sync_wait<int>(s2);

  for (int r : v) {
    EXPECT_EQ(23, r);
  }
}

} // namespace
} // namespace execution
