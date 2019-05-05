/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

// FIXME ostringstream is not so fast, move all string initialization into setup,
//       all ostringstream-related code should be called only one time in dialog init

// TODO translate comments

#include "../core/core.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../ui/cursor.h"
#include "../assets/audio.h"
#include "../assets/texture.h"
#include "../main.h"
#include "../object3d/space_ship/space_ship.h"
#include "../object3d/projectile/projectile.h"
#include "../command.h"
#include "../game.h" // FIXME "game.h" should be replaced by individual headers
#include <sstream>
#include <iomanip>

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
static bool DrawDialogButton200(int X, int Y, const std::u32string &Text, float Transp)
{
    sRECT SrcRect, DstRect;
    bool ON = false;


    // работаем с клавиатурой
    if (Transp >= 0.99f) {
        CurrentActiveMenuElement++;
    }
    bool InFocusByKeyboard = false;
    if (CurrentKeyboardSelectMenuElement > 0
        && CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
        InFocusByKeyboard = true;
    }


    DstRect(X,Y,X+204,Y+35);
    if (vw_MouseOverRect(DstRect) || InFocusByKeyboard) {
        // если тухнем или появляемся - не жать
        ON = true;
        if (Transp == 1.0f) {
            SetCursorStatus(eCursorStatus::ActionAllowed);
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
    if (!ON) {
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/button_dialog200_out.tga"), true, 0.8f*Transp);
    } else {
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/button_dialog200_in.tga"), true, 0.8f*Transp);
    }


    // получаем длину текста
    int Size = vw_TextWidthUTF32(Text);
    // если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
    float WScale = 0;
    if (Size > 176) {
        Size = 176;
        WScale = -176;
    }
    // находим смещение текста
    int SizeI = DstRect.left + (SrcRect.right-SrcRect.left-Size)/2;
    // рисуем текст
    if (!ON) {
        vw_DrawTextUTF32(SizeI, Y+6, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, (0.7f*Transp)/2.0f, Text);
    } else {
        vw_DrawTextUTF32(SizeI, Y+6, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, Transp, Text);
    }

    if (GetCursorStatus() == eCursorStatus::ActionAllowed
        && (vw_GetMouseLeftClick(true)
            || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN))))) {
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
static bool DrawDialogButton128(int X, int Y, const std::u32string &Text, float Transp)
{
    sRECT SrcRect, DstRect;
    bool ON = false;

    // работаем с клавиатурой
    if (Transp >= 0.99f) {
        CurrentActiveMenuElement++;
    }
    bool InFocusByKeyboard = false;
    if (CurrentKeyboardSelectMenuElement > 0
        && CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
        InFocusByKeyboard = true;
    }


    DstRect(X,Y,X+132,Y+35);
    if (vw_MouseOverRect(DstRect) || InFocusByKeyboard) {
        // если тухнем или появляемся - не жать
        ON = true;
        if (Transp == 1.0f)
            SetCursorStatus(eCursorStatus::ActionAllowed);

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
    if (!ON) {
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/button_dialog128_out.tga"), true, 0.8f*Transp);
    } else {
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/button_dialog128_in.tga"), true, 0.8f*Transp);
    }


    // получаем длину текста
    int Size = vw_TextWidthUTF32(Text);
    // если текст сильно большой - сжимаем буквы, чтобы не вылазило за пределы кнопки
    float WScale = 0;
    if (Size > 108) {
        Size = 108;
        WScale = -108;
    }
    // находим смещение текста
    int SizeI = DstRect.left + (SrcRect.right-SrcRect.left-Size)/2;
    // рисуем текст
    if (!ON) {
        vw_DrawTextUTF32(SizeI, Y+6, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, (0.7f*Transp)/2.0f, Text);
    } else {
        vw_DrawTextUTF32(SizeI, Y+6, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, Transp, Text);
    }

    if (GetCursorStatus() == eCursorStatus::ActionAllowed
        && (vw_GetMouseLeftClick(true)
            || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN))))) {
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
static void DrawCheckBox_2(int X, int Y, bool &CheckBoxStatus, const std::u32string &Text, float Transp)
{
    sRECT SrcRect, DstRect;

    // получаем длину текста
    int Size = vw_TextWidthUTF32(Text);

    bool ON = false;


    // работаем с клавиатурой
    if (Transp >= 0.99f && GetShowGameCursor()) {
        CurrentActiveMenuElement++;
    }

    bool InFocusByKeyboard = false;
    if (CurrentKeyboardSelectMenuElement > 0
        && CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
        InFocusByKeyboard = true;
    }


    // 20 - расстояние между текстом
    DstRect(X+4,Y+4,X+40+20+Size,Y+40-4);
    if ((vw_MouseOverRect(DstRect) || InFocusByKeyboard) && GetShowGameCursor()) {
        // если тухнем или появляемся - не жать
        ON = true;
        if (Transp == 1.0f) {
            SetCursorStatus(eCursorStatus::ActionAllowed);
        }
    }



    // рисуем
    SrcRect(0,0,40,38);
    DstRect(X,Y,X+40,Y+38);
    if (!ON || DragWeapon) {
        vw_DrawTextUTF32(X+40+16, Y+8, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, Transp, Text);
    } else {
        vw_DrawTextUTF32(X+40+16, Y+8, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, Transp, Text);
    }

    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/checkbox_main.tga"), true, Transp);
    if (CheckBoxStatus) {
        vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/checkbox_in.tga"), true, Transp);
    }


    if (GetCursorStatus() == eCursorStatus::ActionAllowed && !DragWeapon
        && (vw_GetMouseLeftClick(true)
            || (InFocusByKeyboard && (vw_GetKeyStatus(SDLK_KP_ENTER) || vw_GetKeyStatus(SDLK_RETURN))))) {
        CheckBoxStatus = !CheckBoxStatus;
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
    if (isDialogBoxDrawing()) {
        return;
    }

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

        if (DialogContentTransp <= 0.0f) {
            CurrentDialogBox = eDialogBox::None;
        }
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
    if (!isDialogBoxDrawing()) {
        return;
    }



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
    if (DialogType == eDialogTypeBySize::w512h512) {
        TitleOffset = 20;
    }


    // для кнопок диалога
    int ButtonOffset = 190;
    if (DialogType == eDialogTypeBySize::w512h512) {
        ButtonOffset = 256+190;
    }
    if (DialogType == eDialogTypeBySize::w768h600) {
        ButtonOffset = 256+88+190;
    }






    switch (CurrentDialogBox) {
    case eDialogBox::QuitFromGame: // хотим выйти или нет?
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("QUIT")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("QUIT"));
        // текст диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Are you sure you want to quit?"));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to quit?"));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to quit?"));
        }

        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Note: all game data will be saved."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: all game data will be saved."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: all game data will be saved."));
        }

        // кнопки
        if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetTextUTF32("YES"), DialogContentTransp)) {
            QuitFromMainLoop();
            CloseDialog();
        }
        if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetTextUTF32("NO"), DialogContentTransp)) {
            CloseDialog();
        }
        break;

    case eDialogBox::ProfileCreationError: // в профайле все занято, не можем создать новую запись.
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("ERROR")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("ERROR"));
        // текст диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Cannot create a Pilot Profile."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Cannot create a Pilot Profile."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Cannot create a Pilot Profile."));
        }

        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Tip: you should clear one line first."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Tip: you should clear one line first."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Tip: you should clear one line first."));
        }

        // кнопки
        if (DrawDialogButton200(X+128+64-72/2,Y+ButtonOffset, vw_GetTextUTF32("CLOSE"), DialogContentTransp)) {
            CloseDialog();
        }
        break;

    case eDialogBox::DeleteProfile: // удаление профайла - запрос
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("DELETE")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("DELETE"));
        // текст диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Are you sure you want to delete the Profile?"));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to delete the Profile?"));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to delete the Profile?"));
        }

        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Note: all Pilot Profile data will be lost."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: all Pilot Profile data will be lost."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: all Pilot Profile data will be lost."));
        }

        // кнопки
        if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetTextUTF32("YES"), DialogContentTransp)) {
            DeleteRecord();
            if (CurrentProfile != -1) {
                CurrentMission = GameConfig().Profile[CurrentProfile].LastMission;
            }
            CloseDialog();
        }
        if (DrawDialogButton128(X+256+34,Y+ButtonOffset, vw_GetTextUTF32("NO"), DialogContentTransp)) {
            CloseDialog();
        }
        break;


    case eDialogBox::RepairShip: // не полный ремонт (не достаточно денег)
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("REPAIR")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("REPAIR"));
        // текст диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Not enough money for a full hull repair."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Not enough money for a full hull repair."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Not enough money for a full hull repair."));
        }

        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Note: Repair has been limited by money."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: Repair has been limited by money."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: Repair has been limited by money."));
        }

        // кнопки
        if (DrawDialogButton128(X+192,Y+ButtonOffset, vw_GetTextUTF32("OK"), DialogContentTransp)) {
            CloseDialog();
        }
        break;


    case eDialogBox::QuitNoSave: // хотим выйти или нет?, с предупреждением, что не все сохраним
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("QUIT")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("QUIT"));
        // текст диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Are you sure you want to quit?"));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to quit?"));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to quit?"));
        }

        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Note: the current game data will be lost."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: the current game data will be lost."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: the current game data will be lost."));
        }

        // кнопки
        if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetTextUTF32("YES"), DialogContentTransp)) {
            QuitFromMainLoop();
            CloseDialog();
        }
        if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetTextUTF32("NO"), DialogContentTransp)) {
            CloseDialog();
        }
        break;
    case eDialogBox::QuiToMenuNoSave: // хотим выйти или нет?, с предупреждением, что не все сохраним
        // выход из игры в меню (основное)
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("QUIT")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("QUIT"));
        // текст диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Are you sure you want to quit?"));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to quit?"));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to quit?"));
        }

        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Note: the current game data will be lost."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: the current game data will be lost."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: the current game data will be lost."));
        }

        // кнопки
        if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetTextUTF32("YES"), DialogContentTransp)) {
            ExitGame(eCommand::SWITCH_FROM_GAME_TO_MAIN_MENU);
            CloseDialog();
        }
        if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetTextUTF32("NO"), DialogContentTransp)) {
            CloseDialog();
        }
        break;

    case eDialogBox::RestartLevelNoSave: // хотим рестарт игры?, с предупреждением, что не все сохраним
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("RESTART")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("RESTART"));
        // текст диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Are you sure you want to restart the game?"));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to restart the game?"));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to restart the game?"));
        }

        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Note: the current game data will be lost."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: the current game data will be lost."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: the current game data will be lost."));
        }

        // кнопки
        if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetTextUTF32("YES"), DialogContentTransp)) {
            ExitGame(eCommand::SWITCH_FROM_MENU_TO_GAME);
            CloseDialog();
        }
        if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetTextUTF32("NO"), DialogContentTransp)) {
            CloseDialog();
        }
        break;




    // вывод данных по кораблю
    case eDialogBox::ShowShipInfo: {
        auto sharedDialogSpaceShip = DialogSpaceShip.lock();
        auto sharedWorkshopFighterGame = WorkshopFighterGame.lock();
        if (sharedDialogSpaceShip && sharedWorkshopFighterGame) {
            // название диалога
            SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32(GetWorkshopShipName(sharedDialogSpaceShip->InternalType))))/2;
            vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32(GetWorkshopShipName(sharedDialogSpaceShip->InternalType)));
            // текст диалога
            int Y1 = Y+80;
            int Offset = 31;
            int Size = 240;
            float WScale = -200;
            int X1 = X+45;

            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Object Class:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Space Ship"));
            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Ship Type:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32(GetShipGroupTitle(sharedDialogSpaceShip->InternalType)));
            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Developer:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Earth Federation"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Width:"));
            sRGBCOLOR tmpColor{eRGBCOLOR::white};
            std::ostringstream tmpStream;
            tmpStream << std::fixed << std::setprecision(1);
            if (sharedDialogSpaceShip->Width == sharedWorkshopFighterGame->Width) {
                tmpStream << sharedDialogSpaceShip->Width << " ";
            } else {
                if (sharedDialogSpaceShip->Width > sharedWorkshopFighterGame->Width) {
                    tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                } else {
                    tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                }

                tmpStream << sharedDialogSpaceShip->Width << " (" << sharedWorkshopFighterGame->Width << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, DialogContentTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Length:"));
            tmpColor = sRGBCOLOR{eRGBCOLOR::white};
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (sharedDialogSpaceShip->Length == sharedWorkshopFighterGame->Length) {
                tmpStream << sharedDialogSpaceShip->Length << " ";
            } else {
                if (sharedDialogSpaceShip->Length > sharedWorkshopFighterGame->Length) {
                    tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                } else {
                    tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                }

                tmpStream << sharedDialogSpaceShip->Length << " (" << sharedWorkshopFighterGame->Length << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, DialogContentTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Height:"));
            tmpColor = sRGBCOLOR{eRGBCOLOR::white};
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (sharedDialogSpaceShip->Height == sharedWorkshopFighterGame->Height) {
                tmpStream << sharedDialogSpaceShip->Height << " ";
            } else {
                if (sharedDialogSpaceShip->Height > sharedWorkshopFighterGame->Height) {
                    tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                } else {
                    tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                }

                tmpStream << sharedDialogSpaceShip->Height << " (" << sharedWorkshopFighterGame->Height << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, DialogContentTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Engines:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << sharedDialogSpaceShip->Engines.size() << " " << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Armor:"));
            tmpColor = sRGBCOLOR{eRGBCOLOR::white};
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::setprecision(0);
            if (GetShipArmor(sharedDialogSpaceShip->InternalType) == GetShipArmor(sharedWorkshopFighterGame->InternalType)) {
                tmpStream << GetShipArmor(sharedDialogSpaceShip->InternalType) << " ";
            } else {
                if (GetShipArmor(sharedDialogSpaceShip->InternalType) < GetShipArmor(sharedWorkshopFighterGame->InternalType)) {
                    tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                } else {
                    tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                }

                tmpStream << GetShipArmor(sharedDialogSpaceShip->InternalType) << " (" << GetShipArmor(sharedWorkshopFighterGame->InternalType) << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, DialogContentTransp, tmpStream.str());

            // FIXME for "Upgrade"s are only difference with "Armor" is multiplier, remove code duplication here

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Upgrade Mk2:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (GetShipArmor(sharedDialogSpaceShip->InternalType) == GetShipArmor(sharedWorkshopFighterGame->InternalType)) {
                tmpStream << GetShipArmor(sharedDialogSpaceShip->InternalType) * 2 << " ";
            } else {
                tmpStream << GetShipArmor(sharedDialogSpaceShip->InternalType) * 2 << " (" << GetShipArmor(sharedWorkshopFighterGame->InternalType) * 2 << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, DialogContentTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Upgrade Mk3:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (GetShipArmor(sharedDialogSpaceShip->InternalType) == GetShipArmor(sharedWorkshopFighterGame->InternalType)) {
                tmpStream << GetShipArmor(sharedDialogSpaceShip->InternalType) * 3 << " ";
            } else {
                tmpStream << GetShipArmor(sharedDialogSpaceShip->InternalType) * 3 << " (" << GetShipArmor(sharedWorkshopFighterGame->InternalType) * 3 << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, DialogContentTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Upgrade Mk4:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (GetShipArmor(sharedDialogSpaceShip->InternalType) == GetShipArmor(sharedWorkshopFighterGame->InternalType)) {
                tmpStream << GetShipArmor(sharedDialogSpaceShip->InternalType) * 4 << " ";
            } else {
                tmpStream << GetShipArmor(sharedDialogSpaceShip->InternalType) * 4 << " (" << GetShipArmor(sharedWorkshopFighterGame->InternalType) * 4 << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, DialogContentTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Weapon Slots:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << sharedDialogSpaceShip->WeaponSlots.size() << " " << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Slot Levels:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (!sharedDialogSpaceShip->WeaponSlots.empty()) {
                tmpStream << sharedDialogSpaceShip->WeaponSlots[0].Type;
            }
            for (unsigned i = 1; i < sharedDialogSpaceShip->WeaponSlots.size(); i++) {
                tmpStream << "/" << sharedDialogSpaceShip->WeaponSlots[i].Type;
            }
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

            if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true)) {
                CloseDialog();
            }
        }
    }
    break;

    // вывод данных по оружию
    case eDialogBox::ShowWeaponsInfo: {
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32(GetWeaponName(DialogWeapon->InternalType))))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32(GetWeaponName(DialogWeapon->InternalType)));
        // текст диалога
        int Y1 = Y+80;
        int Offset = 31;
        int Size = 230;
        float WScale = -200;
        int X1 = X+45;

        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Object Class:"));
        if (DialogWeapon->InternalType < 16) {
            vw_DrawTextUTF32(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Weapon, Cannon"));
        } else {
            vw_DrawTextUTF32(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Weapon, Launcher"));
        }

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Type:"));
        vw_DrawTextUTF32(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32(GetWeaponGroupTitle(DialogWeapon->InternalType)));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Developer:"));
        vw_DrawTextUTF32(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Earth Federation"));

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Armor:"));
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << DialogWeapon->ArmorInitialStatus << " " << vw_GetText("units");
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Width:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << DialogWeapon->Width << " " << vw_GetText("units");
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Length:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << DialogWeapon->Length << " " << vw_GetText("units");
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Height:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << DialogWeapon->Height << " " << vw_GetText("units");
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Damage, Kinetic:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(0)
                  << GetProjectileDamageKinetic(DialogWeapon->InternalType) << " ";
        if (DialogWeapon->InternalType == 11
            || DialogWeapon->InternalType == 12
            || DialogWeapon->InternalType == 14) {
            tmpStream << vw_GetText("units/sec");
        } else {
            tmpStream << vw_GetText("units/shot");
        }
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Damage, EM:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(0)
                  << GetProjectileDamageEM(DialogWeapon->InternalType) << " ";
        if (DialogWeapon->InternalType == 11
            || DialogWeapon->InternalType == 12
            || DialogWeapon->InternalType == 14) {
            tmpStream << vw_GetText("units/sec");
        } else {
            tmpStream << vw_GetText("units/shot");
        }
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Energy Use:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << DialogWeapon->EnergyUse << " " << vw_GetText("units per shot");
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Ammo:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(0)
                  << DialogWeapon->AmmoStart << " " << vw_GetText("units");
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Reload:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << DialogWeapon->NextFireTime << " " << vw_GetText("seconds");
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        Y1 += Offset;
        vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Range:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << std::setprecision(1)
                  << GetProjectileRange(DialogWeapon->InternalType) << " " << vw_GetText("units");
        vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        // закрываем...
        if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true)) {
            CloseDialog();
        }
    }
    break;

    // вывод данных по системам
    case eDialogBox::ShowSystemsInfo: {
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32(GetSystemName(DialogSystem))))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32(GetSystemName(DialogSystem)));
        // текст диалога
        int Y1 = Y+80;
        int Offset = 31;
        int Size = 230;
        float WScale = -200;
        int Size2 = 130;
        int X1 = X+45;

        sRGBCOLOR tmpColor{eRGBCOLOR::white};
        float tmpTransp{DialogContentTransp};
        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(1);

        switch (DialogSystem) {
        case 1:
        case 2:
        case 3:
        case 4:
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Object Class:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("System"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Type:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Spaceship Engine"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Developer:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Earth Federation"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Energy Use:"));
            if (NeedMoreEnergyDialog
                && GameConfig().Profile[CurrentProfile].SpaceShipControlMode != 1) {
                tmpColor = sRGBCOLOR{eRGBCOLOR::red};
                tmpTransp = DialogContentTransp * CurrentAlert3;
                tmpStream << GetShipEngineSystemEnergyUse(DialogSystem) << " ";
            } else if (GameConfig().Profile[CurrentProfile].EngineSystem == DialogSystem
                       || GameConfig().Profile[CurrentProfile].EngineSystem == 0) {
                tmpStream << GetShipEngineSystemEnergyUse(DialogSystem) << " ";
            } else if (GetShipEngineSystemEnergyUse(DialogSystem) < GetShipEngineSystemEnergyUse(GameConfig().Profile[CurrentProfile].EngineSystem)) {
                tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                tmpStream << GetShipEngineSystemEnergyUse(DialogSystem) << " ("
                          << GetShipEngineSystemEnergyUse(GameConfig().Profile[CurrentProfile].EngineSystem) << ") ";
            } else {
                tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                tmpStream << GetShipEngineSystemEnergyUse(DialogSystem) << " ("
                          << GetShipEngineSystemEnergyUse(GameConfig().Profile[CurrentProfile].EngineSystem) << ") ";
            }
            tmpStream << vw_GetText("units per sec");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Engine Power:"));
            tmpColor = sRGBCOLOR{eRGBCOLOR::white};
            tmpTransp = DialogContentTransp;
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (GameConfig().Profile[CurrentProfile].EngineSystem == DialogSystem
                || GameConfig().Profile[CurrentProfile].EngineSystem == 0) {
                tmpStream << GetEnginePower(DialogSystem) << " ";
            } else if (GetEnginePower(DialogSystem) > GetEnginePower(GameConfig().Profile[CurrentProfile].EngineSystem)) {
                tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                tmpStream << GetEnginePower(DialogSystem) << " ("
                          << GetEnginePower(GameConfig().Profile[CurrentProfile].EngineSystem) << ") ";
            } else {
                tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                tmpStream << GetEnginePower(DialogSystem) << " ("
                          << GetEnginePower(GameConfig().Profile[CurrentProfile].EngineSystem) << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Acceleration:"));
            tmpColor = sRGBCOLOR{eRGBCOLOR::white};
            tmpTransp = DialogContentTransp;
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (GameConfig().Profile[CurrentProfile].EngineSystem == DialogSystem
                || GameConfig().Profile[CurrentProfile].EngineSystem == 0) {
                tmpStream << GetEngineAcceleration(DialogSystem) << " ";
            } else if (GetEngineAcceleration(DialogSystem) > GetEngineAcceleration(GameConfig().Profile[CurrentProfile].EngineSystem)) {
                tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                tmpStream << GetEngineAcceleration(DialogSystem) << " ("
                          << GetEngineAcceleration(GameConfig().Profile[CurrentProfile].EngineSystem) << ") ";
            } else {
                tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                tmpStream << GetEngineAcceleration(DialogSystem) << " ("
                          << GetEngineAcceleration(GameConfig().Profile[CurrentProfile].EngineSystem) << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Relations/Dependencies:"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetTextUTF32("Power Source"));

            if (GameConfig().Profile[CurrentProfile].SpaceShipControlMode == 1) {
                Y1 += Offset;
                Y1 += Offset;
                Y1 += Offset;
                SizeI = (W-vw_TextWidthUTF32(vw_GetTextUTF32("Useless with Arcade")))/2-45;
                vw_DrawTextUTF32(X1+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, DialogContentTransp, vw_GetTextUTF32("Useless with Arcade"));
                Y1 += Offset;
                SizeI = (W-vw_TextWidthUTF32(vw_GetTextUTF32("SpaceShip Control Mode.")))/2-45;
                vw_DrawTextUTF32(X1+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, DialogContentTransp, vw_GetTextUTF32("SpaceShip Control Mode."));
            }

            break;

        case 5:
        case 6:
        case 7:
        case 8:
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Object Class:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("System"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Type:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Power Source"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Developer:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Earth Federation"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Energy Capacity:"));

            if (GameConfig().Profile[CurrentProfile].PowerSystem == DialogSystem - 4
                || GameConfig().Profile[CurrentProfile].PowerSystem == 0) {
                tmpStream << GetShipMaxEnergy(DialogSystem - 4) << " ";
            } else if (GetShipMaxEnergy(DialogSystem-4) > GetShipMaxEnergy(GameConfig().Profile[CurrentProfile].PowerSystem)) {
                tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                tmpStream << GetShipMaxEnergy(DialogSystem - 4) << " ("
                          << GetShipMaxEnergy(GameConfig().Profile[CurrentProfile].PowerSystem) << ") ";
            } else {
                tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                tmpStream << GetShipMaxEnergy(DialogSystem - 4) << " ("
                          << GetShipMaxEnergy(GameConfig().Profile[CurrentProfile].PowerSystem) << ") ";
            }
            tmpStream << vw_GetText("units");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Recharge Rate:"));
            tmpColor = sRGBCOLOR{eRGBCOLOR::white};
            tmpTransp = DialogContentTransp;
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (NeedMoreEnergyDialog) {
                tmpColor = sRGBCOLOR{eRGBCOLOR::red};
                tmpTransp = DialogContentTransp * CurrentAlert3;
                tmpStream << GetShipRechargeEnergy(DialogSystem - 4) << " ";
            } else if (GameConfig().Profile[CurrentProfile].PowerSystem == (DialogSystem - 4)
                       || GameConfig().Profile[CurrentProfile].PowerSystem == 0) {
                tmpStream << GetShipRechargeEnergy(DialogSystem - 4) << " ";
            } else if (GetShipRechargeEnergy(DialogSystem-4) > GetShipRechargeEnergy(GameConfig().Profile[CurrentProfile].PowerSystem)) {
                tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                tmpStream << GetShipRechargeEnergy(DialogSystem - 4) << " ("
                          << GetShipRechargeEnergy(GameConfig().Profile[CurrentProfile].PowerSystem) << ") ";
            } else {
                tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                tmpStream << GetShipRechargeEnergy(DialogSystem - 4) << " ("
                          << GetShipRechargeEnergy(GameConfig().Profile[CurrentProfile].PowerSystem) << ") ";
            }
            tmpStream << vw_GetText("units per sec");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Relations/Dependencies:"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Weapon Recharge Rate"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Spaceship Engine"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Optical Computer"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Targeting System"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Advanced System"));

            break;

        case 9:
        case 10:
        case 11:
        case 12:

            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Object Class:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("System"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Type:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Optical Computer"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Developer:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Earth Federation"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Energy Use:"));
            tmpStream << 0.1f << " " << vw_GetText("units per sec");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Target Tracing:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::setprecision(0);
            if (DialogSystem == 9 || DialogSystem == 10) {
                tmpStream << 1 << " ";
            } else if (DialogSystem == 11 || DialogSystem == 12) {
                tmpStream << 6 << " ";
            }
            tmpStream << vw_GetText("object(s)");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Auto Adjustment:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            if (DialogSystem == 9 || DialogSystem == 11) {
                tmpStream << vw_GetText("No");
            } else if (DialogSystem == 10 || DialogSystem == 12) {
                tmpStream << vw_GetText("Yes");
            }
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Relations/Dependencies:"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetTextUTF32("Power Source"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetTextUTF32("Weapon Fire Range"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Weapon Orientation"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Targeting System"));

            break;

        case 13:
        case 14:
        case 15:
        case 16:
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Object Class:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("System"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Type:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Targeting System"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Developer:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Earth Federation"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Energy Use:"));
            tmpStream << 0.1f << " " << vw_GetText("units per sec");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Orientation Speed:"));
            tmpStream.clear();
            tmpStream.str(std::string{});
            tmpStream << std::setprecision(0);
            if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem == (DialogSystem - 12)
                || GameConfig().Profile[CurrentProfile].TargetingMechanicSystem == 0) {
                tmpStream << 40.0f * (DialogSystem - 12) << " ";
            } else if (DialogSystem - 12 > GameConfig().Profile[CurrentProfile].TargetingMechanicSystem) {
                tmpColor = sRGBCOLOR{eRGBCOLOR::green};
                tmpStream << 40.0f * (DialogSystem - 12) << " ("
                          << 40.0f * GameConfig().Profile[CurrentProfile].TargetingMechanicSystem << ") ";
            } else {
                tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
                tmpStream << 40.0f * (DialogSystem - 12) << " ("
                          << 40.0f * GameConfig().Profile[CurrentProfile].TargetingMechanicSystem << ") ";
            }
            tmpStream << vw_GetText("deg per sec");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Relations/Dependencies:"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetTextUTF32("Power Source"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetTextUTF32("Optical Computer"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Weapon Orientation"));

            if (GameConfig().Profile[CurrentProfile].WeaponTargetingMode == 1) {
                Y1 += Offset;
                Y1 += Offset;
                Y1 += Offset;
                SizeI = (W-vw_TextWidthUTF32(vw_GetTextUTF32("Useless with Arcade")))/2-45;
                vw_DrawTextUTF32(X1+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, DialogContentTransp, vw_GetTextUTF32("Useless with Arcade"));
                Y1 += Offset;
                SizeI = (W-vw_TextWidthUTF32(vw_GetTextUTF32("Weapon Targeting Mode.")))/2-45;
                vw_DrawTextUTF32(X1+SizeI, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, DialogContentTransp, vw_GetTextUTF32("Weapon Targeting Mode."));
            }

            break;

        case 17:
        case 18:
        case 19:
        case 20:
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Object Class:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("System"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Type:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Advanced System"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Developer:"));
            vw_DrawTextUTF32(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Earth Federation"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Energy Use:"));
            if (NeedMoreEnergyDialog) {
                tmpTransp = DialogContentTransp * CurrentAlert3;
                tmpColor = sRGBCOLOR{eRGBCOLOR::red};
            }
            tmpStream << GetShipProtectionSystemEnergyUse(DialogSystem - 16) << " " << vw_GetText("units per sec");
            vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

            tmpTransp = DialogContentTransp;
            tmpColor = sRGBCOLOR{eRGBCOLOR::white};
            tmpStream.clear();
            tmpStream.str(std::string{});

            Y1 += Offset;
            if (DialogSystem == 17) {
                vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Hull Repair Rate:"));
                tmpStream << 0.5f << "% " << vw_GetText("per sec");
                vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());
            } else if (DialogSystem == 18) {
                vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Resistance, Hull:"));
                vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "x4");

                Y1 += Offset;
                vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Resistance, System:"));
                vw_DrawText(X1+Size, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, "x5");
            } else if (DialogSystem == 19) {
                vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Shield Power:"));
                tmpStream << 100.0f << " " << vw_GetText("units");
                vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

                Y1 += Offset;
                vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Recharge Rate:"));
                tmpStream.clear();
                tmpStream.str(std::string{});
                tmpStream << 2 << "% " << vw_GetText("per sec");
                vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());
            } else if (DialogSystem == 20) {
                vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Shield Power:"));
                tmpStream << 150.0f << " " << vw_GetText("units");
                vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

                Y1 += Offset;
                vw_DrawTextUTF32(X1, Y1, WScale, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Recharge Rate:"));
                tmpStream.clear();
                tmpStream.str(std::string{});
                tmpStream << 3 << "% " << vw_GetText("per sec");
                vw_DrawText(X1 + Size, Y1, WScale, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());
            }

            Y1 += Offset;
            vw_DrawTextUTF32(X1, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Relations/Dependencies:"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::orange}, DialogContentTransp, vw_GetTextUTF32("Power Source"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Hull Resistance"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("Systems Resistance"));

            Y1 += Offset;
            vw_DrawTextUTF32(X1+Size2, Y1, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::green}, DialogContentTransp, vw_GetTextUTF32("SpaceShip Vitality"));

            break;
        }

        if (vw_GetMouseLeftClick(true) || vw_GetMouseRightClick(true)) {
            CloseDialog();
        }
    }
    break;


    case eDialogBox::ProfileTipsAndTricks: { // подсказки на меню профилей
        // название диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Tips and hints"));
        SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("Tips and hints"));
        // текст диалога

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("It is possible to create five Pilot Profiles in the game in"));
        vw_DrawTextUTF32(X+25, Y+ 80, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("It is possible to create five Pilot Profiles in the game in"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("order to store personal data. You can set an individual"));
        vw_DrawTextUTF32(X+25, Y+115, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("order to store personal data. You can set an individual"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("difficulty level for each profile or delete the current"));
        vw_DrawTextUTF32(X+25, Y+150, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("difficulty level for each profile or delete the current"));
        vw_DrawTextUTF32(X+25, Y+185, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("profile to free space for a new profile."));

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Note that the current difficulty level is displayed in percent"));
        vw_DrawTextUTF32(X+25, Y+240, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Note that the current difficulty level is displayed in percent"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("for each profile. This value influences the increase of"));
        vw_DrawTextUTF32(X+25, Y+275, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("for each profile. This value influences the increase of"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("experience. For instance, if you bring down an enemy ship, you"));
        vw_DrawTextUTF32(X+25, Y+310, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("experience. For instance, if you bring down an enemy ship, you"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("will get 100 money units and only 65 experience units if the"));
        vw_DrawTextUTF32(X+25, Y+345, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("will get 100 money units and only 65 experience units if the"));
        vw_DrawTextUTF32(X+25, Y+380, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("difficulty percent is set to 65 for you."));

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Note: you can change your difficulty level before each"));
        vw_DrawTextUTF32(X+25, Y+435, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: you can change your difficulty level before each"));
        vw_DrawTextUTF32(X+25, Y+470, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("new mission."));

        // чекбокс
        bool ttt = !GameConfig().NeedShowHint[0];
        DrawCheckBox_2(X+36, Y+ButtonOffset, ttt, vw_GetTextUTF32("Do not show this tip again."), DialogContentTransp);
        ChangeGameConfig().NeedShowHint[0] = !ttt;
        // кнопки
        if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetTextUTF32("CLOSE"), DialogContentTransp)) {
            CloseDialog();
        }
        break;
    }
    case eDialogBox::ShipyardTipsAndTricks: { // подсказки на меню шипъярд
        // название диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Tips and hints"));
        SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("Tips and hints"));
        // текст диалога

        int k=28;
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("There are more than 20 spaceships with unique characteristics"));
        vw_DrawTextUTF32(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("There are more than 20 spaceships with unique characteristics"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("available in the game. You can use the right part of the screen"));
        vw_DrawTextUTF32(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("available in the game. You can use the right part of the screen"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("to see your current spaceship, its characteristics and cost and"));
        vw_DrawTextUTF32(X+25, Y+80+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("to see your current spaceship, its characteristics and cost and"));
        vw_DrawTextUTF32(X+25, Y+80+k*3, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("also to perform basic operations, such as repair."));

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("The left part of the screen contains Ship Stock where you can see"));
        vw_DrawTextUTF32(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("The left part of the screen contains Ship Stock where you can see"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("all available spaceships and their characteristics. Do not"));
        vw_DrawTextUTF32(X+25, Y+100+k*5, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("all available spaceships and their characteristics. Do not"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("forget that each ship has its dimensions and, which is more"));
        vw_DrawTextUTF32(X+25, Y+100+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("forget that each ship has its dimensions and, which is more"));
        vw_DrawTextUTF32(X+25, Y+100+k*7, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("important, different weapon mounts."));

        vw_DrawTextUTF32(X+25, Y+120+k*8, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("The basic shipbuilding concept is as follows:"));
        vw_DrawTextUTF32(X+25, Y+120+k*9, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Scout - light, small and maneuverable ships with weak weapons."));
        vw_DrawTextUTF32(X+25, Y+120+k*10, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Interceptor - fast ships with light weapons."));
        vw_DrawTextUTF32(X+25, Y+120+k*11, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Fighter - main type of ships with light and semi-heavy weapons."));
        vw_DrawTextUTF32(X+25, Y+120+k*12, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Bomber - heavy destroyers with mostly heavy attack weapons."));
        vw_DrawTextUTF32(X+25, Y+120+k*13, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Attack Ship - heavy, large and heavily armed ships."));

        // чекбокс
        bool ttt = !GameConfig().NeedShowHint[1];
        DrawCheckBox_2(X+36, Y+ButtonOffset, ttt, vw_GetTextUTF32("Do not show this tip again."), DialogContentTransp);
        ChangeGameConfig().NeedShowHint[1] = !ttt;
        // кнопки
        if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetTextUTF32("CLOSE"), DialogContentTransp)) {
            CloseDialog();
        }
        break;
    }
    case eDialogBox::SystemsTipsAndTricks: { // подсказки на меню системы
        // название диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Tips and hints"));
        SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("Tips and hints"));
        // текст диалога

        int k=25;
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("A set of systems is installed on the ship you operate. All systems"));
        vw_DrawTextUTF32(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("A set of systems is installed on the ship you operate. All systems"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("are connected with each other and with ship weapons in one way or"));
        vw_DrawTextUTF32(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("are connected with each other and with ship weapons in one way or"));
        vw_DrawTextUTF32(X+25, Y+80+k*2, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("another and play important role in the game."));

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("You should carefully take into account the peculiarities of each"));
        vw_DrawTextUTF32(X+25, Y+100+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("You should carefully take into account the peculiarities of each"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("system and match their characteristics to each other to get"));
        vw_DrawTextUTF32(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("system and match their characteristics to each other to get"));
        vw_DrawTextUTF32(X+25, Y+100+k*5, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("a well balanced warship that can complete combat tasks."));

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Engine - ship engine system. Pay attention to how this system consumes"));
        vw_DrawTextUTF32(X+25, Y+120+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Engine - ship engine system. Pay attention to how this system consumes"));
        vw_DrawTextUTF32(X+40, Y+120+k*7, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("energy. Otherwise you run the risk of being unable to move."));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("energy. Otherwise you run the risk of being unable to move."));
        vw_DrawTextUTF32(X+25, Y+120+k*8, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Power - ship energy source. Pay attention to the main two characte-"));
        vw_DrawTextUTF32(X+40, Y+120+k*9, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("ristics of this system - Recharge Rate and Capacity."));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("O.T.C. - Optical Target Computer. As you can see from its name,"));
        vw_DrawTextUTF32(X+25, Y+120+k*10, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("O.T.C. - Optical Target Computer. As you can see from its name,"));
        vw_DrawTextUTF32(X+40, Y+120+k*11, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("this system is responsible for optical targeting."));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Targeting - Weapon targeting system. It depends on this system how"));
        vw_DrawTextUTF32(X+25, Y+120+k*12, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Targeting - Weapon targeting system. It depends on this system how"));
        vw_DrawTextUTF32(X+40, Y+120+k*13, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("fast a weapon will aim and how accurate the shot will be."));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Advanced - The most advanced, but yet untested systems for"));
        vw_DrawTextUTF32(X+25, Y+120+k*14, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Advanced - The most advanced, but yet untested systems for"));
        vw_DrawTextUTF32(X+40, Y+120+k*15, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("enhancing ship invulnerability."));

        // чекбокс
        bool ttt = !GameConfig().NeedShowHint[2];
        DrawCheckBox_2(X+36, Y+ButtonOffset, ttt, vw_GetTextUTF32("Do not show this tip again."), DialogContentTransp);
        ChangeGameConfig().NeedShowHint[2] = !ttt;
        // кнопки
        if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetTextUTF32("CLOSE"), DialogContentTransp)) {
            CloseDialog();
        }
        break;
    }
    case eDialogBox::WeaponryTipsAndTricks: { // подсказки на меню оружейная
        // название диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Tips and hints"));
        SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("Tips and hints"));
        // текст диалога

        int k=25;
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("You can use this section to buy, sell and set up the weapons on"));
        vw_DrawTextUTF32(X+25, Y+80+k*0, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("You can use this section to buy, sell and set up the weapons on"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("your ship. To buy a weapon, just drag its icon from Weapon Stock"));
        vw_DrawTextUTF32(X+25, Y+80+k*1, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("your ship. To buy a weapon, just drag its icon from Weapon Stock"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("to a weapon slot on your ship. Note that each weapon has its"));
        vw_DrawTextUTF32(X+25, Y+80+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("to a weapon slot on your ship. Note that each weapon has its"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("own level (from 1 to 5), you should see that the slot level is"));
        vw_DrawTextUTF32(X+25, Y+80+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("own level (from 1 to 5), you should see that the slot level is"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("not less than the weapon level. To sell a weapon, perform"));
        vw_DrawTextUTF32(X+25, Y+80+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("not less than the weapon level. To sell a weapon, perform"));
        vw_DrawTextUTF32(X+25, Y+80+k*5, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("the reverse operation."));

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("The weapon is set up and recharged with the Setup button"));
        vw_DrawTextUTF32(X+25, Y+100+k*6, SizeI > 716 ? -716 : 716, 0,1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("The weapon is set up and recharged with the Setup button"));
        vw_DrawTextUTF32(X+25, Y+100+k*7, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("in the right part of the weapon slot panel."));

        vw_DrawTextUTF32(X+25, Y+120+k*8, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Main types of weapons by their projectiles:"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Projectile - small projectiles based on bullets or energy beams."));
        vw_DrawTextUTF32(X+25, Y+120+k*9, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Projectile - small projectiles based on bullets or energy beams."));
        vw_DrawTextUTF32(X+40, Y+120+k*10, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("It is the main and most widespread weapon in the game."));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Self-propelled projectile - missiles, torpedoes and nuclear bombs."));
        vw_DrawTextUTF32(X+25, Y+120+k*11, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Self-propelled projectile - missiles, torpedoes and nuclear bombs."));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("The disadvantages of these weapons include the low speed"));
        vw_DrawTextUTF32(X+40, Y+120+k*12, SizeI > 701 ? -701 : 701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("The disadvantages of these weapons include the low speed"));
        vw_DrawTextUTF32(X+40, Y+120+k*13, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("of the projectile and the possibility of bringing it down."));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Beam - Lasers and Masers. Short-range highly energy-"));
        vw_DrawTextUTF32(X+25, Y+120+k*14, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Beam - Lasers and Masers. Short-range highly energy-"));
        vw_DrawTextUTF32(X+40, Y+120+k*15, -701, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("consuming weapons."));



        // чекбокс
        bool ttt = !GameConfig().NeedShowHint[3];
        DrawCheckBox_2(X+36, Y+ButtonOffset, ttt, vw_GetTextUTF32("Do not show this tip again."), DialogContentTransp);
        ChangeGameConfig().NeedShowHint[3] = !ttt;
        // кнопки
        if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetTextUTF32("CLOSE"), DialogContentTransp)) {
            CloseDialog();
        }
        break;
    }

    case eDialogBox::ShortkeyTipsAndTricks: { // подсказки на горячие клавиши в игре
        // название диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Tips and hints"));
        SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("Tips and hints"));
        // текст диалога

        int k=23;

        SizeI = (W-vw_TextWidthUTF32(vw_GetTextUTF32("GAME KEYBOARD SHORTCUTS")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+80+k*0, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("GAME KEYBOARD SHORTCUTS"));

        vw_DrawTextUTF32(X+25, Y+100+k*1, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("F2 - toggle show/hide game FPS counter."));

        vw_DrawTextUTF32(X+25, Y+120+k*2, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("F5 - decrease game speed."));
        vw_DrawTextUTF32(X+25, Y+120+k*3, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("F6 - reset game speed to normal."));
        vw_DrawTextUTF32(X+25, Y+120+k*4, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("F7 - increase game speed."));
        vw_DrawTextUTF32(X+25, Y+120+k*5, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: do not increase game speed, if you have low FPS."));

        vw_DrawTextUTF32(X+25, Y+140+k*6, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("F8 - change weapon panels view."));
        vw_DrawTextUTF32(X+25, Y+140+k*7, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("F9 - change Primary weapon group fire mode."));
        vw_DrawTextUTF32(X+25, Y+140+k*8, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("F10 - change Secondary weapon group fire mode."));

        vw_DrawTextUTF32(X+25, Y+160+k*9, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("F12 - capture screenshot. (File will be saved on your Desktop.)"));

        vw_DrawTextUTF32(X+25, Y+180+k*10, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Esc - show/hide game menu. (Pause.)"));
        vw_DrawTextUTF32(X+25, Y+180+k*11, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: all changes will be saved in the game configuration file."));



        // чекбокс
        bool ttt = !GameConfig().NeedShowHint[4];
        DrawCheckBox_2(X+36, Y+ButtonOffset, ttt, vw_GetTextUTF32("Do not show this tip again."), DialogContentTransp);
        ChangeGameConfig().NeedShowHint[4] = !ttt;
        // кнопки
        if (DrawDialogButton128(X+512+90, Y+ButtonOffset, vw_GetTextUTF32("START"), DialogContentTransp)) {
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
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Tips and hints"));
        SizeI = SizeI1 > 210 ? 17 + (WTitle-210)/2 : 17 + (WTitle-SizeI1)/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, SizeI1 > 210 ? -210 : 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("Tips and hints"));
        // текст диалога

        int k=30;

        vw_DrawTextUTF32(X+25, Y+80+k*0, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("You have already completed this mission."));

        std::ostringstream tmpStream;
        tmpStream << std::fixed << std::setprecision(0)
                  << vw_GetText("Your previous best result:") << " "
                  << GameConfig().Profile[CurrentProfile].ByMissionExperience[CurrentMission];
        vw_DrawText(X+25, Y+80+k*1, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, tmpStream.str());

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("You can replay this mission as many times as you like, but you"));
        vw_DrawTextUTF32(X+25, Y+100+k*2, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("You can replay this mission as many times as you like, but you"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("should take into account that only the best result you"));
        vw_DrawTextUTF32(X+25, Y+100+k*3, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("should take into account that only the best result you"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("number of experience points."));
        vw_DrawTextUTF32(X+25, Y+100+k*4, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("achieve in this mission will be used to calculate the total"));
        vw_DrawTextUTF32(X+25, Y+100+k*5, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("number of experience points."));

        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Note that each time you complete the mission again,"));
        vw_DrawTextUTF32(X+25, Y+120+k*6, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("Note that each time you complete the mission again,"));
        SizeI = vw_TextWidthUTF32(vw_GetTextUTF32("the amount of money you get when you destroy the enemy"));
        vw_DrawTextUTF32(X+25, Y+120+k*7, SizeI > 716 ? -716 : 716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("the amount of money you get when you destroy the enemy"));
        vw_DrawTextUTF32(X+25, Y+120+k*8, -716, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("in this mission is reduced two times."));


        // чекбокс
        bool ttt = !GameConfig().NeedShowHint[5];
        DrawCheckBox_2(X+36, Y+ButtonOffset, ttt, vw_GetTextUTF32("Do not show this tip again."), DialogContentTransp);
        ChangeGameConfig().NeedShowHint[5] = !ttt;
        // кнопки
        if (DrawDialogButton200(X+512+90-72, Y+ButtonOffset, vw_GetTextUTF32("CLOSE"), DialogContentTransp)) {
            CloseDialog();
            cCommand::GetInstance().Set(eCommand::SWITCH_TO_WORKSHOP);
            CurrentWorkshop = 3;
            WorkshopCreate();
        }

        break;
    }

    // спрашиваем какой язык при первом старте игры
    case eDialogBox::ChoseLanguage: {
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("Language")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("Language"));


        if (DrawDialogButton128(X+34, Y+ButtonOffset-73, ConvertUTF8.from_bytes("<<<"), DialogContentTransp)) {
            if (GameConfig().MenuLanguage == 0) {
                ChangeGameConfig().MenuLanguage = vw_GetLanguageListCount() - 1;
            } else {
                ChangeGameConfig().MenuLanguage--;
            }

            vw_SetTextLanguage(GameConfig().MenuLanguage);
            // forced to regenerate textures (base texture connected to language-related characters set)
            vw_ReleaseAllFontChars();
            GenerateFonts();
        }
        if (DrawDialogButton128(X+316+34, Y+ButtonOffset-73, ConvertUTF8.from_bytes(">>>"), DialogContentTransp)) {
            if (GameConfig().MenuLanguage >= (vw_GetLanguageListCount() - 1)) {
                ChangeGameConfig().MenuLanguage = 0;
            } else {
                ChangeGameConfig().MenuLanguage++;
            }

            vw_SetTextLanguage(GameConfig().MenuLanguage);
            // forced to regenerate textures (base texture connected to language-related characters set)
            vw_ReleaseAllFontChars();
            GenerateFonts();
        }

        vw_SetFontSize(24);
        int Size;
        Size = vw_TextWidthUTF32(vw_GetTextUTF32("English", GameConfig().MenuLanguage));
        SizeI = (170-Size)/2;
        if (Size > 170) {
            vw_DrawTextUTF32(X+138+34, Y+ButtonOffset-71, -170, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("English", GameConfig().MenuLanguage));
        } else {
            vw_DrawTextUTF32(X+138+34+SizeI, Y+ButtonOffset-71, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, DialogContentTransp, vw_GetTextUTF32("English", GameConfig().MenuLanguage));
        }
        ResetFontSize();

        if (DrawDialogButton200(X+128+64-72/2, Y+ButtonOffset, vw_GetTextUTF32("OK"), DialogContentTransp)) {
            // первоначально, язык голоса ставим такой же, как и язык меню
            ChangeGameConfig().VoiceLanguage = GameConfig().MenuLanguage;
            CloseDialog();
        }
        break;
    }

    case eDialogBox::RestartOnAdvOptChanged: // при изменении продвинутых настроек в самой игре, с предупреждением, что не все сохраним
        // название диалога
        SizeI = 17 + (WTitle-vw_TextWidthUTF32(vw_GetTextUTF32("RESTART")))/2;
        vw_DrawTextUTF32(X+SizeI, Y+TitleOffset, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.7f*DialogContentTransp, vw_GetTextUTF32("RESTART"));
        // текст диалога
        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Are you sure you want to restart the game?"));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+100, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to restart the game?"));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+100, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, DialogContentTransp, vw_GetTextUTF32("Are you sure you want to restart the game?"));
        }

        SizeI1 = vw_TextWidthUTF32(vw_GetTextUTF32("Note: the current game data will be lost."));
        SizeI = (W-SizeI1)/2;
        if (SizeI1 > 470) {
            SizeI = (W - 470)/2;
            vw_DrawTextUTF32(X+SizeI, Y+130, -470, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: the current game data will be lost."));
        } else {
            vw_DrawTextUTF32(X+SizeI, Y+130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*DialogContentTransp, vw_GetTextUTF32("Note: the current game data will be lost."));
        }

        // кнопки
        if (DrawDialogButton128(X+94, Y+ButtonOffset, vw_GetTextUTF32("YES"), DialogContentTransp)) {
            CloseDialog();
            RecreateGameWindow();
            SaveOptionsAdvMenuTmpData();
        }
        if (DrawDialogButton128(X+256+34, Y+ButtonOffset, vw_GetTextUTF32("NO"), DialogContentTransp)) {
            CloseDialog();
        }
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
