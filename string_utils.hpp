// C++ String Utils
//
// C++17 and C++11 std::string_view-based and UTF-8-based utils.
//
// Author: Yurii Blok
// License: BSL-1.0
// https://github.com/yurablok/cpp-string-utils
// History:
// v0.7 2024-May-12     Added `u8`,`u16`,`u32`... `string` and `string_view`.
// v0.6 2023-Apr-26     Fixed build with clang-cl. Fixed `substr` [2].
// v0.5 2023-Feb-14     Fixed `substr`.
// v0.4 2023-Feb-09     Added `checked_string_view`.
// v0.3 2023-Feb-01     `from_string` now checks for a null string.
// v0.2 2022-Dec-23     Added `hex` option into `to_string` and `from_string`.
// v0.1 2022-Dec-23     First release.

#pragma once
#ifndef CPP_STRING_UTILS
#define CPP_STRING_UTILS

#include <string>
#include <functional>
#include <cmath>

#if defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#   define CPP_STRING_UTILS_LIB_CHARCONV
#   define CPP_STRING_UTILS_LIB_CHARCONV_FLOAT
#elif __cplusplus >= 201703L
#   if defined(__GNUG__) && !defined(__llvm__)
#       if __GNUC__ >= 8 && __GNUC_MINOR__ >= 1
#           define CPP_STRING_UTILS_LIB_CHARCONV
#           if defined(__cpp_lib_to_chars) || defined(_GLIBCXX_HAVE_USELOCALE)
#               define CPP_STRING_UTILS_LIB_CHARCONV_FLOAT
#           endif
#       endif
#   else
#       define CPP_STRING_UTILS_LIB_CHARCONV
#       define CPP_STRING_UTILS_LIB_CHARCONV_FLOAT
#   endif
#   ifndef _CONSTEXPR17
#       define _CONSTEXPR17 constexpr
#   endif
#else
#   ifndef _CONSTEXPR17
#       define _CONSTEXPR17 inline
#   endif
#endif

#if defined(CPP_STRING_UTILS_LIB_CHARCONV)
#   include <string_view>
#   include <charconv>
#else
#   include "string_view.hpp" // https://github.com/martinmoene/string-view-lite
namespace std {
    using string_view = nonstd::string_view;
    using wstring_view = nonstd::wstring_view;
    using u16string_view = nonstd::u16string_view;
    using u32string_view = nonstd::u32string_view;
}
inline std::string& operator+=(std::string& a, const std::string_view b) {
    a.insert(a.end(), b.cbegin(), b.cend());
    return a;
}
#endif
#if !defined(CPP_STRING_UTILS_LIB_CHARCONV_FLOAT)
#   define __STDC_FORMAT_MACROS
#   include <cinttypes>
#endif

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4127)
#   pragma warning(disable: 4244)
#endif // _MSC_VER
#ifndef TINY_UTF8_NOEXCEPT
#   define TINY_UTF8_NOEXCEPT
#endif
#include "tinyutf8.h" // https://github.com/DuffsDevice/tiny-utf8
#ifdef _MSC_VER
#   pragma warning(pop)
#endif // _MSC_VER

static_assert(static_cast<uint8_t>(u8"🌍"[0]) == 0xF0, "Wrong UTF-8 config");
static_assert(static_cast<uint8_t>(u8"🌍"[1]) == 0x9F, "Wrong UTF-8 config");
static_assert(static_cast<uint8_t>(u8"🌍"[2]) == 0x8C, "Wrong UTF-8 config");
static_assert(static_cast<uint8_t>(u8"🌍"[3]) == 0x8D, "Wrong UTF-8 config");


#if defined(__cpp_char8_t)
static_assert(sizeof(char) >= sizeof(char8_t), "");
#endif

class u8string;
class u16string;
class u32string;


class u8string_view : public std::string_view {
public:
    constexpr u8string_view() noexcept = default;
    template <typename ch_t>
    constexpr u8string_view(ch_t str) noexcept
        : std::string_view(str == nullptr ? "" : reinterpret_cast<const char*>(str)) {}
    template <typename ch_t>
    constexpr u8string_view(ch_t str, size_t size) noexcept
        : std::string_view(str == nullptr ? "" : reinterpret_cast<const char*>(str), size) {}
    template <size_t size>
    constexpr u8string_view(const char (&str)[size]) noexcept
        : std::string_view(str, size - 1) {}
    _CONSTEXPR17 u8string_view(const std::string& str) noexcept;
    constexpr u8string_view(const std::string_view& str) noexcept;
    inline u8string_view(const u8string& str) noexcept;
    constexpr u8string_view(const u8string_view& str) noexcept = default;
# if defined(__cpp_char8_t)
    constexpr u8string_view(const std::u8string& str) noexcept;
    constexpr u8string_view(const std::u8string_view& str) noexcept;
    template <size_t size>
    constexpr u8string_view(const char8_t (&str)[size]) noexcept
        : std::string_view(reinterpret_cast<const char*>(str), size - 1) {}
# endif

