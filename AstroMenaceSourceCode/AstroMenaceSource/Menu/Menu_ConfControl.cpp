/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_ConfControl.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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
#include "../Defines.h"



#ifdef EN
char * MouseCodeName(char Num)
{
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
		// забой... чтобы не показывать, пока ищем
		case 0: return "?";
	}
	return 0;
}
#endif
#ifdef DE
char * MouseCodeName(char Num)
{
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
		// забой... чтобы не показывать, пока ищем
		case 0: return "?";
	}
	return 0;
}
#endif
#ifdef RU
char * MouseCodeName(char Num)
{
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
		// забой... чтобы не показывать, пока ищем
		case 0: return "?";
	}
	return 0;
}
#endif



char JoystickCodeNameText[12];/*"JoystickXXX"*/
char * JoystickCodeName(char Num)
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
			if (vw_VirtualCodeName(Setup.KeyboardLayout, i) != 0) // если мы играем с этой кнопкой
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






void ConfControlMenu()
{
	// проверяем, нужно ли вытягивать что-то или нет...
	CheckMouseKeybJState();


	RECT SrcRest, DstRest;
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);







	int X1 = Setup.iAspectRatioWidth/2 - 375;
	int Y1 = 65;
	int Prir1 = 55;






	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Mouse_Control"));
	if (DrawButton128_2((int)X1+458, (int)Y1-6, Setup.MouseControl ? GetText("1_On") : GetText("1_Off"), MenuContentTransp, false))
	if (NeedCheck == 0)
	{
		Setup.MouseControl = !Setup.MouseControl;
	}


	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Control_Sensitivity"));
	if (DrawButton128_2(X1+300, Y1-6, GetText("1_Decrease"), MenuContentTransp, Setup.ControlSensivity == 1))
	{
		Setup.ControlSensivity --;
		if (Setup.ControlSensivity<1) Setup.ControlSensivity = 1;
	}
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, Setup.ControlSensivity == 10))
	{
		Setup.ControlSensivity++;
		if (Setup.ControlSensivity>10) Setup.ControlSensivity = 10;
	}
	for (int i=0; i<10; i++)
	{
		SetRect(&SrcRest,0,0,16,32);
		SetRect(&DstRest,X1+443+16*i,Y1-4,X1+443+16+16*i,Y1+32-4);
		if (Setup.ControlSensivity > i)
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc.tga"), true, MenuContentTransp);
		else
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/perc_none.tga"), true, MenuContentTransp);
	}













	Y1 += Prir1;
	int SizeI = (100-FontSize(GetText("3_MOUSE")))/2;
	DrawFont(X1+315+SizeI, Y1, 0, 0, 4, MenuContentTransp, GetText("3_MOUSE"));
	SizeI = (150-FontSize(GetText("3_KEYBOARD")))/2;
	DrawFont(X1+446+SizeI, Y1, 0, 0, 4, MenuContentTransp, GetText("3_KEYBOARD"));
	SizeI = (150-FontSize(GetText("3_JOYSTICK")))/2;
	DrawFont(X1+605+SizeI, Y1, 0, 0, 4, MenuContentTransp, GetText("3_JOYSTICK"));


	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Primary_Attack"));
	float Transp = 1.0f;
	bool Off = false;
	if (NeedCheck == 7) {Transp = But[6]; Off = true;};
	if (DrawButton128_2((int)X1+300, (int)Y1-6, MouseCodeName(Setup.MousePrimary), Transp*MenuContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.MousePrimary = 0;
		NeedCheck = 7;
		for (int i=0; i<8; i++) SDL_MouseCurrentStatus[i] = false;
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 5) {Transp = But[4]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.KeyboardLayout, Setup.KeyBoardPrimary), Transp*MenuContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardPrimary = 0;
		NeedCheck = 5;
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 9) {Transp = But[8]; Off = true;};
	if (DrawButton128_2((int)X1+616, (int)Y1-6, JoystickCodeName(Setup.JoystickPrimary), Transp*MenuContentTransp, !(SDL_NumJoysticks()>0) || Off))
	if (NeedCheck == 0)
	{
		Setup.JoystickPrimary = -1;
		NeedCheck = 9;
		for (int i=0; i<100; i++) JoysticButtons[i] = false;
	}



	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Secondary_Attack"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 8) {Transp = But[7]; Off = true;};
	if (DrawButton128_2((int)X1+300, (int)Y1-6, MouseCodeName(Setup.MouseSecondary), Transp*MenuContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.MouseSecondary = 0;
		NeedCheck = 8;
		for (int i=0; i<8; i++) SDL_MouseCurrentStatus[i] = false;//MouseCurrentStatus[i] = vw_MouseIsBJustDown(i);
	}
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 6) {Transp = But[5]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.KeyboardLayout, Setup.KeyBoardSecondary), Transp*MenuContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardSecondary = 0;
		NeedCheck = 6;
	}

	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 10) {Transp = But[9]; Off = true;};
	if (DrawButton128_2((int)X1+616, (int)Y1-6, JoystickCodeName(Setup.JoystickSecondary), Transp*MenuContentTransp, !(SDL_NumJoysticks()>0) || Off))
	if (NeedCheck == 0)
	{
		Setup.JoystickSecondary = -1;
		NeedCheck = 10;
		for (int i=0; i<100; i++) JoysticButtons[i] = false;
	}




	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Move_Forward"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 1) {Transp = But[0]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.KeyboardLayout, Setup.KeyBoardUp), Transp*MenuContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardUp = 0;
		NeedCheck = 1;
	}

	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Move_Backward"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 2) {Transp = But[1]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.KeyboardLayout, Setup.KeyBoardDown), Transp*MenuContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardDown = 0;
		NeedCheck = 2;
	}

	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Move_Left"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 3) {Transp = But[2]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.KeyboardLayout, Setup.KeyBoardLeft), Transp*MenuContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardLeft = 0;
		NeedCheck = 3;
	}

	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Move_Right"));
	Transp = 1.0f;
	Off = false;
	if (NeedCheck == 4) {Transp = But[3]; Off = true;};
	if (DrawButton128_2((int)X1+458, (int)Y1-6, vw_VirtualCodeName(Setup.KeyboardLayout, Setup.KeyBoardRight), Transp*MenuContentTransp, Off))
	if (NeedCheck == 0)
	{
		Setup.KeyBoardRight = 0;
		NeedCheck = 4;
	}



















	int Prir = 100;
	int X;
	int Y = 165+Prir*4;


	X = Setup.iAspectRatioWidth/2 - 366;
	if (DrawButton200_2(X,Y+28, GetText("1_Advanced"), MenuContentTransp, false))
	{
		ComBuffer = OPTIONS_ADVANCED;
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
	}


	X = Setup.iAspectRatioWidth/2 - 100;
	char Text[1024];
	sprintf(Text,GetText("1_Video_%s_Audio"),"&");
	if (DrawButton200_2(X,Y+28, Text, MenuContentTransp, false))
	{
		ComBuffer = OPTIONS;
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
	}


	X = Setup.iAspectRatioWidth/2 + 166;
	DrawButton200_2(X,Y+28, GetText("1_Config_Controls"), MenuContentTransp, true);














	X = (Setup.iAspectRatioWidth - 384)/2;
	Y = Y+Prir;
	if (DrawButton384(X,Y, GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
	{
		ComBuffer = MAIN_MENU;
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

}









