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
#include "script.h"
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
#include "../gfx/game_level_text/game_level_text.h"


void SetID(cObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetDebugInformation(cObject3D *Object, cXMLEntry *xmlEntry);
void SetShowDeleteOnHide(cObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetShipRotation(cSpaceShip *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetShipLocation(cSpaceShip *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag);
void SetProjectileRotation(cProjectile *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetProjectileLocation(cProjectile *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag);
void SetRotation(cObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetLocation(cObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag);
void SetAIMode(cObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);




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

cScriptEngine::~cScriptEngine()
{
	delete xmlDoc;
};



//-----------------------------------------------------------------------------
// запустить скрипт на выполнение
//-----------------------------------------------------------------------------
bool cScriptEngine::RunScript(const char *FileName, float InitTime)
{
	// установка значений
	StartTime = TimeLastOp = InitTime;

	TimeOpLag = 0;
	xmlEntry = nullptr;

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


	if (xmlDoc != nullptr) {
		delete xmlDoc;
		xmlDoc = nullptr;
	}
	xmlDoc = new cXMLDocument(FileName);

	// проверяем корневой элемент
	if (!xmlDoc->RootXMLEntry || ("AstroMenaceScript" != xmlDoc->RootXMLEntry->Name)) {
		std::cerr << "Can't find AstroMenaceScript element in the: " << FileName << "\n";
		delete xmlDoc;
		xmlDoc = nullptr;
		return false;
	}


	// переходим на действия
	xmlEntry  = xmlDoc->FindFirstChildEntryByName(xmlDoc->RootXMLEntry, "Action");
	if (xmlEntry == nullptr) {
		std::cerr << "Can't find Action element in the: " << FileName << "\n";
		delete xmlDoc;
		xmlDoc = nullptr;
		return false;
	}

	xmlEntry = xmlEntry->FirstChild;
	if (xmlEntry == nullptr) {
		std::cerr << "Can't find Action element in the: " << FileName << "\n";
		delete xmlDoc;
		xmlDoc = nullptr;
		return false;
	}





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
				CreateAsteroid->Speed = AsterMaxSpeed*vw_Randf1;
			else
				CreateAsteroid->Speed = AsterMinFastSpeed + AsterMaxSpeed*vw_Randf1;
			CreateAsteroid->ShowDeleteOnHide = 0;
			CreateAsteroid->SetRotation(sVECTOR3D(0.0f, 180.0f, 0.0f));// !!!учесть камеру

			if (AsterFastCount != 20)
				CreateAsteroid->SetLocation(
					sVECTOR3D(AsterW * vw_Randf0 + AsterXPos, AsterYPos * 2 + AsterH * vw_Randf1, AsterZPos + 20.0f)
					+GamePoint);
			else
				CreateAsteroid->SetLocation(
					sVECTOR3D(AsterW * vw_Randf0 + AsterXPos, AsterYPos * 2 + AsterH * vw_Randf1, AsterZPos)
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






	while (xmlEntry) {
		// TimeLine
		if (xmlEntry->Name == "TimeLine") {
			float onTime = 0.0f;
			if (xmlDoc->TestEntryAttribute(xmlEntry, "value"))
				onTime = xmlDoc->fGetEntryAttribute(xmlEntry, "value");

			// если еще не время выполнять... нужно уйти из процедуры
			if (onTime > TimeDelta) return true;

			// считаем лаг, чтобы правильно вычеслить положение при создании
			TimeOpLag = TimeDelta - onTime;

			// дальше смотрим, что нужно сделать...
			UpdateTimeLine();

			TimeLastOp = Time - TimeOpLag;
			TimeDelta = TimeOpLag;//Time - TimeLastOp;
		} else
			// Debug
			if (xmlEntry->Name == "Debug") {
				ShowDebugModeLine = false;
				if (xmlDoc->TestEntryAttribute(xmlEntry, "showline"))
					ShowDebugModeLine = xmlDoc->bGetEntryAttribute(xmlEntry, "showline");

				NeedShowBB = 0;
				if (xmlDoc->TestEntryAttribute(xmlEntry, "showbb"))
					NeedShowBB = xmlDoc->iGetEntryAttribute(xmlEntry, "showbb");

				UndeadDebugMode = false;
				if (xmlDoc->TestEntryAttribute(xmlEntry, "undead"))
					UndeadDebugMode = xmlDoc->bGetEntryAttribute(xmlEntry, "undead");

				ShowGameTime = false;
				if (xmlDoc->TestEntryAttribute(xmlEntry, "time"))
					ShowGameTime = xmlDoc->bGetEntryAttribute(xmlEntry, "time");

			} else
				// StarSystem
				if (xmlEntry->Name == "StarSystem") {
					if (xmlDoc->TestEntryAttribute(xmlEntry, "system")) {
						int SystemNum = xmlDoc->iGetEntryAttribute(xmlEntry, "system");
						sVECTOR3D TmpBaseRotation(0.0f, 0.0f, 0.0f);
						if (xmlDoc->TestEntryAttribute(xmlEntry, "anglex"))
							TmpBaseRotation.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglex");
						if (xmlDoc->TestEntryAttribute(xmlEntry, "angley"))
							TmpBaseRotation.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angley");
						if (xmlDoc->TestEntryAttribute(xmlEntry, "anglez"))
							TmpBaseRotation.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglez");
						StarSystemInit(SystemNum, TmpBaseRotation);
					}
				} else
					// Music
					if (xmlEntry->Name == "Music") {
						// если корабль игрока уничтожен - не меняем уже музыку в игре вообще,
						// должна проигрываться только музыка поражения
						if (PlayerFighter != nullptr)
							if (PlayerFighter->Strength > 0.0f) {
								if (xmlDoc->TestEntryAttribute(xmlEntry, "theme")) {
									int Theme = xmlDoc->iGetEntryAttribute(xmlEntry, "theme");
									if (Theme == 1)
										StartMusicWithFade(2, 2.0f, 2.0f);
									if (Theme == 2)
										StartMusicWithFade(3, 1.5f, 2.0f);
								}
							}
					} else
						// CreatePlanet
						if (xmlEntry->Name == "CreatePlanet") {
							cPlanet *Planet = new cPlanet;
							if (xmlDoc->TestEntryAttribute(xmlEntry, "type")) {
								Planet->Create(xmlDoc->iGetEntryAttribute(xmlEntry, "type"));
								SetRotation(Planet, xmlEntry, xmlDoc);
								SetLocation(Planet, xmlEntry, xmlDoc, 0.0f);
								Planet->ShowDeleteOnHide = 0;

								if (xmlDoc->TestEntryAttribute(xmlEntry, "speed"))
									Planet->Speed = xmlDoc->fGetEntryAttribute(xmlEntry, "speed");
							}
						} else
							// AsteroidField
							if (xmlEntry->Name == "AsteroidField") {
								if (xmlDoc->TestEntryAttribute(xmlEntry, "status")) {
									AsterOn = xmlDoc->bGetEntryAttribute(xmlEntry, "status");
								}
								if (xmlDoc->TestEntryAttribute(xmlEntry, "persec"))
									AsterQuant = xmlDoc->fGetEntryAttribute(xmlEntry, "persec");
								if (xmlDoc->TestEntryAttribute(xmlEntry, "w"))
									AsterW = xmlDoc->fGetEntryAttribute(xmlEntry, "w");
								if (xmlDoc->TestEntryAttribute(xmlEntry, "h"))
									AsterH = xmlDoc->fGetEntryAttribute(xmlEntry, "h");
								if (xmlDoc->TestEntryAttribute(xmlEntry, "posx"))
									AsterXPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posx");
								if (xmlDoc->TestEntryAttribute(xmlEntry, "posy"))
									AsterYPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posy");
								if (xmlDoc->TestEntryAttribute(xmlEntry, "posz"))
									AsterZPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posz");
								if (xmlDoc->TestEntryAttribute(xmlEntry, "slow"))
									AsterMaxSpeed = xmlDoc->fGetEntryAttribute(xmlEntry, "slow");
								if (xmlDoc->TestEntryAttribute(xmlEntry, "fast"))
									AsterMinFastSpeed = xmlDoc->fGetEntryAttribute(xmlEntry, "fast");
							} else
								// Light
								if (xmlEntry->Name == "Light") {
									eLightType LightType = eLightType::Directional; // по умолчанию, солнце
									if (xmlDoc->TestEntryAttribute(xmlEntry, "type"))
										if (xmlDoc->iGetEntryAttribute(xmlEntry, "type") == 1)
											LightType = eLightType::Point;

									sLight *NewLight = vw_CreateLight(LightType);

									NewLight->Diffuse[0] = 0.0f;
									NewLight->Diffuse[1] = 0.0f;
									NewLight->Diffuse[2] = 0.0f;
									NewLight->Diffuse[3] = 1.0f;
									if (xmlDoc->TestEntryAttribute(xmlEntry, "diffr"))
										NewLight->Diffuse[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffr");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "diffg"))
										NewLight->Diffuse[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffg");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "diffb"))
										NewLight->Diffuse[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffb");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "diffa"))
										NewLight->Diffuse[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffa");

									NewLight->Specular[0] = 0.0f;
									NewLight->Specular[1] = 0.0f;
									NewLight->Specular[2] = 0.0f;
									NewLight->Specular[3] = 1.0f;
									if (xmlDoc->TestEntryAttribute(xmlEntry, "specr"))
										NewLight->Specular[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "specr");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "specg"))
										NewLight->Specular[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "specg");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "specb"))
										NewLight->Specular[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "specb");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "speca"))
										NewLight->Specular[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "speca");

									NewLight->Ambient[0] = 0.0f;
									NewLight->Ambient[1] = 0.0f;
									NewLight->Ambient[2] = 0.0f;
									NewLight->Ambient[3] = 1.0f;
									if (xmlDoc->TestEntryAttribute(xmlEntry, "ambir"))
										NewLight->Ambient[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambir");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "ambig"))
										NewLight->Ambient[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambig");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "ambib"))
										NewLight->Ambient[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambib");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "ambia"))
										NewLight->Ambient[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambia");

									NewLight->Direction = sVECTOR3D(0.0f,0.0f,1.0f);
									if (xmlDoc->TestEntryAttribute(xmlEntry, "dirx"))
										NewLight->Direction.x = xmlDoc->fGetEntryAttribute(xmlEntry, "dirx");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "diry"))
										NewLight->Direction.y = xmlDoc->fGetEntryAttribute(xmlEntry, "diry");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "dirz"))
										NewLight->Direction.z = xmlDoc->fGetEntryAttribute(xmlEntry, "dirz");
									NewLight->Direction.Normalize();

									NewLight->Location = sVECTOR3D(0.0f,0.0f,0.0f);
									if (xmlDoc->TestEntryAttribute(xmlEntry, "posx"))
										NewLight->Location.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "posy"))
										NewLight->Location.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy");
									if (xmlDoc->TestEntryAttribute(xmlEntry, "posz"))
										NewLight->Location.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz");


									NewLight->On = true;
									if (xmlDoc->TestEntryAttribute(xmlEntry, "status"))
										NewLight->On = xmlDoc->bGetEntryAttribute(xmlEntry, "status");
								} else
									// Label
									if (xmlEntry->Name == "Label") {
										// ничего не делаем
										xmlEntry = xmlEntry->Next;
										continue;
									} else
										// Goto
										if (xmlEntry->Name == "Goto") {
											// если есть указатель на метку
											if (xmlDoc->TestEntryAttribute(xmlEntry, "label")) {
												// нужно перебрать все метки и остановится на нужной
												cXMLEntry *tmpEntry = xmlDoc->FindFirstChildEntryByName(xmlDoc->FindFirstChildEntryByName(xmlDoc->RootXMLEntry, "Action"), "Label");

												// перебор по всем меткам
												while (tmpEntry) {
													//	if (!strcmp(tmpEntry->Name, "Label"))
													if (xmlDoc->TestEntryAttribute(tmpEntry, "name"))
														if (xmlDoc->GetEntryAttribute(tmpEntry, "name") == xmlDoc->GetEntryAttribute(xmlEntry, "label")) {
															// ставим новый указатель
															xmlEntry = tmpEntry;
															return true;
														}
													// берем следующую метку
													tmpEntry = tmpEntry->Next;
												}
											}
										} else
											// MissionComplete
											if (xmlEntry->Name == "MissionComplete") {
												SetGameMissionComplete();
											} else
												// MissionCompleteAtNoEnemy
												if (xmlEntry->Name == "MissionCompleteAtNoEnemy") {
													bool SetGameMissionFlag = false;
													NeedCheckSpaceShip = false;
													if (xmlDoc->TestEntryAttribute(xmlEntry, "ships")) {
														NeedCheckSpaceShip = xmlDoc->bGetEntryAttribute(xmlEntry, "ships");
														SetGameMissionFlag = true;
													};
													NeedCheckGroundObject = false;
													if (xmlDoc->TestEntryAttribute(xmlEntry, "grounds")) {
														NeedCheckGroundObject = xmlDoc->bGetEntryAttribute(xmlEntry, "grounds");
														SetGameMissionFlag = true;
													};
													EndDelayMissionComplete = 0.0f;
													if (xmlDoc->TestEntryAttribute(xmlEntry, "delay")) {
														EndDelayMissionComplete = xmlDoc->fGetEntryAttribute(xmlEntry, "delay");
														SetGameMissionFlag = true;
													};

													if (!SetGameMissionFlag) {
														// если время не выставили и нечего ждать, работаем как и с обычным завершением
														SetGameMissionComplete();
													} else {
														LastTimeMissionComplete = Time;
														return true;
													}
												} else
													// Text
													if (xmlEntry->Name == "Text") {
														cGameLvlText *NewText;
														NewText = new cGameLvlText;


														NewText->Lifetime = -1.0f;
														if (xmlDoc->TestEntryAttribute(xmlEntry, "life"))
															NewText->Lifetime = xmlDoc->fGetEntryAttribute(xmlEntry, "life");

														if (xmlDoc->TestEntryAttribute(xmlEntry, "text")) {
															NewText->DrawText = new char[xmlDoc->GetEntryAttribute(xmlEntry, "text").size() + 1];
															strcpy(NewText->DrawText, xmlDoc->GetEntryAttribute(xmlEntry, "text").c_str());
														}

														if (xmlDoc->TestEntryAttribute(xmlEntry, "posx"))
															NewText->PosX = xmlDoc->iGetEntryAttribute(xmlEntry, "posx");
														if (xmlDoc->TestEntryAttribute(xmlEntry, "posy"))
															NewText->PosY = xmlDoc->iGetEntryAttribute(xmlEntry, "posy");

														NewText->Color = 0;
														if (xmlDoc->TestEntryAttribute(xmlEntry, "color"))
															NewText->Color = xmlDoc->iGetEntryAttribute(xmlEntry, "color");
													} else {
														// если тут - значит не нашли директиву, или произошла ошибка
														std::cerr << "ScriptEngine: tag " << xmlEntry->Name
															  << " not found, line " << xmlEntry->LineNumber << "\n";
													}

		// берем следующий элемент по порядку
		xmlEntry = xmlEntry->Next;
	}

	// выходим, скрипт закончился
	return false;
}









//-----------------------------------------------------------------------------
// проверяем скрипт дополнительно для TimeLine
//-----------------------------------------------------------------------------
void cScriptEngine::UpdateTimeLine()
{
	// получаем первый тэг
	cXMLEntry *TL = xmlEntry->FirstChild;

	while (TL) {

		// EarthFighter
		if (TL->Name == "EarthFighter") {
			cEarthSpaceFighter *Fighter = nullptr;
			Fighter = new cEarthSpaceFighter;
			if (xmlDoc->TestEntryAttribute(TL, "type"))
				Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else {
				TL = TL->Next;
				continue;
			}

			SetID(Fighter, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (xmlDoc->TestEntryAttribute(TL, "speed")) {
				Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (xmlDoc->TestEntryAttribute(TL, "speedlr")) {
				Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (xmlDoc->TestEntryAttribute(TL, "speedud")) {
				Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (xmlDoc->TestEntryAttribute(TL, "speedbycamfb")) {
				Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (xmlDoc->TestEntryAttribute(TL, "speedbycamlr")) {
				Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (xmlDoc->TestEntryAttribute(TL, "speedbycamud")) {
				Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}


			if (xmlDoc->TestEntryAttribute(TL, "armour"))
				SetEarthSpaceFighterArmour(Fighter, xmlDoc->iGetEntryAttribute(TL, "armour"));
			if (xmlDoc->TestEntryAttribute(TL, "weapon1"))
				SetEarthSpaceFighterWeapon(Fighter, 1, xmlDoc->iGetEntryAttribute(TL, "weapon1"));
			if (xmlDoc->TestEntryAttribute(TL, "weapon2"))
				SetEarthSpaceFighterWeapon(Fighter, 2, xmlDoc->iGetEntryAttribute(TL, "weapon2"));
			if (xmlDoc->TestEntryAttribute(TL, "weapon3"))
				SetEarthSpaceFighterWeapon(Fighter, 3, xmlDoc->iGetEntryAttribute(TL, "weapon3"));
			if (xmlDoc->TestEntryAttribute(TL, "weapon4"))
				SetEarthSpaceFighterWeapon(Fighter, 4, xmlDoc->iGetEntryAttribute(TL, "weapon4"));
			if (xmlDoc->TestEntryAttribute(TL, "weapon5"))
				SetEarthSpaceFighterWeapon(Fighter, 5, xmlDoc->iGetEntryAttribute(TL, "weapon5"));
			if (xmlDoc->TestEntryAttribute(TL, "weapon6"))
				SetEarthSpaceFighterWeapon(Fighter, 6, xmlDoc->iGetEntryAttribute(TL, "weapon6"));

			SetShowDeleteOnHide(Fighter, TL, xmlDoc);
			SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL, xmlDoc);
			SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			cXMLEntry *TLEarthFighter = TL->FirstChild;
			while (TLEarthFighter) {
				if (TLEarthFighter->Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->TestEntryAttribute(TLEarthFighter, "aimode")) {
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLEarthFighter, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "time"))
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLEarthFighter, "time");

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
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "time"))
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLEarthFighter, "time");
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "speed"))
							TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speed");

						TimeSheet->Acceler = 1.0f;//0-1
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "acceler"))
							TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLEarthFighter, "acceler");
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "speedlr"))
							TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedlr");

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "accelerlr"))
							TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerlr");
						vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "speedud"))
							TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedud");

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "accelerud"))
							TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerud");
						vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);


						TimeSheet->SpeedByCamFB = 0.0f;
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "speedbycamfb"))
							TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamfb");

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "accelerbycamfb"))
							TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamfb");
						vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "speedbycamlr"))
							TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamlr");

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "accelerbycamlr"))
							TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamlr");
						vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "speedbycamud"))
							TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamud");

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "accelerbycamud"))
							TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamud");
						vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);


						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "rotx"))
							TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotx");
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "roty"))
							TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLEarthFighter, "roty");
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "rotz"))
							TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotz");

						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "rotacx"))
							TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacx");
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "rotacy"))
							TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacy");
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "rotacz"))
							TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacz");
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (xmlDoc->TestEntryAttribute(TLEarthFighter, "fire"))
							if (xmlDoc->iGetEntryAttribute(TLEarthFighter, "fire") > 0)
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLEarthFighter = TLEarthFighter->Next;
			}
		} else
			// AlienFighter
			if (TL->Name == "AlienFighter") {
				cAlienSpaceFighter *Fighter = new cAlienSpaceFighter;
				if (xmlDoc->TestEntryAttribute(TL, "type"))
					Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
				else {
					TL = TL->Next;
					continue;
				}

				SetID(Fighter, TL, xmlDoc);
				if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

				if (xmlDoc->TestEntryAttribute(TL, "speed")) {
					Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
					Fighter->Speed = Fighter->NeedSpeed;
				}
				if (xmlDoc->TestEntryAttribute(TL, "speedlr")) {
					Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
					Fighter->SpeedLR = Fighter->NeedSpeedLR;
				}
				if (xmlDoc->TestEntryAttribute(TL, "speedud")) {
					Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
					Fighter->SpeedUD = Fighter->NeedSpeedUD;
				}

				if (xmlDoc->TestEntryAttribute(TL, "speedbycamfb")) {
					Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
					Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
				}
				if (xmlDoc->TestEntryAttribute(TL, "speedbycamlr")) {
					Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
					Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
				}
				if (xmlDoc->TestEntryAttribute(TL, "speedbycamud")) {
					Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
					Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
				}

				SetShowDeleteOnHide(Fighter, TL, xmlDoc);
				SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
				SetShipRotation(Fighter, TL, xmlDoc);
				SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

				// дальше смотрим, что нужно сделать...
				cXMLEntry *TLAlienFighter = TL->FirstChild;
				while (TLAlienFighter) {
					if (TLAlienFighter->Name == "TimeSheet") {
						// собираем новый элемент
						sTimeSheet *TimeSheet;
						TimeSheet = new sTimeSheet;
						Fighter->AttachTimeSheet(TimeSheet);

						if (xmlDoc->TestEntryAttribute(TLAlienFighter, "aimode")) {
							TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLAlienFighter, "aimode");
							TimeSheet->Time = 0.0f;
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "time"))
								TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLAlienFighter, "time");
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
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "time"))
								TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLAlienFighter, "time");
							TimeSheet->InUse = false;

							TimeSheet->Speed = 0.0f;
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "speed"))
								TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speed");

							TimeSheet->Acceler = 1.0f;//0-1
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "acceler"))
								TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLAlienFighter, "acceler");
							vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

							TimeSheet->SpeedLR = 0.0f;
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "speedlr"))
								TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedlr");

							TimeSheet->AccelerLR = 1.0f;//0-1
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "accelerlr"))
								TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerlr");
							vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

							TimeSheet->SpeedUD = 0.0f;
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "speedud"))
								TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedud");

							TimeSheet->AccelerUD = 1.0f;//0-1
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "accelerud"))
								TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerud");
							vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

							TimeSheet->SpeedByCamFB = 0.0f;
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "speedbycamfb"))
								TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamfb");

							TimeSheet->AccelerByCamFB = 1.0f;//0-1
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "accelerbycamfb"))
								TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamfb");
							vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

							TimeSheet->SpeedByCamLR = 0.0f;
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "speedbycamlr"))
								TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamlr");

							TimeSheet->AccelerByCamLR = 1.0f;//0-1
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "accelerbycamlr"))
								TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamlr");
							vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

							TimeSheet->SpeedByCamUD = 0.0f;
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "speedbycamud"))
								TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamud");

							TimeSheet->AccelerByCamUD = 1.0f;//0-1
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "accelerbycamud"))
								TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamud");
							vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

							TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "rotx"))
								TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotx");
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "roty"))
								TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLAlienFighter, "roty");
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "rotz"))
								TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotz");

							TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "rotacx"))
								TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacx");
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "rotacy"))
								TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacy");
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "rotacz"))
								TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacz");
							vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
							vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
							vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

							TimeSheet->Fire = false;
							if (xmlDoc->TestEntryAttribute(TLAlienFighter, "fire"))
								if (xmlDoc->iGetEntryAttribute(TLAlienFighter, "fire") > 0)
									TimeSheet->Fire = true;
							TimeSheet->BossFire = false;

							TimeSheet->Targeting = false;
						}

					}

					// берем следующий элемент
					TLAlienFighter = TLAlienFighter->Next;
				}
			} else
				// AlienMotherShip
				if (TL->Name == "AlienMotherShip") {
					cAlienSpaceMotherShip *Fighter = new cAlienSpaceMotherShip;
					if (xmlDoc->TestEntryAttribute(TL, "type"))
						Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
					else {
						TL = TL->Next;
						continue;
					}

					SetID(Fighter, TL, xmlDoc);
					if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

					if (xmlDoc->TestEntryAttribute(TL, "speed")) {
						Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
						Fighter->Speed = Fighter->NeedSpeed;
					}
					if (xmlDoc->TestEntryAttribute(TL, "speedlr")) {
						Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
						Fighter->SpeedLR = Fighter->NeedSpeedLR;
					}
					if (xmlDoc->TestEntryAttribute(TL, "speedud")) {
						Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
						Fighter->SpeedUD = Fighter->NeedSpeedUD;
					}

					if (xmlDoc->TestEntryAttribute(TL, "speedbycamfb")) {
						Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
						Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
					}
					if (xmlDoc->TestEntryAttribute(TL, "speedbycamlr")) {
						Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
						Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
					}
					if (xmlDoc->TestEntryAttribute(TL, "speedbycamud")) {
						Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
						Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
					}

					SetShowDeleteOnHide(Fighter, TL, xmlDoc);
					SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
					SetShipRotation(Fighter, TL, xmlDoc);
					SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

					// дальше смотрим, что нужно сделать...
					cXMLEntry *TLAlienMotherShip = TL->FirstChild;
					while (TLAlienMotherShip) {
						if (TLAlienMotherShip->Name == "TimeSheet") {
							// собираем новый элемент
							sTimeSheet *TimeSheet;
							TimeSheet = new sTimeSheet;
							Fighter->AttachTimeSheet(TimeSheet);

							if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "aimode")) {
								TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "aimode");
								TimeSheet->Time = 0.0f;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "time"))
									TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "time");
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
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "time"))
									TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "time");
								TimeSheet->InUse = false;

								TimeSheet->Speed = 0.0f;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "speed"))
									TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speed");

								TimeSheet->Acceler = 1.0f;//0-1
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "acceler"))
									TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "acceler");
								vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

								TimeSheet->SpeedLR = 0.0f;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "speedlr"))
									TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedlr");

								TimeSheet->AccelerLR = 1.0f;//0-1
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "accelerlr"))
									TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerlr");
								vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

								TimeSheet->SpeedUD = 0.0f;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "speedud"))
									TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedud");

								TimeSheet->AccelerUD = 1.0f;//0-1
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "accelerud"))
									TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerud");
								vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

								TimeSheet->SpeedByCamFB = 0.0f;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "speedbycamfb"))
									TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamfb");

								TimeSheet->AccelerByCamFB = 1.0f;//0-1
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "accelerbycamfb"))
									TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamfb");
								vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

								TimeSheet->SpeedByCamLR = 0.0f;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "speedbycamlr"))
									TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamlr");

								TimeSheet->AccelerByCamLR = 1.0f;//0-1
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "accelerbycamlr"))
									TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamlr");
								vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

								TimeSheet->SpeedByCamUD = 0.0f;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "speedbycamud"))
									TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamud");

								TimeSheet->AccelerByCamUD = 1.0f;//0-1
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "accelerbycamud"))
									TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamud");
								vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

								TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "rotx"))
									TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotx");
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "roty"))
									TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "roty");
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "rotz"))
									TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotz");

								TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "rotacx"))
									TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacx");
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "rotacy"))
									TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacy");
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "rotacz"))
									TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacz");
								vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
								vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
								vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

								TimeSheet->Fire = false;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "fire"))
									if (xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "fire") > 0)
										TimeSheet->Fire = true;
								TimeSheet->BossFire = false;
								if (xmlDoc->TestEntryAttribute(TLAlienMotherShip, "bossfire"))
									if (xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "bossfire") > 0)
										TimeSheet->BossFire = true;

								TimeSheet->Targeting = false;
							}

						}

						// берем следующий элемент
						TLAlienMotherShip = TLAlienMotherShip->Next;
					}
				} else
					// PirateShip
					if (TL->Name == "PirateShip") {
						cPirateShip *Fighter = new cPirateShip;
						if (xmlDoc->TestEntryAttribute(TL, "type"))
							Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
						else {
							TL = TL->Next;
							continue;
						}

						SetID(Fighter, TL, xmlDoc);
						if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

						if (xmlDoc->TestEntryAttribute(TL, "speed")) {
							Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
							Fighter->Speed = Fighter->NeedSpeed;
						}
						if (xmlDoc->TestEntryAttribute(TL, "speedlr")) {
							Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
							Fighter->SpeedLR = Fighter->NeedSpeedLR;
						}
						if (xmlDoc->TestEntryAttribute(TL, "speedud")) {
							Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
							Fighter->SpeedUD = Fighter->NeedSpeedUD;
						}

						if (xmlDoc->TestEntryAttribute(TL, "speedbycamfb")) {
							Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
							Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
						}
						if (xmlDoc->TestEntryAttribute(TL, "speedbycamlr")) {
							Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
							Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
						}
						if (xmlDoc->TestEntryAttribute(TL, "speedbycamud")) {
							Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
							Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
						}

						SetShowDeleteOnHide(Fighter, TL, xmlDoc);
						SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
						SetShipRotation(Fighter, TL, xmlDoc);
						SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

						// дальше смотрим, что нужно сделать...
						cXMLEntry *TLPirateShip = TL->FirstChild;
						while (TLPirateShip) {
							if (TLPirateShip->Name == "TimeSheet") {
								// собираем новый элемент
								sTimeSheet *TimeSheet;
								TimeSheet = new sTimeSheet;
								Fighter->AttachTimeSheet(TimeSheet);

								if (xmlDoc->TestEntryAttribute(TLPirateShip, "aimode")) {
									TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLPirateShip, "aimode");
									TimeSheet->Time = 0.0f;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "time"))
										TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLPirateShip, "time");
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
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "time"))
										TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLPirateShip, "time");
									TimeSheet->InUse = false;

									TimeSheet->Speed = 0.0f;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "speed"))
										TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLPirateShip, "speed");

									TimeSheet->Acceler = 1.0f;//0-1
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "acceler"))
										TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLPirateShip, "acceler");
									vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

									TimeSheet->SpeedLR = 0.0f;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "speedlr"))
										TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedlr");

									TimeSheet->AccelerLR = 1.0f;//0-1
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "accelerlr"))
										TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerlr");
									vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

									TimeSheet->SpeedUD = 0.0f;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "speedud"))
										TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedud");

									TimeSheet->AccelerUD = 1.0f;//0-1
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "accelerud"))
										TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerud");
									vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

									TimeSheet->SpeedByCamFB = 0.0f;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "speedbycamfb"))
										TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamfb");

									TimeSheet->AccelerByCamFB = 1.0f;//0-1
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "accelerbycamfb"))
										TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamfb");
									vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

									TimeSheet->SpeedByCamLR = 0.0f;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "speedbycamlr"))
										TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamlr");

									TimeSheet->AccelerByCamLR = 1.0f;//0-1
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "accelerbycamlr"))
										TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamlr");
									vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

									TimeSheet->SpeedByCamUD = 0.0f;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "speedbycamud"))
										TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamud");

									TimeSheet->AccelerByCamUD = 1.0f;//0-1
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "accelerbycamud"))
										TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamud");
									vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

									TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "rotx"))
										TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotx");
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "roty"))
										TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLPirateShip, "roty");
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "rotz"))
										TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotz");

									TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "rotacx"))
										TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacx");
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "rotacy"))
										TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacy");
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "rotacz"))
										TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacz");
									vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
									vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
									vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

									TimeSheet->Fire = false;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "fire"))
										if (xmlDoc->iGetEntryAttribute(TLPirateShip, "fire") > 0)
											TimeSheet->Fire = true;

									TimeSheet->BossFire = false;
									if (xmlDoc->TestEntryAttribute(TLPirateShip, "bossfire"))
										if (xmlDoc->iGetEntryAttribute(TLPirateShip, "bossfire") > 0)
											TimeSheet->BossFire = true;

									TimeSheet->Targeting = false;
								}

							}

							// берем следующий элемент
							TLPirateShip = TLPirateShip->Next;
						}
					} else
						// CreateAsteroid
						if (TL->Name == "CreateAsteroid") {
							cAsteroid *Asteroid = new cAsteroid;

							// тип сейчас не задействован, всегда ставим 1
							Asteroid->Create(1);

							SetID(Asteroid, TL, xmlDoc);
							if (ShowDebugModeLine) SetDebugInformation(Asteroid, TL);
							if (xmlDoc->TestEntryAttribute(TL, "speed"))
								Asteroid->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");
							SetShowDeleteOnHide(Asteroid, TL, xmlDoc);

							SetRotation(Asteroid, TL, xmlDoc);
							SetLocation(Asteroid, TL, xmlDoc, TimeOpLag);

							if (xmlDoc->TestEntryAttribute(TL, "rotx"))
								Asteroid->RotationSpeed.x = xmlDoc->fGetEntryAttribute(TL, "rotx");
							if (xmlDoc->TestEntryAttribute(TL, "roty"))
								Asteroid->RotationSpeed.y = xmlDoc->fGetEntryAttribute(TL, "roty");
							if (xmlDoc->TestEntryAttribute(TL, "rotz"))
								Asteroid->RotationSpeed.z = xmlDoc->fGetEntryAttribute(TL, "rotz");
						} else
							// CreateBasePart
							if (TL->Name == "CreateBasePart") {
								cBasePart *BasePart = new cBasePart;

								// тип части
								if (xmlDoc->TestEntryAttribute(TL, "type"))
									BasePart->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
								else {
									TL = TL->Next;
									continue;
								}

								if (xmlDoc->TestEntryAttribute(TL, "speed"))
									BasePart->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");

								SetID(BasePart, TL, xmlDoc);
								if (ShowDebugModeLine) SetDebugInformation(BasePart, TL);
								SetShowDeleteOnHide(BasePart, TL, xmlDoc);

								SetRotation(BasePart, TL, xmlDoc);
								SetLocation(BasePart, TL, xmlDoc, TimeOpLag);
							} else
								// CreateBigAsteroid
								if (TL->Name == "CreateBigAsteroid") {
									cBigAsteroid *BigAsteroid = new cBigAsteroid;

									// тип части
									if (xmlDoc->TestEntryAttribute(TL, "type"))
										BigAsteroid->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
									else {
										TL = TL->Next;
										continue;
									}

									if (xmlDoc->TestEntryAttribute(TL, "speed"))
										BigAsteroid->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");

									SetID(BigAsteroid, TL, xmlDoc);
									if (ShowDebugModeLine) SetDebugInformation(BigAsteroid, TL);
									SetShowDeleteOnHide(BigAsteroid, TL, xmlDoc);

									SetRotation(BigAsteroid, TL, xmlDoc);
									SetLocation(BigAsteroid, TL, xmlDoc, TimeOpLag);
								} else
									// CreateMBuilding
									if (TL->Name == "CreateMBuilding") {
										cMilitaryBuilding *GroundObject = new cMilitaryBuilding;

										// тип части
										if (xmlDoc->TestEntryAttribute(TL, "type"))
											GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
										else {
											TL = TL->Next;
											continue;
										}

										SetID(GroundObject, TL, xmlDoc);
										if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
										SetShowDeleteOnHide(GroundObject, TL, xmlDoc);
										SetAIMode(GroundObject, TL, xmlDoc); // на тот случае если просто ставим и все...

										SetRotation(GroundObject, TL, xmlDoc);
										SetLocation(GroundObject, TL, xmlDoc, TimeOpLag);

										// дальше смотрим, что нужно сделать...
										cXMLEntry *TLGroundObject = TL->FirstChild;
										while (TLGroundObject) {
											if (TLGroundObject->Name == "TimeSheet") {
												// собираем новый элемент
												sTimeSheet *TimeSheet;
												TimeSheet = new sTimeSheet;
												GroundObject->AttachTimeSheet(TimeSheet);

												if (xmlDoc->TestEntryAttribute(TLGroundObject, "aimode")) {
													TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
													TimeSheet->Time = 0.0f;
													if (xmlDoc->TestEntryAttribute(TLGroundObject, "time"))
														TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
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
													if (xmlDoc->TestEntryAttribute(TLGroundObject, "time"))
														TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
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
													if (xmlDoc->TestEntryAttribute(TLGroundObject, "fire"))
														if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
															TimeSheet->Fire = true;

													TimeSheet->BossFire = false;

													TimeSheet->Targeting = false;
													if (xmlDoc->TestEntryAttribute(TLGroundObject, "targeting"))
														if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") != 0) TimeSheet->Targeting = true;
												}

											}

											// берем следующий элемент
											TLGroundObject = TLGroundObject->Next;
										}
									} else
										// CreateBuilding
										if (TL->Name == "CreateBuilding") {
											cBuilding *GroundObject = new cBuilding;
											if (xmlDoc->TestEntryAttribute(TL, "type"))
												GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
											else {
												TL = TL->Next;
												continue;
											}

											SetID(GroundObject, TL, xmlDoc);
											if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
											SetShowDeleteOnHide(GroundObject, TL, xmlDoc);

											SetRotation(GroundObject, TL, xmlDoc);
											SetLocation(GroundObject, TL, xmlDoc, TimeOpLag);
										} else
											// CreateMine
											if (TL->Name == "CreateMine") {
												cProjectile *Mine = new cProjectile;
												// т.к. мины у нас с 214-217, делаем +213
												if (xmlDoc->TestEntryAttribute(TL, "type")) {
													int MineType = xmlDoc->iGetEntryAttribute(TL, "type")+213;
													if (MineType < 214) MineType = 214;
													if (MineType > 217) MineType = 217;
													Mine->Create(MineType);
												} else {
													TL = TL->Next;
													continue;
												}

												Mine->ProjectileType = 1;

												// по умолчанию враг
												Mine->ObjectStatus = 1;
												if (xmlDoc->TestEntryAttribute(TL, "status"))
													Mine->ObjectStatus = xmlDoc->iGetEntryAttribute(TL, "status");

												// общий - пенальти, если не игрок
												float CurrentPenalty = GameNPCWeaponPenalty*1.0f;
												// если игрок или свои - ничего не надо...
												if (Mine->ObjectStatus >= 2) CurrentPenalty = 1.0f;

												Mine->DamageHull = Mine->DamageHull/CurrentPenalty;
												Mine->DamageSystems = Mine->DamageSystems/CurrentPenalty;
												Mine->SpeedStart = Mine->SpeedEnd = Mine->Speed = Mine->SpeedStart/CurrentPenalty;

												SetID(Mine, TL, xmlDoc);
												if (ShowDebugModeLine) SetDebugInformation(Mine, TL);
												SetShowDeleteOnHide(Mine, TL, xmlDoc);


												SetProjectileRotation(Mine, TL, xmlDoc);
												SetProjectileLocation(Mine, TL, xmlDoc, TimeOpLag);
											} else
												// CreateTracked
												if (TL->Name == "CreateTracked") {
													cTracked *GroundObject = new cTracked;
													if (xmlDoc->TestEntryAttribute(TL, "type"))
														GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
													else {
														TL = TL->Next;
														continue;
													}

													SetID(GroundObject, TL, xmlDoc);
													if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
													if (xmlDoc->TestEntryAttribute(TL, "speed")) {
														GroundObject->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
														GroundObject->Speed = GroundObject->NeedSpeed;
													}

													SetShowDeleteOnHide(GroundObject, TL, xmlDoc);
													SetAIMode(GroundObject, TL, xmlDoc); // на тот случае если просто ставим и все...
													SetRotation(GroundObject, TL, xmlDoc);
													SetLocation(GroundObject, TL, xmlDoc, TimeOpLag);

													// дальше смотрим, что нужно сделать...
													cXMLEntry *TLGroundObject = TL->FirstChild;
													while (TLGroundObject) {
														if (TLGroundObject->Name == "TimeSheet") {
															// собираем новый элемент
															sTimeSheet *TimeSheet;
															TimeSheet = new sTimeSheet;
															GroundObject->AttachTimeSheet(TimeSheet);

															if (xmlDoc->TestEntryAttribute(TLGroundObject, "aimode")) {
																TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
																TimeSheet->Time = 0.0f;
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "time"))
																	TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
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
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "time"))
																	TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
																TimeSheet->InUse = false;

																TimeSheet->Speed = 0.0f;
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "speed"))
																	TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLGroundObject, "speed");

																TimeSheet->Acceler = 1.0f;//0-1
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "acceler"))
																	TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLGroundObject, "acceler");
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
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotx"))
																	TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotx");
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "roty"))
																	TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "roty");
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotz"))
																	TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotz");

																TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotacx"))
																	TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacx");
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotacy"))
																	TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacy");
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotacz"))
																	TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacz");
																vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
																vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
																vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

																TimeSheet->Fire = false;
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "fire"))
																	if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
																		TimeSheet->Fire = true;

																TimeSheet->BossFire = false;

																TimeSheet->Targeting = false;
																if (xmlDoc->TestEntryAttribute(TLGroundObject, "targeting"))
																	if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") > 0)
																		TimeSheet->Targeting = true;
															}

														}

														// берем следующий элемент
														TLGroundObject = TLGroundObject->Next;
													}
												} else
													// CreateWheeled
													if (TL->Name == "CreateWheeled") {
														cWheeled *GroundObject = new cWheeled;
														if (xmlDoc->TestEntryAttribute(TL, "type"))
															GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
														else {
															TL = TL->Next;
															continue;
														}

														SetID(GroundObject, TL, xmlDoc);
														if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
														if (xmlDoc->TestEntryAttribute(TL, "speed")) {
															GroundObject->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
															GroundObject->Speed = GroundObject->NeedSpeed;
														}

														SetShowDeleteOnHide(GroundObject, TL, xmlDoc);
														SetAIMode(GroundObject, TL, xmlDoc); // на тот случае если просто ставим и все...
														SetRotation(GroundObject, TL, xmlDoc);
														SetLocation(GroundObject, TL, xmlDoc, TimeOpLag);

														// дальше смотрим, что нужно сделать...
														cXMLEntry *TLGroundObject = TL->FirstChild;
														while (TLGroundObject) {
															if (TLGroundObject->Name == "TimeSheet") {
																// собираем новый элемент
																sTimeSheet *TimeSheet;
																TimeSheet = new sTimeSheet;
																GroundObject->AttachTimeSheet(TimeSheet);

																if (xmlDoc->TestEntryAttribute(TLGroundObject, "aimode")) {
																	TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
																	TimeSheet->Time = 0.0f;
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "time"))
																		TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
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
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "time"))
																		TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
																	TimeSheet->InUse = false;

																	TimeSheet->Speed = 0.0f;
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "speed"))
																		TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLGroundObject, "speed");


																	TimeSheet->Acceler = 1.0f;//0-1
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "acceler"))
																		TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLGroundObject, "acceler");
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
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotx"))
																		TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotx");
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "roty"))
																		TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "roty");
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotz"))
																		TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotz");

																	TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotacx"))
																		TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacx");
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotacy"))
																		TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacy");
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "rotacz"))
																		TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacz");
																	vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
																	vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
																	vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

																	TimeSheet->Fire = false;
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "fire"))
																		if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
																			TimeSheet->Fire = true;

																	TimeSheet->BossFire = false;

																	TimeSheet->Targeting = false;
																	if (xmlDoc->TestEntryAttribute(TLGroundObject, "targeting"))
																		if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") != 0) TimeSheet->Targeting = true;
																}

															}

															// берем следующий элемент
															TLGroundObject = TLGroundObject->Next;
														}
													} else {
														// если тут - значит не нашли директиву, или произошла ошибка
														std::cerr << "ScriptEngine: tag " << TL->Name
															  << " not found, line " << TL->LineNumber << "\n";
													}


		// берем следующий элемент по порядку
		TL = TL->Next;
	}
}




