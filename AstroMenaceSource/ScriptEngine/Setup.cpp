/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Setup.cpp

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
	if ((Setup.Width*1.0f)/(Setup.Height*1.0f) < 1.4f)
	{
		Setup.fAspectRatioWidth = 1024.0f;
		Setup.fAspectRatioHeight = 768.0f;
		Setup.iAspectRatioWidth = 1024;
		Setup.iAspectRatioHeight = 768;
	}
	else
	{
		Setup.fAspectRatioWidth = 1228.0f;
		Setup.fAspectRatioHeight = 768.0f;
		Setup.iAspectRatioWidth = 1228;
		Setup.iAspectRatioHeight = 768;
	}
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
	Setup.Gamma = 5;
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

	Setup.ControlSensivity = 5;
	Setup.MouseControl = true;
	Setup.LastProfile = -1;


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
void iAddLine(TiXmlElement * root, TiXmlElement * Element, const char *Name, const char *Attrib, int Data)
{
	char buffer[1024];
	Element = root->FirstChildElement(Name);
	if (Element == 0)
	{
		Element = new TiXmlElement(Name);
		root->LinkEndChild(Element);
	}
	sprintf(buffer, "%i", Data);
	Element->SetAttribute(Attrib, buffer);
}
void fAddLine(TiXmlElement * root, TiXmlElement * Element, const char *Name, const char *Attrib, float Data)
{
	char buffer[1024];
	Element = root->FirstChildElement(Name);
	if (Element == 0)
	{
		Element = new TiXmlElement(Name);
		root->LinkEndChild(Element);
	}
	sprintf(buffer, "%f", Data);
	Element->SetAttribute(Attrib, buffer);
}
void bAddLine(TiXmlElement * root, TiXmlElement * Element, const char *Name, const char *Attrib, bool Data)
{
	Element = root->FirstChildElement(Name);
	if (Element == 0)
	{
		Element = new TiXmlElement(Name);
		root->LinkEndChild(Element);
	}
	if (Data)
		Element->SetAttribute(Attrib, "on");
	else
		Element->SetAttribute(Attrib, "off");
}
void sAddLine(TiXmlElement * root, TiXmlElement * Element, const char *Name, const char *Attrib, const char *Data)
{
	Element = root->FirstChildElement(Name);
	if (Element == 0)
	{
		Element = new TiXmlElement(Name);
		root->LinkEndChild(Element);
	}
	Element->SetAttribute(Attrib, Data);
}
void AddComment(TiXmlElement * root, const char *Comment)
{
	TiXmlComment *comment = new TiXmlComment();
	comment->SetValue(Comment);
	root->LinkEndChild(comment);
}
void SaveXMLSetupFile()
{
	TiXmlDocument doc;
	TiXmlElement * root = 0;
	TiXmlElement * setting = 0;



	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	root = new TiXmlElement("AstroMenaceSettings");
	doc.LinkEndChild( root );

	AddComment(root, " AstroMenace game Settings");




	iAddLine(root, setting, "GAME_BUILD", "value", GAME_BUILD);

	switch (Setup.MenuLanguage)
	{
		case 1: sAddLine(root, setting, "MenuLanguage", "value", "en"); break;
		case 2: sAddLine(root, setting, "MenuLanguage", "value", "de"); break;
		case 3: sAddLine(root, setting, "MenuLanguage", "value", "ru"); break;
	}
	switch (Setup.VoiceLanguage)
	{
		case 1: sAddLine(root, setting, "VoiceLanguage", "value", "en"); break;
		case 2: sAddLine(root, setting, "VoiceLanguage", "value", "de"); break;
		case 3: sAddLine(root, setting, "VoiceLanguage", "value", "ru"); break;
	}
	iAddLine(root, setting, "FontNumber", "value", Setup.FontNumber);

	iAddLine(root, setting, "Width", "value", Setup.Width);
	iAddLine(root, setting, "Height", "value", Setup.Height);
	AddComment(root, " Windows (BPP = 0) or Full Screen (BPP = 16 or 32) ");
	iAddLine(root, setting, "BPP", "value", Setup.BPP);
	AddComment(root, " Aspect Ratio must be 4:3 or 16:10 ");
	if (Setup.iAspectRatioWidth == 1024)
		sAddLine(root, setting, "AspectRatio", "value", "4:3");
	else
		sAddLine(root, setting, "AspectRatio", "value", "16:10");
	iAddLine(root, setting, "CameraModeWithStandardAspectRatio", "value", Setup.CameraModeWithStandardAspectRatio);


	AddComment(root, " Don't change this setting unless you know what you are doing ");
	iAddLine(root, setting, "VBOCoreMode", "value", Setup.VBOCoreMode);
	iAddLine(root, setting, "VAOCoreMode", "value", Setup.VAOCoreMode);
	iAddLine(root, setting, "FBOCoreMode", "value", Setup.FBOCoreMode);
	AddComment(root, " If your video card have 128+ MB VRAM on board - turn it on ");
	bAddLine(root, setting, "EqualOrMore128MBVideoRAM", "value", Setup.EqualOrMore128MBVideoRAM);
	AddComment(root, " Don't change this setting unless you know what you are doing ");
	bAddLine(root, setting, "HardwareMipMapGeneration", "value", Setup.HardwareMipMapGeneration);


	AddComment(root, " Common settings ");
	iAddLine(root, setting, "TextureFilteringMode", "value", Setup.TextureFilteringMode);
	iAddLine(root, setting, "TexturesQuality", "value", Setup.TexturesQuality);
	iAddLine(root, setting, "MSAA", "value", Setup.MSAA);
	iAddLine(root, setting, "CSAA", "value", Setup.CSAA);
	iAddLine(root, setting, "VisualEffectsQuality", "value", Setup.VisualEffectsQuality);
	iAddLine(root, setting, "AnisotropyLevel", "value", Setup.AnisotropyLevel);
	iAddLine(root, setting, "TexturesCompression", "value", Setup.TexturesCompression);
	bAddLine(root, setting, "UseGLSL", "value", Setup.UseGLSL);
	iAddLine(root, setting, "ShadowMap", "value", Setup.ShadowMap);
	iAddLine(root, setting, "MaxPointLights", "value", Setup.MaxPointLights);
	iAddLine(root, setting, "MusicSw", "value", Setup.MusicSw);
	iAddLine(root, setting, "SoundSw", "value", Setup.SoundSw);
	iAddLine(root, setting, "VoiceSw", "value", Setup.VoiceSw);
	iAddLine(root, setting, "Gamma", "value", Setup.Gamma);
	bAddLine(root, setting, "ShowFPS", "value", Setup.ShowFPS);
	iAddLine(root, setting, "GameWeaponInfoType", "value", Setup.GameWeaponInfoType);
	fAddLine(root, setting, "GameSpeed", "value", Setup.GameSpeed);
	iAddLine(root, setting, "LoadingHint", "value", Setup.LoadingHint);


	AddComment(root, " Control settings ");
	sAddLine(root, setting, "KeyboardDecreaseGameSpeed", "value", vw_KeyboardCodeName(Setup.KeyboardDecreaseGameSpeed));
	sAddLine(root, setting, "KeyboardResetGameSpeed", "value", vw_KeyboardCodeName(Setup.KeyboardResetGameSpeed));
	sAddLine(root, setting, "KeyboardResetGameSpeed", "value", vw_KeyboardCodeName(Setup.KeyboardResetGameSpeed));
	sAddLine(root, setting, "KeyboardIncreaseGameSpeed", "value", vw_KeyboardCodeName(Setup.KeyboardIncreaseGameSpeed));
	sAddLine(root, setting, "KeyboardGameWeaponInfoType", "value", vw_KeyboardCodeName(Setup.KeyboardGameWeaponInfoType));
	sAddLine(root, setting, "KeyboardPrimaryWeaponFireMode", "value", vw_KeyboardCodeName(Setup.KeyboardPrimaryWeaponFireMode));
	sAddLine(root, setting, "KeyboardSecondaryWeaponFireMode", "value", vw_KeyboardCodeName(Setup.KeyboardSecondaryWeaponFireMode));

	sAddLine(root, setting, "KeyBoardLeft", "value", vw_KeyboardCodeName(Setup.KeyBoardLeft));
	sAddLine(root, setting, "KeyBoardRight", "value", vw_KeyboardCodeName(Setup.KeyBoardRight));
	sAddLine(root, setting, "KeyBoardUp", "value", vw_KeyboardCodeName(Setup.KeyBoardUp));
	sAddLine(root, setting, "KeyBoardDown", "value", vw_KeyboardCodeName(Setup.KeyBoardDown));
	sAddLine(root, setting, "KeyBoardPrimary", "value", vw_KeyboardCodeName(Setup.KeyBoardPrimary));
	sAddLine(root, setting, "KeyBoardSecondary", "value", vw_KeyboardCodeName(Setup.KeyBoardSecondary));
	iAddLine(root, setting, "MousePrimary", "value", Setup.MousePrimary);
	iAddLine(root, setting, "MouseSecondary", "value", Setup.MouseSecondary);
	iAddLine(root, setting, "JoystickPrimary", "value", Setup.JoystickPrimary);
	iAddLine(root, setting, "JoystickSecondary", "value", Setup.JoystickSecondary);
	iAddLine(root, setting, "JoystickNum", "value", Setup.JoystickNum);
	iAddLine(root, setting, "ControlSensivity", "value", Setup.ControlSensivity);
	bAddLine(root, setting, "MouseControl", "value", Setup.MouseControl);
	iAddLine(root, setting, "LastProfile", "value", Setup.LastProfile);

	for(int i=0; i<10; i++)
	{
		char Name[128];
		sprintf(Name, "HintStatus%i", i+1);
		bAddLine(root, setting, Name, "value", Setup.NeedShowHint[i]);
	}



	//
	// всегда последние, всегда переписываем их
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

	// если он уже есть - его надо удалить
	setting = root->FirstChildElement("TopScores");
	if (setting != 0) root->RemoveChild(setting);
	// пересоздаем
	setting = new TiXmlElement("TopScores");
	setting->LinkEndChild(new TiXmlText(TopScoresResultString));
	root->LinkEndChild(setting);

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


		// если он уже есть - его надо удалить
		setting = root->FirstChildElement("PilotsProfiles");
		if (setting != 0) root->RemoveChild(setting);
		// переписываем все (пересоздаем)
		setting = new TiXmlElement("PilotsProfiles");
		setting->LinkEndChild(new TiXmlText(ResultString));
		root->LinkEndChild(setting);

		if (ResultString != 0) delete [] ResultString;
		if (ProfileData != 0) delete [] ProfileData;
		if (ProfileDataXORCode != 0) delete [] ProfileDataXORCode;
	}



	// сохраняем что получилось
	TiXmlPrinter printer;
	printer.SetLineBreak("\r\n"); // ставим перевод строки, чтобы в виндовсе было нормальное форматирование
	doc.Accept( &printer );

	SDL_RWops *TempRWops = SDL_RWFromFile(DatFileName, "w");

	if (TempRWops == NULL)
	{
		fprintf(stderr, "Can't save file %s\n", DatFileName);
	}
	else
	{
		SDL_RWwrite(TempRWops, printer.CStr(), strlen(printer.CStr()), 1);
	}

	doc.Clear();
	SDL_RWclose(TempRWops);
}













