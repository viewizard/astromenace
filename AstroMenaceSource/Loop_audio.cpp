/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Loop_audio.cpp

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
// локальная структура данных 3D sfx
//------------------------------------------------------------------------------------
struct GameSoundData
{
	// имя звукового файла
	const	char *FileName;
	// корректировка громкости
	float	VolumeCorrection;
	// номер группы
	int		Group;
	// макс. кол-во одновременно проигрываемых звуков в группе
	int		GroupCount;
	// номер подгруппы
	int		SubGroup;
	// макс. кол-во одновременно проигрываемых звуков в подгруппе
	int		SubGroupCount;
	// приоритет звука в группе (не подгруппе!) от 1 до ... (1-самый высокий)
	int		Priority;
};




// кол-во игровых звуков
const int	GameSoundQuantity = 33;
// перечень имен файлов игровых звуков
GameSoundData GameSoundList[GameSoundQuantity] =
{

{"DATA/SFX/weapon1probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, механическое оружие (Kinetic)
{"DATA/SFX/weapon2probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, "слабое" энергетическое оружие (Ion, Plasma)
{"DATA/SFX/weapon3probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, "лучевое/среднее" энергетическое оружие (Maser, Laser)
{"DATA/SFX/weapon4probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, "мощьное" энергетическое оружие (Antimatter, Gauss)
{"DATA/SFX/weapon5probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, пусковые установки (ракеты, торпеды, бомбы)

{"DATA/SFX/explosion1.wav", 1.0f, 2, 6, 1, 4, 1},		// малый взрыв: ракеты
{"DATA/SFX/explosion2.wav", 1.0f, 2, 6, 1, 4, 1},		// взрыв (обычный): торпеды, истребители землян и пиратов
{"DATA/SFX/explosion3.wav", 1.0f, 2, 6, 1, 4, 1},		// взрыв (энергетический): ядерные бомбы, корабли пришельцев

{"DATA/SFX/weaponfire1.wav", 0.7f, 1, 8, 2, 4, 4},	// выстрел, Kinetic 1
{"DATA/SFX/weaponfire2.wav", 0.65f, 1, 8, 2, 4, 4},	// выстрел, Kinetic 2
{"DATA/SFX/weaponfire3.wav", 0.7f, 1, 8, 2, 4, 4},	// выстрел, Kinetic 3
{"DATA/SFX/weaponfire4.wav", 1.0f, 1, 8, 2, 4, 4},	// выстрел, Kinetic 4
{"DATA/SFX/weaponfire5.wav", 1.0f, 1, 8, 2, 4, 4},	// выстрел, Ion 1
{"DATA/SFX/weaponfire6.wav", 1.0f, 1, 8, 2, 4, 4},	// выстрел, Ion 2
{"DATA/SFX/weaponfire7.wav", 0.7f, 1, 8, 2, 4, 4},	// выстрел, Ion 3
{"DATA/SFX/weaponfire8.wav", 0.85f, 1, 8, 2, 4, 4},	// выстрел, Plasma 1
{"DATA/SFX/weaponfire9.wav", 0.95f, 1, 8, 2, 4, 4},	// выстрел, Plasma 2
{"DATA/SFX/weaponfire10.wav", 0.9f, 1, 8, 2, 4, 4},	// выстрел, Plasma 3
{"DATA/SFX/weaponfire11.wav", 0.6f, 1, 8, 2, 4, 1},	// выстрел, Maser 1 - продолжительный, пока есть луч; приоритет выше, чтобы не останавливали
{"DATA/SFX/weaponfire12.wav", 0.55f, 1, 8, 2, 4, 1},	// выстрел, Maser 2 - продолжительный, пока есть луч; приоритет выше, чтобы не останавливали
{"DATA/SFX/weaponfire13.wav", 0.9f, 1, 8, 2, 4, 4},	// выстрел, Antimatter
{"DATA/SFX/weaponfire14.wav", 0.8f, 1, 8, 2, 4, 1},	// выстрел, Laser - продолжительный, пока есть луч; приоритет выше, чтобы не останавливали
{"DATA/SFX/weaponfire15.wav", 0.8f, 1, 8, 2, 4, 4},	// выстрел, Gauss
{"DATA/SFX/weaponfire16.wav", 1.0f, 1, 8, 3, 2, 2},	// выстрел, одиночная ракета; приоритет выше, не красиво если снимаем "хвост" эффекта
{"DATA/SFX/weaponfire17.wav", 1.0f, 1, 8, 3, 2, 3},	// выстрел, одна ракета из группы; приоритет выше, не красиво если снимаем "хвост" эффекта
{"DATA/SFX/weaponfire18.wav", 1.0f, 1, 8, 3, 2, 2},	// выстрел, торпеда; приоритет выше, не красиво если снимаем "хвост" эффекта
{"DATA/SFX/weaponfire19.wav", 1.0f, 1, 8, 3, 2, 2},	// выстрел, бомба; приоритет выше, не красиво если снимаем "хвост" эффекта


{"DATA/SFX/kinetichit.wav", 1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Kinetic снарядов
{"DATA/SFX/ionhit.wav",		1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Ion снарядов
{"DATA/SFX/plasmahit.wav",	1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Plasma снарядов
{"DATA/SFX/antimaterhit.wav",1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Antimatter снарядов
{"DATA/SFX/gausshit.wav",	1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Gauss снарядов

{"DATA/SFX/explosion4.wav", 1.0f, 2, 6, 1, 4, 1},		// малый взрыв: астероиды

};





//------------------------------------------------------------------------------------
// локальная структура данных 2D sfx
//------------------------------------------------------------------------------------
struct Sound2dData
{
	// имя звукового файла
	const	char *FileName;
	// корректировка громкости
	float	VolumeCorrection;
	// нужно релизить при загрузке
	bool NeedRelease;
};

// кол-во звуков для меню + голоса
const int	MenuSoundQuantity = 25;
// перечень имен файлов звуков для меню
Sound2dData MenuSoundNames[MenuSoundQuantity] =
{
{"DATA/SFX/menu_onbutton.wav", 0.4f, false},	// навели на кнопку
{"DATA/SFX/menu_click.wav", 0.6f, false},		// нажали на кнопку
{"DATA/SFX/menu_new.wav", 1.0f, true},		// меняем меню
{"DATA/SFX/menu_taping.wav", 0.8f, true},		// набор текста-названия профайла
{"DATA/SFX/menu_online.wav", 0.75f, true},		// стоим над профайлом-миссией
{"DATA/SFX/menu_selectline.wav", 1.0f, true},	// выбрали профайл-миссию
{"DATA/SFX/menu_nclick.wav", 1.0f, false},		// не можем нажать на кнопку, но жмем
{"DATA/SFX/drag_error.wav", 1.0f, true},		// не можем установить в слот
{"DATA/SFX/drag_offslot.wav", 0.65f, true},	// снимаем со слота оружие
{"DATA/SFX/drag_onslot.wav", 0.82f, true},		// устанавливаем в слот оружие
{"DATA/SFX/drag_release.wav", 0.6f, true},	// освобождаем курсор от оружия, если тянули
{"DATA/SFX/game_showmenu.wav", 1.0f, false},	// проказываем меню в игре (звук во время его появления)
{"DATA/SFX/game_hidemenu.wav", 1.0f, false},	// скрываем меню в игре (звук во время его исчезновения)
{"DATA/SFX/lowlife.wav", 1.0f, true},			// сирена или что-то подобное, когда менее 10% жизни остается (зацикленный небольшой фрагмент)

{"DATA/VOICE/Attention.wav", 1.0f, true},
{"DATA/VOICE/EngineMalfunction.wav", 1.0f, true},
{"DATA/VOICE/MissileDetected.wav", 1.0f, true},//++
{"DATA/VOICE/PowerSupplyReestablished.wav", 1.0f, true},
{"DATA/VOICE/PrepareForAction.wav", 1.0f, true},//++
{"DATA/VOICE/ReactorMalfunction.wav", 1.0f, true},//++
{"DATA/VOICE/Warning.wav", 1.0f, true},//++
{"DATA/VOICE/WeaponDamaged.wav", 1.0f, true},//++
{"DATA/VOICE/WeaponDestroyed.wav", 1.0f, true},//++
{"DATA/VOICE/WeaponMalfunction.wav", 1.0f, true},//++

{"DATA/SFX/menu_onbutton2.wav", 0.15f, false},	// навели на малую кнопку
};




// кол-во музыкальных тем
const int	MusicQuantity = 6;
// перечень имен файлов музыки
const char *GameMusicNames[MusicQuantity] =
{
"DATA/MUSIC/menu.ogg",			// музыка в меню
"DATA/MUSIC/intro.ogg",			// музыка в заставках перед миссиями
"DATA/MUSIC/game.ogg",			// музыка в игре
"DATA/MUSIC/boss-loop.ogg",		// музыка в игре, когда подходим к опастному участку
"DATA/MUSIC/missionfailed.ogg",	// музыка в игре, когда убили (20 секунд, зацикленная!!!)

// вставки для тем
"DATA/MUSIC/boss-intro.ogg",	// музыка в игре, когда подходим к опастному участку

};
// для перекрытия музыки
bool GameMainMusicSet = false;
char GameMainMusic[MAX_PATH];
bool GameBossMusicSet = false;
char GameBossMusic[MAX_PATH];
bool GameDeathMusicSet = false;
char GameDeathMusic[MAX_PATH];



// массив указателей на музыку, точнее на ее идентификаторы
int MusicList[MusicQuantity] =
{0,0,0,0,0,0};
int CurrentPlayingMusic = -1; //если вулючаем громкость с нуля, нужно знать что запускать









//------------------------------------------------------------------------------------
// Инициализация звукового модуля
//------------------------------------------------------------------------------------
bool InitAudio()
{
	if (!vw_InitSound()) return false;

	for (int i=0; i<MusicQuantity; i++)
		MusicList[i] = -1;

	return true;
}





//------------------------------------------------------------------------------------
// Запуск нужной музыки
//------------------------------------------------------------------------------------
void StartMusicWithFade(int StartMusic, float FadeInTime, float FadeOutTime)
{
	CurrentPlayingMusic = StartMusic;

	// 0 menu музыка в меню
	// 1 intro музыка в заставках перед миссиями
	// 2 game музыка в игре
	// 3 boss музыка в игре, когда подходим к опастному участку
	// 4 missionfailed музыка в игре, когда убили (20 секунд, зацикленная!!!)

	// нужно запустить вставку (не зацикленную), интро
	// в начале проигрывания, а потом вызывать луп часть

	bool MusicLoop = true;
	const char *LoopFileName = 0;
	const char *CurrentPlayingMusicName = GameMusicNames[CurrentPlayingMusic];
	float MusicCorrection = 1.0f;


	switch (StartMusic)
	{
		case 2:
			if (GameMainMusicSet) CurrentPlayingMusicName = GameMainMusic;
			break;

		case 3:
			if (GameBossMusicSet)
			{
				CurrentPlayingMusicName = GameBossMusic;
			}
			else
			{
				CurrentPlayingMusic = 5; MusicLoop = false; LoopFileName = GameMusicNames[StartMusic];
			}

			break;

		case 4:
			if (GameDeathMusicSet)
			{
				CurrentPlayingMusicName = GameDeathMusic;
			}
			else
			{
				MusicCorrection = 0.7f;
			}
			break;
	}




	if (Setup.Music_check) //если можно играть
	if (Setup.MusicSw) // и громкость не нулевая
	if (vw_FindMusicByNum(MusicList[CurrentPlayingMusic]) == 0) // если это еще не играем
	{

		// если что-то играем, нужно поставить чтобы уходило по громкости
		if (vw_GetMusicIsPlaying())
		{
			for (int i=0; i<MusicQuantity; i++)
			if (vw_FindMusicByNum(MusicList[i]) != 0)
			{
				vw_FindMusicByNum(MusicList[i])->FadeOut(FadeOutTime);
				MusicList[i] = -1;
			}
		}


		// создаем новый источник и проигрываем его
		eMusic *Music;
		Music = new eMusic;
		vw_AttachMusic(Music);
		MusicList[CurrentPlayingMusic] = Music->Num;

		// пытаемся загрузить и играть
		if (!Music->Play(CurrentPlayingMusicName, 0.0f, MusicCorrection*(Setup.MusicSw/10.0f), MusicLoop, LoopFileName))
		{
			vw_ReleaseMusic(Music); Music = 0;
			MusicList[CurrentPlayingMusic] = -1;
			CurrentPlayingMusic = -1;
		}
		else // играем, ставим плавное появление громкости
			Music->FadeIn(1.0f, FadeInTime);
	}


	// повторно ставим (могли изменить, если отдельно интро и луп части)
	CurrentPlayingMusic = StartMusic;
}








//------------------------------------------------------------------------------------
// Проигрываем звук в меню, или другие 2д звуки
//------------------------------------------------------------------------------------
int Audio_PlayMenuSound(int SoundID, float fVol, bool Loop)
{
	if (!Setup.Sound_check) return 0;
	if (!Setup.SoundSw) return 0;

#ifdef RU
	// русский голос делаем немного тише
	if (SoundID > 14 && SoundID < 25) MenuSoundNames[SoundID-1].VolumeCorrection = 0.6f;
#endif

	fVol = fVol*MenuSoundNames[SoundID-1].VolumeCorrection;


	// т.к. у нас со смещением же в 1 идет
	SoundID --;


	// если это звук меню и мы его играем, его надо перезапустить
	if (SoundID < 14)
	{
		eSound* Tmp = vw_FindSoundByName(MenuSoundNames[SoundID].FileName);
		if (Tmp != 0)
		{
			Tmp->Replay();
			return Tmp->Num;
		}
	}


	// создаем новый источник и проигрываем его
	eSound *Sound = 0;
	Sound = new eSound;
	if (Sound == 0) return 0;
	vw_AttachSound(Sound);

	// чтобы не было искажения по каналам, делаем установку относительно камеры...
	if (!Sound->Play(MenuSoundNames[SoundID].FileName, fVol, Setup.SoundSw/10.0f, 0.0f, 0.0f, 0.0f, true, Loop, MenuSoundNames[SoundID].NeedRelease, 1))
	{
		vw_ReleaseSound(Sound); Sound = 0;
		return 0;
	}

	return Sound->Num;
}





//------------------------------------------------------------------------------------
// Проигрываем 3д звуки
//------------------------------------------------------------------------------------
int Audio_PlaySound(int SoundID, float fVol, VECTOR3D Location, bool Loop, int AtType)
{
	if (!Setup.Sound_check) return 0;
	if (!Setup.SoundSw) return 0;

	fVol = fVol*GameSoundList[SoundID-1].VolumeCorrection;

	// т.к. у нас со смещением же в 1 идет
	SoundID --;

	// проверяем, можем ли мы играть этот звук
	if (!vw_CheckCanPlaySound(GameSoundList[SoundID].Group, GameSoundList[SoundID].GroupCount, GameSoundList[SoundID].SubGroup,
			GameSoundList[SoundID].SubGroupCount, GameSoundList[SoundID].Priority)) return 0;


	// создаем новый источник и проигрываем его
	eSound *Sound = 0;
	Sound = new eSound;
	if (Sound == 0) return 0;
	vw_AttachSound(Sound);
	if (!Sound->Play(GameSoundList[SoundID].FileName, fVol, Setup.SoundSw/10.0f, Location.x, Location.y, Location.z, false, Loop, true, AtType))
	{
		vw_ReleaseSound(Sound); Sound = 0;
		return 0;
	}

	// устанавливаем нужные данные
	Sound->SetInfo(GameSoundList[SoundID].Group, GameSoundList[SoundID].GroupCount, GameSoundList[SoundID].SubGroup,
			GameSoundList[SoundID].SubGroupCount, GameSoundList[SoundID].Priority);

	return Sound->Num;
}







//------------------------------------------------------------------------------------
// Цикл звука
//------------------------------------------------------------------------------------
void Audio_LoopProc()
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// делаем установку слушателя (он в точке камеры)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// получаем текущее положение камеры
	VECTOR3D CurrentCameraLocation;
	vw_GetCameraLocation(&CurrentCameraLocation);
	VECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);

	VECTOR3D ListenerOriV1(0.0f, 0.0f, -1.0f);
	RotatePoint(&ListenerOriV1, CurrentCameraRotation);
	VECTOR3D ListenerOriV2(0.0f, 1.0f, 0.0f);
	RotatePoint(&ListenerOriV2, CurrentCameraRotation);

	// Position of the Listener.
	ALfloat ListenerPos[3] = { CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z };
	// Velocity of the Listener.
	ALfloat ListenerVel[3] = { 0.0f, 0.0f, 0.0f };
	// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
	// Also note that these should be units of '1'.
	ALfloat ListenerOri[6] = { ListenerOriV1.x, ListenerOriV1.y, ListenerOriV1.z,
								ListenerOriV2.x, ListenerOriV2.y, ListenerOriV2.z };

	vw_Listener(ListenerPos, ListenerVel, ListenerOri);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// передаем управление, чтобы внутри ядра все сделали
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_UpdateSound();
	vw_UpdateMusic();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// запускаем нужную музыку... только включили громкость или выключили
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!vw_GetMusicIsPlaying())
	{
		if (Setup.MusicSw) // если громкость не нулевая
		if (Setup.Music_check) // если можно вообще играть
		if (CurrentPlayingMusic != -1) // если установлен номер
		if (vw_FindMusicByNum(MusicList[CurrentPlayingMusic]) == 0) // если это еще не играем
		{
			// создаем новый источник и проигрываем его
			eMusic *Music;
			Music = new eMusic;
			vw_AttachMusic(Music);
			MusicList[CurrentPlayingMusic] = Music->Num;

			if (!Music->Play(GameMusicNames[CurrentPlayingMusic], 1.0f, Setup.MusicSw/10.0f, true, 0))
			{
				vw_ReleaseMusic(Music); Music = 0;
				MusicList[CurrentPlayingMusic] = -1;
				CurrentPlayingMusic = -1;
			}
		}
	}
	else
	{
		// если что-то играем, но звук уже выключен, нужно все убрать...
		if (!Setup.MusicSw) // если громкость нулевая
		if (Setup.Music_check) // но играть можно
		{
			vw_ReleaseAllMusic();
			for (int i=0; i<MusicQuantity; i++)
				MusicList[i] = -1;
		}
	}


}