    template <typename ch_t>
    constexpr bool operator==(ch_t str) const noexcept {
        return compare(str == nullptr ? "" : reinterpret_cast<const char*>(str)) == 0;
    }
    template <size_t size>
    constexpr bool operator==(const char (&str)[size]) const noexcept {
        return compare(u8string_view(str, size - 1)) == 0;
    }
    _CONSTEXPR17 bool operator==(const std::string& str) const noexcept;
    _CONSTEXPR17 bool operator==(const std::string_view& str) const noexcept;
    inline bool operator==(const u8string& str) const noexcept;
    _CONSTEXPR17 bool operator==(const u8string_view& str) const noexcept;
# if defined(__cpp_char8_t)
    inline bool operator==(const std::u8string& str) const noexcept;
    constexpr bool operator==(const std::u8string_view& str) const noexcept;
    template <size_t size>
    constexpr bool operator==(const char8_t (&str)[size]) const noexcept {
        return compare(u8string_view(str, size - 1)) == 0;
    }
# endif
    template <typename str_t>
    inline bool operator!=(str_t str) const noexcept {
        return !operator==(str);
    }

    // Little-endian
    inline u16string toUtf16(const bool strict = true) const;
    // Little-endian
    inline u32string toUtf32() const;
}; // class u8string_view

class u8string : public tiny_utf8::string {
public:
    inline u8string() = default;

    template <typename ch_t>
    inline u8string(ch_t str) noexcept
        : tiny_utf8::string(str == nullptr ? "" : reinterpret_cast<const char*>(str)) {}

    template <typename ch_t>
    inline u8string(ch_t str, size_t size, enable_if_ptr<ch_t, char>* = {}) noexcept
        : tiny_utf8::string(str == nullptr ? "" : reinterpret_cast<const char*>(str), size) {}

    template <size_t size>
    inline u8string(const char (&str)[size]) noexcept
        : tiny_utf8::string(str, size - 1) {}

    inline u8string(const std::string& str) noexcept;
    inline u8string(const std::string_view& str) noexcept;
    inline u8string(const u8string& str) noexcept = default;
    inline u8string(const u8string_view& str) noexcept;
    inline u8string(const size_t count, const char ch) noexcept;
# if defined(__cpp_char8_t)
    inline u8string(const std::u8string& str) noexcept;
    inline u8string(const std::u8string_view& str) noexcept;
    template <typename ch_t>
    inline u8string(ch_t str, size_t size, enable_if_ptr<ch_t, char8_t>* = {}) noexcept
        : tiny_utf8::string(str == nullptr ? "" : reinterpret_cast<const char*>(str), size) {}
    template <size_t size>
    inline u8string(const char8_t (&str)[size]) noexcept
        : tiny_utf8::string(reinterpret_cast<const char*>(str), size - 1) {}
# endif

    template <typename ch_t>
    inline bool operator==(ch_t str) const noexcept {
        return compare(str == nullptr ? "" : reinterpret_cast<const char*>(str)) == 0;
    }
    template <size_t size>
    inline bool operator==(const char (&str)[size]) const noexcept {
        return u8string_view(*this) == u8string_view(str, size - 1);
    }
    inline bool operator==(const std::string& str) const noexcept;
    inline bool operator==(const std::string_view& str) const noexcept;
    inline bool operator==(const u8string& str) const noexcept;
    inline bool operator==(const u8string_view& str) const noexcept;
# if defined(__cpp_char8_t)
    template <size_t size>
    inline bool operator==(const char8_t (&str)[size]) const noexcept {
        return u8string_view(*this) == u8string_view(str, size - 1);
    }
    inline bool operator==(const std::u8string& str) const noexcept;
    inline bool operator==(const std::u8string_view& str) const noexcept;
# endif
    template <typename str_t>
    inline bool operator!=(str_t str) const noexcept {
        return !operator==(str);
    }

    template <typename ch_t>
    inline u8string& operator+=(ch_t str) noexcept {
        if (str != nullptr) {
            append(reinterpret_cast<const char*>(str));
        }
        return *this;
    }
    template <size_t size>
    inline u8string& operator+=(const char (&str)[size]) noexcept { 
        append(std::string(str, size - 1)); //TODO: Extra copy
        return *this;
    }
    inline u8string& operator+=(const std::string& str) noexcept;
    inline u8string& operator+=(const std::string_view& str) noexcept;
    inline u8string& operator+=(const u8string& str) noexcept;
    inline u8string& operator+=(const u8string_view& str) noexcept;
# if defined(__cpp_char8_t)
    inline u8string& operator+=(const std::u8string& str) noexcept;
    inline u8string& operator+=(const std::u8string_view& str) noexcept;
    template <size_t size>
    inline u8string& operator+=(const char8_t (&str)[size]) noexcept {
        append(std::string(reinterpret_cast<const char*>(str), size - 1)); //TODO: Extra copy
        return *this;
    }
# endif

