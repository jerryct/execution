// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT

#include "bulk.h"
#include "inline_task.h"
#include "sync_wait.h"
#include "then.h"
#include "thread_pool.h"
#include <benchmark/benchmark.h>

#ifdef TRACING
#include "jerryct/tracing/chrome_trace_event_exporter.h"
#include "jerryct/tracing/tracing.h"
#endif

namespace execution {
namespace {

void ThenThreadPool1(benchmark::State &state) {
  thread_pool pool{1};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | then([]() { return 23; }));
  }
}

void ThenThreadPool4(benchmark::State &state) {
  thread_pool pool{4};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | then([]() { return 23; }));
  }
}

void ThenThreadPool8(benchmark::State &state) {
  thread_pool pool{8};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | then([]() { return 23; }));
  }
}

void ThenManual512ThreadPool8(benchmark::State &state) {
  thread_pool pool{8};
  for (auto _ : state) {
    for (int i = 0; i < 512; ++i) {
      this_thread::sync_wait<int>(schedule(pool) | then([]() { return 23; }));
    }
  }
}

void Bulk1ThreadPool1(benchmark::State &state) {
  thread_pool pool{1};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | bulk(1, [](const int) {}));
  }
}

void Bulk1ThreadPool4(benchmark::State &state) {
  thread_pool pool{4};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | bulk(1, [](const int) {}));
  }
}

void Bulk1ThreadPool8(benchmark::State &state) {
  thread_pool pool{8};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | bulk(1, [](const int) {}));
  }
}

void Bulk512ThreadPool1(benchmark::State &state) {
  thread_pool pool{1};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | bulk(512, [](const int) {}));
  }
}

void Bulk512ThreadPool4(benchmark::State &state) {
  thread_pool pool{4};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | bulk(512, [](const int) {}));
  }
}

void Bulk512ThreadPool8(benchmark::State &state) {
  thread_pool pool{8};
  for (auto _ : state) {
    this_thread::sync_wait<int>(schedule(pool) | bulk(512, [](const int) {}));
  }
}

void BulkManual512ThreadPool8(benchmark::State &state) {
  thread_pool pool{8};
  for (auto _ : state) {
    for (int i{0}; i < 512; ++i) {
      this_thread::sync_wait<int>(schedule(pool) | bulk(1, [](const int) {}));
    }
  }
}

struct Foo {
  Foo() {}
  ~Foo() {
#ifdef TRACING
    jerryct::trace::Tracer().Export(ee);
#endif
  }

#ifdef TRACING
  jerryct::trace::ChromeTraceEventExporter ee{"playback.json"};
#endif
  thread_pool pool{8};
};

void ThenThreadPool8Trace(benchmark::State &state) {
  Foo f{};
  for (auto _ : state) {
    auto s = schedule(f.pool) | then([]() {
#ifdef TRACING
               jerryct::trace::Span _{jerryct::trace::Tracer(), "task"};
#endif
             });
    this_thread::sync_wait<void>(s);
  }
}

void Bulk512ThreadPool8Trace(benchmark::State &state) {
  Foo f{};
  for (auto _ : state) {
    auto s = schedule(f.pool) | bulk(512, [](const int) {
#ifdef TRACING
               jerryct::trace::Span _{jerryct::trace::Tracer(), "task"};
#endif
             });
    this_thread::sync_wait<int>(s);
  }
}

BENCHMARK(ThenThreadPool1);
BENCHMARK(ThenThreadPool4);
BENCHMARK(ThenThreadPool8);
BENCHMARK(ThenManual512ThreadPool8);
BENCHMARK(ThenThreadPool8Trace);
BENCHMARK(Bulk1ThreadPool1);
BENCHMARK(Bulk1ThreadPool4);
BENCHMARK(Bulk1ThreadPool8);
BENCHMARK(Bulk512ThreadPool1);
BENCHMARK(Bulk512ThreadPool4);
BENCHMARK(Bulk512ThreadPool8);
BENCHMARK(BulkManual512ThreadPool8);
BENCHMARK(Bulk512ThreadPool8Trace);

} // namespace
} // namespace execution
