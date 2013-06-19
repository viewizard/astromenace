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



const char * MouseCodeName(char Num)
{
	switch (Setup.MenuLanguage)
	{
		default: //en
			switch (Num)
			{
				case 1: return "Mouse1";
				case 2: return "Mouse3";
				case 3: return "Mouse2";
				case 4: return "Mouse4";
				case 5: return "Mouse5";
				case 6: return "Mouse6";
				case 7: return "Mouse7";
				case 8: return "Mouse8";
				case 0: return "?";
			}
			break;

		case 2: //de
			switch (Num)
			{
				case 1: return "Maus1";
				case 2: return "Maus3";
				case 3: return "Maus2";
				case 4: return "Maus4";
				case 5: return "Maus5";
				case 6: return "Maus6";
				case 7: return "Maus7";
				case 8: return "Maus8";
				case 0: return "?";
			}
			break;

		case 3: //ru
			switch (Num)
			{
				case 1: return "Mouse1";
				case 2: return "Mouse3";
				case 3: return "Mouse2";
				case 4: return "Mouse4";
				case 5: return "Mouse5";
				case 6: return "Mouse6";
				case 7: return "Mouse7";
				case 8: return "Mouse8";
				case 0: return "?";
			}
			break;
	}
	return 0;
}




char JoystickCodeNameText[12];/*"JoystickXXX"*/
const char * JoystickCodeName(int Num)
{
	// кнопки еще нет
	if (Num == -1) return "?";

	// Num+1 т.к. начинается счет с 0
	sprintf(JoystickCodeNameText,"Joystick%i",Num+1);
	return JoystickCodeNameText;
}




int ButQuant = 10;
float But[10] =
{1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};

int NeedCheck = 0;
float LastTimeBut = 0.0f;



// проверка, выбрали что-то или нет
void CheckMouseKeybJState()
{
	// проверяем, нужно ли вытягивать что-то или нет...
	if (!isDialogBoxDrawing())
	if (NeedCheck > 0)
	{

		// клавиатура
		if ((NeedCheck > 0 && NeedCheck <= 6) || NeedCheck == 100)
		for (int i=0; i<GetMaxKeys(); i++)
		{
			if (vw_GetKeys(i))
			if (vw_VirtualCodeName(Setup.MenuLanguage, i) != 0) // если мы играем с этой кнопкой
			{
				switch(NeedCheck)
				{
					case 1: Setup.KeyBoardUp = i; break;
					case 2: Setup.KeyBoardDown = i; break;
					case 3: Setup.KeyBoardLeft = i; break;
					case 4: Setup.KeyBoardRight = i; break;
					case 5: Setup.KeyBoardPrimary = i; break;
					case 6: Setup.KeyBoardSecondary = i; break;
					case 100:
							NewWeaponControlType = 1;
							NewWeaponControlTypeData = i;
							break;
				}

				vw_SetKeys(i, false);
				NeedCheck = 0;
			}
		}


		// мышка
		if ((NeedCheck >= 7 && NeedCheck <= 8) || NeedCheck == 100)
		{
			for (int i=0; i<8; i++)
				if (SDL_MouseCurrentStatus[i])
				{
					switch(NeedCheck)
					{
						case 7: Setup.MousePrimary = i; break;
						case 8: Setup.MouseSecondary = i; break;
						case 100:
							NewWeaponControlType = 2;
							NewWeaponControlTypeData = i;
							break;
					}
					NeedCheck = 0;
					vw_GetWindowLBMouse(true);
				}
		}


#ifdef joystick
		// джойстик
		if (Joystick != NULL)
		if ((NeedCheck >= 9 && NeedCheck <= 10) || NeedCheck == 100)
		{
			for (int i=0; i<SDL_JoystickNumButtons(Joystick); i++)
				if (JoysticButtons[i])
				{
					switch(NeedCheck)
					{
						case 9: Setup.JoystickPrimary = i; break;
						case 10: Setup.JoystickSecondary = i; break;
						case 100:
							NewWeaponControlType = 3;
							NewWeaponControlTypeData = i;
							break;
					}
					NeedCheck = 0;
					JoysticButtons[i] = false;
					vw_GetWindowLBMouse(true);
				}
		}
#endif
	}

	// мерцание кнопок, ставим сюда, чтобы не тягать его везде
	float Delta = vw_GetTime() - LastTimeBut;
	for (int i=0; i<ButQuant; i++)
	{
		if (But[i] > 0.3f) But[i] -= 2.0f*Delta;
		if (But[i] < 0.3f) But[i] = 1.0f;
	}
	LastTimeBut = vw_GetTime();
}