    // Little-endian
    inline u16string toUtf16(const bool strict = true) const;
    // Little-endian
    inline u32string toUtf32() const;
}; // class u8string


class u16string_view
# if WCHAR_MAX < 0x10000
    : public std::wstring_view {
public:
    using std::wstring_view::basic_string_view;
# else
    : public std::u16string_view {
public:
    using std::u16string_view::basic_string_view;
# endif
    inline u16string_view(const u16string& str) noexcept;
    inline u8string toUtf8(const bool strict = true) const;
    // Little-endian
    inline u32string toUtf32(const bool strict = true) const;
};


class u16string
# if WCHAR_MAX < 0x10000
    : public std::wstring {
# else
    : public std::u16string {
# endif
public:
    inline u8string toUtf8(const bool strict = true) const;
    // Little-endian
    inline u32string toUtf32(const bool strict = true) const;
};


class u32string_view
# if WCHAR_MAX < 0x10000
    : public std::u32string_view {
public:
    using std::u32string_view::basic_string_view;
# else
    : public std::wstring_view {
public:
    using std::wstring_view::basic_string_view;
# endif
    inline u32string_view(const u32string& str) noexcept;
    inline u8string toUtf8(const bool strict = true) const;
    // Little-endian
    inline u16string toUtf16(const bool strict = true) const;
};


class u32string
# if WCHAR_MAX < 0x10000
    : public std::u32string {
# else
    : public std::wstring {
# endif
public:
    inline u8string toUtf8(const bool strict = true) const;
    // Little-endian
    inline u16string toUtf16(const bool strict = true) const;
};


namespace utils {


inline u8string_view trimm(u8string_view string,
        const std::string_view by = std::string_view("\t\n\r \0", 5)) noexcept {
    while (!string.empty()) {
        if (by.find(string.front()) == std::string_view::npos) {
            break;
        }
        string = string.substr(1);
    }
    while (!string.empty()) {
        if (by.find(string.back()) == std::string_view::npos) {
            break;
        }
        string = string.substr(0, string.size() - 1);
    }
    return string;
}

inline void split(const u8string_view str, const std::string_view by,
        const std::function<void(u8string_view part, uint32_t idx)> handler,
        const bool withEmpty = false, const char escape = '\\') noexcept {
    if (by.empty() || !handler) {
        return;
    }
    size_t begin = 0;
    bool isPrevEscape = false;
    uint32_t idx = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (!isPrevEscape) {
            if (str[i] == escape) {
                isPrevEscape = true;
                continue;
            }
        }
        else {
            isPrevEscape = false;
            continue;
        }
        if (by.find(str[i]) == std::string_view::npos) {
            continue;
        }
        const u8string_view part = str.substr(begin, i - begin);
        if (withEmpty || !part.empty()) {
            handler(part, idx++);
        }
        begin = i + 1;
    }
    const u8string_view part = str.substr(begin);
    if (!part.empty()) {
        handler(part, idx);
    }
}

inline u8string_view substr(const u8string_view str, size_t& offset,
        const std::string_view split_by,
        const bool withEmpty = false, const char escape = '\\') noexcept {
    if (split_by.empty()) {
        return {};
    }
    if (offset >= str.size()) {
        return {};
    }
    size_t begin = offset;
    bool isPrevEscape = false;
    for (; offset < str.size(); ++offset) {
        if (!isPrevEscape) {
            if (str[offset] == escape) {
                isPrevEscape = true;
                continue;
            }
        }
        else {
            isPrevEscape = false;
            continue;
        }
        if (split_by.find(str[offset]) == std::string_view::npos) {
            continue;
        }
        const u8string_view part = str.substr(begin, offset - begin);
        if (withEmpty || !part.empty()) {
            ++offset;
            return part;
        }
        begin = offset + 1;
    }
    const u8string_view part = str.substr(begin);
    ++offset;
    if (!part.empty()) {
        return part;
    }
    return {};
}

inline void parseCSV(const u8string_view csv,
        const std::function<void(u8string_view cell, uint32_t idx)> onCell,
        const std::function<void()> onEndl = nullptr) {
    if (!onCell) {
        return;
    }
    std::string cell;
    bool isString = false;
    bool isPrevQuotes = false;
    bool isPrevEndl = false;
    uint32_t idx = 0;
    for (const char c : csv) {
        if (isString) {
            switch (c) {
            case '"':
                isString = false;
                isPrevQuotes = true;
                break;
            case ',':
            case '\n':
            case '\r':
            default:
                cell.push_back(c);
                break;
            }
        }
        else {
            switch (c) {
            case '"':
                if (isPrevQuotes) {
                    isPrevQuotes = false;
                    cell.push_back('"');
                }
                isString = true;
                break;
            case ',':
                onCell(cell, idx);
                cell.clear();
                ++idx;
                break;
            case 0:
            case '\n':
            case '\r':
                if (!cell.empty()) {
                    onCell(cell, idx);
                    cell.clear();
                }
                if (!isPrevEndl && onEndl) {
                    isPrevEndl = true;
                    onEndl();
                }
                idx = 0;
                break;
            default:
                cell.push_back(c);
                break;
            }
            if (c != '"') {
                isPrevQuotes = false;
            }
            if (c != 0 && c != '\n' && c != '\r') {
                isPrevEndl = false;
            }
        }
    }
    if (!cell.empty()) {
        onCell(cell, idx);
        cell.clear();
    }
    if (!isPrevEndl && onEndl) {
        onEndl();
    }
}

#if defined(CPP_STRING_UTILS_LIB_CHARCONV)

template<typename integer_t,
    typename std::enable_if<
        std::is_integral<integer_t>::value, bool
    >::type = true>
inline u8string_view to_string(const integer_t number, const u8string_view buffer,
        const bool hex = false) noexcept {
    const auto [ptr, ec] = std::to_chars(
        const_cast<char*>(buffer.data()),
        const_cast<char*>(buffer.data() + buffer.size()),
        number,
        hex ? 16 : 10
    );
    if (ec != std::errc(0)) {
        return {};
    }
    return buffer.substr(0, ptr - buffer.data());
}

#else // !CPP_STRING_UTILS_LIB_CHARCONV

inline u8string_view to_string(const int8_t number, const u8string_view buffer) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), "%" PRIi8, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline u8string_view to_string(const uint8_t number, const u8string_view buffer,
        const bool hex = false) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), hex ? "%" PRIx8 : "%" PRIu8, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline u8string_view to_string(const int16_t number, const u8string_view buffer) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), "%" PRIi16, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline u8string_view to_string(const uint16_t number, const u8string_view buffer,
        const bool hex = false) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), hex ? "%" PRIx16 : "%" PRIu16, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline u8string_view to_string(const int32_t number, const u8string_view buffer) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), "%" PRIi32, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline u8string_view to_string(const uint32_t number, const u8string_view buffer,
        const bool hex = false) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), hex ? "%" PRIx32 : "%" PRIu32, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline u8string_view to_string(const int64_t number, const u8string_view buffer) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), "%" PRIi64, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline u8string_view to_string(const uint64_t number, const u8string_view buffer,
        const bool hex = false) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), hex ? "%" PRIx64 : "%" PRIu64, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}

