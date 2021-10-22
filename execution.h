// SPDX-License-Identifier: MIT

#ifndef EXECUTE_H
#define EXECUTE_H

#include "invoke_traits.h"
#include "thread_pool.h"
#include <type_traits>
#include <utility>

namespace execution {

template <typename P> struct inline_task {
  P p_;
  void start() { p_.set_value(); }
};

inline auto schedule() {
  return [](auto p) { return inline_task<decltype(p)>{std::move(p)}; };
}

template <typename P, typename F> struct Receiver {
  P p_;
  F f_;
  template <typename... Ts, typename std::enable_if_t<std::is_void<invoke_result_t<F, Ts...>>::value, bool> = true>
  void set_value(Ts &&... v) {
    f_(std::forward<Ts>(v)...);
    p_.set_value();
  }
  template <typename... Ts, typename std::enable_if_t<!std::is_void<invoke_result_t<F, Ts...>>::value, bool> = true>
  void set_value(Ts &&... v) {
    p_.set_value(f_(std::forward<Ts>(v)...));
  }
};

template <typename Sender, typename Invocable> auto then(Sender input, Invocable function) {
  return [input, function](auto p) { return input(Receiver<decltype(p), Invocable>{std::move(p), function}); };
}
template <typename Invocable> auto then(Invocable function) {
  return [function](auto sender) {
    return [sender, function](auto p) { return sender(Receiver<decltype(p), Invocable>{std::move(p), function}); };
  };
}

template <typename Sender, typename Invocable> auto operator|(Sender &&sender, Invocable &&function) {
  return std::forward<Invocable>(function)(std::forward<Sender>(sender));
}

} // namespace execution

#endif // EXECUTE_H
