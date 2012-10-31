/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Main.cpp

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


#include "Game.h"


#ifdef __unix
#ifdef xinerama
	#include <X11/extensions/Xinerama.h>
#endif // xinerama
#endif // unix


//------------------------------------------------------------------------------------
// настройки
//------------------------------------------------------------------------------------
// настройки игры
GameSetup Setup;
// возможности железа, получаем через рендерер
eDevCaps *CAPS=0;


sFontList FontList[FontQuantity] =
{
{"Ubuntu Family", "DATA/FONT/Ubuntu-B.ttf"},
{"Liberation Mono", "DATA/FONT/LiberationMono-Bold.ttf"},
{"Liberation Sans", "DATA/FONT/LiberationSans-Bold.ttf"},
{"Liberation Serif", "DATA/FONT/LiberationSerif-Bold.ttf"},
{"FreeFont Mono", "DATA/FONT/FreeMonoBold.ttf"},
{"FreeFont Sans", "DATA/FONT/FreeSansBold.ttf"},
{"FreeFont Serif", "DATA/FONT/FreeSerifBold.ttf"},
};


//------------------------------------------------------------------------------------
// общие состояния и статусы
//------------------------------------------------------------------------------------
// текущий статус (текущее меню)
eGameStatus GameStatus;
// защелка на выход, когда нужно перегрузить, а когда просто поменять режим
bool Quit = false;
bool NeedReCreate = false;
// выходим или нет
bool CanQuit = true;
// запоминаем номер успешного установленного разрешения
int GoodResolution = 0;
// для выбора в списке режимов разрешения
sVideoModes *VideoModes = 0;
int VideoModesNum = 0;
// текущие параметры десктопа
sVideoModes CurrentVideoMode;
// статус загрузки, должно быть ОбщееКол-во Миссий+1(для меню)
bool LoadedTypes[1000];
// данные для определения папки пользователя
#ifdef WIN32
#define SD_APPDATA                   0x001a
#define SD_DESKTOPDIRECTORY          0x0010        // <user name>\Desktop
typedef BOOL (WINAPI *SHGETSPECIALFOLDERPATH)(HWND hwndOwner, LPTSTR lpszPath, int nFolder, BOOL fCreate);
SHGETSPECIALFOLDERPATH pSHGetSpecialFolderPath = 0;
#endif // WIN32



//------------------------------------------------------------------------------------
// пути к файлам
//------------------------------------------------------------------------------------
// полное путь к программе
char ProgrammDir[MAX_PATH];
char VFSFileNamePath[MAX_PATH];
// полное имя для файла с данными игры
char DatFileName[MAX_PATH];
// для сохранения скриншотов
char ScreenshotDir[MAX_PATH];


//------------------------------------------------------------------------------------
// управление
//------------------------------------------------------------------------------------
// состояние кнопок мышки
bool SDL_MouseCurrentStatus[8];
// джойстик
SDL_Joystick *Joystick = NULL;
// базовые параметры, или параметры блокировки управления (нет сигнала, нули)
int JoystickAxisX = 0;
int JoystickAxisY = 0;
// кнопки джойстика (текущего)
bool JoysticButtons[100];


//------------------------------------------------------------------------------------
// камера
//------------------------------------------------------------------------------------
// ближайшая точка, основная точка отсчета локальной системы координат игры
VECTOR3D	GamePoint(0.0f, 0.0f, 0.0f);
// направление движения камеры
VECTOR3D	GameCameraMovement(0.0f, 0.0f, 1.0f);


//------------------------------------------------------------------------------------
// графическая часть
//------------------------------------------------------------------------------------
// прорисовка динамической части курсора
eParticleSystem2D *CursorParticleSystem2D = 0;
// статические частицы - звезды на скайбоксе
CSpaceStars *psSpaceStatic = 0;













