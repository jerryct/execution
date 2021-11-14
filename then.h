// SPDX-License-Identifier: MIT

#ifndef THEN_H
#define THEN_H

#include "invoke_traits.h"
#include <type_traits>
#include <utility>

namespace execution {

namespace then_detail {

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

template <typename S, typename F> struct Sender {
  S s_;
  F f_;

  template <typename P> auto connect(P p) const { return s_.connect(Receiver<decltype(p), F>{std::move(p), f_}); }
};

} // namespace then_detail

template <typename Sender, typename Invocable> auto then(Sender sender, Invocable function) {
  return then_detail::Sender<Sender, Invocable>{std::move(sender), std::move(function)};
}
template <typename Invocable> auto then(Invocable function) {
  return [function](auto sender) {
    return then_detail::Sender<decltype(sender), Invocable>{std::move(sender), function};
  };
}

template <typename Sender, typename Invocable> auto operator|(Sender &&sender, Invocable &&function) {
  return std::forward<Invocable>(function)(std::forward<Sender>(sender));
}

} // namespace execution

#endif // THEN_H
