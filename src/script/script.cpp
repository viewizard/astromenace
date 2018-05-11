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

// FIXME continue in the middle of the cycle

// TODO translate comments

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#include "../core/core.h"
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

// FIXME should be fixed, don't allow global scope interaction for local variables
extern sVECTOR3D GamePoint;
extern sVECTOR3D GameCameraMovement;
extern cEarthSpaceFighter *PlayerFighter;
// FIXME should be fixed, use 'include' instead
float GameCameraGetSpeed();
void SetGameMissionComplete();
void StartMusicWithFade(eMusicTheme StartMusic, uint32_t FadeInTicks, uint32_t FadeOutTicks);


//-----------------------------------------------------------------------------
// aimode
//-----------------------------------------------------------------------------
static void SetAIMode(std::list<sTimeSheet> &TimeSheetList, const sXMLEntry &xmlEntry,
		      const std::unique_ptr<cXMLDocument> &xmlDoc, const std::shared_ptr<cXMLDocument> &xmlAI)
{
	int tmpAI_Mode{0};
	if (xmlDoc->iGetEntryAttribute(xmlEntry, "aimode", tmpAI_Mode)) {
		TimeSheetList.emplace_back();
		TimeSheetList.back().Time = -1;
		TimeSheetList.back().AI_Mode = tmpAI_Mode;
		TimeSheetList.back().xmlAI = xmlAI;
	}
}

//-----------------------------------------------------------------------------
//  ID
//-----------------------------------------------------------------------------
static void SetID(cObject3D &Object, const sXMLEntry &xmlEntry,
		  const std::unique_ptr<cXMLDocument> &xmlDoc)
{
	xmlDoc->iGetEntryAttribute(xmlEntry, "id", Object.ID);
}

//-----------------------------------------------------------------------------
// Location
//-----------------------------------------------------------------------------
static void SetShipLocation(cSpaceShip &Object, const sXMLEntry &xmlEntry,
			    const std::unique_ptr<cXMLDocument> &xmlDoc, float TimeOpLag)
{
	sVECTOR3D tmpPosition(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	xmlDoc->fGetEntryAttribute(xmlEntry, "posax", tmpPosition.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "posay", tmpPosition.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "posaz", tmpPosition.z);

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement ^ (-GameCameraGetSpeed() * TimeOpLag);
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posx", tmpPosition.x))
		tmpPosition.x += GamePoint.x + PosWithLag.x;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posy", tmpPosition.y))
		tmpPosition.y += GamePoint.y + PosWithLag.y;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posz", tmpPosition.z))
		tmpPosition.z += GamePoint.z + PosWithLag.z;

	Object.SetLocation(tmpPosition);
}

static void SetProjectileLocation(cProjectile &Object, const sXMLEntry &xmlEntry,
				  const std::unique_ptr<cXMLDocument> &xmlDoc, float TimeOpLag)
{
	sVECTOR3D tmpPosition(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	xmlDoc->fGetEntryAttribute(xmlEntry, "posax", tmpPosition.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "posay", tmpPosition.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "posaz", tmpPosition.z);

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posx", tmpPosition.x))
		tmpPosition.x += GamePoint.x + PosWithLag.x;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posy", tmpPosition.y))
		tmpPosition.y += GamePoint.y + PosWithLag.y;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posz", tmpPosition.z))
		tmpPosition.z += GamePoint.z + PosWithLag.z;

	Object.SetLocation(tmpPosition);
}

static void SetLocation(cObject3D &Object, const sXMLEntry &xmlEntry,
			const std::unique_ptr<cXMLDocument> &xmlDoc, float TimeOpLag)
{
	sVECTOR3D tmpPosition(0.0f, 0.0f, 0.0f);

	// абсолютные координаты
	xmlDoc->fGetEntryAttribute(xmlEntry, "posax", tmpPosition.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "posay", tmpPosition.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "posaz", tmpPosition.z);

	// относительные координаты
	sVECTOR3D PosWithLag(0.0f,0.0f,0.0f);
	// находим на сколько перелетим
	PosWithLag = GameCameraMovement^(-GameCameraGetSpeed()*TimeOpLag);
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posx", tmpPosition.x))
		tmpPosition.x += GamePoint.x + PosWithLag.x;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posy", tmpPosition.y))
		tmpPosition.y += GamePoint.y + PosWithLag.y;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "posz", tmpPosition.z))
		tmpPosition.z += GamePoint.z + PosWithLag.z;

	Object.SetLocation(tmpPosition);
}

