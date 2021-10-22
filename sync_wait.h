// SPDX-License-Identifier: MIT

#ifndef SYNC_WAIT_H
#define SYNC_WAIT_H

#include <condition_variable>
#include <mutex>
#include <type_traits>
#include <utility>

#ifdef TRACING
#include "jerryct/tracing/tracing.h"
#endif

// template <typename C> std::future<int> async_algo(C &&c) {
//  std::promise<int> p{};
//  auto f = p.get_future();
//  std::thread t{[pr = std::move(p), cont = std::forward<C>(c)]() mutable { pr.set_value(cont(42)); }};
//  t.detach();
//  return f;
//}

namespace this_thread {

struct empty {};

template <typename T> struct state {
  enum class Variant { monostate, error, value } variant_;
  T v_;
  std::mutex m;
  std::condition_variable c;
};

template <typename T> struct promise {
  state<T> *s_;
  template <typename... Ts> void set_value(Ts &&... v) {
    {
      std::lock_guard<std::mutex> lk{s_->m};
      s_->v_ = {std::forward<Ts>(v)...};
      s_->variant_ = state<T>::Variant::value;
      // Notify while under the lock to avoid that the waiting thread destroys the condition variable.
      s_->c.notify_one();
    }
  }
};

template <typename Result, typename Sender> auto sync_wait(Sender &&sender) {
#ifdef TRACING
  jerryct::trace::Span _1{jerryct::trace::Tracer(), "sync wait"};
#endif
  using R = std::conditional_t<std::is_void<Result>::value, empty, Result>;
  state<R> st{};
  promise<R> p{&st};
  auto op_state = std::forward<Sender>(sender).connect(std::move(p));
  {
#ifdef TRACING
    jerryct::trace::Span _2{jerryct::trace::Tracer(), "sync wait op state start"};
#endif
    op_state.start();
  }
  {
    std::unique_lock<std::mutex> lk{st.m};
    st.c.wait(lk, [&st]() { return st.variant_ != state<R>::Variant::monostate; });
  }
  if (st.variant_ == state<R>::Variant::error) {
    throw;
  }
  return std::move(st.v_);
}

} // namespace this_thread

#endif // SYNC_WAIT_H
