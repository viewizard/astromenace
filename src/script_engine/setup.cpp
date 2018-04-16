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
	strcpy(Setup.FontName, default_font_family);
	Setup.FontSize = 16;

	Setup.Width = 1228;
	Setup.Height = 768;
	Setup.BPP = 0;
	Setup.fAspectRatioWidth = 1228.0f;
	Setup.fAspectRatioHeight = 768.0f;
	Setup.iAspectRatioWidth = 1228;
	Setup.iAspectRatioHeight = 768;

	Setup.CameraModeWithStandardAspectRatio = 0;

	Setup.TexturesQuality = 3;
	Setup.MSAA = 0;
	Setup.CSAA = 0;
	Setup.VisualEffectsQuality = 0;
	Setup.AnisotropyLevel = 1;
	Setup.TexturesCompressionType = 1;
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
}


//-----------------------------------------------------------------------------
// записываем файл с настройками
//-----------------------------------------------------------------------------
void SaveXMLSetupFile()
{
	cXMLDocument *XMLdoc = new cXMLDocument;

	sXMLEntry *RootXMLEntry = XMLdoc->CreateRootEntry("AstroMenaceSettings");

	if (!RootXMLEntry) {
		std::cerr << __func__ << "(): " << "Can't create XML root element.\n";
		delete XMLdoc;
		return;
	}

	XMLdoc->AddComment(*RootXMLEntry, " AstroMenace game Settings ");



	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "GAME_BUILD"), "value", GAME_BUILD);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MenuLanguage"), "value", vw_GetText("0_code", Setup.MenuLanguage));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "VoiceLanguage"), "value", vw_GetText("0_code", Setup.VoiceLanguage));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "FontNumber"), "value", Setup.FontNumber);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "FontName"), "value", Setup.FontName);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "FontSize"), "value", Setup.FontSize);


	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "Width"), "value", Setup.Width);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "Height"), "value", Setup.Height);
	XMLdoc->AddComment(*RootXMLEntry, " Window (BPP = 0) or Full Screen (BPP = 16, 24 or 32) ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "BPP"), "value", Setup.BPP);
	XMLdoc->AddComment(*RootXMLEntry, " Aspect Ratio must be 4:3 or 16:10 ");
	if (Setup.iAspectRatioWidth == 1024)
		XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "AspectRatio"), "value", "4:3");
	else
		XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "AspectRatio"), "value", "16:10");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "CameraModeWithStandardAspectRatio"), "value", Setup.CameraModeWithStandardAspectRatio);

	XMLdoc->AddComment(*RootXMLEntry, " Common settings ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "TexturesQuality"), "value", Setup.TexturesQuality);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MSAA"), "value", Setup.MSAA);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "CSAA"), "value", Setup.CSAA);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "VisualEffectsQuality"), "value", Setup.VisualEffectsQuality);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "AnisotropyLevel"), "value", Setup.AnisotropyLevel);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "TexturesCompressionType"), "value", Setup.TexturesCompressionType);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "UseGLSL"), "value", Setup.UseGLSL);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "ShadowMap"), "value", Setup.ShadowMap);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MaxPointLights"), "value", Setup.MaxPointLights);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MusicSw"), "value", Setup.MusicSw);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "SoundSw"), "value", Setup.SoundSw);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "VoiceSw"), "value", Setup.VoiceSw);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "VSync"), "value", Setup.VSync);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "Brightness"), "value", Setup.Brightness);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "ShowFPS"), "value", Setup.ShowFPS);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "GameWeaponInfoType"), "value", Setup.GameWeaponInfoType);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "GameSpeed"), "value", Setup.GameSpeed);


	XMLdoc->AddComment(*RootXMLEntry, " Control settings ");
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyboardDecreaseGameSpeed"), "value", SDL_GetKeyName(Setup.KeyboardDecreaseGameSpeed));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyboardResetGameSpeed"), "value", SDL_GetKeyName(Setup.KeyboardResetGameSpeed));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyboardIncreaseGameSpeed"), "value", SDL_GetKeyName(Setup.KeyboardIncreaseGameSpeed));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyboardGameWeaponInfoType"), "value", SDL_GetKeyName(Setup.KeyboardGameWeaponInfoType));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyboardPrimaryWeaponFireMode"), "value", SDL_GetKeyName(Setup.KeyboardPrimaryWeaponFireMode));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyboardSecondaryWeaponFireMode"), "value", SDL_GetKeyName(Setup.KeyboardSecondaryWeaponFireMode));

	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardLeft"), "value", SDL_GetKeyName(Setup.KeyBoardLeft));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardRight"), "value", SDL_GetKeyName(Setup.KeyBoardRight));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardUp"), "value", SDL_GetKeyName(Setup.KeyBoardUp));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardDown"), "value", SDL_GetKeyName(Setup.KeyBoardDown));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardPrimary"), "value", SDL_GetKeyName(Setup.KeyBoardPrimary));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "KeyBoardSecondary"), "value", SDL_GetKeyName(Setup.KeyBoardSecondary));
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MousePrimary"), "value", Setup.MousePrimary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MouseSecondary"), "value", Setup.MouseSecondary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "JoystickPrimary"), "value", Setup.JoystickPrimary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "JoystickSecondary"), "value", Setup.JoystickSecondary);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "JoystickNum"), "value", Setup.JoystickNum);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "JoystickDeadZone"), "value", Setup.JoystickDeadZone);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "ControlSensivity"), "value", Setup.ControlSensivity);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MouseControl"), "value", Setup.MouseControl);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "LastProfile"), "value", Setup.LastProfile);
	XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, "MenuScript"), "value", Setup.MenuScript);

	for(int i=0; i<10; i++) {
		std::string tmpString{"HintStatus" + std::to_string(i + 1)};
		XMLdoc->AddEntryAttribute(XMLdoc->AddEntry(*RootXMLEntry, tmpString.c_str()), "value", Setup.NeedShowHint[i]);
	}



	//
	// всегда последние
	//


	// упаковка таблицы рекордов
	int TopScoresDataSize = sizeof(sTopScores)*10;
	unsigned char *TopScoresData = new unsigned char[TopScoresDataSize];
	memcpy(TopScoresData, Setup.TopScores, TopScoresDataSize);

	unsigned char *TopScoresDataXORCode = new unsigned char[TopScoresDataSize*3];
	char *TopScoresResultString = new char[TopScoresDataSize*4+1];



	// XOR
	for (int i=0; i < TopScoresDataSize; i++) {
		int k1 = i;
		int k2 = TopScoresDataSize + i*2;
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
	int j=0;
	for (int i=0; i < TopScoresDataSize*3; i++) {
		TopScoresResultString[k] = TopScoresDataXORCode[i];
		k++;
		j++;
		if (j >= 125) {
			TopScoresResultString[k] = 0x20;
			k++;
			j=0;
		}
	}
	TopScoresResultString[k] = 0;


	XMLdoc->AddEntryContent(XMLdoc->AddEntry(*RootXMLEntry, "TopScores"), TopScoresResultString);

	if (TopScoresResultString != nullptr)
		delete [] TopScoresResultString;
	if (TopScoresData != nullptr)
		delete [] TopScoresData;
	if (TopScoresDataXORCode != nullptr)
		delete [] TopScoresDataXORCode;


	// упаковка профайлов


	int ProfileDataSize = sizeof(sPilotProfile)*5;
	unsigned char *ProfileData = new unsigned char[ProfileDataSize];
	memcpy(ProfileData, Setup.Profile, ProfileDataSize);

	unsigned char *ProfileDataXORCode = new unsigned char[ProfileDataSize*3];
	char *ResultString = new char[ProfileDataSize*4+1];


	// XOR
	for (int i=0; i < ProfileDataSize; i++) {
		int k1 = i;
		int k2 = ProfileDataSize + i*2;
		ProfileDataXORCode[k1] = 97 + (unsigned char)vw_iRandNum(25);
		ProfileDataXORCode[k2] = 0;
		ProfileDataXORCode[k2+1] = ProfileData[i]^ProfileDataXORCode[k1];
		// в первую - десятки, во вторую - еденицы
		ProfileDataXORCode[k2] = 97 + (unsigned char)(ProfileDataXORCode[k2+1]/10.0f);
		ProfileDataXORCode[k2+1] = 97 + (ProfileDataXORCode[k2+1] - (ProfileDataXORCode[k2]-97)*10);
	}


	// чтобы разбить на блоки вставляем пробел
	// тогда красиво отображаться будет (если врапинг выставлен в редакторе)
	k=0;
	j=0;
	for (int i=0; i < ProfileDataSize*3; i++) {
		ResultString[k] = ProfileDataXORCode[i];
		k++;
		j++;
		if (j >= 125) {
			ResultString[k] = 0x20;
			k++;
			j=0;
		}
	}
	ResultString[k] = 0;

	XMLdoc->AddEntryContent(XMLdoc->AddEntry(*RootXMLEntry, "PilotsProfiles"), ResultString);

	delete [] ResultString;
	if (ProfileData != nullptr)
		delete [] ProfileData;
	if (ProfileDataXORCode != nullptr)
		delete [] ProfileDataXORCode;


	XMLdoc->Save(ConfigFileName);
	delete XMLdoc;
}

