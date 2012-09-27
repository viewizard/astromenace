/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Dialog.cpp

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


#include "../Defines.h"
#include "../Game.h"




//------------------------------------------------------------------------------------
// переменные...
//------------------------------------------------------------------------------------

// текущий приросовываемый диалог
int CurrentDialogBox = -1;

extern float CurrentAlert3;

float LastDialogUpdateTime = 0.0f;
float DialogContentTransp = 0.0f;
bool NeedShowDialog = false;
bool NeedHideDialog = false;
float StartHideTransp = 1.0f;


// что рисовать в диалоге 6,7,8
CSpaceShip *DialogSpaceShip = 0;
char *GetShipGroupTitle(int Num);
char *GetWorkshopShipName(int Num);
float GetShipArmor(int SpaceShipNum);

CWeapon *DialogWeapon = 0;
char *GetWeaponName(int Num);
char *GetWeaponGroupTitle(int Num);
float GetProjectileRange(int Num);
int GetWeaponHullDamage(int Num);
int GetWeaponSystemsDamage(int Num);

// номер системы... 1-20
int DialogSystem = 0;
char *GetSystemName(int Num);
float GetShipEngineSystemEnergyUse(int Num);
float GetShipProtectionSystemEnergyUse(int Num);
float GetShipRechargeEnergy(int Num);
float GetShipMaxEnergy(int Num);
float GetEnginePower(int EngineType);
float GetEngineAcceleration(int EngineType);
float GetEngineRotatePower(int EngineType);


// флаг-тянем
extern bool DragWeapon;
extern int DragWeaponNum;
extern int DragWeaponLevel;
extern int WeaponSetupSlot;
extern bool CanDrawWorkshop;

// если в инфо систем есть проблемы с энергией
bool NeedMoreEnergyDialog = false;



// для кнопок, чтобы проигрывать наведение
// последние координаты кнопки на которую наводились
int NeedPlayDialogOnButtonSoundX = 0;
int NeedPlayDialogOnButtonSoundY = 0;


//------------------------------------------------------------------------------------
// прорисовка кнопки - 200
//------------------------------------------------------------------------------------
bool DrawDialogButton200(int X, int Y, const char *Text, float Transp)
{
	RECT SrcRest, DstRest;


	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);

	bool ON = false;
	bool CanClick = false;


	SetRect(&DstRest,X,Y,X+204,Y+35);
	if  (((DstRest.right  >= MouseX)&
		(DstRest.left<= MouseX)&
		(DstRest.bottom >= MouseY)&
		(DstRest.top<= MouseY)))
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayDialogOnButtonSoundX != X || NeedPlayDialogOnButtonSoundY != Y)
		{
			Audio_PlayMenuSound(25,1.0f);
			NeedPlayDialogOnButtonSoundX = X;
			NeedPlayDialogOnButtonSoundY = Y;
		}
	}
	else
	{
		if (NeedPlayDialogOnButtonSoundX == X && NeedPlayDialogOnButtonSoundY == Y)
		{
			NeedPlayDialogOnButtonSoundX = 0;
			NeedPlayDialogOnButtonSoundY = 0;
		}
	}



	SetRect(&SrcRest,2,2,230-2,64-2);
	// рисуем кнопку
	SetRect(&DstRest,X-14+2,Y-14+2,X+230-14-2,Y+64-14-2);
	if (!ON)
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog200_out.tga"), true, 0.8f*Transp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog200_in.tga"), true, 0.8f*Transp);


	// получаем длину текста
	int Size = FontSize(Text);
	// находим смещение текста
	int SizeI = DstRest.left + (SrcRest.right-SrcRest.left-Size)/2;
	// рисуем текст
	if (!ON)
		DrawFont(SizeI, Y+6, 0, 0, 0, (0.7f*Transp)/2.0f, Text);
	else
		DrawFont(SizeI, Y+6, 0, 0, 0, Transp, Text);

	if (CanClick)
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
			return true;
		}

	return false;
}








//------------------------------------------------------------------------------------
// прорисовка кнопки - 128
//------------------------------------------------------------------------------------
bool DrawDialogButton128(int X, int Y, const char *Text, float Transp)
{
	RECT SrcRest, DstRest;


	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);

	bool ON = false;
	bool CanClick = false;


	SetRect(&DstRest,X,Y,X+132,Y+35);
	if  (((DstRest.right  >= MouseX)&
		(DstRest.left<= MouseX)&
		(DstRest.bottom >= MouseY)&
		(DstRest.top<= MouseY)))
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayDialogOnButtonSoundX != X || NeedPlayDialogOnButtonSoundY != Y)
		{
			Audio_PlayMenuSound(25,1.0f);
			NeedPlayDialogOnButtonSoundX = X;
			NeedPlayDialogOnButtonSoundY = Y;
		}
	}
	else
	{
		if (NeedPlayDialogOnButtonSoundX == X && NeedPlayDialogOnButtonSoundY == Y)
		{
			NeedPlayDialogOnButtonSoundX = 0;
			NeedPlayDialogOnButtonSoundY = 0;
		}
	}


	SetRect(&SrcRest,2,2,158-2,64-2);
	// рисуем кнопку
	SetRect(&DstRest,X-14+2,Y-14+2,X+158-14-2,Y+64-14-2);
	if (!ON)
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog128_out.tga"), true, 0.8f*Transp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_dialog128_in.tga"), true, 0.8f*Transp);


	// получаем длину текста
	int Size = FontSize(Text);
	// находим смещение текста
	int SizeI = DstRest.left + (SrcRest.right-SrcRest.left-Size)/2;
	// рисуем текст
	if (!ON)
		DrawFont(SizeI, Y+6, 0, 0, 0, (0.7f*Transp)/2.0f, Text);
	else
		DrawFont(SizeI, Y+6, 0, 0, 0, Transp, Text);

	if (CanClick)
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
			return true;
		}

	return false;
}








