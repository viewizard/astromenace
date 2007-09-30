/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Main.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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








//------------------------------------------------------------------------------------
// глобальные переменные переменные
//------------------------------------------------------------------------------------
// тайтл и версия игры
char GAME_VERSION[100];
char GAME_TITLE[100];


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
#ifdef WIN32
SHGETSPECIALFOLDERPATH pSHGetSpecialFolderPath = 0;
#endif // WIN32
// статус загрузки, должно быть ОбщееКол-воМиссий+1(для меню)
bool LoadedTypes[1000];


//------------------------------------------------------------------------------------
// пути к файлам
//------------------------------------------------------------------------------------
// полное путь к программе
char ProgrammDir[MAX_PATH];
char VFSFileNamePath[MAX_PATH];
char VFSLangFileNamePath[MAX_PATH];
char VFSLangFileNamePathEn[MAX_PATH];
// полное имя для файла с данными о игре
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
// кодируем-декодируем
//------------------------------------------------------------------------------------
void CodeXOR(char *Text, char *Key, int Count)
{
	int k=0;
	for (int i=0; i < Count; i++)
	{
		if (Text[i] != '\0')
			Text[i] = Text[i]^Key[k];
		k++;
		// чтобы не было переполнения
		if (k >= strlen(Key)) k=0;
	}
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
	ZeroMemory(VFSLangFileNamePath, sizeof(VFSLangFileNamePath));
	ZeroMemory(VFSLangFileNamePathEn, sizeof(VFSLangFileNamePathEn));
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

	strcpy(VFSLangFileNamePath, ProgrammDir);
	strcat(VFSLangFileNamePath, "gamelang.vfs");

	strcpy(VFSLangFileNamePathEn, ProgrammDir);
	strcat(VFSLangFileNamePathEn, "gamelang_en.vfs");

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
			if (argv[i][6] != '~')
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
		strcpy(ProgrammDir, argv[0]);
		char* s = strrchr(ProgrammDir,'/');
		if (s) s[0]=0x0;
		const char *Fi = "/";
		strcat( ProgrammDir, Fi );
	}


	strcpy(ScreenshotDir, homeval);
	strcat(ScreenshotDir, "/Desktop/AstroMenaceScreenshot");

	strcpy(VFSFileNamePath, ProgrammDir);
	strcat(VFSFileNamePath, "gamedata.vfs");

	strcpy(VFSLangFileNamePath, ProgrammDir);
	strcat(VFSLangFileNamePath, "gamelang.vfs");

	strcpy(VFSLangFileNamePathEn, ProgrammDir);
	strcat(VFSLangFileNamePathEn, "gamelang_en.vfs");

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
	// флаг проверки принудительной установки разрешения
	sVideoModes ForceResolution;
	ForceResolution.W = -1;
	ForceResolution.H = -1;
	ForceResolution.BPP = -1;

	for (int i=1; i<argc; i++)
	{
		// проверка ключа "--help"
		if (!strcmp(argv[i], "--help"))
		{
			printf("AstroMenace launch options:\n\n");

			printf("--dir=/game/data/folder/ - folder with gamedata.vfs file\n");
			printf("--mouse - launch the game without system cursor hiding.\n");
			printf("--noAA - disable AA antialiasing test at the game start.\n");
			printf("--mode=N - set game windows mode and resolution (forced)\n");
			printf("  where N is number from 0 till 17:\n");
			printf("    Fullscreen modes\n");
			printf("    0: 640x480x16\n");
			printf("    1: 640x480x32\n");
			printf("    2: 800x600x16\n");
			printf("    3: 800x600x32\n");
			printf("    4: 1024x768x16\n");
			printf("    5: 1024x768x32\n");
			printf("    6: 1280x1024x16\n");
			printf("    7: 1280x1024x32\n");
			printf("    8: 1400x1050x16\n");
			printf("    9: 1400x1050x32\n");
			printf("    10: 1600x1200x16\n");
			printf("    11: 1600x1200x32\n");
			printf("    Windowed modes\n");
			printf("    12: 640x480\n");
			printf("    13: 800x600\n");
			printf("    14: 1024x768\n");
			printf("    15: 1280x1024\n");
			printf("    16: 1400x1050\n");
			printf("    17: 1600x1200\n");
			printf("--safe-mode - reset all settings not connected to Pilots Profiles at the game launch.\n");
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

		// проверка ключа "--mode"
		if (!strncmp(argv[i], "--mode", strlen("--mode")))
		{
			sVideoModes WindowsMode[18] =
			{
				{640, 480, 16, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{640, 480, 32, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{800, 600, 16, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{800, 600, 32, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1024, 768, 16, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1024, 768, 32, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1280, 1024, 16, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1280, 1024, 32, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1400, 1050, 16, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1400, 1050, 32, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1600, 1200, 16, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1600, 1200, 32, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{640, 480, 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{800, 600, 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1024, 768, 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1280, 1024, 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1400, 1050, 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
				{1600, 1200, 0, {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
			};

			char* s = strrchr(argv[i],'=');
			int Mode;
			Mode = atoi(s+1);// берем со 2-го значения, т.к. первое это символ "="
			ForceResolution.W = WindowsMode[Mode].W;
			ForceResolution.H = WindowsMode[Mode].H;
			ForceResolution.BPP = WindowsMode[Mode].BPP;
		}
	}







	// версия
	printf("AstroMenace %1.1f %i\n\n", GAME_VERSION_ID, GAME_VERSION_BUILD);






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
	// подключаем VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (vw_OpenVFS(VFSFileNamePath) != 0)
	{
		fprintf(stderr, "gamedata.vfs file not found or corrupted.\n");
		ReleaseGameOneCopy();
		return 0;
	}
	if (vw_OpenVFS(VFSLangFileNamePath) != 0)
	{
		if (vw_OpenVFS(VFSLangFileNamePathEn) != 0)
		{
			fprintf(stderr, "gamelang.vfs or gamelang_en.vfs files not found or corrupted.\n");
			ReleaseGameOneCopy();
			return 0;
		}
	}
	printf("\n");





	// загружаем все текстовые данные
#ifdef EN
	InitGameText("DATA/SCRIPT/text.xml");
#endif // EN
#ifdef DE
	InitGameText("DATA/SCRIPT/text_de.xml");
#endif // DE
#ifdef RU
	InitGameText("DATA/SCRIPT/text_ru.xml");
#endif // RU




	// "AstroMenace"
	char AMKey[] = "$%^%12364#$%@";
	char AMText[] = {101, 86, 42, 87, 94, 127, 86, 88, 85, 64, 65, '\0'};
	CodeXOR(AMText, AMKey, 12);

	// собираем тайтл
	strcpy(GAME_TITLE, AMText);


	// собираем версию
#if defined(DEMO_VERSION) || defined(DEMO_VERSION_FULL_VFS)
	sprintf(GAME_VERSION, "%s %1.1f %s %i", GetText("11_DEMO"), GAME_VERSION_ID, GetText("11_build"), GAME_VERSION_BUILD);
#else
	sprintf(GAME_VERSION, "%s %1.1f %s %i", GetText("11_Version"), GAME_VERSION_ID, GetText("11_build"), GAME_VERSION_BUILD);
#endif // DEMO_VERSION







	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка звука, всегда до LoadGameData
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool SoundInitSuccess = InitAudio();
	if (!SoundInitSuccess) fprintf(stderr, "Unable to open audio!\n");



ReCreate:



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// иним SDL
	// это нужно сделать сразу, чтобы правильно поставить разрешение
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	if ( SDL_WasInit(SDL_INIT_VIDEO) == 0 )
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
	CurrentVideoMode.W = 1024;
	CurrentVideoMode.H = 768;
	CurrentVideoMode.BPP = 16;
	// пытаемся получить данные
	CurrentVideoMode.BPP = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
	if (CurrentVideoMode.BPP <= 16) CurrentVideoMode.BPP = 16;
	if (CurrentVideoMode.BPP > 16) CurrentVideoMode.BPP = 32;
#ifdef WIN32 // 1.2.10
	CurrentVideoMode.W = SDL_GetVideoInfo()->current_w;
	CurrentVideoMode.H = SDL_GetVideoInfo()->current_h;
#endif // WIN32
#ifdef __unix // 1.2.7
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if ( SDL_GetWMInfo(&info) > 0 )
	{
		if ( info.subsystem == SDL_SYSWM_X11 )
		{
			info.info.x11.lock_func();

			CurrentVideoMode.W = DisplayWidth(info.info.x11.display,
					DefaultScreen(info.info.x11.display));
			CurrentVideoMode.H = DisplayHeight(info.info.x11.display,
					DefaultScreen(info.info.x11.display));

			info.info.x11.unlock_func();
		}
	}
#endif // __unix






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// список поддерживаемых разрешений, если нужно
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ForceResolution.W != -1 && ForceResolution.H != -1)
	{
		if (VideoModes == 0)
		{
			// т.к. у нас 3 вида режимов (0-оконный, 16 и 24/32 полноэкранный)
			VideoModesNum = 3;

			VideoModes = new sVideoModes[VideoModesNum];

			int k=0;

			// 16 и 32 битный
			VideoModes[k].W = ForceResolution.W;
			VideoModes[k].H = ForceResolution.H;
			VideoModes[k].BPP = 32;
			sprintf(VideoModes[k].Title, "%ix%i %ibit", VideoModes[k].W, VideoModes[k].H, VideoModes[k].BPP);
			k++;

			VideoModes[k].W = ForceResolution.W;
			VideoModes[k].H = ForceResolution.H;
			VideoModes[k].BPP = 16;
			sprintf(VideoModes[k].Title, "%ix%i %ibit", VideoModes[k].W, VideoModes[k].H, VideoModes[k].BPP);
			k++;

			// оконный режим
			VideoModes[k].W = ForceResolution.W;
			VideoModes[k].H = ForceResolution.H;
			VideoModes[k].BPP = 0;
			sprintf(VideoModes[k].Title, "%ix%i", VideoModes[k].W, VideoModes[k].H);
			k++;
		}
	}
	else
	{
		if (VideoModes == 0)
		{
			SDL_Rect **modes = 0;
			modes=SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);

			if(modes == (SDL_Rect **)0 || modes == 0)
			{
				// если список получить не можем, берем только текущее разрешение

				// т.к. у нас 3 вида режимов (0-оконный, 16 и 24/32 полноэкранный)
				VideoModesNum = 3;

				VideoModes = new sVideoModes[VideoModesNum];

				int k=0;

				// 16 и 32 битный
				VideoModes[k].W = CurrentVideoMode.W;
				VideoModes[k].H = CurrentVideoMode.H;
				VideoModes[k].BPP = 32;
				sprintf(VideoModes[k].Title, "%ix%i %ibit", VideoModes[k].W, VideoModes[k].H, VideoModes[k].BPP);
				k++;

				VideoModes[k].W = CurrentVideoMode.W;
				VideoModes[k].H = CurrentVideoMode.H;
				VideoModes[k].BPP = 16;
				sprintf(VideoModes[k].Title, "%ix%i %ibit", VideoModes[k].W, VideoModes[k].H, VideoModes[k].BPP);
				k++;

				// оконный режим
				VideoModes[k].W = CurrentVideoMode.W;
				VideoModes[k].H = CurrentVideoMode.H;
				VideoModes[k].BPP = 0;
				sprintf(VideoModes[k].Title, "%ix%i", VideoModes[k].W, VideoModes[k].H);
				k++;
			}
			else
			{
				VideoModesNum = 0;
				for(int i=0; modes[i]; ++i)
					if (modes[i]->w >=640 && modes[i]->h >= 480) VideoModesNum ++;

				// т.к. у нас 3 вида режимов (0-оконный, 16 и 24/32 полноэкранный)
				VideoModesNum = VideoModesNum*3;

				VideoModes = new sVideoModes[VideoModesNum];

				int k=0;

				// 16 и 32 битный
				for(int i=0; modes[i]; ++i)
				if (modes[i]->w >=640 && modes[i]->h >= 480)
				{
					VideoModes[k].W = modes[i]->w;
					VideoModes[k].H = modes[i]->h;
					VideoModes[k].BPP = 32;
					sprintf(VideoModes[k].Title, "%ix%i %ibit", VideoModes[k].W, VideoModes[k].H, VideoModes[k].BPP);
					k++;

					VideoModes[k].W = modes[i]->w;
					VideoModes[k].H = modes[i]->h;
					VideoModes[k].BPP = 16;
					sprintf(VideoModes[k].Title, "%ix%i %ibit", VideoModes[k].W, VideoModes[k].H, VideoModes[k].BPP);
					k++;
				}
				// оконный режим
				for(int i=0; modes[i]; ++i)
				if (modes[i]->w >=640 && modes[i]->h >= 480)
				{
					VideoModes[k].W = modes[i]->w;
					VideoModes[k].H = modes[i]->h;
					VideoModes[k].BPP = 0;
					sprintf(VideoModes[k].Title, "%ix%i", VideoModes[k].W, VideoModes[k].H);
					k++;
				}
			}


			// выводим список поддерживаемых разрешений
			printf("\n");
			printf("Supported resolutions list:\n");
			for(int i=0; i<VideoModesNum; i++)
			{
				 printf("%s \n", VideoModes[i].Title);
			}
			printf("\n");
		}
	}





	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// работа с файлом данных...
	// если первый запуск (нет файла настроек), тянем переменную
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool FirstStart = LoadXMLSetupFile(NeedSafeMode);
	// ставим принудительно разрешение, если нужно
	if (ForceResolution.W != -1 && ForceResolution.H != -1)
	{
		Setup.Width = ForceResolution.W;
		Setup.Height = ForceResolution.H;
		Setup.BPP = ForceResolution.BPP;
	}






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем и иним джойстик
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if(SDL_NumJoysticks()>0)
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


	int InitStatus = vw_InitRenderer(GAME_TITLE, Setup.Width, Setup.Height, &Setup.BPP, FullScreen, &Setup.MultiSampleType);

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
	// если ноль - получить не удалось, ставим больше 100мегабайт а не 128 ровно
	// если шейдерная модель 3-я или выше, можно смело ставить
	if ((CAPS->VidMemTotal != 0 && CAPS->VidMemTotal > 102400) ||
		CAPS->ShaderModel >= 3.0f)
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



loop:


	Quit = false;
	NeedReCreate = false;
	bool NeedLoop = true;


	// начальные установки джойстика
	JoystickAxisX = 0;
	JoystickAxisY = 0;
	float JoystickCurentTime = vw_GetTime();
	float JoystickTimeDelta = 0.0f;
	if (Joystick != NULL)
	{
		JoystickAxisX = SDL_JoystickGetAxis(Joystick, 0);
		JoystickAxisY = SDL_JoystickGetAxis(Joystick, 1);

		for (int i=0; i<100; i++) JoysticButtons[i] = false;
	}



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

	vw_ReleaseAllTextures();
	vw_ShutdownRenderer();

	// закрываем джойстик, если он был
	if(SDL_NumJoysticks()>0)
		if(SDL_JoystickOpened(Setup.JoystickNum))
			if (Joystick != NULL)
				SDL_JoystickClose(Joystick);





	SDL_Quit();

	SaveXMLSetupFile();

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


	ReleaseGameText();

	// закрываем файловую систему
	vw_ShutdownVFS();





	// уходим из программы...
	ReleaseGameOneCopy();
	return 0;

}
