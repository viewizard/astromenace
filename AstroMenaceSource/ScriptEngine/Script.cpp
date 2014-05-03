/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2014 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "../Game.h"



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




//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
ScriptEngine::ScriptEngine()
{
	// еще нет времени
	TimeLastOp = 0;
	TimeOpLag = 0;
	StartTime = 0;
	xmlDoc = 0;
	xmlEntry = 0;


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
}






//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
ScriptEngine::~ScriptEngine()
{
	delete xmlDoc;
}






//-----------------------------------------------------------------------------
// запустить скрипт на выполнение
//-----------------------------------------------------------------------------
bool ScriptEngine::RunScript(const char *FileName, float InitTime)
{
	// установка значений
	StartTime = TimeLastOp = InitTime;

	TimeOpLag = 0;
	xmlEntry = 0;

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


	if (xmlDoc != 0)
	{
		delete xmlDoc;
		xmlDoc = 0;
	}
	xmlDoc = new cXMLDocument;

	// иним скрипт
	if (!xmlDoc->Load(FileName))
	{
		fprintf(stderr, "Can't find script file or file corrupted: %s\n", FileName);
		delete xmlDoc; xmlDoc = 0;
		return false;
	}


	// проверяем корневой элемент
	if (strcmp("AstroMenaceScript", xmlDoc->RootXMLEntry->Name))
	{
		fprintf(stderr, "Can't find AstroMenaceScript element in the: %s\n", FileName);
		delete xmlDoc; xmlDoc = 0;
		return false;
	}


	// переходим на действия
	xmlEntry  = xmlDoc->FindFirstChildEntryByName(xmlDoc->RootXMLEntry, "Action");
	if (xmlEntry == 0)
	{
		fprintf(stderr, "Can't find Action element in the: %s\n", FileName);
		delete xmlDoc; xmlDoc = 0;
		return false;
	}

	xmlEntry = xmlEntry->FirstChild;
	if (xmlEntry == 0)
	{
		fprintf(stderr, "Can't find Action element in the: %s\n", FileName);
		delete xmlDoc; xmlDoc = 0;
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
	if (xmlDoc == 0) return false;

	// находим дельту времени
	float TimeDelta = Time - TimeLastOp;


	// генерация астероидов
	if (AsterOn)
	{
		if (AsterLastTime == -1.0) AsterLastTime = Time;
		float AsterTimeDelta = Time - AsterLastTime;
		AsterLastTime = Time;

		// складываем все
		float NeedGener = AsterQuant*AsterTimeDelta+AsterRealNeed;
		// получаем целое кол-во на генерацию
		unsigned int NeedGenerInt = (unsigned int)NeedGener;
		// находим остаток... который нужно будет потом учесть
		AsterRealNeed = NeedGener - NeedGenerInt;

		while (NeedGenerInt>0)
		{
			CAsteroid *CreateAsteroid = 0;
			CreateAsteroid = new CAsteroid;
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
	if ((EndDelayMissionComplete>0.0f) || NeedCheckSpaceShip || NeedCheckGroundObject)
	{
		if (LastTimeMissionComplete == -1.0) LastTimeMissionComplete = Time;
		float TimeDeltaMissionComplete = Time - LastTimeMissionComplete;
		LastTimeMissionComplete = Time;

		// уменьшаем только тогда, когда уже никого нет... т.е. отступ это от смерти последнего
		bool NeedDecrease = true;


		// собственно сами проверки...
		if (NeedCheckSpaceShip)
		{
			// если нет врагов
			int count = 0;
			CSpaceShip *Tmp1 = StartSpaceShip;
			while (Tmp1 != 0)
			{
				// если это враг, и мы его показали (иначе он где-то там летает)
				if ((Tmp1->ObjectStatus == 1) && (Tmp1->ShowDeleteOnHide != 0)) count++;
				Tmp1 = Tmp1->Next;
			}
			if (count > 0) NeedDecrease = false;

		}
		if (NeedCheckGroundObject)
		{
			// если нет врагов, которых можем уничтожить
			int count = 0;
			CGroundObject *Tmp1 = StartGroundObject;
			while (Tmp1 != 0)
			{
				// если это враг, и мы его показали
				if (NeedCheckCollision(Tmp1) && (Tmp1->ShowDeleteOnHide != 0)) count++;
				Tmp1 = Tmp1->Next;
			}
			if (count > 0) NeedDecrease = false;
		}


		if (NeedDecrease) EndDelayMissionComplete -= TimeDeltaMissionComplete;
		if (EndDelayMissionComplete<=0.0f)
		{
			EndDelayMissionComplete = 0.0f;
			NeedCheckSpaceShip = false;
			NeedCheckGroundObject = false;
			SetGameMissionComplete();
			return false;
		}

		// говорим, что скрипт еще не закончился!
		return true;
	}






	while (xmlEntry)
	{
		// TimeLine
		if (!strcmp(xmlEntry->Name, "TimeLine"))
		{
			float onTime = 0.0f;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "value") != 0)
				onTime = xmlDoc->fGetEntryAttribute(xmlEntry, "value");

			// если еще не время выполнять... нужно уйти из процедуры
			if (onTime > TimeDelta) return true;

			// считаем лаг, чтобы правильно вычеслить положение при создании
			TimeOpLag = TimeDelta - onTime;

			// дальше смотрим, что нужно сделать...
			UpdateTimeLine();

			TimeLastOp = Time - TimeOpLag;
			TimeDelta = TimeOpLag;//Time - TimeLastOp;
		}
		else
		// Debug
		if (!strcmp(xmlEntry->Name, "Debug"))
		{
			ShowDebugModeLine = false;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "showline") != 0)
				ShowDebugModeLine = xmlDoc->bGetEntryAttribute(xmlEntry, "showline");

			NeedShowBB = 0;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "showbb") != 0)
				NeedShowBB = xmlDoc->iGetEntryAttribute(xmlEntry, "showbb");

			UndeadDebugMode = false;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "undead") != 0)
				UndeadDebugMode = xmlDoc->bGetEntryAttribute(xmlEntry, "undead");

			ShowGameTime = false;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "time") != 0)
				ShowGameTime = xmlDoc->bGetEntryAttribute(xmlEntry, "time");

		}
		else
		// StarSystem
		if (!strcmp(xmlEntry->Name, "StarSystem"))
		{
			if (xmlDoc->GetEntryAttribute(xmlEntry, "system") != 0)
			{
				int SystemNum = xmlDoc->iGetEntryAttribute(xmlEntry, "system");
				VECTOR3D TmpBaseRotation(0.0f, 0.0f, 0.0f);
				if (xmlDoc->GetEntryAttribute(xmlEntry, "anglex") != 0) TmpBaseRotation.x = xmlDoc->fGetEntryAttribute(xmlEntry, "anglex");
				if (xmlDoc->GetEntryAttribute(xmlEntry, "angley") != 0) TmpBaseRotation.y = xmlDoc->fGetEntryAttribute(xmlEntry, "angley");
				if (xmlDoc->GetEntryAttribute(xmlEntry, "anglez") != 0) TmpBaseRotation.z = xmlDoc->fGetEntryAttribute(xmlEntry, "anglez");
				StarSystemInit(SystemNum, TmpBaseRotation);
			}
		}
		else
		// Music
		if (!strcmp(xmlEntry->Name, "Music"))
		{
			// если корабль игрока уничтожен - не меняем уже музыку в игре вообще,
			// должна проигрываться только музыка поражения
			if (PlayerFighter != 0)
			if (PlayerFighter->Strength > 0.0f)
			{
				if (xmlDoc->GetEntryAttribute(xmlEntry, "theme") != 0)
				{
					int Theme = xmlDoc->iGetEntryAttribute(xmlEntry, "theme");
					if (Theme == 1)
						StartMusicWithFade(2, 2.0f, 2.0f);
					if (Theme == 2)
						StartMusicWithFade(3, 1.5f, 2.0f);
				}
			}
		}
		else
		// CreatePlanet
		if (!strcmp(xmlEntry->Name, "CreatePlanet"))
		{
			CPlanet *Planet = 0;
			Planet = new CPlanet;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "type") != 0)
			{
				Planet->Create(xmlDoc->iGetEntryAttribute(xmlEntry, "type"));
				SetRotation(Planet, xmlEntry, xmlDoc);
				SetLocation(Planet, xmlEntry, xmlDoc, 0.0f);
				Planet->ShowDeleteOnHide = 0;

				if (xmlDoc->GetEntryAttribute(xmlEntry, "speed") != 0) Planet->Speed = xmlDoc->fGetEntryAttribute(xmlEntry, "speed");
			}
		}
		else
		// AsteroidField
		if (!strcmp(xmlEntry->Name, "AsteroidField"))
		{
			if (xmlDoc->GetEntryAttribute(xmlEntry, "status") != 0)
			{
				AsterOn = xmlDoc->bGetEntryAttribute(xmlEntry, "status");
			}
			if (xmlDoc->GetEntryAttribute(xmlEntry, "persec") != 0) AsterQuant = xmlDoc->fGetEntryAttribute(xmlEntry, "persec");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "w") != 0) AsterW = xmlDoc->fGetEntryAttribute(xmlEntry, "w");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "h") != 0) AsterH = xmlDoc->fGetEntryAttribute(xmlEntry, "h");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != 0) AsterXPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posx");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != 0) AsterYPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posy");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "posz") != 0) AsterZPos = xmlDoc->fGetEntryAttribute(xmlEntry, "posz");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "slow") != 0) AsterMaxSpeed = xmlDoc->fGetEntryAttribute(xmlEntry, "slow");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "fast") != 0) AsterMinFastSpeed = xmlDoc->fGetEntryAttribute(xmlEntry, "fast");
		}
		else
		// Light
		if (!strcmp(xmlEntry->Name, "Light"))
		{
			eLight *NewLight;
			NewLight = new eLight;

			NewLight->LightType = 0; // по умолчанию, солнце
			if (xmlDoc->GetEntryAttribute(xmlEntry, "type") != 0) NewLight->LightType = xmlDoc->iGetEntryAttribute(xmlEntry, "type");

			NewLight->Diffuse[0] = 0.0f;
			NewLight->Diffuse[1] = 0.0f;
			NewLight->Diffuse[2] = 0.0f;
			NewLight->Diffuse[3] = 1.0f;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "diffr") != 0) NewLight->Diffuse[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffr");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "diffg") != 0) NewLight->Diffuse[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffg");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "diffb") != 0) NewLight->Diffuse[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffb");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "diffa") != 0) NewLight->Diffuse[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "diffa");

			NewLight->Specular[0] = 0.0f;
			NewLight->Specular[1] = 0.0f;
			NewLight->Specular[2] = 0.0f;
			NewLight->Specular[3] = 1.0f;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "specr") != 0) NewLight->Specular[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "specr");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "specg") != 0) NewLight->Specular[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "specg");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "specb") != 0) NewLight->Specular[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "specb");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "speca") != 0) NewLight->Specular[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "speca");

			NewLight->Ambient[0] = 0.0f;
			NewLight->Ambient[1] = 0.0f;
			NewLight->Ambient[2] = 0.0f;
			NewLight->Ambient[3] = 1.0f;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "ambir") != 0) NewLight->Ambient[0] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambir");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "ambig") != 0) NewLight->Ambient[1] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambig");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "ambib") != 0) NewLight->Ambient[2] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambib");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "ambia") != 0) NewLight->Ambient[3] = xmlDoc->fGetEntryAttribute(xmlEntry, "ambia");

			NewLight->Direction = VECTOR3D(0.0f,0.0f,1.0f);
			if (xmlDoc->GetEntryAttribute(xmlEntry, "dirx") != 0) NewLight->Direction.x = xmlDoc->fGetEntryAttribute(xmlEntry, "dirx");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "diry") != 0) NewLight->Direction.y = xmlDoc->fGetEntryAttribute(xmlEntry, "diry");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "dirz") != 0) NewLight->Direction.z = xmlDoc->fGetEntryAttribute(xmlEntry, "dirz");
			NewLight->Direction.Normalize();

			NewLight->Location = VECTOR3D(0.0f,0.0f,0.0f);
			if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != 0) NewLight->Location.x = xmlDoc->fGetEntryAttribute(xmlEntry, "posx");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != 0) NewLight->Location.y = xmlDoc->fGetEntryAttribute(xmlEntry, "posy");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "posz") != 0) NewLight->Location.z = xmlDoc->fGetEntryAttribute(xmlEntry, "posz");


			NewLight->On = true;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "status") != 0)
				NewLight->On = xmlDoc->bGetEntryAttribute(xmlEntry, "status");
		}
		else
		// Label
		if (!strcmp(xmlEntry->Name, "Label"))
		{
			// ничего не делаем
			xmlEntry = xmlEntry->Next;
			continue;
		}
		else
		// Goto
		if (!strcmp(xmlEntry->Name, "Goto"))
		{
			// если есть указатель на метку
			if (xmlDoc->GetEntryAttribute(xmlEntry, "label") != 0)
			{
				// нужно перебрать все метки и остановится на нужной
				cXMLEntry *tmpEntry = xmlDoc->FindFirstChildEntryByName(xmlDoc->FindFirstChildEntryByName(xmlDoc->RootXMLEntry, "Action"), "Label");

				// перебор по всем меткам
				while (tmpEntry)
				{
				//	if (!strcmp(tmpEntry->Name, "Label"))
					if (xmlDoc->GetEntryAttribute(tmpEntry, "name") != 0)
					if (!strcmp(xmlDoc->GetEntryAttribute(tmpEntry, "name"), xmlDoc->GetEntryAttribute(xmlEntry, "label")))
					{
						// ставим новый указатель
						xmlEntry = tmpEntry;
						return true;
					}
					// берем следующую метку
					tmpEntry = tmpEntry->Next;
				}
			}
		}
		else
		// MissionComplete
		if (!strcmp(xmlEntry->Name, "MissionComplete"))
		{
			SetGameMissionComplete();
		}
		else
		// MissionCompleteAtNoEnemy
		if (!strcmp(xmlEntry->Name, "MissionCompleteAtNoEnemy"))
		{
			bool SetGameMissionFlag = false;
			NeedCheckSpaceShip = false;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "ships") != 0) {NeedCheckSpaceShip = xmlDoc->bGetEntryAttribute(xmlEntry, "ships");SetGameMissionFlag = true;};
			NeedCheckGroundObject = false;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "grounds") != 0) {NeedCheckGroundObject = xmlDoc->bGetEntryAttribute(xmlEntry, "grounds");SetGameMissionFlag = true;};
			EndDelayMissionComplete = 0.0f;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "delay") != 0) {EndDelayMissionComplete = xmlDoc->fGetEntryAttribute(xmlEntry, "delay");SetGameMissionFlag = true;};

			if (!SetGameMissionFlag)
			{
				// если время не выставили и нечего ждать, работаем как и с обычным завершением
				SetGameMissionComplete();
			}
			else
			{
				LastTimeMissionComplete = Time;
				return true;
			}
		}
		else
		// Text
		if (!strcmp(xmlEntry->Name, "Text"))
		{
			CGameLvlText *NewText;
			NewText = new CGameLvlText;


			NewText->Lifetime = -1.0f;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "life") != 0) NewText->Lifetime = xmlDoc->fGetEntryAttribute(xmlEntry, "life");

			if (xmlDoc->GetEntryAttribute(xmlEntry, "text") != 0)
			{
				NewText->DrawText = new char[strlen(xmlDoc->GetEntryAttribute(xmlEntry, "text"))+1];
				strcpy(NewText->DrawText, xmlDoc->GetEntryAttribute(xmlEntry, "text"));
			}

			if (xmlDoc->GetEntryAttribute(xmlEntry, "posx") != 0) NewText->PosX = xmlDoc->iGetEntryAttribute(xmlEntry, "posx");
			if (xmlDoc->GetEntryAttribute(xmlEntry, "posy") != 0) NewText->PosY = xmlDoc->iGetEntryAttribute(xmlEntry, "posy");

			NewText->Color = 0;
			if (xmlDoc->GetEntryAttribute(xmlEntry, "color") != 0) NewText->Color = xmlDoc->iGetEntryAttribute(xmlEntry, "color");
		}
		else
		{
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

	while (TL)
	{

		// EarthFighter
		if (!strcmp(TL->Name, "EarthFighter"))
		{
			CEarthSpaceFighter *Fighter = 0;
			Fighter = new CEarthSpaceFighter;
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			SetID(Fighter, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0)
			{
				Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedlr") != 0)
			{
				Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedud") != 0)
			{
				Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (xmlDoc->GetEntryAttribute(TL, "speedbycamfb") != 0)
			{
				Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamlr") != 0)
			{
				Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamud") != 0)
			{
				Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}


			if (xmlDoc->GetEntryAttribute(TL, "armour") != 0) SetEarthSpaceFighterArmour(Fighter, xmlDoc->iGetEntryAttribute(TL, "armour"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon1") != 0) SetEarthSpaceFighterWeapon(Fighter, 1, xmlDoc->iGetEntryAttribute(TL, "weapon1"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon2") != 0) SetEarthSpaceFighterWeapon(Fighter, 2, xmlDoc->iGetEntryAttribute(TL, "weapon2"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon3") != 0) SetEarthSpaceFighterWeapon(Fighter, 3, xmlDoc->iGetEntryAttribute(TL, "weapon3"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon4") != 0) SetEarthSpaceFighterWeapon(Fighter, 4, xmlDoc->iGetEntryAttribute(TL, "weapon4"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon5") != 0) SetEarthSpaceFighterWeapon(Fighter, 5, xmlDoc->iGetEntryAttribute(TL, "weapon5"));
			if (xmlDoc->GetEntryAttribute(TL, "weapon6") != 0) SetEarthSpaceFighterWeapon(Fighter, 6, xmlDoc->iGetEntryAttribute(TL, "weapon6"));

			SetShowDeleteOnHide(Fighter, TL, xmlDoc);
			SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL, xmlDoc);
			SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			cXMLEntry *TLEarthFighter = TL->FirstChild;
			while (TLEarthFighter)
			{
				if (!strcmp(TLEarthFighter->Name, "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->GetEntryAttribute(TLEarthFighter, "aimode") != 0)
					{
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLEarthFighter, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "time") != 0)
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
					}
					else
					{
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "time") != 0)
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLEarthFighter, "time");
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speed") != 0)
							TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speed");

						TimeSheet->Acceler = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "acceler") != 0)
							TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLEarthFighter, "acceler");
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedlr") != 0)
							TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedlr");

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerlr") != 0)
							TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerlr");
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedud") != 0)
							TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedud");

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerud") != 0)
							TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerud");
						Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);


						TimeSheet->SpeedByCamFB = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedbycamfb") != 0)
							TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamfb");

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerbycamfb") != 0)
							TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamfb");
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedbycamlr") != 0)
							TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamlr");

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerbycamlr") != 0)
							TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamlr");
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "speedbycamud") != 0)
							TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "speedbycamud");

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "accelerbycamud") != 0)
							TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLEarthFighter, "accelerbycamud");
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);


						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotx") != 0)
							TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotx");
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "roty") != 0)
							TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLEarthFighter, "roty");
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotz") != 0)
							TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotz");

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotacx") != 0)
							TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacx");
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotacy") != 0)
							TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacy");
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "rotacz") != 0)
							TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLEarthFighter, "rotacz");
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (xmlDoc->GetEntryAttribute(TLEarthFighter, "fire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLEarthFighter, "fire") > 0)
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLEarthFighter = TLEarthFighter->Next;
			}
		}
		else
		// AlienFighter
		if (!strcmp(TL->Name, "AlienFighter"))
		{
			CAlienSpaceFighter *Fighter = 0;
			Fighter = new CAlienSpaceFighter;
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			SetID(Fighter, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0)
			{
				Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedlr") != 0)
			{
				Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedud") != 0)
			{
				Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (xmlDoc->GetEntryAttribute(TL, "speedbycamfb") != 0)
			{
				Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamlr") != 0)
			{
				Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamud") != 0)
			{
				Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}

			SetShowDeleteOnHide(Fighter, TL, xmlDoc);
			SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL, xmlDoc);
			SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			cXMLEntry *TLAlienFighter = TL->FirstChild;
			while (TLAlienFighter)
			{
				if (!strcmp(TLAlienFighter->Name, "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->GetEntryAttribute(TLAlienFighter, "aimode") != 0)
					{
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLAlienFighter, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "time") != 0)
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
					}
					else
					{
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "time") != 0)
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLAlienFighter, "time");
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speed") != 0)
							TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speed");

						TimeSheet->Acceler = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "acceler") != 0)
							TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLAlienFighter, "acceler");
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedlr") != 0)
							TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedlr");

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerlr") != 0)
							TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerlr");
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedud") != 0)
							TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedud");

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerud") != 0)
							TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerud");
						Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedbycamfb") != 0)
							TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamfb");

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerbycamfb") != 0)
							TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamfb");
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedbycamlr") != 0)
							TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamlr");

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerbycamlr") != 0)
							TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamlr");
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "speedbycamud") != 0)
							TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "speedbycamud");

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "accelerbycamud") != 0)
							TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLAlienFighter, "accelerbycamud");
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotx") != 0)
							TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotx");
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "roty") != 0)
							TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLAlienFighter, "roty");
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotz") != 0)
							TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotz");

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotacx") != 0)
							TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacx");
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotacy") != 0)
							TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacy");
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "rotacz") != 0)
							TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLAlienFighter, "rotacz");
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (xmlDoc->GetEntryAttribute(TLAlienFighter, "fire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLAlienFighter, "fire") > 0)
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLAlienFighter = TLAlienFighter->Next;
			}
		}
		else
		// AlienMotherShip
		if (!strcmp(TL->Name, "AlienMotherShip"))
		{
			CAlienSpaceMotherShip *Fighter = 0;
			Fighter = new CAlienSpaceMotherShip;
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			SetID(Fighter, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0)
			{
				Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedlr") != 0)
			{
				Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedud") != 0)
			{
				Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (xmlDoc->GetEntryAttribute(TL, "speedbycamfb") != 0)
			{
				Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamlr") != 0)
			{
				Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamud") != 0)
			{
				Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}

			SetShowDeleteOnHide(Fighter, TL, xmlDoc);
			SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL, xmlDoc);
			SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			cXMLEntry *TLAlienMotherShip = TL->FirstChild;
			while (TLAlienMotherShip)
			{
				if (!strcmp(TLAlienMotherShip->Name, "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "aimode") != 0)
					{
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "time") != 0)
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
					}
					else
					{
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "time") != 0)
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "time");
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speed") != 0)
							TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speed");

						TimeSheet->Acceler = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "acceler") != 0)
							TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "acceler");
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedlr") != 0)
							TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedlr");

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerlr") != 0)
							TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerlr");
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedud") != 0)
							TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedud");

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerud") != 0)
							TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerud");
						Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedbycamfb") != 0)
							TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamfb");

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerbycamfb") != 0)
							TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamfb");
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedbycamlr") != 0)
							TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamlr");

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerbycamlr") != 0)
							TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamlr");
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "speedbycamud") != 0)
							TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "speedbycamud");

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "accelerbycamud") != 0)
							TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "accelerbycamud");
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotx") != 0)
							TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotx");
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "roty") != 0)
							TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "roty");
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotz") != 0)
							TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotz");

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotacx") != 0)
							TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacx");
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotacy") != 0)
							TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacy");
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "rotacz") != 0)
							TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLAlienMotherShip, "rotacz");
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "fire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "fire") > 0)
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						if (xmlDoc->GetEntryAttribute(TLAlienMotherShip, "bossfire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLAlienMotherShip, "bossfire") > 0)
								TimeSheet->BossFire = true;

						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLAlienMotherShip = TLAlienMotherShip->Next;
			}
		}
		else
		// PirateShip
		if (!strcmp(TL->Name, "PirateShip"))
		{
			CPirateShip *Fighter = 0;
			Fighter = new CPirateShip;
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) Fighter->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			SetID(Fighter, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0)
			{
				Fighter->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedlr") != 0)
			{
				Fighter->NeedSpeedLR = xmlDoc->fGetEntryAttribute(TL, "speedlr");
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedud") != 0)
			{
				Fighter->NeedSpeedUD = xmlDoc->fGetEntryAttribute(TL, "speedud");
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (xmlDoc->GetEntryAttribute(TL, "speedbycamfb") != 0)
			{
				Fighter->NeedSpeedByCamFB = xmlDoc->fGetEntryAttribute(TL, "speedbycamfb");
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamlr") != 0)
			{
				Fighter->NeedSpeedByCamLR = xmlDoc->fGetEntryAttribute(TL, "speedbycamlr");
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (xmlDoc->GetEntryAttribute(TL, "speedbycamud") != 0)
			{
				Fighter->NeedSpeedByCamUD = xmlDoc->fGetEntryAttribute(TL, "speedbycamud");
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}

			SetShowDeleteOnHide(Fighter, TL, xmlDoc);
			SetAIMode(Fighter, TL, xmlDoc); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL, xmlDoc);
			SetShipLocation(Fighter, TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			cXMLEntry *TLPirateShip = TL->FirstChild;
			while (TLPirateShip)
			{
				if (!strcmp(TLPirateShip->Name, "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (xmlDoc->GetEntryAttribute(TLPirateShip, "aimode") != 0)
					{
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLPirateShip, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "time") != 0)
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
					}
					else
					{
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "time") != 0)
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLPirateShip, "time");
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "speed") != 0)
							TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLPirateShip, "speed");

						TimeSheet->Acceler = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "acceler") != 0)
							TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLPirateShip, "acceler");
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedlr") != 0)
							TimeSheet->SpeedLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedlr");

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerlr") != 0)
							TimeSheet->AccelerLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerlr");
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedud") != 0)
							TimeSheet->SpeedUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedud");

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerud") != 0)
							TimeSheet->AccelerUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerud");
						Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedbycamfb") != 0)
							TimeSheet->SpeedByCamFB = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamfb");

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerbycamfb") != 0)
							TimeSheet->AccelerByCamFB = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamfb");
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedbycamlr") != 0)
							TimeSheet->SpeedByCamLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamlr");

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerbycamlr") != 0)
							TimeSheet->AccelerByCamLR = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamlr");
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "speedbycamud") != 0)
							TimeSheet->SpeedByCamUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "speedbycamud");

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "accelerbycamud") != 0)
							TimeSheet->AccelerByCamUD = xmlDoc->fGetEntryAttribute(TLPirateShip, "accelerbycamud");
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotx") != 0)
							TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotx");
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "roty") != 0)
							TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLPirateShip, "roty");
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotz") != 0)
							TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotz");

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotacx") != 0)
							TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacx");
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotacy") != 0)
							TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacy");
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "rotacz") != 0)
							TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLPirateShip, "rotacz");
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "fire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLPirateShip, "fire") > 0)
								TimeSheet->Fire = true;

						TimeSheet->BossFire = false;
						if (xmlDoc->GetEntryAttribute(TLPirateShip, "bossfire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLPirateShip, "bossfire") > 0)
								TimeSheet->BossFire = true;

						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLPirateShip = TLPirateShip->Next;
			}
		}
		else
		// CreateAsteroid
		if (!strcmp(TL->Name, "CreateAsteroid"))
		{
			CAsteroid *Asteroid = 0;
			Asteroid = new CAsteroid;

			// тип сейчас не задействован, всегда ставим 1
			Asteroid->Create(1);

			SetID(Asteroid, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(Asteroid, TL);
			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0) Asteroid->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");
			SetShowDeleteOnHide(Asteroid, TL, xmlDoc);

			SetRotation(Asteroid, TL, xmlDoc);
			SetLocation(Asteroid, TL, xmlDoc, TimeOpLag);

            if (xmlDoc->GetEntryAttribute(TL, "rotx") != 0)
				Asteroid->RotationSpeed.x = xmlDoc->fGetEntryAttribute(TL, "rotx");
			if (xmlDoc->GetEntryAttribute(TL, "roty") != 0)
				Asteroid->RotationSpeed.y = xmlDoc->fGetEntryAttribute(TL, "roty");
			if (xmlDoc->GetEntryAttribute(TL, "rotz") != 0)
				Asteroid->RotationSpeed.z = xmlDoc->fGetEntryAttribute(TL, "rotz");
		}
		else
		// CreateBasePart
		if (!strcmp(TL->Name, "CreateBasePart"))
		{
			CBasePart *BasePart = 0;
			BasePart = new CBasePart;

			// тип части
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) BasePart->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0) BasePart->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");

			SetID(BasePart, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(BasePart, TL);
			SetShowDeleteOnHide(BasePart, TL, xmlDoc);

			SetRotation(BasePart, TL, xmlDoc);
			SetLocation(BasePart, TL, xmlDoc, TimeOpLag);
		}
		else
		// CreateBigAsteroid
		if (!strcmp(TL->Name, "CreateBigAsteroid"))
		{
			CBigAsteroid *BigAsteroid = 0;
			BigAsteroid = new CBigAsteroid;

			// тип части
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) BigAsteroid->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0) BigAsteroid->Speed = xmlDoc->fGetEntryAttribute(TL, "speed");

			SetID(BigAsteroid, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(BigAsteroid, TL);
			SetShowDeleteOnHide(BigAsteroid, TL, xmlDoc);

			SetRotation(BigAsteroid, TL, xmlDoc);
			SetLocation(BigAsteroid, TL, xmlDoc, TimeOpLag);
		}
		else
		// CreateMBuilding
		if (!strcmp(TL->Name, "CreateMBuilding"))
		{
			CMilitaryBuilding *GroundObject = 0;
			GroundObject = new CMilitaryBuilding;

			// тип части
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
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
			while (TLGroundObject)
			{
				if (!strcmp(TLGroundObject->Name, "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (xmlDoc->GetEntryAttribute(TLGroundObject, "aimode") != 0)
					{
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != 0)
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
					}
					else
					{
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != 0)
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
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "fire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
								TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "targeting") != 0)
							if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") != 0) TimeSheet->Targeting = true;
					}

				}

				// берем следующий элемент
				TLGroundObject = TLGroundObject->Next;
			}
		}
		else
		// CreateBuilding
		if (!strcmp(TL->Name, "CreateBuilding"))
		{
			CBuilding *GroundObject = 0;
			GroundObject = new CBuilding;
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			SetID(GroundObject, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
			SetShowDeleteOnHide(GroundObject, TL, xmlDoc);

			SetRotation(GroundObject, TL, xmlDoc);
			SetLocation(GroundObject, TL, xmlDoc, TimeOpLag);
		}
		else
		// CreateMine
		if (!strcmp(TL->Name, "CreateMine"))
		{
			CProjectile *Mine = 0;
			Mine = new CProjectile;
			// т.к. мины у нас с 214-217, делаем +213
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0)
			{
				int MineType = xmlDoc->iGetEntryAttribute(TL, "type")+213;
				if (MineType < 214) MineType = 214;
				if (MineType > 217) MineType = 217;
				Mine->Create(MineType);
			}
			else
			{
				TL = TL->Next;
				continue;
			}

			Mine->ProjectileType = 1;

			// по умолчанию враг
			Mine->ObjectStatus = 1;
			if (xmlDoc->GetEntryAttribute(TL, "status") != 0) Mine->ObjectStatus = xmlDoc->iGetEntryAttribute(TL, "status");

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
		}
		else
		// CreateTracked
		if (!strcmp(TL->Name, "CreateTracked"))
		{
			CTracked *GroundObject = 0;
			GroundObject = new CTracked;
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			SetID(GroundObject, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0)
			{
				GroundObject->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
				GroundObject->Speed = GroundObject->NeedSpeed;
			}

			SetShowDeleteOnHide(GroundObject, TL, xmlDoc);
			SetAIMode(GroundObject, TL, xmlDoc); // на тот случае если просто ставим и все...
			SetRotation(GroundObject, TL, xmlDoc);
			SetLocation(GroundObject, TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			cXMLEntry *TLGroundObject = TL->FirstChild;
			while (TLGroundObject)
			{
				if (!strcmp(TLGroundObject->Name, "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (xmlDoc->GetEntryAttribute(TLGroundObject, "aimode") != 0)
					{
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != 0)
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
					}
					else
					{
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != 0)
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "speed") != 0)
							TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLGroundObject, "speed");

						TimeSheet->Acceler = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "acceler") != 0)
							TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLGroundObject, "acceler");
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

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
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotx") != 0)
							TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotx");
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "roty") != 0)
							TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "roty");
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotz") != 0)
							TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotz");

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacx") != 0)
							TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacx");
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacy") != 0)
							TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacy");
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacz") != 0)
							TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacz");
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "fire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
								TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "targeting") != 0)
							if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") > 0)
								TimeSheet->Targeting = true;
					}

				}

				// берем следующий элемент
				TLGroundObject = TLGroundObject->Next;
			}
		}
		else
		// CreateWheeled
		if (!strcmp(TL->Name, "CreateWheeled"))
		{
			CWheeled *GroundObject = 0;
			GroundObject = new CWheeled;
			if (xmlDoc->GetEntryAttribute(TL, "type") != 0) GroundObject->Create(xmlDoc->iGetEntryAttribute(TL, "type"));
			else
			{
				TL = TL->Next;
				continue;
			}

			SetID(GroundObject, TL, xmlDoc);
			if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
			if (xmlDoc->GetEntryAttribute(TL, "speed") != 0)
			{
				GroundObject->NeedSpeed = xmlDoc->fGetEntryAttribute(TL, "speed");
				GroundObject->Speed = GroundObject->NeedSpeed;
			}

			SetShowDeleteOnHide(GroundObject, TL, xmlDoc);
			SetAIMode(GroundObject, TL, xmlDoc); // на тот случае если просто ставим и все...
			SetRotation(GroundObject, TL, xmlDoc);
			SetLocation(GroundObject, TL, xmlDoc, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			cXMLEntry *TLGroundObject = TL->FirstChild;
			while (TLGroundObject)
			{
				if (!strcmp(TLGroundObject->Name, "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (xmlDoc->GetEntryAttribute(TLGroundObject, "aimode") != 0)
					{
						TimeSheet->AI_Mode = xmlDoc->iGetEntryAttribute(TLGroundObject, "aimode");
						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != 0)
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
					}
					else
					{
						TimeSheet->AI_Mode = 0;

						TimeSheet->Time = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "time") != 0)
							TimeSheet->Time = xmlDoc->fGetEntryAttribute(TLGroundObject, "time");
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "speed") != 0)
							TimeSheet->Speed = xmlDoc->fGetEntryAttribute(TLGroundObject, "speed");


						TimeSheet->Acceler = 1.0f;//0-1
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "acceler") != 0)
							TimeSheet->Acceler = xmlDoc->fGetEntryAttribute(TLGroundObject, "acceler");
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

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
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotx") != 0)
							TimeSheet->Rotation.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotx");
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "roty") != 0)
							TimeSheet->Rotation.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "roty");
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotz") != 0)
							TimeSheet->Rotation.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotz");

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacx") != 0)
							TimeSheet->RotationAcceler.x = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacx");
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacy") != 0)
							TimeSheet->RotationAcceler.y = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacy");
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "rotacz") != 0)
							TimeSheet->RotationAcceler.z = xmlDoc->fGetEntryAttribute(TLGroundObject, "rotacz");
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "fire") != 0)
							if (xmlDoc->iGetEntryAttribute(TLGroundObject, "fire") > 0)
								TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						if (xmlDoc->GetEntryAttribute(TLGroundObject, "targeting") != 0)
							if (xmlDoc->iGetEntryAttribute(TLGroundObject, "targeting") != 0) TimeSheet->Targeting = true;
					}

				}

				// берем следующий элемент
				TLGroundObject = TLGroundObject->Next;
			}
		}
		else
		{
			// если тут - значит не нашли директиву, или произошла ошибка
			fprintf(stderr, "ScriptEngine: tag %s not found, line %i.", TL->Name, TL->LineNumber);
		}


		// берем следующий элемент по порядку
		TL = TL->Next;
	}
}




