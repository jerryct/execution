// SPDX-License-Identifier: MIT

#include "invoke_traits.h"
#include <gtest/gtest.h>

namespace execution {

void ReturnVoid();
int ReturnInt();
int WithInt(int);
int &WithRefInt(int &);
int *WithPtrInt(int *);
const int &WithConstRefInt(const int &);
const int *WithConstPtrInt(const int *);

namespace {

TEST(is_invocable, lambda) {
  {
    const auto f = []() {};
    EXPECT_TRUE(is_invocable<decltype(f)>::value);
  }
  {
    const auto f = [](int) { return 23; };
    EXPECT_FALSE(is_invocable<decltype(f)>::value);
  }
  {
    const auto f = [](int) { return 23; };
    EXPECT_TRUE((is_invocable<decltype(f), int>::value));
  }
}

TEST(is_invocable, function_object) {
  {
    struct Foo {
      void operator()();
    };
    EXPECT_TRUE(is_invocable<Foo>::value);
  }
  {
    struct Foo {
      int operator()(int);
    };
    EXPECT_FALSE(is_invocable<Foo>::value);
  }
  {
    struct Foo {
      int operator()(int);
    };
    EXPECT_TRUE((is_invocable<Foo, int>::value));
  }
}

TEST(is_invocable, function) {
  EXPECT_TRUE(is_invocable<decltype(ReturnVoid)>::value);
  EXPECT_FALSE(is_invocable<decltype(WithInt)>::value);
  EXPECT_TRUE((is_invocable<decltype(WithInt), int>::value));
}

TEST(is_invocable, not_invocable) {
  EXPECT_FALSE(is_invocable<int>::value);
  struct Foo {};
  EXPECT_FALSE(is_invocable<Foo>::value);
}

TEST(invoke_result, lambda) {
  {
    const auto f = []() {};
    EXPECT_TRUE((std::is_same<void, invoke_result_t<decltype(f)>>::value));
  }
  {
    const auto f = []() { return 23; };
    EXPECT_TRUE((std::is_same<int, invoke_result_t<decltype(f)>>::value));
  }
  {
    const auto f = [](int v) -> int { return v; };
    EXPECT_TRUE((std::is_same<int, invoke_result_t<decltype(f), int>>::value));
  }
  {
    const auto f = [](int &v) -> int & { return v; };
    EXPECT_TRUE((std::is_same<int &, invoke_result_t<decltype(f), int &>>::value));
  }
  {
    const auto f = [](int *v) -> int * { return v; };
    EXPECT_TRUE((std::is_same<int *, invoke_result_t<decltype(f), int *>>::value));
  }
  {
    const auto f = [](const int &v) -> const int & { return v; };
    EXPECT_TRUE((std::is_same<const int &, invoke_result_t<decltype(f), int &>>::value));
  }
  {
    const auto f = [](const int *v) -> const int * { return v; };
    EXPECT_TRUE((std::is_same<const int *, invoke_result_t<decltype(f), int *>>::value));
  }
}

TEST(invoke_result, function_object) {
  {
    struct Foo {
      void operator()();
    };
    EXPECT_TRUE((std::is_same<void, invoke_result_t<Foo>>::value));
  }
  {
    struct Foo {
      int operator()();
    };
    EXPECT_TRUE((std::is_same<int, invoke_result_t<Foo>>::value));
  }
  {
    struct Foo {
      int operator()(int);
    };
    EXPECT_TRUE((std::is_same<int, invoke_result_t<Foo, int>>::value));
  }
  {
    struct Foo {
      int &operator()(int &);
    };
    EXPECT_TRUE((std::is_same<int &, invoke_result_t<Foo, int &>>::value));
  }
  {
    struct Foo {
      int *operator()(int *);
    };
    EXPECT_TRUE((std::is_same<int *, invoke_result_t<Foo, int *>>::value));
  }
  {
    struct Foo {
      const int &operator()(const int &);
    };
    EXPECT_TRUE((std::is_same<const int &, invoke_result_t<Foo, int &>>::value));
  }
  {
    struct Foo {
      const int *operator()(const int *);
    };
    EXPECT_TRUE((std::is_same<const int *, invoke_result_t<Foo, int *>>::value));
  }
}

TEST(invoke_result, function) {
  EXPECT_TRUE((std::is_same<void, invoke_result_t<decltype(ReturnVoid)>>::value));
  EXPECT_TRUE((std::is_same<int, invoke_result_t<decltype(ReturnInt)>>::value));
  EXPECT_TRUE((std::is_same<int, invoke_result_t<decltype(WithInt), int>>::value));
  EXPECT_TRUE((std::is_same<int &, invoke_result_t<decltype(WithRefInt), int &>>::value));
  EXPECT_TRUE((std::is_same<int *, invoke_result_t<decltype(WithPtrInt), int *>>::value));
  EXPECT_TRUE((std::is_same<const int &, invoke_result_t<decltype(WithConstRefInt), const int &>>::value));
  EXPECT_TRUE((std::is_same<const int *, invoke_result_t<decltype(WithConstPtrInt), const int *>>::value));
}

} // namespace
} // namespace execution
