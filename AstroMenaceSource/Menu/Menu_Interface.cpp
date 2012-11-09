/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Interface.cpp

	Copyright (c) 2012 Michael Kurinnoy, Viewizard
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

int Options_MenuLanguage;
int Options_VoiceLanguage;
int Options_FontNumber;


const char *ButtonGameWeaponInfoType[4] =
{"3_Full",
"3_Flat",
"3_Slim",
"1_Off"};



void InterfaceMenu()
{

	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);

	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;



	// выбор языка меню
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Menu_Language"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, false))
	{
		Options_MenuLanguage--;
		if (Options_MenuLanguage < 1) Options_MenuLanguage = 3;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, false))
	{
		Options_MenuLanguage++;
		if (Options_MenuLanguage>3) Options_MenuLanguage = 1;
	}

	int Size, SizeI;
	switch (Options_MenuLanguage)
	{
		case 1:
			Size = vw_FontSize("English");
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "English");
			break;
		case 2:
			Size = vw_FontSize("Deutsch");
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Deutsch");
			break;
		case 3:
			Size = vw_FontSize("Русский");
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Русский");
			break;
	}




	// выбор языка голосовых сообщений
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Voice_Language"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, false))
	{
		Options_VoiceLanguage--;
		if (Options_VoiceLanguage < 1) Options_VoiceLanguage = 3;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, false))
	{
		Options_VoiceLanguage++;
		if (Options_VoiceLanguage>3) Options_VoiceLanguage = 1;
	}

	switch (Options_VoiceLanguage)
	{
		case 1:
			Size = vw_FontSize("English");
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "English");
			break;
		case 2:
			Size = vw_FontSize("Deutsch");
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Deutsch");
			break;
		case 3:
			Size = vw_FontSize("Русский");
			SizeI = (170-Size)/2;
			vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "Русский");
			break;
	}



	// выбор шрифта
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Menu_Font"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, Options_FontNumber <= 0))
	{
		Options_FontNumber --;
		if (Options_FontNumber < 0) Options_FontNumber = 0;

	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, Options_FontNumber >= FontQuantity-1))
	{
		Options_FontNumber ++;
		if (Options_FontNumber > FontQuantity-1) Options_FontNumber = FontQuantity-1;

	}
	// если шрифт - текущий, просто пишем его название
	if (Options_FontNumber == Setup.FontNumber)
	{
		int Scale = 0;
		Size = vw_FontSize(FontList[Options_FontNumber].FontName);
		if (Size > 170)
		{
			Scale = -170;
			Size = 170;
		}
		SizeI = (170-Size)/2;
		vw_DrawFont(X1+438+SizeI, Y1, Scale, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, FontList[Options_FontNumber].FontName);
	}
	else
	{
		// если текстуры с таким названием нет - надо создать
		if (vw_FindTextureByName(FontList[Options_FontNumber].FontName) == 0) vw_TextureFromText(FontList[Options_FontNumber].FontFileName, 16, FontList[Options_FontNumber].FontName);

		// выводим текстуру с превью шрифта
		eTexture *FontPreview = vw_FindTextureByName(FontList[Options_FontNumber].FontName);
		RECT SrcRest, DstRest;
		int Scale = FontPreview->SrcWidth;
		if (Scale > 170) Scale = 170;
		int CurretnXPosition = X1+438+(170-Scale)/2;
		SetRect(&SrcRest,0,0,FontPreview->SrcWidth,FontPreview->SrcHeight);
		SetRect(&DstRest,CurretnXPosition,Y1+2,CurretnXPosition+Scale,Y1+2+FontPreview->SrcHeight);
		vw_DrawTransparent(&DstRest, &SrcRest, FontPreview,	true, MenuContentTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);
	}



	// сброс подсказок
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Tipsandhints_Status"));
	// узнаем есть ли вообще что ресетить
	bool NeedReset = false;
	for (int i=0; i<10; i++)
		if (!Setup.NeedShowHint[i]) NeedReset = true;
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_GetText("3_Reset"), MenuContentTransp, !NeedReset))
	if (NeedCheck == 0)
	{
		for (int i=0; i<10; i++) Setup.NeedShowHint[i] = true;
	}




	// вкл-выкл отображения счетчика фпс
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_FPS_Сounter"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, false) | DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, false))
	{
		Setup.ShowFPS = !Setup.ShowFPS;
	}
	Size = vw_FontSize(Setup.ShowFPS ? vw_GetText("1_On") : vw_GetText("1_Off"));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, Setup.ShowFPS ? vw_GetText("1_On") : vw_GetText("1_Off"));




	// вид панелей с оружием в игре
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText("3_Weapon_Panels_View"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), MenuContentTransp, Setup.GameWeaponInfoType==1))
	{
		Setup.GameWeaponInfoType--;
		if (Setup.GameWeaponInfoType < 1) Setup.GameWeaponInfoType = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), MenuContentTransp, Setup.GameWeaponInfoType==4))
	{
		Setup.GameWeaponInfoType++;
		if (Setup.GameWeaponInfoType > 4) Setup.GameWeaponInfoType = 4;
	}
	Size = vw_FontSize(vw_GetText(ButtonGameWeaponInfoType[Setup.GameWeaponInfoType-1]));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, vw_GetText(ButtonGameWeaponInfoType[Setup.GameWeaponInfoType-1]));















	int Prir = 100;
	int X;
	int Y = 165+Prir*4;


	X = Setup.iAspectRatioWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Advanced"), MenuContentTransp, false))
	{
		ComBuffer = OPTIONS_ADVANCED;
	}

	X = Setup.iAspectRatioWidth/2 - 100;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Video_&_Audio"), MenuContentTransp, false))
	{
		ComBuffer = OPTIONS;
	}

	X = Setup.iAspectRatioWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Config_Controls"), MenuContentTransp, false))
	{
		ComBuffer = CONFCONTROL;
	}






	if (Options_MenuLanguage == Setup.MenuLanguage &&
		Options_VoiceLanguage == Setup.VoiceLanguage &&
		Options_FontNumber == Setup.FontNumber)
	{
		X = (Setup.iAspectRatioWidth - 384)/2;
		Y = Y+Prir;
		if (DrawButton384(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		{
			ComBuffer = MAIN_MENU;
		}
	}
	else
	{
		X = Setup.iAspectRatioWidth/2 - 256 - 38;
		Y = Y+Prir;
		if (DrawButton256(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		{
			ComBuffer = MAIN_MENU;
		}
		X = Setup.iAspectRatioWidth/2 + 38;
		if (DrawButton256(X,Y, vw_GetText("1_APPLY"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime))
		{
			// проверяем, нужно перегружать или нет
			if (Options_MenuLanguage != Setup.MenuLanguage ||
				Options_VoiceLanguage != Setup.VoiceLanguage ||
				Options_FontNumber != Setup.FontNumber)
			{
				CanQuit = false;
				Quit = true;
				NeedReCreate = true;
			}

			Setup.FontNumber = Options_FontNumber;
			Setup.MenuLanguage = Options_MenuLanguage;
			Setup.VoiceLanguage = Options_VoiceLanguage;
		}
	}
}









