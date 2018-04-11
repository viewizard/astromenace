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
#include "../object3d/space_ship/space_ship.h"
#include "../object3d/space_ship/earth_space_fighter/earth_space_fighter.h"

namespace {

eDialogBox CurrentDialogBox{eDialogBox::None}; // current dialog box

} // unnamed namespace

enum class eDialogTypeBySize {
	w512h256,
	w512h512,
	w768h600
};

extern float CurrentAlert3;

float LastDialogUpdateTime = 0.0f;
float DialogContentTransp = 0.0f;
bool NeedShowDialog = false;
bool NeedHideDialog = false;
float StartHideTransp = 1.0f;


// что рисовать в диалоге 6,7,8
cSpaceShip *DialogSpaceShip = nullptr;
extern cEarthSpaceFighter *WorkshopFighterGame; // корабль игрока в меню шипярд
char *GetShipGroupTitle(int Num);
char *GetWorkshopShipName(int Num);
float GetShipArmor(int SpaceShipNum);

cWeapon *DialogWeapon = nullptr;
char *GetWeaponName(int Num);
char *GetWeaponGroupTitle(int Num);
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
	sRECT SrcRect, DstRect;
	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if (Transp >= 0.99f) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}


	DstRect(X,Y,X+204,Y+35);
	if  (vw_MouseOverRect(DstRect) || InFocusByKeyboard) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayDialogOnButtonSoundX != X || NeedPlayDialogOnButtonSoundY != Y) {
			Audio_PlaySound2D(15,1.0f);
			NeedPlayDialogOnButtonSoundX = X;
			NeedPlayDialogOnButtonSoundY = Y;
		}
	} else {
		if (NeedPlayDialogOnButtonSoundX == X && NeedPlayDialogOnButtonSoundY == Y) {
			NeedPlayDialogOnButtonSoundX = 0;
			NeedPlayDialogOnButtonSoundY = 0;
		}
	}



	SrcRect(2,2,230-2,64-2);
	// рисуем кнопку
	DstRect(X-14+2,Y-14+2,X+230-14-2,Y+64-14-2);
	if (!ON)
		vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/button_dialog200_out.tga"), true, 0.8f*Transp);
	else
		vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/button_dialog200_in.tga"), true, 0.8f*Transp);


	// получаем длину текста
	int Size = vw_FontSize(Text);
	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 176) {
		Size = 176;
		WScale = -176;
	}
	// находим смещение текста
	int SizeI = DstRect.left + (SrcRect.right-SrcRect.left-Size)/2;
	// рисуем текст
	if (!ON)
		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, Transp, Text);

	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}








//------------------------------------------------------------------------------------
// прорисовка кнопки - 128
//------------------------------------------------------------------------------------
bool DrawDialogButton128(int X, int Y, const char *Text, float Transp)
{
	sRECT SrcRect, DstRect;
	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if (Transp >= 0.99f) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}


	DstRect(X,Y,X+132,Y+35);
	if  (vw_MouseOverRect(DstRect) || InFocusByKeyboard) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}

		if (NeedPlayDialogOnButtonSoundX != X || NeedPlayDialogOnButtonSoundY != Y) {
			Audio_PlaySound2D(15,1.0f);
			NeedPlayDialogOnButtonSoundX = X;
			NeedPlayDialogOnButtonSoundY = Y;
		}
	} else {
		if (NeedPlayDialogOnButtonSoundX == X && NeedPlayDialogOnButtonSoundY == Y) {
			NeedPlayDialogOnButtonSoundX = 0;
			NeedPlayDialogOnButtonSoundY = 0;
		}
	}


	SrcRect(2,2,158-2,64-2);
	// рисуем кнопку
	DstRect(X-14+2,Y-14+2,X+158-14-2,Y+64-14-2);
	if (!ON)
		vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/button_dialog128_out.tga"), true, 0.8f*Transp);
	else
		vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/button_dialog128_in.tga"), true, 0.8f*Transp);


	// получаем длину текста
	int Size = vw_FontSize(Text);
	// если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
	float WScale = 0;
	if (Size > 108) {
		Size = 108;
		WScale = -108;
	}
	// находим смещение текста
	int SizeI = DstRect.left + (SrcRect.right-SrcRect.left-Size)/2;
	// рисуем текст
	if (!ON)
		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawFont(SizeI, Y+6, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, Transp, Text);

	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
			return true;
		}

	return false;
}








//------------------------------------------------------------------------------------
// прорисовка чекбокса
//------------------------------------------------------------------------------------
void DrawCheckBox_2(int X, int Y, bool *CheckBoxStatus, const char *Text, float Transp)
{
	sRECT SrcRect, DstRect;

	// получаем длину текста
	int Size = vw_FontSize(Text);

	bool ON = false;
	bool CanClick = false;


	// работаем с клавиатурой
	if ((Transp >= 0.99f) && DrawGameCursor) CurrentActiveMenuElement++;
	bool InFocusByKeyboard = false;
	if (CurrentKeyboardSelectMenuElement > 0) {
		if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
			InFocusByKeyboard = true;
		}
	}


	// 20 - расстояние между текстом
	DstRect(X+4,Y+4,X+40+20+Size,Y+40-4);
	if  ((vw_MouseOverRect(DstRect) || InFocusByKeyboard)  && DrawGameCursor) {
		// если тухнем или появляемся - не жать
		ON = true;
		if (Transp==1.0f) {
			CanClick = true;
			CurrentCursorStatus = 1;
		}
	}



	// рисуем
	SrcRect(0,0,40,38);
	DstRect(X,Y,X+40,Y+38);
	if (!ON || DragWeapon)
		vw_DrawFont(X+40+16, Y+8, 0, 0, 1.0f, 1.0f,1.0f,1.0f, Transp, Text);
	else
		vw_DrawFont(X+40+16, Y+8, 0, 0, 1.0f, 1.0f,0.5f,0.0f, Transp, Text);

	vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/checkbox_main.tga"), true, Transp);
	if (*CheckBoxStatus)
		vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/checkbox_in.tga"), true, Transp);


	if (CanClick && !DragWeapon)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			*CheckBoxStatus = !(*CheckBoxStatus);
			Audio_PlaySound2D(2,1.0f);
			if (InFocusByKeyboard) {
				vw_SetKeyStatus(SDLK_KP_ENTER, false);
				vw_SetKeyStatus(SDLK_RETURN, false);
			}
		}
}



