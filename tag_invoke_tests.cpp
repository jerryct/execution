// SPDX-License-Identifier: MIT

#include "tag_invoke.h"
#include <gtest/gtest.h>

namespace execution {
namespace {

constexpr struct function_fn {
  template <typename... T> auto operator()(T &&... t) const -> foo::tag_invoke_result_t<function_fn, T &&...> {
    return foo::tag_invoke(*this, std::forward<T>(t)...);
  }
} function{};

class return_void {
  friend void tag_invoke(foo::tag_t<function>, const return_void &);
};
class return_int {
  friend int tag_invoke(foo::tag_t<function>, const return_int &);
};
class with_int {
  friend int tag_invoke(foo::tag_t<function>, const with_int &, int);
};
class with_ref_int {
  friend int &tag_invoke(foo::tag_t<function>, const with_ref_int &, int &);
};
class with_ptr_int {
  friend int *tag_invoke(foo::tag_t<function>, const with_ptr_int &, int *);
};
class with_const_ref_int {
  friend const int &tag_invoke(foo::tag_t<function>, const with_const_ref_int &, const int &);
};
class with_const_ptr_int {
  friend const int *tag_invoke(foo::tag_t<function>, const with_const_ptr_int &, const int *);
};

TEST(tag_invoke, function) {
  int value{23};
  const int const_value{23};

  EXPECT_TRUE((std::is_same<void, decltype(function(return_void{}))>::value));
  EXPECT_TRUE((std::is_same<int, decltype(function(return_int{}))>::value));
  EXPECT_TRUE((std::is_same<int, decltype(function(with_int{}, 23))>::value));
  EXPECT_TRUE((std::is_same<int &, decltype(function(with_ref_int{}, value))>::value));
  EXPECT_TRUE((std::is_same<int *, decltype(function(with_ptr_int{}, &value))>::value));
  EXPECT_TRUE((std::is_same<const int &, decltype(function(with_const_ref_int{}, const_value))>::value));
  EXPECT_TRUE((std::is_same<const int *, decltype(function(with_const_ptr_int{}, &const_value))>::value));
}

namespace A {
constexpr struct schedule_fn {
  template <typename T> auto operator()(const T &t) const -> foo::tag_invoke_result_t<schedule_fn, const T &> {
    return foo::tag_invoke(*this, t);
  }
} schedule{};
} // namespace A

template <typename T> auto Print(const T &v) { return A::schedule(v); }

namespace B {
struct some_scheduler {
private:
  friend int tag_invoke(foo::tag_t<A::schedule>, const some_scheduler &) { return 23; };
};
} // namespace B

namespace C {
struct some_scheduler {
private:
  friend float tag_invoke(foo::tag_t<A::schedule>, const some_scheduler &) { return 42.0F; };
};
} // namespace C

TEST(tag_invoke, tag_invoke) {
  B::some_scheduler b{};
  EXPECT_EQ(23, Print(b));
  C::some_scheduler c{};
  EXPECT_EQ(42.0F, Print(c));
}

} // namespace
} // namespace execution
