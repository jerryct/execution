// SPDX-License-Identifier: MIT

#ifndef INTRUSIVE_FORWARD_LIST_H
#define INTRUSIVE_FORWARD_LIST_H

#include <cstddef>

namespace execution {

template <typename T> class intrusive_forward_list {
public:
  struct node {
    node *next;
  };

  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = std::size_t;

  constexpr reference back() { return *static_cast<T *>(last); }
  constexpr const_reference back() const { return *static_cast<T *>(last); }
  constexpr reference front() { return *static_cast<T *>(before_begin.next); }
  constexpr const_reference front() const { return *static_cast<T *>(before_begin.next); }

  constexpr void push_back(reference d) {
    last->next = &d;
    last = last->next;
    last->next = nullptr;
  }
  constexpr void pop_front() {
    before_begin.next = before_begin.next->next;
    if (empty()) {
      last = &before_begin;
    }
  }
  constexpr bool empty() const { return before_begin.next == nullptr; }

private:
  node before_begin{nullptr};
  node *last{&before_begin};
};

} // namespace execution

#endif // INTRUSIVE_FORWARD_LIST_H
