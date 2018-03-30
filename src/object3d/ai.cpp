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

#include "../core/core.h"
#include "../object3d/object3d.h"




// основной документ
cXMLDocument *xmlAI = nullptr;



//-----------------------------------------------------------------------------
// освобождаем данные
//-----------------------------------------------------------------------------
void ReleaseGameAI()
{
	delete xmlAI;
	xmlAI = nullptr;
}


//-----------------------------------------------------------------------------
// иним текстовый xml
//-----------------------------------------------------------------------------
void InitGameAI(const char *FileName)
{
	xmlAI = new cXMLDocument(FileName);
	// иним скрипт
	if (!xmlAI->GetRootEntry()) {
		ReleaseGameAI();
		return;
	}
}









//-----------------------------------------------------------------------------
// Установка нового (добавление) TimeSheet в нужное место
// !!! эта процедура заменяет вызов AttachTimeSheet
//-----------------------------------------------------------------------------
void AddNewTimeSheetToPos(cObject3D *Object, sTimeSheet *TimeSheet, sTimeSheet *AfterThisTimeSheet)
{
//	if (TimeSheet == 0) return;
	// никогда быть нулем не может!!! т.к. если что добавляем к первому
//	if (AfterThisTimeSheet == 0) return;

	if (AfterThisTimeSheet->Next != nullptr)
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
void InterAIMode(cObject3D *Object, sTimeSheet *TimeSheetMain)
{
	// 1 нужно проверить время общее и время последовательности

	// если вечный скрипт, в конец добавить тот же, с -1

	if (xmlAI == nullptr)
		return;


	// берем отдельно указатель, т.к. потом будем его менять
	sTimeSheet *AddAfter = TimeSheetMain;

	sXMLEntry *xmlEntry  = xmlAI->FindEntryByName(xmlAI->GetRootEntry(), "AI");


	while (xmlEntry) {
		// если находим нужный, приступаем к его интеграции
		int tmpAI_Mode{0};
		if (xmlAI->iGetEntryAttribute(*xmlEntry, "num", tmpAI_Mode) &&
		    (tmpAI_Mode == TimeSheetMain->AI_Mode)) {

			// дальше смотрим, что нужно сделать...
			sXMLEntry *TChildEntry = xmlEntry->FirstChild;
			while (TChildEntry) {
				if (TChildEntry->Name == "TimeSheet") {
					// собираем новый элемент
					sTimeSheet *TimeSheet;
					TimeSheet = new sTimeSheet;
					AddNewTimeSheetToPos(Object, TimeSheet, AddAfter);
					AddAfter = TimeSheet;

					if (xmlAI->iGetEntryAttribute(*TChildEntry, "aimode", TimeSheet->AI_Mode)) {
						TimeSheet->Time = 0.0f;
						xmlAI->fGetEntryAttribute(*TChildEntry, "time", TimeSheet->Time);

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
					} else {
						TimeSheet->AI_Mode = 0;
						TimeSheet->Time = 0.0f;
						xmlAI->fGetEntryAttribute(*TChildEntry, "time", TimeSheet->Time);

						TimeSheet->InUse = false;

						TimeSheet->Speed = 0.0f;
						xmlAI->fGetEntryAttribute(*TChildEntry, "speed", TimeSheet->Speed);

						TimeSheet->Acceler = 1.0f;//0-1
						xmlAI->fGetEntryAttribute(*TChildEntry, "acceler", TimeSheet->Acceler);
						vw_Clamp(TimeSheet->Acceler, 0.0f, 1.0f);

						TimeSheet->SpeedLR = 0.0f;
						xmlAI->fGetEntryAttribute(*TChildEntry, "speedlr", TimeSheet->SpeedLR);

						TimeSheet->AccelerLR = 1.0f;//0-1
						xmlAI->fGetEntryAttribute(*TChildEntry, "accelerlr", TimeSheet->AccelerLR);
						vw_Clamp(TimeSheet->AccelerLR, 0.0f, 1.0f);

						TimeSheet->SpeedUD = 0.0f;
						xmlAI->fGetEntryAttribute(*TChildEntry, "speedud", TimeSheet->SpeedUD);



						TimeSheet->SpeedByCamFB = 0.0f;
						xmlAI->fGetEntryAttribute(*TChildEntry, "speedbycamfb", TimeSheet->SpeedByCamFB);

						TimeSheet->AccelerByCamFB = 1.0f;//0-1
						xmlAI->fGetEntryAttribute(*TChildEntry, "accelerbycamfb", TimeSheet->AccelerByCamFB);
						vw_Clamp(TimeSheet->AccelerByCamFB, 0.0f, 1.0f);

						TimeSheet->SpeedByCamLR = 0.0f;
						xmlAI->fGetEntryAttribute(*TChildEntry, "speedbycamlr", TimeSheet->SpeedByCamLR);

						TimeSheet->AccelerByCamLR = 1.0f;//0-1
						xmlAI->fGetEntryAttribute(*TChildEntry, "accelerbycamlr", TimeSheet->AccelerByCamLR);
						vw_Clamp(TimeSheet->AccelerByCamLR, 0.0f, 1.0f);

						TimeSheet->SpeedByCamUD = 0.0f;
						xmlAI->fGetEntryAttribute(*TChildEntry, "speedbycamud", TimeSheet->SpeedByCamUD);

						TimeSheet->AccelerByCamUD = 1.0f;//0-1
						xmlAI->fGetEntryAttribute(*TChildEntry, "accelerbycamud", TimeSheet->SpeedByCamUD);
						vw_Clamp(TimeSheet->AccelerByCamUD, 0.0f, 1.0f);



						TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
						xmlAI->fGetEntryAttribute(*TChildEntry, "rotx", TimeSheet->Rotation.x);
						xmlAI->fGetEntryAttribute(*TChildEntry, "roty", TimeSheet->Rotation.y);
						xmlAI->fGetEntryAttribute(*TChildEntry, "rotz", TimeSheet->Rotation.z);

						TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
						xmlAI->fGetEntryAttribute(*TChildEntry, "rotacx", TimeSheet->RotationAcceler.x);
						xmlAI->fGetEntryAttribute(*TChildEntry, "rotacy", TimeSheet->RotationAcceler.y);
						xmlAI->fGetEntryAttribute(*TChildEntry, "rotacz", TimeSheet->RotationAcceler.z);
						vw_Clamp(TimeSheet->RotationAcceler.x, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.y, 0.0f, 1.0f);
						vw_Clamp(TimeSheet->RotationAcceler.z, 0.0f, 1.0f);

						TimeSheet->Fire = false;
						int tmpFire{0};
						if (xmlAI->iGetEntryAttribute(*TChildEntry, "fire", tmpFire) &&
						    (tmpFire > 0))
								TimeSheet->Fire = true;
						TimeSheet->BossFire = false;
						int tmpBossFire{0};
						if (xmlAI->iGetEntryAttribute(*TChildEntry, "bossfire", tmpBossFire) &&
						    (tmpBossFire > 0))
								TimeSheet->BossFire = true;
					}

				}
				// берем следующий элемент
				TChildEntry = TChildEntry->Next;
			}


			// если это елемент с -1, т.е. повторять до бесконечности
			// ставим последним такой же
			if (TimeSheetMain->Time == -1) {
				// собираем новый элемент
				sTimeSheet *TimeSheet;
				TimeSheet = new sTimeSheet;
				AddNewTimeSheetToPos(Object, TimeSheet, AddAfter);

				TimeSheet->AI_Mode = TimeSheetMain->AI_Mode;
				TimeSheet->Time = -1.0f;
				TimeSheet->InUse = false;

				TimeSheet->Speed = 0.0f;
				TimeSheet->Acceler = 1.0f;//0-1
				TimeSheet->Rotation = sVECTOR3D(0.0f, 0.0f, 0.0f);
				TimeSheet->RotationAcceler = sVECTOR3D(1.0f, 1.0f, 1.0f);//0-1
				TimeSheet->Fire = false;
				TimeSheet->BossFire = false;
			}

			return;
		}

		xmlEntry = xmlEntry->Next;
	}

}

