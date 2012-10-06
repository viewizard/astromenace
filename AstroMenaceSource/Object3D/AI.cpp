/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: AI.cpp

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


/// подключаем нужные файлы
#include "../Game.h"




// основной документ
TiXmlDocument	xmlAI;
// основной элемент
TiXmlElement	*xmlAIText = 0;
// буфер с текстом нашего хмл
char 			*xmlAIBuffer = 0;


//-----------------------------------------------------------------------------
// освобождаем данные
//-----------------------------------------------------------------------------
void ReleaseGameAI()
{
	if (xmlAIBuffer != 0)
	{
		delete xmlAIBuffer; xmlAIBuffer = 0;
	}
	xmlAI.Clear();
}


//-----------------------------------------------------------------------------
// иним текстовых xml
//-----------------------------------------------------------------------------
void InitGameAI(const char *FileName)
{
	// иним скрипт
	eFILE *TempF = vw_fopen(FileName);
	if (xmlAIBuffer != 0) {delete [] xmlAIBuffer; xmlAIBuffer = 0;}

	if (TempF == NULL)
	{
		ReleaseGameAI();
		return;
	}

	TempF->fseek(0, SEEK_END);
	int DataLength = TempF->ftell();
	TempF->fseek(0, SEEK_SET);
	xmlAIBuffer = new char[DataLength];
	TempF->fread(xmlAIBuffer, DataLength, 1);
	vw_fclose(TempF);
	xmlAI.Parse((const char*)xmlAIBuffer, 0, TIXML_ENCODING_UTF8);

	// берем первый элемент в скрипте
	xmlAIText = xmlAI.FirstChildElement("AstroMenaceAI");
}









//-----------------------------------------------------------------------------
// Установка нового (добавление) TimeSheet в нужное место
// !!! эта процедура заменяет вызов AttachTimeSheet
//-----------------------------------------------------------------------------
void AddNewTimeSheetToPos(CObject3D *Object, CTimeSheet *TimeSheet, CTimeSheet *AfterThisTimeSheet)
{
//	if (TimeSheet == 0) return;
	// никогда быть нулем не может!!! т.к. если что добавляем к первому
//	if (AfterThisTimeSheet == 0) return;

	if (AfterThisTimeSheet->Next != 0)
		AfterThisTimeSheet->Next->Prev = TimeSheet;
	else
		Object->EndTimeSheet = TimeSheet;

	TimeSheet->Next = AfterThisTimeSheet->Next;
	TimeSheet->Prev = AfterThisTimeSheet;

	AfterThisTimeSheet->Next = TimeSheet;
}




