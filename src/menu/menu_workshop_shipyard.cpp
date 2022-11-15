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
//       all ostringstream-related code should be called only one time in init

// TODO translate comments

#include "../game.h"
#include "../config/config.h"
#include "../ui/font.h"
#include "../assets/texture.h"
#include "../object3d/space_ship/space_ship.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

extern std::weak_ptr<cSpaceShip> WorkshopFighterGame;
extern std::weak_ptr<cSpaceShip> WorkshopNewFighter;
extern int CurrentWorkshopNewFighter;
extern int CurrentWorkshopNewWeapon;
extern float CurrentAlert2;
extern float CurrentAlert3;

void WorkshopDrawShip(std::weak_ptr<cSpaceShip> &SpaceShip, int Mode);
void WorkshopCreateNewShip();
int GetSystemCost(int Num);
int GetWeaponCost(int Num, int Ammo, int AmmoStart);
int GetWeaponBaseCost(int Num);

// what should draw in dialogue 6,7,8
extern std::weak_ptr<cSpaceShip> DialogSpaceShip;






//------------------------------------------------------------------------------------
// название корабля
//------------------------------------------------------------------------------------
const char *GetWorkshopShipName(int Num)
{
    switch (Num) {
    case 1:
        return "Cobra";
    case 2:
        return "SpaceEagle";
    case 3:
        return "FireStorm";
    case 4:
        return "Hedgehog";
    case 5:
        return "Thunder";
    case 6:
        return "Excalibur";
    case 7:
        return "DarkStar";
    case 8:
        return "Scorpion";
    case 9:
        return "Runner";
    case 10:
        return "Kronos";
    case 11:
        return "Chameleon";
    case 12:
        return "Falcon";
    case 13:
        return "DeathRipple";
    case 14:
        return "AstroSword";
    case 15:
        return "Pulser";
    case 16:
        return "ClumsyMace";
    case 17:
        return "Raptor";
    case 18:
        return "Destroyer";
    case 19:
        return "AstroSpear";
    case 20:
        return "Mite";
    case 21:
        return "Viper";
    case 22:
        return "BlackIce";

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return nullptr;
}




//------------------------------------------------------------------------------------
// тип корабля
//------------------------------------------------------------------------------------
const char *GetShipGroupTitle(int Num)
{
    switch (Num) {
    case 1:
    case 4:
    case 6:
    case 10:
    case 12:
    case 14:
    case 15:
    case 17:
    case 20:
        return "Fighter";
    case 2:
    case 5:
    case 19:
        return "Interceptor";
    case 3:
    case 16:
    case 18:
        return "Bomber";
    case 7:
    case 13:
        return "Attack Ship";
    case 8:
    case 9:
    case 11:
    case 21:
    case 22:
        return "Scout";

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return nullptr;
}



//------------------------------------------------------------------------------------
// стоимость корпуса
//------------------------------------------------------------------------------------
int GetWorkshopShipCost(int Num)
{
    switch (Num) {
    case 1:
        return 1100;
    case 2:
        return 900;
    case 3:
        return 1500;
    case 4:
        return 1300;
    case 5:
        return 1350;
    case 6:
        return 2400;
    case 7:
        return 3000;
    case 8:
        return 3500;
    case 9:
        return 4000;
    case 10:
        return 6000;
    case 11:
        return 4500;
    case 12:
        return 5000;
    case 13:
        return 9000;
    case 14:
        return 7500;
    case 15:
        return 7000;
    case 16:
        return 9000;
    case 17:
        return 10000;
    case 18:
        return 12000;
    case 19:
        return 5000;
    case 20:
        return 9000;
    case 21:
        return 10000;
    case 22:
        return 8000;

    default:
        std::cerr << __func__ << "(): " << "wrong Num.\n";
        break;
    }

    return 0;
}






int GetWorkshopShipRepairCost(int Num, std::weak_ptr<cSpaceShip> &Fighter)
{
    int ShipCost = GetWorkshopShipCost(Num) * GameConfig().Profile[CurrentProfile].ShipHullUpgrade;
    // расчет стоимости ремонта корабля
    if (auto sharedFighter = Fighter.lock()) {
        ShipCost -= (int)(ShipCost * (sharedFighter->ArmorCurrentStatus / sharedFighter->ArmorInitialStatus));
    }
    return ShipCost;
}






int GetWorkshopShipFullCost(int Num, std::weak_ptr<cSpaceShip> &Fighter)
{
    // полная стоимость корпуса корабля с повреждениями корабля
    int ShipCost = GetWorkshopShipCost(Num) * GameConfig().Profile[CurrentProfile].ShipHullUpgrade -
                   GetWorkshopShipRepairCost(Num, Fighter);


    // прибавить стоимость оружия
    if (auto sharedWorkshopFighterGame = WorkshopFighterGame.lock()) {
        if (!sharedWorkshopFighterGame->WeaponSlots.empty()) {
            for (auto &tmpWeaponSlot : sharedWorkshopFighterGame->WeaponSlots) {
                if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                    ShipCost += GetWeaponCost(sharedWeapon->InternalType,
                                              sharedWeapon->Ammo,
                                              sharedWeapon->AmmoStart);
                }
            }
        }
    }

    // прибавить стоимость систем
    if (GameConfig().Profile[CurrentProfile].EngineSystem) {
        ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].EngineSystem);
    }
    if (GameConfig().Profile[CurrentProfile].PowerSystem) {
        ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].PowerSystem + 4);
    }
    if (GameConfig().Profile[CurrentProfile].TargetingSystem) {
        ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingSystem + 8);
    }
    if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem) {
        ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem + 12);
    }
    if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem) {
        ShipCost += GetSystemCost(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem + 16);
    }

    return ShipCost;
}