//-----------------------------------------------------------------------------
// Rotation
//-----------------------------------------------------------------------------
static void SetShipRotation(cSpaceShip &Object, const sXMLEntry &xmlEntry,
			    const std::unique_ptr<cXMLDocument> &xmlDoc)
{
	sVECTOR3D tmpAngle(0.0f, 0.0f, 0.0f);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglax", tmpAngle.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglay", tmpAngle.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz", tmpAngle.z);

// пока делает тоже самое!!! потом переделать
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglx", tmpAngle.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "angly", tmpAngle.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglz", tmpAngle.z);

	Object.SetRotation(tmpAngle);
}

static void SetProjectileRotation(cProjectile &Object, const sXMLEntry &xmlEntry,
				  const std::unique_ptr<cXMLDocument> &xmlDoc)
{
	sVECTOR3D tmpAngle(0.0f, 0.0f, 0.0f);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglax", tmpAngle.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglay", tmpAngle.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz", tmpAngle.z);

// пока делает тоже самое!!! потом переделать
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglx", tmpAngle.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "angly", tmpAngle.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglz", tmpAngle.z);

	Object.SetRotation(tmpAngle);
}

static void SetRotation(cObject3D &Object, const sXMLEntry &xmlEntry,
			const std::unique_ptr<cXMLDocument> &xmlDoc)
{
	sVECTOR3D tmpAngle(0.0f, 0.0f, 0.0f);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglax", tmpAngle.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglay", tmpAngle.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglaz" ,tmpAngle.z);

// пока делает тоже самое!!! потом переделать
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglx", tmpAngle.x);
	xmlDoc->fGetEntryAttribute(xmlEntry, "angly", tmpAngle.y);
	xmlDoc->fGetEntryAttribute(xmlEntry, "anglz", tmpAngle.z);

	Object.SetRotation(tmpAngle);
}

//-----------------------------------------------------------------------------
// DeleteOnHide
//-----------------------------------------------------------------------------
static void SetShowDeleteOnHide(cObject3D &Object, const sXMLEntry &xmlEntry,
				const std::unique_ptr<cXMLDocument> &xmlDoc)
{
	Object.ShowDeleteOnHide = 0;
	if (xmlDoc->iGetEntryAttribute(xmlEntry, "onhide", Object.ShowDeleteOnHide) &&
	    (Object.ShowDeleteOnHide <= 0))
			Object.ShowDeleteOnHide = -1;
}

//-----------------------------------------------------------------------------
// DebugInformation
//-----------------------------------------------------------------------------
#ifdef NDEBUG
static void SetDebugInformation(cObject3D &UNUSED(Object), const sXMLEntry &UNUSED(xmlEntry),
				bool UNUSED(ShowLineNumber))
{
	return;
}
#else
static void SetDebugInformation(cObject3D &Object, const sXMLEntry &xmlEntry, bool ShowLineNumber)
{
	// не нужно ничего устанавливать, выходим
	if (!ShowLineNumber)
		return;

	std::string buffer{std::to_string(xmlEntry.LineNumber)};

	Object.DebugInfo = new char[buffer.size()+1];
	strcpy(Object.DebugInfo, buffer.c_str());
}
#endif // NDEBUG

cMissionScript::cMissionScript()
{
	// отладочный режим
	NeedShowBB = 0;
	UndeadDebugMode = false;
}