//------------------------------------------------------------------------------------
// основная процедура...
//------------------------------------------------------------------------------------
int main( int argc, char **argv )
{

#ifdef WIN32
	// иним пути для винды
	ZeroMemory(ProgrammDir, sizeof(ProgrammDir));
	GetModuleFileName(NULL, ProgrammDir, MAX_PATH);
	char* s = strrchr(ProgrammDir,'\\');
	if (s) s[0]=0x0;
	const char *Fi = "\\";
	strcat( ProgrammDir, Fi );

	ZeroMemory(DatFileName, sizeof(DatFileName));
	ZeroMemory(VFSFileNamePath, sizeof(VFSFileNamePath));
	ZeroMemory(ScreenshotDir, sizeof(ScreenshotDir));

	// Получаем данные, где папка пользователя
	bool InitWithoutDLL = true;
	bool InitScrWithoutDLL = true;
	HMODULE hShellDLL = LoadLibrary("shell32.dll");
	if (hShellDLL)
	{
		pSHGetSpecialFolderPath = (SHGETSPECIALFOLDERPATH) GetProcAddress(hShellDLL, "SHGetSpecialFolderPathA");
		char UserPath[MAX_PATH];
		ZeroMemory(UserPath, sizeof(UserPath));

		if (pSHGetSpecialFolderPath != NULL)
		{
			if(SUCCEEDED(pSHGetSpecialFolderPath(NULL,
									UserPath,
									SD_APPDATA, //CSIDL_APPDATA
									true)))
			{
				strcat(UserPath, "\\AstroMenace\\");
				CreateDirectory(UserPath, NULL);

				strcpy(DatFileName, UserPath);
				strcat(DatFileName, "amconfig.xml");

				// уже проинили, дальше не нужно
				InitWithoutDLL = false;
			}

			if(SUCCEEDED(pSHGetSpecialFolderPath(NULL,
									ScreenshotDir,
									SD_DESKTOPDIRECTORY, // SD_DESKTOPDIRECTORY
									true)))
			{

				strcat(ScreenshotDir, "\\AstroMenaceScreenshot");

				// уже проинили, дальше не нужно
				InitScrWithoutDLL = false;
			}

		}

	}
	// освобождаем библиотеку
	FreeLibrary(hShellDLL);


	// иним, если старая винда, или была ошибка
	if (InitWithoutDLL)
	{
		strcpy(DatFileName, ProgrammDir);
		strcat(DatFileName, "amconfig.xml");
	}
	if (InitScrWithoutDLL)
	{
		strcpy(ScreenshotDir, ProgrammDir);
		strcat(ScreenshotDir, "AstroMenaceScreenshot");
	}

	strcpy(VFSFileNamePath, ProgrammDir);
	strcat(VFSFileNamePath, "gamedata.vfs");
#elif defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
	// иним пути для юникса-линукса
	// если передали параметр-путь

	const char* key = "HOME";
	const char* homeval = getenv(key);

	bool dirpresent = false;
	for (int i=1; i<argc; i++)
	{
		if (!strncmp(argv[i], "--dir=", sizeof("--dir")))
		{
			dirpresent = true;
			// если передали относительный путь в папку пользователя с тильдой
			if (argv[i][sizeof("--dir")] != '~')
				strcpy(ProgrammDir, argv[i]+strlen("--dir="));
			else
			{
				strcpy(ProgrammDir, homeval);// -1, это тильда... а в кол-ве нет, т.к. /0 там должен остаться
				strcat(ProgrammDir, argv[i]+strlen("--dir=")+1);
			}
			// если в конце нет слеша - ставим его
			if (ProgrammDir[strlen(ProgrammDir)-1] != '/')
				strncat(ProgrammDir, "/", strlen("/"));

		}
	}
	if (!dirpresent)
	{
#ifdef DATADIR
		strcpy(ProgrammDir, DATADIR "/");
#else
 		strcpy(ProgrammDir, argv[0]);
 		char* s = strrchr(ProgrammDir,'/');
 		if (s) s[0]=0x0;
 		const char *Fi = "/";
 		strcat( ProgrammDir, Fi );
#endif // DATADIR
	}


	strcpy(ScreenshotDir, homeval);
	strcat(ScreenshotDir, "/Desktop/AstroMenaceScreenshot");

	strcpy(VFSFileNamePath, ProgrammDir);
	strcat(VFSFileNamePath, "gamedata.vfs");

	// укладываем в нужном месте (где 100% дають создавать) файл с настройками
	strcpy(DatFileName, homeval);
	strcat(DatFileName, "/.astromenace");
	mkdir(DatFileName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	strcat(DatFileName, "/amconfig.xml");

#endif // unix











	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// исследуем другие ключи
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	// флаг отображать ли системный курсор
	bool NeedShowSystemCursor = false;
	// флаг нужно ли сбрасывать настройки игры при старте
	bool NeedSafeMode = false;
	// флаг перевода игры в режим упаковки gamedata.vfs файла
	bool NeedPack = false;

	for (int i=1; i<argc; i++)
	{
		// проверка ключа "--help"
		if (!strcmp(argv[i], "--help"))
		{
			printf("AstroMenace launch options:\n\n");

			printf("--dir=/game/data/folder/ - folder with gamedata.vfs file (Linux only)\n");
			printf("--mouse - launch the game without system cursor hiding.\n");
			printf("--safe-mode - reset all settings not connected to Pilots Profiles at the game launch.\n");
			printf("--pack - pack data to gamedata.vfs file\n");
			printf("--rawdata=/game/rawdata/folder/ - folder with game raw data for gamedata.vfs.\n");
			printf("--help - info about all game launch options.\n");

			return 0;
		}

		// проверка ключа "--mouse"
		if (!strcmp(argv[i], "--mouse"))
		{
			NeedShowSystemCursor = true;
		}

		// проверка ключа "--safe-mode"
		if (!strcmp(argv[i], "--safe-mode"))
		{
			NeedSafeMode = true;
		}

		// проверка ключа "--safe-mode"
		if (!strcmp(argv[i], "--pack"))
		{
			NeedPack = true;
		}
	}



	// версия
	printf("AstroMenace %s %i\n\n", GAME_VERSION, GAME_BUILD);









	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// переводим в режим генерации gamedata.vfs файла
	// генерируем файл данный gamedata.vfs учитывая текущее его расположение
	// !!! всегда делаем только с одним открытым на запись VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedPack)
	{
		char RawDataDir[MAX_PATH];
		// по умолчанию, считаем что рав данные прямо с нами лежат
		strcpy(RawDataDir, ProgrammDir);
		strcat(RawDataDir, "RAW_VFS_DATA/");


		// ищем, если передали ключем его расположение
		for (int i=1; i<argc; i++)
		{
			if (!strncmp(argv[i], "--rawdata=", sizeof("--rawdata")))
			{
#if defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
				// если передали относительный путь в папку пользователя с тильдой
				if (argv[i][sizeof("--rawdata")] != '~')
					strcpy(RawDataDir, argv[i]+strlen("--rawdata="));
				else
				{
					strcpy(RawDataDir, homeval);// -1, это тильда... а в кол-ве нет, т.к. /0 там должен остаться
					strcat(RawDataDir, argv[i]+strlen("--rawdata=")+1);
				}
#elif defined(WIN32)
				// если есть двоеточия после второго символа - это полный путь с указанием девайса
				if (argv[i][sizeof("--rawdata=")] == ':')
				{
					strcpy(RawDataDir, argv[i]+strlen("--rawdata="));
				}
				else
				{
					strcpy(RawDataDir, ProgrammDir);
					strcat(RawDataDir, argv[i]+strlen("--rawdata="));
				}
#endif // WIN32
				// если в конце нет слеша - ставим его
				if (RawDataDir[strlen(RawDataDir)-1] != '/')
					strncat(RawDataDir, "/", strlen("/"));
			}
		}

		printf("Source Raw Folder: %s\n", RawDataDir);
		return ConvertFS2VFS(RawDataDir);
	}







	// работа с файлом данных... передаем базовый режим окна
	bool FirstStart = LoadXMLSetupFile(NeedSafeMode);




	// инициализация счета времени
	vw_InitTime();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка звука, всегда до LoadGameData
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!InitAudio())
	{
		Setup.Music_check = false;
		Setup.Sound_check = false;
		fprintf(stderr, "Unable to open audio!\n");
		printf("\n");
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// подключаем VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (vw_OpenVFS(VFSFileNamePath) != 0)
	{
		fprintf(stderr, "gamedata.vfs file not found or corrupted.\n");
		return 0;
	}
	printf("\n");



ReCreate:

	// иним фонт
	vw_InitFont(FontList[Setup.FontNumber].FontFileName, 16);


#ifdef __unix
	// для TwinView и Xinerama выбираем нулевой, но не меняем если передали
	setenv("SDL_VIDEO_FULLSCREEN_DISPLAY","0",0);
#endif //unix

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним SDL
	// это нужно сделать сразу, чтобы правильно поставить разрешение
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		fprintf(stderr, "Couldn't init SDL: %s\n", SDL_GetError());
		return 1;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// сбрасываем все, ничего не загружали
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (int i=0; i<1000; i++)
	{
		LoadedTypes[i] = false;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// получаем текущее разрешение экрана
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// предварительно ставим по умолчанию
	int CurrentVideoModeX = 0;
	int CurrentVideoModeY = 0;
	CurrentVideoMode.W = 1024;
	CurrentVideoMode.H = 768;
	CurrentVideoMode.BPP = 16;
	// пытаемся получить данные
	CurrentVideoMode.BPP = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
	if (CurrentVideoMode.BPP <= 16) CurrentVideoMode.BPP = 16;
	if (CurrentVideoMode.BPP > 16) CurrentVideoMode.BPP = 32;
	CurrentVideoMode.W = SDL_GetVideoInfo()->current_w;
	CurrentVideoMode.H = SDL_GetVideoInfo()->current_h;
	printf("Current Video Mode: %ix%i %ibit \n", CurrentVideoMode.W, CurrentVideoMode.H, CurrentVideoMode.BPP);

#ifdef __unix
#ifdef xinerama
	// определяем есть ли xinerama и какое разрешение экранов

	printf("\n");
    Display* display = XOpenDisplay(NULL);
    if (!display)
	{
        fprintf(stderr, "XOpenDisplay() fails.\n");
        return 1;
    }
    int xineramaScreenCount;
    if (XineramaIsActive(display))
	{
        XineramaScreenInfo* xineramaScreenInfo = XineramaQueryScreens(display, &xineramaScreenCount);
		printf("Xinerama/TwinView detected.\n");
        printf("Screen count: %d\n", xineramaScreenCount);
        for (int i = 0; i < xineramaScreenCount; i++)
		{
            printf("Screen #%d: (%d, %d) x (%d, %d)\n", i, xineramaScreenInfo[i].x_org, xineramaScreenInfo[i].y_org, xineramaScreenInfo[i].width, xineramaScreenInfo[i].height);

		}

		// смотрим, если передали параметр SDL_VIDEO_FULLSCREEN_DISPLAY, берем параметры нужного дисплея

		char * pScreen;
		int iScreen = 0;
		pScreen = getenv("SDL_VIDEO_FULLSCREEN_DISPLAY");
		if (pScreen != NULL) iScreen = atoi(pScreen);
		if (iScreen > xineramaScreenCount - 1) iScreen = 0;

		CurrentVideoModeX = xineramaScreenInfo[iScreen].x_org;
		CurrentVideoModeY = xineramaScreenInfo[iScreen].y_org;
		CurrentVideoMode.W = xineramaScreenInfo[iScreen].width;
		CurrentVideoMode.H = xineramaScreenInfo[iScreen].height;

        XFree(xineramaScreenInfo);
    }
#endif // xinerama
#endif // unix





	// детектим и составляем перечень всех возможных разрешений самостоятельно
	int AllSupportedModesCount = 65;
	sVideoModes AllSupportedModes[65] =
	{
		{640, 480, -1},
		{768, 480, -1},
		{800, 480, -1},
		{800, 600, -1},
		{832, 624, -1},
		{854, 480, -1},
		{960, 540, -1},
		{960, 544, -1},
		{960, 640, -1},
		{960, 720, -1},
		{1024, 576, -1},
		{1024, 600, -1},
		{1024, 640, -1},
		{1024, 768, -1},
		{1024, 800, -1},
		{1120, 832, -1},
		{1152, 720, -1},
		{1152, 768, -1},
		{1152, 864, -1},
		{1152, 900, -1},
		{1280, 720, -1},
		{1280, 768, -1},
		{1280, 800, -1},
		{1280, 854, -1},
		{1280, 960, -1},
		{1280, 1024, -1},
		{1366, 768, -1},
		{1400, 1050, -1},
		{1440, 900, -1},
		{1440, 960, -1},
		{1440, 1024, -1},
		{1440, 1080, -1},
		{1600, 900, -1},
		{1600, 1024, -1},
		{1600, 1200, -1},
		{1680, 1050, -1},
		{1792, 1344, -1},
		{1856, 1392, -1},
		{1800, 1440, -1},
		{1920, 1080, -1},
		{1920, 1200, -1},
		{1920, 1400, -1},
		{1920, 1440, -1},
		{2048, 1280, -1},
		{2048, 1536, -1},
		{2304, 1440, -1},
		{2304, 1728, -1},
		{2560, 1440, -1},
		{2560, 1600, -1},
		{2560, 1920, -1},
		{2560, 2048, -1},
		{2800, 2100, -1},
		{2880, 1800, -1},
		{3200, 2048, -1},
		{3200, 2400, -1},
		{3840, 2160, -1},
		{3840, 2400, -1},
		{4096, 2304, -1},
		{4096, 3072, -1},
		{5120, 3200, -1},
		{5120, 4096, -1},
		{6400, 4096, -1},
		{6400, 4800, -1},
		{7680, 4320, -1},
		{7680, 4800, -1},
	};

	// если списка еще нет - создаем его
	if (VideoModes == 0)
	{
		VideoModesNum = 0;

		for(int i=0; i<AllSupportedModesCount; i++)
		if ((AllSupportedModes[i].W <= CurrentVideoMode.W) & (AllSupportedModes[i].H <= CurrentVideoMode.H))
		{
			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, 16, SDL_FULLSCREEN | SDL_OPENGL) != 0) VideoModesNum++;
			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, 32, SDL_FULLSCREEN | SDL_OPENGL) != 0) VideoModesNum++;
			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, CurrentVideoMode.BPP, SDL_ANYFORMAT | SDL_OPENGL) != 0) VideoModesNum++;
		}


		VideoModes = new sVideoModes[VideoModesNum];

		int k=0;
		for(int i=0; i<AllSupportedModesCount; i++)
		if ((AllSupportedModes[i].W <= CurrentVideoMode.W) & (AllSupportedModes[i].H <= CurrentVideoMode.H))
		{
			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, 16, SDL_FULLSCREEN | SDL_OPENGL) != 0)
			{
				VideoModes[k].W = AllSupportedModes[i].W;
				VideoModes[k].H = AllSupportedModes[i].H;
				VideoModes[k].BPP = 16;
				k++;
			}

			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, 32, SDL_FULLSCREEN | SDL_OPENGL) != 0)
			{
				VideoModes[k].W = AllSupportedModes[i].W;
				VideoModes[k].H = AllSupportedModes[i].H;
				VideoModes[k].BPP = 32;
				k++;
			}

			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, CurrentVideoMode.BPP, SDL_ANYFORMAT | SDL_OPENGL) != 0)
			{
				VideoModes[k].W = AllSupportedModes[i].W;
				VideoModes[k].H = AllSupportedModes[i].H;
				VideoModes[k].BPP = 0;
				k++;
			}
		}


		// выводим список поддерживаемых разрешений
		printf("\n");
		printf("Supported resolutions list:\n");
		for(int i=0; i<VideoModesNum; i++)
		{
			printf("%ix%i %ibit \n", VideoModes[i].W, VideoModes[i].H, VideoModes[i].BPP);
		}
		printf("\n");

	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем и корректируем текущие настройки окна
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (FirstStart)
	{
		Setup.Width = CurrentVideoMode.W;
		Setup.Height = CurrentVideoMode.H;
		Setup.BPP = CurrentVideoMode.BPP;
	}
	// если загруженные параметры, больше чем максимальные, ставим максимальные (если Xinerama, например)
	if ((VideoModes[VideoModesNum-1].W < Setup.Width) | (VideoModes[VideoModesNum-1].H < Setup.Height))
	{
		Setup.Width = VideoModes[VideoModesNum-1].W;
		Setup.Height = VideoModes[VideoModesNum-1].H;
		Setup.BPP = CurrentVideoMode.BPP;
	}
	// делаем проверку по листу разрешений экрана, если входит - все ок, если нет - ставим оконный режим принудительно
	bool NeedResetToWindowedMode = true;
	for(int i=0; i<VideoModesNum; i++)
	{
		if ((VideoModes[i].W == Setup.Width) &
			(VideoModes[i].H == Setup.Height) &
			(VideoModes[i].BPP == Setup.BPP))
		{
			NeedResetToWindowedMode = false;
			break;
		}
	}
	if (NeedResetToWindowedMode) Setup.BPP = 0;








	// делаем правильные симлинки на языковые ресурсы, автоматизации нет, просто линкуем нужные файлы

	switch (Setup.MenuLanguage)
	{
		case 1: //en
			vw_CreateEntryLinkVFS("DATA/DATA_EN/GAME/mission.tga", "DATA/GAME/mission.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/GAME/missionfailed.tga", "DATA/GAME/missionfailed.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/GAME/pause.tga", "DATA/GAME/pause.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/SCRIPT/text.xml", "DATA/SCRIPT/text.xml");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/button_weaponry_in.tga", "DATA/MENU/button_weaponry_in.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/button_weaponry_out.tga", "DATA/MENU/button_weaponry_out.tga");
			break;

		case 2: //de
			vw_CreateEntryLinkVFS("DATA/DATA_DE/GAME/mission.tga", "DATA/GAME/mission.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/GAME/missionfailed.tga", "DATA/GAME/missionfailed.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/GAME/pause.tga", "DATA/GAME/pause.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/SCRIPT/text.xml", "DATA/SCRIPT/text.xml");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/button_weaponry_in.tga", "DATA/MENU/button_weaponry_in.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/button_weaponry_out.tga", "DATA/MENU/button_weaponry_out.tga");
			break;

		case 3: //ru
			vw_CreateEntryLinkVFS("DATA/DATA_RU/GAME/mission.tga", "DATA/GAME/mission.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/GAME/missionfailed.tga", "DATA/GAME/missionfailed.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/GAME/pause.tga", "DATA/GAME/pause.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/SCRIPT/text.xml", "DATA/SCRIPT/text.xml");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/button_weaponry_in.tga", "DATA/MENU/button_weaponry_in.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/button_weaponry_out.tga", "DATA/MENU/button_weaponry_out.tga");
			break;
	}

	switch (Setup.VoiceLanguage)
	{
		case 1: //en
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/Attention.wav", "DATA/VOICE/Attention.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/CeaseFire.wav", "DATA/VOICE/CeaseFire.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/EngineMalfunction.wav", "DATA/VOICE/EngineMalfunction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/MissileDetected.wav", "DATA/VOICE/MissileDetected.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/PowerSupplyReestablished.wav", "DATA/VOICE/PowerSupplyReestablished.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/PrepareForAction.wav", "DATA/VOICE/PrepareForAction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/ReactorMalfunction.wav", "DATA/VOICE/ReactorMalfunction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/Warning.wav", "DATA/VOICE/Warning.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/WeaponDamaged.wav", "DATA/VOICE/WeaponDamaged.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/WeaponDestroyed.wav", "DATA/VOICE/WeaponDestroyed.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/VOICE/WeaponMalfunction.wav", "DATA/VOICE/WeaponMalfunction.wav");
			break;

		case 2: //de
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/Attention.wav", "DATA/VOICE/Attention.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/CeaseFire.wav", "DATA/VOICE/CeaseFire.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/EngineMalfunction.wav", "DATA/VOICE/EngineMalfunction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/MissileDetected.wav", "DATA/VOICE/MissileDetected.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/PowerSupplyReestablished.wav", "DATA/VOICE/PowerSupplyReestablished.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/PrepareForAction.wav", "DATA/VOICE/PrepareForAction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/ReactorMalfunction.wav", "DATA/VOICE/ReactorMalfunction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/Warning.wav", "DATA/VOICE/Warning.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/WeaponDamaged.wav", "DATA/VOICE/WeaponDamaged.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/WeaponDestroyed.wav", "DATA/VOICE/WeaponDestroyed.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/VOICE/WeaponMalfunction.wav", "DATA/VOICE/WeaponMalfunction.wav");
			break;

		case 3: //ru
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/Attention.wav", "DATA/VOICE/Attention.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/CeaseFire.wav", "DATA/VOICE/CeaseFire.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/EngineMalfunction.wav", "DATA/VOICE/EngineMalfunction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/MissileDetected.wav", "DATA/VOICE/MissileDetected.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/PowerSupplyReestablished.wav", "DATA/VOICE/PowerSupplyReestablished.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/PrepareForAction.wav", "DATA/VOICE/PrepareForAction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/ReactorMalfunction.wav", "DATA/VOICE/ReactorMalfunction.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/Warning.wav", "DATA/VOICE/Warning.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/WeaponDamaged.wav", "DATA/VOICE/WeaponDamaged.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/WeaponDestroyed.wav", "DATA/VOICE/WeaponDestroyed.wav");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/VOICE/WeaponMalfunction.wav", "DATA/VOICE/WeaponMalfunction.wav");
			break;
	}
	printf("\n");


	// загружаем все текстовые данные
	InitGameText("DATA/SCRIPT/text.xml");








	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем и иним джойстик
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef joystick
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == 0)
	{
		if (SDL_NumJoysticks()>0)
		{
			printf("Found Joystick(s):\n");
			for (int i=0; i<SDL_NumJoysticks(); i++)
				printf("Joystick Name %i: %s\n", i, SDL_JoystickName(i));

			Joystick = SDL_JoystickOpen(Setup.JoystickNum);

			if(Joystick)
			{
				printf("Opened Joystick %i\n", Setup.JoystickNum);
				printf("Joystick Name: %s\n", SDL_JoystickName(Setup.JoystickNum));
				printf("Joystick Number of Axes: %d\n", SDL_JoystickNumAxes(Joystick));
				printf("Joystick Number of Buttons: %d\n", SDL_JoystickNumButtons(Joystick));
				printf("Joystick Number of Balls: %d\n\n", SDL_JoystickNumBalls(Joystick));
			}
			else
				fprintf(stderr, "Couldn't open Joystick 0\n\n");
		}
	}
	else fprintf(stderr, "Can't init Joystick, SDL Error: %s\n", SDL_GetError());
