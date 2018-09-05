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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../game.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../main.h"
#include "../object3d/space_ship/space_ship.h"
#include "../object3d/projectile/projectile.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

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
std::weak_ptr<cSpaceShip> DialogSpaceShip{};
extern std::weak_ptr<cSpaceShip> WorkshopFighterGame; // корабль игрока в меню шипярд
char *GetShipGroupTitle(int Num);
char *GetWorkshopShipName(int Num);

cWeapon *DialogWeapon = nullptr;
char *GetWeaponName(int Num);
char *GetWeaponGroupTitle(int Num);

// номер системы... 1-20
int DialogSystem = 0;
char *GetSystemName(int Num);
float GetShipEngineSystemEnergyUse(int Num);
float GetShipProtectionSystemEnergyUse(int Num);
float GetShipRechargeEnergy(int Num);
float GetShipMaxEnergy(int Num);


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



void InitDialogBoxes()
{
	LastDialogUpdateTime = 0.0f;
	DialogContentTransp = 0.0f;
	NeedShowDialog = false;
	NeedHideDialog = false;
	StartHideTransp = 1.0f;
	CurrentDialogBox = eDialogBox::None;
}


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
			PlayMenuSFX(eMenuSFX::OverSmallButton, 1.0f);
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
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/button_dialog200_out.tga"), true, 0.8f*Transp);
	else
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/button_dialog200_in.tga"), true, 0.8f*Transp);


	// получаем длину текста
	int Size = vw_TextWidth(Text);
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
		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, Transp, Text);

	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
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
			PlayMenuSFX(eMenuSFX::OverSmallButton, 1.0f);
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
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/button_dialog128_out.tga"), true, 0.8f*Transp);
	else
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/button_dialog128_in.tga"), true, 0.8f*Transp);


	// получаем длину текста
	int Size = vw_TextWidth(Text);
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
		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, (0.7f*Transp)/2.0f, Text);
	else
		vw_DrawText(SizeI, Y+6, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, Transp, Text);

	if (CanClick)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
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
	int Size = vw_TextWidth(Text);

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
		vw_DrawText(X+40+16, Y+8, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, Transp, Text);
	else
		vw_DrawText(X+40+16, Y+8, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, Transp, Text);

	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/checkbox_main.tga"), true, Transp);
	if (*CheckBoxStatus)
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/checkbox_in.tga"), true, Transp);


	if (CanClick && !DragWeapon)
		if (vw_GetMouseLeftClick(true) || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN)))) {
			*CheckBoxStatus = !(*CheckBoxStatus);
			PlayMenuSFX(eMenuSFX::Click, 1.0f);
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

	int X = (GameConfig().InternalWidth - W) / 2;
	int Y = (768-H)/2;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// затемнение при выводе
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SrcRect(0, 0, 2, 2);
	DstRect(0, 0, GameConfig().InternalWidth, 768);
	vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/blackpoint.tga"), true, 0.6f*DialogContentTransp);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// подложка диалога
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch (DialogType) {
	case eDialogTypeBySize::w512h256:
		SrcRect(2,2,572-2,316-2 );
		DstRect(X+2-30,Y+2-30,X+572-2-30,Y+316-2-30);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/dialog512_256.tga"), true, DialogContentTransp);
		break;
	case eDialogTypeBySize::w512h512:
		SrcRect(2,2,564-2,564-2 );
		DstRect(X+4-30,Y+2-30,X+564-30,Y+564-2-30);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/dialog512_512.tga"), true, DialogContentTransp);
		break;
	case eDialogTypeBySize::w768h600:
		SrcRect(2,2,828-2,659-2 );
		DstRect(X+2-30,Y+2-30,X+828-2-30,Y+659-2-30);
		vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/dialog768_600.tga"), true, DialogContentTransp);
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
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("QUIT")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("QUIT"));
		// текст диалога
		SizeI1 = vw_TextWidth(vw_GetText("Are you sure you want to quit?"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to quit?"));
		} else
			vw_DrawText(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to quit?"));

		SizeI1 = vw_TextWidth(vw_GetText("Note: all game data will be saved."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: all game data will be saved."));
		} else
			vw_DrawText(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: all game data will be saved."));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("YES"), DialogContentTransp)) {
			QuitFromMainLoop();
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("NO"), DialogContentTransp)) CloseDialog();
		break;

	case eDialogBox::ProfileCreationError: // в профайле все занято, не можем создать новую запись.
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("ERROR")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("ERROR"));
		// текст диалога
		SizeI1 = vw_TextWidth(vw_GetText("Cannot create a Pilot Profile."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Cannot create a Pilot Profile."));
		} else
			vw_DrawText(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Cannot create a Pilot Profile."));

		SizeI1 = vw_TextWidth(vw_GetText("Tip: you should clear one line first."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Tip: you should clear one line first."));
		} else
			vw_DrawText(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Tip: you should clear one line first."));

		// кнопки
		if (DrawDialogButton200(X+128+64-72/2,Y+ButtonOffset, vw_GetText("CLOSE"), DialogContentTransp)) CloseDialog();
		break;

	case eDialogBox::DeleteProfile: // удаление профайла - запрос
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("DELETE")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("DELETE"));
		// текст диалога
		SizeI1 = vw_TextWidth(vw_GetText("Are you sure you want to delete the Profile?"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to delete the Profile?"));
		} else
			vw_DrawText(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to delete the Profile?"));

		SizeI1 = vw_TextWidth(vw_GetText("Note: all Pilot Profile data will be lost."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: all Pilot Profile data will be lost."));
		} else
			vw_DrawText(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: all Pilot Profile data will be lost."));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("YES"), DialogContentTransp)) {
			DeleteRecord();
			if (CurrentProfile != -1)
				CurrentMission = GameConfig().Profile[CurrentProfile].LastMission;
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34,Y+ButtonOffset, vw_GetText("NO"), DialogContentTransp))
			CloseDialog();
		break;


	case eDialogBox::RepairShip: // не полный ремонт (не достаточно денег)
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("REPAIR")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("REPAIR"));
		// текст диалога
		SizeI1 = vw_TextWidth(vw_GetText("Not enough money for a full hull repair."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Not enough money for a full hull repair."));
		} else
			vw_DrawText(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Not enough money for a full hull repair."));

		SizeI1 = vw_TextWidth(vw_GetText("Note: Repair has been limited by money."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: Repair has been limited by money."));
		} else
			vw_DrawText(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: Repair has been limited by money."));

		// кнопки
		if (DrawDialogButton128(X+192,Y+ButtonOffset, vw_GetText("OK"), DialogContentTransp)) CloseDialog();
		break;


	case eDialogBox::QuitNoSave: // хотим выйти или нет?, с предупреждением, что не все сохраним
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("QUIT")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("QUIT"));
		// текст диалога
		SizeI1 = vw_TextWidth(vw_GetText("Are you sure you want to quit?"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to quit?"));
		} else
			vw_DrawText(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to quit?"));

		SizeI1 = vw_TextWidth(vw_GetText("Note: the current game data will be lost."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: the current game data will be lost."));
		} else
			vw_DrawText(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: the current game data will be lost."));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("YES"), DialogContentTransp)) {
			QuitFromMainLoop();
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("NO"), DialogContentTransp)) CloseDialog();
		break;
	case eDialogBox::QuiToMenuNoSave: // хотим выйти или нет?, с предупреждением, что не все сохраним
		// выход из игры в меню (основное)
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("QUIT")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("QUIT"));
		// текст диалога
		SizeI1 = vw_TextWidth(vw_GetText("Are you sure you want to quit?"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to quit?"));
		} else

			vw_DrawText(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to quit?"));

		SizeI1 = vw_TextWidth(vw_GetText("Note: the current game data will be lost."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: the current game data will be lost."));
		} else
			vw_DrawText(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: the current game data will be lost."));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("YES"), DialogContentTransp)) {
			ComBuffer = eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU;
			ExitGame();
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("NO"), DialogContentTransp)) CloseDialog();
		break;

	case eDialogBox::RestartLevelNoSave: // хотим рестарт игры?, с предупреждением, что не все сохраним
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("RESTART")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("RESTART"));
		// текст диалога
		SizeI1 = vw_TextWidth(vw_GetText("Are you sure you want to restart the game?"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to restart the game?"));
		} else
			vw_DrawText(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to restart the game?"));

		SizeI1 = vw_TextWidth(vw_GetText("Note: the current game data will be lost."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: the current game data will be lost."));
		} else
			vw_DrawText(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: the current game data will be lost."));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("YES"), DialogContentTransp)) {
			ComBuffer = eCommand::SWITCH_FROM_MENU_TO_GAME;
			ExitGame();
			CloseDialog();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("NO"), DialogContentTransp)) CloseDialog();
		break;




	// вывод данных по кораблю
	case eDialogBox::ShowShipInfo: {
		auto sharedDialogSpaceShip = DialogSpaceShip.lock();
		auto sharedWorkshopFighterGame = WorkshopFighterGame.lock();
		if (sharedDialogSpaceShip && sharedWorkshopFighterGame) {
			// название диалога
			SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText(GetWorkshopShipName(sharedDialogSpaceShip->InternalType))))/2;
			vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText(GetWorkshopShipName(sharedDialogSpaceShip->InternalType)));
			// текст диалога
			int Y1 = Y+80;
			int Offset = 31;
			int Size = 240;
			float WScale = -200;
			int X1 = X+45;

			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Object Class:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Space Ship"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Ship Type:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText(GetShipGroupTitle(sharedDialogSpaceShip->InternalType)));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Developer:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Earth Federation"));

			Y1 += Offset;
			if (sharedWorkshopFighterGame == sharedDialogSpaceShip) {
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Width:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%2.1f %s", sharedDialogSpaceShip->Width, vw_GetText("units"));
			} else {
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Width:"));
				if (sharedDialogSpaceShip->Width > sharedWorkshopFighterGame->Width)
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%2.1f (%2.1f) %s", sharedDialogSpaceShip->Width, sharedWorkshopFighterGame->Width, vw_GetText("units"));
				else
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%2.1f (%2.1f) %s", sharedDialogSpaceShip->Width, sharedWorkshopFighterGame->Width, vw_GetText("units"));
			}

			Y1 += Offset;
			if (sharedWorkshopFighterGame == sharedDialogSpaceShip) {
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Length:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%2.1f %s", sharedDialogSpaceShip->Length, vw_GetText("units"));
			} else {
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Length:"));
				if (sharedDialogSpaceShip->Length > sharedWorkshopFighterGame->Length)
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%2.1f (%2.1f) %s", sharedDialogSpaceShip->Length, sharedWorkshopFighterGame->Length, vw_GetText("units"));
				else
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%2.1f (%2.1f) %s", sharedDialogSpaceShip->Length, sharedWorkshopFighterGame->Length, vw_GetText("units"));
			}


			Y1 += Offset;
			if (sharedWorkshopFighterGame == sharedDialogSpaceShip) {
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Height:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%2.1f %s", sharedDialogSpaceShip->Height, vw_GetText("units"));
			} else {
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Height:"));
				if (sharedDialogSpaceShip->Height > sharedWorkshopFighterGame->Height)
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%2.1f (%2.1f) %s", sharedDialogSpaceShip->Height, sharedWorkshopFighterGame->Height, vw_GetText("units"));
				else
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%2.1f (%2.1f) %s", sharedDialogSpaceShip->Height, sharedWorkshopFighterGame->Height, vw_GetText("units"));
			}

			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Engines:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", sharedDialogSpaceShip->Engines.size(), vw_GetText("units"));

			Y1 += Offset;
			if (sharedWorkshopFighterGame == sharedDialogSpaceShip) {
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Armor:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", (int)GetShipArmor(sharedDialogSpaceShip->InternalType), vw_GetText("units"));
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk2:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*2), vw_GetText("units"));
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk3:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*3), vw_GetText("units"));
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk4:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*4), vw_GetText("units"));

			} else {
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Armor:"));
				if (GetShipArmor(sharedDialogSpaceShip->InternalType) < GetShipArmor(sharedWorkshopFighterGame->InternalType)) {
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%i (%i) %s", (int)GetShipArmor(sharedDialogSpaceShip->InternalType), (int)GetShipArmor(sharedDialogSpaceShip->InternalType), vw_GetText("units"));
					Y1 += Offset;
					vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk2:"));
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*2), (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*2), vw_GetText("units"));
					Y1 += Offset;
					vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk3:"));
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*3), (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*3), vw_GetText("units"));
					Y1 += Offset;
					vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk4:"));
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*4), (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*4), vw_GetText("units"));
				} else {
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%i (%i) %s", (int)GetShipArmor(sharedDialogSpaceShip->InternalType), (int)GetShipArmor(sharedDialogSpaceShip->InternalType), vw_GetText("units"));
					Y1 += Offset;
					vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk2:"));
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*2), (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*2), vw_GetText("units"));
					Y1 += Offset;
					vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk3:"));
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*3), (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*3), vw_GetText("units"));
					Y1 += Offset;
					vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Upgrade Mk4:"));
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%i (%i) %s", (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*4), (int)(GetShipArmor(sharedDialogSpaceShip->InternalType)*4), vw_GetText("units"));
				}
			}


			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Weapon Slots:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", sharedDialogSpaceShip->WeaponSlots.size(), vw_GetText("units"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Slot Levels:"));
			int SSS = 0;
			if (sharedDialogSpaceShip->WeaponSlots.size()>0) {
				vw_DrawText(X1+Size+SSS, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i", sharedDialogSpaceShip->WeaponSlots[0].Type);
				SSS += vw_TextWidth("%i", sharedDialogSpaceShip->WeaponSlots[0].Type);
			}
			for (unsigned i=1; i<sharedDialogSpaceShip->WeaponSlots.size(); i++) {
				vw_DrawText(X1+Size+SSS, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "/%i", sharedDialogSpaceShip->WeaponSlots[i].Type);
				SSS += vw_TextWidth("/%i", sharedDialogSpaceShip->WeaponSlots[i].Type);
			}

			// закрываем...
			if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true)) CloseDialog();
		}
	}
	break;

	// вывод данных по оружию
	case eDialogBox::ShowWeaponsInfo: {
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText(GetWeaponName(DialogWeapon->InternalType))))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText(GetWeaponName(DialogWeapon->InternalType)));
		// текст диалога
		int Y1 = Y+80;
		int Offset = 31;
		int Size = 230;
		float WScale = -200;
		int X1 = X+45;

		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Object Class:"));
		if (DialogWeapon->InternalType<16)
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Weapon, Cannon"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Weapon, Launcher"));

		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Type:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText(GetWeaponGroupTitle(DialogWeapon->InternalType)));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Developer:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Earth Federation"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Armor:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", (int)DialogWeapon->StrengthStart, vw_GetText("units"));

		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Width:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp,  "%2.1f %s", DialogWeapon->Width, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Length:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%2.1f %s", DialogWeapon->Length, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Height:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%2.1f %s", DialogWeapon->Height, vw_GetText("units"));

		Y1 += Offset;
		vw_DrawText(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Damage, Hull:"));

		if ((DialogWeapon->InternalType == 11) |
		    (DialogWeapon->InternalType == 12) |
		    (DialogWeapon->InternalType == 14))
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", GetProjectileDamageKinetic(DialogWeapon->InternalType), vw_GetText("units/sec"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", GetProjectileDamageKinetic(DialogWeapon->InternalType), vw_GetText("units/shot"));

		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Damage, Systems:"));
		if ((DialogWeapon->InternalType == 11) |
		    (DialogWeapon->InternalType == 12) |
		    (DialogWeapon->InternalType == 14))
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", GetProjectileDamageEM(DialogWeapon->InternalType), vw_GetText("units/sec"));
		else
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", GetProjectileDamageEM(DialogWeapon->InternalType), vw_GetText("units/shot"));

		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Energy Use:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", DialogWeapon->EnergyUse, vw_GetText("units per shot"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Ammo:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", DialogWeapon->AmmoStart, vw_GetText("units"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Reload:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", DialogWeapon->NextFireTime, vw_GetText("seconds"));
		Y1 += Offset;
		vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Range:"));
		vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetProjectileRange(DialogWeapon->InternalType), vw_GetText("units"));

		// закрываем...
		if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true)) CloseDialog();
	}
	break;

	// вывод данных по системам
	case eDialogBox::ShowSystemsInfo: {
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText(GetSystemName(DialogSystem))))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText(GetSystemName(DialogSystem)));
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

			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Object Class:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("System"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Type:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Spaceship Engine"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Developer:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Earth Federation"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Energy Use:"));
			if (NeedMoreEnergyDialog && (GameConfig().Profile[CurrentProfile].SpaceShipControlMode != 1))
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipEngineSystemEnergyUse(DialogSystem), vw_GetText("units per sec"));
			else {
				if (GameConfig().Profile[CurrentProfile].EngineSystem == DialogSystem) {
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetShipEngineSystemEnergyUse(DialogSystem), vw_GetText("units per sec"));
				} else if (GameConfig().Profile[CurrentProfile].EngineSystem == 0)
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetShipEngineSystemEnergyUse(DialogSystem), vw_GetText("units per sec"));
				else {
					if (GetShipEngineSystemEnergyUse(DialogSystem) < GetShipEngineSystemEnergyUse(GameConfig().Profile[CurrentProfile].EngineSystem))
						vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipEngineSystemEnergyUse(DialogSystem), GetShipEngineSystemEnergyUse(GameConfig().Profile[CurrentProfile].EngineSystem), vw_GetText("units per sec"));
					else
						vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipEngineSystemEnergyUse(DialogSystem), GetShipEngineSystemEnergyUse(GameConfig().Profile[CurrentProfile].EngineSystem), vw_GetText("units per sec"));
				}
			}

			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Engine Power:"));
			if (GameConfig().Profile[CurrentProfile].EngineSystem == DialogSystem) {
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetEnginePower(DialogSystem), vw_GetText("units"));
			} else if (GameConfig().Profile[CurrentProfile].EngineSystem == 0)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetEnginePower(DialogSystem), vw_GetText("units"));
			else {
				if (GetEnginePower(DialogSystem) > GetEnginePower(GameConfig().Profile[CurrentProfile].EngineSystem))
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%3.1f (%3.1f) %s", GetEnginePower(DialogSystem), GetEnginePower(GameConfig().Profile[CurrentProfile].EngineSystem), vw_GetText("units"));
				else
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%3.1f (%3.1f) %s", GetEnginePower(DialogSystem), GetEnginePower(GameConfig().Profile[CurrentProfile].EngineSystem), vw_GetText("units"));
			}

			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Acceleration:"));
			if (GameConfig().Profile[CurrentProfile].EngineSystem == DialogSystem) {
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetEngineAcceleration(DialogSystem), vw_GetText("units"));
			} else if (GameConfig().Profile[CurrentProfile].EngineSystem == 0)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetEngineAcceleration(DialogSystem), vw_GetText("units"));
			else {
				if (GetEngineAcceleration(DialogSystem) > GetEngineAcceleration(GameConfig().Profile[CurrentProfile].EngineSystem))
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%3.1f (%3.1f) %s", GetEngineAcceleration(DialogSystem), GetEngineAcceleration(GameConfig().Profile[CurrentProfile].EngineSystem), vw_GetText("units"));
				else
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%3.1f (%3.1f) %s", GetEngineAcceleration(DialogSystem), GetEngineAcceleration(GameConfig().Profile[CurrentProfile].EngineSystem), vw_GetText("units"));
			}

			Y1 += Offset;
			vw_DrawText(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Relations/Dependencies:"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetText("Power Source"));

			if (GameConfig().Profile[CurrentProfile].SpaceShipControlMode == 1) {
				Y1 += Offset;
				Y1 += Offset;
				Y1 += Offset;
				SizeI = (W-vw_TextWidth(vw_GetText("Useless with Arcade")))/2-45;
				vw_DrawText(X1+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, DialogContentTransp, vw_GetText("Useless with Arcade"));
				Y1 += Offset;
				SizeI = (W-vw_TextWidth(vw_GetText("SpaceShip Control Mode.")))/2-45;
				vw_DrawText(X1+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, DialogContentTransp, vw_GetText("SpaceShip Control Mode."));
			}

			break;

		case 5:
		case 6:
		case 7:
		case 8:

			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Object Class:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("System"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Type:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Power Source"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Developer:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Earth Federation"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Energy Capacity:"));

			if (GameConfig().Profile[CurrentProfile].PowerSystem == DialogSystem-4) {
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetShipMaxEnergy(DialogSystem-4), vw_GetText("units"));
			} else if (GameConfig().Profile[CurrentProfile].PowerSystem == 0)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetShipMaxEnergy(DialogSystem-4), vw_GetText("units"));
			else {
				if (GetShipMaxEnergy(DialogSystem-4) > GetShipMaxEnergy(GameConfig().Profile[CurrentProfile].PowerSystem))
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipMaxEnergy(DialogSystem-4), GetShipMaxEnergy(GameConfig().Profile[CurrentProfile].PowerSystem), vw_GetText("units"));
				else
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipMaxEnergy(DialogSystem-4), GetShipMaxEnergy(GameConfig().Profile[CurrentProfile].PowerSystem), vw_GetText("units"));
			}

			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Recharge Rate:"));
			if (NeedMoreEnergyDialog)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipRechargeEnergy(DialogSystem-4), vw_GetText("units per sec"));
			else {
				if (GameConfig().Profile[CurrentProfile].PowerSystem == (DialogSystem - 4)) {
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetShipRechargeEnergy(DialogSystem-4), vw_GetText("units per sec"));
				} else if (GameConfig().Profile[CurrentProfile].PowerSystem == 0)
					vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetShipRechargeEnergy(DialogSystem-4), vw_GetText("units per sec"));
				else {
					if (GetShipRechargeEnergy(DialogSystem-4) > GetShipRechargeEnergy(GameConfig().Profile[CurrentProfile].PowerSystem))
						vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipRechargeEnergy(DialogSystem-4), GetShipRechargeEnergy(GameConfig().Profile[CurrentProfile].PowerSystem), vw_GetText("units per sec"));
					else
						vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%3.1f (%3.1f) %s", GetShipRechargeEnergy(DialogSystem-4), GetShipRechargeEnergy(GameConfig().Profile[CurrentProfile].PowerSystem), vw_GetText("units per sec"));
				}
			}

			Y1 += Offset;
			vw_DrawText(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Relations/Dependencies:"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Weapon Recharge Rate"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Spaceship Engine"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Optical Computer"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Targeting System"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Advanced System"));

			break;

		case 9:
		case 10:
		case 11:
		case 12:

			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Object Class:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("System"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Type:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Optical Computer"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Developer:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Earth Federation"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Energy Use:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", 0.1f, vw_GetText("units per sec"));

			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Target Tracing:"));
			if (DialogSystem == 9)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", 1, vw_GetText("object(s)"));
			if (DialogSystem == 10)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", 1, vw_GetText("object(s)"));
			if (DialogSystem == 11)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", 6, vw_GetText("object(s)"));
			if (DialogSystem == 12)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i %s", 6, vw_GetText("object(s)"));

			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Auto Adjustment:"));
			if (DialogSystem == 9)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("No"));
			if (DialogSystem == 10)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Yes"));
			if (DialogSystem == 11)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("No"));
			if (DialogSystem == 12)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Yes"));

			Y1 += Offset;
			vw_DrawText(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Relations/Dependencies:"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetText("Power Source"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetText("Weapon Fire Range"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Weapon Orientation"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Targeting System"));

			break;

		case 13:
		case 14:
		case 15:
		case 16:

			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Object Class:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("System"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Type:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Targeting System"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Developer:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Earth Federation"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Energy Use:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", 0.1f, vw_GetText("units per sec"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Orientation Speed:"));
			if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem == (DialogSystem - 12)) {
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", 40.0f*(DialogSystem-12), vw_GetText("deg per sec"));
			} else if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem == 0)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", 40.0f*(DialogSystem-12), vw_GetText("deg per sec"));
			else {
				if (DialogSystem-12 > GameConfig().Profile[CurrentProfile].TargetingMechanicSystem)
					vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, "%3.1f (%3.1f) %s", 40.0f * (DialogSystem - 12), 40.0f * GameConfig().Profile[CurrentProfile].TargetingMechanicSystem, vw_GetText("deg per sec"));
				else
					vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, "%3.1f (%3.1f) %s", 40.0f * (DialogSystem - 12), 40.0f * GameConfig().Profile[CurrentProfile].TargetingMechanicSystem, vw_GetText("deg per sec"));
			}

			Y1 += Offset;
			vw_DrawText(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Relations/Dependencies:"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetText("Power Source"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetText("Optical Computer"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Weapon Orientation"));

			if (GameConfig().Profile[CurrentProfile].WeaponTargetingMode == 1) {
				Y1 += Offset;
				Y1 += Offset;
				Y1 += Offset;
				SizeI = (W-vw_TextWidth(vw_GetText("Useless with Arcade")))/2-45;
				vw_DrawText(X1+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, DialogContentTransp, vw_GetText("Useless with Arcade"));
				Y1 += Offset;
				SizeI = (W-vw_TextWidth(vw_GetText("Weapon Targeting Mode.")))/2-45;
				vw_DrawText(X1+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, DialogContentTransp, vw_GetText("Weapon Targeting Mode."));
			}

			break;

		case 17:
		case 18:
		case 19:
		case 20:

			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Object Class:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("System"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Type:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Advanced System"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Developer:"));
			vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Earth Federation"));
			Y1 += Offset;
			vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Energy Use:"));
			if (NeedMoreEnergyDialog)
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3*DialogContentTransp, "%3.1f %s", GetShipProtectionSystemEnergyUse(DialogSystem-16), vw_GetText("units per sec"));
			else
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", GetShipProtectionSystemEnergyUse(DialogSystem-16), vw_GetText("units per sec"));

			if (DialogSystem == 17) {
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Hull Repair Rate:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%0.1f%% %s", 0.5f, vw_GetText("per sec"));
			}
			if (DialogSystem == 18) {
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Resistance, Hull:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "x4");
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Resistance, System:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "x5");
			}
			if (DialogSystem == 19) {
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Shield Power:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", 100.0f, vw_GetText("units"));
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Recharge Rate:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i%% %s", 2, vw_GetText("per sec"));
			}
			if (DialogSystem == 20) {
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Shield Power:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%3.1f %s", 150.0f, vw_GetText("units"));
				Y1 += Offset;
				vw_DrawText(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Recharge Rate:"));
				vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "%i%% %s", 3, vw_GetText("per sec"));
			}

			Y1 += Offset;
			vw_DrawText(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Relations/Dependencies:"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetText("Power Source"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Hull Resistance"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("Systems Resistance"));
			Y1 += Offset;
			vw_DrawText(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetText("SpaceShip Vitality"));

			break;
		}


		// закрываем...
		if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true))
			CloseDialog();
	}
	break;


	case eDialogBox::ProfileTipsAndTricks: { // подсказки на меню профилей
		// название диалога
		SizeI1 = vw_TextWidth(vw_GetText("Tips and hints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("Tips and hints"));
		// текст диалога

		SizeI = vw_TextWidth(vw_GetText("It is possible to create five Pilot Profiles in the game in"));
		vw_DrawText(X+25, Y+ 80, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("It is possible to create five Pilot Profiles in the game in"));
		SizeI = vw_TextWidth(vw_GetText("order to store personal data. You can set an individual"));
		vw_DrawText(X+25, Y+115, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("order to store personal data. You can set an individual"));
		SizeI = vw_TextWidth(vw_GetText("difficulty level for each profile or delete the current"));
		vw_DrawText(X+25, Y+150, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("difficulty level for each profile or delete the current"));
		vw_DrawText(X+25, Y+185, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("profile to free space for a new profile."));

		SizeI = vw_TextWidth(vw_GetText("Note that the current difficulty level is displayed in percent"));
		vw_DrawText(X+25, Y+240, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Note that the current difficulty level is displayed in percent"));
		SizeI = vw_TextWidth(vw_GetText("for each profile. This value influences the increase of"));
		vw_DrawText(X+25, Y+275, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("for each profile. This value influences the increase of"));
		SizeI = vw_TextWidth(vw_GetText("experience. For instance, if you bring down an enemy ship, you"));
		vw_DrawText(X+25, Y+310, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("experience. For instance, if you bring down an enemy ship, you"));
		SizeI = vw_TextWidth(vw_GetText("will get 100 money units and only 65 experience units if the"));
		vw_DrawText(X+25, Y+345, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("will get 100 money units and only 65 experience units if the"));
		vw_DrawText(X+25, Y+380, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("difficulty percent is set to 65 for you."));

		SizeI = vw_TextWidth(vw_GetText("Note: you can change your difficulty level before each"));
		vw_DrawText(X+25, Y+435, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: you can change your difficulty level before each"));
		vw_DrawText(X+25, Y+470, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("new mission."));

		// чекбокс
		bool ttt = !GameConfig().NeedShowHint[0];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("Do not show this tip again."), DialogContentTransp);
		ChangeGameConfig().NeedShowHint[0] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("CLOSE"), DialogContentTransp))
			CloseDialog();
		break;
	}
	case eDialogBox::ShipyardTipsAndTricks: { // подсказки на меню шипъярд
		// название диалога
		SizeI1 = vw_TextWidth(vw_GetText("Tips and hints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("Tips and hints"));
		// текст диалога

		int k=28;
		SizeI = vw_TextWidth(vw_GetText("There are more than 20 spaceships with unique characteristics"));
		vw_DrawText(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("There are more than 20 spaceships with unique characteristics"));
		SizeI = vw_TextWidth(vw_GetText("available in the game. You can use the right part of the screen"));
		vw_DrawText(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("available in the game. You can use the right part of the screen"));
		SizeI = vw_TextWidth(vw_GetText("to see your current spaceship, its characteristics and cost and"));
		vw_DrawText(X+25, Y+80+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("to see your current spaceship, its characteristics and cost and"));
		vw_DrawText(X+25, Y+80+k*3, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("also to perform basic operations, such as repair."));

		SizeI = vw_TextWidth(vw_GetText("The left part of the screen contains Ship Stock where you can see"));
		vw_DrawText(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("The left part of the screen contains Ship Stock where you can see"));
		SizeI = vw_TextWidth(vw_GetText("all available spaceships and their characteristics. Do not"));
		vw_DrawText(X+25, Y+100+k*5, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("all available spaceships and their characteristics. Do not"));
		SizeI = vw_TextWidth(vw_GetText("forget that each ship has its dimensions and, which is more"));
		vw_DrawText(X+25, Y+100+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("forget that each ship has its dimensions and, which is more"));
		vw_DrawText(X+25, Y+100+k*7, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("important, different weapon mounts."));

		vw_DrawText(X+25, Y+120+k*8, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("The basic shipbuilding concept is as follows:"));
		vw_DrawText(X+25, Y+120+k*9, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Scout - light, small and maneuverable ships with weak weapons."));
		vw_DrawText(X+25, Y+120+k*10, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Interceptor - fast ships with light weapons."));
		vw_DrawText(X+25, Y+120+k*11, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Fighter - main type of ships with light and semi-heavy weapons."));
		vw_DrawText(X+25, Y+120+k*12, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Bomber - heavy destroyers with mostly heavy attack weapons."));
		vw_DrawText(X+25, Y+120+k*13, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Attack Ship - heavy, large and heavily armed ships."));

		// чекбокс
		bool ttt = !GameConfig().NeedShowHint[1];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("Do not show this tip again."), DialogContentTransp);
		ChangeGameConfig().NeedShowHint[1] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("CLOSE"), DialogContentTransp))
			CloseDialog();
		break;
	}
	case eDialogBox::SystemsTipsAndTricks: { // подсказки на меню системы
		// название диалога
		SizeI1 = vw_TextWidth(vw_GetText("Tips and hints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("Tips and hints"));
		// текст диалога

		int k=25;
		SizeI = vw_TextWidth(vw_GetText("A set of systems is installed on the ship you operate. All systems"));
		vw_DrawText(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("A set of systems is installed on the ship you operate. All systems"));
		SizeI = vw_TextWidth(vw_GetText("are connected with each other and with ship weapons in one way or"));
		vw_DrawText(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("are connected with each other and with ship weapons in one way or"));
		vw_DrawText(X+25, Y+80+k*2, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("another and play important role in the game."));

		SizeI = vw_TextWidth(vw_GetText("You should carefully take into account the peculiarities of each"));
		vw_DrawText(X+25, Y+100+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("You should carefully take into account the peculiarities of each"));
		SizeI = vw_TextWidth(vw_GetText("system and match their characteristics to each other to get"));
		vw_DrawText(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("system and match their characteristics to each other to get"));
		vw_DrawText(X+25, Y+100+k*5, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("a well balanced warship that can complete combat tasks."));

		SizeI = vw_TextWidth(vw_GetText("Engine - ship engine system. Pay attention to how this system consumes"));
		vw_DrawText(X+25, Y+120+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Engine - ship engine system. Pay attention to how this system consumes"));
		vw_DrawText(X+40, Y+120+k*7, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("energy. Otherwise you run the risk of being unable to move."));
		SizeI = vw_TextWidth(vw_GetText("energy. Otherwise you run the risk of being unable to move."));
		vw_DrawText(X+25, Y+120+k*8, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Power - ship energy source. Pay attention to the main two characte-"));
		vw_DrawText(X+40, Y+120+k*9, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("ristics of this system - Recharge Rate and Capacity."));
		SizeI = vw_TextWidth(vw_GetText("O.T.C. - Optical Target Computer. As you can see from its name,"));
		vw_DrawText(X+25, Y+120+k*10, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("O.T.C. - Optical Target Computer. As you can see from its name,"));
		vw_DrawText(X+40, Y+120+k*11, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("this system is responsible for optical targeting."));
		SizeI = vw_TextWidth(vw_GetText("Targeting - Weapon targeting system. It depends on this system how"));
		vw_DrawText(X+25, Y+120+k*12, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Targeting - Weapon targeting system. It depends on this system how"));
		vw_DrawText(X+40, Y+120+k*13, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("fast a weapon will aim and how accurate the shot will be."));
		SizeI = vw_TextWidth(vw_GetText("Advanced - The most advanced, but yet untested systems for"));
		vw_DrawText(X+25, Y+120+k*14, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Advanced - The most advanced, but yet untested systems for"));
		vw_DrawText(X+40, Y+120+k*15, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("enhancing ship invulnerability."));

		// чекбокс
		bool ttt = !GameConfig().NeedShowHint[2];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("Do not show this tip again."), DialogContentTransp);
		ChangeGameConfig().NeedShowHint[2] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("CLOSE"), DialogContentTransp))
			CloseDialog();
		break;
	}
	case eDialogBox::WeaponryTipsAndTricks: { // подсказки на меню оружейная
		// название диалога
		SizeI1 = vw_TextWidth(vw_GetText("Tips and hints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("Tips and hints"));
		// текст диалога

		int k=25;
		SizeI = vw_TextWidth(vw_GetText("You can use this section to buy, sell and set up the weapons on"));
		vw_DrawText(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("You can use this section to buy, sell and set up the weapons on"));
		SizeI = vw_TextWidth(vw_GetText("your ship. To buy a weapon, just drag its icon from Weapon Stock"));
		vw_DrawText(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("your ship. To buy a weapon, just drag its icon from Weapon Stock"));
		SizeI = vw_TextWidth(vw_GetText("to a weapon slot on your ship. Note that each weapon has its"));
		vw_DrawText(X+25, Y+80+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("to a weapon slot on your ship. Note that each weapon has its"));
		SizeI = vw_TextWidth(vw_GetText("own level (from 1 to 5), you should see that the slot level is"));
		vw_DrawText(X+25, Y+80+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("own level (from 1 to 5), you should see that the slot level is"));
		SizeI = vw_TextWidth(vw_GetText("not less than the weapon level. To sell a weapon, perform"));
		vw_DrawText(X+25, Y+80+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("not less than the weapon level. To sell a weapon, perform"));
		vw_DrawText(X+25, Y+80+k*5, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("the reverse operation."));

		SizeI = vw_TextWidth(vw_GetText("The weapon is set up and recharged with the Setup button"));
		vw_DrawText(X+25, Y+100+k*6, SizeI > 716 ? -716 : 716, 0,1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("The weapon is set up and recharged with the Setup button"));
		vw_DrawText(X+25, Y+100+k*7, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("in the right part of the weapon slot panel."));

		vw_DrawText(X+25, Y+120+k*8, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Main types of weapons by their projectiles:"));
		SizeI = vw_TextWidth(vw_GetText("Projectile - small projectiles based on bullets or energy beams."));
		vw_DrawText(X+25, Y+120+k*9, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Projectile - small projectiles based on bullets or energy beams."));
		vw_DrawText(X+40, Y+120+k*10, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("It is the main and most widespread weapon in the game."));
		SizeI = vw_TextWidth(vw_GetText("Self-propelled projectile - missiles, torpedoes and nuclear bombs."));
		vw_DrawText(X+25, Y+120+k*11, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Self-propelled projectile - missiles, torpedoes and nuclear bombs."));
		SizeI = vw_TextWidth(vw_GetText("The disadvantages of these weapons include the low speed"));
		vw_DrawText(X+40, Y+120+k*12, SizeI > 701 ? -701 : 701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("The disadvantages of these weapons include the low speed"));
		vw_DrawText(X+40, Y+120+k*13, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("of the projectile and the possibility of bringing it down."));
		SizeI = vw_TextWidth(vw_GetText("Beam - Lasers and Masers. Short-range highly energy-"));
		vw_DrawText(X+25, Y+120+k*14, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Beam - Lasers and Masers. Short-range highly energy-"));
		vw_DrawText(X+40, Y+120+k*15, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("consuming weapons."));



		// чекбокс
		bool ttt = !GameConfig().NeedShowHint[3];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("Do not show this tip again."), DialogContentTransp);
		ChangeGameConfig().NeedShowHint[3] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("CLOSE"), DialogContentTransp))
			CloseDialog();
		break;
	}

	case eDialogBox::ShortkeyTipsAndTricks: { // подсказки на горячие клавиши в игре
		// название диалога
		SizeI1 = vw_TextWidth(vw_GetText("Tips and hints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("Tips and hints"));
		// текст диалога

		int k=23;

		SizeI = (W-vw_TextWidth(vw_GetText("GAME KEYBOARD SHORTCUTS")))/2;
		vw_DrawText(X+SizeI, Y+80+k*0, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("GAME KEYBOARD SHORTCUTS"));

		vw_DrawText(X+25, Y+100+k*1, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("F2 - toggle show/hide game FPS counter."));

		vw_DrawText(X+25, Y+120+k*2, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("F5 - decrease game speed."));
		vw_DrawText(X+25, Y+120+k*3, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("F6 - reset game speed to normal."));
		vw_DrawText(X+25, Y+120+k*4, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("F7 - increase game speed."));
		vw_DrawText(X+25, Y+120+k*5, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: do not increase game speed, if you have low FPS."));

		vw_DrawText(X+25, Y+140+k*6, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("F8 - change weapon panels view."));
		vw_DrawText(X+25, Y+140+k*7, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("F9 - change Primary weapon group fire mode."));
		vw_DrawText(X+25, Y+140+k*8, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("F10 - change Secondary weapon group fire mode."));

		vw_DrawText(X+25, Y+160+k*9, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("F12 - capture screenshot. (File will be saved on your Desktop.)"));

		vw_DrawText(X+25, Y+180+k*10, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Esc - show/hide game menu. (Pause.)"));
		vw_DrawText(X+25, Y+180+k*11, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: all changes will be saved in the game configuration file."));



		// чекбокс
		bool ttt = !GameConfig().NeedShowHint[4];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("Do not show this tip again."), DialogContentTransp);
		ChangeGameConfig().NeedShowHint[4] = !ttt;
		// кнопки
		if (DrawDialogButton128(X+512+90, Y+ButtonOffset, vw_GetText("START"), DialogContentTransp)) {
			// ничего не тянем... только включили меню
			DragWeaponNum = 0;
			DragWeaponLevel = 0;
			DragWeapon = false;
			WeaponSetupSlot = -1;
			//
			LastMenuOnOffUpdateTime = vw_GetTimeThread(0);
			NeedOffMenu = true;
			CloseDialog();
		}
		break;
	}

	case eDialogBox::StartMissionSecondTime: { // подсказка, если пытаемся по второму разу пройти миссию
		// название диалога
		SizeI1 = vw_TextWidth(vw_GetText("Tips and hints"));
		SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("Tips and hints"));
		// текст диалога

		int k=30;

		SizeI = vw_TextWidth(vw_GetText("You have already completed this mission. Your previous best"));
		vw_DrawText(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("You have already completed this mission. Your previous best"));
		vw_DrawText(X+25, Y+80+k*1, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("result is %i points of experience."), GameConfig().Profile[CurrentProfile].ByMissionExperience[CurrentMission]);

		SizeI = vw_TextWidth(vw_GetText("You can replay this mission as many times as you like, but you"));
		vw_DrawText(X+25, Y+100+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("You can replay this mission as many times as you like, but you"));
		SizeI = vw_TextWidth(vw_GetText("should take into account that only the best result you"));
		vw_DrawText(X+25, Y+100+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("should take into account that only the best result you"));
		SizeI = vw_TextWidth(vw_GetText("number of experience points."));
		vw_DrawText(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("achieve in this mission will be used to calculate the total"));
		vw_DrawText(X+25, Y+100+k*5, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("number of experience points."));

		SizeI = vw_TextWidth(vw_GetText("Note that each time you complete the mission again,"));
		vw_DrawText(X+25, Y+120+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("Note that each time you complete the mission again,"));
		SizeI = vw_TextWidth(vw_GetText("the amount of money you get when you destroy the enemy"));
		vw_DrawText(X+25, Y+120+k*7, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("the amount of money you get when you destroy the enemy"));
		vw_DrawText(X+25, Y+120+k*8, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("in this mission is reduced two times."));


		// чекбокс
		bool ttt = !GameConfig().NeedShowHint[5];
		DrawCheckBox_2(X+36, Y+ButtonOffset, &ttt, vw_GetText("Do not show this tip again."), DialogContentTransp);
		ChangeGameConfig().NeedShowHint[5] = !ttt;
		// кнопки
		if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetText("CLOSE"), DialogContentTransp)) {
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
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("Language")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("Language"));


		if (DrawDialogButton128(X+34, Y+ButtonOffset-73, "<<<", DialogContentTransp)) {
			if (GameConfig().MenuLanguage == 0)
				ChangeGameConfig().MenuLanguage = vw_GetLanguageListCount() - 1;
			else
				ChangeGameConfig().MenuLanguage--;

			vw_SetTextLanguage(GameConfig().MenuLanguage);
			// forced to regenerate textures (base texture connected to language-related characters set)
			vw_ReleaseAllFontChars();
			GenerateFonts();
		}
		if (DrawDialogButton128(X+316+34, Y+ButtonOffset-73, ">>>", DialogContentTransp)) {
			if (GameConfig().MenuLanguage >= (vw_GetLanguageListCount() - 1))
				ChangeGameConfig().MenuLanguage = 0;
			else
				ChangeGameConfig().MenuLanguage++;

			vw_SetTextLanguage(GameConfig().MenuLanguage);
			// forced to regenerate textures (base texture connected to language-related characters set)
			vw_ReleaseAllFontChars();
			GenerateFonts();
		}

		vw_SetFontSize(24);
		int Size;
		Size = vw_TextWidth(vw_GetText("English", GameConfig().MenuLanguage));
		SizeI = (170-Size)/2;
		if (Size > 170)
			vw_DrawText(X+138+34, Y+ButtonOffset-71, -170, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("English", GameConfig().MenuLanguage));
		else
			vw_DrawText(X+138+34+SizeI, Y+ButtonOffset-71, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetText("English", GameConfig().MenuLanguage));
		ResetFontSize();

		if (DrawDialogButton200(X+128+64-72/2, Y+ButtonOffset, vw_GetText("OK"), DialogContentTransp)) {
			// первоначально, язык голоса ставим такой же, как и язык меню
			ChangeGameConfig().VoiceLanguage = GameConfig().MenuLanguage;
			CloseDialog();
		}
		break;
	}

	case eDialogBox::RestartOnAdvOptChanged: // при изменении продвинутых настроек в самой игре, с предупреждением, что не все сохраним
		// название диалога
		SizeI = 17 + (WTitle-vw_TextWidth(vw_GetText("RESTART")))/2;
		vw_DrawText(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetText("RESTART"));
		// текст диалога
		SizeI1 = vw_TextWidth(vw_GetText("Are you sure you want to restart the game?"));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to restart the game?"));
		} else
			vw_DrawText(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetText("Are you sure you want to restart the game?"));

		SizeI1 = vw_TextWidth(vw_GetText("Note: the current game data will be lost."));
		SizeI = (W-SizeI1)/2;
		if (SizeI1 > 470) {
			SizeI = (W - 470)/2;
			vw_DrawText(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: the current game data will be lost."));
		} else
			vw_DrawText(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetText("Note: the current game data will be lost."));

		// кнопки
		if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetText("YES"), DialogContentTransp)) {
			CloseDialog();
			RecreateGameWindow();
			SaveOptionsAdvMenuTmpData();
		}
		if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetText("NO"), DialogContentTransp)) CloseDialog();
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

} // astromenace namespace
} // viewizard namespace
