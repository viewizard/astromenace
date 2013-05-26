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


#include "config.h"


#ifndef vfs_pack_standalone

#include "Game.h"

#else

#include "Core/VirtualFileSystem/VFS.h"
int ConvertFS2VFS(char RawDataDir[MAX_PATH]);


//------------------------------------------------------------------------------------
// пути к файлам
//------------------------------------------------------------------------------------
// полное путь к программе
char ProgrammDir[MAX_PATH];
char VFSFileNamePath1[MAX_PATH];
char VFSFileNamePath2[MAX_PATH];



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
	ZeroMemory(VFSFileNamePath1, sizeof(VFSFileNamePath1));
	ZeroMemory(VFSFileNamePath2, sizeof(VFSFileNamePath2));

	strcpy(VFSFileNamePath1, ProgrammDir);
	strcat(VFSFileNamePath1, "gamedata.vfs");
	strcpy(VFSFileNamePath2, ProgrammDir);
	strcat(VFSFileNamePath2, "gamedata_cc.vfs");
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

	strcpy(VFSFileNamePath1, ProgrammDir);
	strcat(VFSFileNamePath1, "gamedata.vfs");
	strcpy(VFSFileNamePath2, ProgrammDir);
	strcat(VFSFileNamePath2, "gamedata_cc.vfs");

#endif // unix






	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// исследуем другие ключи
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// флаг перевода игры в режим упаковки gamedata.vfs файла
	bool NeedPack = false;

	for (int i=1; i<argc; i++)
	{
		// проверка ключа "--help"
		if (!strcmp(argv[i], "--help"))
		{
			printf("AstroMenace launch options:\n\n");

			printf("--dir=/game/data/folder/ - folder with gamedata.vfs file (Linux only)\n");
			printf("--pack - pack data to gamedata.vfs file\n");
			printf("--rawdata=/game/rawdata/folder/ - folder with game raw data for gamedata.vfs.\n");
			printf("--help - info about all game launch options.\n");

			return 0;
		}


		// проверка ключа "--pack"
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



	// уходим из программы...
	return 0;

}



#endif // vfs_pack_standalone



