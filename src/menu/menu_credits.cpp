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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../game.h"

namespace {

const std::string CreditsArray[] {
	"PROJECT LEADER",
	"Mikhail Kurinnoi",
	"",
	"2D ARTWORK",
	"Tatiana Savitskaya",
	"",
	"3D MODELS",
	"Max Shelekhov, 3drt.com",
	"Oleg Linkov",
	"Egor Kurteanu",
	"",
	"PLANETARY TEXTURES",
	"Alex (Esgaroth) Reuter",
	"Robert Stein",
	"",
	"MUSIC",
	"Alexander Matchugovsky",
	"",
	"SOUND EFFECTS",
	"Mikhail Sirotkin",
	"Alexander Matchugovsky",
	"Alexander Vassilyevsky",
	"",
	"VOICE",
	"Petra Schulten (de)",
	"Elena Zavalishina (en, ru)",
	"",
	"WEB PROGRAMMING",
	"Pavel Voylov",
	"",
	"TRANSLATORS",
	"Bernd Dau (de)",
	"Mikhail Kurinnoi (en, ru)",
	"Jacek Fiedorowicz (pl)",
	"",
	"TESTERS",
	"Dmitriy Schigolev",
	"Ivan Bobrov",
	"Vadim Sosenko",
	"Dmitriy Tatarinov",
	"Alexey Mosesov",
	"Bernd Dau",
	"Ryan Foster",
	"Jason Crowell",
	"",
	"SPECIAL THANKS TO",
	"Richard Benson",
	"Alexey Madalinskiy",
	"Olga Zhuravleva",
	"Nick O`Connell",
	"Mikhail Storcheous",
	"Boris Pek",
	"",
	"",
	"",
	" ", /* this is correct, we need non empty line for white color for next line */
	"Copyright © 2007-2018, Viewizard",
	"http://www.viewizard.com"
};
constexpr int CreditsArrayCount = sizeof(CreditsArray)/sizeof(CreditsArray[0]);

float CurrentTextPos{0.0f};
float LastUpdate{0.0f};

} // unnamed namespace


/*
 * Credits menu initialization.
 */
void InitCreditsMenu(float Time)
{
	CurrentTextPos = 0.0f;
	LastUpdate = Time;
}

/*
 * Credits menu (draw and interaction).
 */
void CreditsMenu()
{
	// text animation, about 40 pixels in second
	CurrentTextPos -= 40.0f * (vw_GetTimeThread(0) - LastUpdate);
	LastUpdate = vw_GetTimeThread(0);

	// 690 - start inside internal fixed size of screen (768) with "offset"
	int TextY = 690 + (int)CurrentTextPos;
	int Offset = 30;

	// cycling (internal fixed size of screen (768) - "borders" + CreditsArrayCount * Offset)
	if (CurrentTextPos <= -(550 + CreditsArrayCount * Offset))
		CurrentTextPos = 0.0f;

	// we switch white/yellow colors, so, we need change only "blue" part of RGB
	float ColorB = 0.0f;
	for (unsigned long i = 0; i < CreditsArrayCount; i++) {
		float tmpTransp = 0.0f;
		if (TextY > 300) // fade-in
			tmpTransp = (650 - TextY) / 100.0f;
		else // fade-out
			tmpTransp = (TextY - 100) / 100.0f;
		vw_Clamp(tmpTransp, 0.0f, 1.0f);

		if (!CreditsArray[i].empty()) {
			const char *tmpText = vw_GetText(CreditsArray[i].c_str());
			int CenteredX = (Setup.iAspectRatioWidth - vw_FontSize(tmpText)) / 2;
			vw_DrawFont(CenteredX, TextY, 0, 0, 1.0f,
				    1.0f, 1.0f, ColorB, tmpTransp * MenuContentTransp,
				    tmpText);
		}
		TextY += Offset;

		// if current line is empty, next one will be yellow, otherwise - white
		ColorB = CreditsArray[i].empty() ? 0.0f : 1.0f;
	}

	int X = (Setup.iAspectRatioWidth - 384) / 2;
	int Y = 165 + 100 * 5;
	if (DrawButton384(X, Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime)) {
		ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;
		StartMusicWithFade(eMusicTheme::MENU, 2000, 2000);
	}
}
