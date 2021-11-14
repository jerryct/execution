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

} // namespace inline_task_detail

inline auto schedule() {
  return [](auto p) { return inline_task_detail::task<decltype(p)>{std::move(p)}; };
}

} // namespace execution

#endif // INLINE_TASK_H