const int	VFSFilesListCount = 388;
const char *VFSFilesList[VFSFilesListCount] =
{"DATA_RU/VOICE/EngineMalfunction.wav",
"DATA_RU/VOICE/WeaponDamaged.wav",
"DATA_RU/VOICE/MissileDetected.wav",
"DATA_RU/VOICE/PrepareForAction.wav",
"DATA_RU/VOICE/Attention.wav",
"DATA_RU/VOICE/WeaponDestroyed.wav",
"DATA_RU/VOICE/WeaponMalfunction.wav",
"DATA_RU/VOICE/CeaseFire.wav",
"DATA_RU/VOICE/PowerSupplyReestablished.wav",
"DATA_RU/VOICE/ReactorMalfunction.wav",
"DATA_RU/VOICE/Warning.wav",
"DATA_RU/MENU/button_weaponry_in.tga",
"DATA_RU/MENU/button_weaponry_out.tga",
"DATA_RU/GAME/pause.tga",
"DATA_RU/GAME/mission.tga",
"DATA_RU/GAME/missionfailed.tga",
"LOADING/loading_back.tga",
"MENU/weapon_empty_icon.tga",
"MENU/system_engine1.tga",
"MENU/system_engine2.tga",
"MENU/system_engine3.tga",
"MENU/system_engine4.tga",
"MENU/system_target1.tga",
"MENU/system_target2.tga",
"MENU/system_target3.tga",
"MENU/system_target4.tga",
"MENU/system_power1.tga",
"MENU/system_power2.tga",
"MENU/system_power3.tga",
"MENU/system_power4.tga",
"MENU/system_protect1.tga",
"MENU/system_protect2.tga",
"MENU/system_protect3.tga",
"MENU/system_protect4.tga",
"MENU/system_mechan1.tga",
"MENU/system_mechan2.tga",
"MENU/system_mechan3.tga",
"MENU/system_mechan4.tga",
"MENU/system_empty.tga",
"GLSL/light.vert",
"GLSL/light.frag",
"GLSL/light_shadowmap.vert",
"GLSL/light_shadowmap.frag",
"GLSL/particle.frag",
"GLSL/particle_stars.vert",
"GLSL/light_explosion.frag",
"GLSL/light_explosion.vert",
"GLSL/particle.vert",
"CREDITS/gentoo.tga",
"CREDITS/gimp.tga",
"CREDITS/sdl.tga",
"CREDITS/opengl.tga",
"CREDITS/gnugcc.tga",
"CREDITS/codeblocks.tga",
"CREDITS/openal.tga",
"CREDITS/svn.tga",
"MENU/cursor.tga",
"DATA_EN/VOICE/EngineMalfunction.wav",
"DATA_EN/VOICE/WeaponDamaged.wav",
"DATA_EN/VOICE/MissileDetected.wav",
"DATA_EN/VOICE/PrepareForAction.wav",
"DATA_EN/VOICE/Attention.wav",
"DATA_EN/VOICE/WeaponDestroyed.wav",
"DATA_EN/VOICE/WeaponMalfunction.wav",
"DATA_EN/VOICE/CeaseFire.wav",
"DATA_EN/VOICE/PowerSupplyReestablished.wav",
"DATA_EN/VOICE/ReactorMalfunction.wav",
"DATA_EN/VOICE/Warning.wav",
"DATA_EN/MENU/button_weaponry_in.tga",
"DATA_EN/MENU/button_weaponry_out.tga",
"DATA_EN/GAME/pause.tga",
"DATA_EN/GAME/mission.tga",
"DATA_EN/GAME/missionfailed.tga",
"SFX/weaponfire12.wav",
"SFX/drag_offslot.wav",
"SFX/menu_click.wav",
"SFX/drag_error.wav",
"SFX/menu_onbutton.wav",
"SFX/weapon1probl.wav",
"SFX/weaponfire3.wav",
"SFX/gausshit.wav",
"SFX/menu_online.wav",
"SFX/weaponfire5.wav",
"SFX/weapon4probl.wav",
"SFX/drag_release.wav",
"SFX/menu_selectline.wav",
"SFX/weaponfire1.wav",
"SFX/weaponfire14.wav",
"SFX/weaponfire13.wav",
"SFX/weaponfire16.wav",
"SFX/weaponfire17.wav",
"SFX/explosion4.wav",
"SFX/weapon3probl.wav",
"SFX/plasmahit.wav",
"SFX/weaponfire19.wav",
"SFX/menu_nclick.wav",
"SFX/drag_onslot.wav",
"SFX/weaponfire10.wav",
"SFX/ionhit.wav",
"SFX/weaponfire6.wav",
"SFX/weaponfire7.wav",
"SFX/weaponfire15.wav",
"SFX/explosion2.wav",
"SFX/weaponfire8.wav",
"SFX/weaponfire2.wav",
"SFX/menu_onbutton2.wav",
"SFX/weaponfire18.wav",
"SFX/menu_new.wav",
"SFX/weaponfire4.wav",
"SFX/antimaterhit.wav",
"SFX/weapon5probl.wav",
"SFX/explosion3.wav",
"SFX/kinetichit.wav",
"SFX/weaponfire9.wav",
"SFX/menu_taping.wav",
"SFX/game_showmenu.wav",
"SFX/game_hidemenu.wav",
"SFX/weaponfire11.wav",
"SFX/weapon2probl.wav",
"SFX/explosion1.wav",
"SFX/lowlife.wav",
"MODELS/MINE/mine2i.tga",
"MODELS/MINE/mine1.tga",
"MODELS/MINE/mine3i.tga",
"MODELS/MINE/mine3.tga",
"MODELS/MINE/mine-04.VW3D",
"MODELS/MINE/mine-01.VW3D",
"MODELS/MINE/mine2.tga",
"MODELS/MINE/mine-03.VW3D",
"MODELS/MINE/mine1i.tga",
"MODELS/MINE/mine-02.VW3D",
"MODELS/MINE/mine4i.tga",
"MODELS/MINE/mine4.tga",
"MODELS/PLANET/planet5.VW3D",
"MODELS/PLANET/aplanet.VW3D",
"MODELS/PLANET/dplanet.VW3D",
"MODELS/PLANET/m_class7.tga",
"MODELS/PLANET/a_class4.tga",
"MODELS/PLANET/d_class3.tga",
"MODELS/PLANET/planet6.VW3D",
"MODELS/PLANET/q_class2.tga",
"MODELS/PLANET/gplanet.VW3D",
"MODELS/PLANET/moon.VW3D",
"MODELS/PLANET/clouds.tga",
"MODELS/PLANET/asteroid.tga",
"MODELS/SPACEBASE/4/1.VW3D",
"MODELS/SPACEBASE/3/1.VW3D",
"MODELS/SPACEBASE/7/1.VW3D",
"MODELS/SPACEBASE/1/2.VW3D",
"MODELS/SPACEBASE/1/4.VW3D",
"MODELS/SPACEBASE/1/3.VW3D",
"MODELS/SPACEBASE/1/1.VW3D",
"MODELS/SPACEBASE/1/5.VW3D",
"MODELS/SPACEBASE/metal.tga",
"MODELS/SPACEBASE/5/1.VW3D",
"MODELS/SPACEBASE/6/2.VW3D",
"MODELS/SPACEBASE/6/4.VW3D",
"MODELS/SPACEBASE/6/3.VW3D",
"MODELS/SPACEBASE/6/1.VW3D",
"MODELS/SPACEBASE/6/5.VW3D",
"MODELS/SPACEBASE/8/2.VW3D",
"MODELS/SPACEBASE/8/4.VW3D",
"MODELS/SPACEBASE/8/3.VW3D",
"MODELS/SPACEBASE/8/1.VW3D",
"MODELS/SPACEBASE/8/5.VW3D",
"MODELS/SPACEBASE/allalpha.tga",
"MODELS/SPACEBASE/2/2.VW3D",
"MODELS/SPACEBASE/2/4.VW3D",
"MODELS/SPACEBASE/2/3.VW3D",
"MODELS/SPACEBASE/2/1.VW3D",
"MODELS/SPACEBASE/2/5.VW3D",
"MODELS/TURRET/turret-02.VW3D",
"MODELS/TURRET/turret-01.VW3D",
"MODELS/TURRET/turrets.tga",
"MODELS/SPACE/asteroid-0118.VW3D",
"MODELS/SPACE/asteroid-0116.VW3D",
"MODELS/SPACE/asteroid-011.VW3D",
"MODELS/SPACE/bigasteroid-03.VW3D",
"MODELS/SPACE/asteroid-018.VW3D",
"MODELS/SPACE/asteroid-017.VW3D",
"MODELS/SPACE/asteroid-016.VW3D",
"MODELS/SPACE/bigasteroid-02.VW3D",
"MODELS/SPACE/bigasteroid-05.VW3D",
"MODELS/SPACE/asteroid-01.tga",
"MODELS/SPACE/asteroid-0110.VW3D",
"MODELS/SPACE/asteroid-0115.VW3D",
"MODELS/SPACE/asteroid-0112.VW3D",
"MODELS/SPACE/asteroid-0117.VW3D",
"MODELS/SPACE/asteroid-012.VW3D",
"MODELS/SPACE/asteroid-019.VW3D",
"MODELS/SPACE/bigasteroid-01.VW3D",
"MODELS/SPACE/asteroid-013.VW3D",
"MODELS/SPACE/asteroid-0119.VW3D",
"MODELS/SPACE/asteroid-0114.VW3D",
"MODELS/SPACE/asteroid-014.VW3D",
"MODELS/SPACE/asteroid-0113.VW3D",
"MODELS/SPACE/asteroid-015.VW3D",
"MODELS/SPACE/bigasteroid-04.VW3D",
"MODELS/SPACE/asteroid-010.VW3D",
"MODELS/SPACE/asteroid-0111.VW3D",
"MODELS/EARTHFIGHTER/lnch1.VW3D",
"MODELS/EARTHFIGHTER/lnch2.VW3D",
"MODELS/EARTHFIGHTER/lnch3.VW3D",
"MODELS/EARTHFIGHTER/lnch4.VW3D",
"MODELS/EARTHFIGHTER/rockets.tga",
"MODELS/EARTHFIGHTER/torpedo.VW3D",
"MODELS/EARTHFIGHTER/swarm.VW3D",
"MODELS/EARTHFIGHTER/lnch34.tga",
"MODELS/EARTHFIGHTER/missile.VW3D",
"MODELS/EARTHFIGHTER/nuke.VW3D",
"MODELS/EARTHFIGHTER/lnch12.tga",
"MENU/weapon4_icon.tga",
"MENU/ammo.tga",
"MENU/weapon17_icon.tga",
"MENU/workshop_panel1.tga",
"MENU/weapon18_icon.tga",
"MENU/weapon14_icon.tga",
"MENU/weapon19_icon.tga",
"MENU/weapon13_icon.tga",
"MENU/panel800_444_back.tga",
"MENU/button384_back.tga",
"MENU/panel444_333_back.tga",
"MENU/weapon2_icon.tga",
"MENU/button_dialog128_in.tga",
"MENU/weapon6_icon.tga",
"MENU/arrows_blue.tga",
"MENU/arrow_list_up.tga",
"MENU/arrow_list_down.tga",
"MENU/workshop_panel5.tga",
"MENU/workshop_panel3.tga",
"MENU/dialog768_600.tga",
"MENU/dialog512_256.tga",
"MENU/perc_none.tga",
"MENU/weapon5_icon.tga",
"MENU/panel444_333_border.tga",
"MENU/button_dialog200_out.tga",
"MENU/workshop_panel4.tga",
"MENU/weapon12_icon.tga",
"MENU/whitepoint.tga",
"MENU/weapon16_icon.tga",
"MENU/weapon11_icon.tga",
"MENU/back_spot2.tga",
"MENU/button256_off.tga",
"MENU/blackpoint.tga",
"MENU/checkbox_main.tga",
"MENU/button_dialog200_off.tga",
"MENU/workshop_panel1+.tga",
"MENU/weapon10_icon.tga",
"MENU/perc.tga",
"MENU/workshop_panel2.tga",
"MENU/dialog512_512.tga",
"MENU/button_dialog200_in.tga",
"MENU/button384_out.tga",
"MENU/button384_in.tga",
"MENU/weapon15_icon.tga",
"MENU/workshop_panel2+.tga",
"MENU/line.tga",
"MENU/button256_out.tga",
"MENU/weapon3_icon.tga",
"MENU/button256_in.tga",
"MENU/back_spot.tga",
"MENU/button256_back.tga",
"MENU/weapon_on_icon.tga",
"MENU/weapon8_icon.tga",
"MENU/astromenace.tga",
"MENU/weapon9_icon.tga",
"MENU/weapon1_icon.tga",
"MENU/checkbox_in.tga",
"MENU/weapon7_icon.tga",
"MENU/button_dialog128_out.tga",
"MENU/button_dialog128_off.tga",
"GAME/ammo.tga",
"GAME/weapon_panel_left.tga",
"GAME/game_panel.tga",
"GAME/weapon_ammo.tga",
"GAME/game_panel2.tga",
"GAME/energy.tga",
"GAME/nums.tga",
"GAME/game_num.tga",
"GAME/weapon_panel_right.tga",
"GAME/game_panel_el.tga",
"GAME/weapon_energy.tga",
"GFX/flare2.tga",
"GFX/flare.tga",
"GFX/flare1.tga",
"GFX/flare3.tga",
"LOADING/viewizardlogo.tga",
"LOADING/loading03.tga",
"LOADING/loading_line.tga",
"LOADING/loading01.tga",
"LOADING/loading02.tga",
"LOADING/loading04.tga",
"DATA_DE/VOICE/EngineMalfunction.wav",
"DATA_DE/VOICE/WeaponDamaged.wav",
"DATA_DE/VOICE/MissileDetected.wav",
"DATA_DE/VOICE/PrepareForAction.wav",
"DATA_DE/VOICE/Attention.wav",
"DATA_DE/VOICE/WeaponDestroyed.wav",
"DATA_DE/VOICE/WeaponMalfunction.wav",
"DATA_DE/VOICE/CeaseFire.wav",
"DATA_DE/VOICE/PowerSupplyReestablished.wav",
"DATA_DE/VOICE/ReactorMalfunction.wav",
"DATA_DE/VOICE/Warning.wav",
"DATA_DE/MENU/button_weaponry_in.tga",
"DATA_DE/MENU/button_weaponry_out.tga",
"DATA_DE/GAME/pause.tga",
"DATA_DE/GAME/mission.tga",
"DATA_DE/GAME/missionfailed.tga",
"MENU/cursor_shadow.tga",
"MUSIC/boss-intro.ogg",
"MUSIC/menu.ogg",
"MUSIC/boss-loop.ogg",
"MUSIC/game.ogg",
"MUSIC/missionfailed.ogg",
"MUSIC/intro.ogg",
"SCRIPT/mission6.xml",
"SCRIPT/list.xml",
"SCRIPT/mission14.xml",
"SCRIPT/mission8.xml",
"SCRIPT/mission9_icon.tga",
"SCRIPT/mission15_icon.tga",
"SCRIPT/mission3.xml",
"SCRIPT/mission7.xml",
"SCRIPT/mission12.xml",
"SCRIPT/mission5.xml",
"SCRIPT/mission1.xml",
"SCRIPT/mission10_icon.tga",
"SCRIPT/mission13_icon.tga",
"SCRIPT/menu3.xml",
"SCRIPT/mission5_icon.tga",
"SCRIPT/mission13.xml",
"SCRIPT/mission14_icon.tga",
"SCRIPT/mission6_icon.tga",
"SCRIPT/mission3_icon.tga",
"SCRIPT/mission11.xml",
"SCRIPT/mission4.xml",
"SCRIPT/mission2.xml",
"SCRIPT/mission11_icon.tga",
"SCRIPT/mission4_icon.tga",
"SCRIPT/mission8_icon.tga",
"SCRIPT/mission7_icon.tga",
"SCRIPT/aimode.xml",
"SCRIPT/mission10.xml",
"SCRIPT/mission2_icon.tga",
"SCRIPT/mission12_icon.tga",
"SCRIPT/menu2.xml",
"SCRIPT/mission15.xml",
"SCRIPT/mission1_icon.tga",
"SCRIPT/mission9.xml",
"SCRIPT/menu1.xml",
"SKYBOX/1/skybox_front5.tga",
"SKYBOX/1/skybox_left2.tga",
"SKYBOX/1/skybox_right1.tga",
"SKYBOX/1/skybox_back6.tga",
"SKYBOX/1/skybox_bottom4.tga",
"SKYBOX/1/skybox_top3.tga",
"SKYBOX/tile_back.tga",
"SKYBOX/tile_stars.tga",
"SKYBOX/2/skybox_front5.tga",
"SKYBOX/2/skybox_left2.tga",
"SKYBOX/2/skybox_right1.tga",
"SKYBOX/2/skybox_back6.tga",
"SKYBOX/2/skybox_bottom4.tga",
"SKYBOX/2/skybox_top3.tga",
#ifdef fontconfig
"empty",
"empty",
"empty",
"empty",
"empty",
"empty",
"empty",
"empty",
#else
"FONT/LiberationMono-Bold.ttf",
"FONT/LiberationSans-Bold.ttf",
"FONT/LiberationSerif-Bold.ttf",
"FONT/FreeMonoBold.ttf",
"FONT/FreeSansBold.ttf",
"FONT/FreeSerifBold.ttf",
"FONT/LinBiolinumBold.ttf",
"FONT/LinLibertineBold.ttf",
#endif //fontconfig
"CREDITS/freetype.tga",
"CREDITS/oggvorbis.tga",
"text.csv",
"GFX/trail1.tga",
"GFX/trail2.tga",
"GFX/trail3.tga",
"GFX/trail4.tga",
"GFX/trail5.tga",
"MODELS/NORMALMAP/alien_mothership_nm.tga",
"MODELS/NORMALMAP/m_class7_nm.tga",
"MODELS/NORMALMAP/d_class3_nm.tga",
"MODELS/NORMALMAP/a_class4_nm.tga",
"MODELS/NORMALMAP/planet_asteroids_nm.tga",
"MODELS/NORMALMAP/q_class2_nm.tga",
"MODELS/NORMALMAP/bomber_nm.tga",
"MODELS/NORMALMAP/buildings_nm.tga",
};










