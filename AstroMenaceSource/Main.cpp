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
#define _WIN32_WINNT 0x0501 // оверрайдим версию до Windows XP, нужно для получение доступа к функциям
#include <windows.h>

#define EDD_GET_DEVICE_INTERFACE_NAME 0x00000001
#define DISPLAY_DEVICE_ACTIVE              0x00000001
#define DISPLAY_DEVICE_MIRRORING_DRIVER    0x00000008
#define DISPLAY_DEVICE_PRIMARY_DEVICE      0x00000004

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














#ifdef WIN32
//HANDLE Mutex;
#endif

// инициализация механизма запуска 1 копии игры
bool CreateGameOneCopy()
{

#ifdef WIN32

/*	Mutex = CreateMutex(NULL, false, "AstroMenaceGameMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return false;
	}
	else
	{
		return true;
	}*/

#elif __unix


return true;

#endif


}

// удаление механизма запуска 1 копии игры
void ReleaseGameOneCopy()
{

#ifdef WIN32

//	ReleaseMutex(Mutex);

#elif __unix


#endif

}






//------------------------------------------------------------------------------------
// основная процедура...
//------------------------------------------------------------------------------------
int main( int argc, char **argv )
{
	// проверка, что запустили 1 копию
	if (!CreateGameOneCopy()) return 0;





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
#elif __unix
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
				strncpy(ProgrammDir, argv[i]+strlen("--dir="), strlen(argv[i])-strlen("--dir=")+1);
			else
			{
				strcpy(ProgrammDir, homeval);// -1, это тильда... а в кол-ве нет, т.к. /0 там должен остаться
				strncat(ProgrammDir, argv[i]+strlen("--dir=")+1, strlen(argv[i])-strlen("--dir="));
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
	// флаг проверять ли режимы антиалиасинга при старте
	bool NeedCheckAA = true;
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

			printf("--dir=/game/data/folder/ - folder with gamedata.vfs file\n");
			printf("--mouse - launch the game without system cursor hiding.\n");
			printf("--noAA - disable AA antialiasing test at the game start.\n");
			printf("--safe-mode - reset all settings not connected to Pilots Profiles at the game launch.\n");
			printf("--pack - pack data to gamedata.vfs file\n");
			printf("--rawdata=/game/rawdata/folder/ - folder with game raw data for gamedata.vfs.\n");
			printf("--help - info about all game launch options.\n");

			// выходим, только показываем, игру не запускаем
			ReleaseGameOneCopy();
			return 0;
		}

		// проверка ключа "--mouse"
		if (!strcmp(argv[i], "--mouse"))
		{
			NeedShowSystemCursor = true;
		}

		// проверка ключа "--noAA"
		if (!strcmp(argv[i], "--noAA"))
		{
			NeedCheckAA = false;
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
	printf("AstroMenace %1.1f %i\n\n", GAME_VERSION_VERSION, GAME_VERSION_BUILD);









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
#ifdef __unix
				// если передали относительный путь в папку пользователя с тильдой
				if (argv[i][sizeof("--rawdata")] != '~')
					strncpy(RawDataDir, argv[i]+strlen("--rawdata="), strlen(argv[i])-strlen("--rawdata=")+1);
				else
				{
					strcpy(RawDataDir, homeval);// -1, это тильда... а в кол-ве нет, т.к. /0 там должен остаться
					strncat(RawDataDir, argv[i]+strlen("--rawdata=")+1, strlen(argv[i])-strlen("--rawdata="));
				}
#endif // unix
				// если в конце нет слеша - ставим его
				if (RawDataDir[strlen(RawDataDir)-1] != '/')
					strncat(RawDataDir, "/", strlen("/"));

			}
		}

		return ConvertFS2VFS(RawDataDir);
	}









	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если нужно, проверяем какие поддерживает режимы аа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedCheckAA)
	{
		printf("Antialiasing test START\n");
		printf("Use '--noAA' launch option in order to avoid Antialiasing test.\n");
		vw_TestAAModes(640, 480);
		printf("Antialiasing test END\n");
	}






	// инициализация счета времени
	vw_InitTime();






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка звука, всегда до LoadGameData
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool SoundInitSuccess = InitAudio();
	if (!SoundInitSuccess) fprintf(stderr, "Unable to open audio!\n");
	printf("\n");




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// подключаем VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (vw_OpenVFS(VFSFileNamePath) != 0)
	{
		fprintf(stderr, "gamedata.vfs file not found or corrupted.\n");
		ReleaseGameOneCopy();
		return 0;
	}
	printf("\n");