//-----------------------------------------------------------------------------
// запустить скрипт на выполнение
//-----------------------------------------------------------------------------
bool cMissionScript::RunScript(const char *FileName, float InitTime)
{
	if (!FileName)
		return false;

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
	ShowLineNumber = false;
	NeedShowBB = 0;
	UndeadDebugMode = false;
	ShowGameTime = false;

	xmlDoc.reset(new cXMLDocument(FileName, true));

	// проверяем корневой элемент
	if (!xmlDoc->GetRootEntry() || ("AstroMenaceScript" != xmlDoc->GetRootEntry()->Name)) {
		std::cerr << __func__ << "(): " << "Can't find AstroMenaceScript element in the: " << FileName << "\n";
		xmlDoc.reset();
		return false;
	}

	// переходим на действия
	if (xmlDoc->GetRootEntry()->ChildrenList.empty()) {
		std::cerr << __func__ << "(): " << "Can't find element's children in the: " << FileName << "\n";
		xmlDoc.reset();
		return false;
	}

	xmlEntryIter = xmlDoc->GetRootEntry()->ChildrenList.begin();

	// идем и выполняем то, что по времени 0 стоит, т.е. начальную инициализацию
	Update(StartTime);
	return true;
}

//-----------------------------------------------------------------------------
// проверяем скрипт
//-----------------------------------------------------------------------------
bool cMissionScript::Update(float Time)
{
	// скрипт не загружен
	if (!xmlDoc)
		return false;

	// находим дельту времени
	float TimeDelta = Time - TimeLastOp;

	// генерация астероидов
	if (AsterOn) {
		if (AsterLastTime == -1.0)
			AsterLastTime = Time;
		float AsterTimeDelta = Time - AsterLastTime;
		AsterLastTime = Time;

		// складываем все
		float NeedGener = AsterQuant * AsterTimeDelta + AsterRealNeed;
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
		if (LastTimeMissionComplete == -1.0)
			LastTimeMissionComplete = Time;
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
	for (; xmlEntryIter != xmlDoc->GetRootEntry()->ChildrenList.end(); ++xmlEntryIter) {
		sXMLEntry &xmlEntry = *xmlEntryIter;
		switch (xmlEntry.NameHash) {
		case xml::hash("TimeLine"): {
				float onTime = 0.0f;
				xmlDoc->fGetEntryAttribute(xmlEntry, "value", onTime);

				// если еще не время выполнять... нужно уйти из процедуры
				if (onTime > TimeDelta)
					return true;

				// считаем лаг, чтобы правильно вычеслить положение при создании
				TimeOpLag = TimeDelta - onTime;

				// дальше смотрим, что нужно сделать...
				UpdateTimeLine();

				TimeLastOp = Time - TimeOpLag;
				TimeDelta = TimeOpLag;
			}
			break;

		case xml::hash("AIFile"):
			if (!xmlEntry.Content.empty()) {
				xmlAI.reset(new cXMLDocument(xmlEntry.Content));
				if (!xmlAI->GetRootEntry()) {
					xmlAI.reset();
					std::cerr << __func__ << "(): " << "AIFile not loaded.\n";
				}
			}
			break;

		case xml::hash("Debug"):
			ShowLineNumber = false;
			xmlDoc->bGetEntryAttribute(xmlEntry, "showline", ShowLineNumber);

			NeedShowBB = 0;
			xmlDoc->iGetEntryAttribute(xmlEntry, "showbb", NeedShowBB);

			UndeadDebugMode = false;
			xmlDoc->bGetEntryAttribute(xmlEntry, "undead", UndeadDebugMode);

			ShowGameTime = false;
			xmlDoc->bGetEntryAttribute(xmlEntry, "time", ShowGameTime);
			break;

		case xml::hash("StarSystem"): {
				int SystemNum{0};
				if (xmlDoc->iGetEntryAttribute(xmlEntry, "system", SystemNum)) {
					sVECTOR3D TmpBaseRotation(0.0f, 0.0f, 0.0f);
					xmlDoc->fGetEntryAttribute(xmlEntry, "anglex", TmpBaseRotation.x);
					xmlDoc->fGetEntryAttribute(xmlEntry, "angley", TmpBaseRotation.y);
					xmlDoc->fGetEntryAttribute(xmlEntry, "anglez", TmpBaseRotation.z);
					StarSystemInit(SystemNum, TmpBaseRotation);
				}
			}
			break;

		case xml::hash("LayersTransp"): {
				float Layer1TranspStart{0.2f};
				float Layer1TranspEnd{0.7f};
				float Layer2TranspStart{0.9f};
				float Layer2TranspEnd{0.7f};

				xmlDoc->fGetEntryAttribute(xmlEntry, "FirstStart", Layer1TranspStart);
				xmlDoc->fGetEntryAttribute(xmlEntry, "FirstEnd", Layer1TranspEnd);
				xmlDoc->fGetEntryAttribute(xmlEntry, "SecondStart", Layer2TranspStart);
				xmlDoc->fGetEntryAttribute(xmlEntry, "SecondEnd", Layer2TranspEnd);

				StarSystemLayer1Transp(Layer1TranspStart, Layer1TranspEnd);
				StarSystemLayer3Transp(Layer2TranspStart, Layer2TranspEnd);
			}
			break;

		case xml::hash("Music"):
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
			break;

		case xml::hash("CreatePlanet"): {
				cPlanet *Planet = new cPlanet;
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(xmlEntry, "type", tmpType)) {
					Planet->Create(tmpType);
					SetRotation(*Planet, xmlEntry, xmlDoc);
					SetLocation(*Planet, xmlEntry, xmlDoc, 0.0f);
					Planet->ShowDeleteOnHide = 0;
					xmlDoc->fGetEntryAttribute(xmlEntry, "speed", Planet->Speed);
				}
			}
			break;

		case xml::hash("AsteroidField"):
			xmlDoc->bGetEntryAttribute(xmlEntry, "status", AsterOn);
			xmlDoc->fGetEntryAttribute(xmlEntry, "persec", AsterQuant);
			xmlDoc->fGetEntryAttribute(xmlEntry, "w", AsterW);
			xmlDoc->fGetEntryAttribute(xmlEntry, "h", AsterH);
			xmlDoc->fGetEntryAttribute(xmlEntry, "posx", AsterXPos);
			xmlDoc->fGetEntryAttribute(xmlEntry, "posy", AsterYPos);
			xmlDoc->fGetEntryAttribute(xmlEntry, "posz", AsterZPos);
			xmlDoc->fGetEntryAttribute(xmlEntry, "slow", AsterMaxSpeed);
			xmlDoc->fGetEntryAttribute(xmlEntry, "fast", AsterMinFastSpeed);
			break;

		case xml::hash("Light"): {
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
			}
			break;

		case xml::hash("Label"):
			// ничего не делаем
			break;

		case xml::hash("Goto"): {
				// если есть указатель на метку
				std::string tmpLabel{};
				if (xmlDoc->GetEntryAttribute(xmlEntry, "label", tmpLabel)) {
					// нужно перебрать все метки и остановится на нужной
					// we don't check FindEntryByName() result, since we checked it in RunScript()
					sXMLEntry *tmpCycle = xmlDoc->GetRootEntry();
					// перебор по всем меткам
					for (auto tmpEntryIter = tmpCycle->ChildrenList.begin();
					     tmpEntryIter != tmpCycle->ChildrenList.end();
					     ++tmpEntryIter) {
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
			}
			break;

		case xml::hash("MissionComplete"):
			SetGameMissionComplete();
			break;

		case xml::hash("MissionCompleteAtNoEnemy"): {
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
			}
			break;

		default:
			// если тут - значит не нашли директиву, или произошла ошибка
			std::cerr << __func__ << "(): " << "ScriptEngine: tag " << xmlEntry.Name
			  << " not found, line " << xmlEntry.LineNumber << "\n";
			break;
		}
	}

	// выходим, скрипт закончился
	return false;
}

/*
 * Load TimeSheet data from xml entry.
 */
static void LoadTimeSheetData(cXMLDocument &xmlDoc, const sXMLEntry &XMLEntry,
			      sTimeSheet &TimeSheet, const std::shared_ptr<cXMLDocument> &xmlAI)
{
	if (xmlDoc.iGetEntryAttribute(XMLEntry, "aimode", TimeSheet.AI_Mode)) {
		xmlDoc.fGetEntryAttribute(XMLEntry, "time", TimeSheet.Time);
		TimeSheet.xmlAI = xmlAI;
		return;
	}

	xmlDoc.fGetEntryAttribute(XMLEntry, "time", TimeSheet.Time);

	xmlDoc.fGetEntryAttribute(XMLEntry, "speed", TimeSheet.Speed);

	xmlDoc.fGetEntryAttribute(XMLEntry, "acceler", TimeSheet.Acceler);
	vw_Clamp(TimeSheet.Acceler, 0.0f, 1.0f);

	xmlDoc.fGetEntryAttribute(XMLEntry, "speedlr", TimeSheet.SpeedLR);

	xmlDoc.fGetEntryAttribute(XMLEntry, "accelerlr", TimeSheet.AccelerLR);
	vw_Clamp(TimeSheet.AccelerLR, 0.0f, 1.0f);

	xmlDoc.fGetEntryAttribute(XMLEntry, "speedud", TimeSheet.SpeedUD);

	xmlDoc.fGetEntryAttribute(XMLEntry, "accelerud", TimeSheet.AccelerUD);
	vw_Clamp(TimeSheet.AccelerUD, 0.0f, 1.0f);

	xmlDoc.fGetEntryAttribute(XMLEntry, "speedbycamfb", TimeSheet.SpeedByCamFB);

	xmlDoc.fGetEntryAttribute(XMLEntry, "accelerbycamfb", TimeSheet.AccelerByCamFB);
	vw_Clamp(TimeSheet.AccelerByCamFB, 0.0f, 1.0f);

	xmlDoc.fGetEntryAttribute(XMLEntry, "speedbycamlr", TimeSheet.SpeedByCamLR);

	xmlDoc.fGetEntryAttribute(XMLEntry, "accelerbycamlr", TimeSheet.AccelerByCamLR);
	vw_Clamp(TimeSheet.AccelerByCamLR, 0.0f, 1.0f);

	xmlDoc.fGetEntryAttribute(XMLEntry, "speedbycamud", TimeSheet.SpeedByCamUD);

	xmlDoc.fGetEntryAttribute(XMLEntry, "accelerbycamud", TimeSheet.AccelerByCamUD);
	vw_Clamp(TimeSheet.AccelerByCamUD, 0.0f, 1.0f);

	xmlDoc.fGetEntryAttribute(XMLEntry, "rotx", TimeSheet.Rotation.x);
	xmlDoc.fGetEntryAttribute(XMLEntry, "roty", TimeSheet.Rotation.y);
	xmlDoc.fGetEntryAttribute(XMLEntry, "rotz", TimeSheet.Rotation.z);

	xmlDoc.fGetEntryAttribute(XMLEntry, "rotacx", TimeSheet.RotationAcceler.x);
	xmlDoc.fGetEntryAttribute(XMLEntry, "rotacy", TimeSheet.RotationAcceler.y);
	xmlDoc.fGetEntryAttribute(XMLEntry, "rotacz", TimeSheet.RotationAcceler.z);
	vw_Clamp(TimeSheet.RotationAcceler.x, 0.0f, 1.0f);
	vw_Clamp(TimeSheet.RotationAcceler.y, 0.0f, 1.0f);
	vw_Clamp(TimeSheet.RotationAcceler.z, 0.0f, 1.0f);

	int tmpFire{0};
	if (xmlDoc.iGetEntryAttribute(XMLEntry, "fire", tmpFire) &&
	    (tmpFire > 0))
		TimeSheet.Fire = true;

	int tmpBossFire{0};
	if (xmlDoc.iGetEntryAttribute(XMLEntry, "bossfire", tmpBossFire) &&
	    (tmpBossFire > 0))
		TimeSheet.BossFire = true;

	int tmpTargeting{0};
	if (xmlDoc.iGetEntryAttribute(XMLEntry, "targeting", tmpTargeting) &&
	    (tmpTargeting != 0))
		TimeSheet.Targeting = true;
}

static void LoadSpaceShipScript(cSpaceShip &SpaceShip, const std::unique_ptr<cXMLDocument> &xmlDoc,
				const sXMLEntry &xmlEntry, bool ShowLineNumber, float TimeOpLag,
				const std::shared_ptr<cXMLDocument> &xmlAI)
{
	SetID(SpaceShip, xmlEntry, xmlDoc);
	if (ShowLineNumber)
		SetDebugInformation(SpaceShip, xmlEntry, ShowLineNumber);

	if (xmlDoc->fGetEntryAttribute(xmlEntry, "speed", SpaceShip.NeedSpeed))
		SpaceShip.Speed = SpaceShip.NeedSpeed;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "speedlr", SpaceShip.NeedSpeedLR))
		SpaceShip.SpeedLR = SpaceShip.NeedSpeedLR;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "speedud", SpaceShip.NeedSpeedUD))
		SpaceShip.SpeedUD = SpaceShip.NeedSpeedUD;

	if (xmlDoc->fGetEntryAttribute(xmlEntry, "speedbycamfb", SpaceShip.NeedSpeedByCamFB))
		SpaceShip.SpeedByCamFB = SpaceShip.NeedSpeedByCamFB;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "speedbycamlr", SpaceShip.NeedSpeedByCamLR))
		SpaceShip.SpeedByCamLR = SpaceShip.NeedSpeedByCamLR;
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "speedbycamud", SpaceShip.NeedSpeedByCamUD))
		SpaceShip.SpeedByCamUD = SpaceShip.NeedSpeedByCamUD;

	SetShowDeleteOnHide(SpaceShip, xmlEntry, xmlDoc);
	SetAIMode(SpaceShip.TimeSheetList, xmlEntry, xmlDoc, xmlAI);
	SetShipRotation(SpaceShip, xmlEntry, xmlDoc);
	SetShipLocation(SpaceShip, xmlEntry, xmlDoc, TimeOpLag);

	// дальше смотрим, что нужно сделать...
	for (const auto &tmpXMLEntry : xmlEntry.ChildrenList) {
		if (tmpXMLEntry.Name == "TimeSheet") {
			SpaceShip.TimeSheetList.emplace_back();
			LoadTimeSheetData(*xmlDoc.get(), tmpXMLEntry,
					  SpaceShip.TimeSheetList.back(), xmlAI);
		}
	}
}