//------------------------------------------------------------------------------------
// создание корабля игрока при покупке нового корпуса
//------------------------------------------------------------------------------------
void WorkshopCreateBuyShip()
{
    // продаем все оружие и все системы
    // 1 - Системы
    int EngineSystem = GameConfig().Profile[CurrentProfile].EngineSystem;
    if (GameConfig().Profile[CurrentProfile].EngineSystem) {
        ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].EngineSystem);
    }
    ChangeGameConfig().Profile[CurrentProfile].EngineSystem = 0;

    int PowerSystem = GameConfig().Profile[CurrentProfile].PowerSystem;
    if (GameConfig().Profile[CurrentProfile].PowerSystem) {
        ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].PowerSystem+4);
    }
    ChangeGameConfig().Profile[CurrentProfile].PowerSystem = 0;

    int TargetingSystem = GameConfig().Profile[CurrentProfile].TargetingSystem;
    if (GameConfig().Profile[CurrentProfile].TargetingSystem) {
        ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingSystem+8);
    }
    ChangeGameConfig().Profile[CurrentProfile].TargetingSystem = 0;

    int TargetingMechanicSystem = GameConfig().Profile[CurrentProfile].TargetingMechanicSystem;
    if (GameConfig().Profile[CurrentProfile].TargetingMechanicSystem) {
        ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].TargetingMechanicSystem+12);
    }
    ChangeGameConfig().Profile[CurrentProfile].TargetingMechanicSystem = 0;

    int AdvancedProtectionSystem = GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem;
    if (GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem) {
        ChangeGameConfig().Profile[CurrentProfile].Money += GetSystemCost(GameConfig().Profile[CurrentProfile].AdvancedProtectionSystem+16);
    }
    ChangeGameConfig().Profile[CurrentProfile].AdvancedProtectionSystem = 0;

    // 2 - Оружие
    unsigned OldWeaponQuantity = 0;
    if (auto sharedWorkshopFighterGame = WorkshopFighterGame.lock()) {
        OldWeaponQuantity = sharedWorkshopFighterGame->WeaponSlots.size();
        for (auto &tmpWeaponSlot : sharedWorkshopFighterGame->WeaponSlots) {
            if (auto sharedWeapon = tmpWeaponSlot.Weapon.lock()) {
                ChangeGameConfig().Profile[CurrentProfile].Money += GetWeaponCost(sharedWeapon->InternalType,
                                                                                  sharedWeapon->Ammo,
                                                                                  sharedWeapon->AmmoStart);
                ReleaseWeapon(tmpWeaponSlot.Weapon);
            }
        }
    }

    // 3 - корпус
    ChangeGameConfig().Profile[CurrentProfile].Money +=
        GetWorkshopShipCost(ChangeGameConfig().Profile[CurrentProfile].ShipHull) *
        GameConfig().Profile[CurrentProfile].ShipHullUpgrade -
        GetWorkshopShipRepairCost(ChangeGameConfig().Profile[CurrentProfile].ShipHull, WorkshopFighterGame);


    // сейчас у нас все в "деньгах", покупаем корпус
    ChangeGameConfig().Profile[CurrentProfile].ShipHull = CurrentWorkshopNewFighter;
    ChangeGameConfig().Profile[CurrentProfile].Money -= GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].ShipHull);



    // создаем объект
    ReleaseSpaceShip(WorkshopFighterGame);

    int TMPGameEnemyArmorPenalty = GameEnemyArmorPenalty;
    GameEnemyArmorPenalty = 1;

    WorkshopFighterGame = CreateEarthSpaceFighter(GameConfig().Profile[CurrentProfile].ShipHull);
    auto sharedWorkshopFighterGame = WorkshopFighterGame.lock();
    if (!sharedWorkshopFighterGame) {
        return;
    }

    sharedWorkshopFighterGame->ObjectStatus = eObjectStatus::none;
    sharedWorkshopFighterGame->EngineDestroyType = true;
    sharedWorkshopFighterGame->ShowStatus = false;
    ChangeGameConfig().Profile[CurrentProfile].ArmorStatus = sharedWorkshopFighterGame->ArmorCurrentStatus;
    ChangeGameConfig().Profile[CurrentProfile].ShipHullUpgrade = 1;


    // пытаемся купить

    // 1 - Системы
    if (EngineSystem != 0 && GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(EngineSystem)) {
        ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(EngineSystem);
        ChangeGameConfig().Profile[CurrentProfile].EngineSystem = EngineSystem;
    }
    if (PowerSystem != 0 && GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(PowerSystem+4)) {
        ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(PowerSystem+4);
        ChangeGameConfig().Profile[CurrentProfile].PowerSystem = PowerSystem;
    }
    if (TargetingSystem != 0 && GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(TargetingSystem+8)) {
        ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(TargetingSystem+8);
        ChangeGameConfig().Profile[CurrentProfile].TargetingSystem = TargetingSystem;
    }
    if (TargetingMechanicSystem != 0 && GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(TargetingMechanicSystem+12)) {
        ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(TargetingMechanicSystem+12);
        ChangeGameConfig().Profile[CurrentProfile].TargetingMechanicSystem = TargetingMechanicSystem;
    }
    if (AdvancedProtectionSystem != 0 && GameConfig().Profile[CurrentProfile].Money >= GetSystemCost(AdvancedProtectionSystem+16)) {
        ChangeGameConfig().Profile[CurrentProfile].Money -= GetSystemCost(AdvancedProtectionSystem+16);
        ChangeGameConfig().Profile[CurrentProfile].AdvancedProtectionSystem = AdvancedProtectionSystem;
    }



    // покупаем оружие, если можем... нет - ставим что продали его (это сделали ранее)
    // всегда покупаем новое, т.е. с полным боекомплектом!
    for (unsigned i = 0; i < sharedWorkshopFighterGame->WeaponSlots.size(); i++) {
        if (GameConfig().Profile[CurrentProfile].Weapon[i] != 0) {
            if (GameConfig().Profile[CurrentProfile].Money >= GetWeaponBaseCost(GameConfig().Profile[CurrentProfile].Weapon[i])) {
                if (SetEarthSpaceFighterWeapon(WorkshopFighterGame, i + 1, GameConfig().Profile[CurrentProfile].Weapon[i])) {
                    if (auto sharedWeapon = sharedWorkshopFighterGame->WeaponSlots[i].Weapon.lock()) {
                        if (auto sharedFire = sharedWeapon->Fire.lock()) {
                            vw_ReleaseLight(sharedFire->Light);
                        }

                        ChangeGameConfig().Profile[CurrentProfile].Money -= GetWeaponBaseCost(GameConfig().Profile[CurrentProfile].Weapon[i]);
                        ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = sharedWeapon->AmmoStart;
                        ChangeGameConfig().Profile[CurrentProfile].WeaponSlotYAngle[i] = 0.0f;
                    }
                } else { // если поставить не смогли - не такой слот
                    ChangeGameConfig().Profile[CurrentProfile].Weapon[i] = 0;
                    ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = 0;
                }
            } else { // если не хватило денег снова купить
                ChangeGameConfig().Profile[CurrentProfile].Weapon[i] = 0;
                ChangeGameConfig().Profile[CurrentProfile].WeaponAmmo[i] = 0;
            }
        }
    }
    // если было больше слотов чем есть сейчас
    for (unsigned j = sharedWorkshopFighterGame->WeaponSlots.size(); j < OldWeaponQuantity; j++) {
        ChangeGameConfig().Profile[CurrentProfile].Weapon[j] = 0;
    }

    // создаем системы (визуальные)
    SetEarthSpaceFighterEngine(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].EngineSystem);
    SetEarthSpaceFighterArmor(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].ShipHullUpgrade - 1);

    GameEnemyArmorPenalty = TMPGameEnemyArmorPenalty;
    sharedWorkshopFighterGame->SetLocation(sVECTOR3D{1000.0f,
                                           -1000.0f - (sharedWorkshopFighterGame->Height / 2.0f + sharedWorkshopFighterGame->AABB[6].y),
                                           -(sharedWorkshopFighterGame->Length / 2.0f + sharedWorkshopFighterGame->AABB[6].z)});
    // чтобы оружие заняло свое место...
    sharedWorkshopFighterGame->UpdateWithTimeSheetList(vw_GetTimeThread(0));

    sharedWorkshopFighterGame->SetRotation(sVECTOR3D{0.0f, 150.0f, 0.0f});
}