void CheckKeysBeforeExit()
{
	if (Setup.KeyBoardUp == 0) Setup.KeyBoardUp = SDLK_UP;
	if (Setup.KeyBoardDown == 0) Setup.KeyBoardDown = SDLK_DOWN;
	if (Setup.KeyBoardLeft == 0) Setup.KeyBoardLeft = SDLK_LEFT;
	if (Setup.KeyBoardRight == 0) Setup.KeyBoardRight = SDLK_RIGHT;
	if (Setup.KeyBoardPrimary == 0) Setup.KeyBoardPrimary = SDLK_LCTRL;
	if (Setup.KeyBoardSecondary == 0) Setup.KeyBoardSecondary = SDLK_SPACE;
	if (Setup.MousePrimary == 0) Setup.MousePrimary = SDL_BUTTON_LEFT;
	if (Setup.MouseSecondary == 0) Setup.MouseSecondary = SDL_BUTTON_RIGHT;
	NeedCheck = 0;
	for (int i=0; i<ButQuant; i++) But[i] = 1.0f;
}




void ConfControlMenu(float ContentTransp, float *ButtonTransp1, float *LastButtonUpdateTime1)
{
	// проверяем, нужно ли вытягивать что-то или нет...
	CheckMouseKeybJState();


	RECT SrcRect, DstRect;
	SetRect(&SrcRect,0,0,2,2);
	SetRect(&DstRect,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*ContentTransp);







	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;





	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Joystick_DeadZone"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), ContentTransp, Setup.JoystickDeadZone == 0))
	{
		Setup.JoystickDeadZone --;
		if (Setup.JoystickDeadZone<0) Setup.JoystickDeadZone = 0;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), ContentTransp, Setup.JoystickDeadZone == 10))
	{
		Setup.JoystickDeadZone++;
		if (Setup.JoystickDeadZone>10) Setup.JoystickDeadZone = 10;
	}
	for (int i=0; i<10; i++)
	{
		SetRect(&SrcRect,0,0,16,32);
		SetRect(&DstRect,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
		if (Setup.JoystickDeadZone > i)
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc.tga"), true, ContentTransp);
		else
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, ContentTransp);
	}





	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Control_Sensitivity"));
	if (DrawButton128_2(X1+300, Y1-6, vw_GetText("1_Decrease"), ContentTransp, Setup.ControlSensivity == 1))
	{
		Setup.ControlSensivity --;
		if (Setup.ControlSensivity<1) Setup.ControlSensivity = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, vw_GetText("1_Increase"), ContentTransp, Setup.ControlSensivity == 10))
	{
		Setup.ControlSensivity++;
		if (Setup.ControlSensivity>10) Setup.ControlSensivity = 10;
	}
	for (int i=0; i<10; i++)
	{
		SetRect(&SrcRect,0,0,16,32);
		SetRect(&DstRect,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
		if (Setup.ControlSensivity > i)
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc.tga"), true, ContentTransp);
		else
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, ContentTransp);
	}













	Y1 += Prir1;
	int SizeI1 = vw_FontSize(vw_GetText("3_MOUSE"));
	int SizeI = SizeI1 > 130 ? -15 : (100-SizeI1)/2;
	vw_DrawFont(X1+315+SizeI, Y1, SizeI1 > 130 ? -130 : 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("3_MOUSE"));
	SizeI1 = vw_FontSize(vw_GetText("3_KEYBOARD"));
	SizeI = SizeI1 > 130 ? 10 : (150-SizeI1)/2;
	vw_DrawFont(X1+446+SizeI, Y1, SizeI1 > 130 ? -130 : 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("3_KEYBOARD"));
	SizeI1 = vw_FontSize(vw_GetText("3_JOYSTICK"));
	SizeI = SizeI1 > 130 ? 10 : (150-SizeI1)/2;
	vw_DrawFont(X1+605+SizeI, Y1, SizeI1 > 130 ? -130 : 0, 0, 1.0f, 1.0f,0.5f,0.0f, ContentTransp, vw_GetText("3_JOYSTICK"));


	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Primary_Attack"));
	float Transp = 1.0f;
	bool Off = false;
	if (NeedCheck == 7) {Transp = But[6]; Off = true;};
	if (DrawButton128_2((int)X1+300, (int)Y1-6, MouseCodeName(Setup.MousePrimary), Transp*ContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.MousePrimary = 0;
		NeedCheck = 7;
		for (int i=0; i<8; i++) SDL_MouseCurrentStatus[i] = false;
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 5) {Transp = But[4]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardPrimary), Transp*ContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardPrimary = 0;
		NeedCheck = 5;
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 9) {Transp = But[8]; Off = true;};
	if (DrawButton128_2((int)X1+616, (int)Y1-6, JoystickCodeName(Setup.JoystickPrimary), Transp*ContentTransp, !(Joystick != NULL) || Off))
	if (NeedCheck == 0)
	{
		Setup.JoystickPrimary = -1;
		NeedCheck = 9;
		for (int i=0; i<100; i++) JoysticButtons[i] = false;
	}



	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Secondary_Attack"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 8) {Transp = But[7]; Off = true;};
	if (DrawButton128_2((int)X1+300, (int)Y1-6, MouseCodeName(Setup.MouseSecondary), Transp*ContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.MouseSecondary = 0;
		NeedCheck = 8;
		for (int i=0; i<8; i++) SDL_MouseCurrentStatus[i] = false;//MouseCurrentStatus[i] = vw_MouseIsBJustDown(i);
	}
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 6) {Transp = But[5]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardSecondary), Transp*ContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardSecondary = 0;
		NeedCheck = 6;
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 10) {Transp = But[9]; Off = true;};
	if (DrawButton128_2((int)X1+616, (int)Y1-6, JoystickCodeName(Setup.JoystickSecondary), Transp*ContentTransp, !(Joystick != NULL) || Off))
	if (NeedCheck == 0)
	{
		Setup.JoystickSecondary = -1;
		NeedCheck = 10;
		for (int i=0; i<100; i++) JoysticButtons[i] = false;
	}




	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Move_Forward"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 1) {Transp = But[0]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardUp), Transp*ContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardUp = 0;
		NeedCheck = 1;
	}

	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Move_Backward"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 2) {Transp = But[1]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardDown), Transp*ContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardDown = 0;
		NeedCheck = 2;
	}

	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0, 1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Move_Left"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 3) {Transp = But[2]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardLeft), Transp*ContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardLeft = 0;
		NeedCheck = 3;
	}

	Y1 += Prir1;
	vw_DrawFont(X1, Y1, -280, 0,1.0f, 1.0f,1.0f,1.0f, ContentTransp, vw_GetText("3_Move_Right"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 4) {Transp = But[3]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.MenuLanguage, Setup.KeyBoardRight), Transp*ContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardRight = 0;
		NeedCheck = 4;
	}







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
		CheckKeysBeforeExit();
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
		CheckKeysBeforeExit();
	}


	X = Setup.iAspectRatioWidth/2 + 166;
	if (DrawButton200_2(X,Y+28, vw_GetText("1_Interface"), ContentTransp, false))
	{
		if (GameStatus == GAME)
		{
			SetOptionsMenu(INTERFACE);
			GameMenuStatus = 4;
		}
		else ComBuffer = INTERFACE;
		CheckKeysBeforeExit();
	}




	X = (Setup.iAspectRatioWidth - 384)/2;
	Y = Y+Prir;
	if (GameStatus == GAME)
	{
		if (DrawButton384(X,Y, vw_GetText("1_GAME_MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
		{
			GameMenuStatus = 1;
			CheckKeysBeforeExit();
		}
	}
	else
	{
		if (DrawButton384(X,Y, vw_GetText("1_MAIN_MENU"), ContentTransp, ButtonTransp1, LastButtonUpdateTime1))
		{
			ComBuffer = MAIN_MENU;
			CheckKeysBeforeExit();
		}
	}

}