#endif





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// создаем окно и базовые опенжл контекст
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool Fullscreen = (Setup.BPP != 0);
	int InitStatus = vw_InitWindow("AstroMenace", Setup.Width, Setup.Height, &Setup.BPP, Fullscreen, CurrentVideoModeX, CurrentVideoModeY, CurrentVideoMode.W, CurrentVideoMode.H);

	// ошибка окна (размеры)
	if (InitStatus == 1)
	{
		// не можем создать окно или включить полноэкранный режим - ставим минимальный оконный режим
		if (640 != Setup.Width ||
			480 != Setup.Height ||
			0 != Setup.BPP)
		{
			Setup.Width = 640;
			Setup.Height = 480;
			Setup.BPP = 0;
			Setup.fAspectRatioWidth = 1024.0f;
			Setup.fAspectRatioHeight = 768.0f;
			Setup.iAspectRatioWidth = 1024;
			Setup.iAspectRatioHeight = 768;
			Setup.MSAA = 0;
			Setup.CSAA = 0;
			SaveXMLSetupFile();
			SDL_Quit();
			FirstStart = false;
			goto ReCreate;
		}

		fprintf(stderr, "Wrong resolution. Fatal error.\n");
#ifdef WIN32
		MessageBox(NULL,"Wrong resolution. Please, install the newest video drivers from your video card vendor.", "Render system - Fatal Error",MB_OK|MB_APPLMODAL|MB_ICONERROR);
#endif // WIN32
		SDL_Quit();
		return 0;									// Quit If Window Was Not Created
	}
	// даже если 24, все равно тянем как 32 бита, а если меньше 16, то ставим 16
	if (Setup.BPP <= 16) Setup.BPP = 16;
	if (Setup.BPP > 16) Setup.BPP = 32;
	if (!Fullscreen) Setup.BPP = 0;





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем возможности железа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	CAPS = vw_GetDevCaps();

	// если нужно, устанавливаем перерытие значений внутри движка, може только выключить - включить то чего нет нельзя
