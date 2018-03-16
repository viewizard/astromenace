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
// TODO fix mess with vw_PlaySound() AtType parameter, probably, we need setup this
// by additional function call instead of hard code it

#include "../system/system.h"
#include "../math/math.h"
#include "buffer.h"

namespace {

struct sSound {

	~sSound()
	{
		if (!alIsSource(Source))
			return;
		// если останавливать играющий звук, возможен щелчек (и в линуксе и в виндовсе)
		alSourceStop(Source);
		// освобождаем собственно сам источник
		alDeleteSources(1, &Source);
		alGetError(); // сброс ошибок
	};

	// перезапуск воспроизведения
	void Replay();
	// остановка звука (0.0f - остановить сразу)
	void Stop(float StopDelay);
	// для 3д звука установка положения (баланса)
	void SetLocation(float x, float y, float z);
	// установка или изменение общей громкости
	void SetGlobalVolume(float NewMainVolume);

	std::string FileName;

	ALuint Source{0};
	float LocalVolume{0.0f};
	float GlobalVolume{0.0f};
	bool AllowedStop{false};	// allowed stop during vw_StopAllSoundsIfAllowed() call

	float LastUpdateTime{0.0f};

	float DestroyTime{0.0f};
	float DestroyTimeStart{0.0f};
};

std::unordered_map<unsigned int, sSound> SoundsMap;

} // unnamed namespace

ALboolean CheckALError(const char *FunctionName);
ALboolean CheckALUTError(const char *FunctionName);
bool CheckALSourceState(ALuint Source, ALint State);


/*
 * Generate new unique sound ID. Returned number should be non zero,
 * and sounds map should not have an element with this key.
 */
static unsigned int GenerateUniqueSoundID()
{
	static unsigned int LastSoundID{0};

	LastSoundID++;

	// since we could reach the "unsigned int" maximum (4294967295) and back to 0 again
	while (!LastSoundID &&
	       (SoundsMap.find(LastSoundID) != SoundsMap.end())) {
		LastSoundID++;
	}

	return LastSoundID;
}

/*
 * Load sound buffer data according to file extension.
 */
unsigned int vw_LoadSoundBuffer(const std::string &Name)
{
	if (vw_CheckFileExtension(Name, ".wav"))
		return vw_CreateSoundBufferFromWAV(Name);
	else if (vw_CheckFileExtension(Name, ".ogg"))
		return vw_CreateSoundBufferFromOGG(Name);

	return 0;
}

//------------------------------------------------------------------------------------
// Проигрывание звука
//------------------------------------------------------------------------------------
unsigned int vw_PlaySound(const std::string &Name, float _LocalVolume, float _GlobalVolume,
			  float x, float y, float z, bool Relative, bool AllowStop, int AtType)
{
	if (Name.empty())
		return 0;

	unsigned int tmpSoundID = GenerateUniqueSoundID();

	SoundsMap[tmpSoundID].Source = 0; // create entry on first access
	SoundsMap[tmpSoundID].LastUpdateTime = vw_GetTimeThread(0);
	SoundsMap[tmpSoundID].FileName = Name;
	SoundsMap[tmpSoundID].DestroyTime = -1.0f;
	SoundsMap[tmpSoundID].DestroyTimeStart = -1.0f;
	SoundsMap[tmpSoundID].AllowedStop = AllowStop;

	ALuint Buffer = vw_LoadSoundBuffer(Name);
	if (!Buffer) {
		SoundsMap.erase(tmpSoundID);
		return 0;
	}

	SoundsMap[tmpSoundID].LocalVolume = _LocalVolume;
	SoundsMap[tmpSoundID].GlobalVolume = _GlobalVolume;

	// Position of the source sound.
	ALfloat SourcePos[] = {x, y, z}; // -1.0 1.0 по иксам это баланс
	// Velocity of the source sound.
	ALfloat SourceVel[] = {0.0f, 0.0f, 0.0f};

	// Bind the buffer with the source.
	alGenSources(1, &SoundsMap[tmpSoundID].Source);
	if(!CheckALError(__func__)) {
		SoundsMap.erase(tmpSoundID);
		return 0;
	}

	alSourcei(SoundsMap[tmpSoundID].Source, AL_BUFFER, Buffer);

	alSourcef(SoundsMap[tmpSoundID].Source, AL_GAIN, _GlobalVolume * _LocalVolume);
	alSourcefv(SoundsMap[tmpSoundID].Source, AL_POSITION, SourcePos);
	alSourcefv(SoundsMap[tmpSoundID].Source, AL_VELOCITY, SourceVel);
	alSourcei(SoundsMap[tmpSoundID].Source, AL_LOOPING, false);
	alSourcei(SoundsMap[tmpSoundID].Source, AL_SOURCE_RELATIVE, Relative);
	alGetError(); // сброс ошибок

	// параметры затухания сигнала от источника
	if (AtType == 1) {
		alSourcef(SoundsMap[tmpSoundID].Source, AL_REFERENCE_DISTANCE, 30.0f);
		alSourcef(SoundsMap[tmpSoundID].Source, AL_MAX_DISTANCE, 250.0f);
		alSourcef(SoundsMap[tmpSoundID].Source, AL_ROLLOFF_FACTOR, 0.5f);
		alGetError(); // сброс ошибок
	} else if (AtType == 2) {
		alSourcef(SoundsMap[tmpSoundID].Source, AL_REFERENCE_DISTANCE, 150.0f);
		alSourcef(SoundsMap[tmpSoundID].Source, AL_MAX_DISTANCE, 600.0f);
		alSourcef(SoundsMap[tmpSoundID].Source, AL_ROLLOFF_FACTOR, 0.2f);
		alGetError(); // сброс ошибок
	}

	alSourcePlay(SoundsMap[tmpSoundID].Source);
	if(!CheckALError(__func__)) {
		SoundsMap.erase(tmpSoundID);
		return 0;
	}

	return tmpSoundID;
}