void BuyShip()
{
    // сбрасываем особенные настройки слотов оружия
    if (auto sharedWorkshopFighterGame = WorkshopFighterGame.lock()) {
        for (unsigned i = 0; i < sharedWorkshopFighterGame->WeaponSlots.size(); i++) {
            ChangeGameConfig().Profile[CurrentProfile].WeaponSlotYAngle[i] = 0;
        }
    }

    // создаем новый корабль
    WorkshopCreateBuyShip();

    // переводим на другой корабль
    CurrentWorkshopNewFighter++;
    if (CurrentWorkshopNewFighter == GameConfig().Profile[CurrentProfile].ShipHull) {
        CurrentWorkshopNewFighter++;
    }
    if (CurrentWorkshopNewFighter > 22) {
        CurrentWorkshopNewFighter = 1;
    }
    // если это 1-й, переводим на 2-й
    if (CurrentWorkshopNewFighter == GameConfig().Profile[CurrentProfile].ShipHull) {
        CurrentWorkshopNewFighter++;
    }
    WorkshopCreateNewShip();
}






void RepairShip()
{
    auto sharedWorkshopFighterGame = WorkshopFighterGame.lock();
    if (!sharedWorkshopFighterGame) {
        return;
    }

    // смотрим, если денег достаточно для полного ремонта - делаем его
    if (GameConfig().Profile[CurrentProfile].Money >= GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].ShipHull, WorkshopFighterGame)) {
        ChangeGameConfig().Profile[CurrentProfile].Money -= GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].ShipHull, WorkshopFighterGame);
        ChangeGameConfig().Profile[CurrentProfile].ArmorStatus = sharedWorkshopFighterGame->ArmorInitialStatus;
        sharedWorkshopFighterGame->ArmorCurrentStatus = sharedWorkshopFighterGame->ArmorInitialStatus;
        return;
    }


    // вычисляем, сколько можем отремонтировать
    float ArmorRepair = sharedWorkshopFighterGame->ArmorInitialStatus - sharedWorkshopFighterGame->ArmorCurrentStatus;
    float RepairCost = static_cast<float>(GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].ShipHull, WorkshopFighterGame));
    // сколько можем отремонтировать
    float CanRepair = ArmorRepair * (GameConfig().Profile[CurrentProfile].Money / RepairCost);
    // ремонтируем сколько можем
    sharedWorkshopFighterGame->ArmorCurrentStatus += CanRepair;
    ChangeGameConfig().Profile[CurrentProfile].ArmorStatus = sharedWorkshopFighterGame->ArmorCurrentStatus;
    // это будет стоить все имеющиеся деньги
    ChangeGameConfig().Profile[CurrentProfile].Money = 0;

    // выводим диалог с надписью, что не достаточно денег для полного ремонта
    SetCurrentDialogBox(eDialogBox::RepairShip);
}





