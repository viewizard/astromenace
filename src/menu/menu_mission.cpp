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



int	SoundOnMissionID = -1;

int CurrentMission = -1;
// всего доступно миссий
int AllMission;
// начало и конец отображаемых миссий
int StartMission = 0;
int EndMission = 4;

// списки с данными для каждой миссии
char **MissionTitle = nullptr;
char **MissionDescr = nullptr;
float *MissionTitleColorR = nullptr;
float *MissionTitleColorG = nullptr;
float *MissionTitleColorB = nullptr;
float *MissionDescrColorR = nullptr;
float *MissionDescrColorG = nullptr;
float *MissionDescrColorB = nullptr;
char **MissionIcon = nullptr;
int *MissionTitleType = nullptr;
int *MissionDescrType = nullptr;
char **MissionFile = nullptr;

bool SliderUnderMouseControl = false;



// получаем имя файла миссии
char MissionFileName[MAX_PATH];
char *GetMissionFileName()
{
	for(int i = 0; i<MAX_PATH; i++) MissionFileName[i] = 0;

	if (MissionFile != nullptr) {
		if (MissionFile[CurrentMission] != nullptr)
			strcpy(MissionFileName, MissionFile[CurrentMission]);
		else
			return nullptr;
	} else
		return nullptr;

	return MissionFileName;
}














