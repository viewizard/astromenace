/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Workshop_Weaponry.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

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
extern CWeapon *WorkshopNewWeapon;
extern int	CurrentWorkshopNewWeapon;
extern float CurentDeviationSum;
extern VECTOR3D WorkShopPointCamera;
extern float CurrentAlert2;
extern float CurrentAlert3;

void WorkshopCreateNewWeapon();
void WorkshopDrawShip(CEarthSpaceFighter *SpaceFighter, int Mode);
void WorkshopDrawWeapon(CWeapon *Weapon);

char *GetWeaponGroupTitle(int Num);
int GetWeaponHullDamage(int Num);
int GetWeaponSystemsDamage(int Num);

// что рисовать в диалоге 6,7,8
extern CWeapon *DialogWeapon;


// флаг-тянем
bool DragWeapon = false;
// номер оружия, которое тянем
int DragWeaponNum = 0;
// уровень оружия, которое тянем
int DragWeaponLevel = 0;
// кол-во боекомплекта в оружии
int DragWeaponAmmo = 0;
// общее кол-во боекомплекта в заполненном боекомплекте
int DragWeaponAmmoStart = 0;
// управление
int DragWeaponControl = 0;
int DragWeaponAltControl = 0;
int DragWeaponAltControlData = 0;


// сюда вытягиваем данные
int	NewWeaponControlType = 0;
int	NewWeaponControlTypeData = 0;



// вывод большого меню с настройкой оружия
// передаем только номер слота оружия
int WeaponSetupSlot = -1;

// Номер, для проигрывания голосового сообщения об отсутствии снарядов в боекомплекте
int VoiceAmmoOut = 0;







int NextWeaponGroup()
{
	if (CurrentWorkshopNewWeapon>=1 && CurrentWorkshopNewWeapon<=4) return 5;
	if (CurrentWorkshopNewWeapon>=5 && CurrentWorkshopNewWeapon<=7) return 8;
	if (CurrentWorkshopNewWeapon>=8 && CurrentWorkshopNewWeapon<=10) return 11;
	if (CurrentWorkshopNewWeapon>=11 && CurrentWorkshopNewWeapon<=12) return 13;
	if (CurrentWorkshopNewWeapon>=13 && CurrentWorkshopNewWeapon<=13) return 14;
	if (CurrentWorkshopNewWeapon>=14 && CurrentWorkshopNewWeapon<=14) return 15;
	if (CurrentWorkshopNewWeapon>=15 && CurrentWorkshopNewWeapon<=15) return 16;
	if (CurrentWorkshopNewWeapon>=16 && CurrentWorkshopNewWeapon<=19) return 1;

	return 0;
}
int PrevWeaponGroup()
{
	if (CurrentWorkshopNewWeapon>=1 && CurrentWorkshopNewWeapon<=4) return 16;
	if (CurrentWorkshopNewWeapon>=5 && CurrentWorkshopNewWeapon<=7) return 1;
	if (CurrentWorkshopNewWeapon>=8 && CurrentWorkshopNewWeapon<=10) return 5;
	if (CurrentWorkshopNewWeapon>=11 && CurrentWorkshopNewWeapon<=12) return 8;
	if (CurrentWorkshopNewWeapon>=13 && CurrentWorkshopNewWeapon<=13) return 11;
	if (CurrentWorkshopNewWeapon>=14 && CurrentWorkshopNewWeapon<=14) return 13;
	if (CurrentWorkshopNewWeapon>=15 && CurrentWorkshopNewWeapon<=15) return 14;
	if (CurrentWorkshopNewWeapon>=16 && CurrentWorkshopNewWeapon<=19) return 15;

	return 0;
}






int GetWeaponBaseCost(int Num)
{
	switch (Num)
	{
		case 1:		return 150;
		case 2:		return 300;
		case 3:		return 1000;
		case 4:		return 1500;

		case 5:		return 250;
		case 6:		return 1000;
		case 7:		return 1900;

		case 8:		return 1700;
		case 9:		return 2500;
		case 10:	return 4500;

		case 11:	return 2500;
		case 12:	return 5000;

		case 13:	return 6000;

		case 14:	return 8000;

		case 15:	return 10000;

		case 16:	return 1500;
		case 17:	return 4000;
		case 18:	return 5000;
		case 19:	return 8000;

		default:	return 0;
	}
}
int GetWeaponCost(int Num, int Ammo, int AmmoStart)
{
	// получаем базовую стоимость оружия
	int Cost = GetWeaponBaseCost(Num);
	// находим базовую стоимость оружия
	int BaseCost = Cost/2;
	// берем половину стоимости, все четное, но на всякий случай так
	Cost -= BaseCost;
	// находим стоимость боекомплекта
	BaseCost += (int)(Cost*((Ammo*1.0f)/(AmmoStart*1.0f)));

	return BaseCost;
}
int GetWeaponReloadCost(int Num, int Ammo, int AmmoStart)
{
	// получаем базовую стоимость оружия
	int Cost = GetWeaponBaseCost(Num);
	// находим базовую стоимость оружия
	int BaseCost = Cost/2;
	// берем половину стоимости, все четное, но на всякий случай так
	Cost -= BaseCost;
	// находим стоимость боекомплекта
	return Cost - (int)(Cost*((Ammo*1.0f)/(AmmoStart*1.0f)));
}



