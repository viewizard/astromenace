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
#include "sound.h"

cSound *StartSoundMan = nullptr;
cSound *EndSoundMan = nullptr;
int NumSoundMan = 0;

ALboolean CheckALError(const char *FunctionName);
ALboolean CheckALUTError(const char *FunctionName);


//------------------------------------------------------------------------------------
// Проигрывание звука
//------------------------------------------------------------------------------------
bool cSound::Play(const char *Name, float _LocalVolume, float _GlobalVolume, float x, float y, float z,
		  bool Relative, bool Loop, bool NeedReleaseStatus, int AtType)
{
	Source = 0;
	LastUpdateTime = vw_GetTimeThread(0);
	FileName = new char[strlen(Name)+1];
	strcpy(FileName,Name);


	DestroyTime = -1.0f;
	DestroyTimeStart = -1.0f;

	NeedRelease = NeedReleaseStatus;

	ALuint Buffer{0};
	// проверяем, вообще есть расширение или нет, плюс, получаем указатель на последнюю точку
	const char *file_ext = strrchr(Name, '.');
	if (file_ext) {
		if (!strcasecmp(".wav", file_ext))
			Buffer = vw_CreateSoundBufferFromWAV(Name);
		else if (!strcasecmp(".ogg", file_ext))
			Buffer = vw_CreateSoundBufferFromOGG(Name);
	}

	if (!Buffer)
		return false;

	LocalVolume = _LocalVolume;
	GlobalVolume = _GlobalVolume;

	// Position of the source sound.
	ALfloat SourcePos[] = {x, y, z}; // -1.0 1.0 по иксам это баланс
	// Velocity of the source sound.
	ALfloat SourceVel[] = {0.0f, 0.0f, 0.0f};

	// Bind the buffer with the source.
	alGenSources(1, &Source);
	if(!CheckALError(__func__))
		return false;

	alSourcei (Source, AL_BUFFER, Buffer);

	alSourcef (Source, AL_GAIN, _GlobalVolume * _LocalVolume);
	alSourcefv(Source, AL_POSITION, SourcePos);
	alSourcefv(Source, AL_VELOCITY, SourceVel);
	alSourcei (Source, AL_LOOPING, Loop);
	alSourcei (Source, AL_SOURCE_RELATIVE, Relative);
	alGetError(); // сброс ошибок

	// параметры затухания сигнала от источника
	if (AtType == 1) {
		alSourcef(Source, AL_REFERENCE_DISTANCE, 30.0f);
		alSourcef(Source, AL_MAX_DISTANCE, 250.0f);
		alSourcef(Source, AL_ROLLOFF_FACTOR, 0.5f);
		alGetError(); // сброс ошибок
	} else if (AtType == 2) {
		alSourcef(Source, AL_REFERENCE_DISTANCE, 150.0f);
		alSourcef(Source, AL_MAX_DISTANCE, 600.0f);
		alSourcef(Source, AL_ROLLOFF_FACTOR, 0.2f);
		alGetError(); // сброс ошибок
	}

	alSourcePlay(Source);
	if(!CheckALError(__func__))
		return false;

	return true;
}

//------------------------------------------------------------------------------------
// перезапуск
//------------------------------------------------------------------------------------
void cSound::Replay()
{
	if (!alIsSource(Source))
		return;

	alSourceRewind(Source);
	alSourcePlay(Source);
}


//------------------------------------------------------------------------------------
// остановка
//------------------------------------------------------------------------------------
void cSound::Stop(float StopDelay)
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
void cSound::SetLocation(float x, float y, float z)
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
void cSound::SetGlobalVolume(float NewGlobalVolume)
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
void vw_ReleaseSound(cSound* Sound)
{
	// проверка входящих данных
	if (!Sound)
		return;

	// отключаем от менерджера
	vw_DetachSound(Sound);

	if (alIsSource(Sound->Source)) {
		// если останавливать играющий звук, возможен щелчек (и в линуксе и в виндовсе)
		alSourceStop(Sound->Source);
		alGetError(); // сброс ошибок

		// освобождаем собственно сам источник
		alDeleteSources(1, &Sound->Source);
		Sound->Source = 0;
		alGetError(); // сброс ошибок
	}

	// освобождаем память
	if (Sound)
		delete Sound;
}

//------------------------------------------------------------------------------------
// освобождаем все подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllSounds(int ReleaseType)
{
	if (ReleaseType == 0) {
		// Чистка памяти...
		cSound *Tmp = StartSoundMan;
		while (Tmp) {
			cSound *Tmp1 = Tmp->Next;
			vw_ReleaseSound(Tmp);
			Tmp = Tmp1;
		}

		StartSoundMan = nullptr;
		EndSoundMan = nullptr;
	} else {
		cSound *Tmp = StartSoundMan;
		while (Tmp) {
			cSound *Tmp1 = Tmp->Next;
			if (Tmp->NeedRelease) vw_ReleaseSound(Tmp);
			Tmp = Tmp1;
		}

	}
}

//------------------------------------------------------------------------------------
// подключение звука к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachSound(cSound *Sound)
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
void vw_DetachSound(cSound *Sound)
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


//------------------------------------------------------------------------------------
// Нахождение по уникальному номеру
//------------------------------------------------------------------------------------
cSound *vw_FindSoundByNum(int Num)
{
	cSound *Tmp = StartSoundMan;
	while (Tmp) {
		cSound *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num)
			return Tmp;
		Tmp = Tmp1;
	}
	return nullptr;
}


//------------------------------------------------------------------------------------
// Нахождение по имени
//------------------------------------------------------------------------------------
cSound *vw_FindSoundByName(const char *Name)
{
	if (!Name)
		return nullptr;

	cSound *Tmp = StartSoundMan;
	while (Tmp) {
		cSound *Tmp1 = Tmp->Next;
		if ((Tmp->FileName != nullptr) &&
		    (!strcmp(Tmp->FileName, Name)))
			return Tmp;
		Tmp = Tmp1;
	}
	return nullptr;
}

//------------------------------------------------------------------------------------
// Обновляем данные в менеджере
//------------------------------------------------------------------------------------
void vw_UpdateSound()
{
	cSound *Tmp = StartSoundMan;

	float CurrentGetTime = vw_GetTimeThread(0);

	while (Tmp) {
		cSound *Tmp1 = Tmp->Next;

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

//------------------------------------------------------------------------------------
// Установка громкости всем SFX
//------------------------------------------------------------------------------------
void vw_SetSoundGlobalVolume(float NewGlobalVolume)
{
	cSound *Tmp = StartSoundMan;
	while (Tmp) {
		cSound *Tmp1 = Tmp->Next;
		Tmp->SetGlobalVolume(NewGlobalVolume);
		Tmp = Tmp1;
	}
}