//------------------------------------------------------------------------------------
// инициализация данных списка миссий
//------------------------------------------------------------------------------------
void MissionsListInit()
{
	// по скрипту, смотрим что загружать + считаем сколько позиций
	cXMLDocument *xmlDoc = new cXMLDocument;

	std::string ScriptName{"script/list.xml"};

	// читаем данные
	if (!xmlDoc->Load(ScriptName.c_str())) {
		std::cerr << "Can't find script file: " << ScriptName << "\n";
		delete xmlDoc;
		return;
	}


	// проверяем корневой элемент
	if (strcmp("AstroMenaceMissionsList", xmlDoc->RootXMLEntry->Name)) {
		std::cerr << "Can't find AstroMenaceMissionsList element in the: " << ScriptName << "\n";
		delete xmlDoc;
		return;
	}


	AllMission = 0;
	cXMLEntry *xmlEntry = xmlDoc->RootXMLEntry->FirstChild;
	while (xmlEntry) {
		// считаем, сколько миссий в файле
		if (!strcmp(xmlEntry->Name, "Mission")) AllMission++;

		// берем следующий элемент по порядку
		xmlEntry = xmlEntry->Next;
	}

	// резервируем память
	MissionTitle = new char*[AllMission];
	MissionDescr = new char*[AllMission];
	MissionIcon = new char*[AllMission];
	MissionFile = new char*[AllMission];
	MissionTitleColorR = new float[AllMission];
	MissionTitleColorG = new float[AllMission];
	MissionTitleColorB = new float[AllMission];
	MissionDescrColorR = new float[AllMission];
	MissionDescrColorG = new float[AllMission];
	MissionDescrColorB = new float[AllMission];
	MissionTitleType = new int[AllMission];
	MissionDescrType = new int[AllMission];
	// зануляем данные, на всякий случай
	for (int i=0; i<AllMission; i++) {
		MissionTitle[i] = nullptr;
		MissionDescr[i] = nullptr;
		MissionIcon[i] = nullptr;
		MissionFile[i] = nullptr;
		// установка типа, название (не ссылка)
		MissionTitleType[i] = 0;
		MissionDescrType[i] = 0;
		// установка цвета
		MissionTitleColorR[i] = 1.0f;
		MissionTitleColorG[i] = 0.5f;
		MissionTitleColorB[i] = 0.0f;
		MissionDescrColorR[i] = 1.0f;
		MissionDescrColorG[i] = 1.0f;
		MissionDescrColorB[i] = 1.0f;
	}




	// второй проход, заполняем массивы
	xmlEntry = xmlDoc->RootXMLEntry->FirstChild;
	int i = 0;
	while (xmlEntry) {
		// берем каждую миссию и смотрим настройки
		if (!strcmp(xmlEntry->Name, "Mission")) {
			cXMLEntry *TMission = xmlEntry->FirstChild;
			while (TMission) {
				// тайтл миссии
				if (!strcmp(TMission->Name, "Title")) {
					if (xmlDoc->GetEntryAttribute(TMission, "color") != nullptr) {
						switch (xmlDoc->iGetEntryAttribute(TMission, "color")) {
						default: // белый
							MissionTitleColorR[i]=1.0f;
							MissionTitleColorG[i]=1.0f;
							MissionTitleColorB[i]=1.0f;
							break;
						case 1: // желтый
							MissionTitleColorR[i]=1.0f;
							MissionTitleColorG[i]=1.0f;
							MissionTitleColorB[i]=0.0f;
							break;
						case 2: // красный
							MissionTitleColorR[i]=1.0f;
							MissionTitleColorG[i]=0.0f;
							MissionTitleColorB[i]=0.0f;
							break;
						case 3: // зеленый
							MissionTitleColorR[i]=0.0f;
							MissionTitleColorG[i]=1.0f;
							MissionTitleColorB[i]=0.0f;
							break;
						case 4: // оранжевый
							MissionTitleColorR[i]=1.0f;
							MissionTitleColorG[i]=0.5f;
							MissionTitleColorB[i]=0.0f;
							break;
						case 5: // серый,
							MissionTitleColorR[i]=0.5f;
							MissionTitleColorG[i]=0.5f;
							MissionTitleColorB[i]=0.5f;
							break;
						case 6: // темно оранжевый
							MissionTitleColorR[i]=1.0f;
							MissionTitleColorG[i]=0.3f;
							MissionTitleColorB[i]=0.0f;
							break;
						}
					}
					if (xmlDoc->GetEntryAttribute(TMission, "type") != nullptr)
						MissionTitleType[i] = xmlDoc->iGetEntryAttribute(TMission, "type");

					if (TMission->Content != nullptr) {
						MissionTitle[i] = new char[strlen(TMission->Content)+1];
						strcpy(MissionTitle[i], TMission->Content);
					}
				} else
					// описание миссии
					if (!strcmp(TMission->Name, "Descr")) {
						if (xmlDoc->GetEntryAttribute(TMission, "color") != nullptr) {
							switch (xmlDoc->iGetEntryAttribute(TMission, "color")) {
							default: // белый
								MissionDescrColorR[i]=1.0f;
								MissionDescrColorG[i]=1.0f;
								MissionDescrColorB[i]=1.0f;
								break;
							case 1: // желтый
								MissionDescrColorR[i]=1.0f;
								MissionDescrColorG[i]=1.0f;
								MissionDescrColorB[i]=0.0f;
								break;
							case 2: // красный
								MissionDescrColorR[i]=1.0f;
								MissionDescrColorG[i]=0.0f;
								MissionDescrColorB[i]=0.0f;
								break;
							case 3: // зеленый
								MissionDescrColorR[i]=0.0f;
								MissionDescrColorG[i]=1.0f;
								MissionDescrColorB[i]=0.0f;
								break;
							case 4: // оранжевый
								MissionDescrColorR[i]=1.0f;
								MissionDescrColorG[i]=0.5f;
								MissionDescrColorB[i]=0.0f;
								break;
							case 5: // серый,
								MissionDescrColorR[i]=0.5f;
								MissionDescrColorG[i]=0.5f;
								MissionDescrColorB[i]=0.5f;
								break;
							case 6: // темно оранжевый
								MissionDescrColorR[i]=0.8f;
								MissionDescrColorG[i]=0.4f;
								MissionDescrColorB[i]=0.0f;
								break;
							}
						}
						if (xmlDoc->GetEntryAttribute(TMission, "type") != nullptr)
							MissionDescrType[i] = xmlDoc->iGetEntryAttribute(TMission, "type");

						if (TMission->Content != nullptr) {
							MissionDescr[i] = new char[strlen(TMission->Content)+1];
							strcpy(MissionDescr[i], TMission->Content);
						}
					} else
						// иконка миссии
						if (!strcmp(TMission->Name, "Icon")) {
							if (TMission->Content != nullptr) {
								MissionIcon[i] = new char[strlen(TMission->Content)+1];
								strcpy(MissionIcon[i], TMission->Content);
							}
						} else
							// файл миссии
							if (!strcmp(TMission->Name, "File")) {
								if (TMission->Content != nullptr) {
									MissionFile[i] = new char[strlen(TMission->Content)+1];
									strcpy(MissionFile[i], TMission->Content);
								}
							}


				// берем следующий элемент
				TMission = TMission->Next;
			}

			i++;
		} else
			// проверяем музыку, возможно есть необходимость что-то заменить
			if (!strcmp(xmlEntry->Name, "GameMainMusic")) {
				std::unique_ptr<sFILE> file = vw_fopen(xmlEntry->Content);
				if (file != nullptr) {
					strcpy(GameMainMusic, xmlEntry->Content);
					GameMainMusicSet = true;
					vw_fclose(file);
					std::cerr << "New GameMainMusic music file " << xmlEntry->Content << "\n";
				} else
					std::cerr << "Unable to find music file %s\n" << xmlEntry->Content << "\n";
			} else if (!strcmp(xmlEntry->Name, "GameBossMusic")) {
				std::unique_ptr<sFILE> file = vw_fopen(xmlEntry->Content);
				if (file != nullptr) {
					strcpy(GameBossMusic, xmlEntry->Content);
					GameBossMusicSet = true;
					vw_fclose(file);
					std::cerr << "New GameBossMusic music file %s\n" << xmlEntry->Content << "\n";
				} else
					std::cerr << "Unable to find music file %s\n" << xmlEntry->Content << "\n";
			} else if (!strcmp(xmlEntry->Name, "GameDeathMusic")) {
				std::unique_ptr<sFILE> file = vw_fopen(xmlEntry->Content);
				if (file != nullptr) {
					strcpy(GameDeathMusic, xmlEntry->Content);
					GameDeathMusicSet = true;
					vw_fclose(file);
					std::cerr << "New GameDeathMusic music file %s\n" << xmlEntry->Content << "\n";
				} else
					std::cerr << "Unable to find music file %s\n" << xmlEntry->Content << "\n";
			}


		// берем следующий элемент по порядку
		xmlEntry = xmlEntry->Next;
	}


	// чистим память, со скриптом работать больше не надо
	delete xmlDoc;
}




