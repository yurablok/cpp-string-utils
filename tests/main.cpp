#include "../string_utils.hpp"
#include <cassert>
#include <map>
#include <regex>
#include <array>


int32_t main() {
    const char* const null = nullptr;
    std::string str = reinterpret_cast<const char*>(u8"🌍");
    std::string_view strv = reinterpret_cast<const char*>(u8"🌍");
    u8string u8str = u8"🌍";
    u8string_view u8strv = u8"🌍";
# if defined(__cpp_char8_t)
    std::u8string su8str = u8"🌍";
    std::u8string_view su8strv = u8"🌍";
    const char8_t* const u8null = nullptr;
# endif

    assert(u8string("").empty());
    assert(u8string("", 0).empty());
    assert(u8string(u8"").empty());
    assert(u8string(u8"", 0).empty());
    assert(u8string(null).empty());
    assert(u8string(null, 0).empty());
    assert(u8string(static_cast<void*>(nullptr), 0).empty());
    assert(u8string(str) != null);
    assert(u8string(str) != "o");
    assert(u8string(str) == u8"🌍");
    assert(u8string(str) == str);
    assert(u8string(strv) == strv);
    assert(u8string(u8str) == u8str);
    assert(u8string(u8strv) == u8strv);
# if defined(__cpp_char8_t)
    assert(u8string(su8str) == su8str);
    assert(u8string(su8strv) == su8strv);
    assert(u8string(str) != u8null);
    assert(u8string(u8null).empty());
    assert(u8string(u8null, 0).empty());
# endif

    assert(u8string_view("").empty());
    assert(u8string_view("", 0).empty());
    assert(u8string_view(u8"").empty());
    assert(u8string_view(u8"", 0).empty());
    assert(u8string_view(null).empty());
    assert(u8string_view(null, 0).empty());
    assert(u8string_view(static_cast<void*>(nullptr), 0).empty());
    assert(u8string_view(str) != null);
    assert(u8string_view(str) != "o");
    assert(u8string_view(str) == u8"🌍");
    assert(u8string_view(str) == str);
    assert(u8string_view(strv) == strv);
    assert(u8string_view(u8str) == u8str);
    assert(u8string_view(u8strv) == u8strv);
# if defined(__cpp_char8_t)
    assert(u8string_view(su8str) == su8str);
    assert(u8string_view(su8strv) == su8strv);
    assert(u8string_view(str) != u8null);
    assert(u8string_view(u8null).empty());
    assert(u8string_view(u8null, 0).empty());
# endif

    u8str = null;
    u8str = "o";
    u8str = u8"🌍";
    u8str = str;
    u8str = std::move(str);
    u8str = strv;
    u8str = std::move(strv);
    u8str = u8str;
    u8str = std::move(u8str);
    u8str = u8strv;
    u8str = std::move(u8strv);
# if defined(__cpp_char8_t)
    u8str = su8str;
    u8str = std::move(su8str);
    u8str = su8strv;
    u8str = std::move(su8strv);
    u8str = u8null;
# endif

    u8strv = null;
    u8strv = "o";
    u8strv = u8"🌍";
    u8strv = str;
    u8strv = strv;
    u8strv = u8str;
    u8strv = u8strv;
# if defined(__cpp_char8_t)
    u8strv = su8str;
    u8strv = su8strv;
    u8strv = u8null;
# endif

    u8str += null;
    u8str += "o";
    u8str += u8"🌍";
    u8str += str;
    u8str += strv;
    u8str += u8str;
    u8str += u8strv;
# if defined(__cpp_char8_t)
    u8str += su8str;
    u8str += su8strv;
    u8str += u8null;
# endif
    {
        u8string buffer(32, '\0');
        u8string_view result = utils::to_string(12.34f, buffer);
        assert(not result.empty());
        assert(result == "12.34");
    }
    {
        u8string buffer(32, '\0');
        u8string_view result = utils::to_string(0xDEADBEEF, buffer, true);
        assert(not result.empty());
        assert(result == "deadbeef");
    }
    {
        float f32 = 0.0f;
        assert(utils::from_string("12.34", f32));
        assert(f32 == 12.34f);
    }
    {
        uint32_t u32 = 0;
        assert(utils::from_string("DEADBEEF", u32, true));
        assert(u32 == 0xDEADBEEF);
        assert(utils::from_string("deadbeef", u32, true));
        assert(u32 == 0xDEADBEEF);
    }
    {
        assert(u8string_view().trim().empty());
        assert(u8string_view("  ").trim().empty());
        assert(u8string_view("  ").trim("", false) == "  ");
        assert(u8string_view("\n 12.34 \t").trim() == "12.34");
        assert(u8string_view(u8" 😀 ").trim() == u8"😀");
        assert(u8string_view("  ").trim('<', '>') == "");
        assert(u8string_view(" @ ").trim('<', '>') == "@");
        assert(u8string_view(u8" | 🌍 | ").trim("|") == u8"🌍");
        assert(u8string_view(u8"| 🌍 |").trim("|", false) == u8" 🌍 ");
        assert(u8string_view(u8R"( <😀> <\>😀\<> \ 🌍\  <\>😀\<> </😀> )")
            .trim('<', '>') == u8R"(\ 🌍\ )");
        //assert(u8string_view(u8R"( <😀\>">\""> <\>😀\<> \ 🌍\  <\>😀\<> </😀> )")
        //    .trim('<', '>') == u8R"(\ 🌍\ )");
    }
    {
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
    }
    {
        constexpr u8string_view str = "user@email.com";
        size_t offset = 0;
        u8string_view user = utils::substr(str, offset, "@");
        assert(user == "user");
        u8string_view email = utils::substr(str, offset, ".");
        assert(email == "email");
        u8string_view com = utils::substr(str, offset, ".");
        assert(com == "com");
    }
    {
        u16string u16str;
        u32string u32str;

        //u8str = u8"🌍\0";
        //assert(u8str.size() == 5);
        //assert(u8str.length() == 2);

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

        //assert(u8str.at_cp(20) == U'П');
        assert(u8string_view(u8str).at_cp(20) == U'П');
        size_t idx = 0;
        for (auto it = u8str.begin_cp(); it != u8str.end_cp(); ++it) {
            if (idx == 18) {
                assert(*it == U'😀');
            }
            else if (idx == 20) {
                assert(*it == U'П');
            }
            ++idx;
        }
        assert(u16str[21] == u'П');
        assert(u32str[20] == U'П');
    }
    {
        u8str.clear();
        u8str.push_back_cp('1');
        u8str.push_back_cp(U'🌍');
        assert(u8str.size_cu() == 5);
        assert(u8str.size_cp() == 2);
        assert(u8str.front_cp() == '1');
        assert(u8str.back_cp() == U'🌍');
        u8str.pop_back_cp();
        assert(u8str.size_cu() == 1);
        assert(u8str.size_cp() == 1);
        assert(u8str.back_cp() == '1');
    }
    {
        u8strv = "ABCDEFGHIJ";
        assert(u8strv.slice() == u8strv);
        assert(u8strv.slice(3) == "DEFGHIJ");
        assert(u8strv.slice(4, 4).empty());
        assert(u8strv.slice(5, 2).empty());
        assert(u8strv.slice(5, 100) == "FGHIJ");
        assert(u8strv.slice(0, 4) == "ABCD");
        assert(u8strv.slice(2, 6) == "CDEF");
        assert(u8strv.slice(-3) == "HIJ");
        assert(u8strv.slice(0, -2) == "ABCDEFGH");
        assert(u8strv.slice(-5, -2) == "FGH");
        assert(u8strv.slice(5, 100) == "FGHIJ");

        u8strv = u8"A🌍CДEФGH😀J";
        assert(u8strv.slice_cp() == u8strv);
        assert(u8strv.slice_cp(3) == u8"ДEФGH😀J");
        assert(u8strv.slice_cp(4, 4).empty());
        assert(u8strv.slice_cp(5, 2).empty());
        assert(u8strv.slice_cp(5, 100) == u8"ФGH😀J");
        assert(u8strv.slice_cp(0, 4) == u8"A🌍CД");
        assert(u8strv.slice_cp(2, 6) == u8"CДEФ");
        assert(u8strv.slice_cp(-3) == u8"H😀J");
        assert(u8strv.slice_cp(0, -2) == u8"A🌍CДEФGH");
        assert(u8strv.slice_cp(-5, -2) == u8"ФGH");
        assert(u8strv.slice_cp(5, 100) == u8"ФGH😀J");
    }
    {
        std::map<u8string, int32_t> map;
        map["hello"] = 1;
        map[u8"äbc"] = 2;
        map[u8"你好"] = 3;
        map[u8"🌍"] = 4;

        assert(map[std::string("hello")] == 1);
        assert(map[u8"äbc"] == 2);
        assert(map[u8"你好"] == 3);
        assert(map[u8"🌍"] == 4);

        auto it = map.begin();
        assert(it->first == "hello"); ++it;
        assert(it->first == u8"äbc"); ++it;
    }
    {
        std::map<u8string_view, int32_t> map;
        map["hello"] = 1;
        map[u8"äbc"] = 2;
        map[u8"你好"] = 3;
        map[u8"🌍"] = 4;

        assert(map[std::string("hello")] == 1);
        assert(map[u8"äbc"] == 2);
        assert(map[u8"你好"] == 3);
        assert(map[u8"🌍"] == 4);

        auto it = map.begin();
        assert(it->first == "hello"); ++it;
        assert(it->first == u8"äbc"); ++it;
    }
    {
        std::regex re(u8string(u8"ä+"));
        assert(std::regex_search(u8string(u8"äbc"), re));
        assert(not std::regex_search("abc", re));
    }
    return 0;
}