const char *GetWeaponName(int Num)
{
	switch (Num)
	{
		case 1:		return "7_weapon1";
		case 2:		return "7_weapon2";
		case 3:		return "7_weapon3";
		case 4:		return "7_weapon4";

		case 5:		return "7_weapon5";
		case 6:		return "7_weapon6";
		case 7:		return "7_weapon7";

		case 8:		return "7_weapon8";
		case 9:		return "7_weapon9";
		case 10:	return "7_weapon10";

		case 11:	return "7_weapon11";
		case 12:	return "7_weapon12";

		case 13:	return "7_weapon13";

		case 14:	return "7_weapon14";

		case 15:	return "7_weapon15";

		case 16:	return "7_weapon16";
		case 17:	return "7_weapon17";
		case 18:	return "7_weapon18";
		case 19:	return "7_weapon19";
	}

	return 0;
}





const char *GetWeaponIconName(int Num)
{
	switch (Num)
	{
		case 1:		return "DATA/MENU/weapon1_icon.tga";
		case 2:		return "DATA/MENU/weapon2_icon.tga";
		case 3:		return "DATA/MENU/weapon3_icon.tga";
		case 4:		return "DATA/MENU/weapon4_icon.tga";
		case 5:		return "DATA/MENU/weapon5_icon.tga";
		case 6:		return "DATA/MENU/weapon6_icon.tga";
		case 7:		return "DATA/MENU/weapon7_icon.tga";
		case 8:		return "DATA/MENU/weapon8_icon.tga";
		case 9:		return "DATA/MENU/weapon9_icon.tga";
		case 10:	return "DATA/MENU/weapon10_icon.tga";
		case 11:	return "DATA/MENU/weapon11_icon.tga";
		case 12:	return "DATA/MENU/weapon12_icon.tga";
		case 13:	return "DATA/MENU/weapon13_icon.tga";
		case 14:	return "DATA/MENU/weapon14_icon.tga";
		case 15:	return "DATA/MENU/weapon15_icon.tga";
		case 16:	return "DATA/MENU/weapon16_icon.tga";
		case 17:	return "DATA/MENU/weapon17_icon.tga";
		case 18:	return "DATA/MENU/weapon18_icon.tga";
		case 19:	return "DATA/MENU/weapon19_icon.tga";
	}
	return 0;
}
const char *GetSlotIconName(int Num)
{
	switch (Num)
	{
		case 1:		return "DATA/MENU/weapon_empty1_icon.tga";
		case 2:		return "DATA/MENU/weapon_empty2_icon.tga";
		case 3:		return "DATA/MENU/weapon_empty3_icon.tga";
		case 4:		return "DATA/MENU/weapon_empty4_icon.tga";
		case 5:		return "DATA/MENU/weapon_empty5_icon.tga";
	}
	return 0;
}




