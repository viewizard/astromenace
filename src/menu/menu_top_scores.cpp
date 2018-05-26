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
#include "../config/config.h"

namespace {

struct sTopScore {
	std::u32string NameUTF32{};
	int Score{0};
	std::u32string ScoreUTF32{};

	sTopScore(const std::string &_Name, int _Score) :
		Score{_Score}
	{
		NameUTF32 = ConvertUTF8.from_bytes(_Name);
		ScoreUTF32 = ConvertUTF8.from_bytes(std::to_string(Score));
	}

	bool operator > (const sTopScore &B) const
	{
		return Score > B.Score;
	}
};

std::list<sTopScore> DefaultTopScores{{"Viewizard", 100000},
				      {"Alex", 90000},
				      {"Michael", 80000},
				      {"Jeffrey", 70000},
				      {"Christopher", 60000},
				      {"Becky", 40000},
				      {"Greg", 20000},
				      {"Jay", 10000},
				      {"Kelvin", 5000},
				      {"Stephan", 1000}};

std::list<sTopScore> TopScoresList{};

} // unnamed namespace


/*
 * Top Scores menu initialization.
 */
void InitTopScoresMenu()
{
	if (!TopScoresList.empty())
		TopScoresList.clear();

	TopScoresList = DefaultTopScores;

	for (unsigned i = 0; i < config::MAX_PROFILES; i++) {
		if (GameConfig().Profile[i].Used &&
		    (GameConfig().Profile[i].Experience > 0))
			TopScoresList.emplace_back(GameConfig().Profile[i].Name,
						   GameConfig().Profile[i].Experience);
	}

	TopScoresList.sort(std::greater<sTopScore>());

	std::list<sTopScore>::iterator iterEraseStart = TopScoresList.begin();
	std::advance(iterEraseStart, DefaultTopScores.size());
	// we don't check iterator here, since we know for sure, that TopScoresList
	// have at least DefaultTopScores.size() of elements
	TopScoresList.erase(iterEraseStart, TopScoresList.end());
}

/*
 * Draw Top Scores menu.
 */
void TopScoresMenu()
{
	sRECT SrcRect, DstRect;
	SrcRect(0, 0, 2, 2);
	DstRect(0, 0, GameConfig().InternalWidth, 768);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f * MenuContentTransp);


	int X1 = GameConfig().InternalWidth / 2 - 362;
	int Y1 = 165;
	int Prir1 = 42;

	vw_DrawText(X1, Y1, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("NAME"));
	vw_DrawText(X1 + 650, Y1, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("SCORE"));

	Y1 += 10;

	for (auto &tmpTopScore : TopScoresList) {
		Y1 += Prir1;

		vw_DrawTextUTF32(X1, Y1, 0, 530, 1.0f, eRGBCOLOR::white, MenuContentTransp, tmpTopScore.NameUTF32.c_str());
		vw_DrawTextUTF32(X1 + 650, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, tmpTopScore.ScoreUTF32.c_str());
	}

	int X = (GameConfig().InternalWidth - 384) / 2;
	int Y = 165 + 100 * 5;
	if (DrawButton384(X,Y, vw_GetText("MAIN MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;
}
