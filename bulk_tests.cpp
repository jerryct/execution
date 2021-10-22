// SPDX-License-Identifier: MIT

#include "bulk.h"
#include "inline_task.h"
#include "sync_wait.h"
#include "then.h"
#include "thread_pool.h"
#include <gtest/gtest.h>

#ifdef TRACING
#include "jerryct/tracing/chrome_trace_event_exporter.h"
#include "jerryct/tracing/tracing.h"
#endif

namespace execution {
namespace {

TEST(bulk, thread_pool_trace) {
#ifdef TRACING
  jerryct::trace::ChromeTraceEventExporter ee{"playback.json"};
#endif

  std::vector<int> v(23, 42);
  for (int r : v) {
    EXPECT_EQ(42, r);
  }
  {
    thread_pool pool{2};
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
    auto s1 = schedule(pool) | then([]() { return 23; });
    auto s2 = bulk(s1, v.size(), [&v](const int i, const int m) {
#ifdef TRACING
      jerryct::trace::Span _{jerryct::trace::Tracer(), "task"};
#endif
      v[i] = m;
      std::this_thread::sleep_for(std::chrono::milliseconds{10});
    });
    this_thread::sync_wait<int>(s2);

    for (int r : v) {
      EXPECT_EQ(23, r);
    }
  }

#ifdef TRACING
  jerryct::trace::Tracer().Export(ee);
#endif
}

TEST(bulk, continuation_with_named_temporaries) {
  std::vector<int> v(23, 42);
  for (int r : v) {
    EXPECT_EQ(42, r);
  }

  thread_pool pool{8};
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

  thread_pool pool{8};
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

  thread_pool pool{8};
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
