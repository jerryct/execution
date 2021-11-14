#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "intrusive_forward_list.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace execution {

namespace thread_pool_detail {

struct task_base : intrusive_forward_list<task_base>::node {
  using task = void (*)(task_base *);
  task_base(task v) : intrusive_forward_list<task_base>::node{}, execute{v} {}
  task execute;
  void run() { (*execute)(this); }
};

} // namespace thread_pool_detail

class thread_pool {
  struct join_thread {
    join_thread() noexcept = default;
    join_thread(join_thread &&) noexcept = default;
    template <typename F, typename... Args>
    explicit join_thread(F &&f, Args &&... args) : t_{std::forward<F>(f), std::forward<Args>(args)...} {}
    join_thread(const join_thread &) = delete;

    join_thread &operator=(const join_thread &) = delete;
    join_thread &operator=(join_thread &&) noexcept = default;

    ~join_thread() {
      if (t_.joinable()) {
        t_.join();
      }
    }

    std::thread t_;
  };

public:
  thread_pool(const unsigned thread_count = std::thread::hardware_concurrency()) : done_{false} {
    try {
      for (unsigned i{0}; i < thread_count; ++i) {
        threads_.emplace_back(&thread_pool::worker_thread, this);
      }
    } catch (...) {
      {
        std::lock_guard<std::mutex> lk{m_};
        done_ = true;
      }
      c_.notify_all();
      throw;
    }
  }

  ~thread_pool() {
    {
      std::lock_guard<std::mutex> lk{m_};
      done_ = true;
    }
    c_.notify_all();
  }

  void submit(thread_pool_detail::task_base &f) {
    {
      std::lock_guard<std::mutex> lk{m_};
      queue_.push_back(f);
    }
    c_.notify_one();
  }

private:
  bool done_;
  intrusive_forward_list<thread_pool_detail::task_base> queue_;
  std::vector<join_thread> threads_;

  mutable std::mutex m_;
  std::condition_variable c_;

  void worker_thread() {
    thread_pool_detail::task_base *task;
    while (true) {
      {
        std::unique_lock<std::mutex> lk{m_};
        c_.wait(lk, [this]() { return done_ || !queue_.empty(); });
        if (done_) {
          return;
        }
        task = &queue_.front();
        queue_.pop_front();
      }

      task->run();
    }
    std::abort();
  }
};

namespace thread_pool_detail {

template <typename P> class task : public task_base {
public:
  template <typename U>
  task(U &&p, thread_pool &scheduler) : task_base{&task::impl}, p_{std::forward<U>(p)}, scheduler_{&scheduler} {}

  void start() { scheduler_->submit(*this); }

private:
  static void impl(task_base *const b) {
    auto *const t = static_cast<task *>(b);
    t->p_.set_value();
  };

  P p_;
  thread_pool *scheduler_;
};

struct Sender {
  thread_pool &scheduler;

  template <typename P> auto operator()(P p) const { return task<decltype(p)>{std::move(p), scheduler}; };
};

} // namespace thread_pool_detail

inline auto schedule(thread_pool &scheduler) { return thread_pool_detail::Sender{scheduler}; }

} // namespace execution

#endif // THREAD_POOL_H
