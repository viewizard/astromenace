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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../game.h"
#include "script.h"
#include "../gfx/star_system.h"
#include "../object3d/object3d.h"
#include "../object3d/space_ship/space_ship.h"
#include "../object3d/ground_object/ground_object.h"
#include "../object3d/space_object/asteroid/asteroid.h"
#include "../object3d/space_object/planet/planet.h"
#include "../object3d/space_ship/earth_space_fighter/earth_space_fighter.h"
#include "../object3d/space_ship/alien_space_fighter/alien_space_fighter.h"
#include "../object3d/space_ship/alien_space_mothership/alien_space_mothership.h"
#include "../object3d/space_ship/pirate_ship/pirate_ship.h"
#include "../object3d/space_object/base_part/base_part.h"
#include "../object3d/space_object/big_asteroid/big_asteroid.h"
#include "../object3d/ground_object/military_building/military_building.h"
#include "../object3d/ground_object/building/building.h"
#include "../object3d/ground_object/wheeled/wheeled.h"
#include "../object3d/ground_object/tracked/tracked.h"


void SetID(cObject3D *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc);
void SetDebugInformation(cObject3D *Object, sXMLEntry *xmlEntry);
void SetShowDeleteOnHide(cObject3D *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc);
void SetShipRotation(cSpaceShip *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc);
void SetShipLocation(cSpaceShip *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc, float TimeOpLag);
void SetProjectileRotation(cProjectile *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc);
void SetProjectileLocation(cProjectile *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc, float TimeOpLag);
void SetRotation(cObject3D *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc);
void SetLocation(cObject3D *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc, float TimeOpLag);
void SetAIMode(cObject3D *Object, sXMLEntry *xmlEntry, const std::unique_ptr<cXMLDocument> &xmlDoc);




// проверка, если конец уровня как всех убъем

extern cSpaceShip *StartSpaceShip;
extern cSpaceShip *EndSpaceShip;
extern cGroundObject *StartGroundObject;
extern cGroundObject *EndGroundObject;

// отображение коробок... отладка
extern int NeedShowBB;
// неубиваемость... отладка
extern bool UndeadDebugMode;
// показывать время при скорости 1.5
extern bool ShowGameTime;






cScriptEngine::cScriptEngine()
{
	// отладочный режим
	NeedShowBB = 0;
	UndeadDebugMode = false;
};


//-----------------------------------------------------------------------------
// запустить скрипт на выполнение
//-----------------------------------------------------------------------------
bool cScriptEngine::RunScript(const char *FileName, float InitTime)
{
	// установка значений
	StartTime = TimeLastOp = InitTime;

	TimeOpLag = 0;

	NeedCheckSpaceShip = false;
	NeedCheckGroundObject = false;
	EndDelayMissionComplete = 0.0f;
	LastTimeMissionComplete = -1.0f;

	AsterQuant = 2.0f;
	AsterW = 280.0f;
	AsterH = 7.5f;
	AsterXPos = 0.0f;
	AsterYPos = -10.0f;
	AsterZPos = 340.0f;
	AsterRealNeed = 0.0f;
	AsterMaxSpeed = 5.0f;
	AsterMinFastSpeed = 35.0f;
	AsterFastCount = 0;
	AsterOn = false;
	AsterLastTime = -1.0f;



	// отладочный режим
	ShowDebugModeLine = false;
	NeedShowBB = 0;
	UndeadDebugMode = false;
	ShowGameTime = false;


	xmlDoc.reset(new cXMLDocument(FileName));

	// проверяем корневой элемент
	if (!xmlDoc->GetRootEntry() || ("AstroMenaceScript" != xmlDoc->GetRootEntry()->Name)) {
		std::cerr << __func__ << "(): " << "Can't find AstroMenaceScript element in the: " << FileName << "\n";
		xmlDoc.reset();
		return false;
	}


	// переходим на действия
	sXMLEntry *xmlEntry = xmlDoc->FindEntryByName(*xmlDoc->GetRootEntry(), "Action");
	if (!xmlEntry) {
		std::cerr << __func__ << "(): " << "Can't find Action element in the: " << FileName << "\n";
		xmlDoc.reset();
		return false;
	}

	if (xmlEntry->ChildrenList.empty()) {
		std::cerr << __func__ << "(): " << "Can't find Action element's children in the: " << FileName << "\n";
		xmlDoc.reset();
		return false;
	}

	xmlEntryIter = xmlEntry->ChildrenList.begin();



	// идем и выполняем то, что по времени 0 стоит, т.е. начальную инициализацию
	Update(StartTime);
	return true;
}






