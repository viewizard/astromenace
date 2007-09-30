/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Sound.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "Sound.h"
#include "Buffer.h"
#include "../System/System.h"


ALboolean CheckALError();
ALboolean CheckALUTError();





//------------------------------------------------------------------------------------
// Проигрывание звука
//------------------------------------------------------------------------------------
bool eSound::Play(const char *Name, float fVol, float fMainVol, float x, float y, float z, bool Relative, bool Loop, bool NeedReleaseStatus, int AtType)
{
	Source = 0;
	Age = 0.0f;
	LastUpdateTime = vw_GetTime();
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
	if (Buffer == 0)
	{
		if( vw_TestFileExtension( Name, "wav" ) || vw_TestFileExtension( Name, "WAV" ))
		{
			Buffer = vw_CreateSoundBufferFromWAV(Name);
		}
		else
		{
			if( vw_TestFileExtension( Name, "ogg" ) || vw_TestFileExtension( Name, "OGG" ))
				Buffer = vw_CreateSoundBufferFromOGG(Name);
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
	if (AtType == 1)
	{
		alSourcef (Source, AL_REFERENCE_DISTANCE,  30.0f     );
		alSourcef (Source, AL_MAX_DISTANCE,  250.0f     );
		alSourcef (Source, AL_ROLLOFF_FACTOR,  0.5f     );
		alGetError(); // сброс ошибок
	}
	else
	if (AtType == 2)
	{
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
void eSound::Replay()
{
	// если это не источник, уходим
	if (!alIsSource(Source)) return;

	alSourceRewind(Source);
	alSourcePlay(Source);
}




//------------------------------------------------------------------------------------
// остановка
//------------------------------------------------------------------------------------
void eSound::Stop(float StopDelay)
{
	// если это не источник, уходим
	if (!alIsSource(Source)) return;

	if (StopDelay < 0.0f) StopDelay = 0.0f;

	if (StopDelay == 0.0f)
	{
		alSourceStop(Source);
		alGetError(); // сброс ошибок
	}
	else
	{
		DestroyTimeStart = DestroyTime = StopDelay;
	}
}




//------------------------------------------------------------------------------------
// для 3д звука установка положения (баланса)
//------------------------------------------------------------------------------------
void eSound::SetLocation(float x, float y, float z)
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
void eSound::SetMainVolume(float NewMainVolume)
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
void eSound::SetInfo(int NewGroup, int NewGroupCount, int NewSubGroup, int NewSubGroupCount, int NewPriority)
{
	Group = NewGroup;
	GroupCount = NewGroupCount;
	SubGroup = NewSubGroup;
	SubGroupCount = NewSubGroupCount;
	Priority = NewPriority;
}
