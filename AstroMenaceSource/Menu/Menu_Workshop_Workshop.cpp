/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Workshop_Workshop.cpp

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



extern CEarthSpaceFighter *WorkshopFighterGame;
extern float CurrentAlert2;
extern float CurrentAlert3;

void WorkshopDrawShip(CEarthSpaceFighter *SpaceFighter, int Mode);

float GetShipEngineSystemEnergyUse(int Num);
float GetShipProtectionSystemEnergyUse(int Num);
float GetShipRechargeEnergy(int Num);


// проблемы с энергией
extern bool NeedMoreEnergyDialog;
// Номер, для проигрывания голосового сообщения об проблемы с энергией
int VoiceNeedMoreEnergy = 0;


// что рисовать в диалоге 6,7,8
extern int DialogSystem;


// текущий номер системы в мастерской
int CurrentSystemStockNum = 1;




int GetNextSystemStockNum()
{
	int Res = CurrentSystemStockNum + 1;

	if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4) if (Res > 4) Res = 1;
	if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8) if (Res > 8) Res = 5;
	if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12) if (Res > 12) Res = 9;
	if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16) if (Res > 16) Res = 13;
	if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20) if (Res > 20) Res = 17;

//	if (Res > 20) Res = 1;
	return Res;
}
int GetNextSystemStockGroup()
{
	if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4) return 5;
	if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8) return 9;
	if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12) return 13;
	if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16) return 17;
	if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20) return 1;
	return 0;
}
int GetPrevSystemStockNum()
{
	int Res = CurrentSystemStockNum - 1;
//	if (Res < 1) Res = 20;
	if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4) if (Res < 1) Res = 4;
	if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8) if (Res < 5) Res = 8;
	if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12) if (Res < 9) Res = 12;
	if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16) if (Res < 13) Res = 16;
	if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20) if (Res < 17) Res = 20;

	return Res;
}
int GetPrevSystemStockGroup()
{
	if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4) return 17;
	if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8) return 1;
	if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12) return 5;
	if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16) return 9;
	if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20) return 13;
	return 0;
}



const char *GetSystemName(int Num)
{
	switch (Num)
	{
		case 1:		return "7_system1";
		case 2:		return "7_system2";
		case 3:		return "7_system3";
		case 4:		return "7_system4";

		case 5:		return "7_system5";
		case 6:		return "7_system6";
		case 7:		return "7_system7";
		case 8:		return "7_system8";

		case 9:		return "7_system9";
		case 10:	return "7_system10";
		case 11:	return "7_system11";
		case 12:	return "7_system12";

		case 13:	return "7_system13";
		case 14:	return "7_system14";
		case 15:	return "7_system15";
		case 16:	return "7_system16";

		case 17:	return "7_system17";
		case 18:	return "7_system18";
		case 19:	return "7_system19";
		case 20:	return "7_system20";
	}

	return 0;
}




eTexture * GetSystemIcon(int Num)
{
	switch (Num)
	{
		case -4:	return vw_FindTextureByName("DATA/MENU/system_engine_empty.tga");
		case -3:	return vw_FindTextureByName("DATA/MENU/system_mechan_empty.tga");
		case -2:	return vw_FindTextureByName("DATA/MENU/system_power_empty.tga");
		case -1:	return vw_FindTextureByName("DATA/MENU/system_target_empty.tga");
		case 0:		return vw_FindTextureByName("DATA/MENU/system_protect_empty.tga");

		case 1:		return vw_FindTextureByName("DATA/MENU/system_engine1.tga");
		case 2:		return vw_FindTextureByName("DATA/MENU/system_engine2.tga");
		case 3:		return vw_FindTextureByName("DATA/MENU/system_engine3.tga");
		case 4:		return vw_FindTextureByName("DATA/MENU/system_engine4.tga");

		case 5:		return vw_FindTextureByName("DATA/MENU/system_power1.tga");
		case 6:		return vw_FindTextureByName("DATA/MENU/system_power2.tga");
		case 7:		return vw_FindTextureByName("DATA/MENU/system_power3.tga");
		case 8:		return vw_FindTextureByName("DATA/MENU/system_power4.tga");

		case 9:		return vw_FindTextureByName("DATA/MENU/system_target1.tga");
		case 10:	return vw_FindTextureByName("DATA/MENU/system_target2.tga");
		case 11:	return vw_FindTextureByName("DATA/MENU/system_target3.tga");
		case 12:	return vw_FindTextureByName("DATA/MENU/system_target4.tga");

		case 13:	return vw_FindTextureByName("DATA/MENU/system_mechan1.tga");
		case 14:	return vw_FindTextureByName("DATA/MENU/system_mechan2.tga");
		case 15:	return vw_FindTextureByName("DATA/MENU/system_mechan3.tga");
		case 16:	return vw_FindTextureByName("DATA/MENU/system_mechan4.tga");

		case 17:	return vw_FindTextureByName("DATA/MENU/system_protect1.tga");
		case 18:	return vw_FindTextureByName("DATA/MENU/system_protect2.tga");
		case 19:	return vw_FindTextureByName("DATA/MENU/system_protect3.tga");
		case 20:	return vw_FindTextureByName("DATA/MENU/system_protect4.tga");
	}

	return 0;
}