#ifndef vbo // принудительно отключаем вообще работу с vbo
	CAPS->VBOSupported = false;
	printf("Vertex Buffer support forced disabled.\n");
#endif
	if (Setup.VBOCoreMode == 0)
	{
		CAPS->VBOSupported = false;
	}

	// если нужно, выключаем vao
#ifndef vao // принудительно отключаем вообще работу с vao
	CAPS->VAOSupported = false;
	printf("Vertex Array Object support forced disabled.\n");
#endif
	// работаем только если есть VBO
	if ((Setup.VAOCoreMode == 0) | (Setup.VBOCoreMode == 0) | (!CAPS->VBOSupported))
	{
		CAPS->VAOSupported = false;
	}

	// если нужно, выключаем fbo
#ifndef fbo // принудительно отключаем вообще работу с fbo
	CAPS->FramebufferObject = false;
	printf("Frame Buffer Object support forced disabled.\n");
#endif
	if (Setup.FBOCoreMode == 0)
	{
		CAPS->FramebufferObject = false;
	}

	// проверка поддержки шейдеров (нужна 100% поддержка GLSL 1.0)
	if (Setup.UseGLSL)
		if (!CAPS->GLSL100Supported || CAPS->ShaderModel < 3.0f) Setup.UseGLSL = false;

	// управление генерации мипмеп уровней- можем только выключить, нельзя включить если его нет
	if (!Setup.HardwareMipMapGeneration)
	{
		CAPS->HardwareMipMapGeneration = false;
	}

	// анализ системы только если это первый запуск
	if (FirstStart)
	{
		// если шейдерная модель 3-я или выше
		if (CAPS->ShaderModel >= 3.0f)
		{
			// памяти достаточно, включаем другой режим загрузки
			Setup.EqualOrMore128MBVideoRAM = true;
		}
		// если шейдерная модель 4-я или выше
		if (CAPS->ShaderModel >= 4.0f)
		{
			// 100% держит наши шейдеры
			Setup.UseGLSL = true;
			Setup.ShadowMap = 1;
			// 100% больше чем нужно памяти и не надо сжимать текстуры (ув. качество и скорость загрузки)
			Setup.TexturesCompression = false;
			// немного больше ставим другие опции
			Setup.MSAA = 2;
			Setup.CSAA = 2;
			Setup.AnisotropyLevel = CAPS->MaxAnisotropyLevel;
			Setup.MaxPointLights = 4;
		}
		// если шейдерная модель 4.1-я или выше
		if (CAPS->ShaderModel >= 4.1f)
		{
			// немного больше ставим другие опции
			Setup.ShadowMap = 2;
			Setup.MSAA = 4;
			Setup.CSAA = 4;
			Setup.MaxPointLights = 6;
		}

#if defined(__APPLE__) && defined(__MACH__)
		// для маков по умолчанию выключаем сглаживание, тени и шейдеры, т.к. там может все софтово эмулироваться и жутко тормозить
		Setup.MSAA = 0;
		Setup.CSAA = 0;
		Setup.UseGLSL = false;
		Setup.ShadowMap = 0;
#endif

		// устанавливаем соотношение сторон по установленному разрешению экрана
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
	}

	// если не поддерживает железо фбо или шейдеры, выключаем шадовмеп
	if (!CAPS->FramebufferObject | !CAPS->GLSL100Supported | (CAPS->ShaderModel < 3.0f)) Setup.ShadowMap = 0;

	if (Setup.MSAA > CAPS->MaxSamples) Setup.MSAA = Setup.CSAA = CAPS->MaxSamples;
	// на всякий случай проверяем, входит ли текущее сглаживание в список доступных
	int CurrentAAMode = -1;
	if (Setup.MSAA != 0)
	{
		for (int i=0;i<CAPS->MaxMultisampleCoverageModes; i++)
		{
			if ((CAPS->MultisampleCoverageModes[i].ColorSamples == Setup.MSAA) &
				(CAPS->MultisampleCoverageModes[i].CoverageSamples == Setup.CSAA))
				{
					CurrentAAMode = i;
					break;
				}
		}
	}
	// если ничего не нашли, сбрасываем в нули
	if (CurrentAAMode == -1) Setup.MSAA = Setup.CSAA = 0;






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершаем инициализацию
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_InitOpenGL(Setup.Width, Setup.Height, &Setup.MSAA, &Setup.CSAA);


	// вторичная работа с настройками
	// в процессе инициализации opengl контекста мы подключаем указатели на функции, и данные могут измениться


	// если не поддерживаем как минимум 2 текстуры, железо очень слабое - не запустимся
	if (CAPS->MaxMultTextures < 2)
	{
		SDL_Quit();
        fprintf(stderr, "The Multi Textures feature unsupported by hardware. Fatal error.\n");
#ifdef WIN32
		MessageBox(NULL,"OpenGL 1.3 required. Please, install the newest video drivers from your video card vendor.", "Render system - Fatal Error",MB_OK|MB_APPLMODAL|MB_ICONERROR);
#endif // WIN32
		return 0;
	}

	// если поддерживает сторедж - вырубаем вообще поддержку сжатия (все равно работаем без него)
	if (CAPS->TextureStorage) CAPS->TexturesCompression = false;










	// ставим данные к которым приводить, плюс они же являются соотношением сторон
	vw_SetAspectRatio(Setup.fAspectRatioWidth, Setup.fAspectRatioHeight, true);





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// скрываем системный курсор
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!NeedShowSystemCursor)
		SDL_ShowCursor(SDL_DISABLE);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// загрузка текстур согласно моделе загрузки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	LoadGameData(-1);
	// загрузка списка миссий
	MissionsListInit();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка процедуры... основного цикла и переход на нее
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	CanQuit = true;


	// первоначальная установка курсора
	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	vw_SetMousePos(mouse_x, mouse_y);

	// первоначальный сброс кнопок мышки
	for (int i=0; i<8; i++)
		SDL_MouseCurrentStatus[i] = false;



	// если первый запуск игры - предлогаем выбор языка
	if (FirstStart) SetCurrentDialogBox(16);



