// SPDX-License-Identifier: MIT

#include "bulk.h"
#include "inline_task.h"
#include "sync_wait.h"
#include "then.h"
#include "thread_pool.h"
#include <gtest/gtest.h>

namespace execution {
namespace {

TEST(bulk, continuation_with_named_temporaries) {
  std::vector<int> v(23, 42);
  for (int r : v) {
    EXPECT_EQ(42, r);
  }

  detail::thread_pool pool{8};
  auto s1 = then(schedule(pool), []() { return 23; });
  auto s2 = bulk(s1, v.size(), [&v](const int i, const int m) { v[i] = m; });
  this_thread::sync_wait<int>(s2);

  for (int r : v) {
    EXPECT_EQ(23, r);
  }
}

TEST(bulk, continuation_with_nested_function_calls) {
  std::vector<int> v(23, 42);
  for (int r : v) {
    EXPECT_EQ(42, r);
  }

  detail::thread_pool pool{8};
  auto s = bulk(then(schedule(pool), []() { return 23; }), v.size(), [&v](const int i, const int m) { v[i] = m; });
  this_thread::sync_wait<int>(s);

  for (int r : v) {
    EXPECT_EQ(23, r);
  }
}

TEST(bulk, continuation_with_pipe) {
  std::vector<int> v(23, 42);
  for (int r : v) {
    EXPECT_EQ(42, r);
  }

  detail::thread_pool pool{8};
  auto s = schedule(pool) | then([]() { return 23; }) | bulk(v.size(), [&v](const int i, const int m) { v[i] = m; });
  this_thread::sync_wait<int>(s);

  for (int r : v) {
    EXPECT_EQ(23, r);
  }
}

TEST(bulk, inline_task) {
  std::vector<int> v(23, 42);
  for (int r : v) {
    EXPECT_EQ(42, r);
  }

  auto s1 = schedule() | then([]() { return 23; });
  auto s2 = bulk(s1, v.size(), [&v](const int i, const int m) { v[i] = m; });
  this_thread::sync_wait<int>(s2);

  for (int r : v) {
    EXPECT_EQ(23, r);
  }
}

} // namespace
} // namespace execution