//------------------------------------------------------------------------------------
// конвертирование данных в VFS файл
//------------------------------------------------------------------------------------
int ConvertFS2VFS(char RawDataDir[MAX_PATH])
{

	// часть 3д моделей и текстур у нас упакованна, ее надо интегрировать в основной файл данных игры, или тянуть отдельным файлом

	char ModelsPackFile[MAX_PATH];
	strcpy(ModelsPackFile, RawDataDir);
	strcat(ModelsPackFile, "MODELS/models.pack");


#ifdef separate_cc_vfs

	SDL_RWops *MPFile = SDL_RWFromFile(ModelsPackFile, "rb");
    if (MPFile == NULL)
    {
       fprintf(stderr, "models.pack file not found or corrupted.\n");
		return -1;
    }

	SDL_RWseek(MPFile, 0, SEEK_END);
	int filedatasize = SDL_RWtell(MPFile);
	SDL_RWseek(MPFile, 0, SEEK_SET);

	BYTE *filedata = new BYTE[filedatasize];
	SDL_RWread(MPFile, filedata, 1, filedatasize);
	SDL_RWclose(MPFile);


	MPFile = SDL_RWFromFile(VFSFileNamePath2, "wb");
    if (MPFile == NULL)
    {
        printf("Can't open VFS file for write %s\n", VFSFileNamePath2);
        return -1; // ERROR
    }
	SDL_RWwrite(MPFile, filedata, filedatasize, 1);
	SDL_RWclose(MPFile);

	delete [] filedata;

#else

	if (vw_OpenVFS(ModelsPackFile, 0) != 0)
	{
		fprintf(stderr, "models.pack file not found or corrupted.\n");
		return -1;
	}


	// делаем первый проход и подсчитываем кол-во файлов
	int	ModelsPackListCount = 0;
	eVFS_Entry *TmpVFSEntry = vw_GetStarVFSArray();
	while (TmpVFSEntry != 0)
	{
		ModelsPackListCount++;
		TmpVFSEntry = TmpVFSEntry->Next;
	}

	// выделяем память для массивов
	BYTE **filedata;
	filedata = new BYTE*[ModelsPackListCount];
	char **filename;
	filename = new char*[ModelsPackListCount];
	int *filedatasize;
	filedatasize = new int[ModelsPackListCount];

	// заполняем массивы
	TmpVFSEntry = vw_GetStarVFSArray();
	int FileCount = 0;
	while (TmpVFSEntry != 0)
	{
		eFILE *tmpFile = vw_fopen(TmpVFSEntry->Name);
		if (tmpFile == 0)
		{
			fprintf(stderr, "models.pack file corrupted, 3d model or texture %s not found.\n", TmpVFSEntry->Name);
			return -1;
		}

		tmpFile->fseek( 0, SEEK_END );
		filedatasize[FileCount] = tmpFile->ftell();
		tmpFile->fseek( 0, SEEK_SET );

		filedata[FileCount] = new BYTE[filedatasize[FileCount]];
		tmpFile->fread(filedata[FileCount], filedatasize[FileCount], 1);

		filename[FileCount] = new char[strlen(TmpVFSEntry->Name)+1];
		strcpy(filename[FileCount], TmpVFSEntry->Name);

		vw_fclose(tmpFile);
		TmpVFSEntry = TmpVFSEntry->Next;
		FileCount++;
	}


	vw_ShutdownVFS();

#endif // separate_cc_vfs





	// создаем новый файл VFS
	vw_CreateVFS(VFSFileNamePath1, GAME_BUILD);


	// используем не авто поиск по дирректориям, как было, а заранее сформированный список
	// 1) файлы очень редко удаляются/добавляются
	// 2) нужно обязательно проверить "целостность" всей сборки на наличие всех файлов
	// 3) будет работать на любой платформе, где работает libSDL



#ifndef separate_cc_vfs
	// добавляем файлы из пакета 3д моделей
	for (int i=0; i<ModelsPackListCount; i++)
	{
		// запись в VFS
		if (0 != vw_WriteIntoVFSfromMemory(filename[i], filedata[i], filedatasize[i]))
		{
			// какая-то ошибка, не можем записать в VFS
			fprintf(stderr, "Can't write into VFS from memory %s !!!\n", filename[i]);
			fprintf(stderr, "VFS compilation process aborted!\n");
			return -1;
		}

		delete [] filedata[i];
		delete [] filename[i];
	}

	delete [] filedata;
	delete [] filename;
	delete [] filedatasize;
#endif // separate_cc_vfs


	char SrcFileName[MAX_PATH];
	char DstFileName[MAX_PATH];

	// добавляем физические файлы
	for (int i=0; i<VFSFilesListCount; i++)
	if (strcmp(VFSFilesList[i], "empty"))
	{

		strcpy(SrcFileName, RawDataDir);
		strcat(SrcFileName, VFSFilesList[i]);

		// все наши файлы внутри VFS лежат в директории DATA
		strcpy(DstFileName, "DATA/");
		strcat(DstFileName, VFSFilesList[i]);

		if (0 != vw_WriteIntoVFSfromFile(SrcFileName, DstFileName))
		{
			fprintf(stderr, "VFS compilation process aborted!\n");
        	return -1;
		}
	}


	// закрываем VFS файл
	vw_ShutdownVFS();

	return 0;
}