#endif // CPP_STRING_UTILS_LIB_CHARCONV

#if defined(CPP_STRING_UTILS_LIB_CHARCONV_FLOAT)

template<typename floating_t,
    typename std::enable_if<
        std::is_floating_point<floating_t>::value, bool
    >::type = true,
    typename _ = bool>
inline u8string_view to_string(const floating_t number, const u8string_view buffer) noexcept {
    const auto [ptr, ec] = std::to_chars(
        const_cast<char*>(buffer.data()),
        const_cast<char*>(buffer.data() + buffer.size()),
        number
    );
    if (ec != std::errc(0)) {
        return {};
    }
    return buffer.substr(0, ptr - buffer.data());
}

#else // !CPP_STRING_UTILS_LIB_CHARCONV_FLOAT

inline u8string_view to_string(const float number, const u8string_view buffer) noexcept {
    const int32_t length = std::trunc(number) == number
        ? std::snprintf(const_cast<char*>(
            buffer.data()), buffer.size(), "%.0f", number)
        : std::snprintf(const_cast<char*>(
            buffer.data()), buffer.size(), "%.6f", number);
    if (length <= 0) {
        return {};
    }
    return trimm(buffer.substr(0, length), "0\0");
}
inline u8string_view to_string(const double number, const u8string_view buffer) noexcept {
    const int32_t length = std::trunc(number) == number
        ? std::snprintf(const_cast<char*>(
            buffer.data()), buffer.size(), "%.0f", number)
        : std::snprintf(const_cast<char*>(
            buffer.data()), buffer.size(), "%.8f", number);
    if (length <= 0) {
        return {};
    }
    return trimm(buffer.substr(0, length), "0\0");
}

