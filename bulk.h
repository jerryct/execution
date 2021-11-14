// SPDX-License-Identifier: MIT

#ifndef BULK_H
#define BULK_H

#include "invoke_traits.h"
#include "sync_wait.h"
#include <type_traits>
#include <utility>
#include <vector>

namespace execution {

namespace bulk_detail {

template <typename P, typename F> struct Receiver {
  P p_;
  F f_;
  int i_;

  template <typename... Ts, typename std::enable_if_t<std::is_void<invoke_result_t<F, int, Ts...>>::value, bool> = true>
  void set_value(Ts &&... v) {
    f_(i_, std::forward<Ts>(v)...);
    p_.set_value();
  }
  template <typename... Ts,
            typename std::enable_if_t<!std::is_void<invoke_result_t<F, int, Ts...>>::value, bool> = true>
  void set_value(Ts &&... v) {
    p_.set_value(f_(i_, std::forward<Ts>(v)...));
  }
};

template <typename O> struct Operation {
  void start() {
    for (auto &o : v) {
      o.start();
    }
  }
  std::vector<O> v;
};

struct latch {
  this_thread::state<int> *s_;
  template <typename... Ts> void set_value() {
    {
      std::lock_guard<std::mutex> lk{s_->m};
      s_->v_ -= 1;
      if (s_->v_ == 0) {
        s_->variant_ = this_thread::state<int>::Variant::value;
        // Notify while under the lock to avoid that the waiting thread destroys the condition variable.
        s_->c.notify_one();
      }
    }
  }
};

template <typename S, typename F> struct Sender {
  S sender;
  F function;
  int max_;

  template <typename P> auto operator()(P p) const {
    p.s_->v_ = max_;

    using O = decltype(sender(Receiver<latch, F>{latch{p.s_}, function, 0}));
    Operation<O> op;
    op.v.reserve(max_);
    for (int i = 0; i < max_; ++i) {
      op.v.push_back(sender(Receiver<latch, F>{latch{p.s_}, function, i}));
    }
    return op;
  }
};

} // namespace bulk_detail

template <typename Sender, typename Invocable> auto bulk(Sender sender, int i, Invocable function) {
  return bulk_detail::Sender<Sender, Invocable>{sender, function, i};
}
template <typename Invocable> auto bulk(int i, Invocable function) {
  return [i, function](auto sender) {
    return bulk_detail::Sender<decltype(sender), Invocable>{std::move(sender), function, i};
  };
}

} // namespace execution

#endif // BULK_H