//------------------------------------------------------------------------------------
// запрос, прорисовываем что-то?
//------------------------------------------------------------------------------------
bool isDialogBoxDrawing()
{
	return (CurrentDialogBox != eDialogBox::None);
}




//------------------------------------------------------------------------------------
// прорисовка DialogBox
//------------------------------------------------------------------------------------
void SetCurrentDialogBox(eDialogBox DialogBox)
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если что-то рисуем - продолжаем рисовать...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (isDialogBoxDrawing()) return;

	CurrentDialogBox = DialogBox;


	LastDialogUpdateTime = vw_GetTimeThread(0);
	DialogContentTransp = 0.0f;
	NeedShowDialog = true;
	NeedHideDialog = false;
	StartHideTransp = 1.0f;

	// сброс если было перетягивание
	DragWeapon = false;
	DragWeaponNum = 0;
	vw_GetMouseLeftClick(true);
}






// закрываем диалог
void CloseDialog()
{
	NeedMoreEnergyDialog = false;

	StartHideTransp = DialogContentTransp;
	NeedShowDialog = false;
	NeedHideDialog = true;
	LastDialogUpdateTime = vw_GetTimeThread(0);
}





//------------------------------------------------------------------------------------
// рисуем DrawDialogBox, если он установлен
//------------------------------------------------------------------------------------
void DrawDialogBox()
{

	if (NeedHideDialog) {
		DialogContentTransp = StartHideTransp - 3.0f*(vw_GetTimeThread(0) - LastDialogUpdateTime);

		if (DialogContentTransp <= 0.0f)
			CurrentDialogBox = eDialogBox::None;
	}

	if (NeedShowDialog && DialogContentTransp < 1.0f) {
		DialogContentTransp = 3.0f*(vw_GetTimeThread(0)-LastDialogUpdateTime);

		if (DialogContentTransp >= 1.0f) {
			DialogContentTransp = 1.0f;
			NeedShowDialog = false;
			LastDialogUpdateTime = vw_GetTimeThread(0);
		}
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если что-то есть - рисуем
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!isDialogBoxDrawing())
		return;



	// задаем параметры диалога, все остальное посчитаеться по ним...
	eDialogTypeBySize DialogType = eDialogTypeBySize::w512h256;
	switch (CurrentDialogBox) {
/*
Dialogs with default type:
	eDialogBox::QuitFromGame
	eDialogBox::ProfileCreationError
	eDialogBox::DeleteProfile
	eDialogBox::QuiToMenuNoSave
	eDialogBox::RepairShip
	eDialogBox::QuitNoSave
	eDialogBox::RestartLevelNoSave
	eDialogBox::ChoseLanguage
	eDialogBox::RestartOnOptionsChanged
	eDialogBox::RestartOnAdvOptChanged
*/
	case eDialogBox::ShowShipInfo:
	case eDialogBox::ShowWeaponsInfo:
	case eDialogBox::ShowSystemsInfo:
		DialogType = eDialogTypeBySize::w512h512;
		break;

	case eDialogBox::ProfileTipsAndTricks:
	case eDialogBox::ShipyardTipsAndTricks:
	case eDialogBox::SystemsTipsAndTricks:
	case eDialogBox::WeaponryTipsAndTricks:
	case eDialogBox::ShortkeyTipsAndTricks:
	case eDialogBox::StartMissionSecondTime:
		DialogType = eDialogTypeBySize::w768h600;
		break;

	default:
		break;
	}




	sRECT SrcRect, DstRect;
	int WTitle = 234;

	int W = 512;
	int H = 256;
	if (DialogType == eDialogTypeBySize::w512h512) {
		W = 512;
		H = 512;
	}
	if (DialogType == eDialogTypeBySize::w768h600) {
		W = 768;
		H = 600;
	}

	int X = (Setup.iAspectRatioWidth-W)/2;
	int Y = (768-H)/2;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// затемнение при выводе
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SrcRect(0,0,2,2);
	DstRect(0,0,Setup.iAspectRatioWidth,768);
	vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.6f*DialogContentTransp);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// подложка диалога
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch (DialogType) {
	case eDialogTypeBySize::w512h256:
		SrcRect(2,2,572-2,316-2 );
		DstRect(X+2-30,Y+2-30,X+572-2-30,Y+316-2-30);
		vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/dialog512_256.tga"), true, DialogContentTransp);
		break;
	case eDialogTypeBySize::w512h512:
		SrcRect(2,2,564-2,564-2 );
		DstRect(X+4-30,Y+2-30,X+564-30,Y+564-2-30);
		vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/dialog512_512.tga"), true, DialogContentTransp);
		break;
	case eDialogTypeBySize::w768h600:
		SrcRect(2,2,828-2,659-2 );
		DstRect(X+2-30,Y+2-30,X+828-2-30,Y+659-2-30);
		vw_Draw2D(&DstRect, &SrcRect, vw_FindTextureByName("menu/dialog768_600.tga"), true, DialogContentTransp);
		break;
	}



	// для название диалога
	int SizeI;
	int SizeI1;
	int TitleOffset = 25;
	if (DialogType == eDialogTypeBySize::w512h512) TitleOffset = 20;


	// для кнопок диалога
	int ButtonOffset = 190;
	if (DialogType == eDialogTypeBySize::w512h512) ButtonOffset = 256+190;
	if (DialogType == eDialogTypeBySize::w768h600) ButtonOffset = 256+88+190;






	switch (CurrentDialogBox) {
	case eDialogBox::QuitFromGame: // хотим выйти или нет?
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_QUIT")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_QUIT"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_quit_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_quit_line1"));
		} else
			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_quit_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_quit_line2"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_quit_line2"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_quit_line2"));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("1_YES"), DialogContentTransp)) {
			CanQuit = true;
			Quit = true;
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("1_NO"), DialogContentTransp)) CloseDialog();
		break;

	case eDialogBox::ProfileCreationError: // в профайле все занято, не можем создать новую запись.
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_ERROR")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_ERROR"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_ccprof_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_ccprof_line1"));
		} else
			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_ccprof_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_ccprof_line2"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_ccprof_line2"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_ccprof_line2"));

		// кнопки
		if (DrawDialogButton200(X+128+64-72/2,Y+ButtonOffset, vw_GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
		break;

	case eDialogBox::DeleteProfile: // удаление профайла - запрос
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_DELETE")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_DELETE"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_delprof_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_delprof_line1"));
		} else
			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_delprof_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_delprof_line2"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_delprof_line2"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_delprof_line2"));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("1_YES"), DialogContentTransp)) {
			DeleteRecord();
			if (CurrentProfile != -1) CurrentMission = Setup.Profile[CurrentProfile].LastMission;
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34,Y+ButtonOffset, vw_GetText("1_NO"), DialogContentTransp)) CloseDialog();
		break;


	case eDialogBox::RepairShip: // не полный ремонт (не достаточно денег)
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_REPAIR")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_REPAIR"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_cnrepair_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_cnrepair_line1"));
		} else
			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_cnrepair_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_cnrepair_line2"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_cnrepair_line2"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_cnrepair_line2"));

		// кнопки
		if (DrawDialogButton128(X+192,Y+ButtonOffset, vw_GetText("1_OK"), DialogContentTransp)) CloseDialog();
		break;


	case eDialogBox::QuitNoSave: // хотим выйти или нет?, с предупреждением, что не все сохраним
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_QUIT")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_QUIT"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_quit_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_quit_line1"));
		} else
			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_quit_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_quit_line3"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_quit_line3"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_quit_line3"));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("1_YES"), DialogContentTransp)) {
			CanQuit = true;
			Quit = true;
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("1_NO"), DialogContentTransp)) CloseDialog();
		break;
	case eDialogBox::QuiToMenuNoSave: // хотим выйти или нет?, с предупреждением, что не все сохраним
		// выход из игры в меню (основное)
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_QUIT")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_QUIT"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_quit_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_quit_line1"));
		} else

			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_quit_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_quit_line3"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_quit_line3"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_quit_line3"));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("1_YES"), DialogContentTransp)) {
			ComBuffer = eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU;
			ExitGame();
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("1_NO"), DialogContentTransp)) CloseDialog();
		break;

	case eDialogBox::RestartLevelNoSave: // хотим рестарт игры?, с предупреждением, что не все сохраним
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_RESTART")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_RESTART"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_restart_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_restart_line1"));
		} else
			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_restart_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_restart_line2"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_restart_line2"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_restart_line2"));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("1_YES"), DialogContentTransp)) {
			ComBuffer = eCommand::SWITCH_TO_GAME;
			ExitGame();
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("1_NO"), DialogContentTransp)) CloseDialog();
		break;




	// вывод данных по кораблю
	case eDialogBox::ShowShipInfo: {
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText(GetWorkshopShipName(DialogSpaceShip->ObjectCreationType))))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText(GetWorkshopShipName(DialogSpaceShip->ObjectCreationType)));
		// текст диалога
		int Y1 = Y+80;
		int Offset = 31;
		int Size = 240;
		float WScale = -200;
		int X1 = X+45;

		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Object_Class:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Space_Ship"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Ship_Type:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText(GetShipGroupTitle(DialogSpaceShip->ObjectCreationType)));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Earth_Federation"));

		Y1 += Offset;
		if (WorkshopFighterGame == DialogSpaceShip) {
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Width:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", DialogSpaceShip->Width, vw_GetText("4_units"));
		} else {
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Width:"));
			if (DialogSpaceShip->Width > WorkshopFighterGame->Width)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%2.1f (%2.1f) %s", DialogSpaceShip->Width, WorkshopFighterGame->Width, vw_GetText("4_units"));
			else
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%2.1f (%2.1f) %s", DialogSpaceShip->Width, WorkshopFighterGame->Width, vw_GetText("4_units"));
		}

		Y1 += Offset;
		if (WorkshopFighterGame == DialogSpaceShip) {
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Length:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", DialogSpaceShip->Length, vw_GetText("4_units"));
		} else {
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Length:"));
			if (DialogSpaceShip->Length > WorkshopFighterGame->Length)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%2.1f (%2.1f) %s", DialogSpaceShip->Length, WorkshopFighterGame->Length, vw_GetText("4_units"));
			else
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%2.1f (%2.1f) %s", DialogSpaceShip->Length, WorkshopFighterGame->Length, vw_GetText("4_units"));
		}


		Y1 += Offset;
		if (WorkshopFighterGame == DialogSpaceShip) {
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Height:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", DialogSpaceShip->Height, vw_GetText("4_units"));
		} else {
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Height:"));
			if (DialogSpaceShip->Height > WorkshopFighterGame->Height)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%2.1f (%2.1f) %s", DialogSpaceShip->Height, WorkshopFighterGame->Height, vw_GetText("4_units"));
			else
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%2.1f (%2.1f) %s", DialogSpaceShip->Height, WorkshopFighterGame->Height, vw_GetText("4_units"));
		}

		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Engines:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", DialogSpaceShip->Engines.size(), vw_GetText("4_units"));

		Y1 += Offset;
		if (WorkshopFighterGame == DialogSpaceShip) {
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Armor:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", (int)GetShipArmor(DialogSpaceShip->ObjectCreationType), vw_GetText("4_units"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk2:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*2), vw_GetText("4_units"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk3:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*3), vw_GetText("4_units"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk4:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*4), vw_GetText("4_units"));

		} else {
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Armor:"));
			if (GetShipArmor(DialogSpaceShip->ObjectCreationType) < GetShipArmor(WorkshopFighterGame->ObjectCreationType)) {
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%i (%i) %s", (int)GetShipArmor(DialogSpaceShip->ObjectCreationType), (int)GetShipArmor(WorkshopFighterGame->ObjectCreationType), vw_GetText("4_units"));
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk2:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*2), (int)(GetShipArmor(WorkshopFighterGame->ObjectCreationType)*2), vw_GetText("4_units"));
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk3:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*3), (int)(GetShipArmor(WorkshopFighterGame->ObjectCreationType)*3), vw_GetText("4_units"));
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk4:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*4), (int)(GetShipArmor(WorkshopFighterGame->ObjectCreationType)*4), vw_GetText("4_units"));
			} else {
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%i (%i) %s", (int)GetShipArmor(DialogSpaceShip->ObjectCreationType), (int)GetShipArmor(WorkshopFighterGame->ObjectCreationType), vw_GetText("4_units"));
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk2:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*2), (int)(GetShipArmor(WorkshopFighterGame->ObjectCreationType)*2), vw_GetText("4_units"));
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk3:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*3), (int)(GetShipArmor(WorkshopFighterGame->ObjectCreationType)*3), vw_GetText("4_units"));
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Upgrade_Mk4:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(DialogSpaceShip->ObjectCreationType)*4), (int)(GetShipArmor(WorkshopFighterGame->ObjectCreationType)*4), vw_GetText("4_units"));
			}
		}


		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Weapons_Slots:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", DialogSpaceShip->WeaponQuantity, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Slots_Levels:"));
		int SSS = 0;
		if (DialogSpaceShip->WeaponQuantity>0) {
			vw_DrawFont(X1+Size+SSS, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i", DialogSpaceShip->WeaponType[0]);
			SSS += vw_FontSize("%i", DialogSpaceShip->WeaponType[0]);
		}
		for (int i=1; i<DialogSpaceShip->WeaponQuantity; i++) {
			vw_DrawFont(X1+Size+SSS, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "/%i", DialogSpaceShip->WeaponType[i]);
			SSS += vw_FontSize("/%i", DialogSpaceShip->WeaponType[i]);
		}

		// закрываем...
		if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true)) CloseDialog();
	}
	break;

	// вывод данных по оружию
	case eDialogBox::ShowWeaponsInfo: {
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText(GetWeaponName(DialogWeapon->ObjectCreationType))))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText(GetWeaponName(DialogWeapon->ObjectCreationType)));
		// текст диалога
		int Y1 = Y+80;
		int Offset = 31;
		int Size = 230;
		float WScale = -200;
		int X1 = X+45;

		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Object_Class:"));
		if (DialogWeapon->ObjectCreationType<16)
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Weapon,_Cannon"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Weapon,_Launcher"));

		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Type:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText(GetWeaponGroupTitle(DialogWeapon->ObjectCreationType)));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Developer:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Earth_Federation"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Armor:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", (int)DialogWeapon->StrengthStart, vw_GetText("4_units"));

		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Width:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp,  "%2.1f %s", DialogWeapon->Width, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Length:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", DialogWeapon->Length, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Height:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", DialogWeapon->Height, vw_GetText("4_units"));

		Y1 += Offset;
		vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Damage,_Hull:"));

		if ((DialogWeapon->ObjectCreationType == 11) |
		    (DialogWeapon->ObjectCreationType == 12) |
		    (DialogWeapon->ObjectCreationType == 14))
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", GetWeaponHullDamage(DialogWeapon->ObjectCreationType), vw_GetText("4_units/sec"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", GetWeaponHullDamage(DialogWeapon->ObjectCreationType), vw_GetText("4_units/shot"));

		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Damage,_Systems:"));
		if ((DialogWeapon->ObjectCreationType == 11) |
		    (DialogWeapon->ObjectCreationType == 12) |
		    (DialogWeapon->ObjectCreationType == 14))
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", GetWeaponSystemsDamage(DialogWeapon->ObjectCreationType), vw_GetText("4_units/sec"));
		else
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", GetWeaponSystemsDamage(DialogWeapon->ObjectCreationType), vw_GetText("4_units/shot"));

		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Energy_Use:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", DialogWeapon->EnergyUse, vw_GetText("4_units_per_shot"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Ammo:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", DialogWeapon->AmmoStart, vw_GetText("4_units"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Reload:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", DialogWeapon->NextFireTime, vw_GetText("4_seconds"));
		Y1 += Offset;
		vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Range:"));
		vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetProjectileRange(DialogWeapon->ObjectCreationType), vw_GetText("4_units"));

		// закрываем...
		if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true)) CloseDialog();
	}
	break;

	// вывод данных по системам
	case eDialogBox::ShowSystemsInfo: {
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText(GetSystemName(DialogSystem))))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText(GetSystemName(DialogSystem)));
		// текст диалога
		int Y1 = Y+80;
		int Offset = 31;
		int Size = 230;
		float WScale = -200;
		int Size2 = 130;
		int X1 = X+45;

		switch (DialogSystem) {
		case 1:
		case 2:
		case 3:
		case 4:

			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Object_Class:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_System"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Type:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Spaceship_Engine"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Developer:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Earth_Federation"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Ion_Resistance:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", 0.0f, vw_GetText("4_units"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Energy_Use:"));
			if (NeedMoreEnergyDialog && Setup.Profile[CurrentProfile].SpaceShipControlMode != 1)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipEngineSystemEnergyUse(DialogSystem), vw_GetText("4_units_per_sec"));
			else {
				if (Setup.Profile[CurrentProfile].EngineSystem == DialogSystem) {
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetShipEngineSystemEnergyUse(DialogSystem), vw_GetText("4_units_per_sec"));
				} else if (Setup.Profile[CurrentProfile].EngineSystem == 0)
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetShipEngineSystemEnergyUse(DialogSystem), vw_GetText("4_units_per_sec"));
				else {
					if (GetShipEngineSystemEnergyUse(DialogSystem) < GetShipEngineSystemEnergyUse(Setup.Profile[CurrentProfile].EngineSystem))
						vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipEngineSystemEnergyUse(DialogSystem), GetShipEngineSystemEnergyUse(Setup.Profile[CurrentProfile].EngineSystem), vw_GetText("4_units_per_sec"));
					else
						vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipEngineSystemEnergyUse(DialogSystem), GetShipEngineSystemEnergyUse(Setup.Profile[CurrentProfile].EngineSystem), vw_GetText("4_units_per_sec"));
				}
			}

			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Engine_Power:"));
			if (Setup.Profile[CurrentProfile].EngineSystem == DialogSystem) {
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetEnginePower(DialogSystem), vw_GetText("4_units"));
			} else if (Setup.Profile[CurrentProfile].EngineSystem == 0)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetEnginePower(DialogSystem), vw_GetText("4_units"));
			else {
				if (GetEnginePower(DialogSystem) > GetEnginePower(Setup.Profile[CurrentProfile].EngineSystem))
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetEnginePower(DialogSystem), GetEnginePower(Setup.Profile[CurrentProfile].EngineSystem), vw_GetText("4_units"));
				else
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetEnginePower(DialogSystem), GetEnginePower(Setup.Profile[CurrentProfile].EngineSystem), vw_GetText("4_units"));
			}

			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Acceleration:"));
			if (Setup.Profile[CurrentProfile].EngineSystem == DialogSystem) {
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetEngineAcceleration(DialogSystem), vw_GetText("4_units"));
			} else if (Setup.Profile[CurrentProfile].EngineSystem == 0)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetEngineAcceleration(DialogSystem), vw_GetText("4_units"));
			else {
				if (GetEngineAcceleration(DialogSystem) > GetEngineAcceleration(Setup.Profile[CurrentProfile].EngineSystem))
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetEngineAcceleration(DialogSystem), GetEngineAcceleration(Setup.Profile[CurrentProfile].EngineSystem), vw_GetText("4_units"));
				else
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetEngineAcceleration(DialogSystem), GetEngineAcceleration(Setup.Profile[CurrentProfile].EngineSystem), vw_GetText("4_units"));
			}

			Y1 += Offset;
			vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Relations/Dependences:"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, vw_GetText("4_SpaceShip_Weight"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, vw_GetText("4_Power_Source"));

			if (Setup.Profile[CurrentProfile].SpaceShipControlMode == 1) {
				Y1 += Offset;
				Y1 += Offset;
				SizeI = (W-vw_FontSize(vw_GetText("4_ArcadeModeLine1")))/2-45;
				vw_DrawFont(X1+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.0f,0.0f, DialogContentTransp, vw_GetText("4_ArcadeModeLine1"));
				Y1 += Offset;
				SizeI = (W-vw_FontSize(vw_GetText("4_ArcadeModeLine2")))/2-45;
				vw_DrawFont(X1+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.0f,0.0f, DialogContentTransp, vw_GetText("4_ArcadeModeLine2"));
			}

			break;

		case 5:
		case 6:
		case 7:
		case 8:

			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Object_Class:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_System"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Type:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Power_Source"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Developer:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Earth_Federation"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Ion_Resistance:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", 0.0f, vw_GetText("4_units"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Energy_Capacity:"));

			if (Setup.Profile[CurrentProfile].PowerSystem == DialogSystem-4) {
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetShipMaxEnergy(DialogSystem-4), vw_GetText("4_units"));
			} else if (Setup.Profile[CurrentProfile].PowerSystem == 0)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetShipMaxEnergy(DialogSystem-4), vw_GetText("4_units"));
			else {
				if (GetShipMaxEnergy(DialogSystem-4) > GetShipMaxEnergy(Setup.Profile[CurrentProfile].PowerSystem))
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipMaxEnergy(DialogSystem-4), GetShipMaxEnergy(Setup.Profile[CurrentProfile].PowerSystem), vw_GetText("4_units"));
				else
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipMaxEnergy(DialogSystem-4), GetShipMaxEnergy(Setup.Profile[CurrentProfile].PowerSystem), vw_GetText("4_units"));
			}

			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Recharge_Rate:"));
			if (NeedMoreEnergyDialog)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipRechargeEnergy(DialogSystem-4), vw_GetText("4_units_per_sec"));
			else {
				if (Setup.Profile[CurrentProfile].PowerSystem == DialogSystem-4) {
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetShipRechargeEnergy(DialogSystem-4), vw_GetText("4_units_per_sec"));
				} else if (Setup.Profile[CurrentProfile].PowerSystem == 0)
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetShipRechargeEnergy(DialogSystem-4), vw_GetText("4_units_per_sec"));
				else {
					if (GetShipRechargeEnergy(DialogSystem-4) > GetShipRechargeEnergy(Setup.Profile[CurrentProfile].PowerSystem))
						vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipRechargeEnergy(DialogSystem-4), GetShipRechargeEnergy(Setup.Profile[CurrentProfile].PowerSystem), vw_GetText("4_units_per_sec"));
					else
						vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipRechargeEnergy(DialogSystem-4), GetShipRechargeEnergy(Setup.Profile[CurrentProfile].PowerSystem), vw_GetText("4_units_per_sec"));
				}
			}

			Y1 += Offset;
			vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Relations/Dependences:"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Weapon_Recharge_Rate"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Spaceship_Engine"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Optical_Computer"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Targeting_System"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Advanced_System"));

			break;

		case 9:
		case 10:
		case 11:
		case 12:

			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Object_Class:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_System"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Type:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Optical_Computer"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Developer:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Earth_Federation"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Ion_Resistance:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", 0.0f, vw_GetText("4_units"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Energy_Use:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", 0.1f, vw_GetText("4_units_per_sec"));

			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Target_Tracing:"));
			if (DialogSystem == 9)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", 1, vw_GetText("4_object(s)"));
			if (DialogSystem == 10)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", 1, vw_GetText("4_object(s)"));
			if (DialogSystem == 11)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", 6, vw_GetText("4_object(s)"));
			if (DialogSystem == 12)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i %s", 6, vw_GetText("4_object(s)"));

			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Auto_Adjustment:"));
			if (DialogSystem == 9)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_No"));
			if (DialogSystem == 10)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Yes"));
			if (DialogSystem == 11)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_No"));
			if (DialogSystem == 12)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Yes"));

			Y1 += Offset;
			vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Relations/Dependences:"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, vw_GetText("4_Power_Source"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, vw_GetText("4_Weapon_Fire_Range"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Weapon_Orientation"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Targeting_System"));

			break;

		case 13:
		case 14:
		case 15:
		case 16:

			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Object_Class:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_System"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Type:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Targeting_System"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Developer:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Earth_Federation"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Ion_Resistance:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", 0.0f, vw_GetText("4_units"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Energy_Use:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", 0.1f, vw_GetText("4_units_per_sec"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Orientation_Speed:"));
			if (Setup.Profile[CurrentProfile].TargetingMechanicSystem == DialogSystem-12) {
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", 40.0f*(DialogSystem-12), vw_GetText("4_deg_per_sec"));
			} else if (Setup.Profile[CurrentProfile].TargetingMechanicSystem == 0)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", 40.0f*(DialogSystem-12), vw_GetText("4_deg_per_sec"));
			else {
				if (DialogSystem-12 > Setup.Profile[CurrentProfile].TargetingMechanicSystem)
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", 40.0f*(DialogSystem-12), 40.0f*Setup.Profile[CurrentProfile].TargetingMechanicSystem, vw_GetText("4_deg_per_sec"));
				else
					vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, "%3.1f (%3.1f) %s", 40.0f*(DialogSystem-12), 40.0f*Setup.Profile[CurrentProfile].TargetingMechanicSystem, vw_GetText("4_deg_per_sec"));
			}

			Y1 += Offset;
			vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Relations/Dependences:"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, vw_GetText("4_Power_Source"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, vw_GetText("4_Optical_Computer"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Weapon_Orientation"));

			if (Setup.Profile[CurrentProfile].WeaponTargetingMode == 1) {
				Y1 += Offset;
				Y1 += Offset;
				SizeI = (W-vw_FontSize(vw_GetText("4_ArcadeModeLine1")))/2-45;
				vw_DrawFont(X1+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.0f,0.0f, DialogContentTransp, vw_GetText("4_ArcadeModeLine1"));
				Y1 += Offset;
				SizeI = (W-vw_FontSize(vw_GetText("4_ArcadeModeLine3")))/2-45;
				vw_DrawFont(X1+SizeI, Y1, 0, 0, 1.0f, 1.0f,0.0f,0.0f, DialogContentTransp, vw_GetText("4_ArcadeModeLine3"));
			}

			break;

		case 17:
		case 18:
		case 19:
		case 20:

			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Object_Class:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_System"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Type:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Advanced_System"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Developer:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("4_Earth_Federation"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Ion_Resistance:"));
			vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%2.1f %s", 0.0f, vw_GetText("4_units"));
			Y1 += Offset;
			vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Energy_Use:"));
			if (NeedMoreEnergyDialog)
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipProtectionSystemEnergyUse(DialogSystem-16), vw_GetText("4_units_per_sec"));
			else
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", GetShipProtectionSystemEnergyUse(DialogSystem-16), vw_GetText("4_units_per_sec"));

			if (DialogSystem == 17) {
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Hull_Repair_Rate:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%0.1f%% %s", 0.5f, vw_GetText("4_per_sec"));
			}
			if (DialogSystem == 18) {
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Resistance,_Hull:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i", 4);
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Resistance,_System:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i", 5);
			}
			if (DialogSystem == 19) {
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Shield_Power:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", 100.0f, vw_GetText("4_units"));
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Recharge_Rate:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i%% %s", 2, vw_GetText("4_per_sec"));
			}
			if (DialogSystem == 20) {
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Shield_Power:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%3.1f %s", 150.0f, vw_GetText("4_units"));
				Y1 += Offset;
				vw_DrawFont(X1, Y1, WScale, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Recharge_Rate:"));
				vw_DrawFont(X1+Size, Y1, WScale, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, "%i%% %s", 3, vw_GetText("4_per_sec"));
			}

			Y1 += Offset;
			vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Relations/Dependences:"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 1.0f,0.5f,0.0f, DialogContentTransp, vw_GetText("4_Power_Source"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Hull_Resistance"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_Systems_Resistance"));
			Y1 += Offset;
			vw_DrawFont(X1+Size2, Y1, 0, 0, 1.0f, 0.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("4_SpaceShip_Vitality"));

			break;
		}


		// закрываем...
		if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true)) CloseDialog();
	}
	break;


	case eDialogBox::ProfileTipsAndTricks: { // подсказки на меню профилей
		// название диалога
		SizeI1 = vw_FontSize(vw_GetText("9_Tipsandhints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("9_Tipsandhints"));
		// текст диалога

		SizeI = vw_FontSize(vw_GetText("9_9Line1"));
		vw_DrawFont(X+25, Y+ 80, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line1"));
		SizeI = vw_FontSize(vw_GetText("9_9Line2"));
		vw_DrawFont(X+25, Y+115, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line2"));
		SizeI = vw_FontSize(vw_GetText("9_9Line3"));
		vw_DrawFont(X+25, Y+150, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line3"));
		vw_DrawFont(X+25, Y+185, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line4"));

		SizeI = vw_FontSize(vw_GetText("9_9Line5"));
		vw_DrawFont(X+25, Y+240, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line5"));
		SizeI = vw_FontSize(vw_GetText("9_9Line6"));
		vw_DrawFont(X+25, Y+275, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line6"));
		SizeI = vw_FontSize(vw_GetText("9_9Line7"));
		vw_DrawFont(X+25, Y+310, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line7"));
		SizeI = vw_FontSize(vw_GetText("9_9Line8"));
		vw_DrawFont(X+25, Y+345, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line8"));
		vw_DrawFont(X+25, Y+380, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_9Line9"));

		SizeI = vw_FontSize(vw_GetText("9_9Line10"));
		vw_DrawFont(X+25, Y+435, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_9Line10"));
		vw_DrawFont(X+25, Y+470, -716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_9Line11"));

		// чекбокс
		bool ttt = !Setup.NeedShowHint[0];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("9_notshowtip"), DialogContentTransp);
		Setup.NeedShowHint[0] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
		break;
	}
	case eDialogBox::ShipyardTipsAndTricks: { // подсказки на меню шипъярд
		// название диалога
		SizeI1 = vw_FontSize(vw_GetText("9_Tipsandhints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("9_Tipsandhints"));
		// текст диалога

		int k=28;
		SizeI = vw_FontSize(vw_GetText("9_10Line1"));
		vw_DrawFont(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line1"));
		SizeI = vw_FontSize(vw_GetText("9_10Line2"));
		vw_DrawFont(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line2"));
		SizeI = vw_FontSize(vw_GetText("9_10Line3"));
		vw_DrawFont(X+25, Y+80+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line3"));
		vw_DrawFont(X+25, Y+80+k*3, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line4"));

		SizeI = vw_FontSize(vw_GetText("9_10Line5"));
		vw_DrawFont(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line5"));
		SizeI = vw_FontSize(vw_GetText("9_10Line6"));
		vw_DrawFont(X+25, Y+100+k*5, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line6"));
		SizeI = vw_FontSize(vw_GetText("9_10Line7"));
		vw_DrawFont(X+25, Y+100+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line7"));
		vw_DrawFont(X+25, Y+100+k*7, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line8"));

		vw_DrawFont(X+25, Y+120+k*8, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_10Line9"));
		vw_DrawFont(X+25, Y+120+k*9, -716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_10Line10"));
		vw_DrawFont(X+25, Y+120+k*10, -716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_10Line11"));
		vw_DrawFont(X+25, Y+120+k*11, -716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_10Line12"));
		vw_DrawFont(X+25, Y+120+k*12, -716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_10Line13"));
		vw_DrawFont(X+25, Y+120+k*13, -716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_10Line14"));

		// чекбокс
		bool ttt = !Setup.NeedShowHint[1];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("9_notshowtip"), DialogContentTransp);
		Setup.NeedShowHint[1] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
		break;
	}
	case eDialogBox::SystemsTipsAndTricks: { // подсказки на меню системы
		// название диалога
		SizeI1 = vw_FontSize(vw_GetText("9_Tipsandhints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("9_Tipsandhints"));
		// текст диалога

		int k=25;
		SizeI = vw_FontSize(vw_GetText("9_11Line1"));
		vw_DrawFont(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_11Line1"));
		SizeI = vw_FontSize(vw_GetText("9_11Line2"));
		vw_DrawFont(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_11Line2"));
		vw_DrawFont(X+25, Y+80+k*2, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_11Line3"));

		SizeI = vw_FontSize(vw_GetText("9_11Line4"));
		vw_DrawFont(X+25, Y+100+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_11Line4"));
		SizeI = vw_FontSize(vw_GetText("9_11Line5"));
		vw_DrawFont(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_11Line5"));
		vw_DrawFont(X+25, Y+100+k*5, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_11Line6"));

		SizeI = vw_FontSize(vw_GetText("9_11Line7"));
		vw_DrawFont(X+25, Y+120+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line7"));
		vw_DrawFont(X+40, Y+120+k*7, -701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line8"));
		SizeI = vw_FontSize(vw_GetText("9_11Line8"));
		vw_DrawFont(X+25, Y+120+k*8, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line9"));
		vw_DrawFont(X+40, Y+120+k*9, -701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line10"));
		SizeI = vw_FontSize(vw_GetText("9_11Line11"));
		vw_DrawFont(X+25, Y+120+k*10, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line11"));
		vw_DrawFont(X+40, Y+120+k*11, -701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line12"));
		SizeI = vw_FontSize(vw_GetText("9_11Line13"));
		vw_DrawFont(X+25, Y+120+k*12, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line13"));
		vw_DrawFont(X+40, Y+120+k*13, -701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line14"));
		SizeI = vw_FontSize(vw_GetText("9_11Line15"));
		vw_DrawFont(X+25, Y+120+k*14, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line15"));
		vw_DrawFont(X+40, Y+120+k*15, -701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_11Line16"));

		// чекбокс
		bool ttt = !Setup.NeedShowHint[2];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("9_notshowtip"), DialogContentTransp);
		Setup.NeedShowHint[2] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
		break;
	}
	case eDialogBox::WeaponryTipsAndTricks: { // подсказки на меню оружейная
		// название диалога
		SizeI1 = vw_FontSize(vw_GetText("9_Tipsandhints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("9_Tipsandhints"));
		// текст диалога

		int k=25;
		SizeI = vw_FontSize(vw_GetText("9_12Line1"));
		vw_DrawFont(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line1"));
		SizeI = vw_FontSize(vw_GetText("9_12Line2"));
		vw_DrawFont(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line2"));
		SizeI = vw_FontSize(vw_GetText("9_12Line3"));
		vw_DrawFont(X+25, Y+80+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line3"));
		SizeI = vw_FontSize(vw_GetText("9_12Line4"));
		vw_DrawFont(X+25, Y+80+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line4"));
		SizeI = vw_FontSize(vw_GetText("9_12Line5"));
		vw_DrawFont(X+25, Y+80+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line5"));
		vw_DrawFont(X+25, Y+80+k*5, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line6"));

		SizeI = vw_FontSize(vw_GetText("9_12Line7"));
		vw_DrawFont(X+25, Y+100+k*6, SizeI > 716 ? -716 : 716, 0,1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line7"));
		vw_DrawFont(X+25, Y+100+k*7, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line8"));

		vw_DrawFont(X+25, Y+120+k*8, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_12Line9"));
		SizeI = vw_FontSize(vw_GetText("9_12Line10"));
		vw_DrawFont(X+25, Y+120+k*9, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_12Line10"));
		vw_DrawFont(X+40, Y+120+k*10, -701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_12Line11"));
		SizeI = vw_FontSize(vw_GetText("9_12Line12"));
		vw_DrawFont(X+25, Y+120+k*11, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_12Line12"));
		SizeI = vw_FontSize(vw_GetText("9_12Line13"));
		vw_DrawFont(X+40, Y+120+k*12, SizeI > 701 ? -701 : 701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_12Line13"));
		vw_DrawFont(X+40, Y+120+k*13, -701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_12Line14"));
		SizeI = vw_FontSize(vw_GetText("9_12Line15"));
		vw_DrawFont(X+25, Y+120+k*14, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_12Line15"));
		vw_DrawFont(X+40, Y+120+k*15, -701, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_12Line16"));



		// чекбокс
		bool ttt = !Setup.NeedShowHint[3];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("9_notshowtip"), DialogContentTransp);
		Setup.NeedShowHint[3] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("1_CLOSE"), DialogContentTransp)) CloseDialog();
		break;
	}

	case eDialogBox::ShortkeyTipsAndTricks: { // подсказки на горячие клавиши в игре
		// название диалога
		SizeI1 = vw_FontSize(vw_GetText("9_Tipsandhints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("9_Tipsandhints"));
		// текст диалога

		int k=23;

		SizeI = (W-vw_FontSize(vw_GetText("9_13Line1")))/2;
		vw_DrawFont(X+SizeI, Y+80+k*0, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line1"));

		vw_DrawFont(X+25, Y+100+k*1, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line2"));

		vw_DrawFont(X+25, Y+120+k*2, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line6"));
		vw_DrawFont(X+25, Y+120+k*3, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line7"));
		vw_DrawFont(X+25, Y+120+k*4, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line8"));
		vw_DrawFont(X+25, Y+120+k*5, -716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_13Line9"));

		vw_DrawFont(X+25, Y+140+k*6, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line10"));
		vw_DrawFont(X+25, Y+140+k*7, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line11"));
		vw_DrawFont(X+25, Y+140+k*8, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line12"));

		vw_DrawFont(X+25, Y+160+k*9, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line13"));

		vw_DrawFont(X+25, Y+180+k*10, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_13Line14"));
		vw_DrawFont(X+25, Y+180+k*11, -716, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("9_13Line15"));



		// чекбокс
		bool ttt = !Setup.NeedShowHint[4];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("9_notshowtip"), DialogContentTransp);
		Setup.NeedShowHint[4] = !ttt;
		// кнопки
		if (DrawDialogButton128(X+512+90, Y+ButtonOffset, vw_GetText("1_START"), DialogContentTransp)) {
			// ничего не тянем... только включили меню
			DragWeaponNum = 0;
			DragWeaponLevel = 0;
			DragWeapon = false;
			WeaponSetupSlot = -1;
			//
			LastMenuOnOffUpdateTime = vw_GetTimeThread(0);
			ComBuffer = eCommand::TURN_OFF_WORKSHOP_MENU;
			CloseDialog();
		}
		break;
	}

	case eDialogBox::StartMissionSecondTime: { // подсказка, если пытаемся по второму разу пройти миссию
		// название диалога
		SizeI1 = vw_FontSize(vw_GetText("9_Tipsandhints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("9_Tipsandhints"));
		// текст диалога

		int k=30;

		SizeI = vw_FontSize(vw_GetText("9_15Line1"));
		vw_DrawFont(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line1"));
		vw_DrawFont(X+25, Y+80+k*1, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line2"), Setup.Profile[CurrentProfile].ByMissionExperience[CurrentMission]);

		SizeI = vw_FontSize(vw_GetText("9_15Line3"));
		vw_DrawFont(X+25, Y+100+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line3"));
		SizeI = vw_FontSize(vw_GetText("9_15Line4"));
		vw_DrawFont(X+25, Y+100+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line4"));
		SizeI = vw_FontSize(vw_GetText("9_15Line6"));
		vw_DrawFont(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line5"));
		vw_DrawFont(X+25, Y+100+k*5, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line6"));

		SizeI = vw_FontSize(vw_GetText("9_15Line7"));
		vw_DrawFont(X+25, Y+120+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line7"));
		SizeI = vw_FontSize(vw_GetText("9_15Line8"));
		vw_DrawFont(X+25, Y+120+k*7, SizeI > 716 ? -716 : 716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line8"));
		vw_DrawFont(X+25, Y+120+k*8, -716, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("9_15Line9"));


		// чекбокс
		bool ttt = !Setup.NeedShowHint[5];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("9_notshowtip"), DialogContentTransp);
		Setup.NeedShowHint[5] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("1_CLOSE"), DialogContentTransp)) {
			CloseDialog();
			ComBuffer = eCommand::SWITCH_TO_WORKSHOP;
			CurrentWorkshop = 3;
			WorkshopCreate();
		}

		break;
	}

	// спрашиваем какой язык при первом старте игры
	case eDialogBox::ChoseLanguage: {
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("3_Language")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("3_Language"));


		if (DrawDialogButton128(X+34, Y+ButtonOffset-73, vw_GetText("1_Prev"), DialogContentTransp)) {
			Setup.MenuLanguage--;
			if (Setup.MenuLanguage < 1) Setup.MenuLanguage = vw_GetLanguageListCount();

			vw_SetTextLanguage(Setup.MenuLanguage);
			// forced to regenerate textures (base texture connected to language-related characters set)
			vw_ReleaseAllFontChars();
			vw_GenerateFontChars(256, 256, vw_FindCharsSetForLanguage());
		}
		if (DrawDialogButton128(X+316+34, Y+ButtonOffset-73, vw_GetText("1_Next"), DialogContentTransp)) {
			Setup.MenuLanguage++;
			if (Setup.MenuLanguage > vw_GetLanguageListCount()) Setup.MenuLanguage = 1;

			vw_SetTextLanguage(Setup.MenuLanguage);
			// forced to regenerate textures (base texture connected to language-related characters set)
			vw_ReleaseAllFontChars();
			vw_GenerateFontChars(256, 256, vw_FindCharsSetForLanguage());
		}

		vw_SetFontSize(24);
		int Size;
		Size = vw_FontSize(vw_GetText("0_title", Setup.MenuLanguage));
		SizeI = (170-Size)/2;
		if (Size > 170)
			vw_DrawFont(X+138+34, Y+ButtonOffset-71, -170, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("0_title", Setup.MenuLanguage));
		else
			vw_DrawFont(X+138+34+SizeI, Y+ButtonOffset-71, 0, 0, 1.0f, 1.0f,1.0f,1.0f, DialogContentTransp, vw_GetText("0_title", Setup.MenuLanguage));
		vw_SetFontSize(Setup.FontSize);

		if (DrawDialogButton200(X+128+64-72/2, Y+ButtonOffset, vw_GetText("1_OK"), DialogContentTransp)) {
			// первоначально, язык голоса ставим такой же, как и язык меню
			Setup.VoiceLanguage = Setup.MenuLanguage;
			CloseDialog();
		}
		break;
	}

	case eDialogBox::RestartOnOptionsChanged: // при изменении настроек в самой игре спрашиваем, с предупреждением, что не все сохраним
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_RESTART")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_RESTART"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_restart_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_restart_line1"));
		} else
			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_restart_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_restart_line2"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_restart_line2"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_restart_line2"));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("1_YES"), DialogContentTransp)) {
			CloseDialog();
			CanQuit = false;
			Quit = true;
			NeedReCreate = true;
			SaveOptionsMenuTmpData();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("1_NO"), DialogContentTransp)) CloseDialog();
		break;
	case eDialogBox::RestartOnAdvOptChanged: // при изменении продвинутых настроек в самой игре, с предупреждением, что не все сохраним
		// название диалога
		SizeI = 17 + (WTitle-vw_FontSize(vw_GetText("1_RESTART")))/2;
		vw_DrawFont(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.7f*DialogContentTransp, vw_GetText("1_RESTART"));
		// текст диалога
		SizeI1 = vw_FontSize(vw_GetText("2_restart_line1"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+100, -470, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_restart_line1"));
		} else
			vw_DrawFont(X+SizeI, Y+100, 0, 0, 1.0f, 1.0f,1.0f,0.0f, DialogContentTransp, vw_GetText("2_restart_line1"));

		SizeI1 = vw_FontSize(vw_GetText("2_restart_line2"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawFont(X+SizeI, Y+130, -470, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_restart_line2"));
		} else
			vw_DrawFont(X+SizeI, Y+130, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*DialogContentTransp, vw_GetText("2_restart_line2"));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("1_YES"), DialogContentTransp)) {
			CloseDialog();
			CanQuit = false;
			Quit = true;
			NeedReCreate = true;
			SaveOptionsAdvMenuTmpData();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("1_NO"), DialogContentTransp)) CloseDialog();
		break;

	default:
		break;
	}










	// если можно - снимаем диалог
	if (vw_GetKeyStatus(SDLK_ESCAPE)) {
		CloseDialog();
		vw_SetKeyStatus(SDLK_ESCAPE, false);
	}
}