#endif // CPP_STRING_UTILS_LIB_CHARCONV_FLOAT



#if defined(CPP_STRING_UTILS_LIB_CHARCONV)

template<typename integer_t,
    typename std::enable_if_t<std::is_integral_v<integer_t>, bool> = true>
inline bool from_string(const u8string_view string, integer_t& number,
        const bool hex = false) noexcept {
    auto [ptr, ec] = std::from_chars(
        string.data(),
        string.data() + string.size(),
        number,
        hex ? 16 : 10
    );
    if (ptr != reinterpret_cast<const char*>(string.data()) + string.size()
            || ec != std::errc(0)) {
        return false;
    }
    return true;
}

#else // !CPP_STRING_UTILS_LIB_CHARCONV

inline bool from_string(const u8string_view string, int8_t& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%u" SCNi8,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const u8string_view string, uint8_t& number,
        const bool hex = false) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), hex ? "%%%u" SCNx8 : "%%%u" SCNu8,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const u8string_view string, int16_t& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%u" SCNi16,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const u8string_view string, uint16_t& number,
        const bool hex = false) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), hex ? "%%%u" SCNx16 : "%%%u" SCNu16,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const u8string_view string, int32_t& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%u" SCNi32,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const u8string_view string, uint32_t& number,
        const bool hex = false) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), hex ? "%%%u" SCNx32 : "%%%u" SCNu32,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const u8string_view string, int64_t& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%u" SCNi64,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const u8string_view string, uint64_t& number,
        const bool hex = false) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), hex ? "%%%u" SCNx64 : "%%%u" SCNu64,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}

#endif // CPP_STRING_UTILS_LIB_CHARCONV

#if defined(CPP_STRING_UTILS_LIB_CHARCONV_FLOAT)

template<typename floating_t,
    typename std::enable_if_t<std::is_floating_point_v<floating_t>, bool> = true>
inline bool from_string(const u8string_view string, floating_t& number) noexcept {
    auto [ptr, ec] = std::from_chars(
        string.data(),
        string.data() + string.size(),
        number
    );
    if (ptr != string.data() + string.size() || ec != std::errc(0)) {
        return false;
    }
    return true;
}

#else // !CPP_STRING_UTILS_LIB_CHARCONV_FLOAT

inline bool from_string(const u8string_view string, float& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%uf",
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const u8string_view string, double& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%ulf",
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}

#endif // CPP_STRING_UTILS_LIB_CHARCONV_FLOAT

} // namespace utils




_CONSTEXPR17 u8string_view::u8string_view(const std::string& str) noexcept
    : std::string_view(str.data(), str.size()) {}

constexpr u8string_view::u8string_view(const std::string_view& str) noexcept
    : std::string_view(str) {}

inline u8string_view::u8string_view(const u8string& str) noexcept
    : std::string_view(str.data(), str.size()) {}

#if defined(__cpp_char8_t)
constexpr u8string_view::u8string_view(const std::u8string& str) noexcept
    : std::string_view(str.empty() ? "" : reinterpret_cast<const char*>(str.data()), str.size()) {}

constexpr u8string_view::u8string_view(const std::u8string_view& str) noexcept
    : std::string_view(str.empty() ? "" : reinterpret_cast<const char*>(str.data()), str.size()) {}
#endif


_CONSTEXPR17 bool u8string_view::operator==(const std::string& str) const noexcept {
    return compare(str) == 0;
}
_CONSTEXPR17 bool u8string_view::operator==(const std::string_view& str) const noexcept {
    return compare(str) == 0;
}
inline bool u8string_view::operator==(const u8string& str) const noexcept {
    return compare(u8string_view(str)) == 0;
}
_CONSTEXPR17 bool u8string_view::operator==(const u8string_view& str) const noexcept {
    return compare(str) == 0;
}

#if defined(__cpp_char8_t)
inline bool u8string_view::operator==(const std::u8string& str) const noexcept {
    return compare(reinterpret_cast<const char*>(str.c_str())) == 0;
}
constexpr bool u8string_view::operator==(const std::u8string_view& str) const noexcept {
    return !str.empty() && compare(reinterpret_cast<const char*>(str.data())) == 0;
}
#endif


inline u8string::u8string(const std::string& str) noexcept
    : tiny_utf8::string(str) {}

inline u8string::u8string(const std::string_view& str) noexcept
    : tiny_utf8::string(std::string(str)) {}

inline u8string::u8string(const u8string_view& str) noexcept
    : tiny_utf8::string(str.data(), str.size()) {}

inline u8string::u8string(const size_t count, const char ch) noexcept
    : tiny_utf8::string(count, ch) {}

