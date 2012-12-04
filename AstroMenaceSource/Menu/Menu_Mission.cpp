/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#include "../Game.h"



int	SoundOnMissionID = -1;

int CurrentMission = -1;
// всего доступно миссий
int AllMission;
// начало и конец отображаемых миссий
int StartMission = 0;
int EndMission = 4;
// ограничение, сколько миссий может быть открыто (нужно для демо версии)
int MissionLimitation;

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


	// на одну меньше, т.к. это номер миссии, а не кол-во
	MissionLimitation = AllMission-1;

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
	if (Setup.Profile[CurrentProfile].OpenLevelNum > MissionLimitation)
		Setup.Profile[CurrentProfile].OpenLevelNum = MissionLimitation;



	RECT SrcRest, DstRest;
	SetRect(&SrcRest,2,2,863-2,484-2);
	SetRect(&DstRest,Setup.iAspectRatioWidth/2-427,175-15,Setup.iAspectRatioWidth/2-427+863-4,175-15+484-4);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/panel800_444_back.tga"), true, 0.9f*MenuContentTransp);



	int X1 = Setup.iAspectRatioWidth/2 - 372;
	int Y1 = 215;


	// подложка
	SetRect(&SrcRest,0,0,2,2);
	SetRect(&DstRest,X1-2,Y1-2,X1+2+750,Y1+2+320);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.2f*MenuContentTransp);
	SetRect(&DstRest,X1,Y1,X1+750,Y1+320);
	vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/blackpoint.tga"), true, 0.5f*MenuContentTransp);






	// подсвечиваем выбранный уровень
	if (CurrentMission != -1)
	if (StartMission<=CurrentMission && CurrentMission<=EndMission)
	{
		int ShowLine = CurrentMission;
		while (ShowLine>4) ShowLine -= 5;

		int Y1 = 215 + 64*ShowLine;

		SetRect(&SrcRest,0,0,2,2);
		SetRect(&DstRest,X1+1,Y1+1,X1+749,Y1+63);
		vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/whitepoint.tga"), true, 0.1f*MenuContentTransp);
	}




	// выводим миссии текущего листа
	int TMPSoundOnMissionID = -1;
	for (int i=StartMission; i<=EndMission; i++)
	if (AllMission > i) // на тот случай, если у нас кол-во миссий не делится на 5
	{
		// если не можем выбирать...
		if (i > Setup.Profile[CurrentProfile].OpenLevelNum)
		{
			SetRect(&SrcRest,0,0,64,64);
			SetRect(&DstRest,X1+2,Y1+2,X1+62,Y1+62);

			if (MissionIcon[i] != 0)
				vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName(MissionIcon[i]), true, 0.3f*MenuContentTransp);

			int SizeI = vw_FontSize(vw_GetText(MissionTitle[i]));
			if (SizeI < 750-30-64)
			{
				if (MissionTitleType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, vw_GetText(MissionTitle[i]));
				else
					vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, MissionTitle[i]);
			}
			else
			{
				if (MissionTitleType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, vw_GetText(MissionTitle[i]));
				else
					vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, MissionTitle[i]);
			}

			SizeI = vw_FontSize(vw_GetText(MissionDescr[i]));
			if (SizeI < 750-30-64)
			{
				if (MissionDescrType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, vw_GetText(MissionDescr[i]));
				else
					vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, MissionDescr[i]);
			}
			else
			{
				if (MissionDescrType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, vw_GetText(MissionDescr[i]));
				else
					vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, MissionDescr[i]);
			}
		}


		SetRect(&DstRest,X1,Y1+1,X1+750,Y1+64);
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


			if ((vw_OnRect(&DstRest) || InFocusByKeyboard) && !isDialogBoxDrawing())
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
				SetRect(&SrcRest,0,0,64,64);
				SetRect(&DstRest,X1,Y1,X1+64,Y1+64);
				if (MissionIcon != 0)
					vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName(MissionIcon[i]), true, MenuContentTransp);


				int SizeI = vw_FontSize(vw_GetText(MissionTitle[i]));
				if (SizeI < 750-30-64)
				{
					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, vw_GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, MissionTitle[i]);
				}
				else
				{
					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, vw_GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, MissionTitle[i]);
				}
				SizeI = vw_FontSize(vw_GetText(MissionDescr[i]));
				if (SizeI < 750-30-64)
				{
					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, vw_GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, MissionDescr[i]);

				}
				else
				{
					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, vw_GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, MissionDescr[i]);
				}



				if (CurrentMission != i)
				{
					SetRect(&SrcRest,0,0,2,2);
					SetRect(&DstRest,X1+64,Y1+1,X1+749,Y1+63);
					vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/whitepoint.tga"), true, 0.1f*MenuContentTransp);
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
				SetRect(&SrcRest,0,0,64,64);
				SetRect(&DstRest,X1+2,Y1+2,X1+62,Y1+62);
				if (MissionIcon != 0)
					vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName(MissionIcon[i]), true, 0.8f*MenuContentTransp);

				int SizeI = vw_FontSize(vw_GetText(MissionTitle[i]));
				if (SizeI < 750-30-64)
				{
					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, vw_GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, MissionTitle[i]);
				}
				else
				{
					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, vw_GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, MissionTitle[i]);
				}

				SizeI = vw_FontSize(vw_GetText(MissionDescr[i]));
				if (SizeI < 750-30-64)
				{
					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, vw_GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, MissionDescr[i]);

				}
				else
				{
					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, vw_GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, MissionDescr[i]);
				}
			}
		}

		Y1 += 64;
	}
	// если не стоим над профайлами - нужно сбросить флаг
	if (TMPSoundOnMissionID == -1) SoundOnMissionID = -1;














	// чтобы не сбивать.. устанавливаем "железно"
	Y1 = 218+320+24;
	int AllPagesQuant = AllMission/5;
	if (AllMission/5 < (AllMission*1.0f)/5.0f) AllPagesQuant++;

	int AllCurPage = StartMission/5 + 1;
	vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, vw_GetText("3_Page_%i_of_%i"), AllCurPage, AllPagesQuant);



	bool Off = false;
	if (AllCurPage == 1) Off = true;
	if (DrawButton200_2(X1+616-200-30-72, Y1-6, vw_GetText("1_Page_Up"), MenuContentTransp, Off))
	{
		StartMission -= 5;
		EndMission -= 5;
	}
	Off = false;
	if (AllCurPage == AllPagesQuant) Off = true;
	if (DrawButton200_2(X1+616-72, Y1-6, vw_GetText("1_Page_Down"), MenuContentTransp, Off))
	{
		StartMission += 5;
		EndMission += 5;
	}







	int X = Setup.iAspectRatioWidth/2 - 284;
	int Y = 165+100*5;
	if (DrawButton256(X,Y, vw_GetText("1_BACK"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
	{
		ComBuffer = PROFILE;
	}

	Off = true;
	if (CurrentMission >= 0) Off = false;
	X = Setup.iAspectRatioWidth/2 + 28;
	if (DrawButton256(X,Y, vw_GetText("1_NEXT"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime, Off))
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
