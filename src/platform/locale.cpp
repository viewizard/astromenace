/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

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


/*
 * Get user preferred locale.
 */
static bool GetUserLocale(std::string &NeutralLocale, std::string &RegionalLocale)
{
	NeutralLocale.clear();
	RegionalLocale.clear();

#ifdef __unix
	// LC_ALL overrides all other
	const char *LangEnv = SDL_getenv("LC_ALL");

	// LANG environment variable
	if (!LangEnv)
		LangEnv = SDL_getenv("LANG");

	if (!LangEnv)
		return false;

	// C, POSIX
	// en, cmn, ...
	// en_US, en_GB, ...
	// en@boldquot, ca@valencia, ca_ES@valencia, ...
	// en_US.iso88591, en_US.utf8, ...
	std::string tmpLangEnv{LangEnv};

	if ((tmpLangEnv == "C") ||
	    (tmpLangEnv == "POSIX"))
		return false;

	// chop off encoding, if specified
	std::string::size_type tmpPos = tmpLangEnv.find('.');
	if (tmpPos != std::string::npos)
		tmpLangEnv.resize(tmpPos);

	// chop off extra bits, if specified
	tmpPos = tmpLangEnv.find('@');
	if (tmpPos != std::string::npos)
		tmpLangEnv.resize(tmpPos);

	tmpPos = tmpLangEnv.find('_');
	if (tmpPos != std::string::npos) {
		NeutralLocale = tmpLangEnv.substr(0, tmpPos);
		tmpLangEnv[tmpPos] = '-'; // en_US -> en-US
		RegionalLocale = tmpLangEnv;
	} else
		NeutralLocale = tmpLangEnv;
#endif

	return (!NeutralLocale.empty() || !RegionalLocale.empty());
}

/*
 * Find appropriate game language by user preferred locale.
 * Note, this function should be called after vw_InitText().
 */
unsigned int FindPreferredLanguageByLocale()
{
	// first language (index 0) is English, so, we don't count it
	if (vw_GetLanguageListCount() < 2)
		return 0;

	std::string NeutralLocale{}; // en, ru, de, ...
	std::string RegionalLocale{}; // en-US, ru-RU, dv-MV, ...
	if (!GetUserLocale(NeutralLocale, RegionalLocale))
		return 0;

	// check regional locale
	if (!RegionalLocale.empty()) {
		for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
			if (vw_GetText("en", i) == RegionalLocale)
				return i;
		}
	}

	// fallback to "neutral" locale
	if (!NeutralLocale.empty()) {
		for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
			if (vw_GetText("en", i) == NeutralLocale)
				return i;
		}
	}

	return 0;
}