#if defined(__cpp_char8_t)
inline u8string::u8string(const std::u8string& str) noexcept
    : tiny_utf8::string(reinterpret_cast<const char*>(str.data()), str.size()) {}

inline u8string::u8string(const std::u8string_view& str) noexcept
    : tiny_utf8::string(reinterpret_cast<const char*>(str.data()), str.size()) {}
#endif


inline bool u8string::operator==(const std::string& str) const noexcept {
    return u8string_view(*this) == str;
}
inline bool u8string::operator==(const std::string_view& str) const noexcept {
    return u8string_view(*this) == str;
}
inline bool u8string::operator==(const u8string& str) const noexcept {
    return compare(str) == 0;
}
inline bool u8string::operator==(const u8string_view& str) const noexcept {
    return u8string_view(*this) == str;
}
#if defined(__cpp_char8_t)
inline bool u8string::operator==(const std::u8string& str) const noexcept {
    return u8string_view(*this) == u8string_view(reinterpret_cast<const char*>(str.data()), str.size());
}
inline bool u8string::operator==(const std::u8string_view& str) const noexcept {
    return u8string_view(*this) == u8string_view(reinterpret_cast<const char*>(str.data()), str.size());
}
#endif


inline u8string& u8string::operator+=(const std::string& str) noexcept {
    append(str);
    return *this;
}
inline u8string& u8string::operator+=(const std::string_view& str) noexcept {
    append(std::string(str)); //TODO: Extra copy
    return *this;
}
inline u8string& u8string::operator+=(const u8string& str) noexcept {
    append(str);
    return *this;
}
inline u8string& u8string::operator+=(const u8string_view& str) noexcept {
    append(std::string(str)); //TODO: Extra copy
    return *this;
}

#if defined(__cpp_char8_t)
inline u8string& u8string::operator+=(const std::u8string& str) noexcept {
    append(std::string(reinterpret_cast<const char*>(str.data()), str.size())); //TODO: Extra copy
    return *this;
}
inline u8string& u8string::operator+=(const std::u8string_view& str) noexcept {
    append(std::string(reinterpret_cast<const char*>(str.data()), str.size())); //TODO: Extra copy
    return *this;
}
#endif


inline u16string u8string_view::toUtf16(const bool strict) const {
    constexpr uint32_t halfShift = 10; // used for shifting by 10 bits
    constexpr uint32_t halfBase             = 0x00010000;
    constexpr uint32_t halfMask             = 0x000003FF;
    constexpr uint32_t UNI_REPLACEMENT_CHAR = 0x0000FFFD;
    constexpr uint32_t UNI_MAX_BMP          = 0x0000FFFF;
    constexpr uint32_t UNI_MAX_LEGAL_UTF32  = 0x0010FFFF;
    constexpr uint32_t UNI_SUR_HIGH_START   = 0x0000D800;
    constexpr uint32_t UNI_SUR_LOW_START    = 0x0000DC00;
    constexpr uint32_t UNI_SUR_LOW_END      = 0x0000DFFF;
    u16string res;
    for (size_t i = 0; i < size();) {
        uint32_t width = 0;
        uint32_t ch = 0;
        // 1 - 0xxxxxxx
        if ((data()[i] & 0x80) == 0x00) {
            width = 1;
            ch = data()[i];
        }
        // 2 - 110xxxxx 10xxxxxx
        else if ((data()[i] & 0xE0) == 0xC0) {
            width = 2;
            ch = ((data()[i]     & 0x1F) << 6)
               |  (data()[i + 1] & 0x3F);
        }
        // 3 - 1110xxxx 10xxxxxx 10xxxxxx
        else if ((data()[i] & 0xF0) == 0xE0) {
            width = 3;
            ch = ((data()[i]     & 0x0F) << 12)
               | ((data()[i + 1] & 0x3F) << 6)
               |  (data()[i + 2] & 0x3F);
        }
        // 4 - 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if ((data()[i] & 0xF8) == 0xF0) {
            width = 4;
            ch = ((data()[i]     & 0x07) << 18)
               | ((data()[i + 1] & 0x3F) << 12)
               | ((data()[i + 2] & 0x3F) << 6)
               |  (data()[i + 3] & 0x3F);
        }
        // 5 - 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if ((data()[i] & 0xFC) == 0xF8) {
            width = 5;
            ch = ((data()[i]     & 0x03) << 24)
               | ((data()[i + 1] & 0x3F) << 18)
               | ((data()[i + 2] & 0x3F) << 12)
               | ((data()[i + 3] & 0x3F) << 6)
               |  (data()[i + 4] & 0x3F);
        }
        else {
            // Invalid UTF-8 value
            break;
        }
        if (width + i > size()) {
            // Unexpected endl
            break;
        }
        // Not checked for 10xxxxxx
        i += width;

        if (ch <= UNI_MAX_BMP) { // Target is a character <= 0xFFFF
            // UTF-16 surrogate values are illegal in UTF-32;
            // 0xffff or 0xfffe are both reserved values
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (strict) {
                    res.push_back('\4');
                    break;
                }
                else {
                    res.push_back(static_cast<uint16_t>(UNI_REPLACEMENT_CHAR));
                }
            }
            else {
                res.push_back(static_cast<uint16_t>(ch)); // normal case
            }
        }
        else if (ch > UNI_MAX_LEGAL_UTF32) {
            if (strict) {
                res.push_back('\4');
                //TODO: break? throw?
            }
            else {
                res.push_back(static_cast<uint16_t>(UNI_REPLACEMENT_CHAR));
            }
        }
        else {
            ch -= halfBase;
            res.push_back(static_cast<uint16_t>((ch >> halfShift) + UNI_SUR_HIGH_START));
            res.push_back(static_cast<uint16_t>((ch & halfMask) + UNI_SUR_LOW_START));
        }
    }
    return res;
}

