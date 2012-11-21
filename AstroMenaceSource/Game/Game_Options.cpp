/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#include "../Game.h"




void GameOptions()
{
	// проверяем, нужно ли вытягивать что-то или нет...
	CheckMouseKeybJState();


	RECT SrcRest, DstRest;
	SetRect(&SrcRest,1,1,1,1 );
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.4f*GameContentTransp);




	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 45;
	int Prir1 = 55;



	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Music_Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), GameContentTransp, !Setup.Music_check || Setup.MusicSw==0))
	{
		Setup.MusicSw--;
		if (Setup.MusicSw<0) Setup.MusicSw = 0;
		vw_SetMusicMainVolume(Setup.MusicSw/10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), GameContentTransp, !Setup.Music_check || Setup.MusicSw==10))
	{
		Setup.MusicSw++;
		if (Setup.MusicSw>10) Setup.MusicSw = 10;
		vw_SetMusicMainVolume(Setup.MusicSw/10.0f);
	}
	if (!Setup.Music_check)
	{
		int SizeI = (170-vw_FontSize(vw_GetText("3_Not_available")))/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, GameContentTransp, vw_GetText("3_Not_available"));
	}
	else
	{
		for (int i=0; i<10; i++)
		{
			SetRect(&SrcRest,0,0,16,32);
			SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
			if (Setup.MusicSw>i)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, GameContentTransp);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, GameContentTransp);
		}
	}





	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Sound_Effects_Volume"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), GameContentTransp, !Setup.Sound_check || Setup.SoundSw==0))
	{
		Setup.SoundSw--;
		if (Setup.SoundSw<0) Setup.SoundSw = 0;
		Audio_SetSound2DMainVolume(Setup.SoundSw/10.0f);
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), GameContentTransp, !Setup.Sound_check || Setup.SoundSw==10))
	{
		Setup.SoundSw++;
		if (Setup.SoundSw>10) Setup.SoundSw = 10;
		Audio_SetSound2DMainVolume(Setup.SoundSw/10.0f);
	}
	if (!Setup.Sound_check)
	{
		int SizeI = (170-vw_FontSize(vw_GetText("3_Not_available")))/2;
		vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, GameContentTransp, vw_GetText("3_Not_available"));
	}
	else
	{
		for (int i=0; i<10; i++)
		{
			SetRect(&SrcRest,0,0,16,32);
			SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
			if (Setup.SoundSw>i)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, GameContentTransp);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, GameContentTransp);
		}
	}






	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Mouse_Control"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Prev"), GameContentTransp, false) | DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Next"), GameContentTransp, false))
	if (NeedCheck == 0)
	{
		Setup.MouseControl = !Setup.MouseControl;
	}
	int Size = vw_FontSize(Setup.MouseControl ? vw_GetText("1_On") : vw_GetText("1_Off"));
	int SizeI = (170-Size)/2;
	vw_DrawFont(X1+438+SizeI, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, Setup.MouseControl ? vw_GetText("1_On") : vw_GetText("1_Off"));


	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Control_Sensitivity"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), GameContentTransp, Setup.ControlSensivity == 1))
	{
		Setup.ControlSensivity --;
		if (Setup.ControlSensivity<1) Setup.ControlSensivity = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), GameContentTransp, Setup.ControlSensivity == 10))
	{
		Setup.ControlSensivity++;
		if (Setup.ControlSensivity>10) Setup.ControlSensivity = 10;
	}
	for (int i=0; i<10; i++)
	{
		SetRect(&SrcRest,0,0,16,32);
		SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
		if (Setup.ControlSensivity > i)
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, GameContentTransp);
		else
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, GameContentTransp);
	}














	Y1 += Prir1;
	SizeI = (100-vw_FontSize(vw_GetText("3_MOUSE")))/2;
	vw_DrawFont(X1+315+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, GameContentTransp, vw_GetText("3_MOUSE"));
	SizeI = (150-vw_FontSize(vw_GetText("3_KEYBOARD")))/2;
	vw_DrawFont(X1+446+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, GameContentTransp, vw_GetText("3_KEYBOARD"));
	SizeI = (150-vw_FontSize(vw_GetText("3_JOYSTICK")))/2;
	vw_DrawFont(X1+605+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, GameContentTransp, vw_GetText("3_JOYSTICK"));

	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Primary_Attack"));
	float Transp = 1.0f;
	bool Off = false;
	if (NeedCheck == 7) {Transp = But[6]; Off = true;};
	if (DrawButton128_2((int)X1+300, (int)Y1-6, MouseCodeName(Setup.MousePrimary), Transp*GameContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.MousePrimary = 0;
		NeedCheck = 7;
		for (int i=0; i<8; i++) SDL_MouseCurrentStatus[i] = false;//MouseCurrentStatus[i] = vw_MouseIsBJustDown(i);
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 5) {Transp = But[4]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardPrimary), Transp*GameContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardPrimary = 0;
		NeedCheck = 5;
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 9) {Transp = But[8]; Off = true;};
	if (DrawButton128_2((int)X1+616, (int)Y1-6, JoystickCodeName(Setup.JoystickPrimary), Transp*GameContentTransp, !(Joystick != NULL) || Off))
	if (NeedCheck == 0)
	{
		Setup.JoystickPrimary = -1;
		NeedCheck = 9;
		for (int i=0; i<100; i++) JoysticButtons[i] = false;
	}



	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Secondary_Attack"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 8) {Transp = But[7]; Off = true;};
	if (DrawButton128_2((int)X1+300, (int)Y1-6, MouseCodeName(Setup.MouseSecondary), Transp*GameContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.MouseSecondary = 0;
		NeedCheck = 8;
		for (int i=0; i<8; i++) SDL_MouseCurrentStatus[i] = false;//MouseCurrentStatus[i] = vw_MouseIsBJustDown(i);
	}
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 6) {Transp = But[5]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardSecondary), Transp*GameContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardSecondary = 0;
		NeedCheck = 6;
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 10) {Transp = But[9]; Off = true;};
	if (DrawButton128_2((int)X1+616, (int)Y1-6, JoystickCodeName(Setup.JoystickSecondary), Transp*GameContentTransp, !(Joystick != NULL) || Off))
	if (NeedCheck == 0)
	{
		Setup.JoystickSecondary = -1;
		NeedCheck = 10;
		for (int i=0; i<100; i++) JoysticButtons[i] = false;
	}




	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Move_Forward"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 1) {Transp = But[0]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardUp), Transp*GameContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardUp = 0;
		NeedCheck = 1;
	}

	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Move_Backward"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 2) {Transp = But[1]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardDown), Transp*GameContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardDown = 0;
		NeedCheck = 2;
	}

	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Move_Left"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 3) {Transp = But[2]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardLeft), Transp*GameContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardLeft = 0;
		NeedCheck = 3;
	}

	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, GameContentTransp, vw_GetText("3_Move_Right"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 4) {Transp = But[3]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardRight), Transp*GameContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardRight = 0;
		NeedCheck = 4;
	}












	int Prir = 100;
	int X = (Setup.iAspectRatioWidth - 384)/2;
	int Y = 165+Prir*5;
	if (DrawButton384(X,Y, vw_GetText("1_GAME_MENU"), GameContentTransp, &GameButton1Transp, &LastGameButton1UpdateTime))
	{
		if (Setup.KeyBoardUp == 0) Setup.KeyBoardUp = SDLK_UP;
		if (Setup.KeyBoardDown == 0) Setup.KeyBoardDown = SDLK_DOWN;
		if (Setup.KeyBoardLeft == 0) Setup.KeyBoardLeft = SDLK_LEFT;
		if (Setup.KeyBoardRight == 0) Setup.KeyBoardRight = SDLK_RIGHT;
		if (Setup.KeyBoardPrimary == 0) Setup.KeyBoardPrimary = SDLK_LCTRL;
		if (Setup.KeyBoardSecondary == 0) Setup.KeyBoardSecondary = SDLK_SPACE;
		if (Setup.MousePrimary == 0) Setup.MousePrimary = SDL_BUTTON_LEFT;
		if (Setup.MouseSecondary == 0) Setup.MouseSecondary = SDL_BUTTON_RIGHT;
		if (Setup.JoystickPrimary == -1) Setup.JoystickPrimary = 0;
		if (Setup.JoystickSecondary == -1) Setup.JoystickSecondary = 1;
		NeedCheck = 0;
		for (int i=0; i<ButQuant; i++) But[i] = 1.0f;
		GameMenuStatus = 1;
	}

}









