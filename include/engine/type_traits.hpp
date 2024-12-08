#pragma once

#include <type_traits>

// This namespace serves as a layer in-between std in order to implement missing >c++11 features
// Note that not everything is wrappable, so we'd still depend on std for those.
namespace meta
{
    using false_type = std::false_type;
    using true_type = std::true_type;

    // No wrapper possible?
    //template<typename T>
    //auto declval() noexcept -> decltype(std::declval<T>(0))
    //{
    //    return std::declval<T>(0);
    //}

    template <bool B, typename T = void>
    using enable_if = std::enable_if<B, T>;

    template <bool B, typename T = void>
    using enable_if_t = typename enable_if<B, T>::type;

    template <typename T>
    using is_enum = std::is_enum<T>;

    template <typename T, typename Enable = void>
    struct underlying_type;

    template <typename T>
    struct underlying_type<T, enable_if_t<is_enum<T>::value>>
    {
        using type = typename std::underlying_type<T>::type;
    };

    template <typename T>
    struct underlying_type<T, enable_if_t<!is_enum<T>::value>>
    {
        using type = void;
    };

    template <typename T>
    using underlying_type_t = typename underlying_type<T>::type;
}