inline u32string u8string_view::toUtf32() const {
    u32string res;
    for (size_t i = 0; i < size();) {
        uint32_t width = 0;
        uint32_t ch = 0;
        // 1 - 0xxxxxxx
        if ((data()[i] & 0x80) == 0x00) {
            width = 1;
            ch = data()[i];
        }
        // 2 - 110xxxxx 10xxxxxx
        else if ((data()[i] & 0xE0) == 0xC0) {
            width = 2;
            ch = ((data()[i]     & 0x1F) << 6)
               |  (data()[i + 1] & 0x3F);
        }
        // 3 - 1110xxxx 10xxxxxx 10xxxxxx
        else if ((data()[i] & 0xF0) == 0xE0) {
            width = 3;
            ch = ((data()[i]     & 0x0F) << 12)
               | ((data()[i + 1] & 0x3F) << 6)
               |  (data()[i + 2] & 0x3F);
        }
        // 4 - 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if ((data()[i] & 0xF8) == 0xF0) {
            width = 4;
            ch = ((data()[i]     & 0x07) << 18)
               | ((data()[i + 1] & 0x3F) << 12)
               | ((data()[i + 2] & 0x3F) << 6)
               |  (data()[i + 3] & 0x3F);
        }
        // 5 - 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if ((data()[i] & 0xFC) == 0xF8) {
            width = 5;
            ch = ((data()[i]     & 0x03) << 24)
               | ((data()[i + 1] & 0x3F) << 18)
               | ((data()[i + 2] & 0x3F) << 12)
               | ((data()[i + 3] & 0x3F) << 6)
               |  (data()[i + 4] & 0x3F);
        }
        else {
            // Invalid UTF-8 value
            break;
        }
        if (width + i > size()) {
            // Unexpected endl
            break;
        }
        // Not checked for 10xxxxxx
        i += width;

        res.push_back(ch);
    }
    return res;
}

inline u16string u8string::toUtf16(const bool strict) const {
    return u8string_view(*this).toUtf16(strict);
}

inline u32string u8string::toUtf32() const {
    return u8string_view(*this).toUtf32();
}

u16string_view::u16string_view(const u16string& str) noexcept
    : u16string_view(str.c_str(), str.size()) {}

inline u8string u16string_view::toUtf8(const bool strict) const {
    constexpr uint32_t halfShift = 10; // used for shifting by 10 bits
    constexpr uint32_t halfBase             = 0x00010000;
    constexpr uint32_t UNI_SUR_HIGH_START   = 0x0000D800;
    constexpr uint32_t UNI_SUR_HIGH_END     = 0x0000DBFF;
    constexpr uint32_t UNI_SUR_LOW_START    = 0x0000DC00;
    constexpr uint32_t UNI_SUR_LOW_END      = 0x0000DFFF;
    u8string res;
    uint32_t ch, ch2;
    for (size_t i = 0; i < size(); ++i) {
        ch = data()[i];
        // If we have a surrogate pair, convert to UTF32 first.
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            // If the 16 bits following the high surrogate are in the source buffer...
            if (i + 1 < size()) {
                ch2 = data()[++i];
                // If it's a low surrogate, convert to UTF32.
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
                        + (ch2 - UNI_SUR_LOW_START) + halfBase;
                }
                // it's an unpaired high surrogate
                else if (strict) {
                    res.push_back('\4');
                    break;
                }
            }
            else { // We don't have the 16 bits following the high surrogate.
                res.push_back('\4');
                break;
            }
        }
        else if (strict) {
            // UTF-16 surrogate values are illegal in UTF-32
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                res.push_back('\4');
                break;
            }
        }
        res.push_back(ch);
    }
    return res;
}

