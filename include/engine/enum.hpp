#pragma once

#include <engine/type_traits.hpp>

namespace Enum
{
    // Enable implicit conversion from EnumT to its underlying type
    template <typename EnumT>
    constexpr typename meta::enable_if_t<meta::is_enum<EnumT>::value, meta::underlying_type_t<EnumT>>
        as_value(EnumT e) noexcept
    {
        return static_cast<meta::underlying_type_t<EnumT>>(e);
    }

    // Enable implicit conversion from underlying type back to EnumT
    template <typename EnumT>
    constexpr typename meta::enable_if_t<meta::is_enum<EnumT>::value, EnumT>
        as_type(meta::underlying_type_t<EnumT> value) noexcept
    {
        return static_cast<EnumT>(value);
    }
}

namespace Enum
{
    // Detection traits for bitwise and comparison operators for EnumT, EnumT
    template <typename, typename T, typename = void> struct has_bitwise_and : meta::false_type {};
    template <typename T> struct has_bitwise_and<decltype(void(std::declval<T>() & std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_bitwise_or : meta::false_type {};
    template <typename T> struct has_bitwise_or<decltype(void(std::declval<T>() | std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_bitwise_xor : meta::false_type {};
    template <typename T> struct has_bitwise_xor<decltype(void(std::declval<T>() ^ std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_bitwise_not : meta::false_type {};
    template <typename T> struct has_bitwise_not<decltype(void(~std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_compound_and : meta::false_type {};
    template <typename T> struct has_compound_and<decltype(void(std::declval<T&>() &= std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_compound_or : meta::false_type {};
    template <typename T> struct has_compound_or<decltype(void(std::declval<T&>() |= std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_compound_xor : meta::false_type {};
    template <typename T> struct has_compound_xor<decltype(void(std::declval<T&>() ^= std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_greater_equal : meta::false_type {};
    template <typename T> struct has_greater_equal<decltype(void(std::declval<T>() >= std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_less_equal : meta::false_type {};
    template <typename T> struct has_less_equal<decltype(void(std::declval<T>() <= std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_greater : meta::false_type {};
    template <typename T> struct has_greater<decltype(void(std::declval<T>() > std::declval<T>())), T, void> : meta::true_type{};

    template <typename, typename T, typename = void> struct has_less : meta::false_type {};
    template <typename T> struct has_less<decltype(void(std::declval<T>() < std::declval<T>())), T, void> : meta::true_type{};
}

// Bitwise AND operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_and<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator&(EnumT lhs, EnumT rhs) noexcept
{
    return Enum::as_value(lhs) & Enum::as_value(rhs);
}

// Bitwise OR operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_or<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator|(EnumT lhs, EnumT rhs) noexcept
{
    return Enum::as_value(lhs) | Enum::as_value(rhs);
}

// Bitwise XOR operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_xor<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator^(EnumT lhs, EnumT rhs) noexcept
{
    return Enum::as_value(lhs) ^ Enum::as_value(rhs);
}

// Bitwise NOT operator for EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_not<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator~(EnumT e) noexcept
{
    return ~Enum::as_value(e);
}

// Compound assignment AND operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_compound_and<void, EnumT>::value, EnumT&>
operator&=(EnumT& lhs, EnumT rhs) noexcept
{
    lhs = Enum::as_type<EnumT>(Enum::as_value(lhs) & Enum::as_value(rhs));
    return lhs;
}

// Compound assignment OR operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_compound_or<void, EnumT>::value, EnumT&>
operator|=(EnumT& lhs, EnumT rhs) noexcept
{
    lhs = Enum::as_type<EnumT>(Enum::as_value(lhs) | Enum::as_value(rhs));
    return lhs;
}

// Compound assignment XOR operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_compound_xor<void, EnumT>::value, EnumT&>
operator^=(EnumT& lhs, EnumT rhs) noexcept
{
    lhs = Enum::as_type<EnumT>(Enum::as_value(lhs) ^ Enum::as_value(rhs));
    return lhs;
}

// Greater or equal comparison operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_greater_equal<void, EnumT>::value, bool>
operator>=(EnumT lhs, EnumT rhs) noexcept
{
    return Enum::as_value(lhs) >= Enum::as_value(rhs);
}

// Less or equal comparison operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_less_equal<void, EnumT>::value, bool>
operator<=(EnumT lhs, EnumT rhs) noexcept
{
    return Enum::as_value(lhs) <= Enum::as_value(rhs);
}

// Greater than comparison operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_greater<void, EnumT>::value, bool>
operator>(EnumT lhs, EnumT rhs) noexcept
{
    return Enum::as_value(lhs) > Enum::as_value(rhs);
}

// Less than comparison operator for EnumT, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_less<void, EnumT>::value, bool>
operator<(EnumT lhs, EnumT rhs) noexcept
{
    return Enum::as_value(lhs) < Enum::as_value(rhs);
}

namespace Enum
{
    // Detection traits for bitwise and comparison operators for EnumT, UnderlyingType
    template <typename, typename T, typename = void> struct has_bitwise_and_enum_underlying : meta::false_type {};
    template <typename T> struct has_bitwise_and_enum_underlying<decltype(void(std::declval<T>() & std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_bitwise_or_enum_underlying : meta::false_type {};
    template <typename T> struct has_bitwise_or_enum_underlying<decltype(void(std::declval<T>() | std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_bitwise_xor_enum_underlying : meta::false_type {};
    template <typename T> struct has_bitwise_xor_enum_underlying<decltype(void(std::declval<T>() ^ std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_compound_and_enum_underlying : meta::false_type {};
    template <typename T> struct has_compound_and_enum_underlying<decltype(void(std::declval<T&>() &= std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_compound_or_enum_underlying : meta::false_type {};
    template <typename T> struct has_compound_or_enum_underlying<decltype(void(std::declval<T&>() |= std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_compound_xor_enum_underlying : meta::false_type {};
    template <typename T> struct has_compound_xor_enum_underlying<decltype(void(std::declval<T&>() ^= std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_greater_equal_enum_underlying : meta::false_type {};
    template <typename T> struct has_greater_equal_enum_underlying<decltype(void(std::declval<T>() >= std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_less_equal_enum_underlying : meta::false_type {};
    template <typename T> struct has_less_equal_enum_underlying<decltype(void(std::declval<T>() <= std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_greater_enum_underlying : meta::false_type {};
    template <typename T> struct has_greater_enum_underlying<decltype(void(std::declval<T>() > std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type{};

    template <typename, typename T, typename = void> struct has_less_enum_underlying : meta::false_type {};
    template <typename T> struct has_less_enum_underlying<decltype(void(std::declval<T>() < std::declval<meta::underlying_type_t<T>>())), T, void> : meta::true_type{};
}

// Bitwise AND operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_and_enum_underlying<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator&(EnumT lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    return Enum::as_value(lhs) & rhs;
}

// Bitwise OR operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_or_enum_underlying<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator|(EnumT lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    return Enum::as_value(lhs) | rhs;
}

// Bitwise XOR operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_xor_enum_underlying<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator^(EnumT lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    return Enum::as_value(lhs) ^ rhs;
}

// Compound assignment AND operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_compound_and_enum_underlying<void, EnumT>::value, EnumT&>
operator&=(EnumT& lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    lhs = Enum::as_type<EnumT>(Enum::as_value(lhs) & rhs);
    return lhs;
}

// Compound assignment OR operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_compound_or_enum_underlying<void, EnumT>::value, EnumT&>
operator|=(EnumT& lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    lhs = Enum::as_type<EnumT>(Enum::as_value(lhs) | rhs);
    return lhs;
}

// Compound assignment XOR operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_compound_xor_enum_underlying<void, EnumT>::value, EnumT&>
operator^=(EnumT& lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    lhs = Enum::as_type<EnumT>(Enum::as_value(lhs) ^ rhs);
    return lhs;
}

// Greater or equal comparison operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_greater_equal_enum_underlying<void, EnumT>::value, bool>
operator>=(EnumT lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    return Enum::as_value(lhs) >= rhs;
}

// Less or equal comparison operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_less_equal_enum_underlying<void, EnumT>::value, bool>
operator<=(EnumT lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    return Enum::as_value(lhs) <= rhs;
}

// Greater than comparison operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_greater_enum_underlying<void, EnumT>::value, bool>
operator>(EnumT lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    return Enum::as_value(lhs) > rhs;
}

// Less than comparison operator for EnumT, UnderlyingType
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_less_enum_underlying<void, EnumT>::value, bool>
operator<(EnumT lhs, meta::underlying_type_t<EnumT> rhs) noexcept
{
    return Enum::as_value(lhs) < rhs;
}

namespace Enum
{
    // Detection traits for bitwise and comparison operators for UnderlyingType, EnumT
    template <typename, typename T, typename = void> struct has_bitwise_and_underlying_enum : meta::false_type {};
    template <typename T> struct has_bitwise_and_underlying_enum<decltype(void(std::declval<meta::underlying_type_t<T>>()& std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_bitwise_or_underlying_enum : meta::false_type {};
    template <typename T> struct has_bitwise_or_underlying_enum<decltype(void(std::declval<meta::underlying_type_t<T>>() | std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_bitwise_xor_underlying_enum : meta::false_type {};
    template <typename T> struct has_bitwise_xor_underlying_enum<decltype(void(std::declval<meta::underlying_type_t<T>>() ^ std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_greater_equal_underlying_enum : meta::false_type {};
    template <typename T> struct has_greater_equal_underlying_enum<decltype(void(std::declval<meta::underlying_type_t<T>>() >= std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_less_equal_underlying_enum : meta::false_type {};
    template <typename T> struct has_less_equal_underlying_enum<decltype(void(std::declval<meta::underlying_type_t<T>>() <= std::declval<T>())), T, void> : meta::true_type {};

    template <typename, typename T, typename = void> struct has_greater_underlying_enum : meta::false_type {};
    template <typename T> struct has_greater_underlying_enum<decltype(void(std::declval<meta::underlying_type_t<T>>() > std::declval<T>())), T, void> : meta::true_type{};

    template <typename, typename T, typename = void> struct has_less_underlying_enum : meta::false_type {};
    template <typename T> struct has_less_underlying_enum<decltype(void(std::declval<meta::underlying_type_t<T>>() < std::declval<T>())), T, void> : meta::true_type{};
}

// Bitwise AND operator for UnderlyingType, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_and_underlying_enum<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator&(meta::underlying_type_t<EnumT> lhs, EnumT rhs) noexcept
{
    return lhs & Enum::as_value(rhs);
}

// Bitwise OR operator for UnderlyingType, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_or_underlying_enum<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator|(meta::underlying_type_t<EnumT> lhs, EnumT rhs) noexcept
{
    return lhs | Enum::as_value(rhs);
}

// Bitwise XOR operator for UnderlyingType, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_bitwise_xor_underlying_enum<void, EnumT>::value, meta::underlying_type_t<EnumT>>
operator^(meta::underlying_type_t<EnumT> lhs, EnumT rhs) noexcept
{
    return lhs ^ Enum::as_value(rhs);
}

// Greater or equal comparison operator for UnderlyingType, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_greater_equal_underlying_enum<void, EnumT>::value, bool>
operator>=(meta::underlying_type_t<EnumT> lhs, EnumT rhs) noexcept
{
    return lhs >= Enum::as_value(rhs);
}

// Less or equal comparison operator for UnderlyingType, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_less_equal_underlying_enum<void, EnumT>::value, bool>
operator<=(meta::underlying_type_t<EnumT> lhs, EnumT rhs) noexcept
{
    return lhs <= Enum::as_value(rhs);
}

// Greater than comparison operator for UnderlyingType, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_greater_underlying_enum<void, EnumT>::value, bool>
operator>(meta::underlying_type_t<EnumT> lhs, EnumT rhs) noexcept
{
    return lhs > Enum::as_value(rhs);
}

// Less than comparison operator for UnderlyingType, EnumT
template <typename EnumT>
constexpr meta::enable_if_t<meta::is_enum<EnumT>::value && !Enum::has_less_underlying_enum<void, EnumT>::value, bool>
operator<(meta::underlying_type_t<EnumT> lhs, EnumT rhs) noexcept
{
    return lhs < Enum::as_value(rhs);
}