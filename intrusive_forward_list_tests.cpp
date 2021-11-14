// SPDX-License-Identifier: MIT

#include "intrusive_forward_list.h"
#include <gtest/gtest.h>

namespace execution {
namespace {

TEST(intrusive_forward_list, again) {
  struct data : intrusive_forward_list<data>::node {
    data(int v) : intrusive_forward_list<data>::node{}, x{v} {}
    int x;
  };

  data a{23};
  data b{42};

  intrusive_forward_list<data> q;
  EXPECT_TRUE(q.empty());
  q.push_back(a);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(23, q.front().x);

  q.push_back(b);
  q.pop_front();

  q.pop_front();
  EXPECT_TRUE(q.empty());

  // again
  q.push_back(a);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(23, q.front().x);
}

TEST(intrusive_forward_list, intr) {
  struct data : intrusive_forward_list<data>::node {
    data(int v) : intrusive_forward_list<data>::node{}, x{v} {}
    int x;
  };

  data a{23};
  data b{42};
  data c{73};

  intrusive_forward_list<data> q;
  EXPECT_TRUE(q.empty());
  q.push_back(a);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(23, q.front().x);
  q.push_back(b);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(23, q.front().x);
  q.push_back(c);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(23, q.front().x);
  q.pop_front();
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(42, q.front().x);
  q.pop_front();
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(73, q.front().x);
  q.pop_front();
  EXPECT_TRUE(q.empty());
  // again
  q.push_back(a);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(23, q.front().x);
}

} // namespace
} // namespace execution
