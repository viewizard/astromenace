/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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



// информация о победителях, для вывода
char	GameName[10][128];
int		GameScore[10];






void AddTopScores(int Score, char Name[128], bool Type)
{
	// данные буфера
	int ScoreBuffer = Score;
	char NameBuffer[128];
	strcpy(NameBuffer, Name);

	// сдвигаем...
	if (Type) {
		for(int i=0; i<10; i++) {
			if (ScoreBuffer < GameScore[i]) continue;
			else {
				// сохраняем данные текущей строки
				int ScoreBuffer2 = GameScore[i];
				char NameBuffer2[128];
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
		for(int i=0; i<10; i++) {
			if (ScoreBuffer < Setup.TopScores[i].Score) continue;
			else {
				// сохраняем данные текущей строки
				int ScoreBuffer2 = Setup.TopScores[i].Score;
				char NameBuffer2[128];
				strcpy(NameBuffer2, Setup.TopScores[i].Name);

				// записываем данные из буфера...
				Setup.TopScores[i].Score = ScoreBuffer;
				strcpy(Setup.TopScores[i].Name, NameBuffer);

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
	SetRect(&SrcRect,0,0,2,2);
	SetRect(&DstRect,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f*MenuContentTransp);


	int X1 = Setup.iAspectRatioWidth/2 - 362;
	int Y1 = 165;
	int Prir1 = 42;

	vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "#");
	vw_DrawFont(X1+45, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("3_NAME"));
	vw_DrawFont(X1+650, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, vw_GetText("3_SCORE"));

	Y1 += 10;

	for (int i=0; i<10; i++) {
		Y1 += Prir1;

		vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.6f*MenuContentTransp, "%i", i+1);
		vw_DrawFont(X1+45, Y1, 0, 530, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GameName[i]);
		vw_DrawFont(X1+650, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i", GameScore[i]);
	}




	int X = (Setup.iAspectRatioWidth - 384)/2;
	int Y = 165+100*5;
	if (DrawButton384(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;

}
