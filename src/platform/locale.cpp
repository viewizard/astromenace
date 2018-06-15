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
static std::string GetUserLocale()
{
	return std::string{};
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

	std::string UserLocale =  GetUserLocale();
	if (UserLocale.empty())
		return 0;

	return 0;
}