loop:


	Quit = false;
	NeedReCreate = false;
	bool NeedLoop = true;


	// начальные установки джойстика
	JoystickAxisX = 0;
	JoystickAxisY = 0;
	float JoystickCurentTime = vw_GetTime();
	float JoystickTimeDelta = 0.0f;
#ifdef joystick
	if (Joystick != NULL)
	{
		JoystickAxisX = SDL_JoystickGetAxis(Joystick, 0);
		JoystickAxisY = SDL_JoystickGetAxis(Joystick, 1);

		for (int i=0; i<100; i++) JoysticButtons[i] = false;
	}
#endif

	// сразу задаем режим работы с юникодом, чтобы SDL давал нам юникод при нажатии на клавишу
	SDL_EnableUNICODE(1);

	while(!Quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event) )
		{
			switch (event.type)
			{
				// если нажали закрыть окно
				case SDL_QUIT:
					Quit = true;
					goto GotoQuit;
					break;

				// работаем с движением мышки
				case SDL_MOUSEMOTION:
                    vw_SetMousePos(event.motion.x, event.motion.y);
                    //vw_SetMousePosRel(event.motion.xrel, event.motion.yrel);
					break;

				// обрабатываем кнопки мыши
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						vw_SetWindowLBMouse(true);
						vw_SetWindowLBDoubleMouse(true);
					}
					if (event.button.button ==  SDL_BUTTON_RIGHT)
						vw_SetWindowRBMouse(true);
					if (event.button.button<8) // на всякий случай небольшая проверка
						SDL_MouseCurrentStatus[event.button.button] = true;
					break;
				case SDL_JOYBUTTONDOWN:
					vw_SetWindowLBMouse(true);
					vw_SetWindowLBDoubleMouse(true);
					JoysticButtons[event.jbutton.button] = true;
					break;

				case SDL_MOUSEBUTTONUP:
					if (event.button.button ==  SDL_BUTTON_LEFT)
						vw_SetWindowLBMouse(false);
					if (event.button.button ==  SDL_BUTTON_RIGHT)
						vw_SetWindowRBMouse(false);
					if (event.button.button<8) // на всякий случай небольшая проверка
						SDL_MouseCurrentStatus[event.button.button] = false;
					break;
				case SDL_JOYBUTTONUP:
					vw_SetWindowLBMouse(false);
					JoysticButtons[event.jbutton.button] = false;
					break;

				case SDL_ACTIVEEVENT:
					if(event.active.state & SDL_APPACTIVE)
					{
						// iconified
						if( event.active.gain == 0 ) NeedLoop = false;
						// restored
						if( event.active.gain == 1 )  NeedLoop = true;
					}
					if(event.active.state & SDL_APPINPUTFOCUS)
					{
						// input focus was lost
						if( event.active.gain == 0 ) NeedLoop = false;
						// input focus was gained
						if( event.active.gain == 1 )  NeedLoop = true;
					}
					break;

				case SDL_KEYDOWN:
					// устанавливаем текущий юникод нажатоу клавиши
					if (event.key.keysym.unicode > 0x001F) // пропускаем С0 Сontrol Characters (ентер, бэкспейс и прочие...)
					if (!((event.key.keysym.unicode >= 0x007F) & (event.key.keysym.unicode <= 0x009F))) // пропускаем С1 Сontrol Characters
					if (event.key.keysym.unicode != 0x0025) // !!! потом разобраться, на символе % безбожно глючит
						vw_SetCurrentKeyUnicod(event.key.keysym.unicode);
#ifdef gamedebug
					printf("Keydown, Unicode: " );
					if ( event.key.keysym.unicode < 0x80 && event.key.keysym.unicode > 0 )
					{
						printf( "%c (0x%04X)\n", (char)event.key.keysym.unicode,
								event.key.keysym.unicode );
					}
					else
					{
						printf( "? (0x%04X)\n", event.key.keysym.unicode );
					}
#endif // gamedebug
					break;

				case SDL_KEYUP:
					// сбрасываем юникод
					vw_SetCurrentKeyUnicod(0);
					break;

				default:
					break;
			}
		}

		// если есть джойстик, считаем для него время
		if (Joystick != NULL)
		{
			JoystickTimeDelta = vw_GetTime() - JoystickCurentTime;
			JoystickCurentTime = vw_GetTime();
		}

		// если окно видемое - рисуем
		if (NeedLoop)
		{

#ifdef joystick
			// управление джойстиком
			if (Joystick != NULL)
			{
				int X = SDL_JoystickGetAxis(Joystick, 0);
				int Y = SDL_JoystickGetAxis(Joystick, 1);

				if (JoystickAxisX != X || JoystickAxisY != Y)
				{
					JoystickAxisX = 0;
					JoystickAxisY = 0;

					int Xsm = (int)(1000*(X/32768.0f)*JoystickTimeDelta);
					int Ysm = (int)(1000*(Y/32768.0f)*JoystickTimeDelta);

					vw_SetMousePosRel(Xsm, Ysm);
				}
			}
#endif

			// всегда включаем счет времени
			vw_StartTime();
			// переход на основную программу
			Loop_Proc();

		}
		else
		{
			// всегда выключаем счет времени и считаем новый лаг
			vw_StopTime();
			// выключаем музыку
			if (vw_GetMusicIsPlaying()) vw_ReleaseAllMusic();

			// если в игре, ставим паузу, т.е. открываем меню мгновенно
			if (GameStatus == GAME && GameContentTransp < 1.0f)
			{
				NeedShowGameMenu = true;
				NeedHideGameMenu = false;
				GameContentTransp = 1.0f;
				vw_SetTimeThreadSpeed(1, 0.0f);
				DrawGameCursor = true;
			}

			// ждем пока не поступит команда
			SDL_WaitEvent(NULL);
		}
	}