static void LoadGroundObjectScript(cGroundObject &GroundObject, const std::unique_ptr<cXMLDocument> &xmlDoc,
				const sXMLEntry &xmlEntry, bool ShowLineNumber, float TimeOpLag,
				const std::shared_ptr<cXMLDocument> &xmlAI)
{
	SetID(GroundObject, xmlEntry, xmlDoc);
	if (ShowLineNumber)
		SetDebugInformation(GroundObject, xmlEntry, ShowLineNumber);
	if (xmlDoc->fGetEntryAttribute(xmlEntry, "speed", GroundObject.NeedSpeed))
		GroundObject.Speed = GroundObject.NeedSpeed;

	SetShowDeleteOnHide(GroundObject, xmlEntry, xmlDoc);
	SetAIMode(GroundObject.TimeSheetList, xmlEntry, xmlDoc, xmlAI);
	SetRotation(GroundObject, xmlEntry, xmlDoc);
	SetLocation(GroundObject, xmlEntry, xmlDoc, TimeOpLag);

	// дальше смотрим, что нужно сделать...
	for (const auto &tmpXMLEntry : xmlEntry.ChildrenList) {
		if (tmpXMLEntry.Name == "TimeSheet") {
			GroundObject.TimeSheetList.emplace_back();
			LoadTimeSheetData(*xmlDoc.get(), tmpXMLEntry,
					  GroundObject.TimeSheetList.back(), xmlAI);
		}
	}
}