ReCreate:

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
		ReleaseGameOneCopy();
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
		printf("Xinerama is active\n");
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

#ifdef WIN32
	// пытаемся определить в Windows основной экран и его разрешение

	DWORD i = 0;
	DISPLAY_DEVICE dc;
	dc.cb = sizeof(dc);

	// перебираем все мониторы
	while(EnumDisplayDevices(NULL, i, &dc, EDD_GET_DEVICE_INTERFACE_NAME) != 0)
	{
		if ((dc.StateFlags & DISPLAY_DEVICE_ACTIVE) && !(dc.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
		{
			DEVMODE dm;
			if (EnumDisplaySettings(dc.DeviceName, 0, &dm) != 0)
			{
				printf("Screen #%d: %d x %d\n", i, dm.dmPelsWidth, dm.dmPelsHeight);

				// ищем активный, не псевдо, основной
				if (dc.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
				{
					CurrentVideoMode.W = dm.dmPelsWidth;
					CurrentVideoMode.H = dm.dmPelsHeight;
				}
			}
		}

		++i;
	}

#endif // WIN32





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
			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, 16, SDL_FULLSCREEN | SDL_HWSURFACE) != 0) VideoModesNum++;
			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, 32, SDL_FULLSCREEN | SDL_HWSURFACE) != 0) VideoModesNum++;
			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, CurrentVideoMode.BPP, SDL_ANYFORMAT | SDL_HWSURFACE) != 0) VideoModesNum++;
		}


		VideoModes = new sVideoModes[VideoModesNum];

		int k=0;
		for(int i=0; i<AllSupportedModesCount; i++)
		if ((AllSupportedModes[i].W <= CurrentVideoMode.W) & (AllSupportedModes[i].H <= CurrentVideoMode.H))
		{
			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, 16, SDL_FULLSCREEN | SDL_HWSURFACE) != 0)
			{
				VideoModes[k].W = AllSupportedModes[i].W;
				VideoModes[k].H = AllSupportedModes[i].H;
				VideoModes[k].BPP = 16;
				k++;
			}

			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, 32, SDL_FULLSCREEN | SDL_HWSURFACE) != 0)
			{
				VideoModes[k].W = AllSupportedModes[i].W;
				VideoModes[k].H = AllSupportedModes[i].H;
				VideoModes[k].BPP = 32;
				k++;
			}

			if (SDL_VideoModeOK(AllSupportedModes[i].W, AllSupportedModes[i].H, CurrentVideoMode.BPP, SDL_ANYFORMAT | SDL_HWSURFACE) != 0)
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
	// работа с файлом данных...
	// если первый запуск (нет файла настроек), тянем переменную
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FirstStart = LoadXMLSetupFile(NeedSafeMode);
	// если загруженные параметры, больше чем максимальные, ставим максимальные (если Xinerama)
	if ((VideoModes[VideoModesNum-1].W <= Setup.Width) & (VideoModes[VideoModesNum-1].H <= Setup.Height))
	{
		Setup.Width = VideoModes[VideoModesNum-1].W;
		Setup.Height = VideoModes[VideoModesNum-1].H;
		Setup.BPP = CurrentVideoMode.BPP;
	}










	// делаем правильные симлинки на языковые ресурсы, автоматизации нет, просто линкуем нужные файлы

	switch (Setup.MenuLanguage)
	{
		case 1: //en
			vw_CreateEntryLinkVFS("DATA/DATA_EN/GAME/mission.tga", "DATA/GAME/mission.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/GAME/missionfailed.tga", "DATA/GAME/missionfailed.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/GAME/pause.tga", "DATA/GAME/pause.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/LOADING/loading_back.tga", "DATA/LOADING/loading_back.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/SCRIPT/text.xml", "DATA/SCRIPT/text.xml");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/button_weaponry_in.tga", "DATA/MENU/button_weaponry_in.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/button_weaponry_out.tga", "DATA/MENU/button_weaponry_out.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_engine1.tga", "DATA/MENU/system_engine1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_engine2.tga", "DATA/MENU/system_engine2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_engine3.tga", "DATA/MENU/system_engine3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_engine4.tga", "DATA/MENU/system_engine4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_engine_empty.tga", "DATA/MENU/system_engine_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_mechan1.tga", "DATA/MENU/system_mechan1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_mechan2.tga", "DATA/MENU/system_mechan2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_mechan3.tga", "DATA/MENU/system_mechan3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_mechan4.tga", "DATA/MENU/system_mechan4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_mechan_empty.tga", "DATA/MENU/system_mechan_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_power1.tga", "DATA/MENU/system_power1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_power2.tga", "DATA/MENU/system_power2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_power3.tga", "DATA/MENU/system_power3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_power4.tga", "DATA/MENU/system_power4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_power_empty.tga", "DATA/MENU/system_power_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_protect1.tga", "DATA/MENU/system_protect1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_protect2.tga", "DATA/MENU/system_protect2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_protect3.tga", "DATA/MENU/system_protect3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_protect4.tga", "DATA/MENU/system_protect4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_protect_empty.tga", "DATA/MENU/system_protect_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_target1.tga", "DATA/MENU/system_target1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_target2.tga", "DATA/MENU/system_target2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_target3.tga", "DATA/MENU/system_target3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_target4.tga", "DATA/MENU/system_target4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/system_target_empty.tga", "DATA/MENU/system_target_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/weapon_empty1_icon.tga", "DATA/MENU/weapon_empty1_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/weapon_empty2_icon.tga", "DATA/MENU/weapon_empty2_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/weapon_empty3_icon.tga", "DATA/MENU/weapon_empty3_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/weapon_empty4_icon.tga", "DATA/MENU/weapon_empty4_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/weapon_empty5_icon.tga", "DATA/MENU/weapon_empty5_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_EN/MENU/weapon_empty_icon.tga", "DATA/MENU/weapon_empty_icon.tga");
			break;

		case 2: //de
			vw_CreateEntryLinkVFS("DATA/DATA_DE/GAME/mission.tga", "DATA/GAME/mission.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/GAME/missionfailed.tga", "DATA/GAME/missionfailed.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/GAME/pause.tga", "DATA/GAME/pause.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/LOADING/loading_back.tga", "DATA/LOADING/loading_back.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/SCRIPT/text.xml", "DATA/SCRIPT/text.xml");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/button_weaponry_in.tga", "DATA/MENU/button_weaponry_in.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/button_weaponry_out.tga", "DATA/MENU/button_weaponry_out.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_engine1.tga", "DATA/MENU/system_engine1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_engine2.tga", "DATA/MENU/system_engine2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_engine3.tga", "DATA/MENU/system_engine3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_engine4.tga", "DATA/MENU/system_engine4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_engine_empty.tga", "DATA/MENU/system_engine_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_mechan1.tga", "DATA/MENU/system_mechan1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_mechan2.tga", "DATA/MENU/system_mechan2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_mechan3.tga", "DATA/MENU/system_mechan3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_mechan4.tga", "DATA/MENU/system_mechan4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_mechan_empty.tga", "DATA/MENU/system_mechan_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_power1.tga", "DATA/MENU/system_power1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_power2.tga", "DATA/MENU/system_power2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_power3.tga", "DATA/MENU/system_power3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_power4.tga", "DATA/MENU/system_power4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_power_empty.tga", "DATA/MENU/system_power_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_protect1.tga", "DATA/MENU/system_protect1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_protect2.tga", "DATA/MENU/system_protect2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_protect3.tga", "DATA/MENU/system_protect3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_protect4.tga", "DATA/MENU/system_protect4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_protect_empty.tga", "DATA/MENU/system_protect_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_target1.tga", "DATA/MENU/system_target1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_target2.tga", "DATA/MENU/system_target2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_target3.tga", "DATA/MENU/system_target3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_target4.tga", "DATA/MENU/system_target4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/system_target_empty.tga", "DATA/MENU/system_target_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/weapon_empty1_icon.tga", "DATA/MENU/weapon_empty1_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/weapon_empty2_icon.tga", "DATA/MENU/weapon_empty2_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/weapon_empty3_icon.tga", "DATA/MENU/weapon_empty3_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/weapon_empty4_icon.tga", "DATA/MENU/weapon_empty4_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/weapon_empty5_icon.tga", "DATA/MENU/weapon_empty5_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_DE/MENU/weapon_empty_icon.tga", "DATA/MENU/weapon_empty_icon.tga");
			break;

		case 3: //ru
			vw_CreateEntryLinkVFS("DATA/DATA_RU/GAME/mission.tga", "DATA/GAME/mission.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/GAME/missionfailed.tga", "DATA/GAME/missionfailed.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/GAME/pause.tga", "DATA/GAME/pause.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/LOADING/loading_back.tga", "DATA/LOADING/loading_back.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/SCRIPT/text.xml", "DATA/SCRIPT/text.xml");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/button_weaponry_in.tga", "DATA/MENU/button_weaponry_in.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/button_weaponry_out.tga", "DATA/MENU/button_weaponry_out.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_engine1.tga", "DATA/MENU/system_engine1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_engine2.tga", "DATA/MENU/system_engine2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_engine3.tga", "DATA/MENU/system_engine3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_engine4.tga", "DATA/MENU/system_engine4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_engine_empty.tga", "DATA/MENU/system_engine_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_mechan1.tga", "DATA/MENU/system_mechan1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_mechan2.tga", "DATA/MENU/system_mechan2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_mechan3.tga", "DATA/MENU/system_mechan3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_mechan4.tga", "DATA/MENU/system_mechan4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_mechan_empty.tga", "DATA/MENU/system_mechan_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_power1.tga", "DATA/MENU/system_power1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_power2.tga", "DATA/MENU/system_power2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_power3.tga", "DATA/MENU/system_power3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_power4.tga", "DATA/MENU/system_power4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_power_empty.tga", "DATA/MENU/system_power_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_protect1.tga", "DATA/MENU/system_protect1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_protect2.tga", "DATA/MENU/system_protect2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_protect3.tga", "DATA/MENU/system_protect3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_protect4.tga", "DATA/MENU/system_protect4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_protect_empty.tga", "DATA/MENU/system_protect_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_target1.tga", "DATA/MENU/system_target1.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_target2.tga", "DATA/MENU/system_target2.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_target3.tga", "DATA/MENU/system_target3.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_target4.tga", "DATA/MENU/system_target4.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/system_target_empty.tga", "DATA/MENU/system_target_empty.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/weapon_empty1_icon.tga", "DATA/MENU/weapon_empty1_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/weapon_empty2_icon.tga", "DATA/MENU/weapon_empty2_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/weapon_empty3_icon.tga", "DATA/MENU/weapon_empty3_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/weapon_empty4_icon.tga", "DATA/MENU/weapon_empty4_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/weapon_empty5_icon.tga", "DATA/MENU/weapon_empty5_icon.tga");
			vw_CreateEntryLinkVFS("DATA/DATA_RU/MENU/weapon_empty_icon.tga", "DATA/MENU/weapon_empty_icon.tga");
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

	// загружаем все текстовые данные
	InitGameText("DATA/SCRIPT/text.xml");








	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем и иним джойстик
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef joystick
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == 0)
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
#endif



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка параметров звука
	// фактически, нам не нужно хранить эти данные
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!SoundInitSuccess)
	{
		Setup.Music_check = false;
		Setup.Sound_check = false;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// делаем окно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FullScreen = false;
	if (Setup.BPP != 0) FullScreen = true;


#ifdef WIN32
	// если надо, меняем режим экрана и ставим правильную частоту обновления
	if (Setup.ScreenFrequency != 0)
	{
		DEVMODE dmScreenSettings;
		ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));
		dmScreenSettings.dmSize	= sizeof (DEVMODE);
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		dmScreenSettings.dmPelsWidth		= Setup.Width;
		dmScreenSettings.dmPelsHeight		= Setup.Height;
		dmScreenSettings.dmBitsPerPel		= Setup.BPP;
		dmScreenSettings.dmDisplayFrequency	= Setup.ScreenFrequency;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
