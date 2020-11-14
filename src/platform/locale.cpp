/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


    AstroMenace is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AstroMenace is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

// TODO add macOS support

/*

Generally, the pattern <language>-<REGION> is used. Here, language is a lowercase ISO 639 language code.
The codes from ISO 639-1 are used when available. Otherwise, codes from ISO 639-2/T are used. REGION
specifies an uppercase ISO 3166-1 country/region identifier. For example, the locale name for English
(United States) is "en-US" and the locale name for Divehi (Maldives) is "dv-MV".

* A language name never includes a sort order. Thus, the language name corresponding to the locale name
"hu-HU_technl" (Hungarian language, Hungary, with technical sort) is just "hu-HU".

* A language name can be "neutral" with respect to locale. For example, "en" (English) is a valid neutral
language name, as distinct from "en-BZ" (English, Belize).

*/

#include "../core/core.h"
#include "platform.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Get user preferred locale.
 */
static bool GetUserLocale(std::string &NeutralLocale, std::string &RegionalLocale)
{
    NeutralLocale.clear();
    RegionalLocale.clear();

#ifdef __unix
    // LC_ALL > LC_MESSAGES > LANG
    const char *LangEnv = SDL_getenv("LC_ALL");

    if (!LangEnv) {
        LangEnv = SDL_getenv("LC_MESSAGES");
    }

    if (!LangEnv) {
        LangEnv = SDL_getenv("LANG");
    }

    if (!LangEnv) {
        return false;
    }

    // C, POSIX
    // en, cmn, ...
    // en_US, en_GB, ...
    // en@boldquot, ca@valencia, ca_ES@valencia, ...
    // en_US.iso88591, en_US.utf8, ...
    std::string tmpLangEnv{LangEnv};

    if (tmpLangEnv == "C" || tmpLangEnv == "POSIX") {
        return false;
    }

    // chop off encoding, if specified
    std::string::size_type tmpPos = tmpLangEnv.find('.');
    if (tmpPos != std::string::npos) {
        tmpLangEnv.resize(tmpPos);
    }

    // chop off extra bits, if specified
    tmpPos = tmpLangEnv.find('@');
    if (tmpPos != std::string::npos) {
        tmpLangEnv.resize(tmpPos);
    }

    tmpPos = tmpLangEnv.find('_');
    if (tmpPos != std::string::npos) {
        NeutralLocale = tmpLangEnv.substr(0, tmpPos);
        tmpLangEnv[tmpPos] = '-'; // en_US -> en-US
        RegionalLocale = tmpLangEnv;
    } else {
        NeutralLocale = tmpLangEnv;
    }
#elif defined(WIN32)
    // note, only windows vista or newer are supported

#ifndef MUI_LANGUAGE_NAME
#define MUI_LANGUAGE_NAME 0x8 // Use ISO language (culture) name convention
#endif
#ifndef PZZWSTR
#define PZZWSTR WCHAR*
#endif

    HMODULE hKernelDLL = LoadLibrary("kernel32.dll");
    if (!hKernelDLL) {
        return false;
    }

    auto ReleaseDLLAndExitWithFalse = [hKernelDLL] () {
        FreeLibrary(hKernelDLL);
        return false;
    };

    typedef BOOL (WINAPI *GETUSERPREFERREDUILANGUAGES)(DWORD, PULONG, PZZWSTR, PULONG);
    GETUSERPREFERREDUILANGUAGES pGetUserPreferredUILanguages{nullptr};

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
    pGetUserPreferredUILanguages = (GETUSERPREFERREDUILANGUAGES) GetProcAddress(hKernelDLL, "GetUserPreferredUILanguages");
    if (!pGetUserPreferredUILanguages) {
        return ReleaseDLLAndExitWithFalse();
    }
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

    ULONG numLanguages{0};
    DWORD cchLanguagesBuffer{0};
    if (!pGetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, nullptr, &cchLanguagesBuffer)) {
        return ReleaseDLLAndExitWithFalse();
    }

    // buffer in which function retrieves an ordered, null-delimited user preferred UI languages list,
    // this list ends with two null characters
    std::vector<wchar_t> pwszLanguagesBuffer{};
    pwszLanguagesBuffer.resize(cchLanguagesBuffer);
    if (!pGetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, pwszLanguagesBuffer.data(), &cchLanguagesBuffer)) {
        return ReleaseDLLAndExitWithFalse();
    }

    // temporary wchar buffer with first language only (we have at least one '\0' for sure)
    auto tmpIter = std::find(pwszLanguagesBuffer.cbegin(), pwszLanguagesBuffer.cend(), '\0');
    std::wstring tmpWLang{pwszLanguagesBuffer.cbegin(), tmpIter};

    // convert wchar string to char string
    std::string tmpLang{};
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter_wchar;
    tmpLang = converter_wchar.to_bytes(tmpWLang);

    std::string::size_type tmpPos = tmpLang.find('-');
    if (tmpPos != std::string::npos) {
        NeutralLocale = tmpLang.substr(0, tmpPos);
        RegionalLocale = tmpLang;
    } else {
        NeutralLocale = tmpLang;
    }

    FreeLibrary(hKernelDLL);

#endif

    return (!NeutralLocale.empty() || !RegionalLocale.empty());
}

/*
 * Find appropriate UI language by user preferred locale.
 * Note, this function should be called after vw_InitText().
 */
unsigned int FindPreferredLanguageByLocale()
{
    // first language (index 0) is English, so, we don't count it
    if (vw_GetLanguageListCount() < 2) {
        return 0;
    }

    std::string NeutralLocale{}; // en, ru, de, ...
    std::string RegionalLocale{}; // en-US, ru-RU, dv-MV, ...
    if (!GetUserLocale(NeutralLocale, RegionalLocale)) {
        return 0;
    }

    // check regional locale
    if (!RegionalLocale.empty()) {
        for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
            if (vw_GetText("en", i) == RegionalLocale) {
                return i;
            }
        }
    }

    // fallback to "neutral" locale
    if (!NeutralLocale.empty()) {
        for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
            if (vw_GetText("en", i) == NeutralLocale) {
                return i;
            }
        }
    }

    return 0;
}

} // astromenace namespace
} // viewizard namespace
