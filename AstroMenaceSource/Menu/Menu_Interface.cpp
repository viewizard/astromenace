/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "../Game.h"


const char *ButtonGameWeaponInfoType[4] =
{"3_Full",
"3_Flat",
"3_Slim",
"1_Off"};






void InterfaceMenu(float ContentTransp, float *ButtonTransp1, float *LastButtonUpdateTime1)
{

	RECT SrcRect, DstRect;
	SetRect(&SrcRect,0,0,2,2);
	SetRect(&DstRect,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*ContentTransp);

	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;



	// выбор языка меню
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Menu_Language"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, false))
	{
		Setup.MenuLanguage--;
		if (Setup.MenuLanguage < 1) Setup.MenuLanguage = vw_GetLanguageListCount();

		vw_SetTextLanguage(Setup.MenuLanguage);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, false))
	{
		Setup.MenuLanguage++;
		if (Setup.MenuLanguage > vw_GetLanguageListCount()) Setup.MenuLanguage = 1;

		vw_SetTextLanguage(Setup.MenuLanguage);
	}

	int Size, SizeI;
	Size = vw_FontSize(vw_GetLanguageList()[Setup.MenuLanguage-1].title);
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetLanguageList()[Setup.MenuLanguage-1].title);




	// выбор языка голосовых сообщений
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Voice_Language"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, false))
	{
		Setup.VoiceLanguage--;
		if (Setup.VoiceLanguage < 1) Setup.VoiceLanguage = vw_GetLanguageListCount();
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, false))
	{
		Setup.VoiceLanguage++;
		if (Setup.VoiceLanguage > vw_GetLanguageListCount()) Setup.VoiceLanguage = 1;
	}

	Size = vw_FontSize(vw_GetLanguageList()[Setup.VoiceLanguage-1].title);
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetLanguageList()[Setup.VoiceLanguage-1].title);



	// выбор шрифта
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Menu_Font"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, Setup.FontNumber <= 0))
	{
		Setup.FontNumber --;
		if (Setup.FontNumber < 0) Setup.FontNumber = 0;
		strcpy(Setup.FontName, FontList[Setup.FontNumber].FontTitle);

		// удаляем все символы и их текстуры
		vw_ReleaseAllFontCharsWithTextures();
		// инициализируем новый шрифт, вызывать vw_ShutdownFont не нужно
		vw_InitFont(FontList[Setup.FontNumber].FontFileName);
		vw_SetFontSize(Setup.FontSize);
		vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);
		vw_GenerateFontChars(Setup.FontSize > 16 ? 512 : 256, 256, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\():;%&`'*#$=[]@^{}_~><–—«»“”|абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЧЦШЩЪЫЬЭЮЯ©®ÄÖÜäöüß°§/");
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, Setup.FontNumber >= FontQuantity-1))
	{
		Setup.FontNumber ++;
		if (Setup.FontNumber > FontQuantity-1) Setup.FontNumber = FontQuantity-1;
		strcpy(Setup.FontName, FontList[Setup.FontNumber].FontTitle);

		// удаляем все символы и их текстуры
		vw_ReleaseAllFontCharsWithTextures();
		// инициализируем новый шрифт, вызывать vw_ShutdownFont не нужно
		vw_InitFont(FontList[Setup.FontNumber].FontFileName);
		vw_SetFontSize(Setup.FontSize);
		vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);
		vw_GenerateFontChars(Setup.FontSize > 16 ? 512 : 256, 256, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\():;%&`'*#$=[]@^{}_~><–—«»“”|абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЧЦШЩЪЫЬЭЮЯ©®ÄÖÜäöüß°§/");
	}
	int Scale = 0;
	Size = vw_FontSize(FontList[Setup.FontNumber].FontTitle);
	if (Size > 170)
	{
		Scale = -170;
		Size = 170;
	}
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, Scale, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, FontList[Setup.FontNumber].FontTitle);




	// выбор размера шрифта
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Font_Size"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, Setup.FontSize <= 14))
	{
		Setup.FontSize --;
		if (Setup.FontSize < 14) Setup.FontSize = 14;

		// удаляем все символы и их текстуры
		vw_ReleaseAllFontCharsWithTextures();
		// инициализируем новый шрифт, вызывать vw_ShutdownFont не нужно
		vw_InitFont(FontList[Setup.FontNumber].FontFileName);
		vw_SetFontSize(Setup.FontSize);
		vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);
		vw_GenerateFontChars(Setup.FontSize > 16 ? 512 : 256, 256, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\():;%&`'*#$=[]@^{}_~><–—«»“”|абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЧЦШЩЪЫЬЭЮЯ©®ÄÖÜäöüß°§/");
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, Setup.FontSize >= 18))
	{
		Setup.FontSize ++;
		if (Setup.FontSize > 18) Setup.FontSize = 18;

		// удаляем все символы и их текстуры
		vw_ReleaseAllFontCharsWithTextures();
		// инициализируем новый шрифт, вызывать vw_ShutdownFont не нужно
		vw_InitFont(FontList[Setup.FontNumber].FontFileName);
		vw_SetFontSize(Setup.FontSize);
		vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);
		vw_GenerateFontChars(Setup.FontSize > 16 ? 512 : 256, 256, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+\():;%&`'*#$=[]@^{}_~><–—«»“”|абвгдеёжзийклмнопрстуфхцчшщъыьэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЧЦШЩЪЫЬЭЮЯ©®ÄÖÜäöüß°§/");
	}
	Scale = 0;
	Size = vw_FontSize("%i", Setup.FontSize);
	if (Size > 170)
	{
		Scale = -170;
		Size = 170;
	}
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, Scale, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, "%i", Setup.FontSize);






	// сброс подсказок
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Tipsandhints_Status"));
	// узнаем есть ли вообще что ресетить
	bool NeedReset = false;
	for (int i=0; i<10; i++)
		if (!Setup.NeedShowHint[i]) NeedReset = true;
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_GetText("3_Reset"), ContentTransp, !NeedReset))
	if (NeedCheck == 0)
	{
		for (int i=0; i<10; i++) Setup.NeedShowHint[i] = true;
	}




	// вкл-выкл отображения счетчика фпс
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_FPS_Сounter"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, false) || DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, false))
	{
		Setup.ShowFPS = !Setup.ShowFPS;
	}
	Size = vw_FontSize(Setup.ShowFPS ? vw_GetText("1_On") : vw_GetText("1_Off"));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, Setup.ShowFPS ? vw_GetText("1_On") : vw_GetText("1_Off"));




	// вид панелей с оружием в игре
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Weapon_Panels_View"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), ContentTransp, Setup.GameWeaponInfoType==1))
	{
		Setup.GameWeaponInfoType--;
		if (Setup.GameWeaponInfoType < 1) Setup.GameWeaponInfoType = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), ContentTransp, Setup.GameWeaponInfoType==4))
	{
		Setup.GameWeaponInfoType++;
		if (Setup.GameWeaponInfoType > 4) Setup.GameWeaponInfoType = 4;
	}
	Size = vw_FontSize(vw_GetText(ButtonGameWeaponInfoType[Setup.GameWeaponInfoType-1]));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText(ButtonGameWeaponInfoType[Setup.GameWeaponInfoType-1]));















	int Prir = 100;
	int X;
	int Y = 165+Prir*4;


	X = Setup.iAspectRatioWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Advanced"), ContentTransp, false))
	{
		if (GameStatus == GAME)
		{
			SetOptionsMenu(OPTIONS_ADVANCED);
			GameMenuStatus = 3;
		}
		else ComBuffer = OPTIONS_ADVANCED;
	}

	X = Setup.iAspectRatioWidth/2 - 100;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Video_&_Audio"), ContentTransp, false))
	{
		if (GameStatus == GAME)
		{
			SetOptionsMenu(OPTIONS);
			GameMenuStatus = 2;
		}
		else ComBuffer = OPTIONS;
	}

	X = Setup.iAspectRatioWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Config_Controls"), ContentTransp, false))
	{
		if (GameStatus == GAME)
		{
			SetOptionsMenu(CONFCONTROL);
			GameMenuStatus = 5;
		}
		else ComBuffer = CONFCONTROL;
	}






	X = (Setup.iAspectRatioWidth - 384)/2;
	Y = Y+Prir;
	if (GameStatus == GAME)
	{
		if (DrawButton384(X,Y, vw_GetText("1_GAME_MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
		{
			GameMenuStatus = 1;
		}
	}
	else
	{
		if (DrawButton384(X,Y, vw_GetText("1_MAIN_MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
		{
			ComBuffer = MAIN_MENU;
		}
	}
}