void UpgradeShip()
{
    auto sharedWorkshopFighterGame = WorkshopFighterGame.lock();
    if (!sharedWorkshopFighterGame) {
        return;
    }

    // ув. данные о базовой прочности корабля
    float OldStr = sharedWorkshopFighterGame->ArmorInitialStatus;
    sharedWorkshopFighterGame->ArmorInitialStatus /= GameConfig().Profile[CurrentProfile].ShipHullUpgrade;

    // ув. данные о номере апгрейда
    ChangeGameConfig().Profile[CurrentProfile].ShipHullUpgrade++;

    sharedWorkshopFighterGame->ArmorInitialStatus *= GameConfig().Profile[CurrentProfile].ShipHullUpgrade;
    sharedWorkshopFighterGame->ArmorCurrentStatus = sharedWorkshopFighterGame->ArmorInitialStatus - OldStr +
                                                    GameConfig().Profile[CurrentProfile].ArmorStatus;

    ChangeGameConfig().Profile[CurrentProfile].ArmorStatus = sharedWorkshopFighterGame->ArmorCurrentStatus;


    // вычитаем деньги за апгрейд
    ChangeGameConfig().Profile[CurrentProfile].Money -= GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].ShipHull);


    // сообщаем о новых возможностях и т.п. которые дает этот апгрейд
    SetEarthSpaceFighterArmor(WorkshopFighterGame, GameConfig().Profile[CurrentProfile].ShipHullUpgrade - 1);
}

















