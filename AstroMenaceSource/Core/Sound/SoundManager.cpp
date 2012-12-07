/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


#include "Sound.h"
#include "Buffer.h"
#include "../System/System.h"




//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------
eSound *StartSoundMan = 0;
eSound *EndSoundMan = 0;
int NumSoundMan = 0;

eMusic *StartMusicMan = 0;
eMusic *EndMusicMan = 0;
int NumMusicMan = 0;







//------------------------------------------------------------------------------------
// Получение ошибок
//------------------------------------------------------------------------------------
ALboolean CheckALCError(ALCdevice *Device)
{
	ALenum ErrCode;
	char Err[1024];
	strcpy(Err, "ALC error: ");

	if ((ErrCode = alcGetError(Device)) != ALC_NO_ERROR)
	{
		strcat(Err, alcGetString(Device, ErrCode));
		strcat(Err, "\n");
		fprintf(stderr, "%s", Err);
		return AL_FALSE;
	}
	return AL_TRUE;
}
ALboolean CheckALError()
{
	ALenum ErrCode;
	char Err[1024];
	strcpy(Err, "OpenAL error: ");

	if ((ErrCode = alGetError()) != AL_NO_ERROR)
	{
		strcat(Err, alGetString(ErrCode));
		strcat(Err, "\n");
		fprintf(stderr, "%s", Err);
		return AL_FALSE;
	}
	return AL_TRUE;
}
ALboolean CheckALUTError()
{
	ALenum ErrCode;
	char Err[1024];
	strcpy(Err, "OpenAL alut error: ");

	if ((ErrCode = alutGetError()) != ALUT_ERROR_NO_ERROR)
	{
		strcat(Err, alutGetErrorString(ErrCode));
		strcat(Err, "\n");
		fprintf(stderr, "%s", Err);
		return AL_FALSE;
	}
	return AL_TRUE;
}


//------------------------------------------------------------------------------------
// Инициализация звука
//------------------------------------------------------------------------------------
bool vw_InitSound()
{

	// Position of the Listener.
	ALfloat ListenerPos[] = { 0.0f, 0.0f, 0.0f };

	// Velocity of the Listener.
	ALfloat ListenerVel[] = { 0.0f, 0.0f, 0.0f };

	// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
	// Also note that these should be units of '1'.
	ALfloat ListenerOri[] = { 0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f };


	alutInitWithoutContext(NULL, NULL);
	if (!CheckALUTError()) return false;

	ALCcontext *Context = 0;
	ALCdevice *Device = 0;

	// Open default sound device
	Device = alcOpenDevice(NULL); // "Generic Software"
	// Check for errors
	if (!Device)
	{
		fprintf(stderr, "Default sound device not present!\n");
		return false;
	}

	// Creating rendering context
	Context = alcCreateContext(Device, NULL);
	if (!CheckALCError(Device)) return false;

	alcMakeContextCurrent(Context);
	if (!CheckALCError(Device)) return false;

	// сброс ошибок
	alGetError();
	alcGetError(Device);

	// Set listener properties
    alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
    // затухание звука
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);


	printf("Vendor     : %s\n", alGetString(AL_VENDOR));
	printf("Renderer   : %s\n", alGetString(AL_RENDERER));
	printf("Version    : %s\n", alGetString(AL_VERSION));
	printf("ALut ver   : %i.%i\n", alutGetMajorVersion(), alutGetMinorVersion());

#ifdef gamedebug
	// получаем и выводим все поддерживаемые расширения
	char *extensions_tmp;
	size_t len;
	extensions_tmp = (char *)alGetString(AL_EXTENSIONS);
	if (extensions_tmp != 0)
	{
		char *extensions = 0;
		len = strlen(extensions_tmp);
		extensions = new char[len+1];
		if (extensions != 0)
		{
			strcpy(extensions, extensions_tmp);
			for (unsigned int i=0; i<len; i++) // меняем разделитель
				if (extensions[i]==' ') extensions[i]='\n';

			printf("Supported OpenAL extensions:\n%s\n", extensions);
			delete [] extensions;
		}
	}