//------------------------------------------------------------------------------------
// перезапуск
//------------------------------------------------------------------------------------
void sSound::Replay()
{
	if (!alIsSource(Source))
		return;

	alSourceRewind(Source);
	alSourcePlay(Source);
}

//------------------------------------------------------------------------------------
// остановка
//------------------------------------------------------------------------------------
void sSound::Stop(float StopDelay)
{
	if (!alIsSource(Source))
		return;

	if (StopDelay < 0.0f)
		StopDelay = 0.0f;

	if (StopDelay == 0.0f) {
		alSourceStop(Source);
		alGetError(); // сброс ошибок
	} else {
		DestroyTimeStart = DestroyTime = StopDelay;
	}
}

//------------------------------------------------------------------------------------
// для 3д звука установка положения (баланса)
//------------------------------------------------------------------------------------
void sSound::SetLocation(float x, float y, float z)
{
	// если это не источник, уходим
	if (!alIsSource(Source))
		return;

	ALfloat SourcePos[] = {x, y, z};
	alSourcefv(Source, AL_POSITION, SourcePos);
	alGetError(); // сброс ошибок
}

//------------------------------------------------------------------------------------
// установка громкости
//------------------------------------------------------------------------------------
void sSound::SetGlobalVolume(float NewGlobalVolume)
{
	if (!alIsSource(Source))
		return;

	GlobalVolume = NewGlobalVolume;
	alSourcef(Source, AL_GAIN, GlobalVolume * LocalVolume);
	alGetError(); // сброс ошибок
}

//------------------------------------------------------------------------------------
// освобождаем все подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllSounds()
{
	SoundsMap.clear();
}

void vw_StopAllSoundsIfAllowed()
{
	for (auto iter = SoundsMap.begin(); iter != SoundsMap.end();) {
		if (iter->second.AllowedStop) {
			// stopped sound will be released on update() call,
			// so, no reason stop sound, release it right now
			iter = SoundsMap.erase(iter);
		} else
			++iter;
	}
}

bool vw_IsSoundAvailable(unsigned int ID)
{
	if (!ID)
		return false;

	auto tmpSound = SoundsMap.find(ID);
	if (tmpSound != SoundsMap.end())
		return true;

	return false;
}

/*
 * Replay from the beginning first sound, found by name.
 */
unsigned int vw_ReplayFirstFoundSound(const std::string &Name)
{
	if (Name.empty())
		return 0;

	for (auto &tmpSound : SoundsMap) {
		if (tmpSound.second.FileName == Name) {
			tmpSound.second.Replay();
			return tmpSound.first;
		}
	}

	return 0;
}

//------------------------------------------------------------------------------------
// Обновляем данные в менеджере
//------------------------------------------------------------------------------------
void vw_UpdateSound()
{
	float CurrentGetTime = vw_GetTimeThread(0);

	for (auto iter = SoundsMap.begin(); iter != SoundsMap.end();) {
		// считаем сколько играем этот эффект
		float DeltaTime = CurrentGetTime - iter->second.LastUpdateTime;
		iter->second.LastUpdateTime = CurrentGetTime;
		bool NeedRelease{false};

		if ((alIsSource(iter->second.Source)) &&
		    (iter->second.DestroyTimeStart > 0.0f)) {
			if (iter->second.DestroyTime > 0.0f) {
				iter->second.DestroyTime -= DeltaTime;
				if (iter->second.DestroyTime < 0.0f)
					iter->second.DestroyTime = 0.0f;
				alSourcef (iter->second.Source, AL_GAIN, iter->second.GlobalVolume * iter->second.LocalVolume *
									 (iter->second.DestroyTime / iter->second.DestroyTimeStart));
				alGetError(); // сброс ошибок
			} else {
				// уже нулевая громкость, можем удалять
				NeedRelease = true;
			}
		}

		// смотрим, если уже не играем - надо удалить
		if (alIsSource(iter->second.Source) &&
		    CheckALSourceState(iter->second.Source, AL_STOPPED))
			NeedRelease = true;

		if (NeedRelease)
			iter = SoundsMap.erase(iter);
		else
			++iter;
	}
}

void vw_SetSoundGlobalVolume(const std::string &Name, float NewGlobalVolume)
{
	if (Name.empty())
		return;

	for (auto &tmpSound : SoundsMap) {
		if (tmpSound.second.FileName == Name) {
			tmpSound.second.SetGlobalVolume(NewGlobalVolume);
			return;
		}
	}
}

void vw_SetSoundLocation(unsigned int ID, float x, float y, float z)
{
	if (!ID)
		return;

	auto tmpSound = SoundsMap.find(ID);
	if (tmpSound != SoundsMap.end())
		tmpSound->second.SetLocation(x, y, z);
}

void vw_StopSound(unsigned int ID, float StopDelay)
{
	if (!ID)
		return;

	auto tmpSound = SoundsMap.find(ID);
	if (tmpSound != SoundsMap.end())
		tmpSound->second.Stop(StopDelay);
}
