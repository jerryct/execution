// SPDX-License-Identifier: MIT

#ifndef INLINE_TASK_H
#define INLINE_TASK_H

#include <utility>

namespace execution {

namespace inline_task_detail {

template <typename P> struct task {
  P p_;
  void start() { p_.set_value(); }
};

struct Sender {
  template <typename P> auto operator()(P p) const { return task<decltype(p)>{std::move(p)}; }
};

} // namespace inline_task_detail

inline auto schedule() { return inline_task_detail::Sender{}; }

} // namespace execution

#endif // INLINE_TASK_H
