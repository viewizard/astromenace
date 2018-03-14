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

struct sFILE;


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
	bool Play(const char *Name, float fVol, float fMainVol, float x, float y, float z, bool Relative, bool Loop, bool NeedReleaseStatus, int AtType);
	// перезапуск воспроизведения
	void Replay();
	// остановка звука (0.0f - остановить сразу)
	void Stop(float StopDelay);
	// для 3д звука установка положения (баланса)
	void SetLocation(float x, float y, float z);
	// установка или изменение общей громкости
	void SetMainVolume(float NewMainVolume);

	char *FileName{nullptr};

	ALuint Source{0};		// источник
	float Volume{0.0f};		// громкость, внутренняя... для данного источника (чтобы была возможность корректировки общей громкости)
	float MainVolume{0.0f};
	bool NeedRelease{false};	// для 2-х типов релиза...

	// установка информации о звуке
	void SetInfo(int NewGroup, int NewGroupCount, int NewSubGroup, int NewSubGroupCount, int NewPriority);
	int Group{0}; 			// номер группы
	int GroupCount{0}; 		// макс. кол-во одновременно проигрываемых звуков в группе
	int SubGroup{0};		// номер подгруппы
	int SubGroupCount{0};		// макс. кол-во одновременно проигрываемых звуков в подгруппы
	int Priority{0};		// приоритет звука в группе от 1 до ... (1-самый высокий)
	float Age{0.0f};		// время, в течении которого проигрываем звук (для остановки более старого звука)
	float LastUpdateTime{0.0f};	// тянем тут, т.к. глобальный может быть не корректный (если была остановка игры)

	float DestroyTime{0.0f};
	float DestroyTimeStart{0.0f};

	cSound *Prev{nullptr};	// Pointer to the previous loaded Sound in the memory
	cSound *Next{nullptr};	// Pointer to the next loaded Sound in the memory
	int Num{0};		// ID number
};

//------------------------------------------------------------------------------------
// Структура музыки
//------------------------------------------------------------------------------------
struct sMusic {

	~sMusic()
	{
		// обязательно остановить!!!
		if (!alIsSource(Source))
			return;
		alSourceStop(Source);
		// открепляем все буферы от источника
		if (Stream)
			vw_UnqueueStreamBuffer(Stream, Source);
		// освобождаем собственно сам источник
		alDeleteSources(1, &Source);
		alGetError(); // сброс ошибок
	};

	// проигрывание музыки
	bool Play(const std::string &Name, float fVol, float fMainVol, bool Loop, const std::string &LoopFileName);
	// плавное появление
	void FadeIn(float EndVol, float Time);
	// плавное уход на 0 с текущего
	void FadeOut(float Time);
	// обновление данных потока
	bool Update();
	// установка или изменение общей громкости
	void SetMainVolume(float NewMainVolume);

	sStreamBuffer *Stream{nullptr};

	ALuint Source;		// источник
	float Volume;		// громкость, внутренняя... для данного источника (чтобы была возможность корректировки общей громкости)
	float MainVolume;
	bool Looped;		// запоминаем, нужно ли зацикливание
	std::string LoopPart;

	bool FadeInSw{false};
	float FadeInEndVol;
	float FadeInStartVol;
	bool FadeOutSw{false};
	float FadeTime{0.0f};
	float FadeAge{0.0f};
	float LastTime;

	sMusic *Prev;		// Pointer to the previous loaded Sound in the memory
	sMusic *Next;		// Pointer to the next loaded Sound in the memory
	int Num;		// ID number
};

//------------------------------------------------------------------------------------
// Функции для работы со звуком
//------------------------------------------------------------------------------------
bool vw_InitSound();
void vw_ShutdownSound();
void vw_Listener(float ListenerPos[3], float ListenerVel[3], float ListenerOri[6]);

//------------------------------------------------------------------------------------
// Функции для работы с SFX
//------------------------------------------------------------------------------------
cSound *vw_FindSoundByNum(int Num);
cSound *vw_FindSoundByName(const char *Name);
void vw_SetSoundMainVolume(float NewMainVolume);
void vw_UpdateSound();
bool vw_CheckCanPlaySound(int Group, int GroupCount, int SubGroup, int SubGroupCount, int Priority);

void vw_ReleaseSound(cSound *Sound);
void vw_ReleaseAllSounds(int ReleaseType);
void vw_AttachSound(cSound *Sound);
void vw_DetachSound(cSound *Sound);

//------------------------------------------------------------------------------------
// Функции для работы с музыкой
//------------------------------------------------------------------------------------
void vw_SetMusicMainVolume(float NewMainVolume);
bool vw_IsMusicPlaying();
void vw_UpdateMusic();
sMusic *vw_FindMusicByNum(int Num);

void vw_ReleaseMusic(sMusic *Music);
void vw_ReleaseAllMusic();
void vw_AttachMusic(sMusic *Music);
void vw_DetachMusic(sMusic *Music);

#endif // Sound_H
