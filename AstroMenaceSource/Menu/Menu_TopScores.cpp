/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_TopScores.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "../Game.h"



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
	if (Type)
	{
		for(int i=0; i<10; i++)
		{
			if (ScoreBuffer < GameScore[i]) continue;
			else
			{
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
	}
	else
	{
		// если нужно установить в основную (удалили профайл)
		for(int i=0; i<10; i++)
		{
			if (ScoreBuffer < Setup.TopScores[i].Score) continue;
			else
			{
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
	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);


	int X1 = Setup.iAspectRatioWidth/2 - 362;
	int Y1 = 165;
	int Prir1 = 42;

	DrawFont(X1, Y1, 0, 0, 1, MenuContentTransp, "#");
	DrawFont(X1+45, Y1, 0, 0, 1, MenuContentTransp, GetText("3_NAME"));
	DrawFont(X1+650, Y1, 0, 0, 1, MenuContentTransp, GetText("3_SCORE"));

	Y1 += 10;

	for (int i=0; i<10; i++)
	{
		Y1 += Prir1;

		DrawFont(X1, Y1, 0, 0, 0, 0.6f*MenuContentTransp, "%i", i+1);
		DrawFontFreeType(X1+45, Y1, 0, 530, 0, MenuContentTransp, GameName[i]);
		DrawFont(X1+650, Y1, 0, 0, 0, MenuContentTransp, "%i", GameScore[i]);
	}




	int X = (Setup.iAspectRatioWidth - 384)/2;
	int Y = 165+100*5;
	if (DrawButton384(X,Y, GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime)) ComBuffer = MAIN_MENU;

}