//-----------------------------------------------------------------------------
// проверяем скрипт дополнительно для TimeLine
//-----------------------------------------------------------------------------
void cMissionScript::UpdateTimeLine()
{
	for (auto &TL : xmlEntryIter->ChildrenList) {

		switch (TL.NameHash) {
		case xml::hash("EarthFighter"): {
				cEarthSpaceFighter *SpaceShip = nullptr;
				SpaceShip = new cEarthSpaceFighter;
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					SpaceShip->Create(tmpType);
				else
					continue;

				LoadSpaceShipScript(*SpaceShip, xmlDoc, TL, ShowLineNumber, TimeOpLag, xmlAI);

				int tmp{0};
				if (xmlDoc->iGetEntryAttribute(TL, "armour", tmp))
					SetEarthSpaceFighterArmour(SpaceShip, tmp);
				if (xmlDoc->iGetEntryAttribute(TL, "weapon1", tmp))
					SetEarthSpaceFighterWeapon(SpaceShip, 1, tmp);
				if (xmlDoc->iGetEntryAttribute(TL, "weapon2", tmp))
					SetEarthSpaceFighterWeapon(SpaceShip, 2, tmp);
				if (xmlDoc->iGetEntryAttribute(TL, "weapon3", tmp))
					SetEarthSpaceFighterWeapon(SpaceShip, 3, tmp);
				if (xmlDoc->iGetEntryAttribute(TL, "weapon4", tmp))
					SetEarthSpaceFighterWeapon(SpaceShip, 4, tmp);
				if (xmlDoc->iGetEntryAttribute(TL, "weapon5", tmp))
					SetEarthSpaceFighterWeapon(SpaceShip, 5, tmp);
				if (xmlDoc->iGetEntryAttribute(TL, "weapon6", tmp))
					SetEarthSpaceFighterWeapon(SpaceShip, 6, tmp);
			}
			break;

		case xml::hash("AlienFighter"): {
				cAlienSpaceFighter *SpaceShip = new cAlienSpaceFighter;
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					SpaceShip->Create(tmpType);
				else
					continue;

				LoadSpaceShipScript(*SpaceShip, xmlDoc, TL, ShowLineNumber, TimeOpLag, xmlAI);
			}
			break;

		case xml::hash("AlienMotherShip"): {
				cAlienSpaceMotherShip *SpaceShip = new cAlienSpaceMotherShip;
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					SpaceShip->Create(tmpType);
				else
					continue;

				LoadSpaceShipScript(*SpaceShip, xmlDoc, TL, ShowLineNumber, TimeOpLag, xmlAI);
			}
			break;

		case xml::hash("PirateShip"): {
				cPirateShip *SpaceShip = new cPirateShip;
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					SpaceShip->Create(tmpType);
				else
					continue;

				LoadSpaceShipScript(*SpaceShip, xmlDoc, TL, ShowLineNumber, TimeOpLag, xmlAI);
			}
			break;

		case xml::hash("CreateAsteroid"): {
				cAsteroid *Asteroid = new cAsteroid;

				// тип сейчас не задействован, всегда ставим 1
				Asteroid->Create(1);

				SetID(*Asteroid, TL, xmlDoc);
				if (ShowLineNumber)
					SetDebugInformation(*Asteroid, TL, ShowLineNumber);
				xmlDoc->fGetEntryAttribute(TL, "speed", Asteroid->Speed);
				SetShowDeleteOnHide(*Asteroid, TL, xmlDoc);

				SetRotation(*Asteroid, TL, xmlDoc);
				SetLocation(*Asteroid, TL, xmlDoc, TimeOpLag);

				xmlDoc->fGetEntryAttribute(TL, "rotx", Asteroid->RotationSpeed.x);
				xmlDoc->fGetEntryAttribute(TL, "roty", Asteroid->RotationSpeed.y);
				xmlDoc->fGetEntryAttribute(TL, "rotz", Asteroid->RotationSpeed.z);
			}
			break;

		case xml::hash("CreateBasePart"): {
				cBasePart *BasePart = new cBasePart;

				// тип части
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					BasePart->Create(tmpType);
				else
					continue;

				xmlDoc->fGetEntryAttribute(TL, "speed", BasePart->Speed);

				SetID(*BasePart, TL, xmlDoc);
				if (ShowLineNumber)
					SetDebugInformation(*BasePart, TL, ShowLineNumber);
				SetShowDeleteOnHide(*BasePart, TL, xmlDoc);

				SetRotation(*BasePart, TL, xmlDoc);
				SetLocation(*BasePart, TL, xmlDoc, TimeOpLag);
			}
			break;

		case xml::hash("CreateBigAsteroid"): {
				cBigAsteroid *BigAsteroid = new cBigAsteroid;

				// тип части
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					BigAsteroid->Create(tmpType);
				else
					continue;

				xmlDoc->fGetEntryAttribute(TL, "speed", BigAsteroid->Speed);

				SetID(*BigAsteroid, TL, xmlDoc);
				if (ShowLineNumber)
					SetDebugInformation(*BigAsteroid, TL, ShowLineNumber);
				SetShowDeleteOnHide(*BigAsteroid, TL, xmlDoc);

				SetRotation(*BigAsteroid, TL, xmlDoc);
				SetLocation(*BigAsteroid, TL, xmlDoc, TimeOpLag);
			}
			break;

		case xml::hash("CreateMine"): {
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

				SetID(*Mine, TL, xmlDoc);
				if (ShowLineNumber)
					SetDebugInformation(*Mine, TL, ShowLineNumber);
				SetShowDeleteOnHide(*Mine, TL, xmlDoc);

				SetProjectileRotation(*Mine, TL, xmlDoc);
				SetProjectileLocation(*Mine, TL, xmlDoc, TimeOpLag);
			}
			break;

		case xml::hash("CreateMBuilding"): {
				cMilitaryBuilding *GroundObject = new cMilitaryBuilding;

				// тип части
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					GroundObject->Create(tmpType);
				else
					continue;

				LoadGroundObjectScript(*GroundObject, xmlDoc, TL, ShowLineNumber, TimeOpLag, xmlAI);
			}
			break;

		case xml::hash("CreateBuilding"): {
				cBuilding *GroundObject = new cBuilding;
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					GroundObject->Create(tmpType);
				else
					continue;

				LoadGroundObjectScript(*GroundObject, xmlDoc, TL, ShowLineNumber, TimeOpLag, xmlAI);
			}
			break;

		case xml::hash("CreateTracked"): {
				cTracked *GroundObject = new cTracked;
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					GroundObject->Create(tmpType);
				else
					continue;

				LoadGroundObjectScript(*GroundObject, xmlDoc, TL, ShowLineNumber, TimeOpLag, xmlAI);
			}
			break;

		case xml::hash("CreateWheeled"): {
				cWheeled *GroundObject = new cWheeled;
				int tmpType{0};
				if (xmlDoc->iGetEntryAttribute(TL, "type", tmpType))
					GroundObject->Create(tmpType);
				else
					continue;

				LoadGroundObjectScript(*GroundObject, xmlDoc, TL, ShowLineNumber, TimeOpLag, xmlAI);
			}
			break;

		default:
			// если тут - значит не нашли директиву, или произошла ошибка
			std::cerr << __func__ << "(): " << "ScriptEngine: tag " << TL.Name
				  << " not found, line " << TL.LineNumber << "\n";
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Замена маркера действия набором действий
//-----------------------------------------------------------------------------
void InterAIMode(std::list<sTimeSheet> &TimeSheetList)
{
	if (TimeSheetList.empty())
		return;

	auto iter = TimeSheetList.begin();

	for (auto &xmlEntry : TimeSheetList.front().xmlAI->GetRootEntry()->ChildrenList) {
		int tmpAI_Mode{0};
		if (TimeSheetList.front().xmlAI->iGetEntryAttribute(xmlEntry, "num", tmpAI_Mode) &&
		    (tmpAI_Mode == TimeSheetList.front().AI_Mode)) {

			// "unpack" all the elements
			for (auto &tmpChildEntry : xmlEntry.ChildrenList) {
				if (tmpChildEntry.Name == "TimeSheet") {
					iter = TimeSheetList.emplace(++iter);
					LoadTimeSheetData(*TimeSheetList.front().xmlAI.get(), tmpChildEntry,
							  *iter, TimeSheetList.front().xmlAI);
				}
			}

			// for cycled, create duplicate at the end of unpacked elements
			if (TimeSheetList.front().Time == -1)
				TimeSheetList.emplace(++iter, TimeSheetList.front());

			return;
		}
	}
}
