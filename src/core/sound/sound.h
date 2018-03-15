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

// TODO translate comments

#ifndef Sound_H
#define Sound_H

#include "../base.h"
#include "buffer.h"

//------------------------------------------------------------------------------------
// Структура звука
//------------------------------------------------------------------------------------
class cSound
{
public:

	cSound()
	{};
	~cSound()
	{
		if (FileName != nullptr)
			delete [] FileName;
	};

	// проигрывание звука
	// нельзя разделять пока на отдельную загрузку и проигрывание... т.к. удаляем по остановке!!!
	bool Play(const char *Name, float _LocalVolume, float _GlobalVolume, float x, float y, float z, bool Relative, bool Loop, bool NeedReleaseStatus, int AtType);
	// перезапуск воспроизведения
	void Replay();
	// остановка звука (0.0f - остановить сразу)
	void Stop(float StopDelay);
	// для 3д звука установка положения (баланса)
	void SetLocation(float x, float y, float z);
	// установка или изменение общей громкости
	void SetGlobalVolume(float NewMainVolume);

	char *FileName{nullptr};

	ALuint Source{0};		// источник
	float LocalVolume{0.0f};
	float GlobalVolume{0.0f};
	bool NeedRelease{false};	// для 2-х типов релиза...

	float LastUpdateTime{0.0f};	// тянем тут, т.к. глобальный может быть не корректный (если была остановка игры)

	float DestroyTime{0.0f};
	float DestroyTimeStart{0.0f};

	cSound *Prev{nullptr};	// Pointer to the previous loaded Sound in the memory
	cSound *Next{nullptr};	// Pointer to the next loaded Sound in the memory
	int Num{0};		// ID number
};

/*
 * Sound FX.
 */

cSound *vw_FindSoundByNum(int Num);
cSound *vw_FindSoundByName(const char *Name);
void vw_SetSoundGlobalVolume(float NewGlobalVolume);
void vw_UpdateSound();

void vw_ReleaseSound(cSound *Sound);
void vw_ReleaseAllSounds(int ReleaseType);
void vw_AttachSound(cSound *Sound);
void vw_DetachSound(cSound *Sound);

/*
 * Music.
 */

// Create and play music.
bool vw_PlayMusic(const std::string &Name, float LocalVolume, float GlobalVolume,
		  bool Loop, const std::string &LoopFileName);
// Set global music volume.
void vw_SetMusicGlobalVolume(float NewGlobalVolume);
// Check, is any music theme playing.
bool vw_IsAnyMusicPlaying();
// Set all music fade-out, if playing.
void vw_FadeOutIfMusicPlaying(float Time);
// Set music fade-in.
void vw_SetMusicFadeIn(const std::string &Name, float EndVol, float Time);
// Update all music themes status and calculate effects.
void vw_UpdateMusic();
// Release particular music theme by name. Also could be used for "stop" playing.
void vw_ReleaseMusic(const std::string &Name);
// Release all music. Also could be used for "stop" playing all music themes.
void vw_ReleaseAllMusic();

/*
 * Misc.
 */

bool vw_InitSound();
void vw_ShutdownSound();
void vw_Listener(float ListenerPos[3], float ListenerVel[3], float ListenerOri[6]);

#endif // Sound_H
