# C++ String Utils

C++17 and C++11 `std::string_view`-based utils.

## `to_string`
```cpp
std::string buffer(32, '\0');
std::string_view result = utils::to_string(12.34f, buffer);
assert(!result.empty());
assert(result == "12.34");
```
```cpp
std::string buffer(32, '\0');
std::string_view result = utils::to_string(0xDEADBEEF, buffer, true);
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
std::string_view result = utils::trimm("\n 12.34 \t");
assert(result == "12.34");
```

## `split`
```cpp
utils::split("|12||34|5\\|6|", "|", [](std::string_view part, uint32_t idx) {
    switch (idx) {
    case 0: assert(part == "12"); break;
    case 1: assert(part == "34"); break;
    case 2: assert(part == "5\\|6"); break;
    default: assert(false); break;
    }
});
```

## `substr`
```cpp
constexpr std::string_view str = "user@email.com";
uint32_t offset = 0;
std::string_view user = utils::substr(str, offset, "@");
assert(user == "user");
std::string_view email = utils::substr(str, offset, ".");
assert(email == "email");
std::string_view com = utils::substr(str, offset, ".");
assert(com == "com");
```