int GetSystemCost(int Num)
{
	switch (Num)
	{
		case 1:		return 200;
		case 2:		return 1500;
		case 3:		return 3000;
		case 4:		return 6000;

		case 5:		return 250;
		case 6:		return 2000;
		case 7:		return 7000;
		case 8:		return 15000;

		case 9:		return 100;
		case 10:	return 1000;
		case 11:	return 3000;
		case 12:	return 6000;

		case 13:	return 150;
		case 14:	return 1000;
		case 15:	return 3000;
		case 16:	return 7000;

		case 17:	return 2500;
		case 18:	return 6000;
		case 19:	return 10000;
		case 20:	return 15000;
	}

	return 0;
}





void SellCurrentSystem()
{
	if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4)
	{
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].EngineSystem);
		Setup.Profile[CurrentProfile].EngineSystem = 0;
		SetEarthSpaceFighterEngine(WorkshopFighterGame, Setup.Profile[CurrentProfile].EngineSystem);
	}
	if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8)
	{
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].PowerSystem+4);
		Setup.Profile[CurrentProfile].PowerSystem = 0;
	}
	if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12)
	{
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].TargetingSystem+8);
		Setup.Profile[CurrentProfile].TargetingSystem = 0;
	}
	if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16)
	{
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].TargetingMechanicSystem+12);
		Setup.Profile[CurrentProfile].TargetingMechanicSystem = 0;
	}
	if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20)
	{
		Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].AdvancedProtectionSystem+16);
		Setup.Profile[CurrentProfile].AdvancedProtectionSystem = 0;

		SetEarthSpaceFighterArmour(WorkshopFighterGame, Setup.Profile[CurrentProfile].ShipHullUpgrade-1);
	}
}


void BuyCurrentSystem()
{
	if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4)
	{
		if (Setup.Profile[CurrentProfile].EngineSystem != 0)
			Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].EngineSystem);
		Setup.Profile[CurrentProfile].EngineSystem = CurrentSystemStockNum;
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(Setup.Profile[CurrentProfile].EngineSystem);

		SetEarthSpaceFighterEngine(WorkshopFighterGame, Setup.Profile[CurrentProfile].EngineSystem);
	}
	if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8)
	{
		if (Setup.Profile[CurrentProfile].PowerSystem != 0)
			Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].PowerSystem+4);
		Setup.Profile[CurrentProfile].PowerSystem = CurrentSystemStockNum-4;
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(Setup.Profile[CurrentProfile].PowerSystem+4);
	}
	if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12)
	{
		if (Setup.Profile[CurrentProfile].TargetingSystem != 0)
			Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].TargetingSystem+8);
		Setup.Profile[CurrentProfile].TargetingSystem = CurrentSystemStockNum-8;
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(Setup.Profile[CurrentProfile].TargetingSystem+8);
	}
	if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16)
	{
		if (Setup.Profile[CurrentProfile].TargetingMechanicSystem != 0)
			Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].TargetingMechanicSystem+12);
		Setup.Profile[CurrentProfile].TargetingMechanicSystem = CurrentSystemStockNum-12;
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(Setup.Profile[CurrentProfile].TargetingMechanicSystem+12);
	}
	if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20)
	{
		if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem != 0)
			Setup.Profile[CurrentProfile].Money += GetSystemCost(Setup.Profile[CurrentProfile].AdvancedProtectionSystem+16);
		Setup.Profile[CurrentProfile].AdvancedProtectionSystem = CurrentSystemStockNum-16;
		Setup.Profile[CurrentProfile].Money -= GetSystemCost(Setup.Profile[CurrentProfile].AdvancedProtectionSystem+16);

		if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem == 2)
			SetEarthSpaceFighterArmour(WorkshopFighterGame, 7);
		else
			SetEarthSpaceFighterArmour(WorkshopFighterGame, Setup.Profile[CurrentProfile].ShipHullUpgrade-1);
	}
}