#endif // WIN32


	int InitStatus = vw_InitRenderer("AstroMenace", Setup.Width, Setup.Height, &Setup.BPP, FullScreen, &Setup.MultiSampleType, CurrentVideoModeX, CurrentVideoModeY, CurrentVideoMode.W, CurrentVideoMode.H);

	// ошибка окна (размеры)
	if (InitStatus == 1)
	{
		// не можем создать окна, ставим текущее разрешение рабочего стола
		if (CurrentVideoMode.W != Setup.Width ||
			CurrentVideoMode.H != Setup.Height ||
			CurrentVideoMode.BPP != Setup.BPP)
		{
			Setup.Width = CurrentVideoMode.W;
			Setup.Height = CurrentVideoMode.H;
			Setup.BPP = CurrentVideoMode.BPP;
			Setup.MultiSampleType = 0;
			SaveXMLSetupFile();
			goto ReCreate;
		}

		fprintf(stderr, "Wrong resolution. Fatal error.\n");
#ifdef WIN32
		MessageBox(NULL,"Wrong resolution. Please, install the newest video drivers from your video card vendor.", "Render system - Fatal Error",MB_OK|MB_APPLMODAL|MB_ICONERROR);
#endif // WIN32
		SDL_Quit();
		ReleaseGameOneCopy();
		return 0;									// Quit If Window Was Not Created
	}
	// не поддерживаем что-то
	if (InitStatus == 2)
	{
        fprintf(stderr, "Some features are not supported.\n");
#ifdef WIN32
		MessageBox(NULL,"OpenGL 1.3 required. Please, install the newest video drivers from your video card vendor.", "Render system - Fatal Error",MB_OK|MB_APPLMODAL|MB_ICONERROR);
#endif // WIN32
		SDL_Quit();
		ReleaseGameOneCopy();
		return 0;									// Quit If Window Was Not Created
	}

	// даже если 24, все равно тянем как 32 бита, а если меньше 16, то ставим 16
	if (Setup.BPP <= 16) Setup.BPP = 16;
	if (Setup.BPP > 16) Setup.BPP = 32;
	// иначе сбрасывает в текущий, а нам надо тянуть именно ноль
	if (!FullScreen) Setup.BPP = 0;





	// получаем возможности железа
	CAPS = vw_GetDevCaps();


	// если не поддерживаем как минимум 2 мультитекстуры, железо очень слабое - не запустимся
	if (CAPS->MaxMultTextures < 2)
	{
		fprintf(stderr, "The Multi Textures feature unsupported by hardware. Fatal error.\n");
		vw_ShutdownRenderer();
		SDL_Quit();
		ReleaseGameOneCopy();
		return 0;
	}

	// если нужно, устанавливаем перерытие значений внутри движка, може только выключить - включить то чего нет нельзя
	if (Setup.VBOCoreMode == 0 && CAPS->VBOSupported)
	{
		CAPS->VBOSupported = false;
	}
	else // если нет возможности, сбрасываем флаг
		if (!CAPS->VBOSupported) Setup.VBOCoreMode = 0;

	// проверка поддержки шейдеров (нужна 100% поддержка GLSL 1.0)
	if (Setup.UseGLSL)
		if (!CAPS->GLSL100Supported || CAPS->ShaderModel < 3.0f) Setup.UseGLSL = false;

	// установка режима работы менеджера приоритетов
	CAPS->ForceTexturesPriorManager = Setup.ForceTexturesPriorManager;

	// управление генерации мипмеп уровней- можем только выключить, нельзя включить если его нет
	if (CAPS->HardwareMipMapGeneration && !Setup.HardwareMipMapGeneration)
	{
		CAPS->HardwareMipMapGeneration = false;
	}
	else // если нет возможности, сбрасываем флаг
		if (!CAPS->HardwareMipMapGeneration) Setup.HardwareMipMapGeneration = false;


	// анализ системы только если это первый запуск
	if (FirstStart)
	// если шейдерная модель 3-я или выше, можно смело ставить
	if (CAPS->ShaderModel >= 3.0f)
	{
		// памяти достаточно, включаем другой режим загрузки
		Setup.EqualOrMore128MBVideoRAM = true;

		// если шейдерная модель 3.0 или выше, значит GLSL 1.0 работает на 100%
		// в линуксе у карты нвидия 6600 почему-то шейдерная модель 3.0, а не 2.0 - не включать шейдеры вообще, пусть человек включает
		// if (CAPS->ShaderModel >= 3.0f) Setup.UseGLSL = true;
	}




	// ставим данные к которым приводить, плюс они же являются соотношением сторон
	vw_SetAspectRatio(Setup.fAspectRatioWidth, Setup.fAspectRatioHeight, true);


	// установка яркости, если полный экран
	if (Setup.BPP != 0)
		vw_SetGammaRamp(1.0f + ((Setup.Gamma-2)/10.0f), 1.0f, 1.0f);



	// настройка менеджера систем частиц
	vw_SetParticleSystemStates(Setup.UseGLSL, Setup.ParticlesPerSecQuality*1.0f);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// скрываем системный курсор
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!NeedShowSystemCursor)
		SDL_ShowCursor(SDL_DISABLE);


	// иним фонт
	vw_InitFont("DATA/FONT/LiberationMono-Bold.ttf", 16);


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
					if (!(event.key.keysym.mod & KMOD_CTRL))
						vw_SetCurrentKeyUnicod(event.key.keysym.unicode);
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


	// заверщение, и освобождение памяти...
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
	ReleaseAllText();

	vw_ReleaseFont();
	vw_ReleaseAllTextures();
	vw_ShutdownRenderer();