//------------------------------------------------------------------------------------
// прорисовка чекбокса
//------------------------------------------------------------------------------------
void DrawCheckBox_2(int X, int Y, bool *CheckBoxStatus, const char *Text, float Transp)
{
	RECT SrcRest, DstRest;
	// все проверки
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);


	// получаем длину текста
	int Size = FontSize(Text);

	bool ON = false;
	bool CanClick = false;

	// 20 - расстояние между текстом
	SetRect(&DstRest,X+4,Y+4,X+40+20+Size,Y+40-4);
	if  (((DstRest.right  >= MouseX)&
		(DstRest.left<= MouseX)&
		(DstRest.bottom >= MouseY)&
		(DstRest.top<= MouseY))  && DrawGameCursor)
	{
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f)
		{
			CanClick = true;
			CurrentCursorStatus = 1;
		}
	}



	// рисуем
	SetRect(&SrcRest,0,0,40,38);
	SetRect(&DstRest,X,Y,X+40,Y+38);
	if (!ON || DragWeapon)
		DrawFont(X+40+16, Y+8, 0, 0, 0, Transp, Text);
	else
		DrawFont(X+40+16, Y+8, 0, 0, 4, Transp, Text);

	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/checkbox_main.tga"), true, Transp);
	if (*CheckBoxStatus)
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/checkbox_in.tga"), true, Transp);


	if (CanClick && !DragWeapon)
		if (vw_GetWindowLBMouse(true))
		{
			*CheckBoxStatus = !(*CheckBoxStatus);
			Audio_PlayMenuSound(2,1.0f);
		}
}







//------------------------------------------------------------------------------------
// прорисовка DialogBox
//------------------------------------------------------------------------------------
void SetCurrentDialogBox(int DialogBox)
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если что-то рисуем - продолжаем рисовать...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (CurrentDialogBox != -1) return;

	CurrentDialogBox = DialogBox;


	LastDialogUpdateTime = vw_GetTime();
	DialogContentTransp = 0.0f;
	NeedShowDialog = true;
	NeedHideDialog = false;
	StartHideTransp = 1.0f;

	// сброс если было перетягивание
	DragWeapon = false;
	DragWeaponNum = 0;
	vw_GetWindowLBMouse(true);
}





//------------------------------------------------------------------------------------
// запрос, прорисовываем что-то?
//------------------------------------------------------------------------------------
bool isDialogBoxDrawing()
{
	if (CurrentDialogBox == -1) return false;
	else return true;
}



// закрываем диалог
void CloseDialog()
{
	NeedMoreEnergyDialog = false;

	StartHideTransp = DialogContentTransp;
	NeedShowDialog = false;
	NeedHideDialog = true;
	LastDialogUpdateTime = vw_GetTime();
}





