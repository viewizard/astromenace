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



void SetID(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetDebugInformation(CObject3D *Object, cXMLEntry *xmlEntry);
void SetShowDeleteOnHide(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetShipRotation(CSpaceShip *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetShipLocation(CSpaceShip *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag);
void SetProjectileRotation(CProjectile *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetProjectileLocation(CProjectile *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag);
void SetRotation(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);
void SetLocation(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc, float TimeOpLag);
void SetAIMode(CObject3D *Object, cXMLEntry *xmlEntry, cXMLDocument *xmlDoc);




// проверка, если конец уровня как всех убъем

extern CSpaceShip *StartSpaceShip;
extern CSpaceShip *EndSpaceShip;
extern CGroundObject *StartGroundObject;
extern CGroundObject *EndGroundObject;

// отображение коробок... отладка
extern int NeedShowBB;
// неубиваемость... отладка
extern bool UndeadDebugMode;
// показывать время при скорости 1.5
extern bool ShowGameTime;






ScriptEngine::ScriptEngine()
{
	// отладочный режим
	NeedShowBB = 0;
	UndeadDebugMode = false;
};

ScriptEngine::~ScriptEngine()
{
	delete xmlDoc;
};



//-----------------------------------------------------------------------------
// запустить скрипт на выполнение
//-----------------------------------------------------------------------------
bool ScriptEngine::RunScript(const char *FileName, float InitTime)
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
	xmlDoc = new cXMLDocument;

	// иним скрипт
	if (!xmlDoc->Load(FileName)) {
		fprintf(stderr, "Can't find script file or file corrupted: %s\n", FileName);
		delete xmlDoc;
		xmlDoc = nullptr;
		return false;
	}


	// проверяем корневой элемент
	if (strcmp("AstroMenaceScript", xmlDoc->RootXMLEntry->Name)) {
		fprintf(stderr, "Can't find AstroMenaceScript element in the: %s\n", FileName);
		delete xmlDoc;
		xmlDoc = nullptr;
		return false;
	}


	// переходим на действия
	xmlEntry  = xmlDoc->FindFirstChildEntryByName(xmlDoc->RootXMLEntry, "Action");
	if (xmlEntry == nullptr) {
		fprintf(stderr, "Can't find Action element in the: %s\n", FileName);
		delete xmlDoc;
		xmlDoc = nullptr;
		return false;
	}

	xmlEntry = xmlEntry->FirstChild;
	if (xmlEntry == nullptr) {
		fprintf(stderr, "Can't find Action element in the: %s\n", FileName);
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
bool ScriptEngine::Update(float Time)
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
			CAsteroid *CreateAsteroid = new CAsteroid;
			CreateAsteroid->Create(1);
			if (AsterFastCount != 20)
				CreateAsteroid->Speed = AsterMaxSpeed*vw_Randf1;
			else
				CreateAsteroid->Speed = AsterMinFastSpeed + AsterMaxSpeed*vw_Randf1;
			CreateAsteroid->ShowDeleteOnHide = 0;
			CreateAsteroid->SetRotation(VECTOR3D(0.0f, 180.0f, 0.0f));// !!!учесть камеру

			if (AsterFastCount != 20)
				CreateAsteroid->SetLocation(
					VECTOR3D(AsterW*vw_Randf0+AsterXPos, AsterYPos*2+AsterH*vw_Randf1, AsterZPos+20.0f)
					+GamePoint);
			else
				CreateAsteroid->SetLocation(
					VECTOR3D(AsterW*vw_Randf0+AsterXPos, AsterYPos*2+AsterH*vw_Randf1, AsterZPos)
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
			CSpaceShip *Tmp1 = StartSpaceShip;
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
			CGroundObject *Tmp1 = StartGroundObject;
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
		if (!strcmp(xmlEntry->Name, "TimeLine")) {
			float onTime = 0.0f;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "value") != nullptr)
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
			if (!strcmp(xmlEntry->Name, "Debug")) {
				ShowDebugModeLine = false;
				if (xmlDoc->GetEntryAttribute(xmlEntry, "showline") != nullptr)
					ShowDebugModeLine = xmlDoc->bGetEntryAttribute(xmlEntry, "showline");

				NeedShowBB = 0;
				if (xmlDoc->GetEntryAttribute(xmlEntry, "showbb") != nullptr)
					NeedShowBB = xmlDoc->iGetEntryAttribute(xmlEntry, "showbb");

				UndeadDebugMode = false;
				if (xmlDoc->GetEntryAttribute(xmlEntry, "undead") != nullptr)
					UndeadDebugMode = xmlDoc->bGetEntryAttribute(xmlEntry, "undead");

				ShowGameTime = false;
				if (xmlDoc->GetEntryAttribute(xmlEntry, "time") != nullptr)
					ShowGameTime = xmlDoc->bGetEntryAttribute(xmlEntry, "time");

			} else
				// StarSystem
				if (!strcmp(xmlEntry->Name, "StarSystem")) {
					if (xmlDoc->GetEntryAttribute(xmlEntry, "system") != nullptr) {
						int SystemNum = xmlDoc->iGetEntryAttribute(xmlEntry, "system");
						VECTOR3D TmpBaseRotation(0.0f, 0.0f, 0.0f);
						if (xmlDoc->GetEntryAttribute(xmlEntry, "anglex") != nullptr)
							TmpBaseRotation.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglex");
						if (xmlDoc->GetEntryAttribute(xmlEntry, "angley") != nullptr)
							TmpBaseRotation.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angley");
						if (xmlDoc->GetEntryAttribute(xmlEntry, "anglez") != nullptr)
							TmpBaseRotation.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglez");
						StarSystemInit(SystemNum, TmpBaseRotation);
					}
				} else
					// Music
					if (!strcmp(xmlEntry->Name, "Music")) {
						// если корабль игрока уничтожен - не меняем уже музыку в игре вообще,
						// должна проигрываться только музыка поражения
						if (PlayerFighter != nullptr)
							if (PlayerFighter->Strength > 0.0f) {
								if (xmlDoc->GetEntryAttribute(xmlEntry, "theme") != nullptr) {
									int Theme = xmlDoc->iGetEntryAttribute(xmlEntry, "theme");
									if (Theme == 1)
										StartMusicWithFade(2, 2.0f, 2.0f);
									if (Theme == 2)
										StartMusicWithFade(3, 1.5f, 2.0f);
								}
							}
					} else
						// CreatePlanet
						if (!strcmp(xmlEntry->Name, "CreatePlanet")) {
							CPlanet *Planet = new CPlanet;
							if (xmlDoc->GetEntryAttribute(xmlEntry, "type") != nullptr) {
								Planet->Create(xmlDoc->iGetEntryAttribute(xmlEntry, "type"));
								SetRotation(Planet, xmlEntry, xmlDoc);
								SetLocation(Planet, xmlEntry, xmlDoc, 0.0f);
								Planet->ShowDeleteOnHide = 0;

								if (xmlDoc->GetEntryAttribute(xmlEntry, "speed") != nullptr)
									Planet->Speed = xmlDoc->fGetEntryAttribute(xmlEntry, "speed");
							}
						} else
							// AsteroidField
							if (!strcmp(xmlEntry->Name, "AsteroidField")) {
								if (xmlDoc->GetEntryAttribute(xmlEntry, "status") != nullptr) {
									AsterOn = xmlDoc->bGetEntryAttribute(xmlEntry, "status");
								}
								if (xmlDoc->GetEntryAttribute(xmlEntry, "persec") != nullptr)
									AsterQuant = xmlDoc->fGetEntryAttribute(xmlEntry, "persec");
								if (xmlDoc->GetEntryAttribute(xmlEntry, "w") != nullptr)
									AsterW = xmlDoc->fGetEntryAttribute(xmlEntry, "w");
								if (xmlDoc->GetEntryAttribute(xmlEntry, "h") != nullptr)
									AsterH = xmlDoc->fGetEntryAttribute(xmlEntry, "h");
								if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != nullptr)
									AsterXPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posx");
								if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != nullptr)
									AsterYPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posy");
								if (xmlDoc->GetEntryAttribute(xmlEntry, "posz") != nullptr)
									AsterZPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posz");
								if (xmlDoc->GetEntryAttribute(xmlEntry, "slow") != nullptr)
									AsterMaxSpeed = xmlDoc->fGetEntryAttribute(xmlEntry, "slow");
								if (xmlDoc->GetEntryAttribute(xmlEntry, "fast") != nullptr)
									AsterMinFastSpeed = xmlDoc->fGetEntryAttribute(xmlEntry, "fast");
							} else
								// Light
								if (!strcmp(xmlEntry->Name, "Light")) {
									eLight *NewLight;
									NewLight = new eLight;

									NewLight->LightType = 0; // по умолчанию, солнце
									if (xmlDoc->GetEntryAttribute(xmlEntry, "type") != nullptr)
										NewLight->LightType = xmlDoc->iGetEntryAttribute(xmlEntry, "type");

									NewLight->Diffuse[0] = 0.0f;
									NewLight->Diffuse[1] = 0.0f;
									NewLight->Diffuse[2] = 0.0f;
									NewLight->Diffuse[3] = 1.0f;
									if (xmlDoc->GetEntryAttribute(xmlEntry, "diffr") != nullptr)
										NewLight->Diffuse[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffr");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "diffg") != nullptr)
										NewLight->Diffuse[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffg");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "diffb") != nullptr)
										NewLight->Diffuse[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffb");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "diffa") != nullptr)
										NewLight->Diffuse[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffa");

									NewLight->Specular[0] = 0.0f;
									NewLight->Specular[1] = 0.0f;
									NewLight->Specular[2] = 0.0f;
									NewLight->Specular[3] = 1.0f;
									if (xmlDoc->GetEntryAttribute(xmlEntry, "specr") != nullptr)
										NewLight->Specular[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "specr");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "specg") != nullptr)
										NewLight->Specular[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "specg");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "specb") != nullptr)
										NewLight->Specular[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "specb");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "speca") != nullptr)
										NewLight->Specular[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "speca");

									NewLight->Ambient[0] = 0.0f;
									NewLight->Ambient[1] = 0.0f;
									NewLight->Ambient[2] = 0.0f;
									NewLight->Ambient[3] = 1.0f;
									if (xmlDoc->GetEntryAttribute(xmlEntry, "ambir") != nullptr)
										NewLight->Ambient[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambir");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "ambig") != nullptr)
										NewLight->Ambient[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambig");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "ambib") != nullptr)
										NewLight->Ambient[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambib");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "ambia") != nullptr)
										NewLight->Ambient[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambia");

									NewLight->Direction = VECTOR3D(0.0f,0.0f,1.0f);
									if (xmlDoc->GetEntryAttribute(xmlEntry, "dirx") != nullptr)
										NewLight->Direction.x = xmlDoc->fGetEntryAttribute(xmlEntry, "dirx");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "diry") != nullptr)
										NewLight->Direction.y = xmlDoc->fGetEntryAttribute(xmlEntry, "diry");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "dirz") != nullptr)
										NewLight->Direction.z = xmlDoc->fGetEntryAttribute(xmlEntry, "dirz");
									NewLight->Direction.Normalize();

									NewLight->Location = VECTOR3D(0.0f,0.0f,0.0f);
									if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != nullptr)
										NewLight->Location.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != nullptr)
										NewLight->Location.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy");
									if (xmlDoc->GetEntryAttribute(xmlEntry, "posz") != nullptr)
										NewLight->Location.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz");


									NewLight->On = true;
									if (xmlDoc->GetEntryAttribute(xmlEntry, "status") != nullptr)
										NewLight->On = xmlDoc->bGetEntryAttribute(xmlEntry, "status");
								} else
									// Label
									if (!strcmp(xmlEntry->Name, "Label")) {
										// ничего не делаем
										xmlEntry = xmlEntry->Next;
										continue;
									} else
										// Goto
										if (!strcmp(xmlEntry->Name, "Goto")) {
											// если есть указатель на метку
											if (xmlDoc->GetEntryAttribute(xmlEntry, "label") != nullptr) {
												// нужно перебрать все метки и остановится на нужной
												cXMLEntry *tmpEntry = xmlDoc->FindFirstChildEntryByName(xmlDoc->FindFirstChildEntryByName(xmlDoc->RootXMLEntry, "Action"), "Label");

												// перебор по всем меткам
												while (tmpEntry) {
													//	if (!strcmp(tmpEntry->Name, "Label"))
													if (xmlDoc->GetEntryAttribute(tmpEntry, "name") != nullptr)
														if (!strcmp(xmlDoc->GetEntryAttribute(tmpEntry, "name"), xmlDoc->GetEntryAttribute(xmlEntry, "label"))) {
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
											if (!strcmp(xmlEntry->Name, "MissionComplete")) {
												SetGameMissionComplete();
											} else
												// MissionCompleteAtNoEnemy
												if (!strcmp(xmlEntry->Name, "MissionCompleteAtNoEnemy")) {
													bool SetGameMissionFlag = false;
													NeedCheckSpaceShip = false;
													if (xmlDoc->GetEntryAttribute(xmlEntry, "ships") != nullptr) {
														NeedCheckSpaceShip = xmlDoc->bGetEntryAttribute(xmlEntry, "ships");
														SetGameMissionFlag = true;
													};
													NeedCheckGroundObject = false;
													if (xmlDoc->GetEntryAttribute(xmlEntry, "grounds") != nullptr) {
														NeedCheckGroundObject = xmlDoc->bGetEntryAttribute(xmlEntry, "grounds");
														SetGameMissionFlag = true;
													};
													EndDelayMissionComplete = 0.0f;
													if (xmlDoc->GetEntryAttribute(xmlEntry, "delay") != nullptr) {
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
													if (!strcmp(xmlEntry->Name, "Text")) {
														CGameLvlText *NewText;
														NewText = new CGameLvlText;


														NewText->Lifetime = -1.0f;
														if (xmlDoc->GetEntryAttribute(xmlEntry, "life") != nullptr)
															NewText->Lifetime = xmlDoc->fGetEntryAttribute(xmlEntry, "life");

														if (xmlDoc->GetEntryAttribute(xmlEntry, "text") != nullptr) {
															NewText->DrawText = new char[strlen(xmlDoc->GetEntryAttribute(xmlEntry, "text"))+1];
															strcpy(NewText->DrawText, xmlDoc->GetEntryAttribute(xmlEntry, "text"));
														}

														if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != nullptr)
															NewText->PosX = xmlDoc->iGetEntryAttribute(xmlEntry, "posx");
														if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != nullptr)
															NewText->PosY = xmlDoc->iGetEntryAttribute(xmlEntry, "posy");

														NewText->Color = 0;
														if (xmlDoc->GetEntryAttribute(xmlEntry, "color") != nullptr)
															NewText->Color = xmlDoc->iGetEntryAttribute(xmlEntry, "color");
													} else {
														// если тут - значит не нашли директиву, или произошла ошибка
														fprintf(stderr, "ScriptEngine: tag %s not found, line %i.", xmlEntry->Name, xmlEntry->LineNumber);
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
void ScriptEngine::UpdateTimeLine()
{
	// получаем первый тэг
	cXMLEntry *TL = xmlEntry->FirstChild;

	while (TL) {

		// EarthFighter
		if (!strcmp(TL->Name, "EarthFighter")) {
			CEarthSpaceFighter *Fighter = nullptr;
			Fighter = new CEarthSpaceFighter;
			if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
				Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else {
				TL = TL->Next;
				continue;
			}

			SetID(Fighter, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr) {
				Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedlr") != nullptr) {
				Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedud") != nullptr) {
				Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (xmlDoc->GetEntryAttribute(TL, "speedbycamfb") != nullptr) {
				Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamlr") != nullptr) {
				Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamud") != nullptr) {
				Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}


			if (xmlDoc->GetEntryAttribute(TL, "armour") != nullptr)
				SetEarthSpaceFighterArmour(Fighter, xmlDoc->iGetEntryAttribute(TL, "armour"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon1") != nullptr)
				SetEarthSpaceFighterWeapon(Fighter, 1, xmlDoc->iGetEntryAttribute(TL, "weapon1"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon2") != nullptr)
				SetEarthSpaceFighterWeapon(Fighter, 2, xmlDoc->iGetEntryAttribute(TL, "weapon2"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon3") != nullptr)
				SetEarthSpaceFighterWeapon(Fighter, 3, xmlDoc->iGetEntryAttribute(TL, "weapon3"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon4") != nullptr)
				SetEarthSpaceFighterWeapon(Fighter, 4, xmlDoc->iGetEntryAttribute(TL, "weapon4"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon5") != nullptr)
				SetEarthSpaceFighterWeapon(Fighter, 5, xmlDoc->iGetEntryAttribute(TL, "weapon5"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon6") != nullptr)
				SetEarthSpaceFighterWeapon(Fighter, 6, xmlDoc->iGetEntryAttribute(TL, "weapon6"));

			SetShowDeleteOnHide(Fighter, TL, xmlDoc);
			SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL, xmlDoc);
			SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			cXMLEntry *TLEarthFighter = TL->FirstChild;
			while (TLEarthFighter) {
				if (!strcmp(TLEarthFighter->Name, "TimeSheet")) {
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->GetEntryAttribute(TLEarthFighter, "aimode") != nullptr) {
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLEarthFighter, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "time") != nullptr)
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
						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						TimeSheet->Fire = false;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					} else {
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "time") != nullptr)
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLEarthFighter, "time");
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speed") != nullptr)
							TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speed");

						TimeSheet->Acceler = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "acceler") != nullptr)
							TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLEarthFighter, "acceler");
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedlr") != nullptr)
							TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedlr");

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerlr") != nullptr)
							TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerlr");
						vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedud") != nullptr)
							TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedud");

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerud") != nullptr)
							TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerud");
						vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);


						TimeSheet->SpeedByCamFB = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedbycamfb") != nullptr)
							TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamfb");

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerbycamfb") != nullptr)
							TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamfb");
						vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedbycamlr") != nullptr)
							TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamlr");

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerbycamlr") != nullptr)
							TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamlr");
						vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedbycamud") != nullptr)
							TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamud");

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerbycamud") != nullptr)
							TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamud");
						vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);


						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotx") != nullptr)
							TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotx");
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "roty") != nullptr)
							TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLEarthFighter, "roty");
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotz") != nullptr)
							TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotz");

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotacx") != nullptr)
							TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacx");
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotacy") != nullptr)
							TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacy");
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotacz") != nullptr)
							TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacz");
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "fire") != nullptr)
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
			if (!strcmp(TL->Name, "AlienFighter")) {
				CAlienSpaceFighter *Fighter = new CAlienSpaceFighter;
				if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
					Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
				else {
					TL = TL->Next;
					continue;
				}

				SetID(Fighter, TL, xmlDoc);
				if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

				if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr) {
					Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
					Fighter->Speed = Fighter->NeedSpeed;
				}
				if (xmlDoc->GetEntryAttribute(TL, "speedlr") != nullptr) {
					Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
					Fighter->SpeedLR = Fighter->NeedSpeedLR;
				}
				if (xmlDoc->GetEntryAttribute(TL, "speedud") != nullptr) {
					Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
					Fighter->SpeedUD = Fighter->NeedSpeedUD;
				}

				if (xmlDoc->GetEntryAttribute(TL, "speedbycamfb") != nullptr) {
					Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
					Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
				}
				if (xmlDoc->GetEntryAttribute(TL, "speedbycamlr") != nullptr) {
					Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
					Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
				}
				if (xmlDoc->GetEntryAttribute(TL, "speedbycamud") != nullptr) {
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
					if (!strcmp(TLAlienFighter->Name, "TimeSheet")) {
						// собираем новый элемент
						CTimeSheet *TimeSheet;
						TimeSheet = new CTimeSheet;
						Fighter->AttachTimeSheet(TimeSheet);

						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "aimode") != nullptr) {
							TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLAlienFighter, "aimode");
							TimeSheet->Time = 0.0f;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "time") != nullptr)
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
							TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
							TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
							TimeSheet->Fire = false;
							TimeSheet->BossFire = false;
							TimeSheet->Targeting = false;
						} else {
							TimeSheet->AI_Mode = 0;

							TimeSheet->Time = 0.0f;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "time") != nullptr)
								TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLAlienFighter, "time");
							TimeSheet->InUse = false;

							TimeSheet->Speed = 0.0f;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speed") != nullptr)
								TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speed");

							TimeSheet->Acceler = 1.0f;//0-1
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "acceler") != nullptr)
								TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLAlienFighter, "acceler");
							vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

							TimeSheet->SpeedLR = 0.0f;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedlr") != nullptr)
								TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedlr");

							TimeSheet->AccelerLR = 1.0f;//0-1
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerlr") != nullptr)
								TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerlr");
							vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

							TimeSheet->SpeedUD = 0.0f;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedud") != nullptr)
								TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedud");

							TimeSheet->AccelerUD = 1.0f;//0-1
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerud") != nullptr)
								TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerud");
							vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

							TimeSheet->SpeedByCamFB = 0.0f;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedbycamfb") != nullptr)
								TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamfb");

							TimeSheet->AccelerByCamFB = 1.0f;//0-1
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerbycamfb") != nullptr)
								TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamfb");
							vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

							TimeSheet->SpeedByCamLR = 0.0f;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedbycamlr") != nullptr)
								TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamlr");

							TimeSheet->AccelerByCamLR = 1.0f;//0-1
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerbycamlr") != nullptr)
								TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamlr");
							vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

							TimeSheet->SpeedByCamUD = 0.0f;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedbycamud") != nullptr)
								TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamud");

							TimeSheet->AccelerByCamUD = 1.0f;//0-1
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerbycamud") != nullptr)
								TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamud");
							vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

							TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotx") != nullptr)
								TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotx");
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "roty") != nullptr)
								TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLAlienFighter, "roty");
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotz") != nullptr)
								TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotz");

							TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotacx") != nullptr)
								TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacx");
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotacy") != nullptr)
								TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacy");
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotacz") != nullptr)
								TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacz");
							vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
							vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
							vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

							TimeSheet->Fire = false;
							if (xmlDoc->GetEntryAttribute(TLAlienFighter, "fire") != nullptr)
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
				if (!strcmp(TL->Name, "AlienMotherShip")) {
					CAlienSpaceMotherShip *Fighter = new CAlienSpaceMotherShip;
					if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
						Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
					else {
						TL = TL->Next;
						continue;
					}

					SetID(Fighter, TL, xmlDoc);
					if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

					if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr) {
						Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
						Fighter->Speed = Fighter->NeedSpeed;
					}
					if (xmlDoc->GetEntryAttribute(TL, "speedlr") != nullptr) {
						Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
						Fighter->SpeedLR = Fighter->NeedSpeedLR;
					}
					if (xmlDoc->GetEntryAttribute(TL, "speedud") != nullptr) {
						Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
						Fighter->SpeedUD = Fighter->NeedSpeedUD;
					}

					if (xmlDoc->GetEntryAttribute(TL, "speedbycamfb") != nullptr) {
						Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
						Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
					}
					if (xmlDoc->GetEntryAttribute(TL, "speedbycamlr") != nullptr) {
						Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
						Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
					}
					if (xmlDoc->GetEntryAttribute(TL, "speedbycamud") != nullptr) {
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
						if (!strcmp(TLAlienMotherShip->Name, "TimeSheet")) {
							// собираем новый элемент
							CTimeSheet *TimeSheet;
							TimeSheet = new CTimeSheet;
							Fighter->AttachTimeSheet(TimeSheet);

							if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "aimode") != nullptr) {
								TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "aimode");
								TimeSheet->Time = 0.0f;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "time") != nullptr)
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
								TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
								TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
								TimeSheet->Fire = false;
								TimeSheet->BossFire = false;
								TimeSheet->Targeting = false;
							} else {
								TimeSheet->AI_Mode = 0;

								TimeSheet->Time = 0.0f;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "time") != nullptr)
									TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "time");
								TimeSheet->InUse = false;

								TimeSheet->Speed = 0.0f;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speed") != nullptr)
									TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speed");

								TimeSheet->Acceler = 1.0f;//0-1
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "acceler") != nullptr)
									TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "acceler");
								vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

								TimeSheet->SpeedLR = 0.0f;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedlr") != nullptr)
									TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedlr");

								TimeSheet->AccelerLR = 1.0f;//0-1
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerlr") != nullptr)
									TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerlr");
								vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

								TimeSheet->SpeedUD = 0.0f;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedud") != nullptr)
									TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedud");

								TimeSheet->AccelerUD = 1.0f;//0-1
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerud") != nullptr)
									TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerud");
								vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

								TimeSheet->SpeedByCamFB = 0.0f;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedbycamfb") != nullptr)
									TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamfb");

								TimeSheet->AccelerByCamFB = 1.0f;//0-1
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerbycamfb") != nullptr)
									TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamfb");
								vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

								TimeSheet->SpeedByCamLR = 0.0f;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedbycamlr") != nullptr)
									TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamlr");

								TimeSheet->AccelerByCamLR = 1.0f;//0-1
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerbycamlr") != nullptr)
									TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamlr");
								vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

								TimeSheet->SpeedByCamUD = 0.0f;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedbycamud") != nullptr)
									TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamud");

								TimeSheet->AccelerByCamUD = 1.0f;//0-1
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerbycamud") != nullptr)
									TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamud");
								vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

								TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotx") != nullptr)
									TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotx");
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "roty") != nullptr)
									TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "roty");
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotz") != nullptr)
									TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotz");

								TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotacx") != nullptr)
									TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacx");
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotacy") != nullptr)
									TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacy");
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotacz") != nullptr)
									TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacz");
								vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
								vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
								vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

								TimeSheet->Fire = false;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "fire") != nullptr)
									if (xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "fire") > 0)
										TimeSheet->Fire = true;
								TimeSheet->BossFire = false;
								if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "bossfire") != nullptr)
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
					if (!strcmp(TL->Name, "PirateShip")) {
						CPirateShip *Fighter = new CPirateShip;
						if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
							Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
						else {
							TL = TL->Next;
							continue;
						}

						SetID(Fighter, TL, xmlDoc);
						if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

						if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr) {
							Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
							Fighter->Speed = Fighter->NeedSpeed;
						}
						if (xmlDoc->GetEntryAttribute(TL, "speedlr") != nullptr) {
							Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
							Fighter->SpeedLR = Fighter->NeedSpeedLR;
						}
						if (xmlDoc->GetEntryAttribute(TL, "speedud") != nullptr) {
							Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
							Fighter->SpeedUD = Fighter->NeedSpeedUD;
						}

						if (xmlDoc->GetEntryAttribute(TL, "speedbycamfb") != nullptr) {
							Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
							Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
						}
						if (xmlDoc->GetEntryAttribute(TL, "speedbycamlr") != nullptr) {
							Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
							Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
						}
						if (xmlDoc->GetEntryAttribute(TL, "speedbycamud") != nullptr) {
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
							if (!strcmp(TLPirateShip->Name, "TimeSheet")) {
								// собираем новый элемент
								CTimeSheet *TimeSheet;
								TimeSheet = new CTimeSheet;
								Fighter->AttachTimeSheet(TimeSheet);

								if (xmlDoc->GetEntryAttribute(TLPirateShip, "aimode") != nullptr) {
									TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLPirateShip, "aimode");
									TimeSheet->Time = 0.0f;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "time") != nullptr)
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
									TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
									TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
									TimeSheet->Fire = false;
									TimeSheet->BossFire = false;
									TimeSheet->Targeting = false;
								} else {
									TimeSheet->AI_Mode = 0;

									TimeSheet->Time = 0.0f;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "time") != nullptr)
										TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLPirateShip, "time");
									TimeSheet->InUse = false;

									TimeSheet->Speed = 0.0f;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "speed") != nullptr)
										TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLPirateShip, "speed");

									TimeSheet->Acceler = 1.0f;//0-1
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "acceler") != nullptr)
										TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLPirateShip, "acceler");
									vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

									TimeSheet->SpeedLR = 0.0f;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedlr") != nullptr)
										TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedlr");

									TimeSheet->AccelerLR = 1.0f;//0-1
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerlr") != nullptr)
										TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerlr");
									vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

									TimeSheet->SpeedUD = 0.0f;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedud") != nullptr)
										TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedud");

									TimeSheet->AccelerUD = 1.0f;//0-1
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerud") != nullptr)
										TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerud");
									vw_Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

									TimeSheet->SpeedByCamFB = 0.0f;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedbycamfb") != nullptr)
										TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamfb");

									TimeSheet->AccelerByCamFB = 1.0f;//0-1
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerbycamfb") != nullptr)
										TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamfb");
									vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

									TimeSheet->SpeedByCamLR = 0.0f;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedbycamlr") != nullptr)
										TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamlr");

									TimeSheet->AccelerByCamLR = 1.0f;//0-1
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerbycamlr") != nullptr)
										TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamlr");
									vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

									TimeSheet->SpeedByCamUD = 0.0f;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedbycamud") != nullptr)
										TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamud");

									TimeSheet->AccelerByCamUD = 1.0f;//0-1
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerbycamud") != nullptr)
										TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamud");
									vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

									TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotx") != nullptr)
										TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotx");
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "roty") != nullptr)
										TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLPirateShip, "roty");
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotz") != nullptr)
										TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotz");

									TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotacx") != nullptr)
										TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacx");
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotacy") != nullptr)
										TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacy");
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotacz") != nullptr)
										TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacz");
									vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
									vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
									vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

									TimeSheet->Fire = false;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "fire") != nullptr)
										if (xmlDoc->iGetEntryAttribute(TLPirateShip, "fire") > 0)
											TimeSheet->Fire = true;

									TimeSheet->BossFire = false;
									if (xmlDoc->GetEntryAttribute(TLPirateShip, "bossfire") != nullptr)
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
						if (!strcmp(TL->Name, "CreateAsteroid")) {
							CAsteroid *Asteroid = new CAsteroid;

							// тип сейчас не задействован, всегда ставим 1
							Asteroid->Create(1);

							SetID(Asteroid, TL, xmlDoc);
							if (ShowDebugModeLine) SetDebugInformation(Asteroid, TL);
							if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr)
								Asteroid->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");
							SetShowDeleteOnHide(Asteroid, TL, xmlDoc);

							SetRotation(Asteroid, TL, xmlDoc);
							SetLocation(Asteroid, TL, xmlDoc, TimeOpLag);

							if (xmlDoc->GetEntryAttribute(TL, "rotx") != nullptr)
								Asteroid->RotationSpeed.x = xmlDoc->fGetEntryAttribute(TL, "rotx");
							if (xmlDoc->GetEntryAttribute(TL, "roty") != nullptr)
								Asteroid->RotationSpeed.y = xmlDoc->fGetEntryAttribute(TL, "roty");
							if (xmlDoc->GetEntryAttribute(TL, "rotz") != nullptr)
								Asteroid->RotationSpeed.z = xmlDoc->fGetEntryAttribute(TL, "rotz");
						} else
							// CreateBasePart
							if (!strcmp(TL->Name, "CreateBasePart")) {
								CBasePart *BasePart = new CBasePart;

								// тип части
								if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
									BasePart->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
								else {
									TL = TL->Next;
									continue;
								}

								if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr)
									BasePart->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");

								SetID(BasePart, TL, xmlDoc);
								if (ShowDebugModeLine) SetDebugInformation(BasePart, TL);
								SetShowDeleteOnHide(BasePart, TL, xmlDoc);

								SetRotation(BasePart, TL, xmlDoc);
								SetLocation(BasePart, TL, xmlDoc, TimeOpLag);
							} else
								// CreateBigAsteroid
								if (!strcmp(TL->Name, "CreateBigAsteroid")) {
									CBigAsteroid *BigAsteroid = new CBigAsteroid;

									// тип части
									if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
										BigAsteroid->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
									else {
										TL = TL->Next;
										continue;
									}

									if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr)
										BigAsteroid->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");

									SetID(BigAsteroid, TL, xmlDoc);
									if (ShowDebugModeLine) SetDebugInformation(BigAsteroid, TL);
									SetShowDeleteOnHide(BigAsteroid, TL, xmlDoc);

									SetRotation(BigAsteroid, TL, xmlDoc);
									SetLocation(BigAsteroid, TL, xmlDoc, TimeOpLag);
								} else
									// CreateMBuilding
									if (!strcmp(TL->Name, "CreateMBuilding")) {
										CMilitaryBuilding *GroundObject = new CMilitaryBuilding;

										// тип части
										if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
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
											if (!strcmp(TLGroundObject->Name, "TimeSheet")) {
												// собираем новый элемент
												CTimeSheet *TimeSheet;
												TimeSheet = new CTimeSheet;
												GroundObject->AttachTimeSheet(TimeSheet);

												if (xmlDoc->GetEntryAttribute(TLGroundObject, "aimode") != nullptr) {
													TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
													TimeSheet->Time = 0.0f;
													if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != nullptr)
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
													TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
													TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
													TimeSheet->Fire = false;
													TimeSheet->BossFire = false;
													TimeSheet->Targeting = false;
												} else {
													TimeSheet->AI_Mode = 0;

													TimeSheet->Time = 0.0f;
													if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != nullptr)
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
													TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
													TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1

													TimeSheet->Fire = false;
													if (xmlDoc->GetEntryAttribute(TLGroundObject, "fire") != nullptr)
														if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
															TimeSheet->Fire = true;

													TimeSheet->BossFire = false;

													TimeSheet->Targeting = false;
													if (xmlDoc->GetEntryAttribute(TLGroundObject, "targeting") != nullptr)
														if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") != 0) TimeSheet->Targeting = true;
												}

											}

											// берем следующий элемент
											TLGroundObject = TLGroundObject->Next;
										}
									} else
										// CreateBuilding
										if (!strcmp(TL->Name, "CreateBuilding")) {
											CBuilding *GroundObject = new CBuilding;
											if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
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
											if (!strcmp(TL->Name, "CreateMine")) {
												CProjectile *Mine = new CProjectile;
												// т.к. мины у нас с 214-217, делаем +213
												if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr) {
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
												if (xmlDoc->GetEntryAttribute(TL, "status") != nullptr)
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
												if (!strcmp(TL->Name, "CreateTracked")) {
													CTracked *GroundObject = new CTracked;
													if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
														GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
													else {
														TL = TL->Next;
														continue;
													}

													SetID(GroundObject, TL, xmlDoc);
													if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
													if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr) {
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
														if (!strcmp(TLGroundObject->Name, "TimeSheet")) {
															// собираем новый элемент
															CTimeSheet *TimeSheet;
															TimeSheet = new CTimeSheet;
															GroundObject->AttachTimeSheet(TimeSheet);

															if (xmlDoc->GetEntryAttribute(TLGroundObject, "aimode") != nullptr) {
																TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
																TimeSheet->Time = 0.0f;
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != nullptr)
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
																TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
																TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
																TimeSheet->Fire = false;
																TimeSheet->BossFire = false;
																TimeSheet->Targeting = false;
															} else {
																TimeSheet->AI_Mode = 0;

																TimeSheet->Time = 0.0f;
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != nullptr)
																	TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
																TimeSheet->InUse = false;

																TimeSheet->Speed = 0.0f;
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "speed") != nullptr)
																	TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLGroundObject, "speed");

																TimeSheet->Acceler = 1.0f;//0-1
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "acceler") != nullptr)
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

																TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotx") != nullptr)
																	TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotx");
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "roty") != nullptr)
																	TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "roty");
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotz") != nullptr)
																	TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotz");

																TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacx") != nullptr)
																	TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacx");
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacy") != nullptr)
																	TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacy");
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacz") != nullptr)
																	TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacz");
																vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
																vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
																vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

																TimeSheet->Fire = false;
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "fire") != nullptr)
																	if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
																		TimeSheet->Fire = true;

																TimeSheet->BossFire = false;

																TimeSheet->Targeting = false;
																if (xmlDoc->GetEntryAttribute(TLGroundObject, "targeting") != nullptr)
																	if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") > 0)
																		TimeSheet->Targeting = true;
															}

														}

														// берем следующий элемент
														TLGroundObject = TLGroundObject->Next;
													}
												} else
													// CreateWheeled
													if (!strcmp(TL->Name, "CreateWheeled")) {
														CWheeled *GroundObject = new CWheeled;
														if (xmlDoc->GetEntryAttribute(TL, "type") != nullptr)
															GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
														else {
															TL = TL->Next;
															continue;
														}

														SetID(GroundObject, TL, xmlDoc);
														if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
														if (xmlDoc->GetEntryAttribute(TL, "speed") != nullptr) {
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
															if (!strcmp(TLGroundObject->Name, "TimeSheet")) {
																// собираем новый элемент
																CTimeSheet *TimeSheet;
																TimeSheet = new CTimeSheet;
																GroundObject->AttachTimeSheet(TimeSheet);

																if (xmlDoc->GetEntryAttribute(TLGroundObject, "aimode") != nullptr) {
																	TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
																	TimeSheet->Time = 0.0f;
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != nullptr)
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
																	TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
																	TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
																	TimeSheet->Fire = false;
																	TimeSheet->BossFire = false;
																	TimeSheet->Targeting = false;
																} else {
																	TimeSheet->AI_Mode = 0;

																	TimeSheet->Time = 0.0f;
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != nullptr)
																		TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
																	TimeSheet->InUse = false;

																	TimeSheet->Speed = 0.0f;
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "speed") != nullptr)
																		TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLGroundObject, "speed");


																	TimeSheet->Acceler = 1.0f;//0-1
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "acceler") != nullptr)
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

																	TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotx") != nullptr)
																		TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotx");
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "roty") != nullptr)
																		TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "roty");
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotz") != nullptr)
																		TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotz");

																	TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacx") != nullptr)
																		TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacx");
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacy") != nullptr)
																		TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacy");
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacz") != nullptr)
																		TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacz");
																	vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
																	vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
																	vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

																	TimeSheet->Fire = false;
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "fire") != nullptr)
																		if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
																			TimeSheet->Fire = true;

																	TimeSheet->BossFire = false;

																	TimeSheet->Targeting = false;
																	if (xmlDoc->GetEntryAttribute(TLGroundObject, "targeting") != nullptr)
																		if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") != 0) TimeSheet->Targeting = true;
																}

															}

															// берем следующий элемент
															TLGroundObject = TLGroundObject->Next;
														}
													} else {
														// если тут - значит не нашли директиву, или произошла ошибка
														fprintf(stderr, "ScriptEngine: tag %s not found, line %i.", TL->Name, TL->LineNumber);
													}


		// берем следующий элемент по порядку
		TL = TL->Next;
	}
}




