// SPDX-License-Identifier: MIT

#ifndef INLINE_TASK_H
#define INLINE_TASK_H

#include <utility>

namespace execution {

template <typename P> struct inline_task {
  P p_;
  void start() { p_.set_value(); }
};

inline auto schedule() {
  return [](auto p) { return inline_task<decltype(p)>{std::move(p)}; };
}

} // namespace execution

#endif // INLINE_TASK_H
