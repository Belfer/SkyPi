#pragma once

#include <engine/byte_types.hpp>

#include <fmt/core.h>

#include <string>
#include <cstring>
#include <stdexcept>
#include <algorithm>

using String = std::string;

template <SizeType N>
class CString;

class StringView;

template <SizeType N>
class CString
{
public:
    CString();
    CString(const char* str);
    CString(const String& str);
    CString(const StringView& strView);

    const char* c_str() const;
    constexpr SizeType length() const;
    constexpr SizeType size() const;
    constexpr bool empty() const;
    void clear();

    template <typename... Args>
    void format(const char* fmt_str, Args&&... args);

    void set(const char* str);
    void set(const String& str);
    void append(const char* str);
    void append(const String& str);

    int compare(const char* str) const noexcept;
    int compare(const CString& other) const noexcept;
    int compare(const StringView& sv) const noexcept;

    bool starts_with(const char* prefix) const noexcept;
    bool ends_with(const char* suffix) const noexcept;

    constexpr bool operator==(const CString& other) const;
    constexpr bool operator!=(const CString& other) const;
    constexpr bool operator<(const CString& other) const;
    constexpr bool operator<=(const CString& other) const;
    constexpr bool operator>(const CString& other) const;
    constexpr bool operator>=(const CString& other) const;

    CString& operator=(const char* str);
    CString& operator=(const String& str);
    CString& operator+=(const char* str);
    CString& operator+=(const String& str);

    CString substr(SizeType pos, SizeType len = String::npos) const;

    char* data();
    const char* data() const;

    operator const char* () const;

private:
    char m_data[N];
};

class StringView
{
public:
    constexpr StringView() noexcept;
    constexpr StringView(const char* str) noexcept;
    constexpr StringView(const char* str, SizeType len) noexcept;

    template <SizeType N>
    StringView(const CString<N>& cstr) noexcept;

    StringView(const String& str) noexcept;

    constexpr const char& operator[](SizeType pos) const noexcept;
    constexpr const char* data() const noexcept;
    constexpr SizeType length() const noexcept;
    constexpr SizeType size() const noexcept;
    constexpr bool empty() const noexcept;

    String to_string() const;
    SizeType find_last_of(const char* chars) const noexcept;
    StringView substr(SizeType pos, SizeType count = String::npos) const noexcept;

    int compare(const StringView& other) const noexcept;
    constexpr bool operator==(const StringView& other) const noexcept;
    constexpr bool operator!=(const StringView& other) const noexcept;
    bool operator<(const StringView& other) const;
    bool operator<=(const StringView& other) const;
    bool operator>(const StringView& other) const;
    bool operator>=(const StringView& other) const;

private:
    const char* m_data;
    SizeType m_size;
};

// CString Implementation
template <SizeType N>
inline CString<N>::CString()
{
    m_data[0] = '\0';
}

template <SizeType N>
inline CString<N>::CString(const char* str)
{
    set(str);
}

template <SizeType N>
inline CString<N>::CString(const String& str)
{
    set(str);
}

template <SizeType N>
inline CString<N>::CString(const StringView& strView)
{
    set(String(strView.data(), strView.size()));
}

template <SizeType N>
inline const char* CString<N>::c_str() const
{
    return m_data;
}

template <SizeType N>
constexpr SizeType CString<N>::length() const
{
    return std::strlen(m_data);
}

template <SizeType N>
constexpr SizeType CString<N>::size() const
{
    return N;
}

template <SizeType N>
constexpr bool CString<N>::empty() const
{
    return m_data[0] == '\0';
}

template <SizeType N>
inline void CString<N>::clear()
{
    m_data[0] = '\0';
}

template <SizeType N>
template <typename... Args>
inline void CString<N>::format(const char* fmt_str, Args&&... args)
{
    SizeType required_size = fmt::formatted_size(fmt_str, std::forward<Args>(args)...);
    if (required_size >= N)
    {
        throw std::overflow_error("Formatted string is too large for the buffer");
    }
    auto result = fmt::format_to(m_data, fmt_str, std::forward<Args>(args)...);
    *result = '\0';
}

template <SizeType N>
inline void CString<N>::set(const char* str)
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

template <SizeType N>
inline void CString<N>::set(const String& str)
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

template <SizeType N>
inline void CString<N>::append(const char* str)
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

template <SizeType N>
inline void CString<N>::append(const String& str)
{
    append(str.c_str());
}

template <SizeType N>
inline int CString<N>::compare(const char* str) const noexcept
{
    return std::strcmp(m_data, str);
}

