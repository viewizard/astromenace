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

#include "../core/core.h"
#include "../config/config.h"
#include "../assets/texture.h"
#include "../command.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

struct sTopScore {
    std::u32string NameUTF32{};
    int Score{0};
    std::u32string ScoreUTF32{};

    explicit sTopScore(const std::string &_Name, int _Score) :
        Score{_Score}
    {
        NameUTF32 = ConvertUTF8.from_bytes(_Name);
        ScoreUTF32 = ConvertUTF8.from_bytes(std::to_string(Score));
    }

    explicit sTopScore(const std::u32string &_NameUTF32, int _Score, const std::u32string &_ScoreUTF32) :
        NameUTF32{_NameUTF32},
        Score{_Score},
        ScoreUTF32{_ScoreUTF32}
    {}

    bool operator > (const sTopScore &B) const
    {
        return Score > B.Score;
    }
};

std::list<sTopScore> TopScoresList{};

} // unnamed namespace


/*
 * Top Scores menu initialization.
 * Copy default scores list, add pilot profiles, sort, erase elements by draw limit (same as default scores list size).
 */
void InitTopScoresMenu()
{
    // (!) check, that Score and ScoreUTF32 are the same value
    static const std::list<sTopScore> DefaultTopScores{
        sTopScore{U"Viewizard", 100000, U"100000"},
        sTopScore{U"Alex", 90000, U"90000"},
        sTopScore{U"Michael", 80000, U"80000"},
        sTopScore{U"Jeffrey", 70000, U"70000"},
        sTopScore{U"Christopher", 60000, U"60000"},
        sTopScore{U"Becky", 40000, U"40000"},
        sTopScore{U"Greg", 20000, U"20000"},
        sTopScore{U"Jay", 10000, U"10000"},
        sTopScore{U"Kelvin", 5000, U"5000"},
        sTopScore{U"Stephan", 1000, U"1000"}};

    TopScoresList = DefaultTopScores;

    for (int i = 0; i < config::MAX_PROFILES; i++) {
        if (GameConfig().Profile[i].Used && GameConfig().Profile[i].Experience > 0) {
            TopScoresList.emplace_back(GameConfig().Profile[i].Name,
                                       GameConfig().Profile[i].Experience);
        }
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
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.5f * MenuContentTransp);


    int X1 = GameConfig().InternalWidth / 2 - 362;
    int Y1 = 165;
    int Prir1 = 42;

    vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("NAME"));
    vw_DrawTextUTF32(X1 + 650, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, vw_GetTextUTF32("SCORE"));

    Y1 += 10;

    for (auto &tmpTopScore : TopScoresList) {
        Y1 += Prir1;

        vw_DrawTextUTF32(X1, Y1, 0, 530, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpTopScore.NameUTF32);
        vw_DrawTextUTF32(X1 + 650, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpTopScore.ScoreUTF32);
    }

    int X = (GameConfig().InternalWidth - 384) / 2;
    int Y = 165 + 100 * 5;
    if (DrawButton384(X,Y, vw_GetTextUTF32("MAIN MENU"), MenuContentTransp, Button10Transp, LastButton10UpdateTime)) {
        cCommand::GetInstance().Set(eCommand::SWITCH_TO_MAIN_MENU);
    }
}

} // astromenace namespace
} // viewizard namespace