GotoQuit:

	// если не выходим...
	if (!NeedReCreate)
	if (!CanQuit && Quit)
	{
		SetCurrentDialogBox(0);
		goto loop;
	}



	if (!NeedShowSystemCursor)
		SDL_ShowCursor(SDL_ENABLE);


	// завершение, и освобождение памяти...
	if (Script != 0){delete Script; Script = 0;}

	RealExitGame();// удаляем объекты, они могут быть...
	MissionsListRelease();
	if (CursorParticleSystem2D != 0){delete CursorParticleSystem2D; CursorParticleSystem2D = 0;}
	DestroyInfoObject();
	WorkshopDestroyData();

	if (psSpaceStatic!=0){delete psSpaceStatic; psSpaceStatic = 0;}

	ReleaseAllObject3D();
	vw_ReleaseAllModel3D();
	vw_ReleaseAllParticleSystems();
	vw_ReleaseAllLights();
	ReleaseAllGameLvlText();

	vw_ShutdownFont();
	vw_ReleaseAllTextures();
	ShadowMap_Release();
	vw_ShutdownRenderer();

#ifdef joystick
	// закрываем джойстик, если он был
	if(SDL_NumJoysticks()>0)
		if(SDL_JoystickOpened(Setup.JoystickNum))
			if (Joystick != NULL)
				SDL_JoystickClose(Joystick);
#endif

	// полностью выходим из SDL
	SDL_Quit();
	// сохраняем настройки игры
	SaveXMLSetupFile();
	// освобождаем весь подготовленный текст из языкового xml файла
	ReleaseGameText();

	// если нужно перезагрузить игру с новыми параметрами
	if (NeedReCreate)
	{
		FirstStart = false;

		// убираем все голосовые файлы и звуки (для изменения языка голоса)
		// при vw_ShutdownSound освободим все, сейчас только речевые, музыка должна играть
		vw_ReleaseAllBuffers();
		// убираем все линки (для изменения языка меню)
		// при vw_ShutdownVFS вообще все освободим, сейчас убираем только языковые линки на файлы
		vw_DeleteAllLinksVFS();

		// пересоздаем окно
	    goto ReCreate;
	}


	if (VideoModes != 0) {delete [] VideoModes; VideoModes = 0;}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// завершение, освобождение памяти...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!Setup.Music_check || !Setup.Sound_check)
	{
		vw_ShutdownSound();
	}


	vw_ShutdownFont();
	// закрываем файловую систему
	vw_ShutdownVFS();



	// уходим из программы...
	return 0;

}