template <SizeType N>
inline int CString<N>::compare(const CString& other) const noexcept
{
    return compare(other.m_data);
}

template <SizeType N>
inline int CString<N>::compare(const StringView& sv) const noexcept
{
    return StringView(*this).compare(sv);
}

template <SizeType N>
bool CString<N>::starts_with(const char* prefix) const noexcept
{
    if (!prefix) return false;
    SizeType prefix_len = std::strlen(prefix);
    if (prefix_len > length()) return false;
    return std::strncmp(m_data, prefix, prefix_len) == 0;
}

template <SizeType N>
bool CString<N>::ends_with(const char* suffix) const noexcept
{
    if (!suffix) return false;
    SizeType suffix_len = std::strlen(suffix);
    SizeType str_len = length();
    if (suffix_len > str_len) return false;
    return std::strcmp(m_data + str_len - suffix_len, suffix) == 0;
}

template <SizeType N>
constexpr bool CString<N>::operator==(const CString& other) const
{
    return std::strcmp(m_data, other.m_data) == 0;
}

template <SizeType N>
constexpr bool CString<N>::operator!=(const CString& other) const
{
    return !(*this == other);
}

template <SizeType N>
constexpr bool CString<N>::operator<(const CString& other) const
{
    return std::strcmp(m_data, other.m_data) < 0;
}

template <SizeType N>
constexpr bool CString<N>::operator<=(const CString& other) const
{
    return std::strcmp(m_data, other.m_data) <= 0;
}

template <SizeType N>
constexpr bool CString<N>::operator>(const CString& other) const
{
    return std::strcmp(m_data, other.m_data) > 0;
}

template <SizeType N>
constexpr bool CString<N>::operator>=(const CString& other) const
{
    return std::strcmp(m_data, other.m_data) >= 0;
}

template <SizeType N>
inline CString<N>& CString<N>::operator=(const char* str)
{
    set(str);
    return *this;
}

template <SizeType N>
inline CString<N>& CString<N>::operator=(const String& str)
{
    set(str);
    return *this;
}

template <SizeType N>
inline CString<N>& CString<N>::operator+=(const char* str)
{
    append(str);
    return *this;
}

template <SizeType N>
inline CString<N>& CString<N>::operator+=(const String& str)
{
    append(str);
    return *this;
}

template <SizeType N>
inline CString<N> CString<N>::substr(SizeType pos, SizeType len) const
{
    if (pos >= length())
    {
        // Return an empty CString if the position is out of range
        return CString();
    }
    len = std::min(len, length() - pos);
    CString<N> result;
    std::strncpy(result.m_data, m_data + pos, len);
    result.m_data[len] = '\0'; // Ensure null termination
    return result;
}

template <SizeType N>
inline char* CString<N>::data() { return m_data; }

template <SizeType N>
inline const char* CString<N>::data() const { return m_data; }

template <SizeType N>
inline CString<N>::operator const char* () const { return m_data; }

// StringView Implementation
constexpr StringView::StringView() noexcept
    : m_data(nullptr)
    , m_size(0)
{}

constexpr StringView::StringView(const char* str) noexcept
    : m_data(str)
    , m_size(str ? std::strlen(str) : 0)
{}

constexpr StringView::StringView(const char* str, SizeType len) noexcept
    : m_data(str)
    , m_size(len)
{}

template <SizeType N>
inline StringView::StringView(const CString<N>& cstr) noexcept
    : m_data(cstr.data())
    , m_size(cstr.length())
{}

inline StringView::StringView(const String& str) noexcept
    : m_data(str.data())
    , m_size(str.size())
{}

constexpr const char& StringView::operator[](SizeType pos) const noexcept
{
    return m_data[pos];
}

constexpr const char* StringView::data() const noexcept { return m_data; }
constexpr SizeType StringView::length() const noexcept { return m_size; }
constexpr SizeType StringView::size() const noexcept { return m_size; }
constexpr bool StringView::empty() const noexcept { return m_size == 0; }

inline String StringView::to_string() const
{
    return String(m_data, m_size);
}

inline SizeType StringView::find_last_of(const char* chars) const noexcept
{
    for (SizeType i = m_size; i > 0; --i)
    {
        if (std::strchr(chars, m_data[i - 1]))
        {
            return i - 1;
        }
    }
    return String::npos;
}

inline StringView StringView::substr(SizeType pos, SizeType count) const noexcept
{
    if (pos > m_size)
    {
        return StringView(); // Out of bounds
    }
    return StringView(m_data + pos, std::min(count, m_size - pos));
}

