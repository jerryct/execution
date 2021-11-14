// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT

#include "bulk.h"
#include "inline_task.h"
#include "sync_wait.h"
#include "then.h"
#include "thread_pool.h"
#include <benchmark/benchmark.h>

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

BENCHMARK(ThenThreadPool1);
BENCHMARK(ThenThreadPool4);
BENCHMARK(ThenThreadPool8);
BENCHMARK(ThenManual512ThreadPool8);
BENCHMARK(Bulk1ThreadPool1);
BENCHMARK(Bulk1ThreadPool4);
BENCHMARK(Bulk1ThreadPool8);
BENCHMARK(Bulk512ThreadPool1);
BENCHMARK(Bulk512ThreadPool4);
BENCHMARK(Bulk512ThreadPool8);
BENCHMARK(BulkManual512ThreadPool8);

} // namespace
} // namespace execution