#endif // gamedebug

	printf("\n");

	// сброс ошибок

	alGetError();
	alcGetError(Device);

	return true;
}









//------------------------------------------------------------------------------------
// Полностью заканчиваем работу
//------------------------------------------------------------------------------------
void vw_ShutdownSound()
{
	// освобождаем все источники звука
	vw_ReleaseAllSounds(0);
	vw_ReleaseAllMusic();
	// убираем все буферы
	vw_ReleaseAllBuffers();



	ALCcontext *Context = 0;
	ALCdevice *Device = 0;

	//Get active context
	Context = alcGetCurrentContext();
	//Get device for active context
	Device = alcGetContextsDevice(Context);


	// Если можем убрать Context, чтобы он не был текущим
	if (alcMakeContextCurrent(NULL))
	{
		alcGetError(Device);

		// Destroy context
		if (Context != 0)
		if (alcGetCurrentContext() != Context) alcDestroyContext(Context);
		alcGetError(Device);

		// Close sound device
		if (Device != 0) alcCloseDevice(Device);
	}


	alutExit();
	CheckALUTError();
}


//------------------------------------------------------------------------------------
// установка параметров слушателя
//------------------------------------------------------------------------------------
void vw_Listener(float ListenerPos[3], float ListenerVel[3], float ListenerOri[6])
{
    alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
    alGetError(); // сброс ошибок
}









//------------------------------------------------------------------------------------
// Освобождение памяти и удаление
//------------------------------------------------------------------------------------
void vw_ReleaseSound(eSound* Sound)
{
	// проверка входящих данных
	if (Sound == 0) return;

	// отключаем от менерджера
	vw_DetachSound(Sound);

	if (alIsSource(Sound->Source))
	{
		// если останавливать играющий звук, возможен щелчек (и в линуксе и в виндовсе)
		alSourceStop(Sound->Source);
		alGetError(); // сброс ошибок

		// освобождаем собственно сам источник
		alDeleteSources(1, &Sound->Source);
		Sound->Source = 0;
		alGetError(); // сброс ошибок
	}


	// освобождаем память
	if (Sound != 0){delete Sound; Sound = 0;}
}





//------------------------------------------------------------------------------------
// освобождаем все подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllSounds(int ReleaseType)
{
	if (ReleaseType == 0)
	{
		// Чистка памяти...
		eSound *Tmp = StartSoundMan;
		while (Tmp != 0)
		{
			eSound *Tmp1 = Tmp->Next;
			vw_ReleaseSound(Tmp);
			Tmp = Tmp1;
		}

		StartSoundMan = 0;
		EndSoundMan = 0;
	}
	else
	{

		eSound *Tmp = StartSoundMan;
		while (Tmp != 0)
		{
			eSound *Tmp1 = Tmp->Next;
			if (Tmp->NeedRelease) vw_ReleaseSound(Tmp);
			Tmp = Tmp1;
		}

	}
}


//------------------------------------------------------------------------------------
// подключение звука к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachSound(eSound* Sound)
{
	if (Sound == 0) return;

	// первый в списке...
	if (EndSoundMan == 0)
	{
		Sound->Prev = 0;
		Sound->Next = 0;
		NumSoundMan += 1;
		Sound->Num = NumSoundMan;
		StartSoundMan = Sound;
		EndSoundMan = Sound;
	}
	else // продолжаем заполнение...
	{
		Sound->Prev = EndSoundMan;
		Sound->Next = 0;
		EndSoundMan->Next = Sound;
		NumSoundMan += 1;
		Sound->Num = NumSoundMan;
		EndSoundMan = Sound;
	}
}


