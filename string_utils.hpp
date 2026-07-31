// C++ String Utils
//
// C++17 and C++11 std::string-and-std::string_view-based and UTF-8-based utils.
// https://utf8everywhere.org/
//
// Author: Yurii Blok
// License: BSL-1.0
// https://github.com/yurablok/cpp-string-utils
// History:
// v0.9 2026-Jul-31     Added `u8string_view::` `slice`, `slice_cp`, `trim`, `split`.
//                      Deleted `utils::` `trimm`, `split`.
// v0.8 2026-Jul-05     `u8string` is now based on `std::string`.
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

#ifndef _NODISCARD
#   if  __cplusplus >= 201703L or (defined(_MSVC_LANG) and _MSVC_LANG >= 201703L)
#       define _NODISCARD [[nodiscard]]
#   else
#       define _NODISCARD
#   endif
#endif

#ifndef _CONSTEXPR23
#   if  __cplusplus >= 202302L or (defined(_MSVC_LANG) and _MSVC_LANG >= 202302L)
#       define _CONSTEXPR23 constexpr
#   else
#       define _CONSTEXPR23 inline
#   endif
#endif
#ifndef _CONSTEXPR20
#   if  __cplusplus >= 202002L or (defined(_MSVC_LANG) and _MSVC_LANG >= 202002L)
#       define _CONSTEXPR20 constexpr
#   else
#       define _CONSTEXPR20 inline
#   endif
#endif
#ifndef _CONSTEXPR17
#   if  __cplusplus >= 201703L or (defined(_MSVC_LANG) and _MSVC_LANG >= 201703L)
#       define _CONSTEXPR17 constexpr
#   else
#       define _CONSTEXPR17 inline
#   endif
#endif
#ifndef _CONSTEXPR14
#   if  __cplusplus >= 201402L or (defined(_MSVC_LANG) and _MSVC_LANG >= 201402L)
#       define _CONSTEXPR14 constexpr
#   else
#       define _CONSTEXPR14 inline
#   endif
#endif

#if defined(_MSVC_LANG) and _MSVC_LANG >= 201703L
#   define CPP_STRING_UTILS_LIB_CHARCONV
#   define CPP_STRING_UTILS_LIB_CHARCONV_FLOAT
#elif __cplusplus >= 201703L
#   if defined(__GNUG__) and not defined(__llvm__)
#       if __GNUC__ >= 8 and __GNUC_MINOR__ >= 1
#           define CPP_STRING_UTILS_LIB_CHARCONV
#           if defined(__cpp_lib_to_chars) or defined(_GLIBCXX_HAVE_USELOCALE)
#               define CPP_STRING_UTILS_LIB_CHARCONV_FLOAT
#           endif
#       endif
#   else
#       define CPP_STRING_UTILS_LIB_CHARCONV
#       define CPP_STRING_UTILS_LIB_CHARCONV_FLOAT
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
#if not defined(CPP_STRING_UTILS_LIB_CHARCONV_FLOAT)
#   define __STDC_FORMAT_MACROS
#   include <cinttypes>
#endif


static_assert(static_cast<uint8_t>(u8"🌍"[0]) == 0xF0, "Wrong UTF-8 config");
static_assert(static_cast<uint8_t>(u8"🌍"[1]) == 0x9F, "Wrong UTF-8 config");
static_assert(static_cast<uint8_t>(u8"🌍"[2]) == 0x8C, "Wrong UTF-8 config");
static_assert(static_cast<uint8_t>(u8"🌍"[3]) == 0x8D, "Wrong UTF-8 config");

static_assert(sizeof(""[0]) >= sizeof(u8""[0]), "");
#if defined(__cpp_char8_t)
static_assert(sizeof(char) >= sizeof(char8_t), "");
#endif


class u8string;
class u16string;
class u32string;


class u8string_view : public std::string_view {
public:
    constexpr u8string_view() noexcept = default;

    constexpr u8string_view(const char* str) noexcept;
    constexpr u8string_view(const char* str, size_t size_B) noexcept;
    constexpr u8string_view(const void* str, size_t size_B) noexcept;
    template <size_t size_B>
    constexpr u8string_view(const char (&str)[size_B]) noexcept
        : std::string_view(str, size_B - 1) {}

    _CONSTEXPR20 u8string_view(const std::string& str) noexcept;
    constexpr u8string_view(const std::string_view& str) noexcept;
    _CONSTEXPR20 u8string_view(const u8string& str) noexcept;
    constexpr u8string_view(const u8string_view& str) noexcept = default;

# if defined(__cpp_char8_t)
    constexpr u8string_view(const char8_t* str) noexcept;
    constexpr u8string_view(const char8_t* str, size_t size_B) noexcept;
    template <size_t size_B>
    constexpr u8string_view(const char8_t (&str)[size_B]) noexcept
        : std::string_view(reinterpret_cast<const char*>(str), size_B - 1) {}

    constexpr u8string_view(const std::u8string& str) noexcept;
    constexpr u8string_view(const std::u8string_view& str) noexcept;
# endif

    inline u8string_view& operator=(const char* str) noexcept;
# if defined(__cpp_char8_t)
    inline u8string_view& operator=(const char8_t* str) noexcept;
# endif

