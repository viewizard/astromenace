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
#include "../object3d/explosion/space_explosion/space_explosion.h"
#include "../object3d/space_object/space_object.h"
#include "../object3d/ground_object/ground_object.h"
#include "../object3d/space_ship/earth_space_fighter/earth_space_fighter.h"

extern cProjectile *StartProjectile;
extern cProjectile *EndProjectile;
extern cSpaceObject *StartSpaceObject;
extern cGroundObject *StartGroundObject;

float GetEnginePower(int EngineType);
float GetEngineAcceleration(int EngineType);
float GetEngineRotatePower(int EngineType);

// режим неубиваемости... отладка
bool UndeadDebugMode = false;

// состояние управления, нужно, чтобы определять что менялось
int LastMouseX = -1;
int LastMouseY = -1;
// для восстановления положения курсора в меню, точнее при выходе из него
int LastMouseXR = 0;
int LastMouseYR = 0;

// в показателях от -1.0f до 1.0f
// назад-вперед
float MoveFB = 0.0f;
// лево-право
float MoveLR = 0.0f;

// текущая энергия корабля
float CurrentPlayerShipEnergy;

// для управления в аркадном режиме маневровыми двигателями
bool PlayerFighterLeftEng = false;
bool PlayerFighterRightEng = false;

cParticleSystem *Shild1 = nullptr;
cParticleSystem *Shild2 = nullptr;
float ShildRadius;
float ShildEnergyStatus;
float ShildStartHitStatus;

// голос с ворнингом, если столкнулись с несбиваемой частью
unsigned int VoiceWarningCollisionDetected{0};

// Номер, для проигрывания голосового сообщения об обнаружении ракеты
unsigned int VoiceMissileDetected{0};
bool VoiceMissileDetectedStatus{false};
// номер, для проигрывания голосового сообщения о проблемах в орудии
// Номер, для проигрывания голосового сообщения об отсутствии снарядов в боекомплекте
unsigned int VoiceWeaponMalfunction{0};
// для звука - мало жизни
unsigned int SoundLowLife{0};


// тут храним какая часть взорвалась на корабле игрока
int PlayerDeadObjectPieceNum;


// симулятивный режим
sVECTOR3D CurrentMovementVel(0.0f, 0.0f, 0.0f);

// работа с морганием вывода
extern float CurrentAlert2;
extern float CurrentAlert3;

// для переменного типа стрельбы
int PrimaryGroupCurrentFireWeaponNum = 1;
float PrimaryGroupCurrentFireWeaponDelay = 0.0f;
int SecondaryGroupCurrentFireWeaponNum = 1;
float SecondaryGroupCurrentFireWeaponDelay = 0.0f;