//------------------------------------------------------------------------------------
// отключение от менеджера
//------------------------------------------------------------------------------------
void vw_DetachSound(eSound* Sound)
{
	if (Sound == 0) return;

	// переустанавливаем указатели...
	if (StartSoundMan == Sound) StartSoundMan = Sound->Next;
	if (EndSoundMan == Sound) EndSoundMan = Sound->Prev;

	if (Sound->Next != 0) Sound->Next->Prev = Sound->Prev;
		else if (Sound->Prev != 0) Sound->Prev->Next = 0;
	if (Sound->Prev != 0) Sound->Prev->Next = Sound->Next;
		else if (Sound->Next != 0) Sound->Next->Prev = 0;
}


//------------------------------------------------------------------------------------
// Нахождение по уникальному номеру
//------------------------------------------------------------------------------------
eSound* vw_FindSoundByNum(int Num)
{
	eSound *Tmp = StartSoundMan;
	while (Tmp != 0)
	{
		eSound *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num) return Tmp;
		Tmp = Tmp1;
	}
	return 0;
}


//------------------------------------------------------------------------------------
// Нахождение по имени
//------------------------------------------------------------------------------------
eSound* vw_FindSoundByName(const char *Name)
{
	if (Name == 0) return 0;

	eSound *Tmp = StartSoundMan;
	while (Tmp != 0)
	{
		eSound *Tmp1 = Tmp->Next;
		if (Tmp->FileName != 0)
			if (!strcmp(Tmp->FileName, Name)) return Tmp;
		Tmp = Tmp1;
	}
	return 0;
}

//------------------------------------------------------------------------------------
// Обновляем данные в менеджере
//------------------------------------------------------------------------------------
void vw_UpdateSound()
{
	eSound *Tmp = StartSoundMan;

	float CurrentGetTime = vw_GetTime();

	while (Tmp != 0)
	{
		eSound *Tmp1 = Tmp->Next;

		// считаем сколько играем этот эффект
		float DeltaTime = CurrentGetTime - Tmp->LastUpdateTime;
		Tmp->Age += DeltaTime;
		Tmp->LastUpdateTime = CurrentGetTime;

		if (alIsSource(Tmp->Source))
		if (Tmp->DestroyTimeStart > 0.0f)
		{
			if (Tmp->DestroyTime > 0.0f)
			{
				Tmp->DestroyTime -= DeltaTime;
				if (Tmp->DestroyTime < 0.0f) Tmp->DestroyTime = 0.0f;
				alSourcef (Tmp->Source, AL_GAIN, Tmp->MainVolume*Tmp->Volume*(Tmp->DestroyTime/Tmp->DestroyTimeStart));
				alGetError(); // сброс ошибок
			}
			else
			{
				// уже нулевая громкость, можем удалять
				vw_ReleaseSound(Tmp);
				Tmp = 0;
			}

		}

		// смотрим, если уже не играем - надо удалить
		if (Tmp != 0)
		if (alIsSource(Tmp->Source))
		{
			ALint TMPS;
			alGetSourcei(Tmp->Source, AL_SOURCE_STATE, &TMPS);
			alGetError(); // сброс ошибок

			if (TMPS == AL_STOPPED)	vw_ReleaseSound(Tmp);
		}

		Tmp = Tmp1;
	}
}


//------------------------------------------------------------------------------------
// Установка громкости всем SFX
//------------------------------------------------------------------------------------
void vw_SetSoundMainVolume(float NewMainVolume)
{
	eSound *Tmp = StartSoundMan;

	while (Tmp != 0)
	{
		eSound *Tmp1 = Tmp->Next;
		Tmp->SetMainVolume(NewMainVolume);
		Tmp = Tmp1;
	}
}