//-----------------------------------------------------------------------------
// проверяем скрипт
//-----------------------------------------------------------------------------
bool cScriptEngine::Update(float Time)
{
	// скрипт не загружен
	if (xmlDoc == nullptr)
		return false;

	// находим дельту времени
	float TimeDelta = Time - TimeLastOp;


	// генерация астероидов
	if (AsterOn) {
		if (AsterLastTime == -1.0) AsterLastTime = Time;
		float AsterTimeDelta = Time - AsterLastTime;
		AsterLastTime = Time;

		// складываем все
		float NeedGener = AsterQuant*AsterTimeDelta+AsterRealNeed;
		// получаем целое кол-во на генерацию
		unsigned int NeedGenerInt = (unsigned int)NeedGener;
		// находим остаток... который нужно будет потом учесть
		AsterRealNeed = NeedGener - NeedGenerInt;

		while (NeedGenerInt>0) {
			cAsteroid *CreateAsteroid = new cAsteroid;
			CreateAsteroid->Create(1);
			if (AsterFastCount != 20)
				CreateAsteroid->Speed = AsterMaxSpeed * vw_fRand();
			else
				CreateAsteroid->Speed = AsterMinFastSpeed + AsterMaxSpeed * vw_fRand();
			CreateAsteroid->ShowDeleteOnHide = 0;
			CreateAsteroid->SetRotation(sVECTOR3D(0.0f, 180.0f, 0.0f));// !!!учесть камеру

			if (AsterFastCount != 20)
				CreateAsteroid->SetLocation(
					sVECTOR3D(AsterW * vw_Randf0 + AsterXPos, AsterYPos * 2 + AsterH * vw_fRand(), AsterZPos + 20.0f)
					+GamePoint);
			else
				CreateAsteroid->SetLocation(
					sVECTOR3D(AsterW * vw_Randf0 + AsterXPos, AsterYPos * 2 + AsterH * vw_fRand(), AsterZPos)
					+GamePoint);

			NeedGenerInt--;
		}

		AsterFastCount++;
		if (AsterFastCount > 30) AsterFastCount = 0;
	}



	// если нужно, смотрим когда заканчивать миссию
	if ((EndDelayMissionComplete > 0.0f) || NeedCheckSpaceShip || NeedCheckGroundObject) {
		if (LastTimeMissionComplete == -1.0) LastTimeMissionComplete = Time;
		float TimeDeltaMissionComplete = Time - LastTimeMissionComplete;
		LastTimeMissionComplete = Time;

		// уменьшаем только тогда, когда уже никого нет... т.е. отступ это от смерти последнего
		bool NeedDecrease = true;


		// собственно сами проверки...
		if (NeedCheckSpaceShip) {
			// если нет врагов
			int count = 0;
			cSpaceShip *Tmp1 = StartSpaceShip;
			while (Tmp1 != nullptr) {
				// если это враг, и мы его показали (иначе он где-то там летает)
				if ((Tmp1->ObjectStatus == 1) && (Tmp1->ShowDeleteOnHide != 0))
					count++;
				Tmp1 = Tmp1->Next;
			}
			if (count > 0) NeedDecrease = false;

		}
		if (NeedCheckGroundObject) {
			// если нет врагов, которых можем уничтожить
			int count = 0;
			cGroundObject *Tmp1 = StartGroundObject;
			while (Tmp1 != nullptr) {
				// если это враг, и мы его показали
				if (NeedCheckCollision(Tmp1) && (Tmp1->ShowDeleteOnHide != 0))
					count++;
				Tmp1 = Tmp1->Next;
			}
			if (count > 0)
				NeedDecrease = false;
		}


		if (NeedDecrease) EndDelayMissionComplete -= TimeDeltaMissionComplete;
		if (EndDelayMissionComplete<=0.0f) {
			EndDelayMissionComplete = 0.0f;
			NeedCheckSpaceShip = false;
			NeedCheckGroundObject = false;
			SetGameMissionComplete();
			return false;
		}

		// говорим, что скрипт еще не закончился!
		return true;
	}





	// we don't check FindEntryByName() result, since we checked it in RunScript()
	for (; xmlEntryIter != xmlDoc->FindEntryByName(*xmlDoc->GetRootEntry(), "Action")->ChildrenList.end(); ++xmlEntryIter) {
		sXMLEntry &xmlEntry = *xmlEntryIter;
		if (xmlEntry.Name == "TimeLine") {
			float onTime = 0.0f;
			xmlDoc->fGetEntryAttribute(xmlEntry, "value", onTime);

			// если еще не время выполнять... нужно уйти из процедуры
			if (onTime > TimeDelta) return true;

			// считаем лаг, чтобы правильно вычеслить положение при создании
			TimeOpLag = TimeDelta - onTime;

			// дальше смотрим, что нужно сделать...
			UpdateTimeLine();

			TimeLastOp = Time - TimeOpLag;
			TimeDelta = TimeOpLag;//Time - TimeLastOp;
		} else if (xmlEntry.Name == "Debug") {
			ShowDebugModeLine = false;
			xmlDoc->bGetEntryAttribute(xmlEntry, "showline", ShowDebugModeLine);

			NeedShowBB = 0;
			xmlDoc->iGetEntryAttribute(xmlEntry, "showbb", NeedShowBB);

			UndeadDebugMode = false;
			xmlDoc->bGetEntryAttribute(xmlEntry, "undead", UndeadDebugMode);

			ShowGameTime = false;
			xmlDoc->bGetEntryAttribute(xmlEntry, "time", ShowGameTime);

		} else if (xmlEntry.Name == "StarSystem") {
			int SystemNum{0};
			if (xmlDoc->iGetEntryAttribute(xmlEntry, "system", SystemNum)) {
				sVECTOR3D TmpBaseRotation(0.0f, 0.0f, 0.0f);
				xmlDoc->fGetEntryAttribute(xmlEntry, "anglex", TmpBaseRotation.x);
				xmlDoc->fGetEntryAttribute(xmlEntry, "angley", TmpBaseRotation.y);
				xmlDoc->fGetEntryAttribute(xmlEntry, "anglez", TmpBaseRotation.z);
				StarSystemInit(SystemNum, TmpBaseRotation);
			}
		} else if (xmlEntry.Name == "Music") {
			// если корабль игрока уничтожен - не меняем уже музыку в игре вообще,
			// должна проигрываться только музыка поражения
			if (PlayerFighter != nullptr)
				if (PlayerFighter->Strength > 0.0f) {
					int Theme{0};
					if (xmlDoc->iGetEntryAttribute(xmlEntry, "theme", Theme)) {
						if (Theme == 1)
							StartMusicWithFade(eMusicTheme::GAME, 2000, 2000);
						if (Theme == 2)
							StartMusicWithFade(eMusicTheme::BOSS, 1500, 2000);
					}
				}
		} else if (xmlEntry.Name == "CreatePlanet") {
			cPlanet *Planet = new cPlanet;
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(xmlEntry, "type", tmpType)) {
				Planet->Create(tmpType);
				SetRotation(Planet, &xmlEntry, xmlDoc);
				SetLocation(Planet, &xmlEntry, xmlDoc, 0.0f);
				Planet->ShowDeleteOnHide = 0;
				xmlDoc->fGetEntryAttribute(xmlEntry, "speed", Planet->Speed);
			}
		} else if (xmlEntry.Name == "AsteroidField") {
			xmlDoc->bGetEntryAttribute(xmlEntry, "status", AsterOn);
			xmlDoc->fGetEntryAttribute(xmlEntry, "persec", AsterQuant);
			xmlDoc->fGetEntryAttribute(xmlEntry, "w", AsterW);
			xmlDoc->fGetEntryAttribute(xmlEntry, "h", AsterH);
			xmlDoc->fGetEntryAttribute(xmlEntry, "posx", AsterXPos);
			xmlDoc->fGetEntryAttribute(xmlEntry, "posy", AsterYPos);
			xmlDoc->fGetEntryAttribute(xmlEntry, "posz", AsterZPos);
			xmlDoc->fGetEntryAttribute(xmlEntry, "slow", AsterMaxSpeed);
			xmlDoc->fGetEntryAttribute(xmlEntry, "fast", AsterMinFastSpeed);
		} else if (xmlEntry.Name == "Light") {
			eLightType LightType{eLightType::Directional}; // по умолчанию, солнце
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(xmlEntry, "type", tmpType) &&
			    (tmpType == 1))
				LightType = eLightType::Point;

			std::weak_ptr<cLight> NewLight = vw_CreateLight(LightType);
			if (auto sharedLight = NewLight.lock()) {
				sharedLight->Diffuse[0] = 0.0f;
				sharedLight->Diffuse[1] = 0.0f;
				sharedLight->Diffuse[2] = 0.0f;
				sharedLight->Diffuse[3] = 1.0f;
				xmlDoc->fGetEntryAttribute(xmlEntry, "diffr", sharedLight->Diffuse[0]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "diffg", sharedLight->Diffuse[1]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "diffb", sharedLight->Diffuse[2]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "diffa", sharedLight->Diffuse[3]);

				sharedLight->Specular[0] = 0.0f;
				sharedLight->Specular[1] = 0.0f;
				sharedLight->Specular[2] = 0.0f;
				sharedLight->Specular[3] = 1.0f;
				xmlDoc->fGetEntryAttribute(xmlEntry, "specr", sharedLight->Specular[0]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "specg", sharedLight->Specular[1]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "specb", sharedLight->Specular[2]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "speca", sharedLight->Specular[3]);

				sharedLight->Ambient[0] = 0.0f;
				sharedLight->Ambient[1] = 0.0f;
				sharedLight->Ambient[2] = 0.0f;
				sharedLight->Ambient[3] = 1.0f;
				xmlDoc->fGetEntryAttribute(xmlEntry, "ambir", sharedLight->Ambient[0]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "ambig", sharedLight->Ambient[1]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "ambib", sharedLight->Ambient[2]);
				xmlDoc->fGetEntryAttribute(xmlEntry, "ambia", sharedLight->Ambient[3]);

				sharedLight->Direction = sVECTOR3D(0.0f,0.0f,1.0f);
				xmlDoc->fGetEntryAttribute(xmlEntry, "dirx", sharedLight->Direction.x);
				xmlDoc->fGetEntryAttribute(xmlEntry, "diry", sharedLight->Direction.y);
				xmlDoc->fGetEntryAttribute(xmlEntry, "dirz", sharedLight->Direction.z);
				sharedLight->Direction.Normalize();

				sharedLight->Location = sVECTOR3D(0.0f,0.0f,0.0f);
				xmlDoc->fGetEntryAttribute(xmlEntry, "posx", sharedLight->Location.x);
				xmlDoc->fGetEntryAttribute(xmlEntry, "posy", sharedLight->Location.y);
				xmlDoc->fGetEntryAttribute(xmlEntry, "posz", sharedLight->Location.z);

				sharedLight->On = true;
				xmlDoc->bGetEntryAttribute(xmlEntry, "status", sharedLight->On);
			}
		} else if (xmlEntry.Name == "Label") {
			// ничего не делаем
		} else if (xmlEntry.Name == "Goto") {
			// если есть указатель на метку
			std::string tmpLabel{};
			if (xmlDoc->GetEntryAttribute(xmlEntry, "label", tmpLabel)) {
				// нужно перебрать все метки и остановится на нужной
				// we don't check FindEntryByName() result, since we checked it in RunScript()
				sXMLEntry *tmpCycle = xmlDoc->FindEntryByName(*xmlDoc->GetRootEntry(), "Action");
				// перебор по всем меткам
				for (auto tmpEntryIter = tmpCycle->ChildrenList.begin(); tmpEntryIter != tmpCycle->ChildrenList.end(); ++tmpEntryIter) {
					sXMLEntry &tmpEntry = *tmpEntryIter;
					if (tmpEntry.Name == "Label") {
						std::string tmpName{};
						if (xmlDoc->GetEntryAttribute(tmpEntry, "name", tmpName) &&
						    (tmpLabel == tmpName)) {
							// ставим новый указатель
							xmlEntryIter = tmpEntryIter;
							return true;
						}
					}
				}
			}
		} else if (xmlEntry.Name == "MissionComplete") {
			SetGameMissionComplete();
		} else if (xmlEntry.Name == "MissionCompleteAtNoEnemy") {
			bool SetGameMissionFlag = false;
			NeedCheckSpaceShip = false;
			if (xmlDoc->bGetEntryAttribute(xmlEntry, "ships", NeedCheckSpaceShip))
				SetGameMissionFlag = true;
			NeedCheckGroundObject = false;
			if (xmlDoc->bGetEntryAttribute(xmlEntry, "grounds", NeedCheckGroundObject))
				SetGameMissionFlag = true;
			EndDelayMissionComplete = 0.0f;
			if (xmlDoc->fGetEntryAttribute(xmlEntry, "delay", EndDelayMissionComplete))
				SetGameMissionFlag = true;

			if (!SetGameMissionFlag) {
				// если время не выставили и нечего ждать, работаем как и с обычным завершением
				SetGameMissionComplete();
			} else {
				LastTimeMissionComplete = Time;
				return true;
			}
		} else {
			// если тут - значит не нашли директиву, или произошла ошибка
			std::cerr << __func__ << "(): " << "ScriptEngine: tag " << xmlEntry.Name
			  << " not found, line " << xmlEntry.LineNumber << "\n";
		}
	}

	// выходим, скрипт закончился
	return false;
}