//------------------------------------------------------------------------------------
// удаление данных списка миссий
//------------------------------------------------------------------------------------
void MissionsListRelease()
{
	if (MissionFile != nullptr) {
		for (int i = 0; i < AllMission; i++) {
			delete [] MissionFile[i];
			MissionFile[i] = nullptr;
		}
		delete [] MissionFile;
		MissionFile = nullptr;
	}

	if (MissionTitle != nullptr) {
		for (int i = 0; i < AllMission; i++) {
			delete [] MissionTitle[i];
			MissionTitle[i] = nullptr;
		}
		delete [] MissionTitle;
		MissionTitle = nullptr;
	}

	if (MissionDescr != nullptr) {
		for (int i = 0; i < AllMission; i++) {
			delete [] MissionDescr[i];
			MissionDescr[i] = nullptr;
		}
		delete [] MissionDescr;
		MissionDescr = nullptr;
	}

	if (MissionIcon != nullptr) {
		for (int i = 0; i < AllMission; i++) {
			delete [] MissionIcon[i];
			MissionIcon[i] = nullptr;
		}
		delete [] MissionIcon;
		MissionIcon = nullptr;
	}

	if (MissionTitleColorR != nullptr) {
		delete [] MissionTitleColorR;
		MissionTitleColorR = nullptr;
	}
	if (MissionTitleColorG != nullptr) {
		delete [] MissionTitleColorG;
		MissionTitleColorG = nullptr;
	}
	if (MissionTitleColorB != nullptr) {
		delete [] MissionTitleColorB;
		MissionTitleColorB = nullptr;
	}
	if (MissionDescrColorR != nullptr) {
		delete [] MissionDescrColorR;
		MissionDescrColorR = nullptr;
	}
	if (MissionDescrColorG != nullptr) {
		delete [] MissionDescrColorG;
		MissionDescrColorG = nullptr;
	}
	if (MissionDescrColorB != nullptr) {
		delete [] MissionDescrColorB;
		MissionDescrColorB = nullptr;
	}
	if (MissionTitleType != nullptr) {
		delete [] MissionTitleType;
		MissionTitleType = nullptr;
	}
	if (MissionDescrType != nullptr) {
		delete [] MissionDescrType;
		MissionDescrType = nullptr;
	}
}