//------------------------------------------------------------------------------------
// Проверяем, можем ли мы играть звук с данными параметрами
//------------------------------------------------------------------------------------
bool vw_CheckCanPlaySound(int Group, int GroupCount, int SubGroup, int SubGroupCount, int Priority)
{
	// находим кол-во звуков в группе
	int GroupCurrentCount = 0;
	// находим кол-во звуков в подгруппе
	int SubGroupCurrentCount = 0;
	// находим звук, из этой группы с наименьшим приоритетом + самый старый
	eSound *GroupCanStop = 0;
	// находим звук, из этой подгруппы с меньшим или равным приоритетом + самый старый
	eSound *SubGroupCanStop = 0;


	eSound *Tmp = StartSoundMan;
	while (Tmp != 0)
	{
		eSound *Tmp1 = Tmp->Next;

		if (Tmp->DestroyTimeStart == -1.0f)
		if (Tmp->Group == Group)
		{
			if (Tmp->SubGroup == SubGroup)
			{
				SubGroupCurrentCount++;

				// если приоритет выше или такой же, можем его останавливать
				if (Priority <= Tmp->Priority)
				{
					if (SubGroupCanStop == 0)
						SubGroupCanStop = Tmp;// если ничего еще нет, ставим этот
					else
					{
						if (SubGroupCanStop->Priority < Tmp->Priority)
							SubGroupCanStop = Tmp;	// если этот с меньшим приоритетом - берем сразу
						else
							if (SubGroupCanStop->Age < Tmp->Age)
								SubGroupCanStop = Tmp;	// меняем на более старый
					}
				}
			}

			GroupCurrentCount++;
			// если приоритет выше или такой же, можем его останавливать
			if (Priority <= Tmp->Priority)
			{
				if (GroupCanStop == 0)
					GroupCanStop = Tmp;// если ничего еще нет, ставим этот
				else
				{
					if (GroupCanStop->Priority < Tmp->Priority)
							GroupCanStop = Tmp;	// если этот с меньшим приоритетом - берем сразу
					else
						if (GroupCanStop->Age < Tmp->Age)
							GroupCanStop = Tmp;	// меняем на более старый
				}
			}
		}


		Tmp = Tmp1;
	}



	// если в подгруппе нет места - останавливаем звук из подгруппы
	if (SubGroupCount <= SubGroupCurrentCount)
	{
		if (SubGroupCanStop != 0)
		{
			SubGroupCanStop->Stop(0.15f);
			GroupCurrentCount--;
			SubGroupCurrentCount--;
		}
	}
	else // если уже удалили, само собой в группе будет пустое место
	{
		// если в группе нет места - останавливаем звук из группы
		if (GroupCount <= GroupCurrentCount)
		if (GroupCanStop != 0)
		{
			GroupCanStop->Stop(0.15f);
			GroupCurrentCount--;
		}
	}


	// проверяем, если есть место в подгруппе и в группе - говорим, что все ок, можно запускать
	// иначе - выходим и не запускаем новый звук с этими настройками
	if (SubGroupCount > SubGroupCurrentCount && GroupCount > GroupCurrentCount) return true;


	// иначе играть не можем
	return false;
}











//------------------------------------------------------------------------------------
// Возвращаем, если играется хотя бы одна музыка
//------------------------------------------------------------------------------------
bool vw_GetMusicIsPlaying()
{
	eMusic *Tmp = StartMusicMan;

	while (Tmp != 0)
	{
		eMusic *Tmp1 = Tmp->Next;
		// смотрим, если играем что-то, передаем...
		if (alIsSource(Tmp->Source))
		{
			ALint TMPS;
			alGetSourcei(Tmp->Source, AL_SOURCE_STATE, &TMPS);
			alGetError(); // сброс ошибок
			if (TMPS == AL_PLAYING)	return true;
		}
		Tmp = Tmp1;
	}

	return false;
}