//-----------------------------------------------------------------------------
// Замена маркера действия набором действий
//-----------------------------------------------------------------------------
void InterAIMode(CObject3D *Object, CTimeSheet *TimeSheetMain)
{
	// 1 нужно проверить время общее и время последовательности

	// если вечный скрипт, в конец добавить тот же, с -1

	if (xmlAIBuffer == 0) return;
	if (xmlAIText == 0) return;


	// берем отдельно указатель, т.к. потом будем его менять
	CTimeSheet *AddAfter = TimeSheetMain;

	TiXmlElement *xmlElem = xmlAIText->FirstChildElement("AI");


	while (xmlElem)
	{
		// если находим нужный, приступаем к его интеграции
		if (xmlElem->Attribute("num"))
		if (atoi(xmlElem->Attribute("num")) == TimeSheetMain->AI_Mode)
		{

			// дальше смотрим, что нужно сделать...
			TiXmlElement *TChildElement = xmlElem->FirstChildElement();
			while (TChildElement)
			{
				if (!strcmp(TChildElement->Value(), "TimeSheet"))
				{
					// собираем новый элемент
					CTimeSheet *TimeSheet;
					TimeSheet = new CTimeSheet;
					AddNewTimeSheetToPos(Object, TimeSheet, AddAfter);
					AddAfter = TimeSheet;

					if (TChildElement->Attribute("aimode"))
					{
						TimeSheet->AI_Mode = atoi(TChildElement->Attribute("aimode"));
						TimeSheet->Time = 0.0f;
						if (TChildElement->Attribute("time"))
							TimeSheet->Time = (float)atof(TChildElement->Attribute("time"));

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
					}
					else
					{
						TimeSheet->AI_Mode = 0;
						TimeSheet->Time = 0.0f;
						if (TChildElement->Attribute("time"))
							TimeSheet->Time = (float)atof(TChildElement->Attribute("time"));

						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						if (TChildElement->Attribute("speed"))
							TimeSheet->Speed = (float)atof(TChildElement->Attribute("speed"));

						TimeSheet->Acceler = 1.0f;//0-1
						if (TChildElement->Attribute("acceler"))
							TimeSheet->Acceler = (float)atof(TChildElement->Attribute("acceler"));
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->Acceler = 1.0f;//0-1
						if (TChildElement->Attribute("acceler"))
							TimeSheet->Acceler = (float)atof(TChildElement->Attribute("acceler"));
						Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						if (TChildElement->Attribute("speedlr"))
							TimeSheet->SpeedLR = (float)atof(TChildElement->Attribute("speedlr"));

						TimeSheet->AccelerLR = 1.0f;//0-1
						if (TChildElement->Attribute("accelerlr"))
							TimeSheet->AccelerLR = (float)atof(TChildElement->Attribute("accelerlr"));
						Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						if (TChildElement->Attribute("speedud"))
							TimeSheet->SpeedUD = (float)atof(TChildElement->Attribute("speedud"));



						TimeSheet->SpeedByCamFB = 0.0f;
						if (TChildElement->Attribute("speedbycamfb"))
							TimeSheet->SpeedByCamFB = (float)atof(TChildElement->Attribute("speedbycamfb"));

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						if (TChildElement->Attribute("accelerbycamfb"))
							TimeSheet->AccelerByCamFB = (float)atof(TChildElement->Attribute("accelerbycamfb"));
						Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						if (TChildElement->Attribute("speedbycamlr"))
							TimeSheet->SpeedByCamLR = (float)atof(TChildElement->Attribute("speedbycamlr"));

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						if (TChildElement->Attribute("accelerbycamlr"))
							TimeSheet->AccelerByCamLR = (float)atof(TChildElement->Attribute("accelerbycamlr"));
						Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						if (TChildElement->Attribute("speedbycamud"))
							TimeSheet->SpeedByCamUD = (float)atof(TChildElement->Attribute("speedbycamud"));

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						if (TChildElement->Attribute("accelerbycamud"))
							TimeSheet->AccelerByCamUD = (float)atof(TChildElement->Attribute("accelerbycamud"));
						Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);



						TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
						if (TChildElement->Attribute("rotx"))
							TimeSheet->Rotation.x = (float)atof(TChildElement->Attribute("rotx"));
						if (TChildElement->Attribute("roty"))
							TimeSheet->Rotation.y = (float)atof(TChildElement->Attribute("roty"));
						if (TChildElement->Attribute("rotz"))
							TimeSheet->Rotation.z = (float)atof(TChildElement->Attribute("rotz"));

						TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						if (TChildElement->Attribute("rotacx"))
							TimeSheet->RotationAcceler.x = (float)atof(TChildElement->Attribute("rotacx"));
						if (TChildElement->Attribute("rotacy"))
							TimeSheet->RotationAcceler.y = (float)atof(TChildElement->Attribute("rotacy"));
						if (TChildElement->Attribute("rotacz"))
							TimeSheet->RotationAcceler.z = (float)atof(TChildElement->Attribute("rotacz"));
						Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						if (TChildElement->Attribute("fire"))
							if (atoi(TChildElement->Attribute("fire")) > 0)
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						if (TChildElement->Attribute("bossfire"))
							if (atoi(TChildElement->Attribute("bossfire")) > 0)
								TimeSheet->BossFire = true;
					}

				}
				// берем следующий элемент
				TChildElement = TChildElement->NextSiblingElement();
			}


			// если это елемент с -1, т.е. повторять до бесконечности
			// ставим последним такой же
			if (TimeSheetMain->Time == -1)
			{
				// собираем новый элемент
				CTimeSheet *TimeSheet;
				TimeSheet = new CTimeSheet;
				AddNewTimeSheetToPos(Object, TimeSheet, AddAfter);

				TimeSheet->AI_Mode = TimeSheetMain->AI_Mode;
				TimeSheet->Time = -1.0f;
				TimeSheet->InUse = false;

				TimeSheet->Speed = 0.0f;
				TimeSheet->Acceler = 1.0f;//0-1
				TimeSheet->Rotation = VECTOR3D(0.0f, 0.0f, 0.0f);
				TimeSheet->RotationAcceler = VECTOR3D(1.0f, 1.0f, 1.0f);//0-1
				TimeSheet->Fire = false;
				TimeSheet->BossFire = false;
			}

			return;
		}

		xmlElem = xmlElem->NextSiblingElement("AI");
	}

}