//-----------------------------------------------------------------------------
// считываем файл с настройками
//-----------------------------------------------------------------------------
void iGetLine(TiXmlElement * root, TiXmlElement * Element, const char *Name, const char *Attrib, int *Data)
{
	Element = root->FirstChildElement(Name);
	if (Element != 0)
	{
		*Data = atoi(Element->Attribute(Attrib));
	}
}
void fGetLine(TiXmlElement * root, TiXmlElement * Element, const char *Name, const char *Attrib, float *Data)
{
	Element = root->FirstChildElement(Name);
	if (Element != 0)
	{
		*Data = (float)atof(Element->Attribute(Attrib));
	}
}
void bGetLine(TiXmlElement * root, TiXmlElement * Element, const char *Name, const char *Attrib, bool *Data)
{
	Element = root->FirstChildElement(Name);
	if (Element != 0)
	{
		if (!strcmp(Element->Attribute(Attrib), "off")){*Data = false;}
		else
			if (!strcmp(Element->Attribute(Attrib), "on")){*Data = true;}
	}
}
void sGetLine(TiXmlElement * root, TiXmlElement * Element, const char *Name, const char *Attrib, char *Data)
{
	Element = root->FirstChildElement(Name);
	if (Element != 0)
	{
		strcpy(Data, Element->Attribute(Attrib));
	}
}
bool LoadXMLSetupFile(bool NeedSafeMode)
{
	TiXmlDocument doc;
	TiXmlElement * root = 0;
	TiXmlElement * setting = 0;

	// устанавливаем базовые настройки
	InitSetup();


	// читаем данные
	SDL_RWops *TempRWops = SDL_RWFromFile(DatFileName, "r");
	char * buffer = 0;

	if (TempRWops == NULL)
	{
		SaveXMLSetupFile();
		return true;
	}
	else
	{
		SDL_RWseek(TempRWops, 0, SEEK_END);
		int DataLength = SDL_RWtell(TempRWops);
		SDL_RWseek(TempRWops, 0, SEEK_SET);
		buffer = new char[DataLength];
		SDL_RWread(TempRWops, buffer, DataLength, 1);
		SDL_RWclose(TempRWops);

		doc.Parse((const char*)buffer, 0, TIXML_ENCODING_UTF8);
	}


	// берем первый элемент в скрипте
	root = doc.FirstChildElement("AstroMenaceSettings");

	// если установлен NeedSafeMode, не грузим часть данных
	if (NeedSafeMode) goto LoadProfiles;


	char MenuLanguageBuffer[16];
	sGetLine(root, setting, "MenuLanguage", "value", MenuLanguageBuffer);
	if (!strcmp(MenuLanguageBuffer, "en")) Setup.MenuLanguage = 1;
	if (!strcmp(MenuLanguageBuffer, "de")) Setup.MenuLanguage = 2;
	if (!strcmp(MenuLanguageBuffer, "ru")) Setup.MenuLanguage = 3;

	char VoiceLanguageBuffer[16];
	sGetLine(root, setting, "VoiceLanguage", "value", VoiceLanguageBuffer);
	if (!strcmp(VoiceLanguageBuffer, "en")) Setup.VoiceLanguage = 1;
	if (!strcmp(VoiceLanguageBuffer, "de")) Setup.VoiceLanguage = 2;
	if (!strcmp(VoiceLanguageBuffer, "ru")) Setup.VoiceLanguage = 3;

	iGetLine(root, setting, "FontNumber", "value", &(Setup.FontNumber));

	iGetLine(root, setting, "Width", "value", &(Setup.Width));
	iGetLine(root, setting, "Height", "value", &Setup.Height);
	iGetLine(root, setting, "BPP", "value", &Setup.BPP);


	char AspectRatioBuffer[16];
	sGetLine(root, setting, "AspectRatio", "value", AspectRatioBuffer);
	if (!strcmp(AspectRatioBuffer, "16:10"))
	{
		Setup.fAspectRatioWidth = 1228.0f;
		Setup.fAspectRatioHeight = 768.0f;
		Setup.iAspectRatioWidth = 1228;
		Setup.iAspectRatioHeight = 768;
	}
	if (!strcmp(AspectRatioBuffer, "4:3"))
	{
		Setup.fAspectRatioWidth = 1024.0f;
		Setup.fAspectRatioHeight = 768.0f;
		Setup.iAspectRatioWidth = 1024;
		Setup.iAspectRatioHeight = 768;
	}

	iGetLine(root, setting, "CameraModeWithStandardAspectRatio", "value", &Setup.CameraModeWithStandardAspectRatio);


	iGetLine(root, setting, "VBOCoreMode", "value", &Setup.VBOCoreMode);
	iGetLine(root, setting, "VAOCoreMode", "value", &Setup.VAOCoreMode);
	iGetLine(root, setting, "FBOCoreMode", "value", &Setup.FBOCoreMode);
	bGetLine(root, setting, "EqualOrMore128MBVideoRAM", "value", &Setup.EqualOrMore128MBVideoRAM);
	bGetLine(root, setting, "HardwareMipMapGeneration", "value", &Setup.HardwareMipMapGeneration);

	iGetLine(root, setting, "TextureFilteringMode", "value", &Setup.TextureFilteringMode);
	iGetLine(root, setting, "TexturesQuality", "value", &Setup.TexturesQuality);
	iGetLine(root, setting, "MSAA", "value", &Setup.MSAA);
	iGetLine(root, setting, "CSAA", "value", &Setup.CSAA);
	iGetLine(root, setting, "VisualEffectsQuality", "value", &Setup.VisualEffectsQuality);
	iGetLine(root, setting, "AnisotropyLevel", "value", &Setup.AnisotropyLevel);
	iGetLine(root, setting, "TexturesCompression", "value", &Setup.TexturesCompression);
	bGetLine(root, setting, "UseGLSL", "value", &Setup.UseGLSL);
	iGetLine(root, setting, "ShadowMap", "value", &Setup.ShadowMap);
	iGetLine(root, setting, "MaxPointLights", "value", &Setup.MaxPointLights);
	iGetLine(root, setting, "MusicSw", "value", &Setup.MusicSw);
	iGetLine(root, setting, "SoundSw", "value", &Setup.SoundSw);
	iGetLine(root, setting, "VoiceSw", "value", &Setup.VoiceSw);
	iGetLine(root, setting, "Gamma", "value", &Setup.Gamma);
	bGetLine(root, setting, "ShowFPS", "value", &Setup.ShowFPS);
	iGetLine(root, setting, "GameWeaponInfoType", "value", &Setup.GameWeaponInfoType);
	fGetLine(root, setting, "GameSpeed", "value", &Setup.GameSpeed);
	iGetLine(root, setting, "LoadingHint", "value", &Setup.LoadingHint);




	char KeyName[64];
	sGetLine(root, setting, "KeyboardDecreaseGameSpeed", "value", KeyName);
	Setup.KeyboardDecreaseGameSpeed = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyboardResetGameSpeed", "value", KeyName);
	Setup.KeyboardResetGameSpeed = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyboardResetGameSpeed", "value", KeyName);
	Setup.KeyboardResetGameSpeed = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyboardIncreaseGameSpeed", "value", KeyName);
	Setup.KeyboardIncreaseGameSpeed = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyboardGameWeaponInfoType", "value", KeyName);
	Setup.KeyboardGameWeaponInfoType = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyboardPrimaryWeaponFireMode", "value", KeyName);
	Setup.KeyboardPrimaryWeaponFireMode = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyboardSecondaryWeaponFireMode", "value", KeyName);
	Setup.KeyboardSecondaryWeaponFireMode = vw_KeyboardNameCode(KeyName);


	sGetLine(root, setting, "KeyBoardLeft", "value", KeyName);
	Setup.KeyBoardLeft = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyBoardRight", "value", KeyName);
	Setup.KeyBoardRight = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyBoardUp", "value", KeyName);
	Setup.KeyBoardUp = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyBoardDown", "value", KeyName);
	Setup.KeyBoardDown = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyBoardPrimary", "value", KeyName);
	Setup.KeyBoardPrimary = vw_KeyboardNameCode(KeyName);
	sGetLine(root, setting, "KeyBoardSecondary", "value", KeyName);
	Setup.KeyBoardSecondary = vw_KeyboardNameCode(KeyName);

	iGetLine(root, setting, "MousePrimary", "value", &Setup.MousePrimary);
	iGetLine(root, setting, "MouseSecondary", "value", &Setup.MouseSecondary);
	iGetLine(root, setting, "JoystickPrimary", "value", &Setup.JoystickPrimary);
	iGetLine(root, setting, "JoystickSecondary", "value", &Setup.JoystickSecondary);
	iGetLine(root, setting, "JoystickNum", "value", &Setup.JoystickNum);
	iGetLine(root, setting, "ControlSensivity", "value", &Setup.ControlSensivity);
	bGetLine(root, setting, "MouseControl", "value", &Setup.MouseControl);
	iGetLine(root, setting, "LastProfile", "value", &Setup.LastProfile);


	for(int i=0; i<10; i++)
	{
		char Name[128];
		sprintf(Name, "HintStatus%i", i+1);
		bGetLine(root, setting, Name, "value", &Setup.NeedShowHint[i]);
	}




	//
	// заполняем таблицу рекордов
	//

	setting = root->FirstChildElement("TopScores");
	if (setting != 0)
	{

		int TopScoresDataSize = strlen(setting->GetText());
		unsigned char *TopScoresData = new unsigned char[TopScoresDataSize+1];
		unsigned char *TopScoresDataXORCode = new unsigned char[TopScoresDataSize+1];
		char *TopScoresResultString = new char[TopScoresDataSize+1];

		strcpy(TopScoresResultString, setting->GetText());

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


	setting = root->FirstChildElement("PilotsProfiles");
	if (setting != 0)
	{
		int ProfileDataSize = strlen(setting->GetText());
		unsigned char *ProfileData = new unsigned char[ProfileDataSize+1];
		unsigned char *ProfileDataXORCode = new unsigned char[ProfileDataSize+1];
		char *ResultString = new char[ProfileDataSize+1];

		strcpy(ResultString, setting->GetText());

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
		// находим правильное значение, т.к. загружали использую длину строки (!!!)
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

	doc.Clear();
	if (buffer != 0) delete [] buffer;

	return false;
}