    template <typename ch_t>
    constexpr bool operator==(ch_t str) const noexcept {
        return *this == u8string_view(
            str == nullptr ? "" : reinterpret_cast<const char*>(str));
    }
    template <size_t size_B>
    constexpr bool operator==(const char (&str)[size_B]) const noexcept {
        return *this == u8string_view(str, size_B - 1);
    }
    _CONSTEXPR20 bool operator==(const std::string& str) const noexcept;
    _CONSTEXPR14 bool operator==(const std::string_view& str) const noexcept;
    _CONSTEXPR20 bool operator==(const u8string& str) const noexcept;
    _CONSTEXPR14 bool operator==(const u8string_view& str) const noexcept;
# if defined(__cpp_char8_t)
    template <size_t size_B>
    constexpr bool operator==(const char8_t (&str)[size_B]) const noexcept {
        return *this == u8string_view(str, size_B - 1);
    }
    _CONSTEXPR14 bool operator==(const std::u8string& str) const noexcept;
    constexpr bool operator==(const std::u8string_view& str) const noexcept;
# endif
    template <typename str_t>
    _CONSTEXPR14 bool operator!=(str_t str) const noexcept {
        return not operator==(str);
    }

    _CONSTEXPR14 size_t size_B() const noexcept;
    // size
    _CONSTEXPR14 size_t size_cu() const noexcept;
    // O(n)
    _CONSTEXPR14 size_t size_cp() const noexcept;

    // O(n)
    _CONSTEXPR14 char32_t at_cp(size_t pos) const noexcept;

    // Omin(1), Omax(n)
    static _CONSTEXPR14 char32_t at_cp(size_t index,
        size_t& previous, size_t& offset, const char* begin) noexcept;

    static _CONSTEXPR14 size_t cp_width(const char* p) noexcept;

    _NODISCARD _CONSTEXPR14 u8string_view substr(size_t offset = 0, size_t count = npos) const;
    // O(n)
    //_NODISCARD _CONSTEXPR14 u8string_view substr_cp(size_t offset = 0, size_t count = npos) const;

    _NODISCARD _CONSTEXPR14 u8string_view slice(intptr_t begin = 0, intptr_t end = npos) const noexcept;
    _NODISCARD _CONSTEXPR14 u8string_view slice_cp(intptr_t begin = 0, intptr_t end = npos) const noexcept;

    _NODISCARD _CONSTEXPR20 u8string_view trim(
        std::string_view by = "", bool whitespace = true) const noexcept;
    _NODISCARD _CONSTEXPR20 u8string_view trim(char open, char close,
        std::string_view by = "", bool whitespace = true, char escape = '\\') const noexcept;

    inline void split(const std::string_view by,
        const std::function<void(u8string_view part, uint32_t idx)> handler,
        const bool withEmpty = false, const char escape = '\\') const;

    template <typename container_t>
    inline void split(const std::string_view by, container_t& container,
            const bool withEmpty = false, const char escape = '\\') const {
        split(by, [&](u8string_view part, uint32_t idx) {
            if (idx >= container.size()) {
                return;
            }
            container[size_t(idx)] = part;
        });
    }

    static _CONSTEXPR14 char32_t decode(const char* data) noexcept;

    static _CONSTEXPR14 size_t encode(char32_t ch, char* out) noexcept;

    class const_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = char32_t;
        using difference_type = std::ptrdiff_t;
        using pointer = const char32_t*;
        using reference = char32_t;

        _CONSTEXPR14 const_iterator() noexcept {}
        _CONSTEXPR14 const_iterator(const char* ptr) noexcept : m_ptr(ptr) {}

        _CONSTEXPR14 char32_t operator*() const noexcept {
            return u8string_view::decode(m_ptr);
        }

        _CONSTEXPR14 const_iterator& operator++() noexcept {
            m_ptr += u8string_view::cp_width(m_ptr);
            return *this;
        }
        _CONSTEXPR14 const_iterator operator++(int) noexcept {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        _CONSTEXPR20 const_iterator& operator--() noexcept {
            --m_ptr;
            while ((static_cast<uint8_t>(*m_ptr) & 0xC0) == 0x80) {
                --m_ptr;
            }
            return *this;
        }
        _CONSTEXPR20 const_iterator operator--(int) noexcept {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        _CONSTEXPR14 bool operator==(const const_iterator& other) const noexcept {
            return m_ptr == other.m_ptr;
        }
        _CONSTEXPR14 bool operator!=(const const_iterator& other) const noexcept {
            return not (*this == other);
        }
        _CONSTEXPR14 const char* ptr() const noexcept {
            return m_ptr;
        }
    private:
        const char* m_ptr = nullptr;
    }; // class const_iterator

    _CONSTEXPR14 const_iterator begin_cp() const noexcept {
        return const_iterator(data());
    }
    _CONSTEXPR14 const_iterator end_cp() const noexcept {
        return const_iterator(data() + size());
    }

    // Little-endian
    _CONSTEXPR20 u16string toUtf16(const bool strict = true) const;
    // Little-endian
    _CONSTEXPR20 u32string toUtf32() const;
}; // class u8string_view


class u8string : public std::string {
public:
    using std::string::basic_string;
    _CONSTEXPR20 u8string() = default;

    _CONSTEXPR20 u8string(const char* str) noexcept;
    _CONSTEXPR20 u8string(const char* str, size_t size_B) noexcept;
    _CONSTEXPR20 u8string(const void* str, size_t size_B) noexcept;
    template <size_t size_B>
    _CONSTEXPR20 u8string(const char (&str)[size_B]) noexcept
        : std::string(str, size_B - 1) {}

