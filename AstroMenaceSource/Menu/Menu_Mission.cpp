/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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



int	SoundOnMissionID = -1;

int CurrentMission = -1;
// всего доступно миссий
int AllMission;
// начало и конец отображаемых миссий
int StartMission = 0;
int EndMission = 4;

// списки с данными для каждой миссии
char **MissionTitle = 0;
char **MissionDescr = 0;
float *MissionTitleColorR = 0;
float *MissionTitleColorG = 0;
float *MissionTitleColorB = 0;
float *MissionDescrColorR = 0;
float *MissionDescrColorG = 0;
float *MissionDescrColorB = 0;
char **MissionIcon = 0;
int *MissionTitleType = 0;
int *MissionDescrType = 0;
char **MissionFile = 0;

bool SliderUnderMouseControl = false;



// получаем имя файла миссии
char MissionFileName[MAX_PATH];
char *GetMissionFileName()
{
	for(int i = 0; i<MAX_PATH; i++) MissionFileName[i] = 0;

	if (MissionFile != 0)
	{
		if (MissionFile[CurrentMission] != 0)
			strcpy(MissionFileName, MissionFile[CurrentMission]);
		else
			return 0;
	}
	else
		return 0;

	return MissionFileName;
}














//------------------------------------------------------------------------------------
// инициализация данных списка миссий
//------------------------------------------------------------------------------------
void MissionsListInit()
{
	// по скрипту, смотрим что загружать + считаем сколько позиций
	cXMLDocument *xmlDoc = new cXMLDocument;

	// читаем данные
	if (!xmlDoc->Load("DATA/SCRIPT/list.xml"))
	{
		fprintf(stderr, "Can't find script file: %s\n", "DATA/SCRIPT/list.xml");
		delete xmlDoc;
		return;
	}


	// проверяем корневой элемент
	if (strcmp("AstroMenaceMissionsList", xmlDoc->RootXMLEntry->Name))
	{
		fprintf(stderr, "Can't find AstroMenaceMissionsList element in the: %s\n", "DATA/SCRIPT/list.xml");
		delete xmlDoc;
		return;
	}


	AllMission = 0;
	cXMLEntry *xmlEntry = xmlDoc->RootXMLEntry->FirstChild;
	while (xmlEntry)
	{
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
	for (int i=0; i<AllMission; i++)
	{
		MissionTitle[i] = 0;
		MissionDescr[i] = 0;
		MissionIcon[i] = 0;
		MissionFile[i] = 0;
		// установка типа, название (не ссылка)
		MissionTitleType[i] = 0;
		MissionDescrType[i] = 0;
		// установка цвета
		MissionTitleColorR[i] = 1.0f;MissionTitleColorG[i] = 0.5f;MissionTitleColorB[i] = 0.0f;
		MissionDescrColorR[i] = 1.0f;MissionDescrColorG[i] = 1.0f;MissionDescrColorB[i] = 1.0f;
	}




	// второй проход, заполняем массивы
	xmlEntry = xmlDoc->RootXMLEntry->FirstChild;
	int i = 0;
	while (xmlEntry)
	{
		// берем каждую миссию и смотрим настройки
		if (!strcmp(xmlEntry->Name, "Mission"))
		{
			cXMLEntry *TMission = xmlEntry->FirstChild;
			while (TMission)
			{
				// тайтл миссии
				if (!strcmp(TMission->Name, "Title"))
				{
					if (xmlDoc->GetEntryAttribute(TMission, "color") != 0)
					{
						switch (xmlDoc->iGetEntryAttribute(TMission, "color"))
						{
							case 0: // белый
								MissionTitleColorR[i]=1.0f;MissionTitleColorG[i]=1.0f;MissionTitleColorB[i]=1.0f;
								break;
							case 1: // желтый
								MissionTitleColorR[i]=1.0f;MissionTitleColorG[i]=1.0f;MissionTitleColorB[i]=0.0f;
								break;
							case 2: // красный
								MissionTitleColorR[i]=1.0f;MissionTitleColorG[i]=0.0f;MissionTitleColorB[i]=0.0f;
								break;
							case 3: // зеленый
								MissionTitleColorR[i]=0.0f;MissionTitleColorG[i]=1.0f;MissionTitleColorB[i]=0.0f;
								break;
							case 4: // оранжевый
								MissionTitleColorR[i]=1.0f;MissionTitleColorG[i]=0.5f;MissionTitleColorB[i]=0.0f;
								break;
							case 5: // серый,
								MissionTitleColorR[i]=0.5f;MissionTitleColorG[i]=0.5f;MissionTitleColorB[i]=0.5f;
								break;
							case 6: // темно оранжевый
								MissionTitleColorR[i]=1.0f;MissionTitleColorG[i]=0.3f;MissionTitleColorB[i]=0.0f;
								break;
						}
					}
					if (xmlDoc->GetEntryAttribute(TMission, "type") != 0) MissionTitleType[i] = xmlDoc->iGetEntryAttribute(TMission, "type");

					if (TMission->Content != 0)
					{
						MissionTitle[i] = new char[strlen(TMission->Content)+1];
						strcpy(MissionTitle[i], TMission->Content);
					}
				}
				else
				// описание миссии
				if (!strcmp(TMission->Name, "Descr"))
				{
					if (xmlDoc->GetEntryAttribute(TMission, "color") != 0)
					{
						switch (xmlDoc->iGetEntryAttribute(TMission, "color"))
						{
							case 0: // белый
								MissionDescrColorR[i]=1.0f;MissionDescrColorG[i]=1.0f;MissionDescrColorB[i]=1.0f;
								break;
							case 1: // желтый
								MissionDescrColorR[i]=1.0f;MissionDescrColorG[i]=1.0f;MissionDescrColorB[i]=0.0f;
								break;
							case 2: // красный
								MissionDescrColorR[i]=1.0f;MissionDescrColorG[i]=0.0f;MissionDescrColorB[i]=0.0f;
								break;
							case 3: // зеленый
								MissionDescrColorR[i]=0.0f;MissionDescrColorG[i]=1.0f;MissionDescrColorB[i]=0.0f;
								break;
							case 4: // оранжевый
								MissionDescrColorR[i]=1.0f;MissionDescrColorG[i]=0.5f;MissionDescrColorB[i]=0.0f;
								break;
							case 5: // серый,
								MissionDescrColorR[i]=0.5f;MissionDescrColorG[i]=0.5f;MissionDescrColorB[i]=0.5f;
								break;
							case 6: // темно оранжевый
								MissionDescrColorR[i]=0.8f;MissionDescrColorG[i]=0.4f;MissionDescrColorB[i]=0.0f;
								break;
						}
					}
					if (xmlDoc->GetEntryAttribute(TMission, "type") != 0) MissionDescrType[i] = xmlDoc->iGetEntryAttribute(TMission, "type");

					if (TMission->Content != 0)
					{
						MissionDescr[i] = new char[strlen(TMission->Content)+1];
						strcpy(MissionDescr[i], TMission->Content);
					}
				}
				else
				// иконка миссии
				if (!strcmp(TMission->Name, "Icon"))
				{
					if (TMission->Content != 0)
					{
						MissionIcon[i] = new char[strlen(TMission->Content)+1];
						strcpy(MissionIcon[i], TMission->Content);
					}
				}
				else
				// файл миссии
				if (!strcmp(TMission->Name, "File"))
				{
					if (TMission->Content != 0)
					{
						MissionFile[i] = new char[strlen(TMission->Content)+1];
						strcpy(MissionFile[i], TMission->Content);
					}
				}


				// берем следующий элемент
				TMission = TMission->Next;
			}

			i++;
		}
		else
		// проверяем музыку, возможно есть необходимость что-то заменить
		if (!strcmp(xmlEntry->Name, "GameMainMusic"))
		{
			eFILE *file = 0;
			file = vw_fopen(xmlEntry->Content);
			if (file != 0)
			{
				strcpy(GameMainMusic, xmlEntry->Content);
				GameMainMusicSet = true;
				vw_fclose(file);
				fprintf(stderr, "New GameMainMusic music file %s\n", xmlEntry->Content);
			}
			else
				fprintf(stderr, "Unable to find music file %s\n", xmlEntry->Content);
		}
		else
		if (!strcmp(xmlEntry->Name, "GameBossMusic"))
		{
			eFILE *file = 0;
			file = vw_fopen(xmlEntry->Content);
			if (file != 0)
			{
				strcpy(GameBossMusic, xmlEntry->Content);
				GameBossMusicSet = true;
				vw_fclose(file);
				fprintf(stderr, "New GameBossMusic music file %s\n", xmlEntry->Content);
			}
			else
				fprintf(stderr, "Unable to find music file %s\n", xmlEntry->Content);
		}
		else
		if (!strcmp(xmlEntry->Name, "GameDeathMusic"))
		{
			eFILE *file = 0;
			file = vw_fopen(xmlEntry->Content);
			if (file != 0)
			{
				strcpy(GameDeathMusic, xmlEntry->Content);
				GameDeathMusicSet = true;
				vw_fclose(file);
				fprintf(stderr, "New GameDeathMusic music file %s\n", xmlEntry->Content);
			}
			else
				fprintf(stderr, "Unable to find music file %s\n", xmlEntry->Content);
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
	if (MissionFile != 0)
	{
		for (int i=0; i<AllMission; i++)
		{
			delete [] MissionFile[i]; MissionFile[i] = 0;
		}
		delete [] MissionFile; MissionFile = 0;
	}

	if (MissionTitle != 0)
	{
		for (int i=0; i<AllMission; i++)
		{
			delete [] MissionTitle[i]; MissionTitle[i] = 0;
		}
		delete [] MissionTitle; MissionTitle = 0;
	}

	if (MissionDescr != 0)
	{
		for (int i=0; i<AllMission; i++)
		{
			delete [] MissionDescr[i]; MissionDescr[i] = 0;
		}
		delete [] MissionDescr; MissionDescr = 0;
	}

	if (MissionIcon != 0)
	{
		for (int i=0; i<AllMission; i++)
		{
			delete [] MissionIcon[i]; MissionIcon[i] = 0;
		}
		delete [] MissionIcon; MissionIcon = 0;
	}

	if (MissionTitleColorR != 0){delete [] MissionTitleColorR; MissionTitleColorR = 0;}
	if (MissionTitleColorG != 0){delete [] MissionTitleColorG; MissionTitleColorG = 0;}
	if (MissionTitleColorB != 0){delete [] MissionTitleColorB; MissionTitleColorB = 0;}
	if (MissionDescrColorR != 0){delete [] MissionDescrColorR; MissionDescrColorR = 0;}
	if (MissionDescrColorG != 0){delete [] MissionDescrColorG; MissionDescrColorG = 0;}
	if (MissionDescrColorB != 0){delete [] MissionDescrColorB; MissionDescrColorB = 0;}
	if (MissionTitleType != 0){delete [] MissionTitleType; MissionTitleType = 0;}
	if (MissionDescrType != 0){delete [] MissionDescrType; MissionDescrType = 0;}
}












//------------------------------------------------------------------------------------
// выбор миссии
//------------------------------------------------------------------------------------
void MissionMenu()
{

	// проверка ограничения
	if (Setup.Profile[CurrentProfile].OpenLevelNum > AllMission-1)
		Setup.Profile[CurrentProfile].OpenLevelNum = AllMission-1;



	RECT SrcRect, DstRect;
	SetRect(&SrcRect,2,2,863-2,484-2);
	SetRect(&DstRect,Setup.iAspectRatioWidth/2-427,175-15,Setup.iAspectRatioWidth/2-427+863-4,175-15+484-4);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/panel800_444_back.tga"), true, 0.9f*MenuContentTransp);



	int X1 = Setup.iAspectRatioWidth/2 - 372;
	int Y1 = 270;




	// выводим текущий профиль пилота
	int Size = vw_FontSize("%s: ", vw_GetText("3_Pilot_Profile"));
	vw_DrawFont(X1, 208+12, 0, 0, 1.0f, 0.0f,1.0f,0.0f, 1.0f*MenuContentTransp, "%s: ", vw_GetText("3_Pilot_Profile"));

	if (Size+vw_FontSize(Setup.Profile[CurrentProfile].Name) > 500)
	{
		vw_DrawFont(X1+Size, 208+12, 0, 500-Size, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, Setup.Profile[CurrentProfile].Name);
		vw_DrawFont(X1+510, 208+12, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, "...");
	}
	else
		vw_DrawFont(X1+Size, 208+12, 0, 0, 1.0f, 1.0f,1.0f,1.0f, MenuContentTransp, Setup.Profile[CurrentProfile].Name);

	if (DrawButton200_2(X1+616-72, 212, vw_GetText("1_Change_Profile"), MenuContentTransp, false))
	{
		ComBuffer = PROFILE;
	}




	// подложка для вывода описания миссий
	SetRect(&SrcRect,0,0,2,2);
	SetRect(&DstRect,X1-2,Y1-2,X1+2+710,Y1+2+320);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.2f*MenuContentTransp);
	SetRect(&DstRect,X1,Y1,X1+710,Y1+320);
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);




	// подсвечиваем выбранный уровень
	if (CurrentMission != -1)
	if (StartMission<=CurrentMission && CurrentMission<=EndMission)
	{
		int ShowLine = CurrentMission;
		if (ShowLine>=StartMission) ShowLine -= StartMission;

		SetRect(&SrcRect,0,0,2,2);
		SetRect(&DstRect,X1+1,Y1 + 64*ShowLine+1,X1+709,Y1 + 64*ShowLine+63);
		vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/whitepoint.tga"), true, 0.1f*MenuContentTransp);
	}




	// выводим миссии текущего листа
	int TMPSoundOnMissionID = -1;
	for (int i=StartMission; i<=EndMission; i++)
	if (AllMission > i)
	{
		// если не можем выбирать...
		if (i > Setup.Profile[CurrentProfile].OpenLevelNum)
		{
			SetRect(&SrcRect,0,0,64,64);
			SetRect(&DstRect,X1+2,Y1+2,X1+62,Y1+62);

			if (MissionIcon[i] != 0)
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


		SetRect(&DstRect,X1,Y1+1,X1+710,Y1+64);
		if (i <= Setup.Profile[CurrentProfile].OpenLevelNum)
		{
			// работаем с клавиатурой
			if ((MenuContentTransp >= 0.99f) && !isDialogBoxDrawing()) CurrentActiveMenuElement++;
			bool InFocusByKeyboard = false;
			if (CurrentKeyboardSelectMenuElement > 0)
			{
				if (CurrentKeyboardSelectMenuElement == CurrentActiveMenuElement)
				{
					InFocusByKeyboard = true;
				}
			}


			if ((vw_OnRect(&DstRect) || InFocusByKeyboard) && !isDialogBoxDrawing())
			{
				TMPSoundOnMissionID = i;
				CurrentCursorStatus = 1;
				// если только встали - нужно звуком это показать
				if (SoundOnMissionID != i)
				{
					SoundOnMissionID = i;
					// если задействуем клавиатуру - неиграем тут звук
					if (CurrentKeyboardSelectMenuElement == 0) Audio_PlaySound2D(5,1.0f);
				}

				// если стоим над ним...
				SetRect(&SrcRect,0,0,64,64);
				SetRect(&DstRect,X1,Y1,X1+64,Y1+64);
				if (MissionIcon != 0)
					vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName(MissionIcon[i]), true, MenuContentTransp);


				if (MissionTitleType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+9, -610, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, vw_GetText(MissionTitle[i]));
				else
					vw_DrawFont(X1+20+64, Y1+9, -610, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, MissionTitle[i]);

				if (MissionDescrType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+33, -610, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, vw_GetText(MissionDescr[i]));
				else
					vw_DrawFont(X1+20+64, Y1+33, -610, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, MissionDescr[i]);


				if (CurrentMission != i)
				{
					SetRect(&SrcRect,0,0,2,2);
					SetRect(&DstRect,X1+64,Y1+1,X1+709,Y1+63);
					vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/whitepoint.tga"), true, 0.1f*MenuContentTransp);
				}
				if (vw_GetWindowLBMouse(true) || (InFocusByKeyboard && (vw_GetKeys(SDLK_KP_ENTER) || vw_GetKeys(SDLK_RETURN))))
				{

					CurrentMission = i;
					Setup.Profile[CurrentProfile].LastMission = CurrentMission;
					Audio_PlaySound2D(6,1.0f);
					if (InFocusByKeyboard)
					{
						vw_SetKeys(SDLK_KP_ENTER, false);
						vw_SetKeys(SDLK_RETURN, false);
					}
				}

				if (vw_GetWindowLBDoubleMouse(true))
				{
					CurrentMission = i;
					Setup.Profile[CurrentProfile].LastMission = CurrentMission;
					// если уже играли в эту миссию
					if (Setup.Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0)
					{
						if (Setup.NeedShowHint[5])
						{
							SetCurrentDialogBox(14);
						}
						else
						{
							ComBuffer = WORKSHOP;
							CurrentWorkshop = 3;
							WorkshopCreate();
						}
					}
					else
					{
						ComBuffer = WORKSHOP;
						CurrentWorkshop = 3;
						WorkshopCreate();
					}
				}
			}
			else
			{
				// если не стоим над ним, но можем выбирать
				SetRect(&SrcRect,0,0,64,64);
				SetRect(&DstRect,X1+2,Y1+2,X1+62,Y1+62);
				if (MissionIcon != 0)
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
	if (DrawListUpButton(X1+718, Y1, MenuContentTransp, !(StartMission > 0)))
	{
		StartMission--;
		EndMission--;
	}

	if (DrawListDownButton(X1+718,Y1+320-32, MenuContentTransp, !(StartMission < AllMission-5)))
	{
		StartMission++;
		EndMission++;
	}
	// проверяем колесико мышки, если курсор находится над активной частью
	SetRect(&DstRect,X1,Y1,X1+750,Y1+320);
	if (vw_OnRect(&DstRect))
	{
		if (vw_GetWheelStatus() != 0 && !isDialogBoxDrawing())
		{
			StartMission += vw_GetWheelStatus();
			EndMission += vw_GetWheelStatus();

			if (StartMission < 0)
			{
				StartMission = 0;
				EndMission = 4;
			}
			if (EndMission > AllMission-1)
			{
				EndMission = AllMission-1;
				StartMission = EndMission-4;
			}

			vw_ResetWheelStatus();
		}
	}
	else if (vw_GetWheelStatus() != 0)
	{
		vw_ResetWheelStatus();
	}

	// выводим отображение положени в списке на полоске со стрелками
	SetRect(&SrcRect,0,0,32,32);
	SetRect(&DstRect,X1+750-32+4,Y1+32+((320.0f-64)/AllMission)*StartMission,X1+750-4,Y1+32+((320.0f-64)/AllMission)*(EndMission+1));
	vw_DrawTransparent(&DstRect, &SrcRect, vw_FindTextureByName("DATA/MENU/whitepoint.tga"), true, 0.3f*MenuContentTransp);

	// обработка перетягивания ползунка отображения позиции списка
	// если стоим на ползунком и нажали кнопку мышки - "захватываем"
	if (!SliderUnderMouseControl && vw_OnRect(&DstRect) && vw_GetWindowLBMouse(false) && !isDialogBoxDrawing())
	{
		SliderUnderMouseControl = true;
		Audio_PlaySound2D(2,1.0f);
	}
	// если ползунок был захвачен, но уже не над секцией где его можно перетягивать или отпустили мышку - отпускаем
	RECT DstRect2;
	SetRect(&DstRect2,X1+750-32+4,Y1+32,X1+750-4,Y1+32+(320.0f-64));
	if ((SliderUnderMouseControl && (!vw_OnRect(&DstRect2) || !vw_GetWindowLBMouse(false))) || isDialogBoxDrawing())
	{
		SliderUnderMouseControl = false;
	}
	// просто кликнули на зону перетягивания, не на ползунок
	if (!vw_OnRect(&DstRect) && vw_OnRect(&DstRect2) && vw_GetWindowLBMouse(false) && !isDialogBoxDrawing())
	{
		SliderUnderMouseControl = true;
		Audio_PlaySound2D(2,1.0f);
		vw_SetWindowLBMouse(false);
	}
	// отображаем курсором, что можно кликать на полосе прокрутки
	if (vw_OnRect(&DstRect2)) CurrentCursorStatus = 1;
	// корректируем его положение ползунка согласно положению мышки
	if (SliderUnderMouseControl)
	{
		int MouseX, MouseY;
		vw_GetMousePos(&MouseX, &MouseY);
		int SliderNewPosition = (MouseY - Y1-32)/((320.0f-64)/AllMission);

		StartMission = 0;
		EndMission = 4;
		if (SliderNewPosition > 2)
		{
			StartMission = SliderNewPosition-2;
			EndMission = SliderNewPosition+2;

			if (SliderNewPosition >= AllMission-2)
			{
				StartMission = AllMission-5;
				EndMission = AllMission-1;
			}
		}
	}






	int X = Setup.iAspectRatioWidth/2 - 284;
	int Y = 165+100*5;
	if (DrawButton256(X,Y, vw_GetText("1_MAIN_MENU"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
	{
		ComBuffer = MAIN_MENU;
	}

	X = Setup.iAspectRatioWidth/2 + 28;
	if (DrawButton256(X,Y, vw_GetText("1_NEXT"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime, !(CurrentMission >= 0)))
	{
		// если уже играли в эту миссию
		if (Setup.Profile[CurrentProfile].MissionReplayCount[CurrentMission] > 0)
		{
			if (Setup.NeedShowHint[5])
			{
				SetCurrentDialogBox(14);
			}
			else
			{
				ComBuffer = WORKSHOP;
				CurrentWorkshop = 3;
				WorkshopCreate();
			}
		}
		else
		{
			ComBuffer = WORKSHOP;
			CurrentWorkshop = 3;
			WorkshopCreate();
		}
	}




}