//------------------------------------------------------------------------------------
// покупка-апгрейд корпуса
//------------------------------------------------------------------------------------
void Workshop_Shipyard()
{

    // затемнение при выводе
    int SizeI;
    sRECT SrcRect, DstRect;


    // затемнение
    SrcRect(0,0,256,256 );
    DstRect(GameConfig().InternalWidth/2-480, 100-32, GameConfig().InternalWidth/2-32, 450+32);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/back_spot2.tga"), true, 0.45f * MenuContentTransp);
    DstRect(GameConfig().InternalWidth/2+32, 100-32, GameConfig().InternalWidth/2+480, 450+32);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/back_spot2.tga"), true, 0.45f * MenuContentTransp);



    vw_End2DMode();
    WorkshopDrawShip(WorkshopFighterGame, 3);
    WorkshopDrawShip(WorkshopNewFighter, 2);
    vw_Start2DMode(-1,1);


    // условия, при которых корабль купить можно
    bool CanBuy = false;
    if (GetWorkshopShipFullCost(GameConfig().Profile[CurrentProfile].ShipHull, WorkshopFighterGame)
        + GameConfig().Profile[CurrentProfile].Money >= GetWorkshopShipCost(CurrentWorkshopNewFighter)) {
        CanBuy = true;
    }


    // на новом корабле
    vw_SetFontSize(24);
    vw_DrawTextUTF32(GameConfig().InternalWidth/2-445, 600, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Ship Stock"));
    ResetFontSize();


    // вывод названия корабля
    std::ostringstream tmpStream;
    tmpStream << std::fixed << std::setprecision(0)
              << vw_GetText(GetWorkshopShipName(CurrentWorkshopNewFighter)) << " Mk" << 1;
    vw_DrawText(GameConfig().InternalWidth/2-448, 50+6, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());
    if (DrawButton128_2(GameConfig().InternalWidth/2-197,50, vw_GetTextUTF32("Info"), MenuContentTransp, false)) {
        SetCurrentDialogBox(eDialogBox::ShowShipInfo);
        DialogSpaceShip = WorkshopNewFighter;
    }

    auto sharedWorkshopNewFighter = WorkshopNewFighter.lock();
    if (!sharedWorkshopNewFighter) {
        return;
    }

    // находим смещение, чтобы было красиво
    int SmSizeI = vw_TextWidthUTF32(vw_GetTextUTF32("Armor:"));
    int SmSizeI2 = vw_TextWidthUTF32(vw_GetTextUTF32("Weapon Slots:"));
    int SmSizeI3 = vw_TextWidthUTF32(vw_GetTextUTF32("Slot Levels:"));
    SmSizeI = std::max({SmSizeI, SmSizeI2, SmSizeI3});

    vw_DrawTextUTF32(GameConfig().InternalWidth/2-440, 110, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, vw_GetTextUTF32("Armor:"));
    tmpStream.clear();
    tmpStream.str(std::string{});
    tmpStream << sharedWorkshopNewFighter->ArmorInitialStatus;
    vw_DrawText(GameConfig().InternalWidth/2-440+14+SmSizeI, 110, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, tmpStream.str());

    vw_DrawTextUTF32(GameConfig().InternalWidth/2-440, 130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, vw_GetTextUTF32("Weapon Slots:"));
    tmpStream.clear();
    tmpStream.str(std::string{});
    tmpStream << sharedWorkshopNewFighter->WeaponSlots.size();
    vw_DrawText(GameConfig().InternalWidth/2-440+14+SmSizeI, 130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, tmpStream.str());

    vw_DrawTextUTF32(GameConfig().InternalWidth/2-440, 150, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, vw_GetTextUTF32("Slot Levels:"));
    tmpStream.clear();
    tmpStream.str(std::string{});
    if (!sharedWorkshopNewFighter->WeaponSlots.empty()) {
        tmpStream << sharedWorkshopNewFighter->WeaponSlots[0].Type;
    }
    for (unsigned i = 1; i < sharedWorkshopNewFighter->WeaponSlots.size(); i++) {
        tmpStream << "/" << sharedWorkshopNewFighter->WeaponSlots[i].Type;
    }
    vw_DrawText(GameConfig().InternalWidth/2-440+14+SmSizeI, 150, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, tmpStream.str());

    // вывод стоимости корабля
    float tmpTransp{MenuContentTransp};
    sRGBCOLOR tmpColor{eRGBCOLOR::white};
    if (!CanBuy) {
        tmpTransp = MenuContentTransp * CurrentAlert3;
        tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
    }
    tmpStream.clear();
    tmpStream.str(std::string{});
    tmpStream << vw_GetText("Ship Hull Cost") << ": " << GetWorkshopShipCost(CurrentWorkshopNewFighter);
    vw_DrawText(GameConfig().InternalWidth/2-438, 420, 0, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());


    // рамки
    SrcRect(0,0,400,35 );
    DstRect(GameConfig().InternalWidth/2-457, 100-11, GameConfig().InternalWidth/2-57, 100+35-11);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel4.tga"), true, MenuContentTransp);

    SrcRect(0,0,400,173 );
    DstRect(GameConfig().InternalWidth/2-457, 450-13, GameConfig().InternalWidth/2-57, 450+173-13);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel1.tga"), true, MenuContentTransp);


    // проверяем колесо мышки
    DstRect(GameConfig().InternalWidth/2-457, 100+35-11, GameConfig().InternalWidth/2-57, 450-13);
    if (vw_MouseOverRect(DstRect)) {
        if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing()) {
            CurrentWorkshopNewFighter += vw_GetWheelStatus();

            if (CurrentWorkshopNewFighter < 1) {
                CurrentWorkshopNewFighter = 22;
            }
            if (CurrentWorkshopNewFighter > 22) {
                CurrentWorkshopNewFighter = 1;
            }

            WorkshopCreateNewShip();

            vw_ResetWheelStatus();
        }
    } else if (vw_GetWheelStatus() != 0) {
        vw_ResetWheelStatus();
    }

    if (DrawButton128_2(GameConfig().InternalWidth/2-395, 482, vw_GetTextUTF32("Prev"), MenuContentTransp, false)) {
        CurrentWorkshopNewFighter--;
        if (CurrentWorkshopNewFighter < 1) {
            CurrentWorkshopNewFighter = 22;
        }
        WorkshopCreateNewShip();
    }
    if (DrawButton128_2(GameConfig().InternalWidth/2-247, 482, vw_GetTextUTF32("Next"), MenuContentTransp, false)) {
        CurrentWorkshopNewFighter++;
        if (CurrentWorkshopNewFighter > 22) {
            CurrentWorkshopNewFighter = 1;
        }
        WorkshopCreateNewShip();
    }
    if (DrawButton200_2(GameConfig().InternalWidth/2-357, 533, vw_GetTextUTF32("Buy Ship"), MenuContentTransp, !CanBuy)) {
        BuyShip();
    }








    // на корабле игрока
    vw_SetFontSize(24);
    vw_DrawTextUTF32(GameConfig().InternalWidth/2+445-vw_TextWidthUTF32(vw_GetTextUTF32("Player Ship")), 600, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, vw_GetTextUTF32("Player Ship"));
    ResetFontSize();

    // вывод названия корабля
    tmpStream.clear();
    tmpStream.str(std::string{});
    tmpStream << vw_GetText(GetWorkshopShipName(GameConfig().Profile[CurrentProfile].ShipHull))
              << " Mk" << static_cast<int>(GameConfig().Profile[CurrentProfile].ShipHullUpgrade);
    vw_DrawText(GameConfig().InternalWidth/2+64, 56, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::yellow}, MenuContentTransp, tmpStream.str());

    if (DrawButton128_2(GameConfig().InternalWidth/2+315, 50, vw_GetTextUTF32("Info"), MenuContentTransp, false)) {
        SetCurrentDialogBox(eDialogBox::ShowShipInfo);
        DialogSpaceShip = WorkshopFighterGame;
    }



    auto sharedWorkshopFighterGame = WorkshopFighterGame.lock();
    if (!sharedWorkshopFighterGame) {
        return;
    }


    int LinePos = 420;

    // вывод стомости модернизации
    // модернизация корпуса
    bool CanUpgrade = false;
    if (GameConfig().Profile[CurrentProfile].ShipHullUpgrade < 4) {
        CanUpgrade = true;
    }

    if (CanUpgrade) {
        if (GameConfig().Profile[CurrentProfile].Money < GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].ShipHull)) {
            CanUpgrade = false;
        }

        tmpTransp = MenuContentTransp;
        tmpColor = sRGBCOLOR{eRGBCOLOR::white};
        tmpStream.clear();
        tmpStream.str(std::string{});
        if (!CanUpgrade) {
            tmpTransp = MenuContentTransp * CurrentAlert3;
            tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
        }
        tmpStream << vw_GetText("Upgrade Hull Cost") << ": " << GetWorkshopShipCost(GameConfig().Profile[CurrentProfile].ShipHull);
        vw_DrawText(GameConfig().InternalWidth/2+438-vw_TextWidth(tmpStream.str()), LinePos, 0, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

        LinePos -= 20;
    }


    // вывод стомости ремонта, если нужно
    bool NeedRepair = false;
    if (GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].ShipHull, WorkshopFighterGame) > 0) {
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream <<  vw_GetText("Repair Hull Cost") << ": "
                  << GetWorkshopShipRepairCost(GameConfig().Profile[CurrentProfile].ShipHull, WorkshopFighterGame);
        SizeI = vw_TextWidth(tmpStream.str());
        vw_DrawText(GameConfig().InternalWidth/2+438-SizeI, LinePos, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3*MenuContentTransp, tmpStream.str());
        LinePos -= 20;
        NeedRepair = true;

        // надпись Armor, красная
        vw_DrawTextUTF32(GameConfig().InternalWidth/2+74, 110, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, vw_GetTextUTF32("Armor:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << static_cast<int>(sharedWorkshopFighterGame->ArmorCurrentStatus) << "/"
                  << static_cast<int>(sharedWorkshopFighterGame->ArmorInitialStatus);
        vw_DrawText(GameConfig().InternalWidth/2+74+14+SmSizeI, 110, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::red}, CurrentAlert3*MenuContentTransp, tmpStream.str());
    } else {
        // надпись Armor, нормальная
        vw_DrawTextUTF32(GameConfig().InternalWidth/2+74, 110, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, vw_GetTextUTF32("Armor:"));
        tmpStream.clear();
        tmpStream.str(std::string{});
        tmpStream << static_cast<int>(sharedWorkshopFighterGame->ArmorCurrentStatus) << "/"
                  << static_cast<int>(sharedWorkshopFighterGame->ArmorInitialStatus);
        vw_DrawText(GameConfig().InternalWidth/2+74+14+SmSizeI, 110, 0, 0,1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, tmpStream.str());
    }

    vw_DrawTextUTF32(GameConfig().InternalWidth/2+74, 130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, vw_GetTextUTF32("Weapon Slots:"));
    tmpStream.clear();
    tmpStream.str(std::string{});
    tmpStream << sharedWorkshopFighterGame->WeaponSlots.size();
    vw_DrawText(GameConfig().InternalWidth/2+74+14+SmSizeI, 130, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, tmpStream.str());


    vw_DrawTextUTF32(GameConfig().InternalWidth/2+74, 150, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, vw_GetTextUTF32("Slot Levels:"));
    tmpStream.clear();
    tmpStream.str(std::string{});
    if (!sharedWorkshopFighterGame->WeaponSlots.empty()) {
        tmpStream << sharedWorkshopFighterGame->WeaponSlots[0].Type;
    }
    for (unsigned i = 1; i < sharedWorkshopFighterGame->WeaponSlots.size(); i++) {
        tmpStream << "/" << sharedWorkshopFighterGame->WeaponSlots[i].Type;
    }
    vw_DrawText(GameConfig().InternalWidth/2+74+14+SmSizeI, 150, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, 0.5f*MenuContentTransp, tmpStream.str());




    // вывод стоимости корабля (со всеми системами)
    tmpStream.clear();
    tmpStream.str(std::string{});
    tmpStream << vw_GetText("Ship Cost") << ": "
              << GetWorkshopShipFullCost(GameConfig().Profile[CurrentProfile].ShipHull, WorkshopFighterGame);
    SizeI = vw_TextWidth(tmpStream.str());
    vw_DrawText(GameConfig().InternalWidth/2+438-SizeI, LinePos, 0, 0, 1.0f, sRGBCOLOR{eRGBCOLOR::white}, MenuContentTransp, tmpStream.str());


    // рамки
    SrcRect(0,0,400,35 );
    DstRect(GameConfig().InternalWidth/2+57, 100-11, GameConfig().InternalWidth/2+457, 100+35-11);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel4.tga"), true, MenuContentTransp);

    SrcRect(0,0,400,173 );
    DstRect(GameConfig().InternalWidth/2+57, 450-13, GameConfig().InternalWidth/2+457, 450+173-13);
    vw_Draw2D(DstRect, SrcRect, GetPreloadedTextureAsset("menu/workshop_panel1+.tga"), true, MenuContentTransp);



    if (DrawButton200_2(GameConfig().InternalWidth/2+153, 482, vw_GetTextUTF32("Repair Hull"), MenuContentTransp, !NeedRepair)) {
        RepairShip();
    }

    if (DrawButton200_2(GameConfig().InternalWidth/2+153, 533, vw_GetTextUTF32("Upgrade Hull"), MenuContentTransp, !CanUpgrade)) {
        UpgradeShip();
    }



    // вывод информации
    vw_SetFontSize(20);

    tmpTransp = MenuContentTransp;
    tmpColor = sRGBCOLOR{eRGBCOLOR::yellow};
    tmpStream.clear();
    tmpStream.str(std::string{});
    tmpStream << vw_GetText("Money") << ": "
              << GameConfig().Profile[CurrentProfile].Money;
    SizeI = (GameConfig().InternalWidth - vw_TextWidth(tmpStream.str())) / 2;
    if (!CanBuy) {
        tmpTransp = MenuContentTransp * CurrentAlert3;
        tmpColor = sRGBCOLOR{eRGBCOLOR::orange};
    }
    vw_DrawText(SizeI, 630, 0, 0, 1.0f, tmpColor, tmpTransp, tmpStream.str());

    ResetFontSize();
}

} // astromenace namespace
} // viewizard namespace
