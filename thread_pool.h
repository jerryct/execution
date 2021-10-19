#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace execution {

struct task_base {
  void (*execute)(task_base *);
  void run() { (*execute)(this); }
};

namespace detail {

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

  template <typename F> void submit(F &&f) {
    {
      std::lock_guard<std::mutex> lk{m_};
      queue_.emplace(std::forward<F>(f));
    }
    c_.notify_one();
  }

private:
  bool done_;
  std::queue<task_base *> queue_;
  std::vector<join_thread> threads_;

  mutable std::mutex m_;
  std::condition_variable c_;

  void worker_thread() {
    task_base *task;
    while (true) {
      {
        std::unique_lock<std::mutex> lk{m_};
        c_.wait(lk, [this]() { return done_ || !queue_.empty(); });
        if (done_) {
          break;
        }
        task = std::move(queue_.front());
        queue_.pop();
      }

      task->run();
    }
  }
};

} // namespace detail

template <typename P> class task : public task_base {
public:
  template <typename U>
  task(U &&p, detail::thread_pool &scheduler) : task_base{&task::impl}, p_{std::forward<U>(p)}, scheduler_{scheduler} {}

  void start() { scheduler_.submit(this); }

private:
  static void impl(task_base *const b) {
    auto *const t = static_cast<task *>(b);
    t->p_.set_value();
  };

  P p_;
  detail::thread_pool &scheduler_;
};

inline auto schedule(detail::thread_pool &scheduler) {
  return [&scheduler](auto p) { return task<decltype(p)>{std::move(p), scheduler}; };
}

} // namespace execution

#endif