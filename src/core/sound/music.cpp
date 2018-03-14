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

#include "../vfs/vfs.h"
#include "../system/system.h"
#include "sound.h"


ALboolean CheckALError(const char *FunctionName);
ALboolean CheckALUTError(const char *FunctionName);


//------------------------------------------------------------------------------------
// Проигрывание звука
//------------------------------------------------------------------------------------
bool sMusic::Play(const std::string &Name, float fVol, float fMainVol, bool Loop, const std::string &LoopFileName)
{
	if (Name.empty())
		return false;

	Volume = fVol;
	MainVolume = fMainVol;

	LoopPart = LoopFileName;

	FadeInSw = false;
	FadeInStartVol = Volume;
	FadeInEndVol = Volume;
	FadeOutSw = false;
	FadeTime = 0.0f;
	FadeAge = 0.0f;
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

	for (int i = 0; i < NUM_OF_DYNBUF; i++) {
		alSourceQueueBuffers(Source, 1, Stream->Buffers.data() + i);
		if (!CheckALError(__func__))
			return false;
	}

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
