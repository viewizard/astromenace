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
#include "../ui/font.h"


const char *ButtonGameWeaponInfoType[4] = {
	"Full",
	"Flat",
	"Slim",
	"Off"
};






void InterfaceMenu(float ContentTransp, float *ButtonTransp1, float *LastButtonUpdateTime1)
{

	sRECT SrcRect, DstRect;
	SrcRect(0,0,2,2);
	DstRect(0,0,Setup.InternalWidth,768);
	vw_Draw2D(DstRect, SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f*ContentTransp);

	int X1 = Setup.InternalWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;



	// выбор языка меню
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("Menu Language"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, false)) {
		if (Setup.MenuLanguage == 0)
			Setup.MenuLanguage = vw_GetLanguageListCount() - 1;
		else
			Setup.MenuLanguage--;

		vw_SetTextLanguage(Setup.MenuLanguage);
		// forced to regenerate textures (base texture connected to language-related characters set)
		vw_ReleaseAllFontChars();
		GenerateFonts();
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, false)) {
		if (Setup.MenuLanguage >= (vw_GetLanguageListCount() - 1))
			Setup.MenuLanguage = 0;
		else
			Setup.MenuLanguage++;

		vw_SetTextLanguage(Setup.MenuLanguage);
		// forced to regenerate textures (base texture connected to language-related characters set)
		vw_ReleaseAllFontChars();
		GenerateFonts();
	}

	int Size, SizeI;
	Size = vw_FontSize(vw_GetText("English", Setup.MenuLanguage));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("English", Setup.MenuLanguage));




	// выбор языка голосовых сообщений
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("Voice Language"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, false)) {
		if (Setup.VoiceLanguage == 0)
			Setup.VoiceLanguage = vw_GetLanguageListCount() - 1;
		else
			Setup.VoiceLanguage--;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, false)) {
		if (Setup.VoiceLanguage >= (vw_GetLanguageListCount() - 1))
			Setup.VoiceLanguage = 0;
		else
			Setup.VoiceLanguage++;
	}

	Size = vw_FontSize(vw_GetText("English", Setup.VoiceLanguage));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("English", Setup.VoiceLanguage));



	// выбор шрифта
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("Menu Font"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, Setup.FontNumber <= 0)) {
		Setup.FontNumber --;
		if (Setup.FontNumber < 0) Setup.FontNumber = 0;

		// reinitialize fonts and regenerate textures (no need in vw_ShutdownFont() call)
		vw_ReleaseAllFontChars();
		vw_InitFont(FontList[Setup.FontNumber].FontFileName);
		GenerateFonts();
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, Setup.FontNumber >= FontQuantity-1)) {
		Setup.FontNumber ++;
		if (Setup.FontNumber > FontQuantity-1) Setup.FontNumber = FontQuantity-1;

		// reinitialize fonts and regenerate textures (no need in vw_ShutdownFont() call)
		vw_ReleaseAllFontChars();
		vw_InitFont(FontList[Setup.FontNumber].FontFileName);
		GenerateFonts();
	}
	int Scale = 0;
	Size = vw_FontSize(FontList[Setup.FontNumber].FontTitle);
	if (Size > 170) {
		Scale = -170;
		Size = 170;
	}
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, Scale, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, FontList[Setup.FontNumber].FontTitle);




	// сброс подсказок
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("'Tips and hints' Status"));
	// узнаем есть ли вообще что ресетить
	bool NeedReset = false;
	for (int i=0; i<10; i++)
		if (!Setup.NeedShowHint[i]) NeedReset = true;
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_GetText("Reset"), ContentTransp, !NeedReset))
		if (NeedCheck == 0) {
			for (int i=0; i<10; i++) Setup.NeedShowHint[i] = true;
		}




	// вкл-выкл отображения счетчика фпс
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("FPS Сounter"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, false) || DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, false)) {
		Setup.ShowFPS = !Setup.ShowFPS;
	}
	Size = vw_FontSize(Setup.ShowFPS ? vw_GetText("On") : vw_GetText("Off"));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, Setup.ShowFPS ? vw_GetText("On") : vw_GetText("Off"));




	// вид панелей с оружием в игре
	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("Weapon Panels View"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("Prev"), ContentTransp, Setup.GameWeaponInfoType==1)) {
		Setup.GameWeaponInfoType--;
		if (Setup.GameWeaponInfoType < 1) Setup.GameWeaponInfoType = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("Next"), ContentTransp, Setup.GameWeaponInfoType==4)) {
		Setup.GameWeaponInfoType++;
		if (Setup.GameWeaponInfoType > 4) Setup.GameWeaponInfoType = 4;
	}
	Size = vw_FontSize(vw_GetText(ButtonGameWeaponInfoType[Setup.GameWeaponInfoType-1]));
	SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText(ButtonGameWeaponInfoType[Setup.GameWeaponInfoType-1]));















	int Prir = 100;
	int X;
	int Y = 165+Prir*4;


	X = Setup.InternalWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, vw_GetText("Advanced"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::OPTIONS_ADVANCED);
			GameMenuStatus = eGameMenuStatus::OPTIONS_ADVANCED;
		} else
			ComBuffer = eCommand::SWITCH_TO_OPTIONS_ADVANCED;
	}

	X = Setup.InternalWidth/2 - 100;
	if (DrawButton200_2(X,Y+28, vw_GetText("Video & Audio"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::OPTIONS);
			GameMenuStatus = eGameMenuStatus::OPTIONS;
		} else
			ComBuffer = eCommand::SWITCH_TO_OPTIONS;
	}

	X = Setup.InternalWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, vw_GetText("Config Controls"), ContentTransp, false)) {
		if (MenuStatus == eMenuStatus::GAME) {
			SetOptionsMenu(eMenuStatus::CONFCONTROL);
			GameMenuStatus = eGameMenuStatus::CONFCONTROL;
		} else
			ComBuffer = eCommand::SWITCH_TO_CONFCONTROL;
	}






	X = (Setup.InternalWidth - 384)/2;
	Y = Y+Prir;
	if (MenuStatus == eMenuStatus::GAME) {
		if (DrawButton384(X,Y, vw_GetText("GAME MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
			GameMenuStatus = eGameMenuStatus::GAME_MENU;
	} else {
		if (DrawButton384(X,Y, vw_GetText("MAIN MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
			ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;
	}
}









