#include "../string_utils.hpp"
#include <cassert>


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
    assert(u8string(u8"").empty());
    assert(u8string(null).empty());
    assert(u8string(null, 0).empty());
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
    assert(u8string_view(u8"").empty());
    assert(u8string_view(null).empty());
    assert(u8string_view(null, 0).empty());
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
        assert(!result.empty());
        assert(result == "12.34");
    }
    {
        u8string buffer(32, '\0');
        u8string_view result = utils::to_string(0xDEADBEEF, buffer, true);
        assert(!result.empty());
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
        u8string_view result = utils::trimm("\n 12.34 \t");
        assert(result == "12.34");
    }
    {
        utils::split(u8"|12||🌍|34|5\\|6|", "|", [](u8string_view part, uint32_t idx) {
            switch (idx) {
            case 0: assert(part == "12"); break;
            case 1: assert(part == u8"🌍"); break;
            case 2: assert(part == "34"); break;
            case 3: assert(part == "5\\|6"); break;
            default: assert(false); break;
            }
        });
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
        u8string u8str;
        u16string u16str;
        u32string u32str;

        //u8str = u8"🌍\0";
        //assert(u8str.size() == 5);
        //assert(u8str.length() == 2);

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
    }
    return 0;
}
