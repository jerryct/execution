// SPDX-License-Identifier: MIT

#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <type_traits>
#include <utility>

namespace execution {

namespace detail {

template <typename F, typename... Args>
constexpr auto try_invoke(F &&f, Args &&... args) -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));

template <bool, typename, typename...> struct invoke_result {};
template <typename F, typename... Args> struct invoke_result<true, F, Args...> {
  using type = decltype(try_invoke(std::declval<F>(), std::declval<Args>()...));
};

template <typename, typename, typename...> struct is_invocable : std::false_type {};
template <typename F, typename... Args>
struct is_invocable<decltype(static_cast<void>(try_invoke(std::declval<F>(), std::declval<Args>()...))), F, Args...>
    : std::true_type {};

} // namespace detail

template <typename F, typename... Args> struct is_invocable : detail::is_invocable<void, F, Args...> {};

template <typename F, typename... Args> struct invoke_result {
  using type = typename detail::invoke_result<is_invocable<F, Args...>::value, F, Args...>::type;
};
template <typename F, typename... Args> using invoke_result_t = typename invoke_result<F, Args...>::type;

} // namespace execution

#endif // TYPE_TRAITS_H
