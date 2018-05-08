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



// информация о победителях, для вывода
char	GameName[10][PROFILE_NAME_SIZE];
int	GameScore[10];






void AddTopScores(int Score, const char Name[PROFILE_NAME_SIZE], bool Type)
{
	// данные буфера
	int ScoreBuffer = Score;
	char NameBuffer[PROFILE_NAME_SIZE];
	strcpy(NameBuffer, Name);

	// сдвигаем...
	if (Type) {
		for(int i=0; i<10; i++) {
			if (ScoreBuffer < GameScore[i]) continue;
			else {
				// сохраняем данные текущей строки
				int ScoreBuffer2 = GameScore[i];
				char NameBuffer2[PROFILE_NAME_SIZE];
				strcpy(NameBuffer2, GameName[i]);

				// записываем данные из буфера...
				GameScore[i] = ScoreBuffer;
				strcpy(GameName[i], NameBuffer);

				// устанавливаем нужные значения в буфер поиска
				ScoreBuffer = ScoreBuffer2;
				strcpy(NameBuffer, NameBuffer2);
			}
		}
	} else {
		// если нужно установить в основную (удалили профайл)
		for(int i = 0; i < 10; i++) {
			if (ScoreBuffer >= GameConfig().TopScores[i].Score) {
				// сохраняем данные текущей строки
				int ScoreBuffer2 = GameConfig().TopScores[i].Score;
				char NameBuffer2[PROFILE_NAME_SIZE];
				strcpy(NameBuffer2, GameConfig().TopScores[i].Name);

				// записываем данные из буфера...
				ChangeGameConfig().TopScores[i].Score = ScoreBuffer;
				strcpy(ChangeGameConfig().TopScores[i].Name, NameBuffer);

				// устанавливаем нужные значения в буфер поиска
				ScoreBuffer = ScoreBuffer2;
				strcpy(NameBuffer, NameBuffer2);
			}
		}
	}
}




void TopScoresMenu()
{
	sRECT SrcRect, DstRect;
	SrcRect(0, 0, 2, 2);
	DstRect(0, 0, GameConfig().InternalWidth, 768);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f * MenuContentTransp);


	int X1 = GameConfig().InternalWidth / 2 - 362;
	int Y1 = 165;
	int Prir1 = 42;

	vw_DrawText(X1, Y1, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, "#");
	vw_DrawText(X1+45, Y1, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("NAME"));
	vw_DrawText(X1+650, Y1, 0, 0, 1.0f, eRGBCOLOR::yellow, MenuContentTransp, vw_GetText("SCORE"));

	Y1 += 10;

	for (int i=0; i<10; i++) {
		Y1 += Prir1;

		vw_DrawText(X1, Y1, 0, 0, 1.0f, eRGBCOLOR::white, 0.6f*MenuContentTransp, "%i", i+1);
		vw_DrawText(X1+45, Y1, 0, 530, 1.0f, eRGBCOLOR::white, MenuContentTransp, GameName[i]);
		vw_DrawText(X1+650, Y1, 0, 0, 1.0f, eRGBCOLOR::white, MenuContentTransp, "%i", GameScore[i]);
	}




	int X = (GameConfig().InternalWidth - 384) / 2;
	int Y = 165+100*5;
	if (DrawButton384(X,Y, vw_GetText("MAIN MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;

}
