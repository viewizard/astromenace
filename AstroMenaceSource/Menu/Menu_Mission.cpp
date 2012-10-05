/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Menu_Mission.cpp

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
	TiXmlDocument	*xmlDoc = 0;
	TiXmlElement	*xmlElem = 0;
	TiXmlElement	*xmlAstroMenaceScript = 0;

	xmlDoc = new TiXmlDocument("DATA/SCRIPT/list.xml");

	if (!xmlDoc->LoadFile())
	{
		xmlDoc->Clear();
		delete xmlDoc; xmlDoc = 0;
		fprintf(stderr, "Can't find script file: %s\n", "DATA/SCRIPT/list.xml");
		return;
	}

	// берем первый элемент в скрипте
	xmlAstroMenaceScript = xmlDoc->FirstChildElement("AstroMenaceMissionsList");
	if (xmlAstroMenaceScript != 0)
	{
		xmlElem = xmlAstroMenaceScript->FirstChildElement();
	}
	else
	{
		fprintf(stderr, "Can't find AstroMenaceMissionsList element in the: %s\n", "DATA/SCRIPT/list.xml");
		delete xmlDoc; xmlDoc = 0;
		return;
	}


	AllMission = 0;
	while (xmlElem)
	{
		// считаем, сколько миссий в файле
		if (!strcmp(xmlElem->Value(), "Mission")) AllMission++;

		// берем следующий элемент по порядку
		xmlElem = xmlElem->NextSiblingElement();
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
	xmlElem = xmlAstroMenaceScript->FirstChildElement();
	int i = 0;
	while (xmlElem)
	{
		// проверяем музыку, возможно есть необходимость что-то заменить
		if (!strcmp(xmlElem->Value(), "GameMainMusic"))
		{
			eFILE *file = 0;
			file = vw_fopen(xmlElem->GetText());
			if (file != 0)
			{
				strcpy(GameMainMusic, xmlElem->GetText());
				GameMainMusicSet = true;
				vw_fclose(file);
				fprintf(stderr, "New GameMainMusic music file %s\n", xmlElem->GetText());
			}
			else
				fprintf(stderr, "Unable to find music file %s\n", xmlElem->GetText());
		}
		if (!strcmp(xmlElem->Value(), "GameBossMusic"))
		{
			eFILE *file = 0;
			file = vw_fopen(xmlElem->GetText());
			if (file != 0)
			{
				strcpy(GameBossMusic, xmlElem->GetText());
				GameBossMusicSet = true;
				vw_fclose(file);
				fprintf(stderr, "New GameBossMusic music file %s\n", xmlElem->GetText());
			}
			else
				fprintf(stderr, "Unable to find music file %s\n", xmlElem->GetText());
		}
		if (!strcmp(xmlElem->Value(), "GameDeathMusic"))
		{
			eFILE *file = 0;
			file = vw_fopen(xmlElem->GetText());
			if (file != 0)
			{
				strcpy(GameDeathMusic, xmlElem->GetText());
				GameDeathMusicSet = true;
				vw_fclose(file);
				fprintf(stderr, "New GameDeathMusic music file %s\n", xmlElem->GetText());
			}
			else
				fprintf(stderr, "Unable to find music file %s\n", xmlElem->GetText());
		}


		// берем каждую миссию и смотрим настройки
		if (!strcmp(xmlElem->Value(), "Mission"))
		{
			TiXmlElement *TMission = xmlElem->FirstChildElement();
			while (TMission)
			{
				// тайтл миссии
				if (!strcmp(TMission->Value(), "Title"))
				{
					if (TMission->Attribute("color"))
					{
						switch (atoi(TMission->Attribute("color")))
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
					if (TMission->Attribute("type")) MissionTitleType[i] = atoi(TMission->Attribute("type"));

					MissionTitle[i] = new char[strlen(TMission->GetText())+1];
					strcpy(MissionTitle[i], TMission->GetText());
				}

				// описание миссии
				if (!strcmp(TMission->Value(), "Descr"))
				{
					if (TMission->Attribute("color"))
					{
						switch (atoi(TMission->Attribute("color")))
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
					if (TMission->Attribute("type")) MissionDescrType[i] = atoi(TMission->Attribute("type"));

					MissionDescr[i] = new char[strlen(TMission->GetText())+1];
					strcpy(MissionDescr[i], TMission->GetText());
				}

				// иконка миссии
				if (!strcmp(TMission->Value(), "Icon"))
				{
					MissionIcon[i] = new char[strlen(TMission->GetText())+1];
					strcpy(MissionIcon[i], TMission->GetText());
				}

				// файл миссии
				if (!strcmp(TMission->Value(), "File"))
				{
					MissionFile[i] = new char[strlen(TMission->GetText())+1];
					strcpy(MissionFile[i], TMission->GetText());
				}


				// берем следующий элемент
				TMission = TMission->NextSiblingElement();
			}

			i++;
		}

		// берем следующий элемент по порядку
		xmlElem = xmlElem->NextSiblingElement();
	}


	// чистим память, со скриптом работать больше не надо
	xmlDoc->Clear();
	delete xmlDoc; xmlDoc = 0;


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

			int SizeI = vw_FontSize(GetText(MissionTitle[i]));
			if (SizeI < 750-30-64)
			{
				if (MissionTitleType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, GetText(MissionTitle[i]));
				else
					vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, MissionTitle[i]);
			}
			else
			{
				if (MissionTitleType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, GetText(MissionTitle[i]));
				else
					vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.3f*MenuContentTransp, MissionTitle[i]);
			}

			SizeI = vw_FontSize(GetText(MissionDescr[i]));
			if (SizeI < 750-30-64)
			{
				if (MissionDescrType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, GetText(MissionDescr[i]));
				else
					vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, MissionDescr[i]);
			}
			else
			{
				if (MissionDescrType[i] == 1)
					vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, GetText(MissionDescr[i]));
				else
					vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.3f*MenuContentTransp, MissionDescr[i]);
			}
		}


		SetRect(&DstRest,X1,Y1+1,X1+750,Y1+64);
		if (i <= Setup.Profile[CurrentProfile].OpenLevelNum)
		{
			if (vw_OnRect(&DstRest) && !isDialogBoxDrawing())
			{
				TMPSoundOnMissionID = i;
				CurrentCursorStatus = 1;
				// если только встали - нужно звуком это показать
				if (SoundOnMissionID != i)
				{
					SoundOnMissionID = i;
					Audio_PlaySound2D(5,1.0f);
				}

				// если стоим над ним...
				SetRect(&SrcRest,0,0,64,64);
				SetRect(&DstRest,X1,Y1,X1+64,Y1+64);
				if (MissionIcon != 0)
					vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName(MissionIcon[i]), true, MenuContentTransp);


				int SizeI = vw_FontSize(GetText(MissionTitle[i]));
				if (SizeI < 750-30-64)
				{
					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, MissionTitle[i]);
				}
				else
				{
					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], MenuContentTransp, MissionTitle[i]);
				}
				SizeI = vw_FontSize(GetText(MissionDescr[i]));
				if (SizeI < 750-30-64)
				{
					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, MissionDescr[i]);

				}
				else
				{
					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], MenuContentTransp, MissionDescr[i]);
				}



				if (CurrentMission != i)
				{
					SetRect(&SrcRest,0,0,2,2);
					SetRect(&DstRest,X1+64,Y1+1,X1+749,Y1+63);
					vw_DrawTransparent(&DstRest, &SrcRest, vw_FindTextureByName("DATA/MENU/whitepoint.tga"), true, 0.1f*MenuContentTransp);
				}
				if (vw_GetWindowLBMouse(true))
				{

					CurrentMission = i;
					Setup.Profile[CurrentProfile].LastMission = CurrentMission;
					Audio_PlaySound2D(6,1.0f);
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

				int SizeI = vw_FontSize(GetText(MissionTitle[i]));
				if (SizeI < 750-30-64)
				{
					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, MissionTitle[i]);
				}
				else
				{
					if (MissionTitleType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, GetText(MissionTitle[i]));
					else
						vw_DrawFont(X1+20+64, Y1+9, -650, 0, 1.0f, MissionTitleColorR[i],MissionTitleColorG[i],MissionTitleColorB[i], 0.8f*MenuContentTransp, MissionTitle[i]);
				}

				SizeI = vw_FontSize(GetText(MissionDescr[i]));
				if (SizeI < 750-30-64)
				{
					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, GetText(MissionDescr[i]));
					else
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, MissionDescr[i]);

				}
				else
				{
					if (MissionDescrType[i] == 1)
						vw_DrawFont(X1+20+64, Y1+33, -650, 0, 1.0f, MissionDescrColorR[i],MissionDescrColorG[i],MissionDescrColorB[i], 0.8f*MenuContentTransp, GetText(MissionDescr[i]));
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
	vw_DrawFont(X1, Y1, 0, 0, 1.0f, 1.0f,1.0f,1.0f, 0.5f*MenuContentTransp, GetText("3_Page_%i_of_%i"), AllCurPage, AllPagesQuant);



	bool Off = false;
	if (AllCurPage == 1) Off = true;
	if (DrawButton200_2(X1+616-200-30-72, Y1-6, GetText("1_Page_Up"), MenuContentTransp, Off))
	{
		StartMission -= 5;
		EndMission -= 5;
	}
	Off = false;
	if (AllCurPage == AllPagesQuant) Off = true;
	if (DrawButton200_2(X1+616-72, Y1-6, GetText("1_Page_Down"), MenuContentTransp, Off))
	{
		StartMission += 5;
		EndMission += 5;
	}







	int X = Setup.iAspectRatioWidth/2 + 28;
	int Y = 165+100*5;
	Off = true;
	if (CurrentMission >= 0) Off = false;
	if (DrawButton256(X,Y, GetText("1_NEXT"), MenuContentTransp, &Button11Transp, &LastButton11UpdateTime, Off))
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
	X = Setup.iAspectRatioWidth/2 - 284;
	if (DrawButton256(X,Y, GetText("1_BACK"), MenuContentTransp, &Button10Transp, &LastButton10UpdateTime))
	{
		ComBuffer = PROFILE;
	}



}