    _CONSTEXPR20 u8string(const std::string& str) noexcept;
    _CONSTEXPR20 u8string(const std::string_view& str) noexcept;
    _CONSTEXPR20 u8string(const u8string& str) = default;
    _CONSTEXPR20 u8string(const u8string_view& str) noexcept;
    _CONSTEXPR20 u8string(const size_t count, const char ch) noexcept;

# if defined(__cpp_char8_t)
    inline u8string(const char8_t* str) noexcept;
    inline u8string(const char8_t* str, size_t size_B) noexcept;
    template <size_t size_B>
    inline u8string(const char8_t (&str)[size_B]) noexcept
        : std::string(reinterpret_cast<const char*>(str), size_B - 1) {}

    inline u8string(const std::u8string& str) noexcept;
    inline u8string(const std::u8string_view& str) noexcept;
# endif

    _CONSTEXPR20 u8string& operator=(const char* str) noexcept;
# if defined(__cpp_char8_t)
    inline u8string& operator=(const char8_t* str) noexcept;
# endif

    template <typename ch_t>
    inline bool operator==(ch_t str) const noexcept {
        return u8string_view(*this) == u8string_view(
            str == nullptr ? "" : reinterpret_cast<const char*>(str));
    }
    template <size_t size_B>
    inline bool operator==(const char (&str)[size_B]) const noexcept {
        return u8string_view(*this) == u8string_view(str, size_B - 1);
    }
    _CONSTEXPR20 bool operator==(const std::string& str) const noexcept;
    _CONSTEXPR20 bool operator==(const std::string_view& str) const noexcept;
    _CONSTEXPR20 bool operator==(const u8string& str) const noexcept;
    _CONSTEXPR20 bool operator==(const u8string_view& str) const noexcept;
# if defined(__cpp_char8_t)
    template <size_t size_B>
    inline bool operator==(const char8_t (&str)[size_B]) const noexcept {
        return u8string_view(*this) == u8string_view(str, size_B - 1);
    }
    inline bool operator==(const std::u8string& str) const noexcept;
    inline bool operator==(const std::u8string_view& str) const noexcept;
# endif
    template <typename str_t>
    inline bool operator!=(str_t str) const noexcept {
        return not operator==(str);
    }

    template <typename ch_t>
    inline u8string& operator+=(ch_t str) noexcept {
        if (str != nullptr) {
            append(reinterpret_cast<const char*>(str));
        }
        return *this;
    }
    template <size_t size_B>
    _CONSTEXPR20 u8string& operator+=(const char (&str)[size_B]) noexcept {
        append(str, size_B - 1);
        return *this;
    }
    _CONSTEXPR20 u8string& operator+=(const std::string& str) noexcept;
    _CONSTEXPR20 u8string& operator+=(const std::string_view& str) noexcept;
    _CONSTEXPR20 u8string& operator+=(const u8string& str) noexcept;
    _CONSTEXPR20 u8string& operator+=(const u8string_view& str) noexcept;
# if defined(__cpp_char8_t)
    inline u8string& operator+=(const std::u8string& str) noexcept;
    inline u8string& operator+=(const std::u8string_view& str) noexcept;
    template <size_t size_B>
    inline u8string& operator+=(const char8_t (&str)[size_B]) noexcept {
        append(reinterpret_cast<const char*>(str), size_B - 1);
        return *this;
    }
# endif

    _CONSTEXPR20 size_t size_B() const noexcept;
    // size
    _CONSTEXPR20 size_t size_cu() const noexcept;
    // O(n)
    _CONSTEXPR20 size_t size_cp() const noexcept;

    _CONSTEXPR20 u8string_view::const_iterator begin_cp() const noexcept {
        return u8string_view::const_iterator(data());
    }
    _CONSTEXPR20 u8string_view::const_iterator end_cp() const noexcept {
        return u8string_view::const_iterator(data() + size());
    }

    _CONSTEXPR20 char32_t front_cp() const noexcept;
    _CONSTEXPR20 char32_t back_cp() const noexcept;
    _CONSTEXPR20 void push_back_cp(char32_t ch);
    _CONSTEXPR20 void pop_back_cp() noexcept;

    _CONSTEXPR20 u8string_view view() const noexcept {
        return u8string_view(*this);
    }

    // Little-endian
    _CONSTEXPR20 u16string toUtf16(const bool strict = true) const;
    // Little-endian
    _CONSTEXPR20 u32string toUtf32() const;
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
    _CONSTEXPR20 u16string_view(const u16string& str) noexcept;
    _CONSTEXPR20 u8string toUtf8(const bool strict = true) const;
    // Little-endian
    _CONSTEXPR20 u32string toUtf32(const bool strict = true) const;

