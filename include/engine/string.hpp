#pragma once

#include <engine/byte_types.hpp>
#include <string>

using String = std::string;

class StringView
{
public:
    constexpr StringView() noexcept : m_pData(nullptr), m_size(0) {}

    constexpr StringView(const char* str) noexcept
        : m_pData(str)
        , m_size(str ? std::strlen(str) : 0)
    {}

    constexpr StringView(const char* str, SizeType len) noexcept
        : m_pData(str)
        , m_size(len)
    {}

    StringView(const String& str) noexcept
        : m_pData(str.data())
        , m_size(str.size())
    {}

    constexpr const char& operator[](SizeType pos) const noexcept
    {
        return m_pData[pos];
    }

    constexpr const char* data() const noexcept
    {
        return m_pData;
    }

    constexpr SizeType size() const noexcept
    {
        return m_size;
    }

    constexpr bool empty() const noexcept
    {
        return m_size == 0;
    }

    String to_string() const
    {
        return String(m_pData, m_size);
    }

    constexpr std::size_t find_last_of(const char* chars) const noexcept
    {
        for (std::size_t i = m_size; i > 0; --i) {
            if (std::strchr(chars, m_pData[i - 1])) {
                return i - 1;
            }
        }
        return npos;
    }

    constexpr StringView substr(SizeType pos, SizeType count = npos) const noexcept
    {
        if (pos > m_size)
            return StringView();  // Out of bounds

        return StringView(m_pData + pos, (count > m_size - pos) ? (m_size - pos) : count);
    }

private:
    template <typename T>
    static int compare_impl(const T& lhs, const T& rhs, size_t len, std::false_type /*is_runtime*/)
    {
        return std::memcmp(lhs, rhs, len);
    }

    template <typename T>
    static constexpr int compare_impl(const T& lhs, const T& rhs, size_t len, std::true_type /*is_compiletime*/)
    {
        for (size_t i = 0; i < len; ++i)
            if (lhs[i] != rhs[i])
                return static_cast<int>(lhs[i]) - static_cast<int>(rhs[i]);
        return 0;
    }

public:
    constexpr int compare(const StringView& other) const noexcept
    {
        const SizeType min_len = m_size < other.m_size ? m_size : other.m_size;
        const int result = compare_impl(m_pData, other.m_pData, min_len, std::integral_constant<bool, true>{});

        if (result != 0)
            return result;
        if (m_size < other.m_size)
            return -1;
        if (m_size > other.m_size)
            return 1;

        return 0;
    }

    constexpr bool operator==(const StringView& other) const noexcept
    {
        return m_size == other.m_size && compare_impl(m_pData, other.m_pData, m_size, std::integral_constant<bool, true>{}) == 0;
    }

    constexpr bool operator!=(const StringView& other) const noexcept
    {
        return !(*this == other);
    }

    static constexpr SizeType npos = String::npos;

private:
    const char* m_pData;
    SizeType m_size;
};

#include <fmt/core.h>

template <>
struct fmt::formatter<StringView>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const StringView& sv, FormatContext& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", fmt::string_view(sv.data(), sv.size()));
    }
};