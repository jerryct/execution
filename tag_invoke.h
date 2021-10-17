// SPDX-License-Identifier: MIT

#ifndef TAG_INVOKE_H
#define TAG_INVOKE_H

#include "invoke_traits.h"
#include <type_traits>
#include <utility>

namespace execution {

namespace foo {

// functional
inline namespace unspecified {
namespace poison_pill {
// "poison pill" to hide overloads of tag_invoke() that might be found in parent namespace.
void tag_invoke();
struct tag_invoke_t {
  template <typename Tag, typename... Args> decltype(auto) operator()(Tag &&t, Args &&... args) const {
    return tag_invoke(std::forward<Tag>(t), std::forward<Args>(args)...);
  }
};
} // namespace poison_pill
inline constexpr poison_pill::tag_invoke_t tag_invoke{};
} // namespace unspecified

// traits
template <auto &Tag> using tag_t = typename std::decay_t<decltype(Tag)>;

template <typename Tag, typename... Args> using tag_invocable = is_invocable<decltype(tag_invoke), Tag, Args...>;

template <typename Tag, typename... Args>
using nothrow_tag_invocable =
    tag_invocable<Tag, Args...> /* && is_nothrow_invocable<decltype(tag_invoke), Tag, Args...> */;

template <typename Tag, typename... Args> using tag_invoke_result = invoke_result<decltype(tag_invoke), Tag, Args...>;
template <typename Tag, typename... Args> using tag_invoke_result_t = typename tag_invoke_result<Tag, Args...>::type;

} // namespace foo

} // namespace execution

#endif // EXECUTE_H
