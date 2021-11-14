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

} // namespace execution

#endif // EXECUTE_H