// вся работа с еденичным слотом
void ShipSlotWeapon(int SlotNum, int X, int Y)
{
	RECT SrcRest, DstRest;

	SetRect(&SrcRest,0,0,220,128);
	int Xpos = X-45;
	int Ypos = Y-36;
	SetRect(&DstRest,Xpos,Ypos,Xpos+220,Ypos+128);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/workshop_panel3.tga"), true, MenuContentTransp);


	// нужны предупреждения...
	bool WeaponAmmoOut = false;

	// отображаем боекомплект
	if (WorkshopFighterGame->Weapon[SlotNum] != 0)
	{
		int AmmoShow = (int)((56.0f*(WorkshopFighterGame->Weapon[SlotNum]->AmmoStart-WorkshopFighterGame->Weapon[SlotNum]->Ammo))/WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);
		// если меняли боекомплект и сделали его меньше, чтобы не вылазила линия боекомплекта...
		if (AmmoShow < 0) AmmoShow = 0;

		SetRect(&SrcRest,0,AmmoShow,18,56);
		SetRect(&DstRest,Xpos+23,Ypos+40+AmmoShow,Xpos+18+23,Ypos+56+40);
		if (AmmoShow > 0)
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/ammo.tga"), true, CurrentAlert3*MenuContentTransp);
		else
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/ammo.tga"), true, MenuContentTransp);


		if (WorkshopFighterGame->Weapon[SlotNum]->Ammo == 0) WeaponAmmoOut = true;
	}


	if (WeaponAmmoOut)
	{
		// запускаем голос...
		if (vw_FindSoundByNum(VoiceAmmoOut) == 0)
		{
			// уже не играем, нужно запустить опять
			VoiceAmmoOut = Audio_PlayVoice(10, 1.0f);
		}
	}


	// кнопка Setup
	int MouseX, MouseY;
	vw_GetMousePos(&MouseX, &MouseY);

	SetRect(&SrcRest,0,0,18,56);
	SetRect(&DstRest,Xpos+23+154,Ypos+40,Xpos+18+23+154,Ypos+56+40);
	if  (((DstRest.right  >= MouseX)&
		(DstRest.left<= MouseX)&
		(DstRest.bottom >= MouseY)&
		(DstRest.top<= MouseY)) && !isDialogBoxDrawing() && !DragWeapon)
	{
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_weaponry_in.tga"), true, MenuContentTransp);
		CurrentCursorStatus = 1;
		if (vw_GetWindowLBMouse(true))
		{
			Audio_PlaySound2D(2,1.0f);
			WeaponSetupSlot = SlotNum;
		}
	}
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/button_weaponry_out.tga"), true, MenuContentTransp);



	// обработка перетягивания
	SetRect(&DstRest,X,Y,X+128,Y+64);
	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{

		int Money = Setup.Profile[CurrentProfile].Money;
		if (WorkshopFighterGame->Weapon[SlotNum] != 0)
			Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
									WorkshopFighterGame->Weapon[SlotNum]->Ammo,
									WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);

		// если отпустили тут
		if (!vw_GetWindowLBMouse(false) && DragWeapon)
		{
			// есть уровень слота соотв. уровню оружия
			if ((WorkshopFighterGame->WeaponType[SlotNum] >= DragWeaponLevel) &&
			// если стоимость меньше чем есть денег + стоимость оружия
				(Money >= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart)))
			{
				// звук устанавливаемого оружия
				Audio_PlaySound2D(10,1.0f);

				// если тут было оружие - сначало продаем его
				if (WorkshopFighterGame->Weapon[SlotNum] != 0)
				{
					Setup.Profile[CurrentProfile].Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
										WorkshopFighterGame->Weapon[SlotNum]->Ammo,
										WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);
					delete WorkshopFighterGame->Weapon[SlotNum]; WorkshopFighterGame->Weapon[SlotNum] = 0;
					Setup.Profile[CurrentProfile].Weapon[SlotNum] = 0;
					Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
				}

				// покупаем оружие
				Setup.Profile[CurrentProfile].Money -= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart);

				// все проверки сделали до этого, можем просто вызвать функцию, там 100% будет оружие
				SetEarthSpaceFighterWeapon(WorkshopFighterGame, SlotNum+1, DragWeaponNum);
				// убираем источник света
				if (WorkshopFighterGame->Weapon[SlotNum]->Fire != 0)
				if (WorkshopFighterGame->Weapon[SlotNum]->Fire->Light != 0){vw_ReleaseLight(WorkshopFighterGame->Weapon[SlotNum]->Fire->Light); WorkshopFighterGame->Weapon[SlotNum]->Fire->Light = 0;}

				Setup.Profile[CurrentProfile].Weapon[SlotNum] = DragWeaponNum;
				Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = DragWeaponAmmo;
				Setup.Profile[CurrentProfile].WeaponControl[SlotNum] = DragWeaponControl;
				Setup.Profile[CurrentProfile].WeaponAltControl[SlotNum] = DragWeaponAltControl;
				Setup.Profile[CurrentProfile].WeaponAltControlData[SlotNum] = DragWeaponAltControlData;
				WorkshopFighterGame->Weapon[SlotNum]->Ammo = DragWeaponAmmo;
				WorkshopFighterGame->Weapon[SlotNum]->AmmoStart = DragWeaponAmmoStart;

				// если не ракетная установка
				if (DragWeaponNum < 16)
				{
					WorkshopFighterGame->WeaponYAngle[SlotNum] = Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];
					WorkshopFighterGame->Weapon[SlotNum]->SetRotation(WorkshopFighterGame->Weapon[SlotNum]->Rotation^(-1));
					VECTOR3D NeedAngle = WorkshopFighterGame->Weapon[SlotNum]->Rotation;
					NeedAngle.y += Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];
					WorkshopFighterGame->Weapon[SlotNum]->SetRotation(NeedAngle);
				}
				else
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] = 0.0f;

				// чтобы оружие заняло свое место...
				WorkshopFighterGame->Update(vw_GetTime());

				// сброс
				DragWeapon = false;
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeaponAmmo = 0;
				DragWeaponAmmoStart = 0;
				DragWeaponControl = 0;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;
			}
			else
			{
				// особый случай - есть не соответствие, нужно проиграть звук неудачной установки
				Audio_PlaySound2D(8, 1.0f);

				// сброс
				DragWeapon = false;
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeaponAmmo = 0;
				DragWeaponAmmoStart = 0;
				DragWeaponControl = 0;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;
			}
		}


		// взяли оружие из слота, чтобы тащить - фактически продали его
		if (WorkshopFighterGame->Weapon[SlotNum] != 0)
		if (vw_GetWindowLBMouse(false) && !DragWeapon)
		{
			// звук снятия оружия из слота
			Audio_PlaySound2D(9, 1.0f);

			DragWeapon = true;
			DragWeaponNum = WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType;
			DragWeaponLevel = WorkshopFighterGame->Weapon[SlotNum]->WeaponLevel;
			DragWeaponAmmo = WorkshopFighterGame->Weapon[SlotNum]->Ammo;
			DragWeaponAmmoStart = WorkshopFighterGame->Weapon[SlotNum]->AmmoStart;
			DragWeaponControl = Setup.Profile[CurrentProfile].WeaponControl[SlotNum];
			DragWeaponAltControl = Setup.Profile[CurrentProfile].WeaponAltControl[SlotNum];
			DragWeaponAltControlData = Setup.Profile[CurrentProfile].WeaponAltControlData[SlotNum];

			Setup.Profile[CurrentProfile].Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
									WorkshopFighterGame->Weapon[SlotNum]->Ammo,
									WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);
			delete WorkshopFighterGame->Weapon[SlotNum]; WorkshopFighterGame->Weapon[SlotNum] = 0;
			Setup.Profile[CurrentProfile].Weapon[SlotNum] = 0;
			Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
		}



		// если не тянем и в слоте что-то есть, показываем, что можем тянуть
		if (!DragWeapon && (WorkshopFighterGame->Weapon[SlotNum] != 0))
			CurrentCursorStatus = 1;

	}




	// рисуем можем ставить в слот или нет
	bool CanOn = false;
	// проверка по уровню слота
	if (DragWeapon && (WorkshopFighterGame->WeaponType[SlotNum] >= DragWeaponLevel)) CanOn = true;
	if (!DragWeapon && (WorkshopFighterGame->WeaponType[SlotNum] >= WorkshopNewWeapon->WeaponLevel)) CanOn = true;

	SetRect(&SrcRest,0,0,128,64);
	SetRect(&DstRest,X,Y,X+128,Y+64);

	if (CanOn)
	{
		if (!WeaponAmmoOut)
		{
			bool NeedAlert = false;
			// если есть хоть 1 слот меньшего уровня
			for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++)
			{
				// если что-то тянем, смотрим по нему...
				if (DragWeapon)
				{
					if (WorkshopFighterGame->WeaponType[i] < DragWeaponLevel) NeedAlert = true;
				}
				else
				{
					if (WorkshopFighterGame->WeaponType[i] < WorkshopNewWeapon->WeaponLevel) NeedAlert = true;
				}
			}

			if (NeedAlert)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/weapon_on_icon.tga"), true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
			else
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/weapon_on_icon.tga"), true, MenuContentTransp, 0, RI_UL_CORNER, 0.0f, 1.0f, 0.0f);
		}
		else
			vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/weapon_on_icon.tga"), true, CurrentAlert3*MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.0f, 0.0f);
	}
	else
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/weapon_on_icon.tga"), true, MenuContentTransp, 0, RI_UL_CORNER, 1.0f, 0.5f, 0.0f);





	// прорисовка

	if (WorkshopFighterGame->Weapon[SlotNum] == 0)
	{
		// пустой слот, рисуем его

		SetRect(&SrcRest,0,0,128,64);
		SetRect(&DstRest,X,Y,X+128,Y+64);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName(GetSlotIconName(WorkshopFighterGame->WeaponType[SlotNum])), true, MenuContentTransp);

		return;
	}

	SetRect(&SrcRest,0,0,128,64);
	SetRect(&DstRest,X,Y,X+128,Y+64);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName(GetWeaponIconName(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType)), true, MenuContentTransp);




}






