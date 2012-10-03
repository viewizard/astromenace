/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Script.cpp

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



void SetID(CObject3D *Object, TiXmlElement *Element);
void SetDebugInformation(CObject3D *Object, TiXmlElement *Element);
void SetShowDeleteOnHide(CObject3D *Object, TiXmlElement *Element);
void SetShipRotation(CSpaceShip *Object, TiXmlElement *Element);
void SetShipLocation(CSpaceShip *Object, TiXmlElement *Element, float TimeOpLag);
void SetProjectileRotation(CProjectile *Object, TiXmlElement *Element);
void SetProjectileLocation(CProjectile *Object, TiXmlElement *Element, float TimeOpLag);
void SetRotation(CObject3D *Object, TiXmlElement *Element);
void SetLocation(CObject3D *Object, TiXmlElement *Element, float TimeOpLag);
void SetAIMode(CObject3D *Object, TiXmlElement *Element);




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
	xmlElem = 0;
	xmlAstroMenaceScript = 0;


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
	if (xmlDoc != 0)
	{
		xmlDoc->Clear();
		delete xmlDoc; xmlDoc = 0;
	}
}






//-----------------------------------------------------------------------------
// запустить скрипт на выполнение
//-----------------------------------------------------------------------------
bool ScriptEngine::RunScript(const char *FileName, float InitTime)
{
	// установка значений
	StartTime = TimeLastOp = InitTime;

	TimeOpLag = 0;
	xmlDoc = 0;
	xmlElem = 0;
	xmlAstroMenaceScript = 0;

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


	// иним скрипт
	xmlDoc = new TiXmlDocument(FileName);
	if (!xmlDoc->LoadFile())
	{
		fprintf(stderr, "Can't find script file or file corrupted: %s\n", FileName);
		xmlDoc->Clear();
		delete xmlDoc; xmlDoc = 0;
		return false;
	}


	// берем первый элемент в скрипте
	xmlAstroMenaceScript = xmlDoc->FirstChildElement("AstroMenaceScript");
	if (xmlAstroMenaceScript != 0)
	{
		xmlElem = xmlAstroMenaceScript->FirstChildElement();
	}
	else
	{
		fprintf(stderr, "Can't find AstroMenaceScript element in the: %s\n", FileName);
		delete xmlDoc; xmlDoc = 0;
		return false;
	}


	// переходим на действия
	xmlAstroMenaceScript = xmlAstroMenaceScript->FirstChildElement("Action");
	if (xmlAstroMenaceScript != 0)
	{
		xmlElem = xmlAstroMenaceScript->FirstChildElement();
	}
	else
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
	if (xmlAstroMenaceScript == 0) return false;

	// находим дельту времени
	float TimeDelta = Time - TimeLastOp;


	// генерация астероидов
	if (AsterOn)
	{
		if (AsterLastTime == -1.0) AsterLastTime = Time;
		float TimeDelta = Time - AsterLastTime;
		AsterLastTime = Time;

		// складываем все
		float NeedGener = AsterQuant*TimeDelta+AsterRealNeed;
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
				VECTOR3D(/*-AsterW/2.0f+*/AsterW*vw_Randf0+AsterXPos, AsterYPos*2+AsterH*vw_Randf1, AsterZPos+20.0f)
				+GamePoint);
			else
				CreateAsteroid->SetLocation(
				VECTOR3D(/*-AsterW/4.0f+(AsterW/2.0f)*vw_Randf1*/AsterW*vw_Randf0+AsterXPos, AsterYPos*2+AsterH*vw_Randf1, AsterZPos)
				+GamePoint);

			NeedGenerInt--;
		}

		AsterFastCount++;
		if (AsterFastCount > 30) AsterFastCount = 0;
	}



	// если нужно, смотрим когда заканчивать миссию
	if ((EndDelayMissionComplete>0.0f) | NeedCheckSpaceShip | NeedCheckGroundObject)
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
				if ((Tmp1->ObjectStatus == 1) & (Tmp1->ShowDeleteOnHide != 0)) count++;
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
				if (NeedCheckCollision(Tmp1) & (Tmp1->ShowDeleteOnHide != 0)) count++;
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






	while (xmlElem)
	{
		// Debug
		if (!strcmp(xmlElem->Value(), "Debug"))
		{
			ShowDebugModeLine = false;
			if (xmlElem->Attribute("showline"))
				if (atoi(xmlElem->Attribute("showline")) == 1) ShowDebugModeLine = true;

			NeedShowBB = 0;
			if (xmlElem->Attribute("showbb")) NeedShowBB = atoi(xmlElem->Attribute("showbb"));

			UndeadDebugMode = false;
			if (xmlElem->Attribute("undead"))
				if (atoi(xmlElem->Attribute("undead")) == 1) UndeadDebugMode = true;

			ShowGameTime = false;
			if (xmlElem->Attribute("time"))
				if (atoi(xmlElem->Attribute("time")) == 1) ShowGameTime = true;

			goto NextDirective;
		}

		// StarSystem
		if (!strcmp(xmlElem->Value(), "StarSystem"))
		{
			if (xmlElem->Attribute("system"))
			{
				int SystemNum = atoi(xmlElem->Attribute("system"));
				VECTOR3D TmpBaseRotation(0.0f, 0.0f, 0.0f);
				if (xmlElem->Attribute("anglex")) TmpBaseRotation.x = (float)atof(xmlElem->Attribute("anglex"));
				if (xmlElem->Attribute("angley")) TmpBaseRotation.y = (float)atof(xmlElem->Attribute("angley"));
				if (xmlElem->Attribute("anglez")) TmpBaseRotation.z = (float)atof(xmlElem->Attribute("anglez"));
				StarSystemInit(SystemNum, TmpBaseRotation);
			}
			goto NextDirective;
		}


		// Music
		if (!strcmp(xmlElem->Value(), "Music"))
		{
			// если корабль игрока уничтожен - не меняем уже музыку в игре вообще,
			// должна проигрываться только музыка поражения
			if (PlayerFighter != 0)
			if (PlayerFighter->Strength > 0.0f)
			{
				if (xmlElem->Attribute("theme"))
				{
					int Theme = atoi(xmlElem->Attribute("theme"));
					if (Theme == 1)
						StartMusicWithFade(2, 2.0f, 2.0f);
					if (Theme == 2)
						StartMusicWithFade(3, 1.5f, 2.0f);
				}
			}
			goto NextDirective;
		}


		// CreatePlanet
		if (!strcmp(xmlElem->Value(), "CreatePlanet"))
		{
			CPlanet *Planet = 0;
			Planet = new CPlanet;
			if (xmlElem->Attribute("type"))
			{
				Planet->Create(atoi(xmlElem->Attribute("type")));
				SetRotation(Planet, xmlElem);
				SetLocation(Planet, xmlElem, 0.0f);
				Planet->ShowDeleteOnHide = 0;

				if (xmlElem->Attribute("speed")) Planet->Speed = (float)atof(xmlElem->Attribute("speed"));
			}
		}


		// AsteroidField
		if (!strcmp(xmlElem->Value(), "AsteroidField"))
		{
			if (xmlElem->Attribute("status"))
			{
				if (atoi(xmlElem->Attribute("status")) > 0) AsterOn = true;
				else AsterOn = false;
			}
			if (xmlElem->Attribute("persec")) AsterQuant = (float)atof(xmlElem->Attribute("persec"));
			if (xmlElem->Attribute("w")) AsterW = (float)atof(xmlElem->Attribute("w"));
			if (xmlElem->Attribute("h")) AsterH = (float)atof(xmlElem->Attribute("h"));
			if (xmlElem->Attribute("posx")) AsterXPos = (float)atof(xmlElem->Attribute("posx"));
			if (xmlElem->Attribute("posy")) AsterYPos = (float)atof(xmlElem->Attribute("posy"));
			if (xmlElem->Attribute("posz")) AsterZPos = (float)atof(xmlElem->Attribute("posz"));
			if (xmlElem->Attribute("slow")) AsterMaxSpeed = (float)atof(xmlElem->Attribute("slow"));
			if (xmlElem->Attribute("fast")) AsterMinFastSpeed = (float)atof(xmlElem->Attribute("fast"));



			goto NextDirective;
		}



		// Light
		if (!strcmp(xmlElem->Value(), "Light"))
		{
			eLight *NewLight;
			NewLight = new eLight;

			NewLight->LightType = 0; // по умолчанию, солнце
			if (xmlElem->Attribute("type")) NewLight->LightType = atoi(xmlElem->Attribute("type"));

			NewLight->Diffuse[0] = 0.0f;
			NewLight->Diffuse[1] = 0.0f;
			NewLight->Diffuse[2] = 0.0f;
			NewLight->Diffuse[3] = 1.0f;
			if (xmlElem->Attribute("diffr")) NewLight->Diffuse[0] = (float)atof(xmlElem->Attribute("diffr"));
			if (xmlElem->Attribute("diffg")) NewLight->Diffuse[1] = (float)atof(xmlElem->Attribute("diffg"));
			if (xmlElem->Attribute("diffb")) NewLight->Diffuse[2] = (float)atof(xmlElem->Attribute("diffb"));
			if (xmlElem->Attribute("diffa")) NewLight->Diffuse[3] = (float)atof(xmlElem->Attribute("diffa"));

			NewLight->Specular[0] = 0.0f;
			NewLight->Specular[1] = 0.0f;
			NewLight->Specular[2] = 0.0f;
			NewLight->Specular[3] = 1.0f;
			if (xmlElem->Attribute("specr")) NewLight->Specular[0] = (float)atof(xmlElem->Attribute("specr"));
			if (xmlElem->Attribute("specg")) NewLight->Specular[1] = (float)atof(xmlElem->Attribute("specg"));
			if (xmlElem->Attribute("specb")) NewLight->Specular[2] = (float)atof(xmlElem->Attribute("specb"));
			if (xmlElem->Attribute("speca")) NewLight->Specular[3] = (float)atof(xmlElem->Attribute("speca"));

			NewLight->Ambient[0] = 0.0f;
			NewLight->Ambient[1] = 0.0f;
			NewLight->Ambient[2] = 0.0f;
			NewLight->Ambient[3] = 1.0f;
			if (xmlElem->Attribute("ambir")) NewLight->Ambient[0] = (float)atof(xmlElem->Attribute("ambir"));
			if (xmlElem->Attribute("ambig")) NewLight->Ambient[1] = (float)atof(xmlElem->Attribute("ambig"));
			if (xmlElem->Attribute("ambib")) NewLight->Ambient[2] = (float)atof(xmlElem->Attribute("ambib"));
			if (xmlElem->Attribute("ambia")) NewLight->Ambient[3] = (float)atof(xmlElem->Attribute("ambia"));

			NewLight->Direction = VECTOR3D(0.0f,0.0f,1.0f);
			if (xmlElem->Attribute("dirx")) NewLight->Direction.x = (float)atof(xmlElem->Attribute("dirx"));
			if (xmlElem->Attribute("diry")) NewLight->Direction.y = (float)atof(xmlElem->Attribute("diry"));
			if (xmlElem->Attribute("dirz")) NewLight->Direction.z = (float)atof(xmlElem->Attribute("dirz"));
			NewLight->Direction.Normalize();

			NewLight->Location = VECTOR3D(0.0f,0.0f,0.0f);
			if (xmlElem->Attribute("posx")) NewLight->Location.x = (float)atof(xmlElem->Attribute("posx"));
			if (xmlElem->Attribute("posy")) NewLight->Location.y = (float)atof(xmlElem->Attribute("posy"));
			if (xmlElem->Attribute("posz")) NewLight->Location.z = (float)atof(xmlElem->Attribute("posz"));


			NewLight->On = true;
			if (xmlElem->Attribute("status"))
				if (atoi(xmlElem->Attribute("status")) != 1) NewLight->On = false;

			goto NextDirective;
		}

		// Label
		if (!strcmp(xmlElem->Value(), "Label"))
		{
			goto NextDirective;
		}

		// Goto
		if (!strcmp(xmlElem->Value(), "Goto"))
		{
			// если есть указатель на метку
			if (xmlElem->Attribute("label"))
			{
				// нужно перебрать все метки и остановится на нужной
				TiXmlElement *tmpElem = xmlAstroMenaceScript->FirstChildElement("Label");

				// перебор по всем меткам
				while (tmpElem)
				{
					if (tmpElem->Attribute("name"))
					if (!strcmp(tmpElem->Attribute("name"), xmlElem->Attribute("label")))
					{
						// ставим новый указатель
						xmlElem = tmpElem;
						goto NextDirective;
					}
					// берем следующую метку
					tmpElem = tmpElem->NextSiblingElement("Label");
				}
			}

			goto NextDirective;
		}




		// TimeLine
		if (!strcmp(xmlElem->Value(), "TimeLine"))
		{
			float onTime = 0.0f;
			if (xmlElem->Attribute("value"))
				onTime = (float)atof(xmlElem->Attribute("value"));

			// если еще не время выполнять... нужно уйти из процедуры
			if (onTime > TimeDelta) return true;

			// считаем лаг, чтобы правильно вычеслить положение при создании
			TimeOpLag = TimeDelta - onTime;

			// дальше смотрим, что нужно сделать...
			UpdateTimeLine();

			TimeLastOp = Time - TimeOpLag;
			TimeDelta = TimeOpLag;//Time - TimeLastOp;
			goto NextDirective;
		}


		// MissionComplete
		if (!strcmp(xmlElem->Value(), "MissionComplete"))
		{
			SetGameMissionComplete();
			goto NextDirective;
		}


		// MissionCompleteAtNoEnemy
		if (!strcmp(xmlElem->Value(), "MissionCompleteAtNoEnemy"))
		{
			NeedCheckSpaceShip = false;
			if (xmlElem->Attribute("ships")) NeedCheckSpaceShip = (bool)atoi(xmlElem->Attribute("ships"));
			NeedCheckGroundObject = false;
			if (xmlElem->Attribute("grounds")) NeedCheckGroundObject = (bool)atoi(xmlElem->Attribute("grounds"));
			EndDelayMissionComplete = 0.0f;
			if (xmlElem->Attribute("delay")) EndDelayMissionComplete = atof(xmlElem->Attribute("delay"));

			if (!(xmlElem->Attribute("delay") || xmlElem->Attribute("ships") || xmlElem->Attribute("grounds")))
			{
				// если время не выставили и нечего ждать, работаем как и с обычным завершением
				SetGameMissionComplete();
				goto NextDirective;
			}

			LastTimeMissionComplete = Time;
			return true;
		}



		// Text
		if (!strcmp(xmlElem->Value(), "Text"))
		{
			CGameLvlText *NewText;
			NewText = new CGameLvlText;


			NewText->Lifetime = -1.0f;
			if (xmlElem->Attribute("life")) NewText->Lifetime = (float)atof(xmlElem->Attribute("life"));

			if (xmlElem->Attribute("text"))
			{
				NewText->DrawText = new char[strlen(xmlElem->Attribute("text"))+1];
				strcpy(NewText->DrawText, xmlElem->Attribute("text"));
			}

			if (xmlElem->Attribute("posx")) NewText->PosX = atoi(xmlElem->Attribute("posx"));
			if (xmlElem->Attribute("posy")) NewText->PosY = atoi(xmlElem->Attribute("posy"));

			NewText->Color = 0;
			if (xmlElem->Attribute("color")) NewText->Color = atoi(xmlElem->Attribute("color"));

			goto NextDirective;
		}




		// если тут - значит не нашли директиву, или произошла ошибка
//		fprintf(stderr, "ScriptEngine: tag %s not found, line %i.", xmlElem->Value(), xmlElem->Row());

NextDirective:

		// берем следующий элемент по порядку
		xmlElem = xmlElem->NextSiblingElement();
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
	TiXmlElement *TL = xmlElem->FirstChildElement();

	while (TL)
	{

		// EarthFighter
		if (!strcmp(TL->Value(), "EarthFighter"))
		{
			CEarthSpaceFighter *Fighter = 0;
			Fighter = new CEarthSpaceFighter;
			if (TL->Attribute("type")) Fighter->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			SetID(Fighter, TL);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (TL->Attribute("speed"))
			{
				Fighter->NeedSpeed = (float)atof(TL->Attribute("speed"));
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (TL->Attribute("speedlr"))
			{
				Fighter->NeedSpeedLR = (float)atof(TL->Attribute("speedlr"));
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (TL->Attribute("speedud"))
			{
				Fighter->NeedSpeedUD = (float)atof(TL->Attribute("speedud"));
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (TL->Attribute("speedbycamfb"))
			{
				Fighter->NeedSpeedByCamFB = (float)atof(TL->Attribute("speedbycamfb"));
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (TL->Attribute("speedbycamlr"))
			{
				Fighter->NeedSpeedByCamLR = (float)atof(TL->Attribute("speedbycamlr"));
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (TL->Attribute("speedbycamud"))
			{
				Fighter->NeedSpeedByCamUD = (float)atof(TL->Attribute("speedbycamud"));
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}


			if (TL->Attribute("armour")) SetEarthSpaceFighterArmour(Fighter, atoi(TL->Attribute("armour")));
			if (TL->Attribute("weapon1")) SetEarthSpaceFighterWeapon(Fighter, 1, atoi(TL->Attribute("weapon1")));
			if (TL->Attribute("weapon2")) SetEarthSpaceFighterWeapon(Fighter, 2, atoi(TL->Attribute("weapon2")));
			if (TL->Attribute("weapon3")) SetEarthSpaceFighterWeapon(Fighter, 3, atoi(TL->Attribute("weapon3")));
			if (TL->Attribute("weapon4")) SetEarthSpaceFighterWeapon(Fighter, 4, atoi(TL->Attribute("weapon4")));
			if (TL->Attribute("weapon5")) SetEarthSpaceFighterWeapon(Fighter, 5, atoi(TL->Attribute("weapon5")));
			if (TL->Attribute("weapon6")) SetEarthSpaceFighterWeapon(Fighter, 6, atoi(TL->Attribute("weapon6")));

			SetShowDeleteOnHide(Fighter, TL);
			SetAIMode(Fighter, TL); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL);
			SetShipLocation(Fighter, TL, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			TiXmlElement *TLEarthFighter = TL->FirstChildElement();
			while (TLEarthFighter)
			{
				if (!strcmp(TLEarthFighter->Value(), "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (TLEarthFighter->Attribute("aimode"))
					{
						TimeSheet->AI_Mode = atoi(TLEarthFighter->Attribute("aimode"));
						TimeSheet->Time = 0.0f;
						if (TLEarthFighter->Attribute("time"))
							TimeSheet->Time = (float)atof(TLEarthFighter->Attribute("time"));

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
						if (TLEarthFighter->Attribute("time"))
							TimeSheet->Time = (float)atof(TLEarthFighter->Attribute("time"));
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (TLEarthFighter->Attribute("speed"))
							TimeSheet->Speed = (float)atof(TLEarthFighter->Attribute("speed"));

						TimeSheet->Acceler = 1.0f;//0-1
						if (TLEarthFighter->Attribute("acceler"))
							TimeSheet->Acceler = (float)atof(TLEarthFighter->Attribute("acceler"));
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (TLEarthFighter->Attribute("speedlr"))
							TimeSheet->SpeedLR = (float)atof(TLEarthFighter->Attribute("speedlr"));

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerlr"))
							TimeSheet->AccelerLR = (float)atof(TLEarthFighter->Attribute("accelerlr"));
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (TLEarthFighter->Attribute("speedud"))
							TimeSheet->SpeedUD = (float)atof(TLEarthFighter->Attribute("speedud"));

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerud"))
							TimeSheet->AccelerUD = (float)atof(TLEarthFighter->Attribute("accelerud"));
						Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);


						TimeSheet->SpeedByCamFB = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamfb"))
							TimeSheet->SpeedByCamFB = (float)atof(TLEarthFighter->Attribute("speedbycamfb"));

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamfb"))
							TimeSheet->AccelerByCamFB = (float)atof(TLEarthFighter->Attribute("accelerbycamfb"));
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamlr"))
							TimeSheet->SpeedByCamLR = (float)atof(TLEarthFighter->Attribute("speedbycamlr"));

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamlr"))
							TimeSheet->AccelerByCamLR = (float)atof(TLEarthFighter->Attribute("accelerbycamlr"));
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamud"))
							TimeSheet->SpeedByCamUD = (float)atof(TLEarthFighter->Attribute("speedbycamud"));

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamud"))
							TimeSheet->AccelerByCamUD = (float)atof(TLEarthFighter->Attribute("accelerbycamud"));
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);


						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (TLEarthFighter->Attribute("rotx"))
							TimeSheet->Rotation.x = (float)atof(TLEarthFighter->Attribute("rotx"));
						if (TLEarthFighter->Attribute("roty"))
							TimeSheet->Rotation.y = (float)atof(TLEarthFighter->Attribute("roty"));
						if (TLEarthFighter->Attribute("rotz"))
							TimeSheet->Rotation.z = (float)atof(TLEarthFighter->Attribute("rotz"));

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (TLEarthFighter->Attribute("rotacx"))
							TimeSheet->RotationAcceler.x = (float)atof(TLEarthFighter->Attribute("rotacx"));
						if (TLEarthFighter->Attribute("rotacy"))
							TimeSheet->RotationAcceler.y = (float)atof(TLEarthFighter->Attribute("rotacy"));
						if (TLEarthFighter->Attribute("rotacz"))
							TimeSheet->RotationAcceler.z = (float)atof(TLEarthFighter->Attribute("rotacz"));
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (TLEarthFighter->Attribute("fire"))
							if (atoi(TLEarthFighter->Attribute("fire")) > 0)
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLEarthFighter = TLEarthFighter->NextSiblingElement();
			}

			goto NextTLDirective;
		}






		// AlienFighter
		if (!strcmp(TL->Value(), "AlienFighter"))
		{
			CAlienSpaceFighter *Fighter = 0;
			Fighter = new CAlienSpaceFighter;
			if (TL->Attribute("type")) Fighter->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			SetID(Fighter, TL);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (TL->Attribute("speed"))
			{
				Fighter->NeedSpeed = (float)atof(TL->Attribute("speed"));
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (TL->Attribute("speedlr"))
			{
				Fighter->NeedSpeedLR = (float)atof(TL->Attribute("speedlr"));
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (TL->Attribute("speedud"))
			{
				Fighter->NeedSpeedUD = (float)atof(TL->Attribute("speedud"));
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (TL->Attribute("speedbycamfb"))
			{
				Fighter->NeedSpeedByCamFB = (float)atof(TL->Attribute("speedbycamfb"));
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (TL->Attribute("speedbycamlr"))
			{
				Fighter->NeedSpeedByCamLR = (float)atof(TL->Attribute("speedbycamlr"));
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (TL->Attribute("speedbycamud"))
			{
				Fighter->NeedSpeedByCamUD = (float)atof(TL->Attribute("speedbycamud"));
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}

			SetShowDeleteOnHide(Fighter, TL);
			SetAIMode(Fighter, TL); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL);
			SetShipLocation(Fighter, TL, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			TiXmlElement *TLEarthFighter = TL->FirstChildElement();
			while (TLEarthFighter)
			{
				if (!strcmp(TLEarthFighter->Value(), "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (TLEarthFighter->Attribute("aimode"))
					{
						TimeSheet->AI_Mode = atoi(TLEarthFighter->Attribute("aimode"));
						TimeSheet->Time = 0.0f;
						if (TLEarthFighter->Attribute("time"))
							TimeSheet->Time = (float)atof(TLEarthFighter->Attribute("time"));
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
						if (TLEarthFighter->Attribute("time"))
							TimeSheet->Time = (float)atof(TLEarthFighter->Attribute("time"));
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (TLEarthFighter->Attribute("speed"))
							TimeSheet->Speed = (float)atof(TLEarthFighter->Attribute("speed"));

						TimeSheet->Acceler = 1.0f;//0-1
						if (TLEarthFighter->Attribute("acceler"))
							TimeSheet->Acceler = (float)atof(TLEarthFighter->Attribute("acceler"));
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (TLEarthFighter->Attribute("speedlr"))
							TimeSheet->SpeedLR = (float)atof(TLEarthFighter->Attribute("speedlr"));

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerlr"))
							TimeSheet->AccelerLR = (float)atof(TLEarthFighter->Attribute("accelerlr"));
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (TLEarthFighter->Attribute("speedud"))
							TimeSheet->SpeedUD = (float)atof(TLEarthFighter->Attribute("speedud"));

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerud"))
							TimeSheet->AccelerUD = (float)atof(TLEarthFighter->Attribute("accelerud"));
						Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamfb"))
							TimeSheet->SpeedByCamFB = (float)atof(TLEarthFighter->Attribute("speedbycamfb"));

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamfb"))
							TimeSheet->AccelerByCamFB = (float)atof(TLEarthFighter->Attribute("accelerbycamfb"));
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamlr"))
							TimeSheet->SpeedByCamLR = (float)atof(TLEarthFighter->Attribute("speedbycamlr"));

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamlr"))
							TimeSheet->AccelerByCamLR = (float)atof(TLEarthFighter->Attribute("accelerbycamlr"));
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamud"))
							TimeSheet->SpeedByCamUD = (float)atof(TLEarthFighter->Attribute("speedbycamud"));

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamud"))
							TimeSheet->AccelerByCamUD = (float)atof(TLEarthFighter->Attribute("accelerbycamud"));
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (TLEarthFighter->Attribute("rotx"))
							TimeSheet->Rotation.x = (float)atof(TLEarthFighter->Attribute("rotx"));
						if (TLEarthFighter->Attribute("roty"))
							TimeSheet->Rotation.y = (float)atof(TLEarthFighter->Attribute("roty"));
						if (TLEarthFighter->Attribute("rotz"))
							TimeSheet->Rotation.z = (float)atof(TLEarthFighter->Attribute("rotz"));

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (TLEarthFighter->Attribute("rotacx"))
							TimeSheet->RotationAcceler.x = (float)atof(TLEarthFighter->Attribute("rotacx"));
						if (TLEarthFighter->Attribute("rotacy"))
							TimeSheet->RotationAcceler.y = (float)atof(TLEarthFighter->Attribute("rotacy"));
						if (TLEarthFighter->Attribute("rotacz"))
							TimeSheet->RotationAcceler.z = (float)atof(TLEarthFighter->Attribute("rotacz"));
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (TLEarthFighter->Attribute("fire"))
							if (atoi(TLEarthFighter->Attribute("fire")) > 0)
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLEarthFighter = TLEarthFighter->NextSiblingElement();
			}

			goto NextTLDirective;
		}



		// AlienMotherShip
		if (!strcmp(TL->Value(), "AlienMotherShip"))
		{
			CAlienSpaceMotherShip *Fighter = 0;
			Fighter = new CAlienSpaceMotherShip;
			if (TL->Attribute("type")) Fighter->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			SetID(Fighter, TL);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (TL->Attribute("speed"))
			{
				Fighter->NeedSpeed = (float)atof(TL->Attribute("speed"));
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (TL->Attribute("speedlr"))
			{
				Fighter->NeedSpeedLR = (float)atof(TL->Attribute("speedlr"));
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (TL->Attribute("speedud"))
			{
				Fighter->NeedSpeedUD = (float)atof(TL->Attribute("speedud"));
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (TL->Attribute("speedbycamfb"))
			{
				Fighter->NeedSpeedByCamFB = (float)atof(TL->Attribute("speedbycamfb"));
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (TL->Attribute("speedbycamlr"))
			{
				Fighter->NeedSpeedByCamLR = (float)atof(TL->Attribute("speedbycamlr"));
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (TL->Attribute("speedbycamud"))
			{
				Fighter->NeedSpeedByCamUD = (float)atof(TL->Attribute("speedbycamud"));
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}

			SetShowDeleteOnHide(Fighter, TL);
			SetAIMode(Fighter, TL); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL);
			SetShipLocation(Fighter, TL, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			TiXmlElement *TLEarthFighter = TL->FirstChildElement();
			while (TLEarthFighter)
			{
				if (!strcmp(TLEarthFighter->Value(), "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (TLEarthFighter->Attribute("aimode"))
					{
						TimeSheet->AI_Mode = atoi(TLEarthFighter->Attribute("aimode"));
						TimeSheet->Time = 0.0f;
						if (TLEarthFighter->Attribute("time"))
							TimeSheet->Time = (float)atof(TLEarthFighter->Attribute("time"));
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
						if (TLEarthFighter->Attribute("time"))
							TimeSheet->Time = (float)atof(TLEarthFighter->Attribute("time"));
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (TLEarthFighter->Attribute("speed"))
							TimeSheet->Speed = (float)atof(TLEarthFighter->Attribute("speed"));

						TimeSheet->Acceler = 1.0f;//0-1
						if (TLEarthFighter->Attribute("acceler"))
							TimeSheet->Acceler = (float)atof(TLEarthFighter->Attribute("acceler"));
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (TLEarthFighter->Attribute("speedlr"))
							TimeSheet->SpeedLR = (float)atof(TLEarthFighter->Attribute("speedlr"));

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerlr"))
							TimeSheet->AccelerLR = (float)atof(TLEarthFighter->Attribute("accelerlr"));
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (TLEarthFighter->Attribute("speedud"))
							TimeSheet->SpeedUD = (float)atof(TLEarthFighter->Attribute("speedud"));

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerud"))
							TimeSheet->AccelerUD = (float)atof(TLEarthFighter->Attribute("accelerud"));
						Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamfb"))
							TimeSheet->SpeedByCamFB = (float)atof(TLEarthFighter->Attribute("speedbycamfb"));

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamfb"))
							TimeSheet->AccelerByCamFB = (float)atof(TLEarthFighter->Attribute("accelerbycamfb"));
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamlr"))
							TimeSheet->SpeedByCamLR = (float)atof(TLEarthFighter->Attribute("speedbycamlr"));

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamlr"))
							TimeSheet->AccelerByCamLR = (float)atof(TLEarthFighter->Attribute("accelerbycamlr"));
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamud"))
							TimeSheet->SpeedByCamUD = (float)atof(TLEarthFighter->Attribute("speedbycamud"));

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamud"))
							TimeSheet->AccelerByCamUD = (float)atof(TLEarthFighter->Attribute("accelerbycamud"));
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (TLEarthFighter->Attribute("rotx"))
							TimeSheet->Rotation.x = (float)atof(TLEarthFighter->Attribute("rotx"));
						if (TLEarthFighter->Attribute("roty"))
							TimeSheet->Rotation.y = (float)atof(TLEarthFighter->Attribute("roty"));
						if (TLEarthFighter->Attribute("rotz"))
							TimeSheet->Rotation.z = (float)atof(TLEarthFighter->Attribute("rotz"));

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (TLEarthFighter->Attribute("rotacx"))
							TimeSheet->RotationAcceler.x = (float)atof(TLEarthFighter->Attribute("rotacx"));
						if (TLEarthFighter->Attribute("rotacy"))
							TimeSheet->RotationAcceler.y = (float)atof(TLEarthFighter->Attribute("rotacy"));
						if (TLEarthFighter->Attribute("rotacz"))
							TimeSheet->RotationAcceler.z = (float)atof(TLEarthFighter->Attribute("rotacz"));
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (TLEarthFighter->Attribute("fire"))
							if (atoi(TLEarthFighter->Attribute("fire")) > 0)
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						if (TLEarthFighter->Attribute("bossfire"))
							if (atoi(TLEarthFighter->Attribute("bossfire")) > 0)
								TimeSheet->BossFire = true;

						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLEarthFighter = TLEarthFighter->NextSiblingElement();
			}

			goto NextTLDirective;
		}

		// PirateShip
		if (!strcmp(TL->Value(), "PirateShip"))
		{
			CPirateShip *Fighter = 0;
			Fighter = new CPirateShip;
			if (TL->Attribute("type")) Fighter->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			SetID(Fighter, TL);
			if (ShowDebugModeLine) SetDebugInformation(Fighter, TL);

			if (TL->Attribute("speed"))
			{
				Fighter->NeedSpeed = (float)atof(TL->Attribute("speed"));
				Fighter->Speed = Fighter->NeedSpeed;
			}
			if (TL->Attribute("speedlr"))
			{
				Fighter->NeedSpeedLR = (float)atof(TL->Attribute("speedlr"));
				Fighter->SpeedLR = Fighter->NeedSpeedLR;
			}
			if (TL->Attribute("speedud"))
			{
				Fighter->NeedSpeedUD = (float)atof(TL->Attribute("speedud"));
				Fighter->SpeedUD = Fighter->NeedSpeedUD;
			}

			if (TL->Attribute("speedbycamfb"))
			{
				Fighter->NeedSpeedByCamFB = (float)atof(TL->Attribute("speedbycamfb"));
				Fighter->SpeedByCamFB = Fighter->NeedSpeedByCamFB;
			}
			if (TL->Attribute("speedbycamlr"))
			{
				Fighter->NeedSpeedByCamLR = (float)atof(TL->Attribute("speedbycamlr"));
				Fighter->SpeedByCamLR = Fighter->NeedSpeedByCamLR;
			}
			if (TL->Attribute("speedbycamud"))
			{
				Fighter->NeedSpeedByCamUD = (float)atof(TL->Attribute("speedbycamud"));
				Fighter->SpeedByCamUD = Fighter->NeedSpeedByCamUD;
			}

			SetShowDeleteOnHide(Fighter, TL);
			SetAIMode(Fighter, TL); // на тот случае если просто ставим и все...
			SetShipRotation(Fighter, TL);
			SetShipLocation(Fighter, TL, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			TiXmlElement *TLEarthFighter = TL->FirstChildElement();
			while (TLEarthFighter)
			{
				if (!strcmp(TLEarthFighter->Value(), "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					Fighter->AttachTimeSheet(TimeSheet);

					if (TLEarthFighter->Attribute("aimode"))
					{
						TimeSheet->AI_Mode = atoi(TLEarthFighter->Attribute("aimode"));
						TimeSheet->Time = 0.0f;
						if (TLEarthFighter->Attribute("time"))
							TimeSheet->Time = (float)atof(TLEarthFighter->Attribute("time"));
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
						if (TLEarthFighter->Attribute("time"))
							TimeSheet->Time = (float)atof(TLEarthFighter->Attribute("time"));
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (TLEarthFighter->Attribute("speed"))
							TimeSheet->Speed = (float)atof(TLEarthFighter->Attribute("speed"));

						TimeSheet->Acceler = 1.0f;//0-1
						if (TLEarthFighter->Attribute("acceler"))
							TimeSheet->Acceler = (float)atof(TLEarthFighter->Attribute("acceler"));
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (TLEarthFighter->Attribute("speedlr"))
							TimeSheet->SpeedLR = (float)atof(TLEarthFighter->Attribute("speedlr"));

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerlr"))
							TimeSheet->AccelerLR = (float)atof(TLEarthFighter->Attribute("accelerlr"));
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (TLEarthFighter->Attribute("speedud"))
							TimeSheet->SpeedUD = (float)atof(TLEarthFighter->Attribute("speedud"));

						TimeSheet->AccelerUD = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerud"))
							TimeSheet->AccelerUD = (float)atof(TLEarthFighter->Attribute("accelerud"));
						Clamp(TimeSheet->AccelerUD, 0.0f, 1.0f);

						TimeSheet->SpeedByCamFB = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamfb"))
							TimeSheet->SpeedByCamFB = (float)atof(TLEarthFighter->Attribute("speedbycamfb"));

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamfb"))
							TimeSheet->AccelerByCamFB = (float)atof(TLEarthFighter->Attribute("accelerbycamfb"));
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamlr"))
							TimeSheet->SpeedByCamLR = (float)atof(TLEarthFighter->Attribute("speedbycamlr"));

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamlr"))
							TimeSheet->AccelerByCamLR = (float)atof(TLEarthFighter->Attribute("accelerbycamlr"));
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (TLEarthFighter->Attribute("speedbycamud"))
							TimeSheet->SpeedByCamUD = (float)atof(TLEarthFighter->Attribute("speedbycamud"));

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (TLEarthFighter->Attribute("accelerbycamud"))
							TimeSheet->AccelerByCamUD = (float)atof(TLEarthFighter->Attribute("accelerbycamud"));
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);

						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (TLEarthFighter->Attribute("rotx"))
							TimeSheet->Rotation.x = (float)atof(TLEarthFighter->Attribute("rotx"));
						if (TLEarthFighter->Attribute("roty"))
							TimeSheet->Rotation.y = (float)atof(TLEarthFighter->Attribute("roty"));
						if (TLEarthFighter->Attribute("rotz"))
							TimeSheet->Rotation.z = (float)atof(TLEarthFighter->Attribute("rotz"));

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (TLEarthFighter->Attribute("rotacx"))
							TimeSheet->RotationAcceler.x = (float)atof(TLEarthFighter->Attribute("rotacx"));
						if (TLEarthFighter->Attribute("rotacy"))
							TimeSheet->RotationAcceler.y = (float)atof(TLEarthFighter->Attribute("rotacy"));
						if (TLEarthFighter->Attribute("rotacz"))
							TimeSheet->RotationAcceler.z = (float)atof(TLEarthFighter->Attribute("rotacz"));
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (TLEarthFighter->Attribute("fire"))
							if (atoi(TLEarthFighter->Attribute("fire")) > 0)
								TimeSheet->Fire = true;

						TimeSheet->BossFire = false;
						if (TLEarthFighter->Attribute("bossfire"))
							if (atoi(TLEarthFighter->Attribute("bossfire")) > 0)
								TimeSheet->BossFire = true;

						TimeSheet->Targeting = false;
					}

				}

				// берем следующий элемент
				TLEarthFighter = TLEarthFighter->NextSiblingElement();
			}

			goto NextTLDirective;
		}




		// CreateAsteroid
		if (!strcmp(TL->Value(), "CreateAsteroid"))
		{
			CAsteroid *Asteroid = 0;
			Asteroid = new CAsteroid;

			// тип сейчас не задействован, всегда ставим 1
			Asteroid->Create(1);

			SetID(Asteroid, TL);
			if (ShowDebugModeLine) SetDebugInformation(Asteroid, TL);
			if (TL->Attribute("speed")) Asteroid->Speed = (float)atof(TL->Attribute("speed"));
			SetShowDeleteOnHide(Asteroid, TL);

			SetRotation(Asteroid, TL);
			SetLocation(Asteroid, TL, TimeOpLag);

            if (TL->Attribute("rotx"))
				Asteroid->RotationSpeed.x = (float)atof(TL->Attribute("rotx"));
			if (TL->Attribute("roty"))
				Asteroid->RotationSpeed.y = (float)atof(TL->Attribute("roty"));
			if (TL->Attribute("rotz"))
				Asteroid->RotationSpeed.z = (float)atof(TL->Attribute("rotz"));

			goto NextTLDirective;
		}



		// CreateBasePart
		if (!strcmp(TL->Value(), "CreateBasePart"))
		{
			CBasePart *BasePart = 0;
			BasePart = new CBasePart;

			// тип части
			if (TL->Attribute("type")) BasePart->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			if (TL->Attribute("speed")) BasePart->Speed = (float)atof(TL->Attribute("speed"));

			SetID(BasePart, TL);
			if (ShowDebugModeLine) SetDebugInformation(BasePart, TL);
			SetShowDeleteOnHide(BasePart, TL);

			SetRotation(BasePart, TL);
			SetLocation(BasePart, TL, TimeOpLag);

			goto NextTLDirective;
		}



		// CreateBigAsteroid
		if (!strcmp(TL->Value(), "CreateBigAsteroid"))
		{
			CBigAsteroid *BigAsteroid = 0;
			BigAsteroid = new CBigAsteroid;

			// тип части
			if (TL->Attribute("type")) BigAsteroid->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			if (TL->Attribute("speed")) BigAsteroid->Speed = (float)atof(TL->Attribute("speed"));

			SetID(BigAsteroid, TL);
			if (ShowDebugModeLine) SetDebugInformation(BigAsteroid, TL);
			SetShowDeleteOnHide(BigAsteroid, TL);

			SetRotation(BigAsteroid, TL);
			SetLocation(BigAsteroid, TL, TimeOpLag);

			goto NextTLDirective;
		}


		// CreateMBuilding
		if (!strcmp(TL->Value(), "CreateMBuilding"))
		{
			CMilitaryBuilding *GroundObject = 0;
			GroundObject = new CMilitaryBuilding;

			// тип части
			if (TL->Attribute("type")) GroundObject->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			SetID(GroundObject, TL);
			if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
			SetShowDeleteOnHide(GroundObject, TL);
			SetAIMode(GroundObject, TL); // на тот случае если просто ставим и все...

			SetRotation(GroundObject, TL);
			SetLocation(GroundObject, TL, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			TiXmlElement *TLGroundObject = TL->FirstChildElement();
			while (TLGroundObject)
			{
				if (!strcmp(TLGroundObject->Value(), "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (TLGroundObject->Attribute("aimode"))
					{
						TimeSheet->AI_Mode = atoi(TLGroundObject->Attribute("aimode"));
						TimeSheet->Time = 0.0f;
						if (TLGroundObject->Attribute("time"))
							TimeSheet->Time = (float)atof(TLGroundObject->Attribute("time"));
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
						if (TLGroundObject->Attribute("time"))
							TimeSheet->Time = (float)atof(TLGroundObject->Attribute("time"));
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
						if (TLGroundObject->Attribute("fire"))
							if (atoi(TLGroundObject->Attribute("fire")) > 0)
								TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						if (TLGroundObject->Attribute("targeting"))
							if (atoi(TLGroundObject->Attribute("targeting")) != 0) TimeSheet->Targeting = true;
					}

				}

				// берем следующий элемент
				TLGroundObject = TLGroundObject->NextSiblingElement();
			}

			goto NextTLDirective;
		}



		// CreateBuilding
		if (!strcmp(TL->Value(), "CreateBuilding"))
		{
			CBuilding *GroundObject = 0;
			GroundObject = new CBuilding;
			if (TL->Attribute("type")) GroundObject->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			SetID(GroundObject, TL);
			if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
			SetShowDeleteOnHide(GroundObject, TL);

			SetRotation(GroundObject, TL);
			SetLocation(GroundObject, TL, TimeOpLag);

			goto NextTLDirective;
		}





		// CreateMine
		if (!strcmp(TL->Value(), "CreateMine"))
		{
			CProjectile *Mine = 0;
			Mine = new CProjectile;
			// т.к. мины у нас с 214-217, делаем +213
			if (TL->Attribute("type"))
			{
				int MineType = atoi(TL->Attribute("type"))+213;
				if (MineType < 214) MineType = 214;
				if (MineType > 217) MineType = 217;
				Mine->Create(MineType);
			}
			else goto NextTLDirective;

			Mine->ProjectileType = 1;

			// по умолчанию враг
			Mine->ObjectStatus = 1;
			if (TL->Attribute("status")) Mine->ObjectStatus = atoi(TL->Attribute("status"));

			// общий - пенальти, если не игрок
			float CurrentPenalty = GameNPCWeaponPenalty*1.0f;
			// если игрок или свои - ничего не надо...
			if (Mine->ObjectStatus >= 2) CurrentPenalty = 1.0f;

			Mine->DamageHull = Mine->DamageHull/CurrentPenalty;
			Mine->DamageSystems = Mine->DamageSystems/CurrentPenalty;
			Mine->SpeedStart = Mine->SpeedEnd = Mine->Speed = Mine->SpeedStart/CurrentPenalty;

			SetID(Mine, TL);
			if (ShowDebugModeLine) SetDebugInformation(Mine, TL);
			SetShowDeleteOnHide(Mine, TL);


			SetProjectileRotation(Mine, TL);
			SetProjectileLocation(Mine, TL, TimeOpLag);

			goto NextTLDirective;
		}





		// CreateTracked
		if (!strcmp(TL->Value(), "CreateTracked"))
		{
			CTracked *GroundObject = 0;
			GroundObject = new CTracked;
			if (TL->Attribute("type")) GroundObject->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			SetID(GroundObject, TL);
			if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
			if (TL->Attribute("speed"))
			{
				GroundObject->NeedSpeed = (float)atof(TL->Attribute("speed"));
				GroundObject->Speed = GroundObject->NeedSpeed;
			}

			SetShowDeleteOnHide(GroundObject, TL);
			SetAIMode(GroundObject, TL); // на тот случае если просто ставим и все...
			SetRotation(GroundObject, TL);
			SetLocation(GroundObject, TL, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			TiXmlElement *TLGroundObject = TL->FirstChildElement();
			while (TLGroundObject)
			{
				if (!strcmp(TLGroundObject->Value(), "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (TLGroundObject->Attribute("aimode"))
					{
						TimeSheet->AI_Mode = atoi(TLGroundObject->Attribute("aimode"));
						TimeSheet->Time = 0.0f;
						if (TLGroundObject->Attribute("time"))
							TimeSheet->Time = (float)atof(TLGroundObject->Attribute("time"));
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
						if (TLGroundObject->Attribute("time"))
							TimeSheet->Time = (float)atof(TLGroundObject->Attribute("time"));
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (TLGroundObject->Attribute("speed"))
							TimeSheet->Speed = (float)atof(TLGroundObject->Attribute("speed"));

						TimeSheet->Acceler = 1.0f;//0-1
						if (TLGroundObject->Attribute("acceler"))
							TimeSheet->Acceler = (float)atof(TLGroundObject->Attribute("acceler"));
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
						if (TLGroundObject->Attribute("rotx"))
							TimeSheet->Rotation.x = (float)atof(TLGroundObject->Attribute("rotx"));
						if (TLGroundObject->Attribute("roty"))
							TimeSheet->Rotation.y = (float)atof(TLGroundObject->Attribute("roty"));
						if (TLGroundObject->Attribute("rotz"))
							TimeSheet->Rotation.z = (float)atof(TLGroundObject->Attribute("rotz"));

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (TLGroundObject->Attribute("rotacx"))
							TimeSheet->RotationAcceler.x = (float)atof(TLGroundObject->Attribute("rotacx"));
						if (TLGroundObject->Attribute("rotacy"))
							TimeSheet->RotationAcceler.y = (float)atof(TLGroundObject->Attribute("rotacy"));
						if (TLGroundObject->Attribute("rotacz"))
							TimeSheet->RotationAcceler.z = (float)atof(TLGroundObject->Attribute("rotacz"));
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (TLGroundObject->Attribute("fire"))
							if (atoi(TLGroundObject->Attribute("fire")) > 0)
								TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						if (TLGroundObject->Attribute("targeting"))
							if (atoi(TLGroundObject->Attribute("targeting")) > 0)
								TimeSheet->Targeting = true;
					}

				}

				// берем следующий элемент
				TLGroundObject = TLGroundObject->NextSiblingElement();
			}

			goto NextTLDirective;
		}




		// CreateWheeled
		if (!strcmp(TL->Value(), "CreateWheeled"))
		{
			CWheeled *GroundObject = 0;
			GroundObject = new CWheeled;
			if (TL->Attribute("type")) GroundObject->Create(atoi(TL->Attribute("type")));
			else goto NextTLDirective;

			SetID(GroundObject, TL);
			if (ShowDebugModeLine) SetDebugInformation(GroundObject, TL);
			if (TL->Attribute("speed"))
			{
				GroundObject->NeedSpeed = (float)atof(TL->Attribute("speed"));
				GroundObject->Speed = GroundObject->NeedSpeed;
			}

			SetShowDeleteOnHide(GroundObject, TL);
			SetAIMode(GroundObject, TL); // на тот случае если просто ставим и все...
			SetRotation(GroundObject, TL);
			SetLocation(GroundObject, TL, TimeOpLag);

			// дальше смотрим, что нужно сделать...
			TiXmlElement *TLGroundObject = TL->FirstChildElement();
			while (TLGroundObject)
			{
				if (!strcmp(TLGroundObject->Value(), "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					GroundObject->AttachTimeSheet(TimeSheet);

					if (TLGroundObject->Attribute("aimode"))
					{
						TimeSheet->AI_Mode = atoi(TLGroundObject->Attribute("aimode"));
						TimeSheet->Time = 0.0f;
						if (TLGroundObject->Attribute("time"))
							TimeSheet->Time = (float)atof(TLGroundObject->Attribute("time"));
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
						if (TLGroundObject->Attribute("time"))
							TimeSheet->Time = (float)atof(TLGroundObject->Attribute("time"));
						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (TLGroundObject->Attribute("speed"))
							TimeSheet->Speed = (float)atof(TLGroundObject->Attribute("speed"));


						TimeSheet->Acceler = 1.0f;//0-1
						if (TLGroundObject->Attribute("acceler"))
							TimeSheet->Acceler = (float)atof(TLGroundObject->Attribute("acceler"));
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
						if (TLGroundObject->Attribute("rotx"))
							TimeSheet->Rotation.x = (float)atof(TLGroundObject->Attribute("rotx"));
						if (TLGroundObject->Attribute("roty"))
							TimeSheet->Rotation.y = (float)atof(TLGroundObject->Attribute("roty"));
						if (TLGroundObject->Attribute("rotz"))
							TimeSheet->Rotation.z = (float)atof(TLGroundObject->Attribute("rotz"));

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (TLGroundObject->Attribute("rotacx"))
							TimeSheet->RotationAcceler.x = (float)atof(TLGroundObject->Attribute("rotacx"));
						if (TLGroundObject->Attribute("rotacy"))
							TimeSheet->RotationAcceler.y = (float)atof(TLGroundObject->Attribute("rotacy"));
						if (TLGroundObject->Attribute("rotacz"))
							TimeSheet->RotationAcceler.z = (float)atof(TLGroundObject->Attribute("rotacz"));
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (TLGroundObject->Attribute("fire"))
							if (atoi(TLGroundObject->Attribute("fire")) > 0)
								TimeSheet->Fire = true;

						TimeSheet->BossFire = false;

						TimeSheet->Targeting = false;
						if (TLGroundObject->Attribute("targeting"))
							if (atoi(TLGroundObject->Attribute("targeting")) != 0) TimeSheet->Targeting = true;
					}

				}

				// берем следующий элемент
				TLGroundObject = TLGroundObject->NextSiblingElement();
			}

			goto NextTLDirective;
		}






NextTLDirective:

		// берем следующий элемент по порядку
		TL = TL->NextSiblingElement();
	}
}