//------------------------------------------------------------------------------------
// выбор миссии
//------------------------------------------------------------------------------------
void MissionMenu()
{

	// проверка ограничения
	if (Setup.Profile[CurrentProfile].OpenLevelNum > AllMission-1)
		Setup.Profile[CurrentProfile].OpenLevelNum = AllMission-1;



	sRECT SrcRect, DstRect;
	SrcRect(2,2,863-2,484-2);
	DstRect(Setup.iAspectRatioWidth/2-427,175-15,Setup.iAspectRatioWidth/2-427+863-4,175-15+484-4);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/panel800_444_back.tga"), true, 0.9f*MenuContentTransp);



	int X1 = Setup.iAspectRatioWidth/2 - 372;
	int Y1 = 270;




	// выводим текущий профиль пилота
	int Size = vw_FontSize("%s: ", vw_GetText("3_Pilot_Profile"));
	vw_DrawFont(X1, 208+12, 0, 0, 1.0f, 0.0f,1.0f,0.0f, 1.0f*MenuContentTransp, "%s: ", vw_GetText("3_Pilot_Profile"));

	if (Size+vw_FontSize(Setup.Profile[CurrentProfile].Name) > 500) {
		vw_DrawFont(X1+Size, 208+12, 0, 500-Size, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, Setup.Profile[CurrentProfile].Name);
		vw_DrawFont(X1+510, 208+12, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "...");
	} else
		vw_DrawFont(X1+Size, 208+12, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, Setup.Profile[CurrentProfile].Name);

	if (DrawButton200_2(X1+616-72, 212, vw_GetText("1_Change_Profile"), MenuContentTransp, false)) {
		ComBuffer = eCommand::SWITCH_TO_PROFILE;
	}




	// подложка для вывода описания миссий
	SrcRect(0,0,2,2);
	DstRect(X1-2,Y1-2,X1+2+710,Y1+2+320);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.2f*MenuContentTransp);
	DstRect(X1,Y1,X1+710,Y1+320);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/blackpoint.tga"), true, 0.5f*MenuContentTransp);




	// подсвечиваем выбранный уровень
	if (CurrentMission != -1)
		if (StartMission<=CurrentMission && CurrentMission<=EndMission) {
			int ShowLine = CurrentMission;
			if (ShowLine>=StartMission) ShowLine -= StartMission;

			SrcRect(0,0,2,2);
			DstRect(X1+1,Y1 + 64*ShowLine+1,X1+709,Y1 + 64*ShowLine+63);
			vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/whitepoint.tga"), true, 0.1f*MenuContentTransp);
		}




	// выводим миссии текущего листа
	int TMPSoundOnMissionID = -1;
	for (int i=StartMission; i<=EndMission; i++)
		if (AllMission > i) {
			// если не можем выбирать...
			if (i > Setup.Profile[CurrentProfile].OpenLevelNum) {
				SrcRect(0,0,64,64);
				DstRect(X1+2,Y1+2,X1+62,Y1+62);

				if (MissionIcon[i] != nullptr)
					vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(MissionIcon[i]), true, 0.3f*MenuContentTransp);


				if (MissionTitleType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+9, -610, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, vw_GetText(MissionTitle[i]));
				else
					vw_DrawFont(X1+20+64, Y1+9, -610, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, MissionTitle[i]);

				if (MissionDescrType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+33, -610, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, vw_GetText(MissionDescr[i]));
				else
					vw_DrawFont(X1+20+64, Y1+33, -610, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, MissionDescr[i]);

			}


			DstRect(X1,Y1+1,X1+710,Y1+64);
			if (i <= Setup.Profile[CurrentProfile].OpenLevelNum) {
				// работаем с клавиатурой
				if ((MenuContentTransp >= 0.99f) && !isDialogBoxDrawing()) CurrentActiveMenuElement++;
				bool InFocusByKeyboard = false;
				if (CurrentKeyboardSelectMenuElement > 0) {
					if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement) {
						InFocusByKeyboard = true;
					}
				}


				if ((vw_OnRect(DstRect) || InFocusByKeyboard) && !isDialogBoxDrawing()) {
					TMPSoundOnMissionID = i;
					CurrentCursorStatus = 1;
					// если только встали - нужно звуком это показать
					if (SoundOnMissionID != i) {
						SoundOnMissionID = i;
						// если задействуем клавиатуру - неиграем тут звук
						if (CurrentKeyboardSelectMenuElement == 0) Audio_PlaySound2D(5,1.0f);
					}

					// если стоим над ним...
					SrcRect(0,0,64,64);
					DstRect(X1,Y1,X1+64,Y1+64);
					if (MissionIcon != nullptr)
						vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(MissionIcon[i]), true, MenuContentTransp);


					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -610, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, vw_GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -610, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, MissionTitle[i]);

					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -610, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, vw_GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -610, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, MissionDescr[i]);


					if (CurrentMission != i) {
						SrcRect(0,0,2,2);
						DstRect(X1+64,Y1+1,X1+709,Y1+63);
						vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/whitepoint.tga"), true, 0.1f*MenuContentTransp);
					}
					if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN)))) {

						CurrentMission = i;
						Setup.Profile[CurrentProfile].LastMission = CurrentMission;
						Audio_PlaySound2D(6,1.0f);
						if (InFocusByKeyboard) {
							vw_SetKeys(SDLK_KP_ENTER, false);
							vw_SetKeys(SDLK_RETURN, false);
						}
					}

					if (vw_GetWindowLBDoubleMouse(true)) {
						CurrentMission = i;
						Setup.Profile[CurrentProfile].LastMission = CurrentMission;
						// если уже играли в эту миссию
						if (Setup.Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0) {
							if (Setup.NeedShowHint[5]) {
								SetCurrentDialogBox(eDialogBox::StartMissionSecondTime);
							} else {
								ComBuffer = eCommand::SWITCH_TO_WORKSHOP;
								CurrentWorkshop = 3;
								WorkshopCreate();
							}
						} else {
							ComBuffer = eCommand::SWITCH_TO_WORKSHOP;
							CurrentWorkshop = 3;
							WorkshopCreate();
						}
					}
				} else {
					// если не стоим над ним, но можем выбирать
					SrcRect(0,0,64,64);
					DstRect(X1+2,Y1+2,X1+62,Y1+62);
					if (MissionIcon != nullptr)
						vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(MissionIcon[i]), true, 0.8f*MenuContentTransp);


					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -610, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, vw_GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -610, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, MissionTitle[i]);

					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -610, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, vw_GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -610, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, MissionDescr[i]);

				}
			}

			Y1 += 64;
		}
	// если не стоим над профилями - нужно сбросить флаг
	if (TMPSoundOnMissionID == -1) SoundOnMissionID = -1;





	Y1 = 270;
	// стрелки перемещения списка
	if (DrawListUpButton(X1+718, Y1, MenuContentTransp, !(StartMission > 0))) {
		StartMission--;
		EndMission--;
	}

	if (DrawListDownButton(X1+718,Y1+320-32, MenuContentTransp, !(StartMission < AllMission-5))) {
		StartMission++;
		EndMission++;
	}
	// проверяем колесико мышки, если курсор находится над активной частью
	DstRect(X1,Y1,X1+750,Y1+320);
	if (vw_OnRect(DstRect)) {
		if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing()) {
			StartMission += vw_GetWheelStatus();
			EndMission += vw_GetWheelStatus();

			if (StartMission < 0) {
				StartMission = 0;
				EndMission = 4;
			}
			if (EndMission > AllMission-1) {
				EndMission = AllMission-1;
				StartMission = EndMission-4;
			}

			vw_ResetWheelStatus();
		}
	} else if (vw_GetWheelStatus() != 0) {
		vw_ResetWheelStatus();
	}

	// выводим отображение положени в списке на полоске со стрелками
	SrcRect(0,0,32,32);
	DstRect(X1+750-32+4,Y1+32+((320.0f-64)/AllMission)*StartMission,X1+750-4,Y1+32+((320.0f-64)/AllMission)*(EndMission+1));
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("menu/whitepoint.tga"), true, 0.3f*MenuContentTransp);

	// обработка перетягивания ползунка отображения позиции списка
	// если стоим на ползунком и нажали кнопку мышки - "захватываем"
	if (!SliderUnderMouseControl && vw_OnRect(DstRect) && vw_GetWindowLBMouse(false) && !isDialogBoxDrawing()) {
		SliderUnderMouseControl = true;
		Audio_PlaySound2D(2,1.0f);
	}
	// если ползунок был захвачен, но уже не над секцией где его можно перетягивать или отпустили мышку - отпускаем
	sRECT DstRect2;
	DstRect2(X1+750-32+4,Y1+32,X1+750-4,Y1+32+(320.0f-64));
	if ((SliderUnderMouseControl && (!vw_OnRect(DstRect2) || !vw_GetWindowLBMouse(false))) || isDialogBoxDrawing()) {
		SliderUnderMouseControl = false;
	}
	// просто кликнули на зону перетягивания, не на ползунок
	if (!vw_OnRect(DstRect) && vw_OnRect(DstRect2) && vw_GetWindowLBMouse(false) && !isDialogBoxDrawing()) {
		SliderUnderMouseControl = true;
		Audio_PlaySound2D(2,1.0f);
		vw_SetWindowLBMouse(false);
	}
	// отображаем курсором, что можно кликать на полосе прокрутки
	if (vw_OnRect(DstRect2)) CurrentCursorStatus = 1;
	// корректируем его положение ползунка согласно положению мышки
	if (SliderUnderMouseControl) {
		int MouseX, MouseY;
		vw_GetMousePos(&MouseX, &MouseY);
		int SliderNewPosition = (MouseY - Y1-32)/((320.0f-64)/AllMission);

		StartMission = 0;
		EndMission = 4;
		if (SliderNewPosition > 2) {
			StartMission = SliderNewPosition-2;
			EndMission = SliderNewPosition+2;

			if (SliderNewPosition >= AllMission-2) {
				StartMission = AllMission-5;
				EndMission = AllMission-1;
			}
		}
	}






	int X = Setup.iAspectRatioWidth/2 - 284;
	int Y = 165+100*5;
	if (DrawButton256(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
		ComBuffer = eCommand::SWITCH_TO_MAIN_MENU;

	X = Setup.iAspectRatioWidth/2 + 28;
	if (DrawButton256(X,Y, vw_GetText("1_NEXT"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime, !(CurrentMission >= 0))) {
		// если уже играли в эту миссию
		if (Setup.Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0) {
			if (Setup.NeedShowHint[5]) {
				SetCurrentDialogBox(eDialogBox::StartMissionSecondTime);
			} else {
				ComBuffer = eCommand::SWITCH_TO_WORKSHOP;
				CurrentWorkshop = 3;
				WorkshopCreate();
			}
		} else {
			ComBuffer = eCommand::SWITCH_TO_WORKSHOP;
			CurrentWorkshop = 3;
			WorkshopCreate();
		}
	}




}
