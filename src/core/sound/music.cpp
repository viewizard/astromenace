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
#include "sound.h"

sMusic *StartMusicMan = nullptr;
sMusic *EndMusicMan = nullptr;
int NumMusicMan = 0;

ALboolean CheckALError(const char *FunctionName);
ALboolean CheckALUTError(const char *FunctionName);


//------------------------------------------------------------------------------------
// Проигрывание звука
//------------------------------------------------------------------------------------
bool sMusic::Play(const std::string &Name, float fVol, float fMainVol, bool Loop, const std::string &LoopFileName)
{
	if (Name.empty()) // LoopFileName could be empty
		return false;

	Volume = fVol;
	MainVolume = fMainVol;
	LoopPart = LoopFileName;
	FadeInStartVol = Volume;
	FadeInEndVol = Volume;
	LastTime = vw_GetTimeThread(0);

	// Position of the source sound.
	ALfloat SourcePos[] = {0.0f, 0.0f, 0.0f}; // -1.0 1.0 по иксам это баланс
	// Velocity of the source sound.
	ALfloat SourceVel[] = {0.0f, 0.0f, 0.0f};

	alGenSources(1, &Source);
	if(!CheckALError(__func__))
		return false;

	alSourcef (Source, AL_PITCH, 1.0); // 1.0 only!!!
	alSourcef (Source, AL_GAIN, fVol*fMainVol); // фактически громкость
	alSourcefv(Source, AL_POSITION, SourcePos);
	alSourcefv(Source, AL_VELOCITY, SourceVel);
	alSourcei (Source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcei(Source, AL_LOOPING, AL_FALSE);
	alGetError(); // сброс ошибок
	Looped = Loop;

	Stream = vw_CreateStreamBufferFromOGG(Name, LoopPart);
	if (!Stream)
		return false;

	if (!vw_QueueStreamBuffer(Stream, Source))
		return false;

	alSourcePlay(Source);
	if(!CheckALError(__func__))
		return false;

	return true;
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
	ALint TMPS;
	alGetSourcei(Source, AL_SOURCE_STATE, &TMPS);
	alGetError(); // сброс ошибок
	if (TMPS == AL_STOPPED)
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
	alSourcef(Source, AL_GAIN,     MainVolume*Volume );
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

//------------------------------------------------------------------------------------
// Возвращаем, если играется хотя бы одна музыка
//------------------------------------------------------------------------------------
bool vw_IsMusicPlaying()
{
	sMusic *Tmp = StartMusicMan;
	while (Tmp) {
		sMusic *Tmp1 = Tmp->Next;
		// смотрим, если играем что-то, передаем...
		if (alIsSource(Tmp->Source)) {
			ALint TMPS;
			alGetSourcei(Tmp->Source, AL_SOURCE_STATE, &TMPS);
			alGetError(); // сброс ошибок
			if (TMPS == AL_PLAYING)
				return true;
		}
		Tmp = Tmp1;
	}

	return false;
}

//------------------------------------------------------------------------------------
// Освобождение памяти и удаление
//------------------------------------------------------------------------------------
void vw_ReleaseMusic(sMusic *Music)
{
	// проверка входящих данных
	if (!Music)
		return;

	// отключаем от менерджера
	vw_DetachMusic(Music);

	// освобождаем память
	if (Music)
		delete Music;
}

//------------------------------------------------------------------------------------
// освобождаем все подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllMusic()
{
	// Чистка памяти...
	sMusic *Tmp = StartMusicMan;
	while (Tmp) {
		sMusic *Tmp1 = Tmp->Next;
		delete Tmp;
		Tmp = Tmp1;
	}

	StartMusicMan = nullptr;
	EndMusicMan = nullptr;
}

//------------------------------------------------------------------------------------
// подключение звука к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachMusic(sMusic *Music)
{
	if (!Music)
		return;

	// первый в списке...
	if (EndMusicMan == nullptr) {
		Music->Prev = nullptr;
		Music->Next = nullptr;
		NumMusicMan += 1;
		Music->Num = NumMusicMan;
		StartMusicMan = Music;
		EndMusicMan = Music;
	} else { // продолжаем заполнение...
		Music->Prev = EndMusicMan;
		Music->Next = nullptr;
		EndMusicMan->Next = Music;
		NumMusicMan += 1;
		Music->Num = NumMusicMan;
		EndMusicMan = Music;
	}
}


//------------------------------------------------------------------------------------
// отключение от менеджера
//------------------------------------------------------------------------------------
void vw_DetachMusic(sMusic *Music)
{
	if (!Music)
		return;

	// переустанавливаем указатели...
	if (StartMusicMan == Music)
		StartMusicMan = Music->Next;
	if (EndMusicMan == Music)
		EndMusicMan = Music->Prev;

	if (Music->Next != nullptr)
		Music->Next->Prev = Music->Prev;
	else if (Music->Prev != nullptr)
		Music->Prev->Next = nullptr;

	if (Music->Prev != nullptr)
		Music->Prev->Next = Music->Next;
	else if (Music->Next != nullptr)
		Music->Next->Prev = nullptr;
}

//------------------------------------------------------------------------------------
// Обновляем данные в менеджере
//------------------------------------------------------------------------------------
void vw_UpdateMusic()
{
	sMusic *Tmp = StartMusicMan;
	while (Tmp) {
		sMusic *Tmp1 = Tmp->Next;
		if (!Tmp->Update())
			vw_ReleaseMusic(Tmp);
		Tmp = Tmp1;
	}
}

//------------------------------------------------------------------------------------
// Установка громкости у всех
//------------------------------------------------------------------------------------
void vw_SetMusicMainVolume(float NewMainVolume)
{
	sMusic *Tmp = StartMusicMan;
	while (Tmp) {
		sMusic *Tmp1 = Tmp->Next;
		Tmp->SetMainVolume(NewMainVolume);
		Tmp = Tmp1;
	}
}

//------------------------------------------------------------------------------------
// Нахождение по уникальному номеру...
//------------------------------------------------------------------------------------
sMusic* vw_FindMusicByNum(int Num)
{
	sMusic *Tmp = StartMusicMan;
	while (Tmp) {
		sMusic *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num)
			return Tmp;
		Tmp = Tmp1;
	}
	return nullptr;
}