#ifdef joystick
	// закрываем джойстик, если он был
	if(SDL_NumJoysticks()>0)
		if(SDL_JoystickOpened(Setup.JoystickNum))
			if (Joystick != NULL)
				SDL_JoystickClose(Joystick);
#endif




	SDL_Quit();

	SaveXMLSetupFile();

	ReleaseGameText();


	if (NeedReCreate)
	{

#ifdef WIN32
		// проблема в SDL
		// если пересоздаем окно с полноэкранного режима, или пересоздаем окно меньшего размера
		// titlebar перестает работать - его считает активной областью окна...
		// пока вот такое кривое решение - перезапускать игру

		// полностью выходим из игры
		if (VideoModes != 0) {delete [] VideoModes; VideoModes = 0;}
		if (!Setup.Music_check || !Setup.Sound_check)
		{
			vw_ShutdownSound();
		}
		ReleaseGameText();
		// закрываем файловую систему
		vw_ShutdownVFS();
		// уходим из программы...
		ReleaseGameOneCopy();

		// запускаем новую копию игры
		char FileName[MAX_PATH];
		ZeroMemory(FileName, sizeof(FileName));
		GetModuleFileName(NULL, FileName, MAX_PATH);
		// сначало пытаемся запустить через ShellExecute, если не получается - WinExec
		HINSTANCE result = ShellExecute(NULL, "open", FileName, NULL,NULL, SW_SHOWNORMAL);
		if ((UINT)result <= HINSTANCE_ERROR) WinExec(FileName,SW_SHOWNORMAL);

		// завершаем процесс этой копии игры
		return 0;
#endif

		// убираем все голосовые файлы и звуки (для изменения языка голоса)
		vw_ReleaseAllBuffers();
		// убираем все линки (для изменения языка меню)
		vw_DeleteAllLinksVFS();

		// для линукса - просто пересоздаем окно
	    goto ReCreate;
	}


	if (VideoModes != 0) {delete [] VideoModes; VideoModes = 0;}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// заверщение, освобождение памяти...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!Setup.Music_check || !Setup.Sound_check)
	{
		vw_ShutdownSound();
	}


	// закрываем файловую систему
	vw_ShutdownVFS();



	// уходим из программы...
	ReleaseGameOneCopy();
	return 0;

}
