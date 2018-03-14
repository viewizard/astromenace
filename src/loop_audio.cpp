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

#include "game.h"

namespace {

std::string CurrentPlayingMusicName;

} // unnamed namespace


//------------------------------------------------------------------------------------
// локальная структура данных 3D sfx
//------------------------------------------------------------------------------------
struct sGameSoundData {
	// имя звукового файла
	const	char *FileName;
	// корректировка громкости
	float	VolumeCorrection;
	// номер группы
	int	Group;
	// макс. кол-во одновременно проигрываемых звуков в группе
	int	GroupCount;
	// номер подгруппы
	int	SubGroup;
	// макс. кол-во одновременно проигрываемых звуков в подгруппе
	int	SubGroupCount;
	// приоритет звука в группе (не подгруппе!) от 1 до ... (1-самый высокий)
	int	Priority;
};




// перечень имен файлов игровых звуков
static sGameSoundData GameSoundList[] = {

	{"sfx/weapon1probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, механическое оружие (Kinetic)
	{"sfx/weapon2probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, "слабое" энергетическое оружие (Ion, Plasma)
	{"sfx/weapon3probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, "лучевое/среднее" энергетическое оружие (Maser, Laser)
	{"sfx/weapon4probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, "мощьное" энергетическое оружие (Antimatter, Gauss)
	{"sfx/weapon5probl.wav", 1.0f, 1, 8, 1, 2, 10},	// оружие повреждено или нечем стрелять, пусковые установки (ракеты, торпеды, бомбы)

	{"sfx/explosion1.wav", 1.0f, 2, 6, 1, 4, 1},	// малый взрыв: ракеты
	{"sfx/explosion2.wav", 1.0f, 2, 6, 1, 4, 1},	// взрыв (обычный): торпеды, истребители землян и пиратов
	{"sfx/explosion3.wav", 1.0f, 2, 6, 1, 4, 1},	// взрыв (энергетический): ядерные бомбы, корабли пришельцев

	{"sfx/weaponfire1.wav", 0.7f, 1, 8, 2, 4, 4},	// выстрел, Kinetic 1
	{"sfx/weaponfire2.wav", 0.65f, 1, 8, 2, 4, 4},	// выстрел, Kinetic 2
	{"sfx/weaponfire3.wav", 0.7f, 1, 8, 2, 4, 4},	// выстрел, Kinetic 3
	{"sfx/weaponfire4.wav", 1.0f, 1, 8, 2, 4, 4},	// выстрел, Kinetic 4
	{"sfx/weaponfire5.wav", 1.0f, 1, 8, 2, 4, 4},	// выстрел, Ion 1
	{"sfx/weaponfire6.wav", 1.0f, 1, 8, 2, 4, 4},	// выстрел, Ion 2
	{"sfx/weaponfire7.wav", 0.7f, 1, 8, 2, 4, 4},	// выстрел, Ion 3
	{"sfx/weaponfire8.wav", 0.85f, 1, 8, 2, 4, 4},	// выстрел, Plasma 1
	{"sfx/weaponfire9.wav", 0.95f, 1, 8, 2, 4, 4},	// выстрел, Plasma 2
	{"sfx/weaponfire10.wav", 0.9f, 1, 8, 2, 4, 4},	// выстрел, Plasma 3
	{"sfx/weaponfire11.wav", 0.6f, 1, 8, 2, 4, 1},	// выстрел, Maser 1 - продолжительный, пока есть луч; приоритет выше, чтобы не останавливали
	{"sfx/weaponfire12.wav", 0.55f, 1, 8, 2, 4, 1},	// выстрел, Maser 2 - продолжительный, пока есть луч; приоритет выше, чтобы не останавливали
	{"sfx/weaponfire13.wav", 0.9f, 1, 8, 2, 4, 4},	// выстрел, Antimatter
	{"sfx/weaponfire14.wav", 0.8f, 1, 8, 2, 4, 1},	// выстрел, Laser - продолжительный, пока есть луч; приоритет выше, чтобы не останавливали
	{"sfx/weaponfire15.wav", 0.8f, 1, 8, 2, 4, 4},	// выстрел, Gauss
	{"sfx/weaponfire16.wav", 1.0f, 1, 8, 3, 2, 2},	// выстрел, одиночная ракета; приоритет выше, не красиво если снимаем "хвост" эффекта
	{"sfx/weaponfire17.wav", 1.0f, 1, 8, 3, 2, 3},	// выстрел, одна ракета из группы; приоритет выше, не красиво если снимаем "хвост" эффекта
	{"sfx/weaponfire18.wav", 1.0f, 1, 8, 3, 2, 2},	// выстрел, торпеда; приоритет выше, не красиво если снимаем "хвост" эффекта
	{"sfx/weaponfire19.wav", 1.0f, 1, 8, 3, 2, 2},	// выстрел, бомба; приоритет выше, не красиво если снимаем "хвост" эффекта


	{"sfx/kinetichit.wav", 1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Kinetic снарядов
	{"sfx/ionhit.wav", 1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Ion снарядов
	{"sfx/plasmahit.wav", 1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Plasma снарядов
	{"sfx/antimaterhit.wav",1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Antimatter снарядов
	{"sfx/gausshit.wav", 1.0f, 2, 6, 2, 2, 10},	// попадание и "разваливание" снаряда о корпус объекта, для Gauss снарядов

	{"sfx/explosion4.wav", 1.0f, 2, 6, 1, 4, 1},	// малый взрыв: астероиды

};
#define GameSoundQuantity sizeof(GameSoundList)/sizeof(GameSoundList[0])




//------------------------------------------------------------------------------------
// локальная структура данных 2D sfx
//------------------------------------------------------------------------------------
struct sSound2DData {
	// имя звукового файла
	const	char *FileName;
	// корректировка громкости
	float	VolumeCorrection;
	// нужно релизить при загрузке
	bool	NeedRelease;
};

// перечень имен файлов звуков для меню
static sSound2DData MenuSoundNames[] = {
	{"sfx/menu_onbutton.wav", 0.4f, false},		// навели на кнопку
	{"sfx/menu_click.wav", 0.6f, false},		// нажали на кнопку
	{"sfx/menu_new.wav", 1.0f, true},		// меняем меню
	{"sfx/menu_taping.wav", 0.8f, true},		// набор текста-названия профайла
	{"sfx/menu_online.wav", 0.75f, true},		// стоим над профайлом-миссией
	{"sfx/menu_selectline.wav", 1.0f, true},	// выбрали профайл-миссию
	{"sfx/menu_nclick.wav", 1.0f, false},		// не можем нажать на кнопку, но жмем
	{"sfx/drag_error.wav", 1.0f, true},		// не можем установить в слот
	{"sfx/drag_offslot.wav", 0.65f, true},		// снимаем со слота оружие
	{"sfx/drag_onslot.wav", 0.82f, true},		// устанавливаем в слот оружие
	{"sfx/drag_release.wav", 0.6f, true},		// освобождаем курсор от оружия, если тянули
	{"sfx/game_showmenu.wav", 1.0f, false},		// проказываем меню в игре (звук во время его появления)
	{"sfx/game_hidemenu.wav", 1.0f, false},		// скрываем меню в игре (звук во время его исчезновения)
	{"sfx/lowlife.wav", 1.0f, true},		// сирена или что-то подобное, когда менее 10% жизни остается (зацикленный небольшой фрагмент)
	{"sfx/menu_onbutton2.wav", 0.15f, false},	// навели на малую кнопку
};
#define MenuSoundQuantity sizeof(MenuSoundNames)/sizeof(MenuSoundNames[0])



// перечень имен файлов звуков для меню
static sSound2DData VoiceNames[] = {
	{"13_Attention.wav", 1.0f, true},
	{"13_EngineMalfunction.wav", 1.0f, true},
	{"13_MissileDetected.wav", 1.0f, true},//++
	{"13_PowerSupplyReestablished.wav", 1.0f, true},
	{"13_PrepareForAction.wav", 1.0f, true},//++
	{"13_ReactorMalfunction.wav", 1.0f, true},//++
	{"13_Warning.wav", 1.0f, true},//++
	{"13_WeaponDamaged.wav", 1.0f, true},//++
	{"13_WeaponDestroyed.wav", 1.0f, true},//++
	{"13_WeaponMalfunction.wav", 1.0f, true},//++
};
#define VoiceQuantity sizeof(VoiceNames)/sizeof(VoiceNames[0])









//------------------------------------------------------------------------------------
// Инициализация звукового модуля
//------------------------------------------------------------------------------------
bool InitAudio()
{
	if (!vw_InitSound())
		return false;

	return true;
}




//------------------------------------------------------------------------------------
// Завершение работы со звуком
//------------------------------------------------------------------------------------
void ShutdownAudio()
{
	vw_ShutdownSound();
}



//------------------------------------------------------------------------------------
// Запуск нужной музыки
//------------------------------------------------------------------------------------
void StartMusicWithFade(eMusicTheme StartMusic, float FadeInTime, float FadeOutTime)
{
	bool MusicLoop = true;
	std::string LoopFileName;
	float MusicCorrection = 1.0f;

	switch (StartMusic) {
	case eMusicTheme::NONE:
		CurrentPlayingMusicName.clear();
		break;
	case eMusicTheme::MENU:
		CurrentPlayingMusicName = "music/menu.ogg";
		break;
	case eMusicTheme::GAME:
		CurrentPlayingMusicName = "music/game.ogg";
		// during in-game mission restart we need "restart" music as well
		// so, if this music already playing - release it, and start it again (see code below)
		vw_ReleaseMusic(vw_FindMusicByName(CurrentPlayingMusicName));
		break;
	case eMusicTheme::BOSS:
		CurrentPlayingMusicName = "music/boss-loop.ogg";
		break;
	case eMusicTheme::FAILED:
		CurrentPlayingMusicName = "music/missionfailed.ogg";
		MusicCorrection = 0.7f;
		break;
	case eMusicTheme::CREDITS:
		CurrentPlayingMusicName = "music/boss-intro.ogg";
		MusicLoop = false;
		LoopFileName = "music/boss-loop.ogg";
		break;
	}

	if (Setup.Music_check && //если можно играть
	    Setup.MusicSw && // и громкость не нулевая
	    !CurrentPlayingMusicName.empty()) {

		// FadeOut all music themes, if we need something, we FadeIn it in code below
		vw_FadeOutAllIfMusicPlaying(FadeOutTime);

		if (!vw_FindMusicByName(CurrentPlayingMusicName)) {

			// создаем новый источник и проигрываем его
			sMusic *Music;
			Music = new sMusic;
			vw_AttachMusic(Music);

			// пытаемся загрузить и играть
			if (!Music->Play(CurrentPlayingMusicName, 0.0f, MusicCorrection*(Setup.MusicSw/10.0f),
					 MusicLoop, LoopFileName)) {
				vw_ReleaseMusic(Music);
				CurrentPlayingMusicName.clear();
			} else // we are playing new music theme, FadeIn it
				Music->FadeIn(1.0f, FadeInTime);
		} else // if we already playing this one - FadeIn it (rest will continue FadeOut, see code above)
			vw_FindMusicByName(CurrentPlayingMusicName)->FadeIn(1.0f, FadeInTime);
	}
}




// установка громкости на все 2д звуки
void Audio_SetSound2DMainVolume(float NewMainVolume)
{
	for (unsigned int i=0; i<MenuSoundQuantity; i++) {
		cSound* Tmp = vw_FindSoundByName(MenuSoundNames[i].FileName);
		if (Tmp != nullptr)
			Tmp->SetMainVolume(NewMainVolume);
	}
}
// установка громкости на все голосовые звуки
void Audio_SetVoiceMainVolume(float NewMainVolume)
{
	for (unsigned int i=0; i<VoiceQuantity; i++) {
		cSound* Tmp = vw_FindSoundByName(vw_GetText(VoiceNames[i].FileName, Setup.VoiceLanguage));
		if (Tmp != nullptr)
			Tmp->SetMainVolume(NewMainVolume);
	}
}




//------------------------------------------------------------------------------------
// Проигрываем звук в меню, или другие 2д звуки
//------------------------------------------------------------------------------------
int Audio_PlaySound2D(unsigned int SoundID, float fVol, bool Loop)
{
	if (!Setup.Sound_check ||
	    !Setup.SoundSw ||
	    (SoundID > MenuSoundQuantity))
		return 0;

	// т.к. у нас со смещением же в 1 идет
	SoundID --;

	fVol = fVol*MenuSoundNames[SoundID].VolumeCorrection;

	// если это звук меню и мы его игрываем, его надо перезапустить
	cSound* Tmp = vw_FindSoundByName(MenuSoundNames[SoundID].FileName);
	if (Tmp != nullptr) {
		Tmp->Replay();
		return Tmp->Num;
	}


	// создаем новый источник и проигрываем его
	cSound *Sound = new cSound;
	vw_AttachSound(Sound);

	// чтобы не было искажения по каналам, делаем установку относительно камеры...
	if (!Sound->Play(MenuSoundNames[SoundID].FileName, fVol, Setup.SoundSw/10.0f, 0.0f, 0.0f, 0.0f,
			 true, Loop, MenuSoundNames[SoundID].NeedRelease, 1)) {
		vw_ReleaseSound(Sound);
		return 0;
	}

	return Sound->Num;
}






//------------------------------------------------------------------------------------
// Проигрываем голос
//------------------------------------------------------------------------------------
int Audio_PlayVoice(unsigned int VoiceID, float fVol, bool Loop)
{
	if (!Setup.Sound_check ||
	    !Setup.SoundSw ||
	    (VoiceID > VoiceQuantity))
		return 0;

	// т.к. у нас со смещением же в 1 идет
	VoiceID --;

	// русский голос делаем немного тише
	if (Setup.VoiceLanguage==3)
		VoiceNames[VoiceID].VolumeCorrection = 0.6f;

	fVol = fVol*VoiceNames[VoiceID].VolumeCorrection;

	// создаем новый источник и проигрываем его
	cSound *Sound = new cSound;
	vw_AttachSound(Sound);

	// чтобы не было искажения по каналам, делаем установку относительно камеры...
	if (!Sound->Play(vw_GetText(VoiceNames[VoiceID].FileName, Setup.VoiceLanguage), fVol, Setup.VoiceSw/10.0f, 0.0f, 0.0f, 0.0f, true, Loop, VoiceNames[VoiceID].NeedRelease, 1)) {
		vw_ReleaseSound(Sound);
		return 0;
	}

	return Sound->Num;
}




//------------------------------------------------------------------------------------
// Проигрываем 3д звуки
//------------------------------------------------------------------------------------
int Audio_PlaySound3D(int SoundID, float fVol, sVECTOR3D Location, bool Loop, int AtType)
{
	if (!Setup.Sound_check ||
	    !Setup.SoundSw)
		return 0;

	fVol = fVol*GameSoundList[SoundID-1].VolumeCorrection;

	// т.к. у нас со смещением же в 1 идет
	SoundID --;

	// проверяем, можем ли мы играть этот звук
	if (!vw_CheckCanPlaySound(GameSoundList[SoundID].Group, GameSoundList[SoundID].GroupCount, GameSoundList[SoundID].SubGroup,
				  GameSoundList[SoundID].SubGroupCount, GameSoundList[SoundID].Priority))
		return 0;


	// создаем новый источник и проигрываем его
	cSound *Sound = new cSound;
	vw_AttachSound(Sound);
	if (!Sound->Play(GameSoundList[SoundID].FileName, fVol, Setup.SoundSw/10.0f, Location.x, Location.y, Location.z, false, Loop, true, AtType)) {
		vw_ReleaseSound(Sound);
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
	sVECTOR3D CurrentCameraLocation;
	vw_GetCameraLocation(&CurrentCameraLocation);
	sVECTOR3D CurrentCameraRotation;
	vw_GetCameraRotation(&CurrentCameraRotation);

	sVECTOR3D ListenerOriV1(0.0f, 0.0f, -1.0f);
	vw_RotatePoint(ListenerOriV1, CurrentCameraRotation);
	sVECTOR3D ListenerOriV2(0.0f, 1.0f, 0.0f);
	vw_RotatePoint(ListenerOriV2, CurrentCameraRotation);

	// Position of the Listener.
	ALfloat ListenerPos[3] = { CurrentCameraLocation.x, CurrentCameraLocation.y, CurrentCameraLocation.z };
	// Velocity of the Listener.
	ALfloat ListenerVel[3] = { 0.0f, 0.0f, 0.0f };
	// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
	// Also note that these should be units of '1'.
	ALfloat ListenerOri[6] = { ListenerOriV1.x, ListenerOriV1.y, ListenerOriV1.z,
				   ListenerOriV2.x, ListenerOriV2.y, ListenerOriV2.z
				 };

	vw_Listener(ListenerPos, ListenerVel, ListenerOri);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// передаем управление, чтобы внутри ядра все сделали
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_UpdateSound();
	vw_UpdateMusic();




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// запускаем нужную музыку... только включили громкость или выключили
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!vw_IsMusicPlaying()) {
		if (Setup.MusicSw && // если громкость не нулевая
		    Setup.Music_check && // если можно вообще играть
		    (!CurrentPlayingMusicName.empty()) && // если установлен
		    (!vw_FindMusicByName(CurrentPlayingMusicName))) { // если это еще не играем
			// создаем новый источник и проигрываем его
			sMusic *Music;
			Music = new sMusic;
			vw_AttachMusic(Music);

			if (!Music->Play(CurrentPlayingMusicName, 1.0f, Setup.MusicSw/10.0f, true, "")) {
				vw_ReleaseMusic(Music);
				CurrentPlayingMusicName.clear();
			}
		}
	} else {
		// если что-то играем, но звук уже выключен, нужно все убрать...
		if (!Setup.MusicSw && // если громкость нулевая
		    Setup.Music_check) { // но играть можно
			vw_ReleaseAllMusic();
		}
	}


}
