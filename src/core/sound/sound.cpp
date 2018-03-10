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

#include "../system/system.h"
#include "sound.h"


ALboolean CheckALError();
ALboolean CheckALUTError();





//------------------------------------------------------------------------------------
// Проигрывание звука
//------------------------------------------------------------------------------------
bool cSound::Play(const char *Name, float fVol, float fMainVol, float x, float y, float z, bool Relative, bool Loop, bool NeedReleaseStatus, int AtType)
{
	Source = 0;
	Age = 0.0f;
	LastUpdateTime = vw_GetTimeThread(0);
	Group = 0;
	GroupCount = 0;
	SubGroup = 0;
	SubGroupCount = 0;
	Priority = 0;
	FileName = new char[strlen(Name)+1];
	strcpy(FileName,Name);


	DestroyTime = -1.0f;
	DestroyTimeStart = -1.0f;

	NeedRelease = NeedReleaseStatus;


	// находим существующий или делаем новый буфер
	ALuint Buffer = vw_FindBufferIDByName(Name);
	// у нас еще нет такого буфера, его надо попытаться создать
	if (Buffer == 0) {
		// проверяем, вообще есть расширение или нет, плюс, получаем указатель на последнюю точку
		const char *file_ext = strrchr(Name, '.');
		if (file_ext) {
			if (!strcasecmp(".wav", file_ext)) Buffer = vw_CreateSoundBufferFromWAV(Name);
			else if (!strcasecmp(".ogg", file_ext)) Buffer = vw_CreateSoundBufferFromOGG(Name);

		}
	}

	if (Buffer == 0) return AL_FALSE;

	Volume = fVol;
	MainVolume = fMainVol;

	// Position of the source sound.
	ALfloat SourcePos[] = { x, y, z }; // -1.0 1.0 по иксам это баланс
	// Velocity of the source sound.
	ALfloat SourceVel[] = { 0.0f, 0.0f, 0.0f };

	// Bind the buffer with the source.
	alGenSources(1, &Source);
	if(!CheckALError())return AL_FALSE;

	alSourcei (Source, AL_BUFFER,   Buffer   );

	alSourcef (Source, AL_GAIN,     fVol*fMainVol	 ); // фактически громкость
	alSourcefv(Source, AL_POSITION, SourcePos);
	alSourcefv(Source, AL_VELOCITY, SourceVel);
	alSourcei (Source, AL_LOOPING,  Loop     );
	alSourcei (Source, AL_SOURCE_RELATIVE, Relative);
	alGetError(); // сброс ошибок



	// параметры затухания сигнала от источника
	if (AtType == 1) {
		alSourcef (Source, AL_REFERENCE_DISTANCE,  30.0f     );
		alSourcef (Source, AL_MAX_DISTANCE,  250.0f     );
		alSourcef (Source, AL_ROLLOFF_FACTOR,  0.5f     );
		alGetError(); // сброс ошибок
	} else if (AtType == 2) {
		alSourcef (Source, AL_REFERENCE_DISTANCE,  150.0f     );
		alSourcef (Source, AL_MAX_DISTANCE,  600.0f     );
		alSourcef (Source, AL_ROLLOFF_FACTOR,  0.2f     );
		alGetError(); // сброс ошибок
	}



	alSourcePlay(Source);
	if(!CheckALError()) return AL_FALSE;

	return AL_TRUE;
}



//------------------------------------------------------------------------------------
// перезапуск
//------------------------------------------------------------------------------------
void cSound::Replay()
{
	// если это не источник, уходим
	if (!alIsSource(Source)) return;

	alSourceRewind(Source);
	alSourcePlay(Source);
}




//------------------------------------------------------------------------------------
// остановка
//------------------------------------------------------------------------------------
void cSound::Stop(float StopDelay)
{
	// если это не источник, уходим
	if (!alIsSource(Source)) return;

	if (StopDelay < 0.0f) StopDelay = 0.0f;

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
	if (!alIsSource(Source)) return;

	ALfloat SourcePos[] = { x, y, z };
	alSourcefv(Source, AL_POSITION, SourcePos);
	alGetError(); // сброс ошибок
}



//------------------------------------------------------------------------------------
// установка громкости
//------------------------------------------------------------------------------------
void cSound::SetMainVolume(float NewMainVolume)
{
	// если это не источник, уходим
	if (!alIsSource(Source)) return;
	MainVolume = NewMainVolume;
	alSourcef (Source, AL_GAIN,     MainVolume*Volume );
	alGetError(); // сброс ошибок
}



//------------------------------------------------------------------------------------
// установка информации о звуке
//------------------------------------------------------------------------------------
void cSound::SetInfo(int NewGroup, int NewGroupCount, int NewSubGroup, int NewSubGroupCount, int NewPriority)
{
	Group = NewGroup;
	GroupCount = NewGroupCount;
	SubGroup = NewSubGroup;
	SubGroupCount = NewSubGroupCount;
	Priority = NewPriority;
}