//-----------------------------------------------------------------------------
// проверяем скрипт дополнительно для TimeLine
//-----------------------------------------------------------------------------
void cScriptEngine::UpdateTimeLine()
{
	for (auto &TL : xmlEntryIter->ChildrenList) {

		if (TL.Name == "EarthFighter") {
			cEarthSpaceFighter *Fighter = nullptr;
			Fighter = new cEarthSpaceFighter;
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				Fighter->Create(tmpType);
			else
				continue;

			SetID(Fighter, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(Fighter, &TL);

			if (xmlDoc->fGetEntryAttribute(TL, "speed", Fighter->NeedSpeed))
				Fighter->Speed = Fighter->NeedSpeed;
			if (xmlDoc->fGetEntryAttribute(TL, "speedlr", Fighter->NeedSpeedLR))
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			if (xmlDoc->fGetEntryAttribute(TL, "speedud", Fighter->NeedSpeedUD))
				Fighter->SpeedUD = Fighter->NeedSpeedUD;

			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamfb", Fighter->NeedSpeedByCamFB))
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamlr", Fighter->NeedSpeedByCamLR))
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamud", Fighter->NeedSpeedByCamUD))
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;


			int tmp{0};
			if (xmlDoc->iGetEntryAttribute(TL, "armour", tmp))
				SetEarthSpaceFighterArmour(Fighter, tmp);
			if (xmlDoc->iGetEntryAttribute(TL, "weapon1", tmp))
				SetEarthSpaceFighterWeapon(Fighter, 1, tmp);
			if (xmlDoc->iGetEntryAttribute(TL, "weapon2", tmp))
				SetEarthSpaceFighterWeapon(Fighter, 2, tmp);
			if (xmlDoc->iGetEntryAttribute(TL, "weapon3", tmp))
				SetEarthSpaceFighterWeapon(Fighter, 3, tmp);
			if (xmlDoc->iGetEntryAttribute(TL, "weapon4", tmp))
				SetEarthSpaceFighterWeapon(Fighter, 4, tmp);
			if (xmlDoc->iGetEntryAttribute(TL, "weapon5", tmp))
				SetEarthSpaceFighterWeapon(Fighter, 5, tmp);
			if (xmlDoc->iGetEntryAttribute(TL, "weapon6", tmp))
				SetEarthSpaceFighterWeapon(Fighter, 6, tmp);

			SetShowDeleteOnHide(Fighter, &TL, xmlDoc);
			SetAIMode(Fighter, &TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, &TL, xmlDoc);
			SetShipLocation(Fighter, &TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			for (const auto &TLEarthFighter :  TL.ChildrenList) {
				if (TLEarthFighter.Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->iGetEntryAttribute(TLEarthFighter, "aimode", TimeSheet->AI_Mode)) {
						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "time", TimeSheet->Time);

						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						TimeSheet->Acceler = 1.0f;//0-1
						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						TimeSheet->Fire = false;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					} else {
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "speed", TimeSheet->Speed);

						TimeSheet->Acceler = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "acceler", TimeSheet->Acceler);
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedlr", TimeSheet->SpeedLR);

						TimeSheet->AccelerLR = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerlr", TimeSheet->AccelerLR);
						vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedud", TimeSheet->SpeedUD);

						TimeSheet->AccelerUD = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerud", TimeSheet->AccelerUD);
						vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);


						TimeSheet->SpeedByCamFB = 0.0f;
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamfb", TimeSheet->SpeedByCamFB);

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamfb", TimeSheet->AccelerByCamFB);
						vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamlr", TimeSheet->SpeedByCamLR);

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamlr", TimeSheet->AccelerByCamLR);
						vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamud", TimeSheet->SpeedByCamUD);

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamud", TimeSheet->AccelerByCamUD);
						vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);


						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotx", TimeSheet->Rotation.x);
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "roty", TimeSheet->Rotation.y);
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotz", TimeSheet->Rotation.z);

						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacx", TimeSheet->RotationAcceler.x);
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacy", TimeSheet->RotationAcceler.y);
						xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacz", TimeSheet->RotationAcceler.z);
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						int tmpFire{0};
						if (xmlDoc->iGetEntryAttribute(TLEarthFighter, "fire", tmpFire) &&
						    (tmpFire > 0))
							TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					}
				}
			}
		} else if (TL.Name == "AlienFighter") {
			cAlienSpaceFighter *Fighter = new cAlienSpaceFighter;
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				Fighter->Create(tmpType);
			else
				continue;

			SetID(Fighter, &TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, &TL);

			if (xmlDoc->fGetEntryAttribute(TL, "speed", Fighter->NeedSpeed))
				Fighter->Speed = Fighter->NeedSpeed;
			if (xmlDoc->fGetEntryAttribute(TL, "speedlr", Fighter->NeedSpeedLR))
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			if (xmlDoc->fGetEntryAttribute(TL, "speedud", Fighter->NeedSpeedUD))
				Fighter->SpeedUD = Fighter->NeedSpeedUD;

			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamfb", Fighter->NeedSpeedByCamFB))
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamlr", Fighter->NeedSpeedByCamLR))
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamud", Fighter->NeedSpeedByCamUD))
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;

			SetShowDeleteOnHide(Fighter, &TL, xmlDoc);
			SetAIMode(Fighter, &TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, &TL, xmlDoc);
			SetShipLocation(Fighter, &TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			for (const auto &TLAlienFighter : TL.ChildrenList) {
				if (TLAlienFighter.Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->iGetEntryAttribute(TLAlienFighter, "aimode", TimeSheet->AI_Mode)) {
						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						TimeSheet->Acceler = 1.0f;//0-1
						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						TimeSheet->Fire = false;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					} else {
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "speed", TimeSheet->Speed);

						TimeSheet->Acceler = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "acceler", TimeSheet->Acceler);
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedlr", TimeSheet->SpeedLR);

						TimeSheet->AccelerLR = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerlr", TimeSheet->AccelerLR);
						vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedud", TimeSheet->SpeedUD);

						TimeSheet->AccelerUD = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerud", TimeSheet->AccelerUD);
						vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamfb", TimeSheet->SpeedByCamFB);

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamfb", TimeSheet->AccelerByCamFB);
						vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamlr", TimeSheet->SpeedByCamLR);

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamlr", TimeSheet->AccelerByCamLR);
						vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamud", TimeSheet->SpeedByCamUD);

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamud", TimeSheet->AccelerByCamUD);
						vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotx", TimeSheet->Rotation.x);
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "roty", TimeSheet->Rotation.y);
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotz", TimeSheet->Rotation.z);

						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacx", TimeSheet->RotationAcceler.x);
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacy", TimeSheet->RotationAcceler.y);
						xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacz", TimeSheet->RotationAcceler.z);
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						int tmpFire{0};
						if (xmlDoc->iGetEntryAttribute(TLAlienFighter, "fire", tmpFire) &&
						    (tmpFire > 0))
							TimeSheet->Fire = true;
						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
					}
				}
			}
		} else if (TL.Name == "AlienMotherShip") {
			cAlienSpaceMotherShip *Fighter = new cAlienSpaceMotherShip;
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				Fighter->Create(tmpType);
			else
				continue;

			SetID(Fighter, &TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, &TL);

			if (xmlDoc->fGetEntryAttribute(TL, "speed", Fighter->NeedSpeed))
				Fighter->Speed = Fighter->NeedSpeed;
			if (xmlDoc->fGetEntryAttribute(TL, "speedlr", Fighter->NeedSpeedLR))
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			if (xmlDoc->fGetEntryAttribute(TL, "speedud", Fighter->NeedSpeedUD))
				Fighter->SpeedUD = Fighter->NeedSpeedUD;

			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamfb", Fighter->NeedSpeedByCamFB))
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamlr", Fighter->NeedSpeedByCamLR))
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamud", Fighter->NeedSpeedByCamUD))
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;

			SetShowDeleteOnHide(Fighter, &TL, xmlDoc);
			SetAIMode(Fighter, &TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, &TL, xmlDoc);
			SetShipLocation(Fighter, &TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			for (const auto &TLAlienMotherShip : TL.ChildrenList) {
				if (TLAlienMotherShip.Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "aimode", TimeSheet->AI_Mode)) {
						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						TimeSheet->Acceler = 1.0f;//0-1
						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						TimeSheet->Fire = false;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					} else {
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speed", TimeSheet->Speed);

						TimeSheet->Acceler = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "acceler", TimeSheet->Acceler);
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedlr", TimeSheet->SpeedLR);

						TimeSheet->AccelerLR = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerlr", TimeSheet->AccelerLR);
						vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedud", TimeSheet->SpeedUD);

						TimeSheet->AccelerUD = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerud", TimeSheet->AccelerUD);
						vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamfb", TimeSheet->SpeedByCamFB);

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamfb", TimeSheet->AccelerByCamFB);
						vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamlr", TimeSheet->SpeedByCamLR);

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamlr", TimeSheet->AccelerByCamLR);
						vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamud", TimeSheet->SpeedByCamUD);

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamud", TimeSheet->AccelerByCamUD);
						vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotx", TimeSheet->Rotation.x);
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "roty", TimeSheet->Rotation.y);
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotz", TimeSheet->Rotation.z);

						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacx", TimeSheet->RotationAcceler.x);
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacy", TimeSheet->RotationAcceler.y);
						xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacz", TimeSheet->RotationAcceler.z);
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						int tmpFire{0};
						if (xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "fire", tmpFire) &&
						    (tmpFire > 0))
							TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						int tmpBossFire{0};
						if (xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "bossfire", tmpBossFire) &&
						    (tmpBossFire > 0))
							TimeSheet->BossFire = true;

						TimeSheet->Targeting = false;
					}
				}
			}
		} else if (TL.Name == "PirateShip") {
			cPirateShip *Fighter = new cPirateShip;
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				Fighter->Create(tmpType);
			else
				continue;

			SetID(Fighter, &TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, &TL);

			if (xmlDoc->fGetEntryAttribute(TL, "speed", Fighter->NeedSpeed))
				Fighter->Speed = Fighter->NeedSpeed;
			if (xmlDoc->fGetEntryAttribute(TL, "speedlr", Fighter->NeedSpeedLR))
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			if (xmlDoc->fGetEntryAttribute(TL, "speedud", Fighter->NeedSpeedUD))
				Fighter->SpeedUD = Fighter->NeedSpeedUD;

			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamfb", Fighter->NeedSpeedByCamFB))
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamlr", Fighter->NeedSpeedByCamLR))
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			if (xmlDoc->fGetEntryAttribute(TL, "speedbycamud", Fighter->NeedSpeedByCamUD))
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;

			SetShowDeleteOnHide(Fighter, &TL, xmlDoc);
			SetAIMode(Fighter, &TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, &TL, xmlDoc);
			SetShipLocation(Fighter, &TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			for (const auto &TLPirateShip: TL.ChildrenList) {
				if (TLPirateShip.Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->iGetEntryAttribute(TLPirateShip, "aimode", TimeSheet->AI_Mode)) {
						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLPirateShip, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						TimeSheet->Acceler = 1.0f;//0-1
						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						TimeSheet->Fire = false;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					} else {
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLPirateShip, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						xmlDoc->fGetEntryAttribute(TLPirateShip, "speed", TimeSheet->Speed);

						TimeSheet->Acceler = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLPirateShip, "acceler", TimeSheet->Acceler);
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						xmlDoc->fGetEntryAttribute(TLPirateShip, "speedlr", TimeSheet->SpeedLR);

						TimeSheet->AccelerLR = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerlr", TimeSheet->AccelerLR);
						vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						xmlDoc->fGetEntryAttribute(TLPirateShip, "speedud", TimeSheet->SpeedUD);

						TimeSheet->AccelerUD = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerud", TimeSheet->AccelerUD);
						vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamfb", TimeSheet->SpeedByCamFB);

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamfb", TimeSheet->AccelerByCamFB);
						vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamlr", TimeSheet->SpeedByCamLR);

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamlr", TimeSheet->AccelerByCamLR);
						vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamud", TimeSheet->SpeedByCamUD);

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamud", TimeSheet->AccelerByCamUD);
						vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						xmlDoc->fGetEntryAttribute(TLPirateShip, "rotx", TimeSheet->Rotation.x);
						xmlDoc->fGetEntryAttribute(TLPirateShip, "roty", TimeSheet->Rotation.y);
						xmlDoc->fGetEntryAttribute(TLPirateShip, "rotz", TimeSheet->Rotation.z);

						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacx", TimeSheet->RotationAcceler.x);
						xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacy", TimeSheet->RotationAcceler.y);
						xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacz", TimeSheet->RotationAcceler.z);
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						int tmpFire{0};
						if (xmlDoc->iGetEntryAttribute(TLPirateShip, "fire", tmpFire) &&
						    (tmpFire > 0))
							TimeSheet->Fire = true;

						TimeSheet->BossFire = false;
						int tmpBossFire{0};
						if (xmlDoc->iGetEntryAttribute(TLPirateShip, "bossfire", tmpBossFire) &&
						    (tmpBossFire > 0))
							TimeSheet->BossFire = true;

						TimeSheet->Targeting = false;
					}
				}
			}
		} else if (TL.Name == "CreateAsteroid") {
			cAsteroid *Asteroid = new cAsteroid;

			// тип сейчас не задействован, всегда ставим 1
			Asteroid->Create(1);

			SetID(Asteroid, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(Asteroid, &TL);
			xmlDoc->fGetEntryAttribute(TL, "speed", Asteroid->Speed);
			SetShowDeleteOnHide(Asteroid, &TL, xmlDoc);

			SetRotation(Asteroid, &TL, xmlDoc);
			SetLocation(Asteroid, &TL, xmlDoc, TimeOpLag);

			xmlDoc->fGetEntryAttribute(TL, "rotx", Asteroid->RotationSpeed.x);
			xmlDoc->fGetEntryAttribute(TL, "roty", Asteroid->RotationSpeed.y);
			xmlDoc->fGetEntryAttribute(TL, "rotz", Asteroid->RotationSpeed.z);
		} else if (TL.Name == "CreateBasePart") {
			cBasePart *BasePart = new cBasePart;

			// тип части
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				BasePart->Create(tmpType);
			else
				continue;

			xmlDoc->fGetEntryAttribute(TL, "speed", BasePart->Speed);

			SetID(BasePart, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(BasePart, &TL);
			SetShowDeleteOnHide(BasePart, &TL, xmlDoc);

			SetRotation(BasePart, &TL, xmlDoc);
			SetLocation(BasePart, &TL, xmlDoc, TimeOpLag);
		} else if (TL.Name == "CreateBigAsteroid") {
			cBigAsteroid *BigAsteroid = new cBigAsteroid;

			// тип части
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				BigAsteroid->Create(tmpType);
			else
				continue;

			xmlDoc->fGetEntryAttribute(TL, "speed", BigAsteroid->Speed);

			SetID(BigAsteroid, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(BigAsteroid, &TL);
			SetShowDeleteOnHide(BigAsteroid, &TL, xmlDoc);

			SetRotation(BigAsteroid, &TL, xmlDoc);
			SetLocation(BigAsteroid, &TL, xmlDoc, TimeOpLag);
		} else if (TL.Name == "CreateMBuilding") {
			cMilitaryBuilding *GroundObject = new cMilitaryBuilding;

			// тип части
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				GroundObject->Create(tmpType);
			else
				continue;

			SetID(GroundObject, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(GroundObject, &TL);
			SetShowDeleteOnHide(GroundObject, &TL, xmlDoc);
			SetAIMode(GroundObject, &TL, xmlDoc); // на тот случае если просто ставим и все...

			SetRotation(GroundObject, &TL, xmlDoc);
			SetLocation(GroundObject, &TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			for (const auto &TLGroundObject : TL.ChildrenList) {
				if (TLGroundObject.Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode", TimeSheet->AI_Mode)) {
						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLGroundObject, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						TimeSheet->Acceler = 1.0f;//0-1
						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						TimeSheet->Fire = false;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					} else {
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLGroundObject, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						TimeSheet->Acceler = 1.0f;//0-1
						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1

						TimeSheet->Fire = false;
						int tmpFire{0};
						if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire", tmpFire) &&
						    (tmpFire > 0))
							TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						int tmpTargeting{0};
						if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting", tmpTargeting) &&
						    (tmpTargeting != 0))
							TimeSheet->Targeting = true;
					}
				}
			}
		} else if (TL.Name == "CreateBuilding") {
			cBuilding *GroundObject = new cBuilding;
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				GroundObject->Create(tmpType);
			else
				continue;

			SetID(GroundObject, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(GroundObject, &TL);
			SetShowDeleteOnHide(GroundObject, &TL, xmlDoc);

			SetRotation(GroundObject, &TL, xmlDoc);
			SetLocation(GroundObject, &TL, xmlDoc, TimeOpLag);
		} else if (TL.Name == "CreateMine") {
			cProjectile *Mine = new cProjectile;
			// т.к. мины у нас с 214-217, делаем +213
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType)) {
				int MineType = tmpType + 213;
				vw_Clamp(MineType, 214, 217);
				Mine->Create(MineType);
			} else
				continue;

			Mine->ProjectileType = 1;

			// по умолчанию враг
			Mine->ObjectStatus = 1;
			xmlDoc->iGetEntryAttribute(TL, "status", Mine->ObjectStatus);

			// общий - пенальти, если не игрок
			float CurrentPenalty = GameNPCWeaponPenalty * 1.0f;
			// если игрок или свои - ничего не надо...
			if (Mine->ObjectStatus >= 2)
				CurrentPenalty = 1.0f;

			Mine->DamageHull = Mine->DamageHull / CurrentPenalty;
			Mine->DamageSystems = Mine->DamageSystems / CurrentPenalty;
			Mine->SpeedStart = Mine->SpeedEnd = Mine->Speed = Mine->SpeedStart / CurrentPenalty;

			SetID(Mine, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(Mine, &TL);
			SetShowDeleteOnHide(Mine, &TL, xmlDoc);


			SetProjectileRotation(Mine, &TL, xmlDoc);
			SetProjectileLocation(Mine, &TL, xmlDoc, TimeOpLag);
		} else if (TL.Name == "CreateTracked") {
			cTracked *GroundObject = new cTracked;
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				GroundObject->Create(tmpType);
			else
				continue;

			SetID(GroundObject, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(GroundObject, &TL);
			if (xmlDoc->fGetEntryAttribute(TL, "speed", GroundObject->NeedSpeed))
				GroundObject->Speed = GroundObject->NeedSpeed;

			SetShowDeleteOnHide(GroundObject, &TL, xmlDoc);
			SetAIMode(GroundObject, &TL, xmlDoc); // на тот случае если просто ставим и все...
			SetRotation(GroundObject, &TL, xmlDoc);
			SetLocation(GroundObject, &TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			for (const auto &TLGroundObject : TL.ChildrenList) {
				if (TLGroundObject.Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode", TimeSheet->AI_Mode)) {
						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLGroundObject, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						TimeSheet->Acceler = 1.0f;//0-1
						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						TimeSheet->Fire = false;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					} else {
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLGroundObject, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						xmlDoc->fGetEntryAttribute(TLGroundObject, "speed", TimeSheet->Speed);

						TimeSheet->Acceler = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLGroundObject, "acceler", TimeSheet->Acceler);
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1

						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotx", TimeSheet->Rotation.x);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "roty", TimeSheet->Rotation.y);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotz", TimeSheet->Rotation.z);

						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacx", TimeSheet->RotationAcceler.x);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacy", TimeSheet->RotationAcceler.y);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacz", TimeSheet->RotationAcceler.z);
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						int tmpFire{0};
						if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire", tmpFire) &&
						    (tmpFire > 0))
							TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						int tmpTargeting{0};
						if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting", tmpTargeting) &&
						    (tmpTargeting > 0))
							TimeSheet->Targeting = true;
					}
				}
			}
		} else if (TL.Name == "CreateWheeled") {
			cWheeled *GroundObject = new cWheeled;
			int tmpType{0};
			if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
				GroundObject->Create(tmpType);
			else
				continue;

			SetID(GroundObject, &TL, xmlDoc);
			if (ShowDebugModeLine)
				SetDebugInformation(GroundObject, &TL);
			if (xmlDoc->fGetEntryAttribute(TL, "speed", GroundObject->NeedSpeed))
				GroundObject->Speed = GroundObject->NeedSpeed;

			SetShowDeleteOnHide(GroundObject, &TL, xmlDoc);
			SetAIMode(GroundObject, &TL, xmlDoc); // на тот случае если просто ставим и все...
			SetRotation(GroundObject, &TL, xmlDoc);
			SetLocation(GroundObject, &TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			for (const auto &TLGroundObject : TL.ChildrenList) {
				if (TLGroundObject.Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode", TimeSheet->AI_Mode)) {
						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLGroundObject, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						TimeSheet->Acceler = 1.0f;//0-1
						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						TimeSheet->Fire = false;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					} else {
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						xmlDoc->fGetEntryAttribute(TLGroundObject, "time", TimeSheet->Time);
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						xmlDoc->fGetEntryAttribute(TLGroundObject, "speed", TimeSheet->Speed);


						TimeSheet->Acceler = 1.0f;//0-1
						xmlDoc->fGetEntryAttribute(TLGroundObject, "acceler", TimeSheet->Acceler);
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						TimeSheet->AccelerLR = 1.0f;//0-1
						TimeSheet->SpeedUD = 0.0f;
						TimeSheet->AccelerUD = 1.0f;//0-1
						TimeSheet->SpeedByCamFB = 0.0f;
						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						TimeSheet->SpeedByCamLR = 0.0f;
						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						TimeSheet->SpeedByCamUD = 0.0f;
						TimeSheet->AccelerByCamUD = 1.0f;//0-1

						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotx", TimeSheet->Rotation.x);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "roty", TimeSheet->Rotation.y);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotz", TimeSheet->Rotation.z);

						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacx", TimeSheet->RotationAcceler.x);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacy", TimeSheet->RotationAcceler.y);
						xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacz", TimeSheet->RotationAcceler.z);
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						int tmpFire{0};
						if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire", tmpFire) &&
						    (tmpFire > 0))
							TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						int tmpTargeting{0};
						if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting", tmpTargeting) &&
						    (tmpTargeting != 0))
							TimeSheet->Targeting = true;
					}
				}
			}
		} else {
			// если тут - значит не нашли директиву, или произошла ошибка
			std::cerr << __func__ << "(): " << "ScriptEngine: tag " << TL.Name
				  << " not found, line " << TL.LineNumber << "\n";
		}
	}
}




