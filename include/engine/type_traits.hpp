#pragma once

#include <type_traits>

namespace meta
{
    template <bool B, typename T = void>
    struct enable_if_t {};

    template <typename T>
    struct enable_if_t<true, T>
    {
        using type = T;
    };

    template <class T>
    using underlying_type_t = typename std::underlying_type<T>::type;
}