//-----------------------------------------------------------------------------
// считываем файл с настройками
//-----------------------------------------------------------------------------
bool LoadXMLSetupFile(bool NeedSafeMode)
{
	// устанавливаем базовые настройки
	InitSetup();

	cXMLDocument *XMLdoc = new cXMLDocument(ConfigFileName);

	// читаем данные
	if (!XMLdoc->GetRootEntry()) {
		delete XMLdoc;
		SaveXMLSetupFile();
		return true;
	}

	// берем первый элемент в скрипте
	sXMLEntry *RootXMLEntry = XMLdoc->GetRootEntry();

	// дополнительная проверка на содержимое конфигурационного файла
	if (!RootXMLEntry) {
		std::cerr << __func__ << "(): " << "Game configuration file corrupted: " << ConfigFileName << "\n";
		// файл поврежден, надо завершить работу с ним
		delete XMLdoc;
		// сохранить дефолтные настройки, перезаписав файл
		SaveXMLSetupFile();
		// и сказать игре что это "первый запуск"
		return true;
	}
	if ("AstroMenaceSettings" != RootXMLEntry->Name) {
		std::cerr << __func__ << "(): " << "Game configuration file corrupted: " << ConfigFileName << "\n";
		// файл поврежден, надо завершить работу с ним
		delete XMLdoc;
		// сохранить дефолтные настройки, перезаписав файл
		SaveXMLSetupFile();
		// и сказать игре что это "первый запуск"
		return true;
	}

	// если установлен NeedSafeMode, не грузим часть данных
	if (NeedSafeMode)
		goto LoadProfiles;

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MenuLanguage")) {
		std::string tmpMenuLanguage{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MenuLanguage"), "value", tmpMenuLanguage)) {
			for (unsigned int i=0; i<vw_GetLanguageListCount(); i++) {
				if (tmpMenuLanguage == vw_GetText("0_code", i + 1/*first column contain index, not data*/)) {
					Setup.MenuLanguage = i+1;
					break;
				}
			}
		}
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "VoiceLanguage")) {
		std::string tmpVoiceLanguage{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "VoiceLanguage"), "value", tmpVoiceLanguage)) {
			for (unsigned int i=0; i<vw_GetLanguageListCount(); i++) {
				if (tmpVoiceLanguage == vw_GetText("0_code", i + 1/*first column contain index, not data*/)) {
					Setup.VoiceLanguage = i+1;
					break;
				}
			}
		}
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "FontNumber"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "FontNumber"), "value", Setup.FontNumber);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "FontName")) {
		std::string tmpFontName{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "FontName"), "value", tmpFontName))
			strcpy(Setup.FontName, tmpFontName.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "FontSize"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "FontSize"), "value", Setup.FontSize);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "Width"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "Width"), "value", Setup.Width);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "Height"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "Height"), "value", Setup.Height);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "BPP"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "BPP"), "value", Setup.BPP);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "AspectRatio")) {
		std::string tmpAspectRatio{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "AspectRatio"), "value", tmpAspectRatio)) {
			if (tmpAspectRatio == "16:10") {
				Setup.fAspectRatioWidth = 1228.0f;
				Setup.fAspectRatioHeight = 768.0f;
				Setup.iAspectRatioWidth = 1228;
				Setup.iAspectRatioHeight = 768;
			} else {
				Setup.fAspectRatioWidth = 1024.0f;
				Setup.fAspectRatioHeight = 768.0f;
				Setup.iAspectRatioWidth = 1024;
				Setup.iAspectRatioHeight = 768;
			}
		}
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "CameraModeWithStandardAspectRatio"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "CameraModeWithStandardAspectRatio"), "value", Setup.CameraModeWithStandardAspectRatio);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "TexturesQuality"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "TexturesQuality"), "value", Setup.TexturesQuality);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MSAA"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MSAA"), "value", Setup.MSAA);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "CSAA"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "CSAA"), "value", Setup.CSAA);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "VisualEffectsQuality"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "VisualEffectsQuality"), "value", Setup.VisualEffectsQuality);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "AnisotropyLevel"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "AnisotropyLevel"), "value", Setup.AnisotropyLevel);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "TexturesCompressionType"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "TexturesCompressionType"), "value", Setup.TexturesCompressionType);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "UseGLSL"))
		XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "UseGLSL"), "value", Setup.UseGLSL);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "ShadowMap"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "ShadowMap"), "value", Setup.ShadowMap);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MaxPointLights"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MaxPointLights"), "value", Setup.MaxPointLights);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MusicSw"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MusicSw"), "value", Setup.MusicSw);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "SoundSw"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "SoundSw"), "value", Setup.SoundSw);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "VoiceSw"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "VoiceSw"), "value", Setup.VoiceSw);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "VSync"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "VSync"), "value", Setup.VSync);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "Brightness"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "Brightness"), "value", Setup.Brightness);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "ShowFPS"))
		XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "ShowFPS"), "value", Setup.ShowFPS);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "GameWeaponInfoType"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "GameWeaponInfoType"), "value", Setup.GameWeaponInfoType);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "GameSpeed"))
		XMLdoc->fGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "GameSpeed"), "value", Setup.GameSpeed);



	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardDecreaseGameSpeed")) {
		std::string tmpKeyboardDecreaseGameSpeed{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardDecreaseGameSpeed"), "value", tmpKeyboardDecreaseGameSpeed))
			Setup.KeyboardDecreaseGameSpeed = SDL_GetKeyFromName(tmpKeyboardDecreaseGameSpeed.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardResetGameSpeed")) {
		std::string tmpKeyboardResetGameSpeed{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardResetGameSpeed"), "value", tmpKeyboardResetGameSpeed))
			Setup.KeyboardResetGameSpeed = SDL_GetKeyFromName(tmpKeyboardResetGameSpeed.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardIncreaseGameSpeed")) {
		std::string tmpKeyboardIncreaseGameSpeed{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardIncreaseGameSpeed"), "value", tmpKeyboardIncreaseGameSpeed))
			Setup.KeyboardIncreaseGameSpeed = SDL_GetKeyFromName(tmpKeyboardIncreaseGameSpeed.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardGameWeaponInfoType")) {
		std::string tmpKeyboardGameWeaponInfoType{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardGameWeaponInfoType"), "value", tmpKeyboardGameWeaponInfoType))
			Setup.KeyboardGameWeaponInfoType = SDL_GetKeyFromName(tmpKeyboardGameWeaponInfoType.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardPrimaryWeaponFireMode")) {
		std::string tmpKeyboardPrimaryWeaponFireMode{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardPrimaryWeaponFireMode"), "value", tmpKeyboardPrimaryWeaponFireMode))
			Setup.KeyboardPrimaryWeaponFireMode = SDL_GetKeyFromName(tmpKeyboardPrimaryWeaponFireMode.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardSecondaryWeaponFireMode")) {
		std::string tmpKeyboardSecondaryWeaponFireMode{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyboardSecondaryWeaponFireMode"), "value", tmpKeyboardSecondaryWeaponFireMode))
			Setup.KeyboardSecondaryWeaponFireMode = SDL_GetKeyFromName(tmpKeyboardSecondaryWeaponFireMode.c_str());
	}


	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardLeft")) {
		std::string tmpKeyBoardLeft{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardLeft"), "value", tmpKeyBoardLeft))
			Setup.KeyBoardLeft = SDL_GetKeyFromName(tmpKeyBoardLeft.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardRight")) {
		std::string tmpKeyBoardRight{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardRight"), "value", tmpKeyBoardRight))
			Setup.KeyBoardRight = SDL_GetKeyFromName(tmpKeyBoardRight.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardUp")) {
		std::string tmpKeyBoardUp{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardUp"), "value", tmpKeyBoardUp))
			Setup.KeyBoardUp = SDL_GetKeyFromName(tmpKeyBoardUp.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardDown")) {
		std::string tmpKeyBoardDown{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardDown"), "value", tmpKeyBoardDown))
			Setup.KeyBoardDown = SDL_GetKeyFromName(tmpKeyBoardDown.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardPrimary")) {
		std::string tmpKeyBoardPrimary{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardPrimary"), "value", tmpKeyBoardPrimary))
			Setup.KeyBoardPrimary = SDL_GetKeyFromName(tmpKeyBoardPrimary.c_str());
	}
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardSecondary")) {
		std::string tmpKeyBoardSecondary{};
		if (XMLdoc->GetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "KeyBoardSecondary"), "value", tmpKeyBoardSecondary))
			Setup.KeyBoardSecondary = SDL_GetKeyFromName(tmpKeyBoardSecondary.c_str());
	}

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MousePrimary"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MousePrimary"), "value", Setup.MousePrimary);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MouseSecondary"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MouseSecondary"), "value", Setup.MouseSecondary);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickPrimary"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickPrimary"), "value", Setup.JoystickPrimary);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickSecondary"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickSecondary"), "value", Setup.JoystickSecondary);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickNum"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickNum"), "value", Setup.JoystickNum);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickDeadZone"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "JoystickDeadZone"), "value", Setup.JoystickDeadZone);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "ControlSensivity"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "ControlSensivity"), "value", Setup.ControlSensivity);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MouseControl"))
		XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MouseControl"), "value", Setup.MouseControl);

	if (XMLdoc->FindEntryByName(*RootXMLEntry, "LastProfile"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "LastProfile"), "value", Setup.LastProfile);
	if (XMLdoc->FindEntryByName(*RootXMLEntry, "MenuScript"))
		XMLdoc->iGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, "MenuScript"), "value", Setup.MenuScript);

	for(int i=0; i<10; i++) {
		std::string tmpString{"HintStatus" + std::to_string(i + 1)};
		if (XMLdoc->FindEntryByName(*RootXMLEntry, tmpString.c_str()))
			 XMLdoc->bGetEntryAttribute(*XMLdoc->FindEntryByName(*RootXMLEntry, tmpString.c_str()), "value", Setup.NeedShowHint[i]);
	}

	//
	// заполняем таблицу рекордов
	//

	if ((XMLdoc->FindEntryByName(*RootXMLEntry, "TopScores")) &&
	    !XMLdoc->FindEntryByName(*RootXMLEntry, "TopScores")->Content.empty()) {

		int TopScoresDataSize = XMLdoc->FindEntryByName(*RootXMLEntry, "TopScores")->Content.size();
		unsigned char *TopScoresData = new unsigned char[TopScoresDataSize+1];
		unsigned char *TopScoresDataXORCode = new unsigned char[TopScoresDataSize+1];
		char *TopScoresResultString = new char[TopScoresDataSize+1];

		strcpy(TopScoresResultString, XMLdoc->FindEntryByName(*RootXMLEntry, "TopScores")->Content.c_str());

		// первый цикл, восстанавливаем правильную последовательность, убираем все лишние элементы
		int k=0;
		for (int i=0; i<TopScoresDataSize; i++) {
			// берем только нужные символы
			if (TopScoresResultString[i] >= 97 && TopScoresResultString[i] <= 97+26) {
				TopScoresDataXORCode[k] = TopScoresResultString[i];
				k++;
			}
		}
		// находим правильное значение, т.к. загружали использую длину строки (!!!)
		TopScoresDataSize = k/3;

		// второй цикл, восстанавливаем последовательность структуры
		for (int i=0; i<TopScoresDataSize; i++) {
			int k1 = i;
			int k2 = TopScoresDataSize + i*2;

			unsigned char XORhash = TopScoresDataXORCode[k1];
			unsigned char XORdata = ((TopScoresDataXORCode[k2]-97)*10) + (TopScoresDataXORCode[k2+1]-97);

			TopScoresData[i] = XORdata^XORhash;
		}

		// переносим данные в структуру
		memcpy(Setup.TopScores, TopScoresData, TopScoresDataSize);

		if (TopScoresResultString != nullptr)
			delete [] TopScoresResultString;
		if (TopScoresData != nullptr)
			delete [] TopScoresData;
		if (TopScoresDataXORCode != nullptr)
			delete [] TopScoresDataXORCode;
	}


LoadProfiles:

	//
	// загрузка профайлов пилотов
	//

	if ((XMLdoc->FindEntryByName(*RootXMLEntry, "PilotsProfiles") != nullptr) &&
	    !XMLdoc->FindEntryByName(*RootXMLEntry, "PilotsProfiles")->Content.empty()) {
		int ProfileDataSize = XMLdoc->FindEntryByName(*RootXMLEntry, "PilotsProfiles")->Content.size();
		unsigned char *ProfileData = new unsigned char[ProfileDataSize+1];
		unsigned char *ProfileDataXORCode = new unsigned char[ProfileDataSize+1];
		char *ResultString = new char[ProfileDataSize+1];

		strcpy(ResultString, XMLdoc->FindEntryByName(*RootXMLEntry, "PilotsProfiles")->Content.c_str());

		// первый цикл, восстанавливаем правильную последовательность, убираем все лишние элементы
		int k=0;
		for (unsigned int i=0; i<strlen(ResultString); i++) {
			// берем только нужные символы
			if (ResultString[i] >= 97 && ResultString[i] <= 97+26) {
				ProfileDataXORCode[k] = ResultString[i];
				k++;
			}
		}
		// находим правильное значение, т.к. загружали используя длину строки (!!!)
		ProfileDataSize = k/3;

		// второй цикл, восстанавливаем последовательность структуры
		for (int i=0; i<ProfileDataSize; i++) {
			int k1 = i;
			int k2 = ProfileDataSize + i*2;

			unsigned char XORhash = ProfileDataXORCode[k1];
			unsigned char XORdata = ((ProfileDataXORCode[k2]-97)*10) + (ProfileDataXORCode[k2+1]-97);

			ProfileData[i] = XORdata^XORhash;
		}

		// переносим данные в структуру
		memcpy(Setup.Profile, ProfileData, ProfileDataSize);

		if (ResultString != nullptr)
			delete [] ResultString;
		if (ProfileData != nullptr)
			delete [] ProfileData;
		if (ProfileDataXORCode != nullptr)
			delete [] ProfileDataXORCode;
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

	if ((Setup.LastProfile >= 0) &&
	    (Setup.LastProfile <= 4) &&
	    Setup.Profile[Setup.LastProfile].Used)
		CurrentProfile = Setup.LastProfile;
	if (CurrentProfile != -1) CurrentMission = Setup.Profile[Setup.LastProfile].LastMission;

	if ((Setup.FontNumber > FontQuantity-1) || (Setup.FontNumber < 0)) Setup.FontNumber = 0;
	if ((Setup.FontSize > 18) || (Setup.FontSize < 14)) Setup.FontSize = 16;
	if (Setup.ControlSensivity > 10) Setup.ControlSensivity = 10;
	if (Setup.Brightness > 10) Setup.Brightness = 10;
	if (Setup.JoystickDeadZone > 10) Setup.JoystickDeadZone = 10;

	delete XMLdoc;

	return false;
}
