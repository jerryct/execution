// SPDX-License-Identifier: MIT

#include "execution.h"
#include "sync_wait.h"
#include <gtest/gtest.h>

namespace execution {
namespace {

TEST(then, run_in_same_thread) {
  auto s = then(schedule(), []() { return std::this_thread::get_id(); });
  EXPECT_EQ(std::this_thread::get_id(), this_thread::sync_wait<std::thread::id>(s));
}

TEST(then, run_in_separate_thread) {
  detail::thread_pool pool{1};
  auto s = then(schedule(pool), []() { return std::this_thread::get_id(); });
  EXPECT_NE(std::this_thread::get_id(), this_thread::sync_wait<std::thread::id>(s));
}

TEST(then, continuation_with_named_temporaries) {
  detail::thread_pool pool{1};
  auto s1 = then(schedule(pool), []() { return 42; });
  auto s2 = then(s1, [](int v) { return v + 1; });
  auto s3 = then(s2, [](int v) { return v + 1; });
  EXPECT_EQ(44, this_thread::sync_wait<int>(s3));
}

TEST(then, continuation_with_nested_function_calls) {
  detail::thread_pool pool{1};
  auto s =
      then(then(then(schedule(pool), []() { return 42; }), [](int v) { return v + 1; }), [](int v) { return v + 1; });
  EXPECT_EQ(44, this_thread::sync_wait<int>(s));
}

TEST(pipe, pipe) {
  detail::thread_pool pool{1};
  auto s = schedule(pool) | []() { return 42; } | [](int v) { return v + 1; } | [](int v) { return v + 1; };
  EXPECT_EQ(44, this_thread::sync_wait<int>(s));
}

TEST(pipe, non_copyble) {
  struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;
    NonCopyable(NonCopyable &&) = default;
    NonCopyable &operator=(NonCopyable &&) = default;
    int v{42};
  };

  detail::thread_pool pool{1};
  auto s = schedule(pool) | []() -> NonCopyable { return {}; } | [](NonCopyable v) { return v; };
  EXPECT_EQ(42, this_thread::sync_wait<NonCopyable>(s).v);
}

TEST(just, just) {
  detail::thread_pool pool{1};
  auto s = schedule(pool) | just(42) | [](int v) { return v + 1; } | [](int v) { return v + 1; };
  EXPECT_EQ(44, this_thread::sync_wait<int>(s));
}

TEST(new_thread, new_thread) {
  detail::thread_pool pool{1};
  auto s = schedule(pool);
  this_thread::sync_wait<void>(s);
}

TEST(then, void_return) {
  int v = 23;
  detail::thread_pool pool{1};
  auto s = schedule(pool) | [&v]() { v = 42; };
  this_thread::sync_wait<void>(s);
  EXPECT_EQ(42, v);
}

TEST(then, multiple_invocations) {
  int v = 0;
  detail::thread_pool pool{1};
  auto s = schedule(pool) | [&v]() { v += 1; };
  this_thread::sync_wait<void>(s);
  EXPECT_EQ(1, v);
  this_thread::sync_wait<void>(s);
  EXPECT_EQ(2, v);
  this_thread::sync_wait<void>(s);
  EXPECT_EQ(3, v);
}

} // namespace
} // namespace execution