//------------------------------------------------------------------------------------
// рисуем DrawDialogBox, если он установлен
//------------------------------------------------------------------------------------
void DrawDialogBox()
{

	if (NeedHideDialog)
	{
		DialogContentTransp = StartHideTransp - 3.0f*(vw_GetTime() - LastDialogUpdateTime);

		if (DialogContentTransp <= 0.0f)
			CurrentDialogBox = -1;
	}

	if (NeedShowDialog && DialogContentTransp < 1.0f)
	{
		DialogContentTransp = 3.0f*(vw_GetTime()-LastDialogUpdateTime);

		if (DialogContentTransp >= 1.0f)
		{
			DialogContentTransp = 1.0f;
			NeedShowDialog = false;
			LastDialogUpdateTime = vw_GetTime();
		}
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если что-то есть - рисуем
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (CurrentDialogBox == -1) return;





	// задаем параметры диалога, все остальное посчитаеться по ним...
	int DialogType = 1;
	switch (CurrentDialogBox)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			DialogType = 1;
			break;

		case 6:
		case 7:
		case 8:
			DialogType = 2;
			break;

		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			DialogType = 3;
			break;

	}




	RECT SrcRest, DstRest;
	int WTitle = 234;

	int W = 512;
	int H = 256;
	if (DialogType == 2)
	{	W = 512;H = 512;	}
	if (DialogType == 3)
	{	W = 768;H = 600;	}

	int X = (Setup.iAspectRatioWidth-W)/2;
	int Y = (768-H)/2;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// затемнение при выводе
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,0,0,Setup.iAspectRatioWidth,768);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.6f*DialogContentTransp);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// подложка диалога
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (DialogType == 1)
	{
		SetRect(&SrcRest,2,2,572-2,316-2 );
		SetRect(&DstRest,X+2-30,Y+2-30,X+572-2-30,Y+316-2-30);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/dialog512_256.tga"),
			true, DialogContentTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);
	}
	if (DialogType == 2)
	{
		SetRect(&SrcRest,2,2,564-2,564-2 );
		SetRect(&DstRest,X+4-30,Y+2-30,X+564-30,Y+564-2-30);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/dialog512_512.tga"),
			true, DialogContentTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);
	}
	if (DialogType == 3)
	{
		SetRect(&SrcRest,2,2,828-2,659-2 );
		SetRect(&DstRest,X+2-30,Y+2-30,X+828-2-30,Y+659-2-30);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/dialog768_600.tga"),
			true, DialogContentTransp, 0.0f, RI_UL_CORNER, 1.0f, 1.0f, 1.0f);
	}




	// для название диалога
	int SizeI;
	int SizeI1;
	int TitleOffset = 25;
	if (DialogType == 2) TitleOffset = 22;


	// для кнопок диалога
	int ButtonOffset = 190;
	if (DialogType == 2) ButtonOffset = 256+190;
	if (DialogType == 3) ButtonOffset = 256+88+190;






	switch (CurrentDialogBox)
	{
		case 0: // хотим выйти или нет?
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("1_QUIT")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("1_QUIT"));
			// текст диалога
			SizeI1 = FontSize(GetText("2_quit_line1"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+100, 476, 0, 1, DialogContentTransp, GetText("2_quit_line1"));
			}
			else
				DrawFont(X+SizeI, Y+100, 0, 0, 1, DialogContentTransp, GetText("2_quit_line1"));

			SizeI1 = FontSize(GetText("2_quit_line2"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+130, 476, 0, 0, 0.5f*DialogContentTransp, GetText("2_quit_line2"));
			}
			else
				DrawFont(X+SizeI, Y+130, 0, 0, 0, 0.5f*DialogContentTransp, GetText("2_quit_line2"));

			// кнопки
			if (DrawDialogButton128(X+94, Y+ButtonOffset, GetText("1_YES"), DialogContentTransp))
			{
				CanQuit = true;
				Quit = true;
				CloseDialog();
			}
			if (DrawDialogButton128(X+256+34, Y+ButtonOffset, GetText("1_NO"), DialogContentTransp)) CloseDialog();
			break;

		case 1: // в профайле все занято, не можем создать новую запись.
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("1_ERROR")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("1_ERROR"));
			// текст диалога
			SizeI1 = FontSize(GetText("2_ccprof_line1"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+100, 476, 0, 1, DialogContentTransp, GetText("2_ccprof_line1"));
			}
			else
				DrawFont(X+SizeI, Y+100, 0, 0, 1, DialogContentTransp, GetText("2_ccprof_line1"));

			SizeI1 = FontSize(GetText("2_ccprof_line2"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+130, 476, 0, 0, 0.5f*DialogContentTransp, GetText("2_ccprof_line2"));
			}
			else
				DrawFont(X+SizeI, Y+130, 0, 0, 0, 0.5f*DialogContentTransp, GetText("2_ccprof_line2"));

			// кнопки
			if (DrawDialogButton200(X+128+64-72/2,Y+ButtonOffset, GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
			break;

		case 2: // удаление профайла - запрос
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("1_DELETE")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("1_DELETE"));
			// текст диалога
			SizeI1 = FontSize(GetText("2_delprof_line1"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+100, 476, 0, 1, DialogContentTransp, GetText("2_delprof_line1"));
			}
			else
				DrawFont(X+SizeI, Y+100, 0, 0, 1, DialogContentTransp, GetText("2_delprof_line1"));

			SizeI1 = FontSize(GetText("2_delprof_line2"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+130, 476, 0, 0, 0.5f*DialogContentTransp, GetText("2_delprof_line2"));
			}
			else
				DrawFont(X+SizeI, Y+130, 0, 0, 0, 0.5f*DialogContentTransp, GetText("2_delprof_line2"));

			// кнопки
			if (DrawDialogButton128(X+94, Y+ButtonOffset, GetText("1_YES"), DialogContentTransp))
			{
				DeleteRecord();
				if (CurrentProfile != -1) CurrentMission = Setup.Profile[CurrentProfile].LastMission;
				CloseDialog();
			}
			if (DrawDialogButton128(X+256+34,Y+ButtonOffset, GetText("1_NO"), DialogContentTransp)) CloseDialog();
			break;


		case 3: // не полный ремонт (не достаточно денег)
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("1_REPAIR")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("1_REPAIR"));
			// текст диалога
			SizeI1 = FontSize(GetText("2_cnrepair_line1"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+100, 476, 0, 1, DialogContentTransp, GetText("2_cnrepair_line1"));
			}
			else
				DrawFont(X+SizeI, Y+100, 0, 0, 1, DialogContentTransp, GetText("2_cnrepair_line1"));

			SizeI1 = FontSize(GetText("2_cnrepair_line2"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+130, 476, 0, 0, 0.5f*DialogContentTransp, GetText("2_cnrepair_line2"));
			}
			else
				DrawFont(X+SizeI, Y+130, 0, 0, 0, 0.5f*DialogContentTransp, GetText("2_cnrepair_line2"));

			// кнопки
			if (DrawDialogButton128(X+192,Y+ButtonOffset, GetText("1_OK"), DialogContentTransp)) CloseDialog();
			break;


		case 4: // хотим выйти или нет?, с предупреждением, что не все сохраним
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("1_QUIT")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("1_QUIT"));
			// текст диалога
			SizeI1 = FontSize(GetText("2_quit_line1"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+100, 476, 0, 1, DialogContentTransp, GetText("2_quit_line1"));
			}
			else
				DrawFont(X+SizeI, Y+100, 0, 0, 1, DialogContentTransp, GetText("2_quit_line1"));

			SizeI1 = FontSize(GetText("2_quit_line3"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+130, 476, 0, 0, 0.5f*DialogContentTransp, GetText("2_quit_line3"));
			}
			else
				DrawFont(X+SizeI, Y+130, 0, 0, 0, 0.5f*DialogContentTransp, GetText("2_quit_line3"));

			// кнопки
			if (DrawDialogButton128(X+94, Y+ButtonOffset, GetText("1_YES"), DialogContentTransp))
			{
				CanQuit = true;
				Quit = true;
				CloseDialog();
			}
			if (DrawDialogButton128(X+256+34, Y+ButtonOffset, GetText("1_NO"), DialogContentTransp)) CloseDialog();
			break;
		case 41: // хотим выйти или нет?, с предупреждением, что не все сохраним
			// выход из игры в меню (основное)
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("1_QUIT")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("1_QUIT"));
			// текст диалога
			SizeI1 = FontSize(GetText("2_quit_line1"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+100, 476, 0, 1, DialogContentTransp, GetText("2_quit_line1"));
			}
			else

				DrawFont(X+SizeI, Y+100, 0, 0, 1, DialogContentTransp, GetText("2_quit_line1"));

			SizeI1 = FontSize(GetText("2_quit_line3"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+130, 476, 0, 0, 0.5f*DialogContentTransp, GetText("2_quit_line3"));
			}
			else
				DrawFont(X+SizeI, Y+130, 0, 0, 0, 0.5f*DialogContentTransp, GetText("2_quit_line3"));

			// кнопки
			if (DrawDialogButton128(X+94, Y+ButtonOffset, GetText("1_YES"), DialogContentTransp))
			{
				ComBuffer = 101;
				ExitGame();
				CloseDialog();
			}
			if (DrawDialogButton128(X+256+34, Y+ButtonOffset, GetText("1_NO"), DialogContentTransp)) CloseDialog();
			break;

		case 5: // хотим выйти из игры?, с предупреждением, что не все сохраним
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("1_RESTART")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("1_RESTART"));
			// текст диалога
			SizeI1 = FontSize(GetText("2_restart_line1"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+100, 476, 0, 1, DialogContentTransp, GetText("2_restart_line1"));
			}
			else
				DrawFont(X+SizeI, Y+100, 0, 0, 1, DialogContentTransp, GetText("2_restart_line1"));

			SizeI1 = FontSize(GetText("2_restart_line2"));
			SizeI = (W-SizeI1)/2;
			if (SizeI1 > 476)
			{
				SizeI = 476/2;
				SizeI = W/2 - SizeI-2;
				DrawFont(X+SizeI, Y+130, 476, 0, 0, 0.5f*DialogContentTransp, GetText("2_restart_line2"));
			}
			else
				DrawFont(X+SizeI, Y+130, 0, 0, 0, 0.5f*DialogContentTransp, GetText("2_restart_line2"));

			// кнопки
			if (DrawDialogButton128(X+94, Y+ButtonOffset, GetText("1_YES"), DialogContentTransp))
			{
				ComBuffer = GAME;
				ExitGame();
				CloseDialog();
			}
			if (DrawDialogButton128(X+256+34, Y+ButtonOffset, GetText("1_NO"), DialogContentTransp)) CloseDialog();
			break;




		// вывод данных по кораблю
		case 6:
			{
				// название диалога
				SizeI = 17 + (WTitle-FontSize(GetText(GetWorkshopShipName(DialogSpaceShip->ObjectCreationType))))/2;
				DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText(GetWorkshopShipName(DialogSpaceShip->ObjectCreationType)));
				// текст диалога
				int Y1 = Y+80;
				int Offset = 31;
				int Size = 240;
				int X1 = X+45;

				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Object_Class:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Space_Ship"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Ship_Type:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText(GetShipGroupTitle(DialogSpaceShip->ObjectCreationType)));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Developer:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Earth_Federation"));

				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Width:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", DialogSpaceShip->Width, GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Length:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", DialogSpaceShip->Length, GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Height:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", DialogSpaceShip->Height, GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Engines:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", DialogSpaceShip->EngineQuantity, GetText("4_units"));

				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Armor:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", (int)GetShipArmor(DialogSpaceShip->ObjectCreationType), GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Upgrade_Mk2:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*2), GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Upgrade_Mk3:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*3), GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Upgrade_Mk4:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*4), GetText("4_units"));

				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Weapons_Slots:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", DialogSpaceShip->WeaponQuantity, GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Slots_Levels:"));
				int SSS = 0;
				if (DialogSpaceShip->WeaponQuantity>0)
				{
					DrawFont(X1+Size+SSS, Y1, 0, 0, 0, DialogContentTransp, "%i", DialogSpaceShip->WeaponType[0]);
					SSS += FontSize("%i", DialogSpaceShip->WeaponType[0]);
				}
				for (int i=1; i<DialogSpaceShip->WeaponQuantity; i++)
				{
					DrawFont(X1+Size+SSS, Y1, 0, 0, 0, DialogContentTransp, "/%i", DialogSpaceShip->WeaponType[i]);
					SSS += FontSize("/%i", DialogSpaceShip->WeaponType[i]);
				}

				// закрываем...
				if (vw_GetWindowLBMouse(true) || vw_GetWindowRBMouse(true)) CloseDialog();
			}
			break;

		// вывод данных по оружию
		case 7:
			{
				// название диалога
				SizeI = 17 + (WTitle-FontSize(GetText(GetWeaponName(DialogWeapon->ObjectCreationType))))/2;
				DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText(GetWeaponName(DialogWeapon->ObjectCreationType)));
				// текст диалога
				int Y1 = Y+80;
				int Offset = 31;
				int Size = 230;
				int X1 = X+45;

				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Object_Class:"));
				if (DialogWeapon->ObjectCreationType<16)
					DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Weapon,_Cannon"));
				else
					DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Weapon,_Launcher"));

				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Type:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText(GetWeaponGroupTitle(DialogWeapon->ObjectCreationType)));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Developer:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Earth_Federation"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Armor:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", (int)DialogWeapon->StrengthStart, GetText("4_units"));

				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Width:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp,  "%2.1f %s", DialogWeapon->Width, GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Length:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", DialogWeapon->Length, GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Height:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", DialogWeapon->Height, GetText("4_units"));

				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Damage,_Hull:"));

				if ((DialogWeapon->ObjectCreationType == 11) |
					(DialogWeapon->ObjectCreationType == 12) |
					(DialogWeapon->ObjectCreationType == 14))
					DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", GetWeaponHullDamage(DialogWeapon->ObjectCreationType), GetText("4_units/sec"));
				else
					DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", GetWeaponHullDamage(DialogWeapon->ObjectCreationType), GetText("4_units/shot"));

				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Damage,_Systems:"));
				if ((DialogWeapon->ObjectCreationType == 11) |
					(DialogWeapon->ObjectCreationType == 12) |
					(DialogWeapon->ObjectCreationType == 14))
					DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", GetWeaponSystemsDamage(DialogWeapon->ObjectCreationType), GetText("4_units/sec"));
				else
					DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", GetWeaponSystemsDamage(DialogWeapon->ObjectCreationType), GetText("4_units/shot"));

				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Energy_Use:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", DialogWeapon->EnergyUse, GetText("4_units_per_shot"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Ammo:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", DialogWeapon->AmmoStart, GetText("4_units"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Reload:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", DialogWeapon->NextFireTime, GetText("4_seconds"));
				Y1 += Offset;
				DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Range:"));
				DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", GetProjectileRange(DialogWeapon->ObjectCreationType), GetText("4_units"));

				// закрываем...
				if (vw_GetWindowLBMouse(true) || vw_GetWindowRBMouse(true)) CloseDialog();
			}
			break;

		// вывод данных по системам
		case 8:
			{
				// название диалога
				SizeI = 17 + (WTitle-FontSize(GetText(GetSystemName(DialogSystem))))/2;
				DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText(GetSystemName(DialogSystem)));
				// текст диалога
				int Y1 = Y+80;
				int Offset = 31;
				int Size = 230;
				int Size2 = 130;
				int X1 = X+45;

				switch (DialogSystem)
				{
					case 1:
					case 2:
					case 3:
					case 4:

						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Object_Class:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_System"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Type:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Spaceship_Engine"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Developer:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Earth_Federation"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Ion_Resistance:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", 0.0f, GetText("4_units"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Energy_Use:"));
						if (NeedMoreEnergyDialog && Setup.Profile[CurrentProfile].SpaceShipControlMode != 1)
							DrawFont(X1+Size, Y1, 0, 0, 2, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipEngineSystemEnergyUse(DialogSystem), GetText("4_units_per_sec"));
						else
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", GetShipEngineSystemEnergyUse(DialogSystem), GetText("4_units_per_sec"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Engine_Power:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", GetEnginePower(DialogSystem), GetText("4_units"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Acceleration:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", GetEngineAcceleration(DialogSystem), GetText("4_units"));

						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Relations/Dependences:"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 4, DialogContentTransp, GetText("4_SpaceShip_Weight"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 4, DialogContentTransp, GetText("4_Power_Source"));

						if (Setup.Profile[CurrentProfile].SpaceShipControlMode == 1)
						{
							Y1 += Offset;
							Y1 += Offset;
							SizeI = (W-FontSize(GetText("4_ArcadeModeLine1")))/2-45;
							DrawFont(X1+SizeI, Y1, 0, 0, 2, DialogContentTransp, GetText("4_ArcadeModeLine1"));
							Y1 += Offset;
							SizeI = (W-FontSize(GetText("4_ArcadeModeLine2")))/2-45;
							DrawFont(X1+SizeI, Y1, 0, 0, 2, DialogContentTransp, GetText("4_ArcadeModeLine2"));
						}

						break;

					case 5:
					case 6:
					case 7:
					case 8:

						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Object_Class:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_System"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Type:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Power_Source"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Developer:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Earth_Federation"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Ion_Resistance:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", 0.0f, GetText("4_units"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Energy_Capacity:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", GetShipMaxEnergy(DialogSystem-4), GetText("4_units"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Recharge_Rate:"));
						if (NeedMoreEnergyDialog)
							DrawFont(X1+Size, Y1, 0, 0, 2, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipRechargeEnergy(DialogSystem-4), GetText("4_units_per_sec"));
						else
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", GetShipRechargeEnergy(DialogSystem-4), GetText("4_units_per_sec"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Relations/Dependences:"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Weapon_Recharge_Rate"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Spaceship_Engine"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Optical_Computer"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Targeting_System"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Advanced_System"));

						break;

					case 9:
					case 10:
					case 11:
					case 12:

						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Object_Class:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_System"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Type:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Optical_Computer"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Developer:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Earth_Federation"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Ion_Resistance:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", 0.0f, GetText("4_units"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Energy_Use:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", 0.1f, GetText("4_units_per_sec"));

						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Target_Tracing:"));
						if (DialogSystem == 9)
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", 1, GetText("4_object(s)"));
						if (DialogSystem == 10)
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", 1, GetText("4_object(s)"));
						if (DialogSystem == 11)
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", 6, GetText("4_object(s)"));
						if (DialogSystem == 12)
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i %s", 6, GetText("4_object(s)"));

						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Auto_Adjustment:"));
						if (DialogSystem == 9)
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_No"));
						if (DialogSystem == 10)
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Yes"));
						if (DialogSystem == 11)
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_No"));
						if (DialogSystem == 12)
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Yes"));

						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Relations/Dependences:"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 4, DialogContentTransp, GetText("4_Power_Source"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 4, DialogContentTransp, GetText("4_Weapon_Fire_Range"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Weapon_Orientation"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Targeting_System"));

						break;

					case 13:
					case 14:
					case 15:
					case 16:

						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Object_Class:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_System"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Type:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Targeting_System"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Developer:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Earth_Federation"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Ion_Resistance:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", 0.0f, GetText("4_units"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Energy_Use:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", 0.1f, GetText("4_units_per_sec"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Orientation_Speed:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", 40.0f*(DialogSystem-12), GetText("4_deg_per_sec"));

						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Relations/Dependences:"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 4, DialogContentTransp, GetText("4_Power_Source"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 4, DialogContentTransp, GetText("4_Optical_Computer"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Weapon_Orientation"));

						if (Setup.Profile[CurrentProfile].WeaponTargetingMode == 1)
						{
							Y1 += Offset;
							Y1 += Offset;
							SizeI = (W-FontSize(GetText("4_ArcadeModeLine1")))/2-45;
							DrawFont(X1+SizeI, Y1, 0, 0, 2, DialogContentTransp, GetText("4_ArcadeModeLine1"));
							Y1 += Offset;
							SizeI = (W-FontSize(GetText("4_ArcadeModeLine3")))/2-45;
							DrawFont(X1+SizeI, Y1, 0, 0, 2, DialogContentTransp, GetText("4_ArcadeModeLine3"));
						}

						break;

					case 17:
					case 18:
					case 19:
					case 20:

						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Object_Class:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_System"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Type:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Advanced_System"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Developer:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, GetText("4_Earth_Federation"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Ion_Resistance:"));
						DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%2.1f %s", 0.0f, GetText("4_units"));
						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Energy_Use:"));
						if (NeedMoreEnergyDialog)
							DrawFont(X1+Size, Y1, 0, 0, 2, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipProtectionSystemEnergyUse(DialogSystem-16), GetText("4_units_per_sec"));
						else
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", GetShipProtectionSystemEnergyUse(DialogSystem-16), GetText("4_units_per_sec"));

						if (DialogSystem == 17)
						{
							Y1 += Offset;
							DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Hull_Repair_Rate:"));
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%0.1f%% %s", 0.5f, GetText("4_per_sec"));
						}
						if (DialogSystem == 18)
						{
							Y1 += Offset;
							DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Resistance,_Hull:"));
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i", 4);
							Y1 += Offset;
							DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Resistance,_System:"));
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i", 5);
						}
						if (DialogSystem == 19)
						{
							Y1 += Offset;
							DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Shield_Power:"));
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", 100.0f, GetText("4_units"));
							Y1 += Offset;
							DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Recharge_Rate:"));
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i%% %s", 2, GetText("4_per_sec"));
						}
						if (DialogSystem == 20)
						{
							Y1 += Offset;
							DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Shield_Power:"));
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%3.1f %s", 150.0f, GetText("4_units"));
							Y1 += Offset;
							DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Recharge_Rate:"));
							DrawFont(X1+Size, Y1, 0, 0, 0, DialogContentTransp, "%i%% %s", 3, GetText("4_per_sec"));
						}

						Y1 += Offset;
						DrawFont(X1, Y1, 0, 0, 1, DialogContentTransp, GetText("4_Relations/Dependences:"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 4, DialogContentTransp, GetText("4_Power_Source"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Hull_Resistance"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_Systems_Resistance"));
						Y1 += Offset;
						DrawFont(X1+Size2, Y1, 0, 0, 3, DialogContentTransp, GetText("4_SpaceShip_Vitality"));

						break;
				}


				// закрываем...
				if (vw_GetWindowLBMouse(true) || vw_GetWindowRBMouse(true)) CloseDialog();
			}
			break;


		case 9: // подсказки на меню профайлов
		{
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("9_Tipsandhints")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("9_Tipsandhints"));
			// текст диалога

			DrawFont(X+25, Y+ 80, 716, 0, 0, DialogContentTransp, GetText("9_9Line1"));
			DrawFont(X+25, Y+115, 716, 0, 0, DialogContentTransp, GetText("9_9Line2"));
			DrawFont(X+25, Y+150, 716, 0, 0, DialogContentTransp, GetText("9_9Line3"));
			DrawFont(X+25, Y+185, 0, 0, 0, DialogContentTransp, GetText("9_9Line4"));

			DrawFont(X+25, Y+240, 716, 0, 0, DialogContentTransp, GetText("9_9Line5"));
			DrawFont(X+25, Y+275, 716, 0, 0, DialogContentTransp, GetText("9_9Line6"));
			DrawFont(X+25, Y+310, 716, 0, 0, DialogContentTransp, GetText("9_9Line7"));
			DrawFont(X+25, Y+345, 716, 0, 0, DialogContentTransp, GetText("9_9Line8"));
			DrawFont(X+25, Y+380, 0, 0, 0, DialogContentTransp, GetText("9_9Line9"));

			DrawFont(X+25, Y+435, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_9Line10"));
			DrawFont(X+25, Y+470, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_9Line11"));

			// чекбокс
			bool ttt = !Setup.NeedShowHint[0];
			DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, GetText("9_notshowtip"), DialogContentTransp);
			Setup.NeedShowHint[0] = !ttt;
			// кнопки
			if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
			break;
		}
		case 10: // подсказки на меню шипъярд
		{
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("9_Tipsandhints")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("9_Tipsandhints"));
			// текст диалога

			int k=28;
			DrawFont(X+25, Y+80+k*0, 716, 0, 0, DialogContentTransp, GetText("9_10Line1"));
			DrawFont(X+25, Y+80+k*1, 716, 0, 0, DialogContentTransp, GetText("9_10Line2"));
			DrawFont(X+25, Y+80+k*2, 716, 0, 0, DialogContentTransp, GetText("9_10Line3"));
			DrawFont(X+25, Y+80+k*3, 0, 0, 0, DialogContentTransp, GetText("9_10Line4"));

			DrawFont(X+25, Y+100+k*4, 716, 0, 0, DialogContentTransp, GetText("9_10Line5"));
			DrawFont(X+25, Y+100+k*5, 716, 0, 0, DialogContentTransp, GetText("9_10Line6"));
			DrawFont(X+25, Y+100+k*6, 716, 0, 0, DialogContentTransp, GetText("9_10Line7"));
			DrawFont(X+25, Y+100+k*7, 0, 0, 0, DialogContentTransp, GetText("9_10Line8"));

			DrawFont(X+25, Y+120+k*8, 0, 0, 0, DialogContentTransp, GetText("9_10Line9"));
			DrawFont(X+25, Y+120+k*9, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_10Line10"));
			DrawFont(X+25, Y+120+k*10, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_10Line11"));
			DrawFont(X+25, Y+120+k*11, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_10Line12"));
			DrawFont(X+25, Y+120+k*12, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_10Line13"));
			DrawFont(X+25, Y+120+k*13, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_10Line14"));

			// чекбокс
			bool ttt = !Setup.NeedShowHint[1];
			DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, GetText("9_notshowtip"), DialogContentTransp);
			Setup.NeedShowHint[1] = !ttt;
			// кнопки
			if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
			break;
		}
		case 11: // подсказки на меню системы
		{
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("9_Tipsandhints")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("9_Tipsandhints"));
			// текст диалога

			int k=25;
			DrawFont(X+25, Y+80+k*0, 716, 0, 0, DialogContentTransp, GetText("9_11Line1"));
			DrawFont(X+25, Y+80+k*1, 716, 0, 0, DialogContentTransp, GetText("9_11Line2"));
			DrawFont(X+25, Y+80+k*2, 0, 0, 0, DialogContentTransp, GetText("9_11Line3"));

			DrawFont(X+25, Y+100+k*3, 716, 0, 0, DialogContentTransp, GetText("9_11Line4"));
			DrawFont(X+25, Y+100+k*4, 716, 0, 0, DialogContentTransp, GetText("9_11Line5"));
			DrawFont(X+25, Y+100+k*5, 0, 0, 0, DialogContentTransp, GetText("9_11Line6"));

			DrawFont(X+25, Y+120+k*6, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line7"));
			DrawFont(X+40, Y+120+k*7, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line8"));
			DrawFont(X+25, Y+120+k*8, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line9"));
			DrawFont(X+40, Y+120+k*9, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line10"));
			DrawFont(X+25, Y+120+k*10, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line11"));
			DrawFont(X+40, Y+120+k*11, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line12"));
			DrawFont(X+25, Y+120+k*12, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line13"));
			DrawFont(X+40, Y+120+k*13, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line14"));
			DrawFont(X+25, Y+120+k*14, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line15"));
			DrawFont(X+40, Y+120+k*15, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_11Line16"));

			// чекбокс
			bool ttt = !Setup.NeedShowHint[2];
			DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, GetText("9_notshowtip"), DialogContentTransp);
			Setup.NeedShowHint[2] = !ttt;
			// кнопки
			if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
			break;
		}
		case 12: // подсказки на меню оружейная
		{
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("9_Tipsandhints")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("9_Tipsandhints"));
			// текст диалога

			int k=25;
			DrawFont(X+25, Y+80+k*0, 716, 0, 0, DialogContentTransp, GetText("9_12Line1"));
			DrawFont(X+25, Y+80+k*1, 716, 0, 0, DialogContentTransp, GetText("9_12Line2"));
			DrawFont(X+25, Y+80+k*2, 716, 0, 0, DialogContentTransp, GetText("9_12Line3"));
			DrawFont(X+25, Y+80+k*3, 716, 0, 0, DialogContentTransp, GetText("9_12Line4"));
			DrawFont(X+25, Y+80+k*4, 716, 0, 0, DialogContentTransp, GetText("9_12Line5"));
			DrawFont(X+25, Y+80+k*5, 0, 0, 0, DialogContentTransp, GetText("9_12Line6"));

			DrawFont(X+25, Y+100+k*6, 716, 0, 0, DialogContentTransp, GetText("9_12Line7"));
			DrawFont(X+25, Y+100+k*7, 0, 0, 0, DialogContentTransp, GetText("9_12Line8"));

			DrawFont(X+25, Y+120+k*8, 0, 0, 0, DialogContentTransp, GetText("9_12Line9"));
			DrawFont(X+25, Y+120+k*9, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_12Line10"));
			DrawFont(X+40, Y+120+k*10, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_12Line11"));
			DrawFont(X+25, Y+120+k*11, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_12Line12"));
			DrawFont(X+40, Y+120+k*12, 716-15, 0, 0, 0.5f*DialogContentTransp, GetText("9_12Line13"));
			DrawFont(X+40, Y+120+k*13, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_12Line14"));
			DrawFont(X+25, Y+120+k*14, 716, 0, 0, 0.5f*DialogContentTransp, GetText("9_12Line15"));
			DrawFont(X+40, Y+120+k*15, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_12Line16"));



			// чекбокс
			bool ttt = !Setup.NeedShowHint[3];
			DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, GetText("9_notshowtip"), DialogContentTransp);
			Setup.NeedShowHint[3] = !ttt;
			// кнопки
			if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
			break;
		}

		case 13: // подсказки на горячие клавиши в игре
		{
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("9_Tipsandhints")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("9_Tipsandhints"));
			// текст диалога

			int k=23;

			SizeI = (W-FontSize(GetText("9_13Line1")))/2;
			DrawFont(X+SizeI, Y+80+k*0, 0, 0, 0, DialogContentTransp, GetText("9_13Line1"));

			DrawFont(X+25, Y+100+k*1, 0, 0, 0, DialogContentTransp, GetText("9_13Line2"));

			DrawFont(X+25, Y+120+k*2, 0, 0, 0, DialogContentTransp, GetText("9_13Line6"));
			DrawFont(X+25, Y+120+k*3, 0, 0, 0, DialogContentTransp, GetText("9_13Line7"));
			DrawFont(X+25, Y+120+k*4, 0, 0, 0, DialogContentTransp, GetText("9_13Line8"));
			DrawFont(X+25, Y+120+k*5, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_13Line9"));

			DrawFont(X+25, Y+140+k*6, 0, 0, 0, DialogContentTransp, GetText("9_13Line10"));
			DrawFont(X+25, Y+140+k*7, 0, 0, 0, DialogContentTransp, GetText("9_13Line11"));
			DrawFont(X+25, Y+140+k*8, 0, 0, 0, DialogContentTransp, GetText("9_13Line12"));

			DrawFont(X+25, Y+160+k*9, 0, 0, 0, DialogContentTransp, GetText("9_13Line13"));

			DrawFont(X+25, Y+180+k*10, 0, 0, 0, DialogContentTransp, GetText("9_13Line14"));
			DrawFont(X+25, Y+180+k*11, 0, 0, 0, 0.5f*DialogContentTransp, GetText("9_13Line15"));



			// чекбокс
			bool ttt = !Setup.NeedShowHint[4];
			DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, GetText("9_notshowtip"), DialogContentTransp);
			Setup.NeedShowHint[4] = !ttt;
			// кнопки
			if (DrawDialogButton128(X+512+90, Y+ButtonOffset, GetText("1_START"), DialogContentTransp))
			{
			//	WorkshopDestroyData();
			//	CanDrawWorkshop = false;
				// ничего не тянем... только включили меню
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeapon = false;
				WeaponSetupSlot = -1;
				//
				ComBuffer = 99;
				CloseDialog();
			}
			break;
		}

		case 14: // подсказка, если пытаемся по второму разу пройти миссию
		{
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("9_Tipsandhints")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("9_Tipsandhints"));
			// текст диалога

			int k=30;

			DrawFont(X+25, Y+80+k*0, 716, 0, 0, DialogContentTransp, GetText("9_15Line1"));
			DrawFont(X+25, Y+80+k*1, 0, 0, 0, DialogContentTransp, GetText("9_15Line2"), Setup.Profile[CurrentProfile].ByMissionExperience[CurrentMission]);

			DrawFont(X+25, Y+100+k*2, 716, 0, 0, DialogContentTransp, GetText("9_15Line3"));
			DrawFont(X+25, Y+100+k*3, 716, 0, 0, DialogContentTransp, GetText("9_15Line4"));
			DrawFont(X+25, Y+100+k*4, 716, 0, 0, DialogContentTransp, GetText("9_15Line5"));
			DrawFont(X+25, Y+100+k*5, 0, 0, 0, DialogContentTransp, GetText("9_15Line6"));

			DrawFont(X+25, Y+120+k*6, 716, 0, 0, DialogContentTransp, GetText("9_15Line7"));
			DrawFont(X+25, Y+120+k*7, 716, 0, 0, DialogContentTransp, GetText("9_15Line8"));
			DrawFont(X+25, Y+120+k*8, 0, 0, 0, DialogContentTransp, GetText("9_15Line9"));


			// чекбокс
			bool ttt = !Setup.NeedShowHint[5];
			DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, GetText("9_notshowtip"), DialogContentTransp);
			Setup.NeedShowHint[5] = !ttt;
			// кнопки
			if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, GetText("1_CLOSE"), DialogContentTransp))
			{
				CloseDialog();
				ComBuffer = WORKSHOP;
				CurrentWorkshop = 3;
				WorkshopCreate();
			}

			break;
		}


		case 15: // подсказка, для линукс версии - пожертвование
		{
			// название диалога
			SizeI = 17 + (WTitle-FontSize(GetText("9_Tipsandhints")))/2;
			DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 0, 0.7f*DialogContentTransp, GetText("9_Tipsandhints"));
			// текст диалога



			int k=33;

			SizeI = (768 - FontSize(GetText("9_18Line1")))/2;
			DrawFont(X+SizeI, Y+80+k*0, 0, 0, 3, DialogContentTransp, GetText("9_18Line1"));

			DrawFont(X+25, Y+80+k*2, 716, 0, 0, DialogContentTransp, GetText("9_18Line2"));
			DrawFont(X+25, Y+80+k*3, 0, 0, 0, DialogContentTransp, GetText("9_18Line3"));

			DrawFont(X+25, Y+80+k*5, 716, 0, 0, DialogContentTransp, GetText("9_18Line4"));
			DrawFont(X+25, Y+80+k*6, 716, 0, 0, DialogContentTransp, GetText("9_18Line5"));
			DrawFont(X+25, Y+80+k*7, 0, 0, 0, DialogContentTransp, GetText("9_18Line6"));

			if (DrawDialogButton200(X+284, Y+80+k*9, GetText("1_DONATE"), DialogContentTransp))
			{

				// нужно свернуть игру, запустить броузер и выйти в основное меню
				SDL_WM_IconifyWindow();

				switch (Setup.MenuLanguage)
				{
					case 1: //en
						vw_OpenBrouser("http://www.viewizard.com/astromenace/donors.php");
						break;
					case 2: //de
						vw_OpenBrouser("http://www.viewizard.com/de/astromenace/donors.php");
						break;
					case 3: //ru
						vw_OpenBrouser("http://www.viewizard.com/ru/astromenace/donors.php");
						break;
				}
			}


			SizeI = (768 - FontSize(GetText("9_18Line7")))/2;
			DrawFont(X+SizeI, Y+80+k*11, 0, 0, 3, DialogContentTransp, GetText("9_18Line7"));

			SizeI = (768 - FontSize(GetText("9_18Line8")))/2;
			DrawFont(X+SizeI, Y+80+k*12, 0, 0, 3, DialogContentTransp, GetText("9_18Line8"));




/*
			DrawFont(X+25, Y+80+k*0, 716, 0, 0, DialogContentTransp, GetText("9_15Line1"));
			DrawFont(X+25, Y+80+k*1, 0, 0, 0, DialogContentTransp, GetText("9_15Line2"), Setup.Profile[CurrentProfile].ByMissionExperience[CurrentMission]);

			DrawFont(X+25, Y+100+k*2, 716, 0, 0, DialogContentTransp, GetText("9_15Line3"));
			DrawFont(X+25, Y+100+k*3, 716, 0, 0, DialogContentTransp, GetText("9_15Line4"));
			DrawFont(X+25, Y+100+k*4, 716, 0, 0, DialogContentTransp, GetText("9_15Line5"));
			DrawFont(X+25, Y+100+k*5, 0, 0, 0, DialogContentTransp, GetText("9_15Line6"));

			DrawFont(X+25, Y+120+k*6, 716, 0, 0, DialogContentTransp, GetText("9_15Line7"));
			DrawFont(X+25, Y+120+k*7, 716, 0, 0, DialogContentTransp, GetText("9_15Line8"));
			DrawFont(X+25, Y+120+k*8, 0, 0, 0, DialogContentTransp, GetText("9_15Line9"));
*/

			// чекбокс
			bool ttt = !Setup.NeedShowHint[6];
			DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, GetText("9_notshowtip"), DialogContentTransp);
			Setup.NeedShowHint[6] = !ttt;
			// кнопки
			if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
			break;
		}


	}










	// если можно - снимаем диалог
	if (vw_GetKeys(SDLK_ESCAPE))
	{
		CloseDialog();
		vw_SetKeys(SDLK_ESCAPE, false);
	}
}
