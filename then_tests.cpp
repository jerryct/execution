// SPDX-License-Identifier: MIT

#include "inline_task.h"
#include "sync_wait.h"
#include "then.h"
#include "thread_pool.h"
#include <gtest/gtest.h>
#include <thread>

namespace execution {
namespace {

TEST(then, run_in_same_thread) {
  auto s = then(schedule(), []() { return std::this_thread::get_id(); });
  EXPECT_EQ(std::this_thread::get_id(), this_thread::sync_wait<std::thread::id>(s));
}

TEST(then, run_in_separate_thread) {
  thread_pool pool{1};
  auto s = then(schedule(pool), []() { return std::this_thread::get_id(); });
  EXPECT_NE(std::this_thread::get_id(), this_thread::sync_wait<std::thread::id>(s));
}

TEST(then, continuation_with_named_temporaries) {
  thread_pool pool{1};
  auto s1 = then(schedule(pool), []() { return 42; });
  auto s2 = then(s1, [](int v) { return v + 1; });
  auto s3 = then(s2, [](int v) { return v + 1; });
  EXPECT_EQ(44, this_thread::sync_wait<int>(s3));
}

TEST(then, continuation_with_nested_function_calls) {
  thread_pool pool{1};
  auto s =
      then(then(then(schedule(pool), []() { return 42; }), [](int v) { return v + 1; }), [](int v) { return v + 1; });
  EXPECT_EQ(44, this_thread::sync_wait<int>(s));
}

TEST(pipe, pipe) {
  thread_pool pool{1};
  auto s = schedule(pool) | then([]() { return 42; }) | then([](int v) { return v + 1; }) |
           then([](int v) { return v + 1; });
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

  thread_pool pool{1};
  auto s = schedule(pool) | then([]() -> NonCopyable { return {}; }) | then([](NonCopyable v) { return v; });
  EXPECT_EQ(42, this_thread::sync_wait<NonCopyable>(s).v);
}

TEST(new_thread, new_thread) {
  thread_pool pool{1};
  auto s = schedule(pool);
  this_thread::sync_wait<void>(s);
}

TEST(then, void_return) {
  int v = 23;
  thread_pool pool{1};
  auto s = schedule(pool) | then([&v]() { v = 42; });
  this_thread::sync_wait<void>(s);
  EXPECT_EQ(42, v);
}

TEST(then, multiple_invocations) {
  int v = 0;
  thread_pool pool{1};
  auto s = schedule(pool) | then([&v]() { v += 1; });
  this_thread::sync_wait<void>(s);
  EXPECT_EQ(1, v);
  this_thread::sync_wait<void>(s);
  EXPECT_EQ(2, v);
  this_thread::sync_wait<void>(s);
  EXPECT_EQ(3, v);
}

} // namespace
} // namespace execution
