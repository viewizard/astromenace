/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Difficulty.cpp

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


char *OnOff[2] =
{"1_On",
"1_Off"};


char *ArcadeSim[2] =
{"1_Sim",
"1_Arcade"};


void DifficultyMenu()
{
	RECT SrcRest, DstRest;
	SetRect(&SrcRest,2,2,863-2,484-2);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-427,175-15,Setup.iAspectRatioWidth/2-427+863-4,175-15+484-4);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/panel800_444_back.tga"), true, 0.9f*MenuContentTransp);


	int CurrentPos = 0;

	int X1 = Setup.iAspectRatioWidth/2 - 372;
	int Y1 = 217;
	int Prir1 = 50;
	int Size;
	int SizeI;



	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Enemy_Weapon_Penalty"));
	bool ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCWeaponPenalty == 1) ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, GetText("1_Decrease"), MenuContentTransp, ButOff))
	{
		Setup.Profile[CurrentProfile].NPCWeaponPenalty--;
		if (Setup.Profile[CurrentProfile].NPCWeaponPenalty < 1) Setup.Profile[CurrentProfile].NPCWeaponPenalty = 1;
	}
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCWeaponPenalty == 3) ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, ButOff))
	{
		Setup.Profile[CurrentProfile].NPCWeaponPenalty++;
		if (Setup.Profile[CurrentProfile].NPCWeaponPenalty > 3) Setup.Profile[CurrentProfile].NPCWeaponPenalty = 3;
	}
	if (Setup.Profile[CurrentProfile].NPCWeaponPenalty == 1)
	{
		Size = FontSize(GetText("3_None"));
		SizeI = (110-Size)/2;
		DrawFont(X1+498+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText("3_None"));
	}
	else
	{
		Size = FontSize("x%i", Setup.Profile[CurrentProfile].NPCWeaponPenalty);
		SizeI = (110-Size)/2;
		DrawFont(X1+498+SizeI, Y1, 0, 0, 0, MenuContentTransp, "x%i", Setup.Profile[CurrentProfile].NPCWeaponPenalty);
	}




	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Enemy_Armor_Penalty"));
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCArmorPenalty == 1) ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, GetText("1_Decrease"), MenuContentTransp, ButOff))
	{
		Setup.Profile[CurrentProfile].NPCArmorPenalty--;
		if (Setup.Profile[CurrentProfile].NPCArmorPenalty < 1) Setup.Profile[CurrentProfile].NPCArmorPenalty = 1;
	}
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCArmorPenalty == 4) ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, ButOff))
	{
		Setup.Profile[CurrentProfile].NPCArmorPenalty++;
		if (Setup.Profile[CurrentProfile].NPCArmorPenalty > 4) Setup.Profile[CurrentProfile].NPCArmorPenalty = 4;
	}
	if (Setup.Profile[CurrentProfile].NPCArmorPenalty == 1)
	{
		Size = FontSize(GetText("3_None"));
		SizeI = (110-Size)/2;
		DrawFont(X1+498+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText("3_None"));
	}
	else
	{
		Size = FontSize("x%i", Setup.Profile[CurrentProfile].NPCArmorPenalty);
		SizeI = (110-Size)/2;
		DrawFont(X1+498+SizeI, Y1, 0, 0, 0, MenuContentTransp, "x%i", Setup.Profile[CurrentProfile].NPCArmorPenalty);
	}




	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Enemy_Targeting_Penalty"));
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty == 1) ButOff = true;
	if (DrawButton128_2(X1+360, Y1-6, GetText("1_Decrease"), MenuContentTransp, ButOff))
	{
		Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty--;
		if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty < 1) Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty = 1;
	}
	ButOff = false;
	if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty == 4) ButOff = true;
	if (DrawButton128_2(X1+616, Y1-6, GetText("1_Increase"), MenuContentTransp, ButOff))
	{
		Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty++;
		if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty > 4) Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty = 4;
	}
	if (Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty == 1)
	{
		Size = FontSize(GetText("3_None"));
		SizeI = (110-Size)/2;
		DrawFont(X1+498+SizeI, Y1, 0, 0, 0, MenuContentTransp, GetText("3_None"));
	}
	else
	{
		Size = FontSize("x%i", Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty);
		SizeI = (110-Size)/2;
		DrawFont(X1+498+SizeI, Y1, 0, 0, 0, MenuContentTransp, "x%i", Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty);
	}




	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Limited_Ammo"));
	if (DrawButton128_2(X1+360+128, Y1-6, GetText(OnOff[Setup.Profile[CurrentProfile].LimitedAmmo]), MenuContentTransp, false))
	{
		Setup.Profile[CurrentProfile].LimitedAmmo++;
		if (Setup.Profile[CurrentProfile].LimitedAmmo > 1) Setup.Profile[CurrentProfile].LimitedAmmo = 0;
	}




	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Destroyable_Weapon"));
	if (DrawButton128_2(X1+360+128, Y1-6, GetText(OnOff[Setup.Profile[CurrentProfile].DestroyableWeapon]), MenuContentTransp, false))
	{
		Setup.Profile[CurrentProfile].DestroyableWeapon++;
		if (Setup.Profile[CurrentProfile].DestroyableWeapon > 1) Setup.Profile[CurrentProfile].DestroyableWeapon = 0;
	}





	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_Weapon_Targeting_Mode"));
	if (DrawButton128_2(X1+360+128, Y1-6, GetText(ArcadeSim[Setup.Profile[CurrentProfile].WeaponTargetingMode]), MenuContentTransp, false))
	{
		Setup.Profile[CurrentProfile].WeaponTargetingMode++;
		if (Setup.Profile[CurrentProfile].WeaponTargetingMode > 1) Setup.Profile[CurrentProfile].WeaponTargetingMode = 0;
	}




	Y1 += Prir1;
	DrawFont(X1, Y1, 0, 0, 0, MenuContentTransp, GetText("3_SpaceShip_Control_Mode"));
	if (DrawButton128_2(X1+360+128, Y1-6, GetText(ArcadeSim[Setup.Profile[CurrentProfile].SpaceShipControlMode]), MenuContentTransp, false))
	{
		Setup.Profile[CurrentProfile].SpaceShipControlMode++;
		if (Setup.Profile[CurrentProfile].SpaceShipControlMode > 1) Setup.Profile[CurrentProfile].SpaceShipControlMode = 0;
	}





	// считаем общую сложность
	Setup.Profile[CurrentProfile].Difficulty = 100-( (Setup.Profile[CurrentProfile].NPCWeaponPenalty-1)*6+
		(Setup.Profile[CurrentProfile].NPCArmorPenalty-1)*6+
		(Setup.Profile[CurrentProfile].NPCTargetingSpeedPenalty-1)*6+
		Setup.Profile[CurrentProfile].LimitedAmmo*14+
		Setup.Profile[CurrentProfile].DestroyableWeapon*11+
		Setup.Profile[CurrentProfile].WeaponTargetingMode*12+
		Setup.Profile[CurrentProfile].SpaceShipControlMode*15);



	Y1 += Prir1;
	Size = FontSize("%s: %i%%", GetText("3_Current_Profile_Difficulty"), Setup.Profile[CurrentProfile].Difficulty);
	SizeI = (Setup.iAspectRatioWidth-Size)/2;
	DrawFont(SizeI, Y1, 0, 0, 1, MenuContentTransp, "%s: %i%%", GetText("3_Current_Profile_Difficulty"), Setup.Profile[CurrentProfile].Difficulty);








	int X = Setup.iAspectRatioWidth/2 - 192;
	int Y = 175+95*5;
	if (DrawButton384(X,Y, GetText("1_BACK"), MenuContentTransp, &Button1Transp, &LastButton1UpdateTime))
	{
		ComBuffer = PROFILE;
	}




}