inline int StringView::compare(const StringView& other) const noexcept
{
    const SizeType min_len = m_size < other.m_size ? m_size : other.m_size;
    const int result = std::memcmp(m_data, other.m_data, min_len);

    if (result != 0)
        return result;
    if (m_size < other.m_size)
        return -1;
    if (m_size > other.m_size)
        return 1;
    return 0;
}

constexpr bool StringView::operator==(const StringView& other) const noexcept
{
    return m_size == other.m_size && std::memcmp(m_data, other.m_data, m_size) == 0;
}

constexpr bool StringView::operator!=(const StringView& other) const noexcept
{
    return !(*this == other);
}

inline bool StringView::operator<(const StringView& other) const
{
    return std::strcmp(m_data, other.m_data) < 0;
}

inline bool StringView::operator<=(const StringView& other) const
{
    return std::strcmp(m_data, other.m_data) <= 0;
}

inline bool StringView::operator>(const StringView& other) const
{
    return std::strcmp(m_data, other.m_data) > 0;
}

inline bool StringView::operator>=(const StringView& other) const
{
    return std::strcmp(m_data, other.m_data) >= 0;
}

// fmt::formatter Implementation
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

/*
template <SizeType N>
class CString
{
public:
    // Constructors
    CString();
    CString(const char* str);
    CString(const String& str);
    CString(const StringView& strView);

    // Element Access
    const char* c_str() const;
    constexpr SizeType length() const;
    constexpr SizeType size() const;
    constexpr bool empty() const;
    void clear();

    // Formatting
    template <typename... Args>
    void format(const char* fmt_str, Args&&... args);

    // Modifiers
    void set(const char* str);
    void set(const String& str);
    void append(const char* str);
    void append(const String& str);

    // Comparison
    int compare(const char* str) const noexcept;
    int compare(const CString& other) const noexcept;
    int compare(const StringView& sv) const noexcept;

    // Prefix/Suffix checks
    bool starts_with(const char* prefix) const noexcept;
    bool ends_with(const char* suffix) const noexcept;

    // Operators
    constexpr bool operator==(const CString& other) const;
    constexpr bool operator!=(const CString& other) const;
    constexpr bool operator<(const CString& other) const;
    constexpr bool operator<=(const CString& other) const;
    constexpr bool operator>(const CString& other) const;
    constexpr bool operator>=(const CString& other) const;

    CString& operator=(const char* str);
    CString& operator=(const String& str);
    CString& operator+=(const char* str);
    CString& operator+=(const String& str);

    // Substrings
    CString substr(SizeType pos, SizeType len = String::npos) const;

    // Data Access
    char* data();
    const char* data() const;

    // Conversion to const char*
    operator const char* () const;

    // New functions from the spec
    using traits_type = std::char_traits<char>;
    using value_type = char;
    using allocator_type = std::allocator<char>;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;

    static constexpr size_type npos = static_cast<size_type>(-1);

    // Element Access
    reference at(SizeType pos);
    const_reference at(SizeType pos) const;
    reference operator[](SizeType pos);
    const_reference operator[](SizeType pos) const;
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    // Iterators
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;

    // Capacity
    constexpr bool empty() const;
    constexpr SizeType size() const;
    SizeType capacity() const;
    void reserve(SizeType n);
    void shrink_to_fit();

    // Modifiers
    void clear();
    void insert(SizeType pos, SizeType len, const char* str);
    void insert(SizeType pos, SizeType len, const CString& str);
    void erase(SizeType pos, SizeType len);
    void push_back(char ch);
    void pop_back();
    void append(const char* str);
    void append(const CString& str);
    void append(SizeType n, char ch);
    void replace(SizeType pos, SizeType len, const char* str);
    void replace(SizeType pos, SizeType len, const CString& str);
    void replace(SizeType pos, SizeType len, SizeType replace_len, char replace_char);
    void swap(CString& other);

    // Search
    SizeType find(const char* str) const noexcept;
    SizeType find(const CString& str) const noexcept;
    SizeType find(const StringView& sv) const noexcept;
    SizeType find_first_of(const char* str) const noexcept;
    SizeType find_first_of(const CString& str) const noexcept;
    SizeType find_first_not_of(const char* str) const noexcept;
    SizeType find_first_not_of(const CString& str) const noexcept;
    SizeType find_last_of(const char* str) const noexcept;
    SizeType find_last_of(const CString& str) const noexcept;
    SizeType find_last_not_of(const char* str) const noexcept;
    SizeType find_last_not_of(const CString& str) const noexcept;

    // Conversion
    int stoi() const;
    long stol() const;
    long long stoll() const;
    unsigned long stoul() const;
    unsigned long long stoull() const;
    float stof() const;
    double stod() const;
    long double stold() const;
    CString to_string() const;
    CString to_wstring() const;

    // Non-member functions
    friend std::ostream& operator<<(std::ostream& os, const CString& str);
    friend std::istream& operator>>(std::istream& is, CString& str);

private:
    char m_data[N];
};

// Function Implementations

template <SizeType N>
CString<N>::CString() {
    m_data[0] = '\0';
}

template <SizeType N>
CString<N>::CString(const char* str) {
    std::strncpy(m_data, str, N - 1);
    m_data[N - 1] = '\0';
}

template <SizeType N>
CString<N>::CString(const String& str) {
    std::strncpy(m_data, str.c_str(), N - 1);
    m_data[N - 1] = '\0';
}

template <SizeType N>
CString<N>::CString(const StringView& strView) {
    std::strncpy(m_data, strView.data(), N - 1);
    m_data[N - 1] = '\0';
}

template <SizeType N>
const char* CString<N>::c_str() const {
    return m_data;
}

template <SizeType N>
constexpr SizeType CString<N>::length() const {
    return std::strlen(m_data);
}

template <SizeType N>
constexpr SizeType CString<N>::size() const {
    return N;
}

template <SizeType N>
constexpr bool CString<N>::empty() const {
    return m_data[0] == '\0';
}

template <SizeType N>
void CString<N>::clear() {
    m_data[0] = '\0';
}

template <SizeType N>
template <typename... Args>
void CString<N>::format(const char* fmt_str, Args&&... args) {
    std::snprintf(m_data, N, fmt_str, std::forward<Args>(args)...);
}

template <SizeType N>
void CString<N>::set(const char* str) {
    std::strncpy(m_data, str, N - 1);
    m_data[N - 1] = '\0';
}

template <SizeType N>
void CString<N>::set(const String& str) {
    std::strncpy(m_data, str.c_str(), N - 1);
    m_data[N - 1] = '\0';
}

template <SizeType N>
void CString<N>::append(const char* str) {
    std::strncat(m_data, str, N - std::strlen(m_data) - 1);
}

template <SizeType N>
void CString<N>::append(const String& str) {
    std::strncat(m_data, str.c_str(), N - std::strlen(m_data) - 1);
}

template <SizeType N>
int CString<N>::compare(const char* str) const noexcept {
    return std::strcmp(m_data, str);
}

template <SizeType N>
int CString<N>::compare(const CString& other) const noexcept {
    return std::strcmp(m_data, other.m_data);
}

template <SizeType N>
int CString<N>::compare(const StringView& sv) const noexcept {
    return std::strncmp(m_data, sv.data(), sv.size());
}

template <SizeType N>
bool CString<N>::starts_with(const char* prefix) const noexcept {
    return std::strncmp(m_data, prefix, std::strlen(prefix)) == 0;
}

template <SizeType N>
bool CString<N>::ends_with(const char* suffix) const noexcept {
    SizeType len = std::strlen(m_data);
    SizeType suffix_len = std::strlen(suffix);
    return len >= suffix_len && std::strcmp(m_data + len - suffix_len, suffix) == 0;
}

template <SizeType N>
constexpr bool CString<N>::operator==(const CString& other) const {
    return std::strcmp(m_data, other.m_data) == 0;
}

template <SizeType N>
constexpr bool CString<N>::operator!=(const CString& other) const {
    return !(*this == other);
}

template <SizeType N>
constexpr bool CString<N>::operator<(const CString& other) const {
    return std::strcmp(m_data, other.m_data) < 0;
}

template <SizeType N>
constexpr bool CString<N>::operator<=(const CString& other) const {
    return std::strcmp(m_data, other.m_data) <= 0;
}

template <SizeType N>
constexpr bool CString<N>::operator>(const CString& other) const {
    return std::strcmp(m_data, other.m_data) > 0;
}

template <SizeType N>
constexpr bool CString<N>::operator>=(const CString& other) const {
    return std::strcmp(m_data, other.m_data) >= 0;
}

template <SizeType N>
CString<N>& CString<N>::operator=(const char* str) {
    set(str);
    return *this;
}

template <SizeType N>
CString<N>& CString<N>::operator=(const String& str) {
    set(str);
    return *this;
}

template <SizeType N>
CString<N>& CString<N>::operator+=(const char* str) {
    append(str);
    return *this;
}

template <SizeType N>
CString<N>& CString<N>::operator+=(const String& str) {
    append(str);
    return *this;
}

template <SizeType N>
CString<N> CString<N>::substr(SizeType pos, SizeType len) const {
    if (pos >= length()) {
        return CString();
    }
    if (len == String::npos || pos + len > length()) {
        len = length() - pos;
    }
    CString<N> result;
    std::strncpy(result.m_data, m_data + pos, len);
    result.m_data[len] = '\0';
    return result;
}

template <SizeType N>
char* CString<N>::data() {
    return m_data;
}

template <SizeType N>
const char* CString<N>::data() const {
    return m_data;
}

template <SizeType N>
CString<N>::operator const char* () const {
    return m_data;
}

template <SizeType N>
typename CString<N>::reference CString<N>::at(SizeType pos) {
    if (pos >= size()) {
        throw std::out_of_range("CString::at: position out of range");
    }
    return m_data[pos];
}

template <SizeType N>
typename CString<N>::const_reference CString<N>::at(SizeType pos) const {
    if (pos >= size()) {
        throw std::out_of_range("CString::at: position out of range");
    }
    return m_data[pos];
}

template <SizeType N>
typename CString<N>::reference CString<N>::operator[](SizeType pos) {
    return m_data[pos];
}

template <SizeType N>
typename CString<N>::const_reference CString<N>::operator[](SizeType pos) const {
    return m_data[pos];
}

template <SizeType N>
typename CString<N>::reference CString<N>::front() {
    return m_data[0];
}

template <SizeType N>
typename CString<N>::const_reference CString<N>::front() const {
    return m_data[0];
}

template <SizeType N>
typename CString<N>::reference CString<N>::back() {
    return m_data[length() - 1];
}

template <SizeType N>
typename CString<N>::const_reference CString<N>::back() const {
    return m_data[length() - 1];
}

template <SizeType N>
typename CString<N>::iterator CString<N>::begin() {
    return m_data;
}

template <SizeType N>
typename CString<N>::const_iterator CString<N>::begin() const {
    return m_data;
}

template <SizeType N>
typename CString<N>::iterator CString<N>::end() {
    return m_data + length();
}

template <SizeType N>
typename CString<N>::const_iterator CString<N>::end() const {
    return m_data + length();
}

template <SizeType N>
typename CString<N>::reverse_iterator CString<N>::rbegin() {
    return reverse_iterator(end());
}

template <SizeType N>
typename CString<N>::const_reverse_iterator CString<N>::rbegin() const {
    return const_reverse_iterator(end());
}

template <SizeType N>
typename CString<N>::reverse_iterator CString<N>::rend() {
    return reverse_iterator(begin());
}

template <SizeType N>
typename CString<N>::const_reverse_iterator CString<N>::rend() const {
    return const_reverse_iterator(begin());
}

template <SizeType N>
constexpr bool CString<N>::empty() const {
    return m_data[0] == '\0';
}

template <SizeType N>
constexpr SizeType CString<N>::size() const {
    return N;
}

template <SizeType N>
SizeType CString<N>::capacity() const {
    return N;
}

template <SizeType N>
void CString<N>::reserve(SizeType n) {
    // No-op, since we don't allocate dynamic memory
}

template <SizeType N>
void CString<N>::shrink_to_fit() {
    // No-op, since we don't allocate dynamic memory
}

template <SizeType N>
void CString<N>::insert(SizeType pos, SizeType len, const char* str) {
    if (pos > length()) {
        throw std::out_of_range("CString::insert: position out of range");
    }
    SizeType copy_len = std::min(len, N - length() - 1);
    std::memmove(m_data + pos + copy_len, m_data + pos, length() - pos + 1);
    std::memcpy(m_data + pos, str, copy_len);
}

template <SizeType N>
void CString<N>::insert(SizeType pos, SizeType len, const CString& str) {
    insert(pos, len, str.c_str());
}

template <SizeType N>
void CString<N>::erase(SizeType pos, SizeType len) {
    if (pos >= length()) {
        throw std::out_of_range("CString::erase: position out of range");
    }
    std::memmove(m_data + pos, m_data + pos + len, length() - pos - len + 1);
}

template <SizeType N>
void CString<N>::push_back(char ch) {
    if (length() < N - 1) {
        m_data[length()] = ch;
        m_data[length() + 1] = '\0';
    }
}

template <SizeType N>
void CString<N>::pop_back() {
    if (length() > 0) {
        m_data[length() - 1] = '\0';
    }
}

template <SizeType N>
void CString<N>::replace(SizeType pos, SizeType len, const char* str) {
    if (pos >= length()) {
        throw std::out_of_range("CString::replace: position out of range");
    }
    SizeType copy_len = std::min(len, N - length() - 1);
    std::memmove(m_data + pos + copy_len, m_data + pos, length() - pos + 1);
    std::memcpy(m_data + pos, str, copy_len);
}

template <SizeType N>
void CString<N>::replace(SizeType pos, SizeType len, const CString& str) {
    replace(pos, len, str.c_str());
}

template <SizeType N>
void CString<N>::replace(SizeType pos, SizeType len, SizeType replace_len, char replace_char) {
    if (pos >= length()) {
        throw std::out_of_range("CString::replace: position out of range");
    }
    std::memset(m_data + pos, replace_char, replace_len);
}

template <SizeType N>
void CString<N>::swap(CString& other) {
    std::swap_ranges(m_data, m_data + N, other.m_data);
}

template <SizeType N>
SizeType CString<N>::find(const char* str) const noexcept {
    const char* found = std::strstr(m_data, str);
    return found ? found - m_data : npos;
}

template <SizeType N>
SizeType CString<N>::find(const CString& str) const noexcept {
    return find(str.c_str());
}

template <SizeType N>
SizeType CString<N>::find(const StringView& sv) const noexcept {
    return find(sv.data());
}

template <SizeType N>
SizeType CString<N>::find_first_of(const char* str) const noexcept {
    for (SizeType i = 0; i < length(); ++i) {
        if (std::strchr(str, m_data[i])) {
            return i;
        }
    }
    return npos;
}

template <SizeType N>
SizeType CString<N>::find_first_of(const CString& str) const noexcept {
    return find_first_of(str.c_str());
}

template <SizeType N>
SizeType CString<N>::find_first_not_of(const char* str) const noexcept {
    for (SizeType i = 0; i < length(); ++i) {
        if (!std::strchr(str, m_data[i])) {
            return i;
        }
    }
    return npos;
}

template <SizeType N>
SizeType CString<N>::find_first_not_of(const CString& str) const noexcept {
    return find_first_not_of(str.c_str());
}

template <SizeType N>
SizeType CString<N>::find_last_of(const char* str) const noexcept {
    for (SizeType i = length(); i > 0; --i) {
        if (std::strchr(str, m_data[i - 1])) {
            return i - 1;
        }
    }
    return npos;
}

template <SizeType N>
SizeType CString<N>::find_last_of(const CString& str) const noexcept {
    return find_last_of(str.c_str());
}

template <SizeType N>
SizeType CString<N>::find_last_not_of(const char* str) const noexcept {
    for (SizeType i = length(); i > 0; --i) {
        if (!std::strchr(str, m_data[i - 1])) {
            return i - 1;
        }
    }
    return npos;
}

template <SizeType N>
SizeType CString<N>::find_last_not_of(const CString& str) const noexcept {
    return find_last_not_of(str.c_str());
}

template <SizeType N>
int CString<N>::stoi() const {
    return std::stoi(m_data);
}

template <SizeType N>
long CString<N>::stol() const {
    return std::stol(m_data);
}

template <SizeType N>
long long CString<N>::stoll() const {
    return std::stoll(m_data);
}

template <SizeType N>
unsigned long CString<N>::stoul() const {
    return std::stoul(m_data);
}

template <SizeType N>
unsigned long long CString<N>::stoull() const {
    return std::stoull(m_data);
}

template <SizeType N>
float CString<N>::stof() const {
    return std::stof(m_data);
}

template <SizeType N>
double CString<N>::stod() const {
    return std::stod(m_data);
}

template <SizeType N>
long double CString<N>::stold() const {
    return std::stold(m_data);
}

template <SizeType N>
CString<N> CString<N>::to_string() const {
    return CString<N>(std::to_string(stoi()).c_str());
}

template <SizeType N>
CString<N> CString<N>::to_wstring() const {
    return CString<N>(std::wstring(m_data).c_str());
}

template <SizeType N>
std::ostream& operator<<(std::ostream& os, const CString<N>& str) {
    return os << str.c_str();
}

template <SizeType N>
std::istream& operator>>(std::istream& is, CString<N>& str) {
    is >> str.m_data;
    return is;
}

*/