    _CONSTEXPR14 size_t size_B() const noexcept;
    // size
    _CONSTEXPR14 size_t size_cu() const noexcept;
    // O(n)
    _CONSTEXPR14 size_t size_cp() const noexcept;
};


class u16string
# if WCHAR_MAX < 0x10000
    : public std::wstring {
# else
    : public std::u16string {
# endif
public:
    _CONSTEXPR20 u8string toUtf8(const bool strict = true) const;
    // Little-endian
    _CONSTEXPR20 u32string toUtf32(const bool strict = true) const;

    _CONSTEXPR20 size_t size_B() const noexcept;
    // size
    _CONSTEXPR20 size_t size_cu() const noexcept;
    // O(n)
    _CONSTEXPR20 size_t size_cp() const noexcept;
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
    _CONSTEXPR20 u32string_view(const u32string& str) noexcept;
    _CONSTEXPR20 u8string toUtf8(const bool strict = true) const;
    // Little-endian
    _CONSTEXPR20 u16string toUtf16(const bool strict = true) const;

    _CONSTEXPR14 size_t size_B() const noexcept;
    // size
    _CONSTEXPR14 size_t size_cu() const noexcept;
    // size
    _CONSTEXPR14 size_t size_cp() const noexcept;
};


class u32string
# if WCHAR_MAX < 0x10000
    : public std::u32string {
# else
    : public std::wstring {
# endif
public:
    _CONSTEXPR20 u8string toUtf8(const bool strict = true) const;
    // Little-endian
    _CONSTEXPR20 u16string toUtf16(const bool strict = true) const;

    _CONSTEXPR20 size_t size_B() const noexcept;
    // size
    _CONSTEXPR20 size_t size_cu() const noexcept;
    // size
    _CONSTEXPR20 size_t size_cp() const noexcept;
};


namespace utils {

_CONSTEXPR20 u8string_view substr(const u8string_view str, size_t& offset,
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
        if (not isPrevEscape) {
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
        if (withEmpty or not part.empty()) {
            ++offset;
            return part;
        }
        begin = offset + 1;
    }
    const u8string_view part = str.substr(begin);
    ++offset;
    if (not part.empty()) {
        return part;
    }
    return {};
}

inline void parseCSV(const u8string_view csv,
        const std::function<void(u8string_view cell, uint32_t idx)> onCell,
        const std::function<void()> onEndl = nullptr) {
    if (not onCell) {
        return;
    }
    u8string cell;
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
                if (not cell.empty()) {
                    onCell(cell, idx);
                    cell.clear();
                }
                if (not isPrevEndl and onEndl) {
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
            if (c != 0 and c != '\n' and c != '\r') {
                isPrevEndl = false;
            }
        }
    }
    if (not cell.empty()) {
        onCell(cell, idx);
        cell.clear();
    }
    if (not isPrevEndl and onEndl) {
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
    return buffer.substr(0, length).trim("0\0", false);
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
    return buffer.substr(0, length).trim("0\0", false);
}

#endif // CPP_STRING_UTILS_LIB_CHARCONV_FLOAT



#if defined(CPP_STRING_UTILS_LIB_CHARCONV)

template<typename integer_t,
    typename std::enable_if_t<std::is_integral_v<integer_t>, bool> = true>
_CONSTEXPR23 bool from_string(const u8string_view string, integer_t& number,
        const bool hex = false) noexcept {
    auto [ptr, ec] = std::from_chars(
        string.data(),
        string.data() + string.size(),
        number,
        hex ? 16 : 10
    );
    if (ptr != reinterpret_cast<const char*>(string.data()) + string.size()
            or ec != std::errc(0)) {
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
_CONSTEXPR23 bool from_string(const u8string_view string, floating_t& number) noexcept {
    auto [ptr, ec] = std::from_chars(
        string.data(),
        string.data() + string.size(),
        number
    );
    if (ptr != string.data() + string.size() or ec != std::errc(0)) {
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




constexpr u8string_view::u8string_view(const char* str) noexcept
    : std::string_view(str == nullptr ? "" : str) {}
constexpr u8string_view::u8string_view(const char* str, size_t size_B) noexcept
    : std::string_view(str == nullptr ? "" : str, size_B) {}
constexpr u8string_view::u8string_view(const void* str, size_t size_B) noexcept
    : std::string_view(str == nullptr ? "" : static_cast<const char*>(str), size_B) {}

_CONSTEXPR20 u8string_view::u8string_view(const std::string& str) noexcept
    : std::string_view(str.data(), str.size()) {}
constexpr u8string_view::u8string_view(const std::string_view& str) noexcept
    : std::string_view(str) {}
_CONSTEXPR20 u8string_view::u8string_view(const u8string& str) noexcept
    : std::string_view(str.data(), str.size()) {}

#if defined(__cpp_char8_t)
constexpr u8string_view::u8string_view(const char8_t* str) noexcept
    : std::string_view(str == nullptr ? "" : reinterpret_cast<const char*>(str)) {}
constexpr u8string_view::u8string_view(const char8_t* str, size_t size_B) noexcept
    : std::string_view(str == nullptr ? "" : reinterpret_cast<const char*>(str), size_B) {}

constexpr u8string_view::u8string_view(const std::u8string& str) noexcept
    : std::string_view(str.empty() ? "" : reinterpret_cast<const char*>(str.data()), str.size()) {}
constexpr u8string_view::u8string_view(const std::u8string_view& str) noexcept
    : std::string_view(str.empty() ? "" : reinterpret_cast<const char*>(str.data()), str.size()) {}
#endif

inline u8string_view& u8string_view::operator=(const char* str) noexcept {
    std::string_view::operator=(str == nullptr ? "" : str);
    return *this;
}
#if defined(__cpp_char8_t)
inline u8string_view& u8string_view::operator=(const char8_t* str) noexcept {
    std::string_view::operator=(str == nullptr ? ""
        : std::string_view(reinterpret_cast<const char*>(str)));
    return *this;
}
#endif

_CONSTEXPR20 bool u8string_view::operator==(const std::string& str) const noexcept {
    return *this == u8string_view(str);
}
_CONSTEXPR14 bool u8string_view::operator==(const std::string_view& str) const noexcept {
    return *this == u8string_view(str);
}
_CONSTEXPR20 bool u8string_view::operator==(const u8string& str) const noexcept {
    return *this == u8string_view(str);
}
_CONSTEXPR14 bool u8string_view::operator==(const u8string_view& str) const noexcept {
    if (size() != str.size()) {
        return false;
    }
    for (size_t i = 0; i < size(); ++i) {
        if ((*this)[i] != str[i]) {
            return false;
        }
    }
    return true;
}
#if defined(__cpp_char8_t)
_CONSTEXPR14 bool u8string_view::operator==(const std::u8string& str) const noexcept {
    return *this == u8string_view(str);
}
constexpr bool u8string_view::operator==(const std::u8string_view& str) const noexcept {
    return *this == u8string_view(str);
}
#endif

_CONSTEXPR14 size_t u8string_view::size_B() const noexcept {
    return size();
}
_CONSTEXPR14 size_t u8string_view::size_cu() const noexcept {
    return size();
}
_CONSTEXPR14 size_t u8string_view::size_cp() const noexcept {
    size_t count = 0;
    for (size_t i = 0; i < size();) {
        i += cp_width(data() + i);
        ++count;
    }
    return count;
}

_CONSTEXPR14 char32_t u8string_view::at_cp(size_t pos) const noexcept {
    size_t offset = 0;
    for (size_t i = 0; i < pos; ++i) {
        offset += cp_width(data() + offset);
    }
    return decode(data() + offset);
}

_CONSTEXPR14 char32_t u8string_view::at_cp(size_t index,
        size_t& previous, size_t& offset, const char* begin) noexcept {
    if (index == 0) {
        previous = 0;
        offset = 0;
        return decode(begin);
    }
    while (previous < index) {
        offset += cp_width(begin + offset);
        ++previous;
    }
    while (index < previous) {
        --offset;
        while ((static_cast<uint8_t>(begin[offset]) & 0xC0) == 0x80) {
            --offset;
        }
        --previous;
    }
    return decode(begin + offset);
}

_CONSTEXPR14 size_t u8string_view::cp_width(const char* p) noexcept {
    uint8_t c = static_cast<uint8_t>(*p);
    if ((c & 0x80) == 0x00) { return 1; }
    if ((c & 0xE0) == 0xC0) { return 2; }
    if ((c & 0xF0) == 0xE0) { return 3; }
    if ((c & 0xF8) == 0xF0) { return 4; }
    if ((c & 0xFC) == 0xF8) { return 5; }
    return 1; // invalid, treat as 1 byte
}

_NODISCARD _CONSTEXPR14 u8string_view u8string_view::substr(size_t offset, size_t count) const {
    return std::string_view::substr(offset, count);
}

_NODISCARD _CONSTEXPR14 u8string_view u8string_view::slice(intptr_t begin, intptr_t end) const noexcept {
    const intptr_t len = static_cast<intptr_t>(size_B());
    intptr_t b = begin < 0 ? len + begin : begin;
    if (b < 0) {
        b = 0;
    }
    if (b > len) {
        b = len;
    }
    intptr_t e = npos;
    if (static_cast<size_t>(end) == npos) {
        e = len;
    }
    else if (end < 0) {
        e = len + end;
    }
    else {
        e = end;
    }
    if (e < 0) {
        e = 0;
    }
    if (e > len) {
        e = len;
    }
    if (b >= e) {
        return {};
    }
    return std::string_view::substr(static_cast<size_t>(b), static_cast<size_t>(e - b));
}

_NODISCARD _CONSTEXPR14 u8string_view u8string_view::slice_cp(intptr_t begin, intptr_t end) const noexcept {
    const size_t len = size_B();
    const char* b = nullptr;
    if (begin >= 0) {
        b = data();
        for (intptr_t i = 0; i < begin and static_cast<size_t>(b - data()) < len; ++i) {
            b += cp_width(b);
        }
    }
    else {
        b = data() + len;
        for (intptr_t i = 0; i < -begin and b > data(); ++i) {
            --b;
            while (b > data() and (static_cast<uint8_t>(*b) & 0xC0) == 0x80) {
                --b;
            }
        }
    }
    const char* e = nullptr;
    if (static_cast<size_t>(end) == npos) {
        e = data() + len;
    }
    else if (end >= 0) {
        e = data();
        for (intptr_t i = 0; i < end and static_cast<size_t>(e - data()) < len; ++i) {
            e += cp_width(e);
        }
    }
    else {
        e = data() + len;
        for (intptr_t i = 0; i < -end and e > data(); ++i) {
            --e;
            while (e > data() and (static_cast<uint8_t>(*e) & 0xC0) == 0x80) {
                --e;
            }
        }
    }
    if (b >= e) {
        return {};
    }
    return std::string_view::substr(static_cast<size_t>(b - data()), static_cast<size_t>(e - b));
}

_NODISCARD _CONSTEXPR20 u8string_view u8string_view::trim(
        std::string_view by, bool whitespace) const noexcept {
    constexpr std::string_view s("\r\n\t\f\v \0", 7);
    const char* beg = data();
    const char* end = data() + size_B();
    while (beg < end) {
        if ((not whitespace or s.find(*beg) == std::string_view::npos)
                and by.find(*beg) == std::string_view::npos) {
            break;
        }
        ++beg;
    }
    while (beg < end) {
        if ((not whitespace or s.find(*(end - 1)) == std::string_view::npos)
                and by.find(*(end - 1)) == std::string_view::npos) {
            break;
        }
        --end;
    }
    return substr(static_cast<size_t>(beg - data()), static_cast<size_t>(end - beg));
}
_NODISCARD _CONSTEXPR20 u8string_view u8string_view::trim(char open, char close,
        std::string_view by, bool whitespace, char escape) const noexcept {
    constexpr std::string_view s("\r\n\t\f\v \0", 7);
    const char* beg = data();
    const char* b = beg;
    const char* e = beg;
    const char* end = data() + size_B();
    bool opened = false;
    //const char* opened = nullptr;
    while (beg < end) {
        if (opened) {
            if (*beg == escape) {
                beg += cp_width(beg + 1) + 1;
                continue;
            }
            else if (*beg == close) {
                opened = false;
            }
        }
        else if (*beg == open) {
            opened = true;
        }
        else if ((not whitespace or s.find(*beg) == std::string_view::npos)
                and by.find(*beg) == std::string_view::npos) {
            b = beg;
            break;
        }
        ++beg;
    }
    while (beg < end) {
        if (opened) {
            if (*beg == escape) {
                beg += cp_width(beg + 1) + 1;
                continue;
            }
            else if (*beg == close) {
                opened = false;
            }
        }
        else if (*beg == escape) {
            beg += cp_width(beg + 1) + 1;
            e = beg;
            continue;
        }
        else if (*beg == open) {
            opened = true;
        }
        else if ((not whitespace or s.find(*beg) == std::string_view::npos)
                and by.find(*beg) == std::string_view::npos) {
            e = beg + 1;
        }
        ++beg;
    }
    if (e <= b) {
        return {};
    }
    return substr(static_cast<size_t>(b - data()), static_cast<size_t>(e - b));
}

inline void u8string_view::split(const std::string_view by,
        const std::function<void(u8string_view part, uint32_t idx)> handler,
        const bool withEmpty, const char escape) const {
    if (by.empty() or not handler) {
        return;
    }
    const char* beg = data();
    const char* ptr = beg;
    const char* end = data() + size_B();
    uint32_t idx = 0;
    while (ptr < end) {
        if (*ptr == escape) {
            ptr += cp_width(ptr + 1) + 1;
        }
        else if (by.find(*ptr) != std::string_view::npos) {
            const u8string_view part = substr(beg - data(), ptr - beg);
            if (withEmpty or not part.empty()) {
                handler(part, idx++);
            }
            ++ptr;
            beg = ptr;
        }
        else {
            ++ptr;
        }
    }
    const u8string_view part = substr(beg - data());
    if (withEmpty or not part.empty()) {
        handler(part, idx);
    }
}

_CONSTEXPR14 char32_t u8string_view::decode(const char* data) noexcept {
    // 1 - 0xxxxxxx
    if ((static_cast<uint8_t>(data[0]) & 0x80) == 0x00) {
        return static_cast<char32_t>(data[0]);
    }
    // 2 - 110xxxxx 10xxxxxx
    if ((static_cast<uint8_t>(data[0]) & 0xE0) == 0xC0) {
        return static_cast<char32_t>(
                ((static_cast<uint8_t>(data[0]) & 0x1F) << 6)
            |  (static_cast<uint8_t>(data[1]) & 0x3F));
    }
    // 3 - 1110xxxx 10xxxxxx 10xxxxxx
    if ((static_cast<uint8_t>(data[0]) & 0xF0) == 0xE0) {
        return static_cast<char32_t>(
                ((static_cast<uint8_t>(data[0]) & 0x0F) << 12)
            | ((static_cast<uint8_t>(data[1]) & 0x3F) << 6)
            |  (static_cast<uint8_t>(data[2]) & 0x3F));
    }
    // 4 - 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if ((static_cast<uint8_t>(data[0]) & 0xF8) == 0xF0) {
        return static_cast<char32_t>(
                ((static_cast<uint8_t>(data[0]) & 0x07) << 18)
            | ((static_cast<uint8_t>(data[1]) & 0x3F) << 12)
            | ((static_cast<uint8_t>(data[2]) & 0x3F) << 6)
            |  (static_cast<uint8_t>(data[3]) & 0x3F));
    }
    // 5 - 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    if ((static_cast<uint8_t>(data[0]) & 0xFC) == 0xF8) {
        return static_cast<char32_t>(
                ((static_cast<uint8_t>(data[0]) & 0x03) << 24)
            | ((static_cast<uint8_t>(data[1]) & 0x3F) << 18)
            | ((static_cast<uint8_t>(data[2]) & 0x3F) << 12)
            | ((static_cast<uint8_t>(data[3]) & 0x3F) << 6)
            |  (static_cast<uint8_t>(data[4]) & 0x3F));
    }
    return static_cast<char32_t>(data[0]);
}

_CONSTEXPR14 size_t u8string_view::encode(char32_t ch, char* out) noexcept {
    if (ch <= 0x7F) {
        out[0] = static_cast<char>(ch);
        return 1;
    }
    if (ch <= 0x7FF) {
        out[0] = static_cast<char>(0xC0 | (ch >> 6));
        out[1] = static_cast<char>(0x80 | (ch & 0x3F));
        return 2;
    }
    if (ch <= 0xFFFF) {
        if (0xD800 <= ch and ch <= 0xDFFF) {
            out[0] = '?';
            return 1;
        }
        out[0] = static_cast<char>(0xE0 | (ch >> 12));
        out[1] = static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
        out[2] = static_cast<char>(0x80 | (ch & 0x3F));
        return 3;
    }
    if (ch <= 0x10FFFF) {
        out[0] = static_cast<char>(0xF0 | (ch >> 18));
        out[1] = static_cast<char>(0x80 | ((ch >> 12) & 0x3F));
        out[2] = static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
        out[3] = static_cast<char>(0x80 | (ch & 0x3F));
        return 4;
    }
    out[0] = '?';
    return 1;
}

_CONSTEXPR20 u16string u8string_view::toUtf16(const bool strict) const {
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
        size_t width = cp_width(data() + i);
        if (i + width > size()) {
            break;
        }
        uint32_t ch = static_cast<uint32_t>(decode(data() + i));
        i += width;

        if (ch <= UNI_MAX_BMP) { // Target is a character <= 0xFFFF
            // UTF-16 surrogate values are illegal in UTF-32;
            // 0xffff or 0xfffe are both reserved values
            if (ch >= UNI_SUR_HIGH_START and ch <= UNI_SUR_LOW_END) {
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
_CONSTEXPR20 u32string u8string_view::toUtf32() const {
    u32string res;
    for (size_t i = 0; i < size();) {
        size_t width = cp_width(data() + i);
        if (i + width > size()) {
            break;
        }
        res.push_back(decode(data() + i));
        i += width;
    }
    return res;
}


_CONSTEXPR20 u8string::u8string(const char* str) noexcept
    : std::string(str == nullptr ? "" : str) {}
_CONSTEXPR20 u8string::u8string(const char* str, size_t size_B) noexcept
    : std::string(str == nullptr ? "" : str, size_B) {}
_CONSTEXPR20 u8string::u8string(const void* str, size_t size_B) noexcept
    : std::string(str == nullptr ? "" : static_cast<const char*>(str), size_B) {}

_CONSTEXPR20 u8string::u8string(const std::string& str) noexcept
    : std::string(str) {}
_CONSTEXPR20 u8string::u8string(const std::string_view& str) noexcept
    : std::string(str) {}
_CONSTEXPR20 u8string::u8string(const u8string_view& str) noexcept
    : std::string(str.data(), str.size()) {}
_CONSTEXPR20 u8string::u8string(const size_t count, const char ch) noexcept
    : std::string(count, ch) {}

#if defined(__cpp_char8_t)
inline u8string::u8string(const char8_t* str) noexcept
    : std::string(str == nullptr ? "" : reinterpret_cast<const char*>(str)) {}
inline u8string::u8string(const char8_t* str, size_t size_B) noexcept
    : std::string(reinterpret_cast<const char*>(str), size_B) {}

inline u8string::u8string(const std::u8string& str) noexcept
    : std::string(reinterpret_cast<const char*>(str.data()), str.size()) {}
inline u8string::u8string(const std::u8string_view& str) noexcept
    : std::string(reinterpret_cast<const char*>(str.data()), str.size()) {}
#endif

_CONSTEXPR20 u8string& u8string::operator=(const char* str) noexcept {
    if (str == nullptr) {
        clear();
    }
    else {
        std::string::operator=(str);
    }
    return *this;
}
#if defined(__cpp_char8_t)
inline u8string& u8string::operator=(const char8_t* str) noexcept {
    if (str == nullptr) {
        clear();
    }
    else {
        std::string::operator=(reinterpret_cast<const char*>(str));
    }
    return *this;
}
#endif

_CONSTEXPR20 bool u8string::operator==(const std::string& str) const noexcept {
    return u8string_view(*this) == u8string_view(str);
}
_CONSTEXPR20 bool u8string::operator==(const std::string_view& str) const noexcept {
    return u8string_view(*this) == u8string_view(str);
}
_CONSTEXPR20 bool u8string::operator==(const u8string& str) const noexcept {
    return u8string_view(*this) == u8string_view(str);
}
_CONSTEXPR20 bool u8string::operator==(const u8string_view& str) const noexcept {
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

_CONSTEXPR20 u8string& u8string::operator+=(const std::string& str) noexcept {
    append(str);
    return *this;
}
_CONSTEXPR20 u8string& u8string::operator+=(const std::string_view& str) noexcept {
    append(str.begin(), str.end());
    return *this;
}
_CONSTEXPR20 u8string& u8string::operator+=(const u8string& str) noexcept {
    append(str);
    return *this;
}
_CONSTEXPR20 u8string& u8string::operator+=(const u8string_view& str) noexcept {
    append(str.begin(), str.end());
    return *this;
}
#if defined(__cpp_char8_t)
inline u8string& u8string::operator+=(const std::u8string& str) noexcept {
    //TODO: std::is_constant_evaluated();
    append(reinterpret_cast<const char*>(str.data()), str.size());
    return *this;
}
inline u8string& u8string::operator+=(const std::u8string_view& str) noexcept {
    append(reinterpret_cast<const char*>(str.data()), str.size());
    return *this;
}
#endif

_CONSTEXPR20 size_t u8string::size_B() const noexcept {
    return size();
}
_CONSTEXPR20 size_t u8string::size_cu() const noexcept {
    return size();
}
_CONSTEXPR20 size_t u8string::size_cp() const noexcept {
    size_t count = 0;
    for (size_t i = 0; i < size();) {
        i += u8string_view::cp_width(data() + i);
        ++count;
    }
    return count;
}

_CONSTEXPR20 char32_t u8string::front_cp() const noexcept {
    return *begin_cp();
}
_CONSTEXPR20 char32_t u8string::back_cp() const noexcept {
    return *(--end_cp());
}
_CONSTEXPR20 void u8string::push_back_cp(char32_t ch) {
    char buf[5];
    size_t n = u8string_view::encode(ch, buf);
    append(buf, n);
}
_CONSTEXPR20 void u8string::pop_back_cp() noexcept {
    resize((--end_cp()).ptr() - data());
}

_CONSTEXPR20 u16string u8string::toUtf16(const bool strict) const {
    return u8string_view(*this).toUtf16(strict);
}
_CONSTEXPR20 u32string u8string::toUtf32() const {
    return u8string_view(*this).toUtf32();
}


_CONSTEXPR20 u16string_view::u16string_view(const u16string& str) noexcept
    : u16string_view(str.c_str(), str.size()) {}

_CONSTEXPR20 u8string u16string_view::toUtf8(const bool strict) const {
    constexpr uint32_t halfShift = 10; // used for shifting by 10 bits
    constexpr uint32_t halfBase             = 0x00010000;
    constexpr uint32_t UNI_SUR_HIGH_START   = 0x0000D800;
    constexpr uint32_t UNI_SUR_HIGH_END     = 0x0000DBFF;
    constexpr uint32_t UNI_SUR_LOW_START    = 0x0000DC00;
    constexpr uint32_t UNI_SUR_LOW_END      = 0x0000DFFF;
    u8string res(size() * 4, '\0');
    size_t pos = 0;
    uint32_t ch, ch2;
    for (size_t i = 0; i < size(); ++i) {
        ch = data()[i];
        // If we have a surrogate pair, convert to UTF32 first.
        if (ch >= UNI_SUR_HIGH_START and ch <= UNI_SUR_HIGH_END) {
            // If the 16 bits following the high surrogate are in the source buffer...
            if (i + 1 < size()) {
                ch2 = data()[++i];
                // If it's a low surrogate, convert to UTF32.
                if (ch2 >= UNI_SUR_LOW_START and ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
                        + (ch2 - UNI_SUR_LOW_START) + halfBase;
                }
                // it's an unpaired high surrogate
                else if (strict) {
                    res[pos++] = '\4';
                    break;
                }
            }
            else { // We don't have the 16 bits following the high surrogate.
                res[pos++] = '\4';
                break;
            }
        }
        else if (strict) {
            // UTF-16 surrogate values are illegal in UTF-32
            if (ch >= UNI_SUR_LOW_START and ch <= UNI_SUR_LOW_END) {
                res[pos++] = '\4';
                break;
            }
        }
        pos += u8string_view::encode(ch, &res[pos]);
    }
    res.resize(pos);
    return res;
}
_CONSTEXPR20 u32string u16string_view::toUtf32(const bool strict) const {
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
        if (ch >= UNI_SUR_HIGH_START and ch <= UNI_SUR_HIGH_END) {
            // If the 16 bits following the high surrogate are in the source buffer...
            if (i + 1 < size()) {
                ch2 = data()[++i];
                // If it's a low surrogate, convert to UTF32.
                if (ch2 >= UNI_SUR_LOW_START and ch2 <= UNI_SUR_LOW_END) {
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
            if (ch >= UNI_SUR_LOW_START and ch <= UNI_SUR_LOW_END) {
                res.push_back('\4');
                break;
            }
        }
        res.push_back(ch);
    }
    return res;
}

_CONSTEXPR14 size_t u16string_view::size_B() const noexcept {
    return size() * sizeof(char16_t);
}
_CONSTEXPR14 size_t u16string_view::size_cu() const noexcept {
    return size();
}
_CONSTEXPR14 size_t u16string_view::size_cp() const noexcept {
    size_t count = 0;
    for (size_t i = 0; i < size(); ++i) {
        const char16_t ch = data()[i];
        if (ch >= 0xD800 and ch <= 0xDBFF) {
            ++i;
        }
        ++count;
    }
    return count;
}


_CONSTEXPR20 u8string u16string::toUtf8(const bool strict) const {
    return u16string_view(*this).toUtf8(strict);
}
_CONSTEXPR20 u32string u16string::toUtf32(const bool strict) const {
    return u16string_view(*this).toUtf32(strict);
}

_CONSTEXPR20 size_t u16string::size_B() const noexcept {
    return size() * sizeof(char16_t);
}
_CONSTEXPR20 size_t u16string::size_cu() const noexcept {
    return size();
}
_CONSTEXPR20 size_t u16string::size_cp() const noexcept {
    return u16string_view(*this).size_cp();
}


_CONSTEXPR20 u32string_view::u32string_view(const u32string& str) noexcept
    : u32string_view(str.c_str(), str.size()) {}

_CONSTEXPR20 u8string u32string_view::toUtf8(const bool strict) const {
    constexpr uint32_t UNI_SUR_HIGH_START   = 0x0000D800;
    constexpr uint32_t UNI_SUR_LOW_END      = 0x0000DFFF;
    u8string res(size() * 4, '\0');
    size_t pos = 0;
    for (uint32_t i = 0; i < size(); ++i) {
        uint32_t ch = data()[i];
        if (strict) {
            // UTF-16 surrogate values are illegal in UTF-32
            if (ch >= UNI_SUR_HIGH_START and ch <= UNI_SUR_LOW_END) {
                res[pos++] = '\4';
                break;
            }
        }
        pos += u8string_view::encode(ch, &res[pos]);
    }
    res.resize(pos);
    return res;
}
_CONSTEXPR20 u16string u32string_view::toUtf16(const bool strict) const {
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
            if (ch >= UNI_SUR_HIGH_START and ch <= UNI_SUR_LOW_END) {
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

_CONSTEXPR14 size_t u32string_view::size_B() const noexcept {
    return size() * sizeof(char32_t);
}
_CONSTEXPR14 size_t u32string_view::size_cu() const noexcept {
    return size();
}
_CONSTEXPR14 size_t u32string_view::size_cp() const noexcept {
    return size();
}


_CONSTEXPR20 u8string u32string::toUtf8(const bool strict) const {
    return u32string_view(*this).toUtf8(strict);
}
_CONSTEXPR20 u16string u32string::toUtf16(const bool strict) const {
    return u32string_view(*this).toUtf16(strict);
}

_CONSTEXPR20 size_t u32string::size_B() const noexcept {
    return size() * sizeof(char32_t);
}
_CONSTEXPR20 size_t u32string::size_cu() const noexcept {
    return size();
}
_CONSTEXPR20 size_t u32string::size_cp() const noexcept {
    return size();
}


#endif // CPP_STRING_UTILS