// для кнопок, чтобы проигрывать наведение
// последние координаты кнопки на которую наводились
int NeedPlayWorkshopOnButtonSoundX = 0;
int NeedPlayWorkshopOnButtonSoundY = 0;




//------------------------------------------------------------------------------------
// покупка-установка систем корабля
//------------------------------------------------------------------------------------
void Workshop_Workshop()
{
	RECT SrcRest, DstRest;


	SetRect(&SrcRest,0,0,256,256);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-256,0,Setup.iAspectRatioWidth/2-256+512,412);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/back_spot.tga"), true, 0.55f*MenuContentTransp);



	vw_End2DMode();
	WorkshopDrawShip(WorkshopFighterGame, 1);
	vw_Start2DMode(-1,1);


	SetRect(&SrcRest,0,0,210,600);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-492,50-10,Setup.iAspectRatioWidth/2-492+210,50+600-10);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/workshop_panel2.tga"), true, MenuContentTransp);



	// стрелки

	SetRect(&SrcRest,0,0,74,120);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-461,494,Setup.iAspectRatioWidth/2-461+74,494+120);
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);
	if  (((DstRest.right-8  >= MouseX)&
		(DstRest.left+8<= MouseX)&
		(DstRest.bottom-10 >= MouseY)&
		(DstRest.top+10<= MouseY))  && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
			CurrentSystemStockNum = GetPrevSystemStockNum();
		}

		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrows_on.tga"), true, MenuContentTransp);
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}

		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrows_off.tga"), true, MenuContentTransp);
	}

	SetRect(&SrcRest,74,0,148,120);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-461+74,494,Setup.iAspectRatioWidth/2-461+148,494+120);
	if  (((DstRest.right-8  >= MouseX)&
		(DstRest.left+8<= MouseX)&
		(DstRest.bottom-10 >= MouseY)&
		(DstRest.top+10<= MouseY))  && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
			CurrentSystemStockNum = GetNextSystemStockNum();
		}

		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrows_on.tga"), true, MenuContentTransp);
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}

		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/arrows_off.tga"), true, MenuContentTransp);
	}





	// выводим иконки систем в магазине, по которым тоже можно кликать

	SetRect(&SrcRest,0,0,128,128);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-436,50+20+5-5,Setup.iAspectRatioWidth/2-338,50+128-20+5+5);
	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			CurrentSystemStockNum = GetPrevSystemStockNum();
			Audio_PlayMenuSound(2,1.0f);
		}
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(GetPrevSystemStockNum()), true, MenuContentTransp);
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}

		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(GetPrevSystemStockNum()), true, 0.35f*MenuContentTransp);
	}



	SetRect(&SrcRest,0,0,128,128);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-451,55+128+20,Setup.iAspectRatioWidth/2-451+128,55+128+128+20);
	vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(CurrentSystemStockNum), true, MenuContentTransp);
	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing()) CurrentCursorStatus = 1;




	SetRect(&SrcRest,0,0,128,128);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-451+20-5,60+20+256+40-5-5,Setup.iAspectRatioWidth/2-451+128-20+5,60-20+128+256+40-5 +5);
	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
			CurrentSystemStockNum = GetNextSystemStockNum();
		}
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(GetNextSystemStockNum()), true, MenuContentTransp);
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}

		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(GetNextSystemStockNum()), true, 0.35f*MenuContentTransp);
	}

	DrawFontX2(Setup.iAspectRatioWidth/2-482, 630, 0, 0, 0, MenuContentTransp, GetText("7_System_Stock"));














	SetRect(&SrcRest,0,0,210,600);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+282,50-10,Setup.iAspectRatioWidth/2+492,50+600-10);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/workshop_panel2+.tga"), true, MenuContentTransp);



	// проверяем, а все ли нормально с энергией... если не нормально, будем моргать и выведем соотв. надпись
	bool NeedMoreEnergy = false;

	if (Setup.Profile[CurrentProfile].PowerSystem != 0)
	{

		float Have = GetShipRechargeEnergy(Setup.Profile[CurrentProfile].PowerSystem);

		float Need = 0.0f;

		// если это не аркадный режим, нужно учитывать двигатель
		if (Setup.Profile[CurrentProfile].SpaceShipControlMode != 1)
			if (Setup.Profile[CurrentProfile].EngineSystem != 0)
				Need += GetShipEngineSystemEnergyUse(Setup.Profile[CurrentProfile].EngineSystem);
		// если есть спец система, ее нужно тоже учитывать
		if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem != 0)
			Need += GetShipProtectionSystemEnergyUse(Setup.Profile[CurrentProfile].AdvancedProtectionSystem);

		// слабый реактор
		if (Need >= Have) NeedMoreEnergy = true;

	}
	else
	{
		// у нас вообще нет реактора... караул!!!
		NeedMoreEnergy = true;
	}

	// выводим информационную надпись + голосовое сообщение
	if (NeedMoreEnergy)
	{
		// запускаем голос...
		if (vw_FindSoundByNum(VoiceNeedMoreEnergy) == 0)
		{
			// уже не играем, нужно запустить опять
			VoiceNeedMoreEnergy = Audio_PlayMenuSound(20, 1.0f);
		}

		// вывод текста
		int SizeI = (Setup.iAspectRatioWidth-FontSize(GetText("9_14Line1")))/2;
		DrawFont(SizeI, 60, 0, 0, 2, CurrentAlert3*MenuContentTransp, GetText("9_14Line1"));

		SizeI = (Setup.iAspectRatioWidth-FontSize(GetText("9_14Line2")))/2;
		DrawFont(SizeI, 80, 0, 0, 2, CurrentAlert3*MenuContentTransp, GetText("9_14Line2"));

		SizeI = (Setup.iAspectRatioWidth-FontSize(GetText("9_14Line3")))/2;
		DrawFont(SizeI, 100, 0, 0, 2, CurrentAlert3*MenuContentTransp, GetText("9_14Line3"));
	}




	// сбор данных
	float Current = 0.5f;
	int Cost = 0;
	bool CanSell = false;
	bool CanBuy = true;

	// Engine
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,70,Setup.iAspectRatioWidth/2+437,70+100);

	if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4)
	{
		if (Setup.Profile[CurrentProfile].EngineSystem != 0)
		{
			CanSell = true;
			Cost = GetSystemCost(Setup.Profile[CurrentProfile].EngineSystem);
			if (Cost + Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
		else
		{
			if (Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}


	// Power
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,180,Setup.iAspectRatioWidth/2+437,180+100);

	if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8)
	{
		if (Setup.Profile[CurrentProfile].PowerSystem != 0)
		{
			CanSell = true;
			Cost = GetSystemCost(Setup.Profile[CurrentProfile].PowerSystem+4);
			if (Cost + Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
		else
		{
			if (Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}

	// Target
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,290,Setup.iAspectRatioWidth/2+437,290+100);

	if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12)
	{
		if (Setup.Profile[CurrentProfile].TargetingSystem != 0)
		{
			CanSell = true;
			Cost = GetSystemCost(Setup.Profile[CurrentProfile].TargetingSystem+8);
			if (Cost + Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
		else
		{
			if (Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}


	// Mech
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,400,Setup.iAspectRatioWidth/2+437,400+100);
	if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16)
	{
		if (Setup.Profile[CurrentProfile].TargetingMechanicSystem != 0)
		{
			CanSell = true;
			Cost = GetSystemCost(Setup.Profile[CurrentProfile].TargetingMechanicSystem+12);
			if (Cost + Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
		else
		{
			if (Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}

	// Protect
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,510,Setup.iAspectRatioWidth/2+437,510+100);
	if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20)
	{
		if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem != 0)
		{
			CanSell = true;
			Cost = GetSystemCost(Setup.Profile[CurrentProfile].AdvancedProtectionSystem+16);
			if (Cost + Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
		else
		{
			if (Setup.Profile[CurrentProfile].Money < GetSystemCost(CurrentSystemStockNum))
				CanBuy = false;
		}
	}



	// прорисовка


	// Engine
	SetRect(&SrcRest,0,0,128,128);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,70,Setup.iAspectRatioWidth/2+437,70+100);
	Current = 0.5f;

	if (NeedMoreEnergy &&
			Setup.Profile[CurrentProfile].EngineSystem != 0 &&
			Setup.Profile[CurrentProfile].SpaceShipControlMode != 1)
	{
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
				true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.0f, 0.0f);
	}


	if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4)
	{
		//SetRect(&DstRest,1024-75-100 -14,70 -14,1024-75 +14,70+100 +14);
		if (!NeedMoreEnergy || Setup.Profile[CurrentProfile].EngineSystem == 0 ||
			Setup.Profile[CurrentProfile].SpaceShipControlMode == 1)
		{
			if (CanBuy)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
					true, MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
					true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.5f, 0.0f);
		}
		Current = 1.0f;
	}

	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			CurrentSystemStockNum = 1;
			Audio_PlayMenuSound(2,1.0f);
		}
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (Setup.Profile[CurrentProfile].EngineSystem == 0)
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(-4), true, Current*MenuContentTransp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(Setup.Profile[CurrentProfile].EngineSystem), true, Current*MenuContentTransp);



	// Power
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,180,Setup.iAspectRatioWidth/2+437,180+100);


	if (NeedMoreEnergy)
	{
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
				true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.0f, 0.0f);
	}

	Current = 0.5f;
	if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8)
	{
		//SetRect(&DstRest,1024-75-100 -14,180 -14,1024-75 +14,180+100 +14);
		if (!NeedMoreEnergy)
		{
			if (CanBuy)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
					true, MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
					true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.5f, 0.0f);
		}
		Current = 1.0f;
	}

	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			CurrentSystemStockNum = 5;
			Audio_PlayMenuSound(2,1.0f);
		}
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (Setup.Profile[CurrentProfile].PowerSystem == 0)
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(-2), true, Current*MenuContentTransp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(Setup.Profile[CurrentProfile].PowerSystem+4), true, Current*MenuContentTransp);




	// Target
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,290,Setup.iAspectRatioWidth/2+437,290+100);

	Current = 0.5f;
	if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12)
	{
		//SetRect(&DstRest,1024-75-100 -14,290 -14,1024-75 +14,290+100 +14);
		if (CanBuy)
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
				true, MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
		else
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
				true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.5f, 0.0f);
		Current = 1.0f;
	}

	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			CurrentSystemStockNum = 9;
			Audio_PlayMenuSound(2,1.0f);
		}
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (Setup.Profile[CurrentProfile].TargetingSystem == 0)
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(-1), true, Current*MenuContentTransp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(Setup.Profile[CurrentProfile].TargetingSystem+8), true, Current*MenuContentTransp);



	// Mech
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,400,Setup.iAspectRatioWidth/2+437,400+100);

	Current = 0.5f;
	if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16)
	{
		//SetRect(&DstRest,1024-75-100 -14,400 -14,1024-75 +14,400+100 +14);
		if (CanBuy)
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
				true, MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
		else
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
				true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.5f, 0.0f);
		Current = 1.0f;
	}

	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
			CurrentSystemStockNum = 13;
		}
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (Setup.Profile[CurrentProfile].TargetingMechanicSystem == 0)
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(-3), true, Current*MenuContentTransp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(Setup.Profile[CurrentProfile].TargetingMechanicSystem+12), true, Current*MenuContentTransp);



	// Protect
	SetRect(&DstRest,Setup.iAspectRatioWidth/2+337,510,Setup.iAspectRatioWidth/2+437,510+100);

	if (NeedMoreEnergy && Setup.Profile[CurrentProfile].AdvancedProtectionSystem != 0)
	{
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
				true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.0f, 0.0f);
	}

	Current = 0.5f;
	if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20)
	{
		//SetRect(&DstRest,1024-75-100 -14,510 -14,1024-75 +14,510+100 +14);
		if (!NeedMoreEnergy || Setup.Profile[CurrentProfile].AdvancedProtectionSystem == 0)
		{
			if (CanBuy)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
					true, MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/system_onoff.tga"),
					true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.5f, 0.0f);
		}
		Current = 1.0f;
	}

	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{
		if (NeedPlayWorkshopOnButtonSoundX != DstRest.left || NeedPlayWorkshopOnButtonSoundY != DstRest.top)
		{
			Audio_PlayMenuSound(1,1.0f);
			NeedPlayWorkshopOnButtonSoundX = DstRest.left;
			NeedPlayWorkshopOnButtonSoundY = DstRest.top;
		}

		Current = 1.0f;
		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlayMenuSound(2,1.0f);
			CurrentSystemStockNum = 17;
		}
	}
	else
	{
		if (NeedPlayWorkshopOnButtonSoundX == DstRest.left && NeedPlayWorkshopOnButtonSoundY == DstRest.top)
		{
			NeedPlayWorkshopOnButtonSoundX = 0;
			NeedPlayWorkshopOnButtonSoundY = 0;
		}
	}

	if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem == 0)
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(0), true, Current*MenuContentTransp);
	else
		vw_DrawTransparent(&DstRest, &SrcRest, GetSystemIcon(Setup.Profile[CurrentProfile].AdvancedProtectionSystem+16), true, Current*MenuContentTransp);



	DrawFontX2((Setup.iAspectRatioWidth/2+512)-30-FontSizeX2(GetText("7_Installed_Systems")), 630, 0, 0, 0, MenuContentTransp, GetText("7_Installed_Systems"));


	// текущая система
	DrawFont(Setup.iAspectRatioWidth/2-250, 430, 0, 0, 1, MenuContentTransp, GetText("7_Selected_System"));
	DrawFont(Setup.iAspectRatioWidth/2+250-FontSize(GetText("7_Installed_System")), 430, 0, 0, 1, MenuContentTransp, GetText("7_Installed_System"));

	if (CanBuy)
		DrawFont(Setup.iAspectRatioWidth/2-250, 485, 0, 0, 0, MenuContentTransp, "%s: %i", GetText("7_Cost"), GetSystemCost(CurrentSystemStockNum));
	else
		DrawFont(Setup.iAspectRatioWidth/2-250, 485, 0, 0, 4, CurrentAlert3*MenuContentTransp, "%s: %i", GetText("7_Cost"), GetSystemCost(CurrentSystemStockNum));

	DrawFont(Setup.iAspectRatioWidth/2+250-FontSize("%s: %i", GetText("7_Cost"), Cost), 485, 0, 0, 0, MenuContentTransp, "%s: %i", GetText("7_Cost"), Cost);


	if (DrawButton128_2(Setup.iAspectRatioWidth/2-250,580-55, GetText("1_Info"), MenuContentTransp, false))
	{
		SetCurrentDialogBox(8);
		DialogSystem = CurrentSystemStockNum;
	}

	if (DrawButton128_2(Setup.iAspectRatioWidth/2+250-128,580-55, GetText("1_Info"), MenuContentTransp, !CanSell))
	{
		SetCurrentDialogBox(8);

		NeedMoreEnergyDialog = NeedMoreEnergy;

		if (CurrentSystemStockNum>=1 && CurrentSystemStockNum<=4)
			DialogSystem = Setup.Profile[CurrentProfile].EngineSystem;
		if (CurrentSystemStockNum>=5 && CurrentSystemStockNum<=8)
			DialogSystem = Setup.Profile[CurrentProfile].PowerSystem+4;
		if (CurrentSystemStockNum>=9 && CurrentSystemStockNum<=12)
			DialogSystem = Setup.Profile[CurrentProfile].TargetingSystem+8;
		if (CurrentSystemStockNum>=13 && CurrentSystemStockNum<=16)
			DialogSystem = Setup.Profile[CurrentProfile].TargetingMechanicSystem+12;
		if (CurrentSystemStockNum>=17 && CurrentSystemStockNum<=20)
			DialogSystem = Setup.Profile[CurrentProfile].AdvancedProtectionSystem+16;
	}


	if (DrawButton128_2(Setup.iAspectRatioWidth/2-250,50+580-55, GetText("1_Buy"), MenuContentTransp, !CanBuy))
	{
		BuyCurrentSystem();
	}
	// покупка, если 2 раза кликнули на иконку текущей системы
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-451,55+128+20,Setup.iAspectRatioWidth/2-451+128,55+128+128+20);
	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing() && CanBuy)
		if (vw_GetWindowLBDoubleMouse(true))
			BuyCurrentSystem();



	if (DrawButton128_2(Setup.iAspectRatioWidth/2+250-128,50+580-55, GetText("1_Sell"), MenuContentTransp, !CanSell))
	{
		SellCurrentSystem();
	}


	// вывод информации
	int SizeI = (Setup.iAspectRatioWidth-FontSizeX15("%s: %i", GetText("3_Money"), Setup.Profile[CurrentProfile].Money))/2;
	if (CanBuy)
		DrawFontX15(SizeI, 630, 0, 0, 1, MenuContentTransp, "%s: %i", GetText("3_Money"), Setup.Profile[CurrentProfile].Money);
	else
		DrawFontX15(SizeI, 630, 0, 0, 4, CurrentAlert3*MenuContentTransp, "%s: %i", GetText("3_Money"), Setup.Profile[CurrentProfile].Money);




}

