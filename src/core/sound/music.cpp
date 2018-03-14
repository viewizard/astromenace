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
// TODO switch from vw_GetTimeThread() to SDL_GetTicks() usage

/*
In the case of music we based on several main principles:
1. All music connected to its name, so, name is the key (index).
   If music have 'loop' part, only main part (its file name) taken into account.
2. Music can be played simultaneously only in one stream.
   This is connected not only to p.1, but also to our stream buffers realization,
   that will not allow create more then one stream buffer for same source.
*/

#include "../system/system.h"
#include "buffer.h"

namespace {

struct sMusic {

	~sMusic()
	{
		if (!alIsSource(Source))
			return;
		// обязательно остановить!!!
		alSourceStop(Source);
		// открепляем все буферы от источника
		if (Stream)
			vw_UnqueueStreamBuffer(Stream, Source);
		// освобождаем собственно сам источник
		alDeleteSources(1, &Source);
		alGetError(); // сброс ошибок
	};

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
};

std::unordered_map<std::string, sMusic> MusicMap;

} // unnamed namespace

ALboolean CheckALError(const char *FunctionName);
ALboolean CheckALUTError(const char *FunctionName);


//------------------------------------------------------------------------------------
// Проигрывание звука
//------------------------------------------------------------------------------------
bool vw_PlayMusic(const std::string &Name, float fVol, float fMainVol, bool Loop, const std::string &LoopFileName)
{
	if (Name.empty()) // LoopFileName could be empty
		return false;

	MusicMap[Name].Volume = fVol; // create entry on first access
	MusicMap[Name].MainVolume = fMainVol;
	MusicMap[Name].LoopPart = LoopFileName;
	MusicMap[Name].FadeInStartVol = fVol;
	MusicMap[Name].FadeInEndVol = fVol;
	MusicMap[Name].LastTime = vw_GetTimeThread(0);

	// Position of the source sound.
	ALfloat SourcePos[] = {0.0f, 0.0f, 0.0f}; // -1.0 1.0 по иксам это баланс
	// Velocity of the source sound.
	ALfloat SourceVel[] = {0.0f, 0.0f, 0.0f};

	alGenSources(1, &MusicMap[Name].Source);
	if(!CheckALError(__func__))
		return false;

	alSourcef(MusicMap[Name].Source, AL_PITCH, 1.0); // 1.0 only!!!
	alSourcef(MusicMap[Name].Source, AL_GAIN, fVol*fMainVol); // фактически громкость
	alSourcefv(MusicMap[Name].Source, AL_POSITION, SourcePos);
	alSourcefv(MusicMap[Name].Source, AL_VELOCITY, SourceVel);
	alSourcei(MusicMap[Name].Source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcei(MusicMap[Name].Source, AL_LOOPING, AL_FALSE);
	alGetError(); // сброс ошибок
	MusicMap[Name].Looped = Loop;

	MusicMap[Name].Stream = vw_CreateStreamBufferFromOGG(Name, LoopFileName);
	if (!MusicMap[Name].Stream)
		return false;

	if (!vw_QueueStreamBuffer(MusicMap[Name].Stream, MusicMap[Name].Source))
		return false;

	alSourcePlay(MusicMap[Name].Source);
	if(!CheckALError(__func__))
		return false;

	return true;
}

static bool CheckSourceState(ALuint Source, ALint State)
{
	ALint tmpState;
	alGetSourcei(Source, AL_SOURCE_STATE, &tmpState);
	alGetError(); // сброс ошибок
	return (tmpState == State);
}

//------------------------------------------------------------------------------------
// обновление потока
//------------------------------------------------------------------------------------
bool sMusic::Update()
{
	vw_UpdateStreamBuffer(Stream, Source, Looped, LoopPart);

	// обрабатываем эффекты
	float TimeDelta = vw_GetTimeThread(0) - LastTime;

	if (FadeInSw && (Volume < FadeInEndVol)) {
		FadeTime += TimeDelta;
		Volume = FadeInEndVol*(FadeTime/FadeAge);
		alSourcef(Source, AL_GAIN, MainVolume*Volume );
		alGetError(); // сброс ошибок
	}

	if (FadeOutSw && (Volume > 0.0f)) {
		FadeTime += TimeDelta;
		Volume = FadeInStartVol*((FadeAge-FadeTime)/FadeAge);
		alSourcef(Source, AL_GAIN, MainVolume*Volume);
		alGetError(); // сброс ошибок
		if (Volume <= 0.0f)
			return false;
	}

	LastTime = vw_GetTimeThread(0);

	// если по какой-то причине перестали играть...
	if (CheckSourceState(Source, AL_STOPPED))
		return false;

	return true;
}

//------------------------------------------------------------------------------------
// установка громкости
//------------------------------------------------------------------------------------
void sMusic::SetMainVolume(float NewMainVolume)
{
	if (!alIsSource(Source))
		return;

	MainVolume = NewMainVolume;
	alSourcef(Source, AL_GAIN, MainVolume*Volume);
}

//------------------------------------------------------------------------------------
// плавное появление
//------------------------------------------------------------------------------------
void sMusic::FadeIn(float EndVol, float Time)
{
	FadeInSw = true;
	FadeOutSw = false;
	FadeInEndVol = EndVol;
	FadeTime = 0.0f;
	FadeAge = Time;
	LastTime = vw_GetTimeThread(0);
}

//------------------------------------------------------------------------------------
// плавное уход на 0 с текущего
//------------------------------------------------------------------------------------
void sMusic::FadeOut(float Time)
{
	FadeOutSw = true;
	FadeInSw = false;
	FadeInStartVol = Volume;
	FadeTime = 0.0f;
	FadeAge = Time;
	LastTime = vw_GetTimeThread(0);
}

void vw_FadeOutIfMusicPlaying(float Time)
{
	for (auto &tmpMusic : MusicMap) {
		if (alIsSource(tmpMusic.second.Source) &&
		    CheckSourceState(tmpMusic.second.Source, AL_PLAYING))
			tmpMusic.second.FadeOut(Time);
	}
}

//------------------------------------------------------------------------------------
// Возвращаем, если играется хотя бы одна музыка
//------------------------------------------------------------------------------------
bool vw_IsMusicPlaying()
{
	for (auto &tmpMusic : MusicMap) {
		return alIsSource(tmpMusic.second.Source) &&
		       CheckSourceState(tmpMusic.second.Source, AL_PLAYING);
	}

	return false;
}

//------------------------------------------------------------------------------------
// Освобождение памяти и удаление
//------------------------------------------------------------------------------------
void vw_ReleaseMusic(const std::string &Name)
{
	if (Name.empty())
		return;

	MusicMap.erase(Name);
}

//------------------------------------------------------------------------------------
// освобождаем все подключенные
//------------------------------------------------------------------------------------
void vw_ReleaseAllMusic()
{
	MusicMap.clear();
}

//------------------------------------------------------------------------------------
// Обновляем данные
//------------------------------------------------------------------------------------
void vw_UpdateMusic()
{
	for (auto iter = MusicMap.begin(); iter != MusicMap.end();) {
		if(!iter->second.Update()) {
			iter = MusicMap.erase(iter);
		} else
			++iter;
	}
}

void vw_SetMusicFadeIn(const std::string &Name, float EndVol, float Time)
{
	auto tmpMusic = MusicMap.find(Name);
	if (tmpMusic != MusicMap.end())
		tmpMusic->second.FadeIn(EndVol, Time);
}

//------------------------------------------------------------------------------------
// Установка громкости у всех
//------------------------------------------------------------------------------------
void vw_SetMusicMainVolume(float NewMainVolume)
{
	for (auto & tmpMusic : MusicMap) {
		tmpMusic.second.SetMainVolume(NewMainVolume);
	}
}

//------------------------------------------------------------------------------------
// Проверка наличия по имени
//------------------------------------------------------------------------------------
bool vw_CheckMusicAvailabilityByName(const std::string &Name)
{
	auto tmpMusic = MusicMap.find(Name);
	if (tmpMusic != MusicMap.end())
		return true;

	return false;
}
