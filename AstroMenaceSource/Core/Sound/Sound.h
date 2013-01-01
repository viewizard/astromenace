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


#ifndef Sound_H
#define Sound_H

#include "../Base.h"
#include "../Math/Math.h"
#include "../VirtualFileSystem/VFS.h"


#if defined(__APPLE__) && defined(__MACH__)
	#include "OpenAL/al.h"
	#include "OpenAL/alc.h"
#else // defined(__APPLE__) && defined(__MACH__)
	#include "AL/al.h"
	#include "AL/alc.h"
#endif // defined(__APPLE__) && defined(__MACH__)
#include "AL/alut.h"

#include "vorbis/vorbisfile.h"
#include "ogg/ogg.h"

#include "Buffer.h"

#define NUM_OF_DYNBUF	20		// num buffers in queue
#define DYNBUF_SIZE		16384	// Buffer size


//------------------------------------------------------------------------------------
// Структура звука
//------------------------------------------------------------------------------------
class eSound
{
public:

	eSound(){FileName=0;};
	~eSound(){if (FileName != 0){delete [] FileName; FileName=0;}};

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


	char*		FileName;

	ALuint		Source;		// источник
	float		Volume;		// громкость, внутренняя... для данного источника (чтобы была возможность корректировки общей громкости)
	float		MainVolume;
	bool		NeedRelease; // для 2-х типов релиза...

	// установка информации о звуке
	void SetInfo(int NewGroup, int NewGroupCount, int NewSubGroup, int NewSubGroupCount, int NewPriority);
	int			Group; 			// номер группы
	int			GroupCount; 	// макс. кол-во одновременно проигрываемых звуков в группе
	int			SubGroup;		// номер подгруппы
	int			SubGroupCount;	// макс. кол-во одновременно проигрываемых звуков в подгруппы
	int			Priority;		// приоритет звука в группе от 1 до ... (1-самый высокий)
	float 		Age;			// время, в течении которого проигрываем звук (для остановки более старого звука)
	float		LastUpdateTime; // тянем тут, т.к. глобальный может быть не корректный (если была остановка игры)


	float		DestroyTime;
	float		DestroyTimeStart;

	eSound*		Prev;		// Pointer to the previous loaded Sound in the memory
	eSound*		Next;		// Pointer to the next loaded Sound in the memory
	int			Num;		// ID number
};




//------------------------------------------------------------------------------------
// Структура музыки
//------------------------------------------------------------------------------------
struct eMusic
{
	// проигрывание музыки
	bool Play(const char * Name, float fVol, float fMainVol, bool Loop, const char *LoopFileName);
	char LoopPart[MAX_PATH];
	// плавное появление
	void FadeIn(float EndVol, float Time);
	// плавное уход на 0 с текущего
	void FadeOut(float Time);

	// обновление данных потока
	bool Update();
	// читаем фрагмент огг файла
	bool ReadOggBlock(ALuint BufID, size_t Size);

	// установка или изменение общей громкости
	void SetMainVolume(float NewMainVolume);

	ALuint		Source;		// источник
	float		Volume;		// громкость, внутренняя... для данного источника (чтобы была возможность корректировки общей громкости)
	float		MainVolume;
	ALuint 		Buffers[NUM_OF_DYNBUF];		// у музыки свой собственный буфер
	bool		Looped;		// запоминаем, нужно ли зацикливание

	eFILE 		*MusicFile; // файл, для потока

	ALsizei		Rate; // OAL specific
	ALenum		Format;	 // OAL specific
	// vorbisfile specific vars
	OggVorbis_File	*mVF;
	vorbis_comment	*mComment;
	vorbis_info		*mInfo;

	bool		FadeInSw;
	float		FadeInEndVol;
	float		FadeInStartVol;
	bool		FadeOutSw;
	float		FadeTime;
	float		FadeAge;
	float		LastTime;

	eMusic*		Prev;		// Pointer to the previous loaded Sound in the memory
	eMusic*		Next;		// Pointer to the next loaded Sound in the memory
	int			Num;		// ID number
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
eSound* vw_FindSoundByNum(int Num);
eSound* vw_FindSoundByName(const char *Name);
void vw_SetSoundMainVolume(float NewMainVolume);
void vw_UpdateSound();
bool vw_CheckCanPlaySound(int Group, int GroupCount, int SubGroup, int SubGroupCount, int Priority);

void vw_ReleaseSound(eSound* Sound);
void vw_ReleaseAllSounds(int ReleaseType);
void vw_AttachSound(eSound* Sound);
void vw_DetachSound(eSound* Sound);



//------------------------------------------------------------------------------------
// Функции для работы с музыкой
//------------------------------------------------------------------------------------
void vw_SetMusicMainVolume(float NewMainVolume);
bool vw_GetMusicIsPlaying();
void vw_UpdateMusic();
eMusic* vw_FindMusicByNum(int Num);

void vw_ReleaseMusic(eMusic* Music);
void vw_ReleaseAllMusic();
void vw_AttachMusic(eMusic* Music);
void vw_DetachMusic(eMusic* Music);





#endif // Sound_H