void ShipSlotSetupWeapon(int Slot)
{
	RECT SrcRest, DstRest;

	// проверяем, нужно ли вытягивать что-то или нет...
	CheckMouseKeybJState();

	SetRect(&SrcRest,0,0,404,570);
	int Xpos = Setup.iAspectRatioWidth/2+55;
	int Ypos = 50-10;
	SetRect(&DstRest,Xpos,Ypos,Xpos+404,Ypos+570);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/workshop_panel5.tga"), true, MenuContentTransp);



	if (WorkshopFighterGame->Weapon[Slot] == 0)
	{
		Xpos += 74;
		Ypos += 128;
		// пустой слот, рисуем его
		SetRect(&SrcRest,0,0,256,256);
		SetRect(&DstRest,Xpos,Ypos,Xpos+256,Ypos+256);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/weapon_empty_icon.tga"), true, MenuContentTransp);
	}
	else
	{
		Xpos += 74+64;
		Ypos += 33;
		// выводим боекомплект   текущий/полный
		Xpos = Setup.iAspectRatioWidth/2+55 + 50;
		vw_DrawFont(Xpos, Ypos, -170, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("7_Weapon_Ammo:"));
		Xpos = (Setup.iAspectRatioWidth/2+512)-55 - 50 - vw_FontSize("%i/%i", WorkshopFighterGame->Weapon[Slot]->Ammo, WorkshopFighterGame->Weapon[Slot]->AmmoStart);


		// если все нормально - белым... иначе подмаргиваем
		if (WorkshopFighterGame->Weapon[Slot]->Ammo == WorkshopFighterGame->Weapon[Slot]->AmmoStart)
			vw_DrawFont(Xpos, Ypos, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i/%i", WorkshopFighterGame->Weapon[Slot]->Ammo, WorkshopFighterGame->Weapon[Slot]->AmmoStart);
		else
			vw_DrawFont(Xpos, Ypos, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%i/%i", WorkshopFighterGame->Weapon[Slot]->Ammo, WorkshopFighterGame->Weapon[Slot]->AmmoStart);


		// стоимость перезарядки
		Xpos = Setup.iAspectRatioWidth/2+55 + 50;
		Ypos += 30;
		vw_DrawFont(Xpos, Ypos, -230, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("7_Weapon_Reload_Cost:"));
		// находим стоимость перезарядки
		int ReloadCost = GetWeaponReloadCost(WorkshopFighterGame->Weapon[Slot]->ObjectCreationType,
			WorkshopFighterGame->Weapon[Slot]->Ammo,
			WorkshopFighterGame->Weapon[Slot]->AmmoStart);
		Xpos = (Setup.iAspectRatioWidth/2+512)-55 - 50 - vw_FontSize("%i", ReloadCost);
		if (ReloadCost == 0)
			vw_DrawFont(Xpos, Ypos, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i", ReloadCost);
		else
			vw_DrawFont(Xpos, Ypos, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%i", ReloadCost);


		// кнопка перезарядить оружие
		Ypos += 40;
		if (DrawButton200_2(Setup.iAspectRatioWidth/2+155, Ypos, GetText("1_Reload"), MenuContentTransp, ReloadCost==0 || Setup.Profile[CurrentProfile].Money<ReloadCost))
		{
			WorkshopFighterGame->Weapon[Slot]->Ammo = WorkshopFighterGame->Weapon[Slot]->AmmoStart;
			Setup.Profile[CurrentProfile].WeaponAmmo[Slot] = WorkshopFighterGame->Weapon[Slot]->Ammo;
			Setup.Profile[CurrentProfile].Money -= ReloadCost;
		}



		// настройка управления для этого оружия

		bool Status1 = false;
		bool Status2 = false;

		Xpos = Setup.iAspectRatioWidth/2+55+34 + 16;
		Ypos += 60;
		vw_DrawFont(Xpos, Ypos, -300, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Weapon_Fire_Control:"));
		// вкл-выкл первичного управления
		if (Setup.Profile[CurrentProfile].WeaponControl[Slot] == 1 ||
		Setup.Profile[CurrentProfile].WeaponControl[Slot] ==3) Status1 = true;
		Xpos = Setup.iAspectRatioWidth/2+55+54 + 16;
		Ypos += 30;
		DrawCheckBox(Xpos,Ypos, &Status1, GetText("3_Primary_Attack"), MenuContentTransp);
		// вкл-выкл вторичного управления
		if (Setup.Profile[CurrentProfile].WeaponControl[Slot] == 2 ||
		Setup.Profile[CurrentProfile].WeaponControl[Slot] ==3) Status2 = true;
		Xpos = Setup.iAspectRatioWidth/2+55+54 + 16;
		Ypos += 40;
		DrawCheckBox(Xpos,Ypos, &Status2, GetText("3_Secondary_Attack"), MenuContentTransp);
		// получаем данны обратно
		Setup.Profile[CurrentProfile].WeaponControl[Slot] = 0;
		if (Status1) Setup.Profile[CurrentProfile].WeaponControl[Slot] += 1;
		if (Status2) Setup.Profile[CurrentProfile].WeaponControl[Slot] += 2;

		// получение альтернативного управления
		Xpos = Setup.iAspectRatioWidth/2+55+34 + 16;
		Ypos += 40;
		vw_DrawFont(Xpos, Ypos, -300, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Alternative_Fire_Control:"));

		Ypos += 40;

		const char *TextTmp = "?";
		// установка надписи на кнопке
		if (NeedCheck != 100)
		{
			TextTmp = (char*)GetText("1_Unimplemented");

			if (NewWeaponControlType != 0)
			{
				Setup.Profile[CurrentProfile].WeaponAltControl[Slot] = NewWeaponControlType;
				Setup.Profile[CurrentProfile].WeaponAltControlData[Slot] = NewWeaponControlTypeData;
				NewWeaponControlType = 0;
				NewWeaponControlTypeData = 0;
			}
			if (Setup.Profile[CurrentProfile].WeaponAltControl[Slot] != 0)
			{
				if (Setup.Profile[CurrentProfile].WeaponAltControl[Slot] == 1)
					TextTmp = vw_VirtualCodeName(Setup.MenuLanguage, Setup.Profile[CurrentProfile].WeaponAltControlData[Slot]);
				if (Setup.Profile[CurrentProfile].WeaponAltControl[Slot] == 2)
					TextTmp = MouseCodeName(Setup.Profile[CurrentProfile].WeaponAltControlData[Slot]);
				if (Setup.Profile[CurrentProfile].WeaponAltControl[Slot] == 3)
					TextTmp = JoystickCodeName(Setup.Profile[CurrentProfile].WeaponAltControlData[Slot]);

			}
		}

		// собственно сама кнопка
		float Transp = 1.0f;
		bool Off = false;
		if (NeedCheck == 100){Transp = But[1]; Off = true;}
		if (DrawButton200_2(Setup.iAspectRatioWidth/2+155, Ypos, TextTmp, Transp*MenuContentTransp, Off))
		{
			NeedCheck = 100;
			for (int i=0; i<8; i++) SDL_MouseCurrentStatus[i] = false;
			NewWeaponControlType = 0;
			NewWeaponControlTypeData = 0;
		}



		if (Setup.Profile[CurrentProfile].Weapon[Slot] < 16)
		{

			// выводим угол поворота ствола
			Xpos = Setup.iAspectRatioWidth/2+55+34 + 16;
			Ypos += 60;
			vw_DrawFont(Xpos, Ypos, -300, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("3_Weapon_Angle:_%2.1f"), Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot]);
			Ypos += 40;

			float Min = 0.0f;
			float Max = 0.0f;
			GetShipWeaponSlotAngle(Setup.Profile[CurrentProfile].Ship, Slot, &Min, &Max);


			if (Setup.Profile[CurrentProfile].TargetingSystem <= 2)
			{
				vw_DrawFont(Xpos, Ypos-15, 300, 0, 1.0f, 1.0f,0.5f,0.0f, 1.0f, GetText("9_17Line1"));
				vw_DrawFont(Xpos, Ypos+5, 300, 0, 1.0f, 1.0f,0.5f,0.0f, 1.0f, GetText("9_17Line2"));
				vw_DrawFont(Xpos, Ypos+25, 300, 0, 1.0f, 1.0f,0.5f,0.0f, 1.0f, GetText("9_17Line3"));
			}
			else
			{
				if (DrawButton128_2(Setup.iAspectRatioWidth/2+118, Ypos, GetText("1_Left"), MenuContentTransp, Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot] <= Min))
				{
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot] -= 5.0f;
					WorkshopFighterGame->WeaponYAngle[Slot] = Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot];

					WorkshopFighterGame->Weapon[Slot]->SetRotation(WorkshopFighterGame->Weapon[Slot]->Rotation^(-1));
					VECTOR3D NeedAngle = WorkshopFighterGame->Weapon[Slot]->Rotation;
					NeedAngle.y += Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot];
					WorkshopFighterGame->Weapon[Slot]->SetRotation(NeedAngle);
				}
				if (DrawButton128_2(Setup.iAspectRatioWidth/2+266, Ypos, GetText("1_Right"), MenuContentTransp, Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot] >= Max))
				{
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot] += 5.0f;
					WorkshopFighterGame->WeaponYAngle[Slot] = Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot];

					WorkshopFighterGame->Weapon[Slot]->SetRotation(WorkshopFighterGame->Weapon[Slot]->Rotation^(-1));
					VECTOR3D NeedAngle = WorkshopFighterGame->Weapon[Slot]->Rotation;
					NeedAngle.y += Setup.Profile[CurrentProfile].WeaponSlotYAngle[Slot];
					WorkshopFighterGame->Weapon[Slot]->SetRotation(NeedAngle);
				}
			}
		}
	}





	// обработка перетягивания
	int SlotNum = Slot;
	Xpos = Setup.iAspectRatioWidth/2+55;
	Ypos = 50-10;
	SetRect(&DstRest,Xpos+10,Ypos+10,Xpos+404-10,Ypos+570-10);
	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	{

		int Money = Setup.Profile[CurrentProfile].Money;
		if (WorkshopFighterGame->Weapon[SlotNum] != 0)
			Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
									WorkshopFighterGame->Weapon[SlotNum]->Ammo,
									WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);

		// если отпустили тут
		if (!vw_GetWindowLBMouse(false) && DragWeapon)
		{
			// есть уровень слота соотв. уровню оружия
			if ((WorkshopFighterGame->WeaponType[SlotNum] >= DragWeaponLevel) &&
			// если стоимость меньше чем есть денег + стоимость оружия
				(Money >= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart)))
			{
				// звук устанавливаемого оружия
				Audio_PlaySound2D(10,1.0f);

				// если тут было оружие - сначало продаем его
				if (WorkshopFighterGame->Weapon[SlotNum] != 0)
				{
					Setup.Profile[CurrentProfile].Money += GetWeaponCost(WorkshopFighterGame->Weapon[SlotNum]->ObjectCreationType,
										WorkshopFighterGame->Weapon[SlotNum]->Ammo,
										WorkshopFighterGame->Weapon[SlotNum]->AmmoStart);
					delete WorkshopFighterGame->Weapon[SlotNum]; WorkshopFighterGame->Weapon[SlotNum] = 0;
					Setup.Profile[CurrentProfile].Weapon[SlotNum] = 0;
					Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = 0;
				}

				// покупаем оружие
				Setup.Profile[CurrentProfile].Money -= GetWeaponCost(DragWeaponNum, DragWeaponAmmo, DragWeaponAmmoStart);

				// все проверки сделали до этого, можем просто вызвать функцию, там 100% будет оружие
				SetEarthSpaceFighterWeapon(WorkshopFighterGame, SlotNum+1, DragWeaponNum);
				// убираем источник света
				if (WorkshopFighterGame->Weapon[SlotNum]->Fire != 0)
				if (WorkshopFighterGame->Weapon[SlotNum]->Fire->Light != 0){vw_ReleaseLight(WorkshopFighterGame->Weapon[SlotNum]->Fire->Light); WorkshopFighterGame->Weapon[SlotNum]->Fire->Light = 0;}

				Setup.Profile[CurrentProfile].Weapon[SlotNum] = DragWeaponNum;
				Setup.Profile[CurrentProfile].WeaponAmmo[SlotNum] = DragWeaponAmmo;
				Setup.Profile[CurrentProfile].WeaponControl[SlotNum] = DragWeaponControl;
				Setup.Profile[CurrentProfile].WeaponAltControl[SlotNum] = DragWeaponAltControl;
				Setup.Profile[CurrentProfile].WeaponAltControlData[SlotNum] = DragWeaponAltControlData;
				WorkshopFighterGame->Weapon[SlotNum]->Ammo = DragWeaponAmmo;
				WorkshopFighterGame->Weapon[SlotNum]->AmmoStart = DragWeaponAmmoStart;

				// если не ракетная установка
				if (DragWeaponNum < 16)
				{
					WorkshopFighterGame->WeaponYAngle[SlotNum] = Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];
					WorkshopFighterGame->Weapon[SlotNum]->SetRotation(WorkshopFighterGame->Weapon[SlotNum]->Rotation^(-1));
					VECTOR3D NeedAngle = WorkshopFighterGame->Weapon[SlotNum]->Rotation;
					NeedAngle.y += Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum];
					WorkshopFighterGame->Weapon[SlotNum]->SetRotation(NeedAngle);
				}
				else
					Setup.Profile[CurrentProfile].WeaponSlotYAngle[SlotNum] = 0.0f;

				// чтобы оружие заняло свое место...
				WorkshopFighterGame->Update(vw_GetTime());

				// сброс
				DragWeapon = false;
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeaponAmmo = 0;
				DragWeaponAmmoStart = 0;
				DragWeaponControl = 0;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;
			}
			else
			{
				// особый случай - есть не соответствие, нужно проиграть звук неудачной установки
				Audio_PlaySound2D(8, 1.0f);

				// сброс
				DragWeapon = false;
				DragWeaponNum = 0;
				DragWeaponLevel = 0;
				DragWeaponAmmo = 0;
				DragWeaponAmmoStart = 0;
				DragWeaponControl = 0;
				DragWeaponAltControl = 0;
				DragWeaponAltControlData = 0;
			}
		}

	}





	//кнопка закрыть
	if (DrawButton200_2(Setup.iAspectRatioWidth/2+155, 533, GetText("1_Close"), MenuContentTransp, false))
	{
		WeaponSetupSlot = -1;
		NeedCheck = 0;
	}


}