//------------------------------------------------------------------------------------
// Освобождение памяти и удаление
//------------------------------------------------------------------------------------
void vw_ReleaseMusic(eMusic* Music)
{
	// проверка входящих данных
	if (Music == 0) return;

	// отключаем от менерджера
	vw_DetachMusic(Music);

	// обязательно остановить!!!
	if (alIsSource(Music->Source))
	{
		alSourceStop(Music->Source);

		// открепляем все буферы от источника
		int queued;
		alGetSourcei(Music->Source, AL_BUFFERS_QUEUED, &queued);
		alGetError(); // сброс ошибок
		while(queued--)
		{
			ALuint buffer;
			alSourceUnqueueBuffers(Music->Source, 1, &buffer);
			alGetError(); // сброс ошибок
		}

		// освобождаем собственно сам источник
		alDeleteSources(1, &Music->Source);
		Music->Source = 0;
		alGetError(); // сброс ошибок
    }


	// удаляем буфер (для музыки именно тут)
	alDeleteBuffers(NUM_OF_DYNBUF, Music->Buffers);
	alGetError(); // сброс ошибок

	if (Music->mVF != 0){ov_clear(Music->mVF);delete Music->mVF; Music->mVF = 0;}


	if (Music->MusicFile != 0)
	{
		eFILE *TMPFile = (eFILE *)Music->MusicFile;
		vw_fclose(TMPFile);
	}

	// освобождаем память
	if (Music != 0){delete Music; Music = 0;}
}


//------------------------------------------------------------------------------------
// освобождаем все подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllMusic()
{
	// Чистка памяти...
	eMusic *Tmp = StartMusicMan;
	while (Tmp != 0)
	{
		eMusic *Tmp1 = Tmp->Next;
		vw_ReleaseMusic(Tmp);
		Tmp = Tmp1;
	}

	StartMusicMan = 0;
	EndMusicMan = 0;
}


//------------------------------------------------------------------------------------
// подключение звука к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachMusic(eMusic* Music)
{
	if (Music == 0) return;

	// первый в списке...
	if (EndMusicMan == 0)
	{
		Music->Prev = 0;
		Music->Next = 0;
		NumMusicMan += 1;
		Music->Num = NumMusicMan;
		StartMusicMan = Music;
		EndMusicMan = Music;
	}
	else // продолжаем заполнение...
	{
		Music->Prev = EndMusicMan;
		Music->Next = 0;
		EndMusicMan->Next = Music;
		NumMusicMan += 1;
		Music->Num = NumMusicMan;
		EndMusicMan = Music;
	}
}


//------------------------------------------------------------------------------------
// отключение от менеджера
//------------------------------------------------------------------------------------
void vw_DetachMusic(eMusic* Music)
{
	if (Music == 0) return;

	// переустанавливаем указатели...
	if (StartMusicMan == Music) StartMusicMan = Music->Next;
	if (EndMusicMan == Music) EndMusicMan = Music->Prev;

	if (Music->Next != 0) Music->Next->Prev = Music->Prev;
		else if (Music->Prev != 0) Music->Prev->Next = 0;
	if (Music->Prev != 0) Music->Prev->Next = Music->Next;
		else if (Music->Next != 0) Music->Next->Prev = 0;
}



//------------------------------------------------------------------------------------
// Обновляем данные в менеджере
//------------------------------------------------------------------------------------
void vw_UpdateMusic()
{
	eMusic *Tmp = StartMusicMan;

	while (Tmp != 0)
	{
		eMusic *Tmp1 = Tmp->Next;
		if (!Tmp->Update()) vw_ReleaseMusic(Tmp);
		Tmp = Tmp1;
	}
}


//------------------------------------------------------------------------------------
// Установка громкости у всех
//------------------------------------------------------------------------------------
void vw_SetMusicMainVolume(float NewMainVolume)
{
	eMusic *Tmp = StartMusicMan;

	while (Tmp != 0)
	{
		eMusic *Tmp1 = Tmp->Next;
		Tmp->SetMainVolume(NewMainVolume);
		Tmp = Tmp1;
	}
}


//------------------------------------------------------------------------------------
// Нахождение по уникальному номеру...
//------------------------------------------------------------------------------------
eMusic* vw_FindMusicByNum(int Num)
{
	eMusic *Tmp = StartMusicMan;
	while (Tmp != 0)
	{
		eMusic *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num) return Tmp;
		Tmp = Tmp1;
	}
	return 0;
}
