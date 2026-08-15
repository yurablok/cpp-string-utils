# C++ String Utils

C++17 and C++11 `std::string`-and-`std::string_view`-based UTF-8-based utils.  
https://utf8everywhere.org/

## `to_string`
```cpp
u8string buffer(32, '\0');
u8string_view result = utils::to_string(12.34f, buffer);
assert(!result.empty());
assert(result == "12.34");
```
```cpp
u8string buffer(32, '\0');
u8string_view result = utils::to_string(0xDEADBEEF, buffer, true);
assert(!result.empty());
assert(result == "deadbeef");
```

## `from_string`
```cpp
float f32 = 0.0f;
assert(utils::from_string("12.34", f32));
assert(f32 == 12.34f);
```
```cpp
uint32_t u32 = 0;
assert(utils::from_string("DEADBEEF", u32, true));
assert(u32 == 0xDEADBEEF);
assert(utils::from_string("deadbeef", u32, true));
assert(u32 == 0xDEADBEEF);
```

## `trimm`
```cpp
assert(u8string_view("\n 12.34 \t").trim() == "12.34")
assert(u8string_view(u8" | 🌍 | ").trim("|") == u8"🌍");
assert(u8string_view(u8"<span\>>😀</span>").trim('<', '>') == u8"😀");
```

## `split`
```cpp
u8string_view(u8"|12||🌍|34|5\\|6|").split("|", [](u8string_view part, uint32_t idx) {
    switch (idx) {
    case 0: assert(part == "12"); break;
    case 1: assert(part == u8"🌍"); break;
    case 2: assert(part == "34"); break;
    case 3: assert(part == "5\\|6"); break;
    default: assert(false); break;
    }
});

std::array<u8string_view, 4> parts;
u8string_view(u8"|12||🌍|34|5\\|6|").split("|", parts);
assert(parts[0] == "12");
assert(parts[1] == u8"🌍");
assert(parts[2] == "34");
assert(parts[3] == "5\\|6");
```

## `slice`
```cpp
u8string_view u8strv = "ABCDEFGHIJ";
assert(u8strv.slice(2, 6) == "CDEF");
assert(u8strv.slice(-3) == "HIJ");
assert(u8strv.slice(0, -2) == "ABCDEFGH");
assert(u8strv.slice(-5, -2) == "FGH");

u8strv = u8"A🌍CДEФGH😀J";
assert(u8strv.slice_cp(2, 6) == u8"CДEФ");
assert(u8strv.slice_cp(-3) == u8"H😀J");
assert(u8strv.slice_cp(0, -2) == u8"A🌍CДEФGH");
assert(u8strv.slice_cp(-5, -2) == u8"ФGH");
```

## `substr`
```cpp
constexpr u8string_view str = "user@email.com";
size_t offset = 0;
u8string_view user = utils::substr(str, offset, "@");
assert(user == "user");
u8string_view email = utils::substr(str, offset, ".");
assert(email == "email");
u8string_view com = utils::substr(str, offset, ".");
assert(com == "com");
```

## conversion
```cpp
u8string u8str;
u16string u16str;
u32string u32str;

u8str = u8"🌍";
assert(u8str.size_B() == 4);
assert(u8str.size_cu() == 4);
assert(u8str.size_cp() == 1);

u8str = u8"你好 Hola Hello Olá 😀 Привет こんにちは 안녕하세요 Bonjour Hallo Ciao";
assert(u8str.size_B() == 91);
assert(u8str.size_cu() == 91);
assert(u8str.size_cp() == 57);

u16str = u8str.toUtf16();
assert(u16str.size_B() == 116);
assert(u16str.size_cu() == 58);
assert(u16str.size_cp() == 57);
assert(u16str.toUtf8() == u8str);
assert(u16str.toUtf32().toUtf8() == u8str);

u32str = u8str.toUtf32();
assert(u32str.size_B() == 228);
assert(u32str.size_cu() == 57);
assert(u32str.size_cp() == 57);
assert(u32str.toUtf8() == u8str);
assert(u32str.toUtf16().toUtf8() == u8str);
```

## `std::string`-compatibility
```cpp
std::map<u8string, int32_t> map;
std::map<u8string_view, int32_t> map;
std::unordered_map<u8string, int32_t> map;
std::unordered_map<u8string_view, int32_t> map;

std::regex re(u8string(u8"ä+"));
assert(std::regex_search(u8string(u8"äbc"), re));
assert(not std::regex_search("abc", re));

etc...
```
