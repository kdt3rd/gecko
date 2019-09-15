// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#pragma once

#include <type_traits>

////////////////////////////////////////

namespace base
{
template <typename...> struct make_void
{
    using type = void;
};
template <typename... Ts> using make_void_t = typename make_void<Ts...>::type;

template <typename T, class = void> struct has_arithmetic_ops : std::false_type
{};

template <typename T>
struct has_arithmetic_ops<
    T,
    make_void_t<
        decltype( std::declval<T>() + std::declval<T>() ),
        decltype( std::declval<T>() - std::declval<T>() ),
        decltype( std::declval<T>() * std::declval<T>() ),
        decltype( std::declval<T>() / std::declval<T>() )>> : std::true_type
{};

} // namespace base