//------------------------------------------------------------------------------------
// Получаем максимально возможное значение энергии для устройства реактора-батареи
//------------------------------------------------------------------------------------
float GetShipMaxEnergy(int Num)
{
	switch (Num) {
	// аккамулятор
	case 1:
		return 100.0f;
	// ядерный
	case 2:
		return 200.0f;
	// плазменный
	case 3:
		return 400.0f;
	// антиматерия
	case 4:
		return 800.0f;

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return 0.0f;
}
//------------------------------------------------------------------------------------
// Получаем перезарядку энергии, в секунду
//------------------------------------------------------------------------------------
float GetShipRechargeEnergy(int Num)
{
	switch (Num) {
	// аккамулятор
	case 1:
		return 20.0f;
	// ядерный
	case 2:
		return 50.0f;
	// плазменный
	case 3:
		return 130.0f;
	// антиматерия
	case 4:
		return 250.0f;

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return 0.0f;
}
//------------------------------------------------------------------------------------
// Получаем расход энергии в секунду для доп систем (GameAdvancedProtectionSystem)
//------------------------------------------------------------------------------------
float GetShipProtectionSystemEnergyUse(int Num)
{
	switch (Num) {
	// нано роботы
	case 1:
		return 10.0f;
	// спец защитный слой
	case 2:
		return 0.0f;
	// щит
	case 3:
		return 50.0f;
	// отражатель
	case 4:
		return 100.0f;

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return 0.0f;
}
//------------------------------------------------------------------------------------
// Получаем расход энергии в секунду для двигателей GameEngineSystem
//------------------------------------------------------------------------------------
float GetShipEngineSystemEnergyUse(int Num)
{
	switch (Num) {
	// обычные двигатели
	case 1:
		return 5.0f;
	// фатоновые
	case 2:
		return 10.0f;
	// плазменные
	case 3:
		return 30.0f;
	// на антиматерии
	case 4:
		return 60.0f;

	default:
		std::cerr << __func__ << "(): " << "wrong Num.\n";
		break;
	}

	return 0.0f;
}






//------------------------------------------------------------------------------------
// Инициализация корабля игрока
//------------------------------------------------------------------------------------
void InitGamePlayerShip()
{
	// создаем корабль игрока по настройкам в профайле
	if (PlayerFighter != nullptr) {
		delete PlayerFighter;
		PlayerFighter = nullptr;
	}

	VoiceMissileDetected = 0;
	VoiceMissileDetectedStatus = false;
	VoiceWeaponMalfunction = 0;
	SoundLowLife = 0;

	int TMPGameNPCArmorPenalty = GameNPCArmorPenalty;
	GameNPCArmorPenalty = 1;

	// если не создано, здесь будет ноль скорее всего
	if (Setup.Profile[CurrentProfile].Ship == 0)
		std::cerr << __func__ << "(): " << "Error, Pilot Profile not created.\n";

	PlayerFighter = new cEarthSpaceFighter;
	PlayerFighter->Create(Setup.Profile[CurrentProfile].Ship);
	PlayerFighter->DeviationOn = true;
	PlayerFighter->Deviation[0] = sVECTOR3D(0.0f, 0.0f, 1.0f);

	PlayerFighter->ID = -1;
	PlayerFighter->ObjectStatus = 3;
	PlayerFighter->StrengthStart *= Setup.Profile[CurrentProfile].ShipHullUpgrade;
	PlayerFighter->Strength = Setup.Profile[CurrentProfile].ShipHullCurrentStrength;
	PlayerFighter->ShowStrength = false;

	// создаем оружие
	for (int i=0; i<PlayerFighter->WeaponQuantity; i++) {
		if (Setup.Profile[CurrentProfile].Weapon[i] != 0) {
			if (SetEarthSpaceFighterWeapon(PlayerFighter, i+1, Setup.Profile[CurrentProfile].Weapon[i])) {
				PlayerFighter->Weapon[i]->Ammo = Setup.Profile[CurrentProfile].WeaponAmmo[i];
				PlayerFighter->WeaponYAngle[i] = -Setup.Profile[CurrentProfile].WeaponSlotYAngle[i];
			}
		}
	}

	// создаем системы (визуальные)
	SetEarthSpaceFighterEngine(PlayerFighter, GameEngineSystem);

	if (GameAdvancedProtectionSystem == 2)
		SetEarthSpaceFighterArmour(PlayerFighter, 7);
	else
		// установка защитного слоя (синяя броня), который снижает наносимые повреждения
		SetEarthSpaceFighterArmour(PlayerFighter, Setup.Profile[CurrentProfile].ShipHullUpgrade-1);

	GameNPCArmorPenalty = TMPGameNPCArmorPenalty;



	float Width2 = PlayerFighter->Width/2.0f;
	float Length2 = PlayerFighter->Length/2.0f;
	ShildRadius = vw_sqrtf(Width2*Width2+Length2*Length2);
	ShildEnergyStatus = 0.0f;
	ShildStartHitStatus = 0.0f;



	if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem == 3) {
		Shild1 = vw_CreateParticleSystem();
		Shild1->ColorStart.r = 0.20f;
		Shild1->ColorStart.g = 0.50f;
		Shild1->ColorStart.b = 0.10f;
		Shild1->ColorEnd.r = 0.20f;
		Shild1->ColorEnd.g = 0.50f;
		Shild1->ColorEnd.b = 0.10f;
		Shild1->AlphaStart = 1.00f;
		Shild1->AlphaEnd   = 0.00f;
		Shild1->SizeStart  = 0.60f;
		Shild1->SizeVar    = 0.10f;
		Shild1->SizeEnd    = 0.10f;
		Shild1->Speed      = 0.00f;
		Shild1->SpeedOnCreation	   = -1.00f;
		Shild1->Theta      = 360.00f;
		Shild1->Life       = 1.00f;
		Shild1->ParticlesPerSec = (int)(40*ShildRadius);
		Shild1->CreationType = eParticleCreationType::Sphere;
		Shild1->CreationSize = sVECTOR3D(ShildRadius,0.05f*ShildRadius,ShildRadius);
		Shild1->DeadZone = ShildRadius-0.05;
		Shild1->AlphaShowHide = true;
		Shild1->IsMagnet = true;
		Shild1->MagnetFactor = -3.0f;
		Shild1->Texture = vw_FindTextureByName("gfx/flare1.tga");
		Shild1->Direction = sVECTOR3D(0.0f, 0.0f, -1.0f);
		Shild1->SetStartLocation(PlayerFighter->Location+PlayerFighter->OBBLocation);

		ShildStartHitStatus = 100.0f;
		ShildEnergyStatus = 1.0f;
	}
	if (Setup.Profile[CurrentProfile].AdvancedProtectionSystem == 4) {
		Shild1 = vw_CreateParticleSystem();
		Shild1->ColorStart.r = 0.50f;
		Shild1->ColorStart.g = 0.50f;
		Shild1->ColorStart.b = 1.00f;
		Shild1->ColorEnd.r = 0.50f;
		Shild1->ColorEnd.g = 0.50f;
		Shild1->ColorEnd.b = 1.00f;
		Shild1->AlphaStart = 0.50f;
		Shild1->AlphaEnd   = 0.00f;
		Shild1->SizeStart  = 0.40f;
		Shild1->SizeVar    = 0.10f;
		Shild1->SizeEnd    = 0.20f;
		Shild1->Speed      = 0.00f;
		Shild1->SpeedOnCreation	   = -1.00f;
		Shild1->Theta      = 360.00f;
		Shild1->Life       = 1.00f;
		Shild1->ParticlesPerSec = (int)(40*ShildRadius);
		Shild1->CreationType = eParticleCreationType::Sphere;
		Shild1->CreationSize = sVECTOR3D(ShildRadius,0.05f*ShildRadius,ShildRadius);
		Shild1->DeadZone = ShildRadius-0.05;
		Shild1->IsMagnet = true;
		Shild1->AlphaShowHide = true;
		Shild1->MagnetFactor = 2.5f;
		Shild1->Texture = vw_FindTextureByName("gfx/flare1.tga");
		Shild1->Direction = sVECTOR3D(0.0f, 0.0f, -1.0f);
		Shild1->SetStartLocation(PlayerFighter->Location+PlayerFighter->OBBLocation);

		Shild2 = vw_CreateParticleSystem();
		Shild2->ColorStart.r = 0.50f;
		Shild2->ColorStart.g = 0.50f;
		Shild2->ColorStart.b = 1.00f;
		Shild2->ColorEnd.r = 0.50f;
		Shild2->ColorEnd.g = 0.50f;
		Shild2->ColorEnd.b = 1.00f;
		Shild2->AlphaStart = 0.70f;
		Shild2->AlphaEnd   = 0.10f;
		Shild2->SizeStart  = 0.50f;
		Shild2->SizeVar    = 0.10f;
		Shild2->SizeEnd    = 0.30f;
		Shild2->Speed      = 0.00f;
		Shild2->SpeedOnCreation	   = -1.00f;
		Shild2->Theta      = 360.00f;
		Shild2->Life       = 1.00f;
		Shild2->ParticlesPerSec = (int)(5*ShildRadius);
		Shild2->CreationType = eParticleCreationType::Sphere;
		Shild2->CreationSize = sVECTOR3D(ShildRadius,0.05f*ShildRadius,ShildRadius);
		Shild2->DeadZone = ShildRadius-0.05;
		Shild2->IsMagnet = true;
		Shild2->MagnetFactor = 20.0f;
		Shild2->Texture = vw_FindTextureByName("gfx/flare1.tga");
		Shild2->Direction = sVECTOR3D(0.0f, 0.0f, -1.0f);

		ShildStartHitStatus = 150.0f;
		ShildEnergyStatus = 1.0f;
	}



	// предварительная установка, полностью заряженное устройство
	CurrentPlayerShipEnergy = GetShipMaxEnergy(GamePowerSystem);

	// предварительно иним состояния управления
	LastMouseX = -1;
	LastMouseY = -1;
	MoveFB = 0.0f;
	MoveLR = 0.0f;
	CurrentMovementVel = sVECTOR3D(0.0f, 0.0f, 0.0f);

	// сброс стрельбы...
	PrimaryGroupCurrentFireWeaponNum = 1;
	PrimaryGroupCurrentFireWeaponDelay = 0.0f;
	SecondaryGroupCurrentFireWeaponNum = 1;
	SecondaryGroupCurrentFireWeaponDelay = 0.0f;
}








//------------------------------------------------------------------------------------
// Основная процедура обработки состояния корабля игрока
//------------------------------------------------------------------------------------
void GamePlayerShip()
{
	// если корабля нет - нам тут делать нечего
	if (PlayerFighter == nullptr)
		return;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем, корабль живой еще, или сбили и нужно его удалить...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (PlayerFighter->Strength <= 0.0f) {
		// редкий случай
		if (UndeadDebugMode) {
			PlayerFighter->Strength = PlayerFighter->StrengthStart;
		} else {
			// делаем взрыв
			// + 10.0f движение камеры
			new cSpaceExplosion(PlayerFighter, 31, PlayerFighter->Location, PlayerFighter->Speed+10.0f, PlayerDeadObjectPieceNum);

			// включаем музыку и отображение "миссия провалена"
			StartMusicWithFade(eMusicTheme::FAILED, 2000, 2000);

			// удаляем и уходим отсюда
			delete PlayerFighter;
			PlayerFighter = nullptr;

			// 20 секунд просто проигрываем музыку...
			GameSetMissionFailedData(20);

			return;
		}
	}



	// голос выводим только в игре! в меню включается пауза
	// и если не закончился уровень
	if (GameContentTransp < 0.99f && !GameMissionCompleteStatus) {
		int WarningMessagesCount = 0;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Вывод голосового предупреждения, если навелась ракета
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		cProjectile *tmpProjectile = StartProjectile;
		bool CheckStatus = false;
		while (tmpProjectile != nullptr) {
			cProjectile *tmpProjectile2 = tmpProjectile->Next;
			// если навелись на этот объект ракетой
			// т.к. только у ракеты тут не ноль
			if (tmpProjectile->Target == PlayerFighter)
				// если это не мина
				if (tmpProjectile->Num < 26 || tmpProjectile->Num > 29) {
					// проверяем, есть ли ракеты или нет
					CheckStatus = true;
				}

			tmpProjectile = tmpProjectile2;
		}

		if (CheckStatus) {
			// проверяем, действительно еще играем (играем только 1 раз!)
			if (!vw_IsSoundAvailable(VoiceMissileDetected) &&
			    !VoiceMissileDetectedStatus) {
				// уже не играем, нужно запустить опять
				VoiceMissileDetected = Audio_PlayVoice(3, 1.0f);
				VoiceMissileDetectedStatus = true;
			}

			// визуальный вывод - выводим постоянно
			vw_SetFontSize(24);
			vw_SetFontOffsetY(2);
			int TmpFontSize = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("4_Missile_Detected")))/2;
			vw_DrawFont(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3, vw_GetText("4_Missile_Detected"));
			vw_SetFontSize(Setup.FontSize);
			vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);
			WarningMessagesCount++;
		} else {
			if (CurrentAlert3 == 1.0f) { // сделали полный цикл , предыдущее значение счетчика было минимальное
				VoiceMissileDetectedStatus = false;
			} else if (VoiceMissileDetectedStatus) {
				// визуальный вывод - выводим постоянно
				vw_SetFontSize(24);
				vw_SetFontOffsetY(2);
				int TmpFontSize = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("4_Missile_Detected")))/2;
				vw_DrawFont(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, 1.0f,0.5f,0.0f, CurrentAlert3, vw_GetText("4_Missile_Detected"));
				vw_SetFontSize(Setup.FontSize);
				vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);
				WarningMessagesCount++;
			}
		}



		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Вывод голосового предупреждения если возможно столкновение
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		bool CollisionDetected = false;
		cSpaceObject *tmpS = StartSpaceObject;
		while ((tmpS != nullptr) && (PlayerFighter != nullptr)) {
			cSpaceObject *tmpSpace2 = tmpS->Next;

			// проверка на возможное столкновение с неразрушаемым объектом, вывод голосового предупреждения
			// если объект ближе чем радиус, кричим
			if (tmpS->ObjectType == 15) // только с большими астероидами
				if (vw_SphereSphereCollision(PlayerFighter->Radius, PlayerFighter->Location,
							     tmpS->Radius, tmpS->Location, tmpS->PrevLocation))
					if (vw_SphereAABBCollision(tmpS->AABB, tmpS->Location,
								   PlayerFighter->Radius, PlayerFighter->Location, PlayerFighter->PrevLocation)) {
						CollisionDetected = true;
					}

			tmpS = tmpSpace2;
		}
		cGroundObject *tmpG = StartGroundObject;
		while ((tmpG != nullptr) && (PlayerFighter != nullptr)) {
			cGroundObject *tmpGround2 = tmpG->Next;

			// проверка на возможное столкновение с неразрушаемым объектом, вывод голосового предупреждения
			// если объект ближе чем радиус, кричим
			if (tmpG->ObjectType == 12) // только со зданиями (чтобы не цепляло блоки конструкций баз)
				if (vw_SphereSphereCollision(PlayerFighter->Radius, PlayerFighter->Location,
							     tmpG->Radius, tmpG->Location, tmpG->PrevLocation))
					if (vw_SphereAABBCollision(tmpG->AABB, tmpG->Location,
								   PlayerFighter->Radius, PlayerFighter->Location, PlayerFighter->PrevLocation))
						if (vw_SphereOBBCollision(tmpG->OBB, tmpG->OBBLocation, tmpG->Location, tmpG->CurrentRotationMat,
									  PlayerFighter->Radius, PlayerFighter->Location, PlayerFighter->PrevLocation)) {
							CollisionDetected = true;
						}

			tmpG = tmpGround2;
		}
		if (CollisionDetected) {
			// голос, ворнинг, можем столкнуться с объектом
			// проверяем, действительно еще играем
			if (!vw_IsSoundAvailable(VoiceWarningCollisionDetected)) {
				// уже не играем, нужно запустить опять
				VoiceWarningCollisionDetected = Audio_PlayVoice(7, 1.0f);
			}

			// визуальный вывод - выводим постоянно
			vw_SetFontSize(24);
			vw_SetFontOffsetY(2);
			int TmpFontSize = (Setup.iAspectRatioWidth-vw_FontSize(vw_GetText("4_Collision_Course_Detected")))/2;
			vw_DrawFont(TmpFontSize, 720 - 40*WarningMessagesCount, 0, 0, 1.0f, 1.0f,0.0f,0.0f, CurrentAlert3, vw_GetText("4_Collision_Course_Detected"));
			vw_SetFontSize(Setup.FontSize);
			vw_SetFontOffsetY(8 - ceil(Setup.FontSize/2.0f) + 2);
			WarningMessagesCount++;
		}





		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Вывод голосового предупреждения, если в оружие нет пуль
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (PlayerFighter->WeaponSetFire != nullptr) // если вообще есть оружие
			for (int i = 0; i < PlayerFighter->WeaponQuantity; i++) {
				// если нажали стрелять, а патронов нет в одном из орудий
				if ((PlayerFighter->Weapon[i] != nullptr) &&
				    (PlayerFighter->WeaponSetFire[i]) &&
				    (PlayerFighter->Weapon[i]->Ammo <= 0)) {
					// проверяем, действительно еще играем
					if (!vw_IsSoundAvailable(VoiceWeaponMalfunction)) {
						// уже не играем, нужно запустить опять
						VoiceWeaponMalfunction = Audio_PlayVoice(10, 1.0f);
					}
				}

			}


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Звуковое оповещение, если жизни менее 10%
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// если меньше 10% нужно бить тревогу
		if (PlayerFighter->Strength < PlayerFighter->StrengthStart/10.0f) {
			// если не играем, запускаем звук сирены
			if (!vw_IsSoundAvailable(SoundLowLife))
				SoundLowLife = Audio_PlaySound2D(14, 1.0f);
		}
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// управление кораблем - движение
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (GameContentTransp < 1.0f) { // если не в меню нажимают
		// получаем данные, для обоих типов управления
		// уже получили данные, нужно игнорировать остальные источникик
		bool NeedSkip = false;

		// мышка
		if (Setup.MouseControl) {
			SDL_GetMouseState(&LastMouseXR, &LastMouseYR);

			int X, Y;
			vw_GetMousePos(X, Y);
			if (LastMouseX == -1 && LastMouseY == -1) {
				LastMouseX = X;
				LastMouseY = Y;
			} else {
				if (X != LastMouseX || Y != LastMouseY) {
					// 0.9+0.1 = 1.0 - минимум, всегда 1.0 должен быть!
					float Koef = 0.9f+Setup.ControlSensivity/10.0f;

					// при любом реальном разрешении у нас x и y меняются с учетом AspectRatio
					float AWw2 = Setup.iAspectRatioWidth/2.0f;
					float AHw2 = Setup.iAspectRatioHeight/2.0f;

					MoveFB += (-(Y-LastMouseY)/AHw2)*Koef;
					MoveLR += ( (X-LastMouseX)/AWw2)*Koef;

					NeedSkip = true;
				}

				LastMouseX = X;
				LastMouseY = Y;
			}

		}


#ifdef joystick
		// джойстик
		if (Joystick != nullptr && !NeedSkip) {
			int X, Y;
			X = SDL_JoystickGetAxis(Joystick, 0);
			Y = SDL_JoystickGetAxis(Joystick, 1);


			if (JoystickAxisX != X || JoystickAxisY != Y) {
				JoystickAxisX = 0;
				JoystickAxisY = 0;

				// скорость изменения
				float AccMoveFB = fabsf(Y/32768.0f);
				float AccMoveLR = fabsf(X/32768.0f);

				// -3000 3000 мертвая зона
				if (Y > 3000) {
					MoveFB -= 2.0f*(Setup.ControlSensivity/10.0f)*AccMoveFB*PlayerFighter->TimeDelta;
					NeedSkip = true;
				}
				if (Y < -3000) {
					MoveFB += 2.0f*(Setup.ControlSensivity/10.0f)*AccMoveFB*PlayerFighter->TimeDelta;
					NeedSkip = true;
				}
				if (X < -3000) {
					MoveLR -= 2.0f*(Setup.ControlSensivity/10.0f)*AccMoveLR*PlayerFighter->TimeDelta;
					NeedSkip = true;
				}
				if (X > 3000) {
					MoveLR += 2.0f*(Setup.ControlSensivity/10.0f)*AccMoveLR*PlayerFighter->TimeDelta;
					NeedSkip = true;
				}
			}
		}
#endif

		// клавиатура
		if (!NeedSkip) {
			if (vw_GetKeyStatus(Setup.KeyBoardDown)) MoveFB -= 2.0f*(Setup.ControlSensivity/10.0f)*PlayerFighter->TimeDelta;
			if (vw_GetKeyStatus(Setup.KeyBoardUp)) MoveFB += 2.0f*(Setup.ControlSensivity/10.0f)*PlayerFighter->TimeDelta;
			if (vw_GetKeyStatus(Setup.KeyBoardLeft)) MoveLR -= 2.0f*(Setup.ControlSensivity/10.0f)*PlayerFighter->TimeDelta;
			if (vw_GetKeyStatus(Setup.KeyBoardRight)) MoveLR += 2.0f*(Setup.ControlSensivity/10.0f)*PlayerFighter->TimeDelta;
		}



		// дополнительная проверка, т.к. можем выйти выйти за пределы
		if (MoveFB < -1.0f) MoveFB = -1.0f;
		if (MoveFB > 1.0f) MoveFB = 1.0f;
		if (MoveLR < -1.0f) MoveLR = -1.0f;
		if (MoveLR > 1.0f) MoveLR = 1.0f;






		// находим конечную точку перемещения
		sVECTOR3D PlayerFighterEndLocation;
		if (Setup.iAspectRatioWidth == 1024)
			PlayerFighterEndLocation = sVECTOR3D(-(73.15f-PlayerFighter->Width/2.0f+MoveFB*(20.05f-PlayerFighter->Length/6.0f))*MoveLR, 0.0f, (46.0f-PlayerFighter->Length/2.0f)*MoveFB);
		else
			PlayerFighterEndLocation = sVECTOR3D(-(70.0f-PlayerFighter->Width/2.0f+MoveFB*(23.2f-PlayerFighter->Length/6.0f))*MoveLR, 0.0f, (46.0f-PlayerFighter->Length/2.0f)*MoveFB);

		// учет положение камеры
		PlayerFighterEndLocation += GamePoint;

		// если есть двигатель
		if (GameEngineSystem != 0) {
			// в зависимости от типа управления выполняем действия
			if (Setup.Profile[CurrentProfile].SpaceShipControlMode == 1) {
				// аркадный режим

				// запускаем маневровые двигатели, если тянем корабль в сторону
				if ((int)PlayerFighter->Location.x > (int)PlayerFighterEndLocation.x) {
					PlayerFighterLeftEng = true;
					PlayerFighterRightEng = false;
				}
				if ((int)PlayerFighter->Location.x < (int)PlayerFighterEndLocation.x) {
					PlayerFighterLeftEng = false;
					PlayerFighterRightEng = true;
				}
				// если не двигаем, останавливаем маневровые двигатели
				if ((int)PlayerFighter->Location.x == (int)PlayerFighterEndLocation.x) {
					PlayerFighterLeftEng = false;
					PlayerFighterRightEng = false;
				}


				// находим расстояние
				sVECTOR3D PlayerFighterNewDirection = PlayerFighterEndLocation - PlayerFighter->Location;
				float EndLocationDistance = PlayerFighterNewDirection.Length();

				// находим направление движения
				PlayerFighterNewDirection.Normalize();

				float SimMoveSpeed = EndLocationDistance;

				if (SimMoveSpeed > 30.0f) SimMoveSpeed = 30.0f;

				SimMoveSpeed = SimMoveSpeed*4.0f*PlayerFighter->TimeDelta;


				// получаем текущее движение
				CurrentMovementVel = PlayerFighterNewDirection^SimMoveSpeed;

				// проверка
				float MaxSpeed = CurrentMovementVel.Length();
				CurrentMovementVel.Normalize();
				if (MaxSpeed > 30.0f) MaxSpeed = 30.0f;

				CurrentMovementVel = CurrentMovementVel^MaxSpeed;

			} else {
				// симулятивный режим


				// запускаем маневровые двигатели, если тянем корабль в сторону
				if ((int)PlayerFighter->Location.x > (int)PlayerFighterEndLocation.x) {
					PlayerFighterLeftEng = true;
					PlayerFighterRightEng = false;
				}
				if ((int)PlayerFighter->Location.x < (int)PlayerFighterEndLocation.x) {
					PlayerFighterLeftEng = false;
					PlayerFighterRightEng = true;
				}
				// если не двигаем, останавливаем маневровые двигатели
				if ((int)PlayerFighter->Location.x == (int)PlayerFighterEndLocation.x) {
					PlayerFighterLeftEng = false;
					PlayerFighterRightEng = false;
				}


				// находим расстояние
				sVECTOR3D PlayerFighterNewDirection = PlayerFighterEndLocation - PlayerFighter->Location;
				float EndLocationDistance = PlayerFighterNewDirection.Length();

				// находим направление движения
				PlayerFighterNewDirection.Normalize();

				float SimMoveSpeed = EndLocationDistance;

				if (SimMoveSpeed > PlayerFighter->MaxSpeed)
					SimMoveSpeed = PlayerFighter->MaxSpeed;

				SimMoveSpeed = SimMoveSpeed*(PlayerFighter->MaxAcceler/14.0f)*PlayerFighter->TimeDelta;


				// получаем текущее движение
				CurrentMovementVel = PlayerFighterNewDirection^SimMoveSpeed;

				// проверка
				float MaxSpeed = CurrentMovementVel.Length();
				CurrentMovementVel.Normalize();
				if (MaxSpeed > PlayerFighter->MaxSpeed)
					MaxSpeed = PlayerFighter->MaxSpeed;

				CurrentMovementVel = CurrentMovementVel^MaxSpeed;

			}
		}

		// переносим корабль
		sVECTOR3D CurrentVel = PlayerFighter->Location + CurrentMovementVel;
		CurrentVel.y = 0.0f;
		PlayerFighter->SetLocationArcadePlayer(CurrentVel);



		// если стандартный аспект рейшен, надо смещать камеру
		if (Setup.iAspectRatioWidth == 1024)
			if (Setup.CameraModeWithStandardAspectRatio == 0) {
				float DeviationSize = 14.55f;


				if (PlayerFighter->Location.x < 0.0f) {
					float Diff = PlayerFighter->Location.x/3.5f;
					if (Diff < -DeviationSize) Diff = -DeviationSize;

					sVECTOR3D TMPCameraLocation;
					vw_GetCameraLocation(&TMPCameraLocation);
					TMPCameraLocation.x = Diff;
					vw_SetCameraLocation(TMPCameraLocation);
				} else {
					float Diff = PlayerFighter->Location.x/3.5f;
					if (Diff > DeviationSize) Diff = DeviationSize;

					sVECTOR3D TMPCameraLocation;
					vw_GetCameraLocation(&TMPCameraLocation);
					TMPCameraLocation.x = Diff;
					vw_SetCameraLocation(TMPCameraLocation);
				}
			}

	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// управление кораблем - стрельба
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (GameContentTransp < 0.5f) // если не в меню нажимают
		if (PlayerFighter->WeaponSetFire != nullptr) { // если вообще есть оружие

			int PrimCount = 0;
			float PrimTime = 0.0f;
			int SecCount = 0;
			float SecTime = 0.0f;

			PrimaryGroupCurrentFireWeaponDelay -= PlayerFighter->TimeDelta;
			SecondaryGroupCurrentFireWeaponDelay -= PlayerFighter->TimeDelta;

			// находим кол-во оружия в группах
			for (int i=0; i<PlayerFighter->WeaponQuantity; i++)
				if (Setup.Profile[CurrentProfile].Weapon[i] != 0) { // если это оружие установлено

					if (Setup.Profile[CurrentProfile].WeaponControl[i] == 1 ||
					    Setup.Profile[CurrentProfile].WeaponControl[i] ==3) {
						PrimCount++;
						PrimTime += PlayerFighter->Weapon[i]->NextFireTime;
					}

					if (Setup.Profile[CurrentProfile].WeaponControl[i] == 2 ||
					    Setup.Profile[CurrentProfile].WeaponControl[i] ==3) {
						SecCount++;
						SecTime += PlayerFighter->Weapon[i]->NextFireTime;
					}

				}


			int PrimNum = 0;
			int SecNum = 0;

			for (int i=0; i<PlayerFighter->WeaponQuantity; i++)
				if (Setup.Profile[CurrentProfile].Weapon[i] != 0) { // если это оружие установлено

					PlayerFighter->WeaponSetFire[i] = false;

					// получаем данные, в какую группу относится
					bool primary_fire = false;
					bool secondary_fire = false;
					if (Setup.Profile[CurrentProfile].WeaponControl[i] == 1 ||
					    Setup.Profile[CurrentProfile].WeaponControl[i] ==3) primary_fire = true;
					if (Setup.Profile[CurrentProfile].WeaponControl[i] == 2 ||
					    Setup.Profile[CurrentProfile].WeaponControl[i] ==3) secondary_fire = true;

					// мышка
					if (Setup.MouseControl) {
						// primary fire
						if (primary_fire)
							if (SDL_MouseCurrentStatus[Setup.MousePrimary]) {
								if (Setup.Profile[CurrentProfile].PrimaryWeaponFireMode == 1) {
									PlayerFighter->WeaponSetFire[i] = true;
								} else {
									PrimNum++;
									if (PrimaryGroupCurrentFireWeaponNum == PrimNum &&
									    PrimaryGroupCurrentFireWeaponDelay <= 0.0f) {
										PrimaryGroupCurrentFireWeaponDelay = PrimTime/(PrimCount*PrimCount);
										PlayerFighter->WeaponSetFire[i] = true;
										PrimaryGroupCurrentFireWeaponNum ++;
										if (PrimaryGroupCurrentFireWeaponNum > PrimCount) PrimaryGroupCurrentFireWeaponNum = 1;
									}
								}
							}

						// secondary fire
						if (secondary_fire)
							if (SDL_MouseCurrentStatus[Setup.MouseSecondary]) {
								if (Setup.Profile[CurrentProfile].SecondaryWeaponFireMode == 1) {
									PlayerFighter->WeaponSetFire[i] = true;
								} else {
									SecNum++;
									if (SecondaryGroupCurrentFireWeaponNum == SecNum &&
									    SecondaryGroupCurrentFireWeaponDelay <= 0.0f) {
										SecondaryGroupCurrentFireWeaponDelay = SecTime/(SecCount*SecCount);
										PlayerFighter->WeaponSetFire[i] = true;
										SecondaryGroupCurrentFireWeaponNum ++;
										if (SecondaryGroupCurrentFireWeaponNum > SecCount) SecondaryGroupCurrentFireWeaponNum = 1;
									}
								}
							}

						// альтернативное управление
						if (Setup.Profile[CurrentProfile].WeaponAltControl[i] == 2)
							if (SDL_MouseCurrentStatus[Setup.Profile[CurrentProfile].WeaponAltControlData[i]])
								PlayerFighter->WeaponSetFire[i] = true;
					}


#ifdef joystick
					// джойстик
					if (Joystick != nullptr) {
						// primary fire
						if (primary_fire)
							if (SDL_JoystickGetButton(Joystick, Setup.JoystickPrimary) == 1) {
								if (Setup.Profile[CurrentProfile].PrimaryWeaponFireMode == 1) {
									PlayerFighter->WeaponSetFire[i] = true;
								} else {
									PrimNum++;
									if (PrimaryGroupCurrentFireWeaponNum == PrimNum &&
									    PrimaryGroupCurrentFireWeaponDelay <= 0.0f) {
										PrimaryGroupCurrentFireWeaponDelay = PrimTime/(PrimCount*PrimCount);
										PlayerFighter->WeaponSetFire[i] = true;
										PrimaryGroupCurrentFireWeaponNum ++;
										if (PrimaryGroupCurrentFireWeaponNum > PrimCount) PrimaryGroupCurrentFireWeaponNum = 1;
									}
								}
							}

						// secondary fire
						if (secondary_fire)
							if (SDL_JoystickGetButton(Joystick, Setup.JoystickSecondary) == 1) {
								if (Setup.Profile[CurrentProfile].SecondaryWeaponFireMode == 1) {
									PlayerFighter->WeaponSetFire[i] = true;
								} else {
									SecNum++;
									if (SecondaryGroupCurrentFireWeaponNum == SecNum &&
									    SecondaryGroupCurrentFireWeaponDelay <= 0.0f) {
										SecondaryGroupCurrentFireWeaponDelay = SecTime/(SecCount*SecCount);
										PlayerFighter->WeaponSetFire[i] = true;
										SecondaryGroupCurrentFireWeaponNum ++;
										if (SecondaryGroupCurrentFireWeaponNum > SecCount) SecondaryGroupCurrentFireWeaponNum = 1;
									}
								}
							}

						// альтернативное управление
						if (Setup.Profile[CurrentProfile].WeaponAltControl[i] == 3)
							if (SDL_JoystickGetButton(Joystick, Setup.Profile[CurrentProfile].WeaponAltControlData[i]) == 1)
								PlayerFighter->WeaponSetFire[i] = true;
					}
#endif

					// клавиатура

					// primary fire
					if (primary_fire)
						if (vw_GetKeyStatus(Setup.KeyBoardPrimary)) {
							if (Setup.Profile[CurrentProfile].PrimaryWeaponFireMode == 1) {
								PlayerFighter->WeaponSetFire[i] = true;
							} else {
								PrimNum++;
								if (PrimaryGroupCurrentFireWeaponNum == PrimNum &&
								    PrimaryGroupCurrentFireWeaponDelay <= 0.0f) {
									PrimaryGroupCurrentFireWeaponDelay = PrimTime/(PrimCount*PrimCount);
									PlayerFighter->WeaponSetFire[i] = true;
									PrimaryGroupCurrentFireWeaponNum ++;
									if (PrimaryGroupCurrentFireWeaponNum > PrimCount) PrimaryGroupCurrentFireWeaponNum = 1;
								}
							}
						}

					// secondary fire
					if (secondary_fire)
						if (vw_GetKeyStatus(Setup.KeyBoardSecondary)) {
							if (Setup.Profile[CurrentProfile].SecondaryWeaponFireMode == 1) {
								PlayerFighter->WeaponSetFire[i] = true;
							} else {
								SecNum++;
								if (SecondaryGroupCurrentFireWeaponNum == SecNum &&
								    SecondaryGroupCurrentFireWeaponDelay <= 0.0f) {
									SecondaryGroupCurrentFireWeaponDelay = SecTime/(SecCount*SecCount);
									PlayerFighter->WeaponSetFire[i] = true;
									SecondaryGroupCurrentFireWeaponNum ++;
									if (SecondaryGroupCurrentFireWeaponNum > SecCount) SecondaryGroupCurrentFireWeaponNum = 1;
								}
							}
						}

					// альтернативное управление
					if (Setup.Profile[CurrentProfile].WeaponAltControl[i] == 1)
						if (vw_GetKeyStatus(Setup.Profile[CurrentProfile].WeaponAltControlData[i]))
							PlayerFighter->WeaponSetFire[i] = true;

				}

		}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// управление и работа внутренних систем корабля
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// сделать:
	// учитывать, как работает двигатель... стоим или летим...
	// если не аркадный режим...
	if (GameSpaceShipControlMode != 1) {
		if (CurrentPlayerShipEnergy < GetShipEngineSystemEnergyUse(GameEngineSystem)*PlayerFighter->TimeDelta) {
			PlayerFighter->MaxSpeed = 0.0f;
			PlayerFighter->MaxAcceler = 0.0f;
			PlayerFighter->MaxSpeedRotate = 0.0f;
			// глушим двигатели
			for (auto tmpEngine : PlayerFighter->Engines) {
				if (tmpEngine)
					tmpEngine->IsSuppressed = true;
			}
			if (!PlayerFighter->EnginesLeft.empty()) {
				for (auto tmpEngineLeft : PlayerFighter->EnginesLeft) {
					if (tmpEngineLeft)
						tmpEngineLeft->IsSuppressed = true;
				}
			}
			if (!PlayerFighter->EnginesRight.empty()) {
				for (auto EngineRight : PlayerFighter->EnginesRight) {
					if (EngineRight)
						EngineRight->IsSuppressed = true;
				}
			}
		} else {
			PlayerFighter->MaxSpeed = GetEnginePower(GameEngineSystem)*2.0f - PlayerFighter->Weight/1000.0f;
			PlayerFighter->MaxAcceler = GetEngineAcceleration(GameEngineSystem)*2.0f - PlayerFighter->Weight/1000.0f;
			PlayerFighter->MaxSpeedRotate = GetEngineRotatePower(GameEngineSystem)*2.0f - PlayerFighter->Weight/1000.0f;
			// запускаем прорисовку
			for (auto tmpEngine : PlayerFighter->Engines) {
				if (tmpEngine)
					tmpEngine->IsSuppressed = false;
			}
			if (!PlayerFighter->EnginesLeft.empty()) {
				for (auto tmpEngineLeft : PlayerFighter->EnginesLeft) {
					if (tmpEngineLeft)
						tmpEngineLeft->IsSuppressed = false;
				}
			}
			if (!PlayerFighter->EnginesRight.empty()) {
				for (auto tmpEngineRight : PlayerFighter->EnginesRight) {
					if (tmpEngineRight)
						tmpEngineRight->IsSuppressed = false;
				}
			}
			CurrentPlayerShipEnergy -= GetShipEngineSystemEnergyUse(GameEngineSystem)*PlayerFighter->TimeDelta;
		}
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// энергия для перезарядки и выстрела...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// сейчас получаем всю энергию для перезарядки и выстрела
	// потом лучше будет переделать на постепенный отбор энергии
	for (int i=0; i<PlayerFighter->WeaponQuantity; i++)
		if (Setup.Profile[CurrentProfile].Weapon[i] != 0) {
			if (PlayerFighter->Weapon[i]->CurrentEnergyAccumulated < PlayerFighter->Weapon[i]->EnergyUse) {
				// если энергии не достаточно для зарядки орудия
				if (CurrentPlayerShipEnergy < PlayerFighter->Weapon[i]->EnergyUse) {
					// останавливаем перезарядку оружия
					PlayerFighter->Weapon[i]->LastFireTime += PlayerFighter->TimeDelta;
					PlayerFighter->Weapon[i]->Fire->IsSuppressed = true;
				} else {
					// если энергии достаточно, все нормально берем ее и перезаряжаем оружие
					PlayerFighter->Weapon[i]->CurrentEnergyAccumulated = PlayerFighter->Weapon[i]->EnergyUse;
					CurrentPlayerShipEnergy -= PlayerFighter->Weapon[i]->EnergyUse;
				}
			}
		}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// питание других (защитных) систем
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (CurrentPlayerShipEnergy >= GetShipEngineSystemEnergyUse(GameEngineSystem)*PlayerFighter->TimeDelta) {

		switch (GameAdvancedProtectionSystem) {
		// нано роботы
		case 1:
			// восстанавливаем на 0.5% в секунду
			if (PlayerFighter->Strength < PlayerFighter->StrengthStart) {
				CurrentPlayerShipEnergy -= GetShipProtectionSystemEnergyUse(GameAdvancedProtectionSystem)*PlayerFighter->TimeDelta;
				PlayerFighter->Strength += (PlayerFighter->StrengthStart/200.0f) * PlayerFighter->TimeDelta;
				if (PlayerFighter->Strength > PlayerFighter->StrengthStart) PlayerFighter->Strength = PlayerFighter->StrengthStart;
			}
			break;
		// спец защитный слой
		case 2:
			break; // ничего не делаем
		// щит
		case 3:
			// восстанавливаем полностью за 4 секунды
			if (ShildEnergyStatus < 1.0f) {
				CurrentPlayerShipEnergy -= GetShipProtectionSystemEnergyUse(GameAdvancedProtectionSystem)*PlayerFighter->TimeDelta;
				ShildEnergyStatus += 0.02f*PlayerFighter->TimeDelta;
				if (ShildEnergyStatus > 1.0f) ShildEnergyStatus = 1.0f;
			}
			break;
		// отражатель
		case 4:
			// восстанавливаем полностью за 2 секунды
			if (ShildEnergyStatus < 1.0f) {
				CurrentPlayerShipEnergy -= GetShipProtectionSystemEnergyUse(GameAdvancedProtectionSystem)*PlayerFighter->TimeDelta;
				ShildEnergyStatus += 0.03f*PlayerFighter->TimeDelta;
				if (ShildEnergyStatus > 1.0f) ShildEnergyStatus = 1.0f;
			}
			break;
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// управление визуализацией щитов-дефлекторов
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Shild1 != nullptr) {
		Shild1->MoveSystem(PlayerFighter->Location+PlayerFighter->OBBLocation);
		Shild1->SetStartLocation(PlayerFighter->Location+PlayerFighter->OBBLocation);
		Shild1->RotateSystemAndParticlesByAngle(PlayerFighter->Rotation);
		Shild1->ParticlesPerSec = (int)(40*ShildEnergyStatus*ShildRadius);
	}
	if (Shild2 != nullptr) {
		Shild2->MoveSystem(PlayerFighter->Location+PlayerFighter->OBBLocation);
		Shild2->SetStartLocation(PlayerFighter->Location+PlayerFighter->OBBLocation);
		Shild2->RotateSystemAndParticlesByAngle(PlayerFighter->Rotation);
		Shild2->ParticlesPerSec = (int)(5*ShildEnergyStatus*ShildRadius);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если реактор - можем генерировать энергию, если баттарея - нет
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	CurrentPlayerShipEnergy += GetShipRechargeEnergy(GamePowerSystem)*PlayerFighter->TimeDelta;
	if (CurrentPlayerShipEnergy > GetShipMaxEnergy(GamePowerSystem)) CurrentPlayerShipEnergy = GetShipMaxEnergy(GamePowerSystem);

}






