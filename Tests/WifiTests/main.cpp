#include "stdafx.h"

#ifndef WLAN_PROFILE_GET_PLAINTEXT_KEY
#define WLAN_PROFILE_GET_PLAINTEXT_KEY 4
#endif

#define LOG_COLOR_WHITE 7
#define LOG_COLOR_GREEN 10
#define LOG_COLOR_YELLOW 14
#define LOG_COLOR_MAGENTA 13
#define LOG_COLOR_CIAN 11

static std::wstring StringToHexString(const std::wstring& cs)
{
    std::wstring ret;
    for (size_t i = 0; i < cs.size(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(cs[static_cast<int>(i)]);
        wchar_t tmp[16];
        swprintf_s(tmp, 16, L"%hX", c);
        ret += tmp;
    }
    return ret;
}

namespace
{
    int g_failed = 0;
    int g_passed = 0;

    void Expect(bool cond, const char* name)
    {
        if (cond)
        {
            ++g_passed;
            std::cout << "PASS " << name << std::endl;
        }
        else
        {
            ++g_failed;
            std::cout << "FAIL " << name << std::endl;
        }
    }
}

int main()
{
    Expect(WLAN_PROFILE_GET_PLAINTEXT_KEY == 4, "WLAN plaintext key flag");
    Expect(PST_KEY_CURRENT_USER == 0x00000000, "PStore current-user key");
    Expect(PST_KEY_LOCAL_MACHINE == 0x00000001, "PStore local-machine key");
    Expect(PST_CF_NONE == 0x00000001, "PStore silent confirmation");
    Expect(LOG_COLOR_CIAN == 11 && LOG_COLOR_YELLOW == 14, "console log colors");
    Expect(StringToHexString(L"A") == L"41", "hex dump of ASCII A");
    Expect(StringToHexString(L"Wi") == L"5769", "hex dump of Wi");

    std::cout << g_passed << " passed, " << g_failed << " failed" << std::endl;
    return g_failed == 0 ? 0 : 1;
}