void DrawShipWeaponsInSlots()
{
	// если нужно - выводим настройку нужного оружия
	if (WeaponSetupSlot != -1)
	{
		ShipSlotSetupWeapon(WeaponSetupSlot);
		return;
	}

	switch (Setup.Profile[CurrentProfile].Ship)
	{
		case 1:
			ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 500);
			ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 500);
			ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 100);
			ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 100);
			ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+256)-64, 300);
			break;
		case 2:
			ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 500);
			ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 500);
			ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 100);
			ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 100);
			break;
		case 3:
			ShipSlotWeapon(0, (Setup.iAspectRatioWidth/2+256)-64, 100);
			ShipSlotWeapon(1, Setup.iAspectRatioWidth/2+50, 500);
			ShipSlotWeapon(2, (Setup.iAspectRatioWidth/2+512)-128-50, 500);
			break;
		case 4:
			ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 500);
			ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 500);
			ShipSlotWeapon(2, Setup.iAspectRatioWidth/2+50, 300);
			ShipSlotWeapon(3, (Setup.iAspectRatioWidth/2+512)-128-50, 300);
			ShipSlotWeapon(4, (Setup.iAspectRatioWidth/2+256)-64, 100);
			break;
		case 5:
			ShipSlotWeapon(0, Setup.iAspectRatioWidth/2+50, 300);
			ShipSlotWeapon(1, (Setup.iAspectRatioWidth/2+512)-128-50, 300);
			break;
	}

}



