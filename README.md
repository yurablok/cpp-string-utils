# C++ String Utils

C++17 and C++11 `std::string_view`-based UTF-8-based utils.  
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
u8string_view result = utils::trimm("\n 12.34 \t");
assert(result == "12.34");
```

## `split`
```cpp
utils::split(u8"|12||🌍|34|5\\|6|", "|", [](u8string_view part, uint32_t idx) {
    switch (idx) {
    case 0: assert(part == "12"); break;
    case 1: assert(part == u8"🌍"); break;
    case 2: assert(part == "34"); break;
    case 3: assert(part == "5\\|6"); break;
    default: assert(false); break;
    }
});
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
assert(u8str.size() == 4);
assert(u8str.length() == 1);
assert(u8str.sso_active());

u8str = u8"你好 Hola Hello Olá Привет こんにちは 안녕하세요 Bonjour Hallo Ciao";
assert(u8str.size() == 86);
assert(u8str.length() == 55);
assert(!u8str.sso_active());

u16str = u8str.toUtf16();
assert(u16str.toUtf8() == u8str);
assert(u16str.toUtf32().toUtf8() == u8str);

u32str = u8str.toUtf32();
assert(u32str.toUtf8() == u8str);
assert(u32str.toUtf16().toUtf8() == u8str);
```
