// C++ String Utils
//
// C++17 and C++11 std::string_view-based utils.
//
// Author: Yurii Blok
// License: BSL-1.0
// https://github.com/yurablok/cpp-string-utils
// History:
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
#   if defined(__GNUG__)
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
#endif

#if defined(CPP_STRING_UTILS_LIB_CHARCONV)
#   include <string_view>
#   include <charconv>
#else
#   include "string_view.hpp" // https://github.com/martinmoene/string-view-lite
namespace std {
    using string_view = nonstd::string_view;
}
#endif
#if !defined(CPP_STRING_UTILS_LIB_CHARCONV_FLOAT)
#   define __STDC_FORMAT_MACROS
#   include <cinttypes>
#endif

namespace utils {

inline std::string_view trimm(std::string_view string, const std::string_view by = "\t\n\r ") noexcept {
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

inline void split(const std::string_view str, const std::string_view by,
        const std::function<void(std::string_view part, uint32_t idx)> handler,
        const bool withEmpty = false, const char escape = '\\') noexcept {
    if (by.empty() || !handler) {
        return;
    }
    uint32_t begin = 0;
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
        const std::string_view part = str.substr(begin, i - begin);
        if (withEmpty || !part.empty()) {
            handler(part, idx++);
        }
        begin = i + 1;
    }
    const std::string_view part = str.substr(begin);
    if (!part.empty()) {
        handler(part, idx);
    }
}

inline std::string_view substr(const std::string_view str, uint32_t& offset,
        const std::string_view split_by,
        const bool withEmpty = false, const char escape = '\\') noexcept {
    if (split_by.empty()) {
        return std::string_view();
    }
    if (offset >= str.size()) {
        return std::string_view();
    }
    uint32_t begin = offset;
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
        const std::string_view part = str.substr(begin, offset - begin);
        ++offset;
        if (withEmpty || !part.empty()) {
            return part;
        }
        begin = offset + 1;
    }
    const std::string_view part = str.substr(begin);
    ++offset;
    if (!part.empty()) {
        return part;
    }
    return std::string_view();
}

inline void parseCSV(const std::string_view csv,
        const std::function<void(std::string_view cell, uint32_t idx)> onCell,
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
inline std::string_view to_string(const integer_t number, const std::string_view buffer,
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

inline std::string_view to_string(const int8_t number, const std::string_view buffer) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), "%" PRIi8, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline std::string_view to_string(const uint8_t number, const std::string_view buffer,
        const bool hex = false) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), hex ? "%" PRIx8 : "%" PRIu8, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline std::string_view to_string(const int16_t number, const std::string_view buffer) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), "%" PRIi16, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline std::string_view to_string(const uint16_t number, const std::string_view buffer,
        const bool hex = false) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), hex ? "%" PRIx16 : "%" PRIu16, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline std::string_view to_string(const int32_t number, const std::string_view buffer) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), "%" PRIi32, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline std::string_view to_string(const uint32_t number, const std::string_view buffer,
        const bool hex = false) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), hex ? "%" PRIx32 : "%" PRIu32, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline std::string_view to_string(const int64_t number, const std::string_view buffer) noexcept {
    const int32_t length = std::snprintf(const_cast<char*>(
        buffer.data()), buffer.size(), "%" PRIi64, number);
    if (length <= 0) {
        return {};
    }
    return buffer.substr(0, length);
}
inline std::string_view to_string(const uint64_t number, const std::string_view buffer,
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
    >::type = true>
inline std::string_view to_string(const floating_t number, const std::string_view buffer) noexcept {
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

inline std::string_view to_string(const float number, const std::string_view buffer) noexcept {
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
inline std::string_view to_string(const double number, const std::string_view buffer) noexcept {
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
    typename std::enable_if<
        std::is_integral<integer_t>::value, bool
    >::type = true>
inline bool from_string(const std::string_view string, integer_t& number,
        const bool hex = false) noexcept {
    auto [ptr, ec] = std::from_chars(
        string.data(),
        string.data() + string.size(),
        number,
        hex ? 16 : 10
    );
    if (ptr != string.data() + string.size() || ec != std::errc(0)) {
        return false;
    }
    return true;
}

#else // !CPP_STRING_UTILS_LIB_CHARCONV

inline bool from_string(const std::string_view string, int8_t& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%u" SCNi8,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const std::string_view string, uint8_t& number,
        const bool hex = false) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), hex ? "%%%u" SCNx8 : "%%%u" SCNu8,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const std::string_view string, int16_t& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%u" SCNi16,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const std::string_view string, uint16_t& number,
        const bool hex = false) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), hex ? "%%%u" SCNx16 : "%%%u" SCNu16,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const std::string_view string, int32_t& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%u" SCNi32,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const std::string_view string, uint32_t& number,
        const bool hex = false) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), hex ? "%%%u" SCNx32 : "%%%u" SCNu32,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const std::string_view string, int64_t& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%u" SCNi64,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const std::string_view string, uint64_t& number,
        const bool hex = false) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), hex ? "%%%u" SCNx64 : "%%%u" SCNu64,
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}

#endif

#if defined(CPP_STRING_UTILS_LIB_CHARCONV_FLOAT)

template<typename floating_t,
    typename std::enable_if<
        std::is_floating_point<floating_t>::value, bool
    >::type = true>
inline bool from_string(const std::string_view string, floating_t& number) noexcept {
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

inline bool from_string(const std::string_view string, float& number) noexcept {
    char format[8];
    std::snprintf(format, sizeof(format), "%%%uf",
        static_cast<uint32_t>(string.size()));
    if (std::sscanf(string.data(), format, &number) != 1) {
        return false;
    }
    return true;
}
inline bool from_string(const std::string_view string, double& number) noexcept {
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

#endif // CPP_STRING_UTILS
