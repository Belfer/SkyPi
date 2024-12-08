#pragma once

#include <engine/byte_types.hpp>

#include <fmt/core.h>

#include <string>
#include <cstring>
#include <stdexcept>
#include <algorithm>

using String = std::string;

template <SizeType N>
class CString
{
public:
    CString()
    {
        m_data[0] = '\0';
    }

    CString(const char* str)
    {
        if (str)
        {
            std::strncpy(m_data, str, N - 1);
            m_data[N - 1] = '\0';
        }
        else
        {
            m_data[0] = '\0';
        }
    }

    CString(const String& str)
    {
        if (str.size() < N)
        {
            std::strncpy(m_data, str.c_str(), N - 1);
            m_data[N - 1] = '\0';
        }
        else
        {
            throw std::overflow_error("String is too large for the buffer");
        }
    }

    const char* c_str() const
    {
        return m_data;
    }

    SizeType length() const
    {
        return std::strlen(m_data);
    }

    SizeType size() const
    {
        return N;
    }

    bool empty() const
    {
        return m_data[0] == '\0';
    }

    void clear()
    {
        m_data[0] = '\0';
    }

    template <typename... Args>
    void format(const char* fmt_str, Args&&... args)
    {
        SizeType required_size = fmt::formatted_size(fmt_str, std::forward<Args>(args)...);
        if (required_size >= N)
        {
            throw std::overflow_error("Formatted string is too large for the buffer");
        }

        auto result = fmt::format_to(m_data, fmt_str, std::forward<Args>(args)...);
        *result = '\0';
    }

    void set(const char* str)
    {
        if (str)
        {
            std::strncpy(m_data, str, N - 1);
            m_data[N - 1] = '\0';
        }
        else
        {
            m_data[0] = '\0';
        }
    }

    void set(const String& str)
    {
        if (str.size() < N)
        {
            std::strncpy(m_data, str.c_str(), N - 1);
            m_data[N - 1] = '\0';
        }
        else
        {
            throw std::overflow_error("String is too large for the buffer");
        }
    }

    void append(const char* str)
    {
        if (str)
        {
            SizeType currentLength = length();
            SizeType strLength = std::strlen(str);
            if (currentLength + strLength < N)
            {
                std::strncat(m_data, str, N - currentLength - 1);
                m_data[N - 1] = '\0';  // Ensure null termination
            }
            else
            {
                throw std::overflow_error("String is too large for the buffer after append");
            }
        }
    }

    void append(const String& str)
    {
        append(str.c_str());
    }

    bool operator==(const CString& other) const
    {
        return std::strcmp(m_data, other.m_data) == 0;
    }

    bool operator!=(const CString& other) const
    {
        return !(*this == other);
    }

    bool operator<(const CString& other) const
    {
        return std::strcmp(m_data, other.m_data) < 0;
    }

    bool operator<=(const CString& other) const
    {
        return std::strcmp(m_data, other.m_data) <= 0;
    }

    bool operator>(const CString& other) const
    {
        return std::strcmp(m_data, other.m_data) > 0;
    }

    bool operator>=(const CString& other) const
    {
        return std::strcmp(m_data, other.m_data) >= 0;
    }

    CString& operator=(const char* str)
    {
        set(str);
        return *this;
    }

    CString& operator=(const String& str)
    {
        set(str);
        return *this;
    }

    CString& operator+=(const char* str)
    {
        append(str);
        return *this;
    }

    CString& operator+=(const String& str)
    {
        append(str);
        return *this;
    }

    CString substr(SizeType pos, SizeType len = String::npos) const
    {
        if (pos >= length())
        {
            // Return empty string if position is out of range
            return CString();
        }

        len = std::min(len, length() - pos);
        CString result;
        std::strncpy(result.m_data, m_data + pos, len);
        result.m_data[len] = '\0'; // Ensure null termination
        return result;
    }

    char* data()
    {
        return m_data;
    }

    const char* data() const
    {
        return m_data;
    }

    operator const char* () const
    {
        return c_str();
    }

private:
    char m_data[N];
};

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

    constexpr SizeType length() const noexcept
    {
        return m_size;
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

    /*constexpr*/ SizeType find_last_of(const char* chars) const noexcept
    {
        for (SizeType i = m_size; i > 0; --i) {
            if (std::strchr(chars, m_pData[i - 1])) {
                return i - 1;
            }
        }
        return npos;
    }

    /*constexpr*/ StringView substr(SizeType pos, SizeType count = npos) const noexcept
    {
        if (pos > m_size)
            return StringView();  // Out of bounds

        return StringView(m_pData + pos, (count > m_size - pos) ? (m_size - pos) : count);
    }

public:
    /*constexpr*/ int compare(const StringView& other) const noexcept
    {
        const SizeType min_len = m_size < other.m_size ? m_size : other.m_size;
        const int result = std::memcmp(m_pData, other.m_pData, min_len);

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
        return m_size == other.m_size && std::memcmp(m_pData, other.m_pData, m_size) == 0;
    }

    constexpr bool operator!=(const StringView& other) const noexcept
    {
        return !(*this == other);
    }

    static const SizeType npos = String::npos;

private:
    const char* m_pData;
    SizeType m_size;
};

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