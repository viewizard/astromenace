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



void InterfaceMenu()
{

	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);


	int CurrentPos = 0;

	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;




	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Menu_Language"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, false))
	{
		Options_MenuLanguage--;
		if (Options_MenuLanguage < 1) Options_MenuLanguage = 3;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, false))
	{
		Options_MenuLanguage++;
		if (Options_MenuLanguage>3) Options_MenuLanguage = 1;
	}

	int Size, SizeI;
	switch (Options_MenuLanguage)
	{
		case 1:
			Size = FontSize("English");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "English");
			break;
		case 2:
			Size = FontSize("Deutsch");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "Deutsch");
			break;
		case 3:
			Size = FontSize("Русский");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "Русский");
			break;
	}




	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Keyboard_Layout"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, false))
	{
		Setup.KeyboardLayout--;
		if (Setup.KeyboardLayout < 1) Setup.KeyboardLayout = 3;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, false))
	{
		Setup.KeyboardLayout++;
		if (Setup.KeyboardLayout>3) Setup.KeyboardLayout = 1;
	}

	switch (Setup.KeyboardLayout)
	{
		case 1:
			Size = FontSize("English");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "English");
			break;
		case 2:
			Size = FontSize("Deutsch");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "Deutsch");
			break;
		case 3:
			Size = FontSize("Русская");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "Русская");
			break;
	}




	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Voice_Language"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Prev"), MenuContentTransp, false))
	{
		Options_VoiceLanguage--;
		if (Options_VoiceLanguage < 1) Options_VoiceLanguage = 3;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Next"), MenuContentTransp, false))
	{
		Options_VoiceLanguage++;
		if (Options_VoiceLanguage>3) Options_VoiceLanguage = 1;
	}

	switch (Options_VoiceLanguage)
	{
		case 1:
			Size = FontSize("English");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "English");
			break;
		case 2:
			Size = FontSize("Deutsch");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "Deutsch");
			break;
		case 3:
			Size = FontSize("Русский");
			SizeI = (170-Size)/2;
			DrawFont(X1+438+SizeI, Y1, 0, 0, 0, MenuContentTransp, "Русский");
			break;
	}















	int Prir = 100;
	int X;
	int Y = 165+Prir*4;


	X = Setup.iAspectRatioWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, GetText("1_Advanced"), MenuContentTransp, false))
	{
		ComBuffer = OPTIONS_ADVANCED;
	}

	X = Setup.iAspectRatioWidth/2 - 100;
	char Text[1024];
	sprintf(Text,GetText("1_Video_%s_Audio"),"&");
	if (DrawButton200_2(X,Y+28, Text, MenuContentTransp, false))
	{
		ComBuffer = OPTIONS;
	}

	X = Setup.iAspectRatioWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, GetText("1_Config_Controls"), MenuContentTransp, false))
	{
		ComBuffer = CONFCONTROL;
	}






	if (Options_MenuLanguage == Setup.MenuLanguage &&
		Options_VoiceLanguage == Setup.VoiceLanguage)
	{
		X = (Setup.iAspectRatioWidth - 384)/2;
		Y = Y+Prir;
		if (DrawButton384(X,Y, GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		{
			ComBuffer = MAIN_MENU;
		}
	}
	else
	{
		X = Setup.iAspectRatioWidth/2 - 256 - 38;
		Y = Y+Prir;
		if (DrawButton256(X,Y, GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		{
			ComBuffer = MAIN_MENU;
		}
		X = Setup.iAspectRatioWidth/2 + 38;
		if (DrawButton256(X,Y, GetText("1_APPLY"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime))
		{
			// проверяем, нужно перегружать или нет
			if (Options_MenuLanguage != Setup.MenuLanguage ||
				Options_VoiceLanguage != Setup.VoiceLanguage)
			{
				CanQuit = false;
				Quit = true;
				NeedReCreate = true;
			}

			Setup.MenuLanguage = Options_MenuLanguage;
			Setup.VoiceLanguage = Options_VoiceLanguage;
		}
	}
}









