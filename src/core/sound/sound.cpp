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

#include "../system/system.h"
#include "../math/math.h"
#include "buffer.h"

namespace {

struct sSound
{
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

	sSound *Prev{nullptr};	// Pointer to the previous loaded Sound in the memory
	sSound *Next{nullptr};	// Pointer to the next loaded Sound in the memory
	int Num{0};		// ID number
};

} // unnamed namespace

sSound *StartSoundMan = nullptr;
sSound *EndSoundMan = nullptr;
int NumSoundMan = 0;
void vw_ReleaseSound(sSound *Sound);
void vw_AttachSound(sSound *Sound);
void vw_DetachSound(sSound *Sound);

ALboolean CheckALError(const char *FunctionName);
ALboolean CheckALUTError(const char *FunctionName);


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
			  float x, float y, float z, bool Relative, bool Loop, bool AllowStop, int AtType)
{
	sSound *Sound = new sSound;
	vw_AttachSound(Sound);

	Sound->Source = 0;
	Sound->LastUpdateTime = vw_GetTimeThread(0);
	Sound->FileName = Name;

	Sound->DestroyTime = -1.0f;
	Sound->DestroyTimeStart = -1.0f;

	Sound->AllowedStop = AllowStop;

	ALuint Buffer = vw_LoadSoundBuffer(Name);
	if (!Buffer) {
		vw_ReleaseSound(Sound);
		return 0;
	}

	Sound->LocalVolume = _LocalVolume;
	Sound->GlobalVolume = _GlobalVolume;

	// Position of the source sound.
	ALfloat SourcePos[] = {x, y, z}; // -1.0 1.0 по иксам это баланс
	// Velocity of the source sound.
	ALfloat SourceVel[] = {0.0f, 0.0f, 0.0f};

	// Bind the buffer with the source.
	alGenSources(1, &Sound->Source);
	if(!CheckALError(__func__)) {
		vw_ReleaseSound(Sound);
		return 0;
	}

	alSourcei(Sound->Source, AL_BUFFER, Buffer);

	alSourcef(Sound->Source, AL_GAIN, _GlobalVolume * _LocalVolume);
	alSourcefv(Sound->Source, AL_POSITION, SourcePos);
	alSourcefv(Sound->Source, AL_VELOCITY, SourceVel);
	alSourcei(Sound->Source, AL_LOOPING, Loop);
	alSourcei(Sound->Source, AL_SOURCE_RELATIVE, Relative);
	alGetError(); // сброс ошибок

	// параметры затухания сигнала от источника
	if (AtType == 1) {
		alSourcef(Sound->Source, AL_REFERENCE_DISTANCE, 30.0f);
		alSourcef(Sound->Source, AL_MAX_DISTANCE, 250.0f);
		alSourcef(Sound->Source, AL_ROLLOFF_FACTOR, 0.5f);
		alGetError(); // сброс ошибок
	} else if (AtType == 2) {
		alSourcef(Sound->Source, AL_REFERENCE_DISTANCE, 150.0f);
		alSourcef(Sound->Source, AL_MAX_DISTANCE, 600.0f);
		alSourcef(Sound->Source, AL_ROLLOFF_FACTOR, 0.2f);
		alGetError(); // сброс ошибок
	}

	alSourcePlay(Sound->Source);
	if(!CheckALError(__func__)) {
		vw_ReleaseSound(Sound);
		return 0;
	}

	return Sound->Num;
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
	alSourcef(Source, AL_GAIN, GlobalVolume * LocalVolume );
	alGetError(); // сброс ошибок
}

//------------------------------------------------------------------------------------
// Освобождение памяти и удаление
//------------------------------------------------------------------------------------
void vw_ReleaseSound(sSound* Sound)
{
	// проверка входящих данных
	if (!Sound)
		return;

	// отключаем от менерджера
	vw_DetachSound(Sound);

	// освобождаем память
	if (Sound)
		delete Sound;
}

//------------------------------------------------------------------------------------
// освобождаем все подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllSounds()
{
	// Чистка памяти...
	sSound *Tmp = StartSoundMan;
	while (Tmp) {
		sSound *Tmp1 = Tmp->Next;
		delete Tmp;
		Tmp = Tmp1;
	}

	StartSoundMan = nullptr;
	EndSoundMan = nullptr;
}


void vw_StopAllSoundsIfAllowed()
{
	sSound *Tmp = StartSoundMan;
	while (Tmp) {
		sSound *Tmp1 = Tmp->Next;
		if (Tmp->AllowedStop)
			// stopped sound will be released on update() call,
			// so, no reason stop sound, release it right now
			vw_ReleaseSound(Tmp);
		Tmp = Tmp1;
	}
}