inline u32string u16string_view::toUtf32(const bool strict) const {
    constexpr uint32_t halfShift = 10; // used for shifting by 10 bits
    constexpr uint32_t halfBase             = 0x00010000;
    constexpr uint32_t UNI_SUR_HIGH_START   = 0x0000D800;
    constexpr uint32_t UNI_SUR_HIGH_END     = 0x0000DBFF;
    constexpr uint32_t UNI_SUR_LOW_START    = 0x0000DC00;
    constexpr uint32_t UNI_SUR_LOW_END      = 0x0000DFFF;
    u32string res;
    uint32_t ch, ch2;
    for (uint32_t i = 0; i < size(); ++i) {
        ch = data()[i];
        // If we have a surrogate pair, convert to UTF32 first.
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            // If the 16 bits following the high surrogate are in the source buffer...
            if (i + 1 < size()) {
                ch2 = data()[++i];
                // If it's a low surrogate, convert to UTF32.
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
                        + (ch2 - UNI_SUR_LOW_START) + halfBase;
                }
                // it's an unpaired high surrogate
                else if (strict) {
                    res.push_back('\4');
                    break;
                }
            }
            else { // We don't have the 16 bits following the high surrogate.
                res.push_back('\4');
                break;
            }
        }
        else if (strict) {
            // UTF-16 surrogate values are illegal in UTF-32
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                res.push_back('\4');
                break;
            }
        }
        res.push_back(ch);
    }
    return res;
}

inline u8string u16string::toUtf8(const bool strict) const {
    return u16string_view(*this).toUtf8(strict);
}

inline u32string u16string::toUtf32(const bool strict) const {
    return u16string_view(*this).toUtf32(strict);
}

u32string_view::u32string_view(const u32string& str) noexcept
    : u32string_view(str.c_str(), str.size()) {}

inline u8string u32string_view::toUtf8(const bool strict) const {
    constexpr uint32_t UNI_SUR_HIGH_START   = 0x0000D800;
    constexpr uint32_t UNI_SUR_LOW_END      = 0x0000DFFF;
    u8string res;
    for (uint32_t i = 0; i < size(); ++i) {
        uint32_t ch = data()[i];
        if (strict) {
            // UTF-16 surrogate values are illegal in UTF-32
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                res.push_back('\4');
                break;
            }
        }
        res.push_back(ch);
    }
    return res;
}

inline u16string u32string_view::toUtf16(const bool strict) const {
    constexpr uint32_t halfShift = 10; // used for shifting by 10 bits
    constexpr uint32_t halfBase             = 0x00010000;
    constexpr uint32_t halfMask             = 0x000003FF;
    constexpr uint32_t UNI_REPLACEMENT_CHAR = 0x0000FFFD;
    constexpr uint32_t UNI_MAX_BMP          = 0x0000FFFF;
    constexpr uint32_t UNI_MAX_LEGAL_UTF32  = 0x0010FFFF;
    constexpr uint32_t UNI_SUR_HIGH_START   = 0x0000D800;
    constexpr uint32_t UNI_SUR_LOW_START    = 0x0000DC00;
    constexpr uint32_t UNI_SUR_LOW_END      = 0x0000DFFF;
    u16string res;
    for (size_t i = 0; i < size(); ++i) {
        uint32_t ch = data()[i];
        if (ch <= UNI_MAX_BMP) { // Target is a character <= 0xFFFF
            // UTF-16 surrogate values are illegal in UTF-32;
            // 0xffff or 0xfffe are both reserved values
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (strict) {
                    res.push_back('\4');
                    break;
                }
                else {
                    res.push_back(static_cast<uint16_t>(UNI_REPLACEMENT_CHAR));
                }
            }
            else {
                res.push_back(static_cast<uint16_t>(ch)); // normal case
            }
        }
        else if (ch > UNI_MAX_LEGAL_UTF32) {
            if (strict) {
                res.push_back('\4');
                //TODO: break? throw?
            }
            else {
                res.push_back(static_cast<uint16_t>(UNI_REPLACEMENT_CHAR));
            }
        }
        else {
            ch -= halfBase;
            res.push_back(static_cast<uint16_t>((ch >> halfShift) + UNI_SUR_HIGH_START));
            res.push_back(static_cast<uint16_t>((ch & halfMask) + UNI_SUR_LOW_START));
        }
    }
    return res;
}

inline u8string u32string::toUtf8(const bool strict) const {
    return u32string_view(*this).toUtf8(strict);
}

inline u16string u32string::toUtf16(const bool strict) const {
    return u32string_view(*this).toUtf16(strict);
}


#endif // CPP_STRING_UTILS
