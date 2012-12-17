/************************************************************************************

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


	Web Site: http://www.viewizard.com/
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "../Game.h"




//-----------------------------------------------------------------------------
// задаем базовые настройки
//-----------------------------------------------------------------------------
void InitSetup()
{
	// базовые настройки, устанавливаем всегда - кто там знает, что в xml файле есть
	// всегда устанавливаем текущее разрешение рабочего стола

	// устанавливаем номер билда
	Setup.BuildVersion = GAME_BUILD;

	Setup.MenuLanguage = 1; // en by default
	Setup.VoiceLanguage = 1; // en by default
	Setup.FontNumber = 0;

	Setup.Width = 1228;
	Setup.Height = 768;
	Setup.BPP = 0;
	Setup.fAspectRatioWidth = 1228.0f;
	Setup.fAspectRatioHeight = 768.0f;
	Setup.iAspectRatioWidth = 1228;
	Setup.iAspectRatioHeight = 768;

	Setup.CameraModeWithStandardAspectRatio = 0;

	Setup.TextureFilteringMode = 2;
	Setup.TexturesQuality = 3;
	Setup.MSAA = 0;
	Setup.CSAA = 0;
	Setup.VisualEffectsQuality = 0;
	Setup.AnisotropyLevel = 1;
	Setup.TexturesCompression = 1;
	Setup.UseGLSL = false;
	Setup.ShadowMap = 0;
	Setup.MaxPointLights = 3;
	Setup.MusicSw = 8;
	Setup.Music_check = true;
	Setup.SoundSw = 10;
	Setup.VoiceSw = 10;
	Setup.Sound_check = true;
	Setup.VSync = 0;
	Setup.Brightness = 5;
	Setup.ShowFPS = false;
	Setup.GameWeaponInfoType = 1;
	Setup.GameSpeed = 1.5f;
	Setup.LoadingHint = 0;


	Setup.KeyboardDecreaseGameSpeed = SDLK_F5;
	Setup.KeyboardResetGameSpeed = SDLK_F6;
	Setup.KeyboardIncreaseGameSpeed = SDLK_F7;
	Setup.KeyboardGameWeaponInfoType = SDLK_F8;
	Setup.KeyboardPrimaryWeaponFireMode = SDLK_F9;
	Setup.KeyboardSecondaryWeaponFireMode = SDLK_F10;

	Setup.KeyBoardLeft = SDLK_LEFT;
	Setup.KeyBoardRight = SDLK_RIGHT;
	Setup.KeyBoardUp = SDLK_UP;
	Setup.KeyBoardDown = SDLK_DOWN;
	Setup.KeyBoardPrimary = SDLK_LCTRL;
	Setup.KeyBoardSecondary = SDLK_SPACE;
	Setup.MousePrimary = SDL_BUTTON_LEFT;
	Setup.MouseSecondary = SDL_BUTTON_RIGHT;
	Setup.JoystickPrimary = 0;
	Setup.JoystickSecondary = 1;
	Setup.JoystickNum = 0;
	Setup.JoystickDeadZone = 2;

	Setup.ControlSensivity = 5;
	Setup.MouseControl = true;
	Setup.LastProfile = -1;
	Setup.MenuScript = 0;


	strcpy(Setup.TopScores[0].Name, "Viewizard");
	Setup.TopScores[0].Score = 100000;
	strcpy(Setup.TopScores[1].Name, "Alex");
	Setup.TopScores[1].Score = 90000;
	strcpy(Setup.TopScores[2].Name, "Michael");
	Setup.TopScores[2].Score = 80000;
	strcpy(Setup.TopScores[3].Name, "Jeffrey");
	Setup.TopScores[3].Score = 70000;
	strcpy(Setup.TopScores[4].Name, "Christopher Hughson");
	Setup.TopScores[4].Score = 60000;
	strcpy(Setup.TopScores[5].Name, "Becky");
	Setup.TopScores[5].Score = 40000;
	strcpy(Setup.TopScores[6].Name, "Greg");
	Setup.TopScores[6].Score = 20000;
	strcpy(Setup.TopScores[7].Name, "Jay Coleman");
	Setup.TopScores[7].Score = 10000;
	strcpy(Setup.TopScores[8].Name, "Kelvin");
	Setup.TopScores[8].Score = 5000;
	strcpy(Setup.TopScores[9].Name, "Stephan Gregory");
	Setup.TopScores[9].Score = 1000;


	for(int i=0; i<5; i++)
		Setup.Profile[i].Used = false;

	// сбрасываем хинты в тру, чтобы показывать
	for(int i=0; i<10; i++)
		Setup.NeedShowHint[i] = true;


	// всегда включен по умолчанию, если что железо само поймет что ему надо
	Setup.VBOCoreMode = 1;
	Setup.VAOCoreMode = 1;
	Setup.FBOCoreMode = 1;
	// по умолчанию всегда меньше 128 метров
	Setup.EqualOrMore128MBVideoRAM = false;
	// по умолчанию генерируем в видео карте (после иниц. окна поставим правильное), по крайней мере в виндовс немного быстрее
	Setup.HardwareMipMapGeneration = true;
}














//-----------------------------------------------------------------------------
// записываем файл с настройками
//-----------------------------------------------------------------------------
void SaveXMLSetupFile()
{
	cXMLDocument *XMLdoc = new cXMLDocument;

	cXMLEntry *RootXMLEntry = XMLdoc->AddEntry(0, "AstroMenaceSettings");

	XMLdoc->AddComment(RootXMLEntry, " AstroMenace game Settings ");



	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "GAME_BUILD"), "value", GAME_BUILD);
	switch (Setup.MenuLanguage)
	{
		case 1: XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MenuLanguage"), "value", "en"); break;
		case 2: XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MenuLanguage"), "value", "de"); break;
		case 3: XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MenuLanguage"), "value", "ru"); break;
	}
	switch (Setup.VoiceLanguage)
	{
		case 1: XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "VoiceLanguage"), "value", "en"); break;
		case 2: XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "VoiceLanguage"), "value", "de"); break;
		case 3: XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "VoiceLanguage"), "value", "ru"); break;
	}
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "FontNumber"), "value", Setup.FontNumber);


	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "Width"), "value", Setup.Width);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "Height"), "value", Setup.Height);
	XMLdoc->AddComment(RootXMLEntry, " Windows (BPP = 0) or Full Screen (BPP = 16 or 32) ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "BPP"), "value", Setup.BPP);
	XMLdoc->AddComment(RootXMLEntry, " Aspect Ratio must be 4:3 or 16:10 ");
	if (Setup.iAspectRatioWidth == 1024)
		XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "AspectRatio"), "value", "4:3");
	else
		XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "AspectRatio"), "value", "16:10");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "CameraModeWithStandardAspectRatio"), "value", Setup.CameraModeWithStandardAspectRatio);

	XMLdoc->AddComment(RootXMLEntry, " Don't change this setting unless you know what you are doing ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "VBOCoreMode"), "value", Setup.VBOCoreMode);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "VAOCoreMode"), "value", Setup.VAOCoreMode);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "FBOCoreMode"), "value", Setup.FBOCoreMode);
	XMLdoc->AddComment(RootXMLEntry, " If your video card have 128+ MB VRAM on board - turn it on ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "EqualOrMore128MBVideoRAM"), "value", Setup.EqualOrMore128MBVideoRAM);
	XMLdoc->AddComment(RootXMLEntry, " Don't change this setting unless you know what you are doing ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "HardwareMipMapGeneration"), "value", Setup.HardwareMipMapGeneration);


	XMLdoc->AddComment(RootXMLEntry, " Common settings ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "TextureFilteringMode"), "value", Setup.TextureFilteringMode);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "TexturesQuality"), "value", Setup.TexturesQuality);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MSAA"), "value", Setup.MSAA);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "CSAA"), "value", Setup.CSAA);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "VisualEffectsQuality"), "value", Setup.VisualEffectsQuality);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "AnisotropyLevel"), "value", Setup.AnisotropyLevel);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "TexturesCompression"), "value", Setup.TexturesCompression);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "UseGLSL"), "value", Setup.UseGLSL);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "ShadowMap"), "value", Setup.ShadowMap);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MaxPointLights"), "value", Setup.MaxPointLights);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MusicSw"), "value", Setup.MusicSw);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "SoundSw"), "value", Setup.SoundSw);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "VoiceSw"), "value", Setup.VoiceSw);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "VSync"), "value", Setup.VSync);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "Brightness"), "value", Setup.Brightness);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "ShowFPS"), "value", Setup.ShowFPS);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "GameWeaponInfoType"), "value", Setup.GameWeaponInfoType);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "GameSpeed"), "value", Setup.GameSpeed);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "LoadingHint"), "value", Setup.LoadingHint);


	XMLdoc->AddComment(RootXMLEntry, " Control settings ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyboardDecreaseGameSpeed"), "value", vw_KeyboardCodeName(Setup.KeyboardDecreaseGameSpeed));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyboardResetGameSpeed"), "value", vw_KeyboardCodeName(Setup.KeyboardResetGameSpeed));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyboardIncreaseGameSpeed"), "value", vw_KeyboardCodeName(Setup.KeyboardIncreaseGameSpeed));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyboardGameWeaponInfoType"), "value", vw_KeyboardCodeName(Setup.KeyboardGameWeaponInfoType));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyboardPrimaryWeaponFireMode"), "value", vw_KeyboardCodeName(Setup.KeyboardPrimaryWeaponFireMode));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyboardSecondaryWeaponFireMode"), "value", vw_KeyboardCodeName(Setup.KeyboardSecondaryWeaponFireMode));

	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyBoardLeft"), "value", vw_KeyboardCodeName(Setup.KeyBoardLeft));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyBoardRight"), "value", vw_KeyboardCodeName(Setup.KeyBoardRight));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyBoardUp"), "value", vw_KeyboardCodeName(Setup.KeyBoardUp));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyBoardDown"), "value", vw_KeyboardCodeName(Setup.KeyBoardDown));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyBoardPrimary"), "value", vw_KeyboardCodeName(Setup.KeyBoardPrimary));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "KeyBoardSecondary"), "value", vw_KeyboardCodeName(Setup.KeyBoardSecondary));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MousePrimary"), "value", Setup.MousePrimary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MouseSecondary"), "value", Setup.MouseSecondary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "JoystickPrimary"), "value", Setup.JoystickPrimary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "JoystickSecondary"), "value", Setup.JoystickSecondary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "JoystickNum"), "value", Setup.JoystickNum);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "JoystickDeadZone"), "value", Setup.JoystickDeadZone);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "ControlSensivity"), "value", Setup.ControlSensivity);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MouseControl"), "value", Setup.MouseControl);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "LastProfile"), "value", Setup.LastProfile);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, "MenuScript"), "value", Setup.MenuScript);

	for(int i=0; i<10; i++)
	{
		char Name[128];
		sprintf(Name, "HintStatus%i", i+1);
		XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(RootXMLEntry, Name), "value", Setup.NeedShowHint[i]);
	}



	//
	// всегда последние
	//


	// упаковка таблицы рекордов
	int TopScoresDataSize = sizeof(sTopScores)*10;
	unsigned char *TopScoresData = new unsigned char[TopScoresDataSize];
	memcpy(TopScoresData, Setup.TopScores, TopScoresDataSize);

	// сразу архивируем Хаффманом
	BYTE *dstVFS;
	int dsizeVFS;
	vw_DATAtoHAFF(&dstVFS, (BYTE *)TopScoresData, &dsizeVFS, TopScoresDataSize);
	delete [] TopScoresData;
	TopScoresDataSize = dsizeVFS;
	TopScoresData = (unsigned char *)dstVFS;

	unsigned char *TopScoresDataXORCode = new unsigned char[TopScoresDataSize*3];
	char *TopScoresResultString = new char[TopScoresDataSize*4+1];


	{
		// XOR
		int k1;
		int k2;
		for (int i=0; i < TopScoresDataSize; i++)
		{
			k1 = i;
			k2 = TopScoresDataSize + i*2;
			TopScoresDataXORCode[k1] = 97 + (unsigned char)vw_iRandNum(25);
			TopScoresDataXORCode[k2] = 0;
			TopScoresDataXORCode[k2+1] = TopScoresData[i]^TopScoresDataXORCode[k1];
			// в первую - десятки, во вторую - еденицы
			TopScoresDataXORCode[k2] = 97 + (unsigned char)(TopScoresDataXORCode[k2+1]/10.0f);
			TopScoresDataXORCode[k2+1] = 97 + (TopScoresDataXORCode[k2+1] - (TopScoresDataXORCode[k2]-97)*10);
		}

		// чтобы разбить на блоки вставляем пробел
		// тогда красиво отображаться будет (если врапинг выставлен в редакторе)
		int k=0;
		int l=0;
		for (int i=0; i < TopScoresDataSize*3; i++)
		{
			TopScoresResultString[k] = TopScoresDataXORCode[i];
			k++;
			l++;
			if (l >= 125)
			{
				TopScoresResultString[k] = 0x20;
				k++;
				l=0;
			}
		}
		TopScoresResultString[k] = 0;
	}

	XMLdoc->AddEntryContent(XMLdoc->AddEntry(RootXMLEntry, "TopScores"), TopScoresResultString);

	if (TopScoresResultString != 0) delete [] TopScoresResultString;
	if (TopScoresData != 0) delete [] TopScoresData;
	if (TopScoresDataXORCode != 0) delete [] TopScoresDataXORCode;




	// упаковка профайлов

	{
		int ProfileDataSize = sizeof(GameProfile)*5;
		unsigned char *ProfileData = new unsigned char[ProfileDataSize];
		memcpy(ProfileData, Setup.Profile, ProfileDataSize);

		// сразу архивируем Хаффманом
		BYTE *dstVFS;
		int dsizeVFS;
		vw_DATAtoHAFF(&dstVFS, (BYTE *)ProfileData, &dsizeVFS, ProfileDataSize);
		delete [] ProfileData;
		ProfileDataSize = dsizeVFS;
		ProfileData = (unsigned char *)dstVFS;

		unsigned char *ProfileDataXORCode = new unsigned char[ProfileDataSize*3];
		char *ResultString = new char[ProfileDataSize*4+1];


		// XOR
		int k1;
		int k2;
		for (int i=0; i < ProfileDataSize; i++)
		{
			k1 = i;
			k2 = ProfileDataSize + i*2;
			ProfileDataXORCode[k1] = 97 + (unsigned char)vw_iRandNum(25);
			ProfileDataXORCode[k2] = 0;
			ProfileDataXORCode[k2+1] = ProfileData[i]^ProfileDataXORCode[k1];
			// в первую - десятки, во вторую - еденицы
			ProfileDataXORCode[k2] = 97 + (unsigned char)(ProfileDataXORCode[k2+1]/10.0f);
			ProfileDataXORCode[k2+1] = 97 + (ProfileDataXORCode[k2+1] - (ProfileDataXORCode[k2]-97)*10);
			//fprintf(stderr, "--%i %i %i %i\n", ProfileDataXORCode[k], ProfileDataXORCode[k+1],ProfileDataXORCode[k+2],ProfileDataXORCode[k+3]);
		}


		// чтобы разбить на блоки вставляем пробел
		// тогда красиво отображаться будет (если врапинг выставлен в редакторе)
		int k=0;
		int l=0;
		for (int i=0; i < ProfileDataSize*3; i++)
		{
			ResultString[k] = ProfileDataXORCode[i];
			k++;
			l++;
			if (l >= 125)
			{
				ResultString[k] = 0x20;
				k++;
				l=0;
			}
		}
		ResultString[k] = 0;

		XMLdoc->AddEntryContent(XMLdoc->AddEntry(RootXMLEntry, "PilotsProfiles"), ResultString);

		if (ResultString != 0) delete [] ResultString;
		if (ProfileData != 0) delete [] ProfileData;
		if (ProfileDataXORCode != 0) delete [] ProfileDataXORCode;
	}




	XMLdoc->Save(ConfigFileName);
	delete XMLdoc;
}













//-----------------------------------------------------------------------------
// считываем файл с настройками
//-----------------------------------------------------------------------------
bool LoadXMLSetupFile(bool NeedSafeMode)
{
	cXMLDocument *XMLdoc = new cXMLDocument;

	// устанавливаем базовые настройки
	InitSetup();


	// читаем данные
	if (!XMLdoc->Load(ConfigFileName))
	{
		delete XMLdoc;
		SaveXMLSetupFile();
		return true;
	}



	// берем первый элемент в скрипте
	cXMLEntry *RootXMLEntry = XMLdoc->RootXMLEntry;



	// дополнительная проверка на содержимое конфигурационного файла
	if (RootXMLEntry == 0)
	{
		fprintf(stderr, "Game configuration file corrupted: %s\n", ConfigFileName);
		// файл поврежден, надо завершить работу с ним
		delete XMLdoc;
		// сохранить дефолтные настройки, перезаписав файл
		SaveXMLSetupFile();
		// и сказать игре что это "первый запуск"
		return true;
	}
	if (strcmp("AstroMenaceSettings", RootXMLEntry->Name))
	{
		fprintf(stderr, "Game configuration file corrupted: %s\n", ConfigFileName);
		// файл поврежден, надо завершить работу с ним
		delete XMLdoc;
		// сохранить дефолтные настройки, перезаписав файл
		SaveXMLSetupFile();
		// и сказать игре что это "первый запуск"
		return true;
	}




	// если установлен NeedSafeMode, не грузим часть данных
	if (NeedSafeMode) goto LoadProfiles;


	if (XMLdoc->FindEntryByName(RootXMLEntry, "MenuLanguage") != 0)
	{
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MenuLanguage"), "value") != 0)
		{
			if (!strcmp(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MenuLanguage"), "value"), "en")) Setup.MenuLanguage = 1;
			else
			if (!strcmp(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MenuLanguage"), "value"), "de")) Setup.MenuLanguage = 2;
			else
			if (!strcmp(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MenuLanguage"), "value"), "ru")) Setup.MenuLanguage = 3;
		}
	}
	if (XMLdoc->FindEntryByName(RootXMLEntry, "VoiceLanguage") != 0)
	{
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VoiceLanguage"), "value") != 0)
		{
			if (!strcmp(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VoiceLanguage"), "value"), "en")) Setup.VoiceLanguage = 1;
			else
			if (!strcmp(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VoiceLanguage"), "value"), "de")) Setup.VoiceLanguage = 2;
			else
			if (!strcmp(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VoiceLanguage"), "value"), "ru")) Setup.VoiceLanguage = 3;
		}
	}
	if (XMLdoc->FindEntryByName(RootXMLEntry, "FontNumber") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "FontNumber"), "value") != 0)
			Setup.FontNumber = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "FontNumber"), "value");

	if (XMLdoc->FindEntryByName(RootXMLEntry, "Width") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "Width"), "value") != 0)
			Setup.Width = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "Width"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "Height") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "Height"), "value") != 0)
			Setup.Height = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "Height"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "BPP") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "BPP"), "value") != 0)
			Setup.BPP = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "BPP"), "value");

	if (XMLdoc->FindEntryByName(RootXMLEntry, "AspectRatio") != 0)
	{
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "AspectRatio"), "value") != 0)
		{
			if (!strcmp(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "AspectRatio"), "value"), "16:10"))
			{
				Setup.fAspectRatioWidth = 1228.0f;
				Setup.fAspectRatioHeight = 768.0f;
				Setup.iAspectRatioWidth = 1228;
				Setup.iAspectRatioHeight = 768;
			}
			else
			{
				Setup.fAspectRatioWidth = 1024.0f;
				Setup.fAspectRatioHeight = 768.0f;
				Setup.iAspectRatioWidth = 1024;
				Setup.iAspectRatioHeight = 768;
			}
		}
	}
	if (XMLdoc->FindEntryByName(RootXMLEntry, "CameraModeWithStandardAspectRatio") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "CameraModeWithStandardAspectRatio"), "value") != 0)
			Setup.CameraModeWithStandardAspectRatio = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "CameraModeWithStandardAspectRatio"), "value");

	if (XMLdoc->FindEntryByName(RootXMLEntry, "VBOCoreMode") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VBOCoreMode"), "value") != 0)
			Setup.VBOCoreMode = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VBOCoreMode"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "VAOCoreMode") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VAOCoreMode"), "value") != 0)
			Setup.VAOCoreMode = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VAOCoreMode"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "FBOCoreMode") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "FBOCoreMode"), "value") != 0)
			Setup.FBOCoreMode = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "FBOCoreMode"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "EqualOrMore128MBVideoRAM") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "EqualOrMore128MBVideoRAM"), "value") != 0)
			Setup.EqualOrMore128MBVideoRAM = XMLdoc->bGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "EqualOrMore128MBVideoRAM"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "HardwareMipMapGeneration") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "HardwareMipMapGeneration"), "value") != 0)
			Setup.HardwareMipMapGeneration = XMLdoc->bGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "HardwareMipMapGeneration"), "value");

	if (XMLdoc->FindEntryByName(RootXMLEntry, "TextureFilteringMode") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "TextureFilteringMode"), "value") != 0)
			Setup.TextureFilteringMode = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "TextureFilteringMode"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "TexturesQuality") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "TexturesQuality"), "value") != 0)
			Setup.TexturesQuality = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "TexturesQuality"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "MSAA") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MSAA"), "value") != 0)
			Setup.MSAA = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MSAA"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "CSAA") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "CSAA"), "value") != 0)
			Setup.CSAA = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "CSAA"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "VisualEffectsQuality") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VisualEffectsQuality"), "value") != 0)
			Setup.VisualEffectsQuality = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VisualEffectsQuality"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "AnisotropyLevel") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "AnisotropyLevel"), "value") != 0)
			Setup.AnisotropyLevel = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "AnisotropyLevel"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "TexturesCompression") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "TexturesCompression"), "value") != 0)
			Setup.TexturesCompression = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "TexturesCompression"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "UseGLSL") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "UseGLSL"), "value") != 0)
			Setup.UseGLSL = XMLdoc->bGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "UseGLSL"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "ShadowMap") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "ShadowMap"), "value") != 0)
			Setup.ShadowMap = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "ShadowMap"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "MaxPointLights") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MaxPointLights"), "value") != 0)
			Setup.MaxPointLights = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MaxPointLights"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "MusicSw") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MusicSw"), "value") != 0)
			Setup.MusicSw = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MusicSw"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "SoundSw") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "SoundSw"), "value") != 0)
			Setup.SoundSw = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "SoundSw"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "VoiceSw") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VoiceSw"), "value") != 0)
			Setup.VoiceSw = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VoiceSw"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "VSync") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VSync"), "value") != 0)
			Setup.VSync = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "VSync"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "Brightness") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "Brightness"), "value") != 0)
			Setup.Brightness = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "Brightness"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "ShowFPS") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "ShowFPS"), "value") != 0)
			Setup.ShowFPS = XMLdoc->bGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "ShowFPS"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "GameWeaponInfoType") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "GameWeaponInfoType"), "value") != 0)
			Setup.GameWeaponInfoType = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "GameWeaponInfoType"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "GameSpeed") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "GameSpeed"), "value") != 0)
			Setup.GameSpeed = XMLdoc->fGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "GameSpeed"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "LoadingHint") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "LoadingHint"), "value") != 0)
			Setup.LoadingHint = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "LoadingHint"), "value");



	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardDecreaseGameSpeed") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardDecreaseGameSpeed"), "value") != 0)
			Setup.KeyboardDecreaseGameSpeed = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardDecreaseGameSpeed"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardResetGameSpeed") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardResetGameSpeed"), "value") != 0)
			Setup.KeyboardResetGameSpeed = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardResetGameSpeed"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardIncreaseGameSpeed") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardIncreaseGameSpeed"), "value") != 0)
			Setup.KeyboardIncreaseGameSpeed = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardIncreaseGameSpeed"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardGameWeaponInfoType") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardGameWeaponInfoType"), "value") != 0)
			Setup.KeyboardGameWeaponInfoType = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardGameWeaponInfoType"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardPrimaryWeaponFireMode") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardPrimaryWeaponFireMode"), "value") != 0)
			Setup.KeyboardPrimaryWeaponFireMode = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardPrimaryWeaponFireMode"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardSecondaryWeaponFireMode") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardSecondaryWeaponFireMode"), "value") != 0)
			Setup.KeyboardSecondaryWeaponFireMode = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyboardSecondaryWeaponFireMode"), "value"));


	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardLeft") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardLeft"), "value") != 0)
			Setup.KeyBoardLeft = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardLeft"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardRight") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardRight"), "value") != 0)
			Setup.KeyBoardRight = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardRight"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardUp") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardUp"), "value") != 0)
			Setup.KeyBoardUp = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardUp"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardDown") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardDown"), "value") != 0)
			Setup.KeyBoardDown = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardDown"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardPrimary") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardPrimary"), "value") != 0)
			Setup.KeyBoardPrimary = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardPrimary"), "value"));
	if (XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardSecondary") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardSecondary"), "value") != 0)
			Setup.KeyBoardSecondary = vw_KeyboardNameCode(XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "KeyBoardSecondary"), "value"));

	if (XMLdoc->FindEntryByName(RootXMLEntry, "MousePrimary") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MousePrimary"), "value") != 0)
			Setup.MousePrimary = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MousePrimary"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "MouseSecondary") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MouseSecondary"), "value") != 0)
			Setup.MouseSecondary = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MouseSecondary"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "JoystickPrimary") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "JoystickPrimary"), "value") != 0)
			Setup.JoystickPrimary = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "JoystickPrimary"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "JoystickSecondary") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "JoystickSecondary"), "value") != 0)
			Setup.JoystickSecondary = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "JoystickSecondary"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "JoystickNum") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "JoystickNum"), "value") != 0)
			Setup.JoystickNum = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "JoystickNum"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "JoystickDeadZone") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "JoystickDeadZone"), "value") != 0)
			Setup.JoystickDeadZone = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "JoystickDeadZone"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "ControlSensivity") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "ControlSensivity"), "value") != 0)
			Setup.ControlSensivity = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "ControlSensivity"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "MouseControl") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MouseControl"), "value") != 0)
			Setup.MouseControl = XMLdoc->bGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MouseControl"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "LastProfile") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "LastProfile"), "value") != 0)
			Setup.LastProfile = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "LastProfile"), "value");
	if (XMLdoc->FindEntryByName(RootXMLEntry, "MenuScript") != 0)
		if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MenuScript"), "value") != 0)
			Setup.MenuScript = XMLdoc->iGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, "MenuScript"), "value");


	for(int i=0; i<10; i++)
	{
		char Name[128];
		sprintf(Name, "HintStatus%i", i+1);
		if (XMLdoc->FindEntryByName(RootXMLEntry, Name) != 0)
			if (XMLdoc->GetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, Name), "value") != 0)
				Setup.NeedShowHint[i] = XMLdoc->bGetEntryAttribute(XMLdoc->FindEntryByName(RootXMLEntry, Name), "value");
	}




	//
	// заполняем таблицу рекордов
	//

	if ((XMLdoc->FindEntryByName(RootXMLEntry, "TopScores") != 0) && (XMLdoc->FindEntryByName(RootXMLEntry, "TopScores")->Content != 0))
	{

		int TopScoresDataSize = strlen(XMLdoc->FindEntryByName(RootXMLEntry, "TopScores")->Content);
		unsigned char *TopScoresData = new unsigned char[TopScoresDataSize+1];
		unsigned char *TopScoresDataXORCode = new unsigned char[TopScoresDataSize+1];
		char *TopScoresResultString = new char[TopScoresDataSize+1];

		strcpy(TopScoresResultString, XMLdoc->FindEntryByName(RootXMLEntry, "TopScores")->Content);

		// первый цикл, восстанавливаем правильную последовательность, убираем все лишние элементы
		int k=0;
		for (int i=0; i<TopScoresDataSize; i++)
		{
			// берем только нужные символы
			if (TopScoresResultString[i] >= 97 && TopScoresResultString[i] <= 97+26)
			{
				TopScoresDataXORCode[k] = TopScoresResultString[i];
				k++;
			}
		}
		// находим правильное значение, т.к. загружали использую длину строки (!!!)
		TopScoresDataSize = k/3;

		// второй цикл, восстанавливаем последовательность структуры
		int k1;
		int k2;
		for (int i=0; i<TopScoresDataSize; i++)
		{
			k1 = i;
			k2 = TopScoresDataSize + i*2;

			unsigned char XORhash = TopScoresDataXORCode[k1];
			unsigned char XORdata = ((TopScoresDataXORCode[k2]-97)*10) + (TopScoresDataXORCode[k2+1]-97);

			TopScoresData[i] = XORdata^XORhash;
		}

		// третий цикл, распаковка Хаффмана
		BYTE *dstVFS;
		int dsizeVFS;
		vw_HAFFtoDATA(sizeof(sTopScores)*10, &dstVFS, (BYTE *)TopScoresData, &dsizeVFS, TopScoresDataSize);
		delete [] TopScoresData;
		TopScoresDataSize = dsizeVFS;
		TopScoresData = (unsigned char *)dstVFS;

		// переносим данные в структуру
		memcpy(Setup.TopScores, TopScoresData, TopScoresDataSize);

		if (TopScoresResultString != 0) delete [] TopScoresResultString;
		if (TopScoresData != 0) delete [] TopScoresData;
		if (TopScoresDataXORCode != 0) delete [] TopScoresDataXORCode;
	}





LoadProfiles:

	//
	// загрузка профайлов пилотов
	//

	if ((XMLdoc->FindEntryByName(RootXMLEntry, "PilotsProfiles") != 0) && (XMLdoc->FindEntryByName(RootXMLEntry, "PilotsProfiles")->Content != 0))
	{
		int ProfileDataSize = strlen(XMLdoc->FindEntryByName(RootXMLEntry, "PilotsProfiles")->Content);
		unsigned char *ProfileData = new unsigned char[ProfileDataSize+1];
		unsigned char *ProfileDataXORCode = new unsigned char[ProfileDataSize+1];
		char *ResultString = new char[ProfileDataSize+1];

		strcpy(ResultString, XMLdoc->FindEntryByName(RootXMLEntry, "PilotsProfiles")->Content);

		// первый цикл, восстанавливаем правильную последовательность, убираем все лишние элементы
		int k=0;
		for (unsigned int i=0; i<strlen(ResultString); i++)
		{
			// берем только нужные символы
			if (ResultString[i] >= 97 && ResultString[i] <= 97+26)
			{
				ProfileDataXORCode[k] = ResultString[i];
				k++;
			}
		}
		// находим правильное значение, т.к. загружали используя длину строки (!!!)
		ProfileDataSize = k/3;

		// второй цикл, восстанавливаем последовательность структуры
		int k1;
		int k2;
		for (int i=0; i<ProfileDataSize; i++)
		{
			k1 = i;
			k2 = ProfileDataSize + i*2;

			unsigned char XORhash = ProfileDataXORCode[k1];
			unsigned char XORdata = ((ProfileDataXORCode[k2]-97)*10) + (ProfileDataXORCode[k2+1]-97);

			ProfileData[i] = XORdata^XORhash;
		}

		// третий цикл, распаковка Хаффмана
		BYTE *dstVFS;
		int dsizeVFS;
		vw_HAFFtoDATA(sizeof(GameProfile)*5, &dstVFS, (BYTE *)ProfileData, &dsizeVFS, ProfileDataSize);
		delete [] ProfileData;
		ProfileDataSize = dsizeVFS;
		ProfileData = (unsigned char *)dstVFS;

		// переносим данные в структуру
		memcpy(Setup.Profile, ProfileData, ProfileDataSize);

		if (ResultString != 0) delete [] ResultString;
		if (ProfileData != 0) delete [] ProfileData;
		if (ProfileDataXORCode != 0) delete [] ProfileDataXORCode;
	}







	//
	// делаем изменения, проверки и т.п.
	//

	if (Setup.KeyBoardUp == 0) Setup.KeyBoardUp = SDLK_UP;
	if (Setup.KeyBoardDown == 0) Setup.KeyBoardDown = SDLK_DOWN;
	if (Setup.KeyBoardLeft == 0) Setup.KeyBoardLeft = SDLK_LEFT;
	if (Setup.KeyBoardRight == 0) Setup.KeyBoardRight = SDLK_RIGHT;
	if (Setup.KeyBoardPrimary == 0) Setup.KeyBoardPrimary = SDLK_LCTRL;
	if (Setup.KeyBoardSecondary == 0) Setup.KeyBoardSecondary = SDLK_SPACE;
	if (Setup.MousePrimary == 0) Setup.MousePrimary = SDL_BUTTON_LEFT;
	if (Setup.MouseSecondary == 0) Setup.MouseSecondary = SDL_BUTTON_RIGHT;
	if (Setup.JoystickPrimary == -1) Setup.JoystickPrimary = 0;
	if (Setup.JoystickSecondary == -1) Setup.JoystickSecondary = 1;

	CurrentProfile = Setup.LastProfile;
	if (CurrentProfile != -1) CurrentMission = Setup.Profile[Setup.LastProfile].LastMission;

	if ((Setup.FontNumber > FontQuantity-1) || (Setup.FontNumber < 0)) Setup.FontNumber = 0;
	if (Setup.ControlSensivity > 10) Setup.ControlSensivity = 10;
	if (Setup.Brightness > 10) Setup.Brightness = 10;
	if (Setup.JoystickDeadZone > 10) Setup.JoystickDeadZone = 10;


	delete XMLdoc;

	return false;
}