//------------------------------------------------------------------------------------
// подключение звука к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachSound(sSound *Sound)
{
	if (!Sound)
		return;

	// первый в списке...
	if (EndSoundMan == nullptr) {
		Sound->Prev = nullptr;
		Sound->Next = nullptr;
		NumSoundMan += 1;
		Sound->Num = NumSoundMan;
		StartSoundMan = Sound;
		EndSoundMan = Sound;
	} else { // продолжаем заполнение...
		Sound->Prev = EndSoundMan;
		Sound->Next = nullptr;
		EndSoundMan->Next = Sound;
		NumSoundMan += 1;
		Sound->Num = NumSoundMan;
		EndSoundMan = Sound;
	}
}


//------------------------------------------------------------------------------------
// отключение от менеджера
//------------------------------------------------------------------------------------
void vw_DetachSound(sSound *Sound)
{
	if (!Sound)
		return;

	// переустанавливаем указатели...
	if (StartSoundMan == Sound)
		StartSoundMan = Sound->Next;
	if (EndSoundMan == Sound)
		EndSoundMan = Sound->Prev;

	if (Sound->Next != nullptr)
		Sound->Next->Prev = Sound->Prev;
	else if (Sound->Prev != nullptr)
		Sound->Prev->Next = nullptr;

	if (Sound->Prev != nullptr)
		Sound->Prev->Next = Sound->Next;
	else if (Sound->Next != nullptr)
		Sound->Next->Prev = nullptr;
}

bool vw_IsSoundAvailable(int Num)
{
	if (!Num)
		return false;

	sSound *Tmp = StartSoundMan;
	while (Tmp) {
		sSound *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num)
			return true;
		Tmp = Tmp1;
	}
	return false;
}

/*
 * Replay from the beginning first sound, found by name.
 */
int vw_ReplayFirstFoundSound(const std::string &Name)
{
	if (Name.empty())
		return 0;

	sSound *Tmp = StartSoundMan;
	while (Tmp) {
		sSound *Tmp1 = Tmp->Next;
		if (Tmp->FileName == Name) {
			Tmp->Replay();
			return Tmp->Num;
		}
		Tmp = Tmp1;
	}

	return 0;
}

//------------------------------------------------------------------------------------
// Обновляем данные в менеджере
//------------------------------------------------------------------------------------
void vw_UpdateSound()
{
	sSound *Tmp = StartSoundMan;

	float CurrentGetTime = vw_GetTimeThread(0);

	while (Tmp) {
		sSound *Tmp1 = Tmp->Next;

		// считаем сколько играем этот эффект
		float DeltaTime = CurrentGetTime - Tmp->LastUpdateTime;
		Tmp->LastUpdateTime = CurrentGetTime;

		if ((alIsSource(Tmp->Source)) &&
		    (Tmp->DestroyTimeStart > 0.0f)) {
			if (Tmp->DestroyTime > 0.0f) {
				Tmp->DestroyTime -= DeltaTime;
				if (Tmp->DestroyTime < 0.0f)
					Tmp->DestroyTime = 0.0f;
				alSourcef (Tmp->Source, AL_GAIN, Tmp->GlobalVolume * Tmp->LocalVolume * (Tmp->DestroyTime / Tmp->DestroyTimeStart));
				alGetError(); // сброс ошибок
			} else {
				// уже нулевая громкость, можем удалять
				vw_ReleaseSound(Tmp);
				Tmp = nullptr;
			}
		}

		// смотрим, если уже не играем - надо удалить
		if (Tmp && (alIsSource(Tmp->Source))) {
			ALint TMPS;
			alGetSourcei(Tmp->Source, AL_SOURCE_STATE, &TMPS);
			alGetError(); // сброс ошибок

			if (TMPS == AL_STOPPED)
				vw_ReleaseSound(Tmp);
		}

		Tmp = Tmp1;
	}
}

void vw_SetSoundGlobalVolume(const std::string &Name, float NewGlobalVolume)
{
	sSound *Tmp = StartSoundMan;
	while (Tmp) {
		sSound *Tmp1 = Tmp->Next;
		if (Tmp->FileName == Name) {
			Tmp->SetGlobalVolume(NewGlobalVolume);
			break;
		}
		Tmp = Tmp1;
	}
}

void vw_SetSoundLocation(int Num, float x, float y, float z)
{
	sSound *Tmp = StartSoundMan;
	while (Tmp) {
		sSound *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num) {
			Tmp->SetLocation(x, y, z);
			break;
		}
		Tmp = Tmp1;
	}
}

void vw_StopSound(int Num, float StopDelay)
{
	sSound *Tmp = StartSoundMan;
	while (Tmp) {
		sSound *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num) {
			Tmp->Stop(StopDelay);
			break;
		}
		Tmp = Tmp1;
	}
}