int GetShipWeaponsMaxSlotLevel()
{
	int max = 1;

	for (int i=0; i<WorkshopFighterGame->WeaponQuantity; i++)
		if (WorkshopFighterGame->WeaponType[i] > max)
			max = WorkshopFighterGame->WeaponType[i];

	return max;
}




// вызываем прямо перед прорисовкой курсора
void DrawDragingWeaponIcon(int X, int Y)
{
	// если не тянем - соотв. и не рисуем
	if (!DragWeapon) return;

	// в х и у - положение точки курсора
	RECT SrcRest, DstRest;

	SetRect(&SrcRest,0,0,128,64);
	SetRect(&DstRest,X-64,Y-32,X+64,Y+32);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName(GetWeaponIconName(DragWeaponNum)), true, MenuContentTransp);
}




















//------------------------------------------------------------------------------------
// покупка-установка и ремонт вооружения корабля
//------------------------------------------------------------------------------------
void Workshop_Weaponry()
{
	RECT SrcRest, DstRest;



	// если нажали - установка и тянем
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-416,100+32,Setup.iAspectRatioWidth/2-96,450-32);
	if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
	if (!DragWeapon)
	{
		CurrentCursorStatus = 1;

		if (vw_GetWindowLBMouse(false))
		{
			// звук взяли оружие
			Audio_PlaySound2D(9,1.0f);

			// установка
			DragWeapon = true;
			DragWeaponNum = CurrentWorkshopNewWeapon;
			DragWeaponLevel = WorkshopNewWeapon->WeaponLevel;
			DragWeaponAmmo = WorkshopNewWeapon->Ammo;
			DragWeaponAmmoStart = WorkshopNewWeapon->AmmoStart;
			// второй по умолчанию только для рокет
			if (CurrentWorkshopNewWeapon<=15) DragWeaponControl = 1;
			else DragWeaponControl = 2;
			DragWeaponAltControl = 0;
			DragWeaponAltControlData = 0;

		}
	}


	// затемнение
	SetRect(&SrcRest,0,0,256,256);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-480,100-32,Setup.iAspectRatioWidth/2-32,450+32);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/back_spot2.tga"), true, 0.55f*MenuContentTransp);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2,0,Setup.iAspectRatioWidth/2+512,622);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/back_spot.tga"), true, 0.55f*MenuContentTransp);


	vw_End2DMode();
	WorkshopDrawShip(WorkshopFighterGame, 4);
	WorkshopDrawWeapon(WorkshopNewWeapon);
	vw_Start2DMode(-1,1);


	// вывод названия
	vw_DrawFont(Setup.iAspectRatioWidth/2-438, 50+6, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, GetText(GetWeaponName(CurrentWorkshopNewWeapon)));
	if (DrawButton128_2(Setup.iAspectRatioWidth/2-197,50, GetText("1_Info"), MenuContentTransp, false))
	{
		SetCurrentDialogBox(7);
		DialogWeapon = WorkshopNewWeapon;
	}

	// поставить GameNPCWeaponPenalty=1!!!
	vw_DrawFont(Setup.iAspectRatioWidth/2-438, 110, -170, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%s:", GetText("7_Weapon_Type"));
	vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 110, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText(GetWeaponGroupTitle(CurrentWorkshopNewWeapon)));


	int k2 = 0;
	if (GetWeaponHullDamage(WorkshopNewWeapon->ObjectCreationType) > 0.0f)
	{
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 130, -170, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("4_Damage,_Hull:"));
		if ((WorkshopNewWeapon->ObjectCreationType == 11) |
			(WorkshopNewWeapon->ObjectCreationType == 12) |
			(WorkshopNewWeapon->ObjectCreationType == 14))
			vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 130, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponHullDamage(WorkshopNewWeapon->ObjectCreationType), GetText("4_units/sec"));
		else
			vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 130, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponHullDamage(WorkshopNewWeapon->ObjectCreationType), GetText("4_units/shot"));

		k2=20;
	}
	if (GetWeaponSystemsDamage(WorkshopNewWeapon->ObjectCreationType) > 0.0f)
	{
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 130+k2, -170, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("4_Damage,_Systems:"));
		if ((WorkshopNewWeapon->ObjectCreationType == 11) |
			(WorkshopNewWeapon->ObjectCreationType == 12) |
			(WorkshopNewWeapon->ObjectCreationType == 14))
			vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 130+k2, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(WorkshopNewWeapon->ObjectCreationType), GetText("4_units/sec"));
		else
			vw_DrawFont(Setup.iAspectRatioWidth/2-438+175, 130+k2, -184, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "%i %s", GetWeaponSystemsDamage(WorkshopNewWeapon->ObjectCreationType), GetText("4_units/shot"));

	}



	// вывод уровня оружия
	if (WorkshopNewWeapon->WeaponLevel <= GetShipWeaponsMaxSlotLevel())
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 400, 0, 0, 1.0f, 0.0f,1.0f,0.0f, MenuContentTransp, "%s: %i", GetText("7_Weapon_Level"), WorkshopNewWeapon->WeaponLevel);
	else
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 400, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", GetText("7_Weapon_Level"), WorkshopNewWeapon->WeaponLevel);

	// вывод стоимости
	if (Setup.Profile[CurrentProfile].Money >= GetWeaponBaseCost(CurrentWorkshopNewWeapon))// всегда ведь новое, считать боекомплект не нужно
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 420, 0, 0, 1.0f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s: %i", GetText("7_Weapon_Cost"), GetWeaponBaseCost(CurrentWorkshopNewWeapon));
	else
		vw_DrawFont(Setup.iAspectRatioWidth/2-438, 420, 0, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", GetText("7_Weapon_Cost"), GetWeaponBaseCost(CurrentWorkshopNewWeapon));



	// рамки
	SetRect(&SrcRest,0,0,400,35 );
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-457,100-11,Setup.iAspectRatioWidth/2-57,100+35-11);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/workshop_panel4.tga"), true, MenuContentTransp);

	SetRect(&SrcRest,0,0,400,173 );
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-457,450-13,Setup.iAspectRatioWidth/2-57,450+173-13);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/workshop_panel1.tga"), true, MenuContentTransp);





	if (DrawButton128_2(Setup.iAspectRatioWidth/2-395,482, GetText("1_Prev"), MenuContentTransp, false))
	{
		CurrentWorkshopNewWeapon --;
		if (CurrentWorkshopNewWeapon < 1) CurrentWorkshopNewWeapon = 19;
		WorkshopCreateNewWeapon();
	}
	if (DrawButton128_2(Setup.iAspectRatioWidth/2-247,482, GetText("1_Next"), MenuContentTransp, false))
	{
		CurrentWorkshopNewWeapon ++;
		if (CurrentWorkshopNewWeapon > 19) CurrentWorkshopNewWeapon = 1;
		WorkshopCreateNewWeapon();
	}


	if (DrawButton128_2(Setup.iAspectRatioWidth/2-395,533, GetText(GetWeaponGroupTitle(PrevWeaponGroup())), MenuContentTransp, false))
	{
		CurrentWorkshopNewWeapon = PrevWeaponGroup();
		WorkshopCreateNewWeapon();
	}
	if (DrawButton128_2(Setup.iAspectRatioWidth/2-247,533, GetText(GetWeaponGroupTitle(NextWeaponGroup())), MenuContentTransp, false))
	{
		CurrentWorkshopNewWeapon = NextWeaponGroup();
		WorkshopCreateNewWeapon();
	}

	vw_DrawFont(Setup.iAspectRatioWidth/2-445, 600, 0, 0, 1.5f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("7_Weapon_Stock"));


	DrawShipWeaponsInSlots();



	vw_DrawFont(Setup.iAspectRatioWidth/2+445-vw_FontSize(GetText("7_Installed_Weapons"))*1.5f, 600, 0, 0, 1.5f, 1.0f,1.0f,1.0f, MenuContentTransp, GetText("7_Installed_Weapons"));

	// вывод информации
	int SizeI = (Setup.iAspectRatioWidth-vw_FontSize("%s: %i", GetText("3_Money"), Setup.Profile[CurrentProfile].Money)*1.25f)/2;
	if (Setup.Profile[CurrentProfile].Money >= GetWeaponBaseCost(CurrentWorkshopNewWeapon))
		vw_DrawFont(SizeI, 630, 0, 0, 1.25f, 1.0f,1.0f,0.0f, MenuContentTransp, "%s: %i", GetText("3_Money"), Setup.Profile[CurrentProfile].Money);
	else
		vw_DrawFont(SizeI, 630, 0, 0, 1.25f, 1.0f,0.0f,0.0f, CurrentAlert3*MenuContentTransp, "%s: %i", GetText("3_Money"), Setup.Profile[CurrentProfile].Money);



	// проверяем состояние, если тянули и отжали, и сюда пришли - значит никто не перехватил, нужно сделать сброс
	if (!vw_GetWindowLBMouse(false) && DragWeapon)
	{
		// звук пропадающего оружия
		Audio_PlaySound2D(11,1.0f);

		// сброс
		DragWeapon = false;
		DragWeaponNum = 0;
		DragWeaponLevel = 0;
		DragWeaponAmmo = 0;
		DragWeaponAmmoStart = 0;
	}



	if (DragWeapon) CurrentCursorStatus = 3;


}



