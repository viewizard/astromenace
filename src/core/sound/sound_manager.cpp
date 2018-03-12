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



//------------------------------------------------------------------------------------
// переменные
//------------------------------------------------------------------------------------
cSound *StartSoundMan = nullptr;
cSound *EndSoundMan = nullptr;
int NumSoundMan = 0;

sMusic *StartMusicMan = nullptr;
sMusic *EndMusicMan = nullptr;
int NumMusicMan = 0;







//------------------------------------------------------------------------------------
// Получение ошибок
//------------------------------------------------------------------------------------
ALboolean CheckALCError(ALCdevice *Device)
{
	ALenum ErrCode;
	if ((ErrCode = alcGetError(Device)) != ALC_NO_ERROR) {
		std::cerr << "ALC error: " << alcGetString(Device, ErrCode) << "\n";
		return AL_FALSE;
	}
	return AL_TRUE;
}
ALboolean CheckALError()
{
	ALenum ErrCode;
	if ((ErrCode = alGetError()) != AL_NO_ERROR) {
		std::cerr << "OpenAL error: " << alGetString(ErrCode) << "\n";
		return AL_FALSE;
	}
	return AL_TRUE;
}
ALboolean CheckALUTError()
{
	ALenum ErrCode;
	if ((ErrCode = alutGetError()) != ALUT_ERROR_NO_ERROR) {
		std::cerr << "OpenAL alut error: " << alutGetErrorString(ErrCode) << "\n";
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


	alutInitWithoutContext(nullptr, nullptr);
	if (!CheckALUTError())
		return false;

	ALCcontext *Context = nullptr;
	ALCdevice *Device = nullptr;

	// Open default sound device
	Device = alcOpenDevice(nullptr); // "Generic Software"
	// Check for errors
	if (!Device) {
		std::cerr << "Default sound device not found.\n";
		return false;
	}

	// Creating rendering context
	Context = alcCreateContext(Device, nullptr);
	if (!CheckALCError(Device))
		return false;

	alcMakeContextCurrent(Context);
	if (!CheckALCError(Device))
		return false;

	// сброс ошибок
	alGetError();
	alcGetError(Device);

	// Set listener properties
	alListenerfv(AL_POSITION,    ListenerPos);
	alListenerfv(AL_VELOCITY,    ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
	// затухание звука
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);


	std::cout << "Vendor     : " << alGetString(AL_VENDOR) << "\n";
	std::cout << "Renderer   : " << alGetString(AL_RENDERER) << "\n";
	std::cout << "Version    : " << alGetString(AL_VERSION) << "\n";
	std::cout << "ALut ver   : " << alutGetMajorVersion() << "." << alutGetMinorVersion() << "\n";

#ifdef gamedebug
	// print all supported OpenAL extensions (one per line)
	if (alGetString(AL_EXTENSIONS) != nullptr) {
		std::string extensions{(char *)alGetString(AL_EXTENSIONS)};
		if (!extensions.empty()) {
			std::replace(extensions.begin(), extensions.end(), ' ', '\n'); // replace all ' ' to '\n'
			std::cout << "Supported OpenAL extensions:\n" << extensions.c_str() << "\n";
		}
	}
#endif // gamedebug

	std::cout << "\n";

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
	vw_ReleaseAllSoundBuffers();



	ALCcontext *Context = nullptr;
	ALCdevice *Device = nullptr;

	//Get active context
	Context = alcGetCurrentContext();
	//Get device for active context
	Device = alcGetContextsDevice(Context);


	// Если можем убрать Context, чтобы он не был текущим
	if (alcMakeContextCurrent(nullptr)) {
		alcGetError(Device);

		// Destroy context
		if ((Context != nullptr) &&
		    (alcGetCurrentContext() != Context))
			alcDestroyContext(Context);
		alcGetError(Device);

		// Close sound device
		if (Device != nullptr)
			alcCloseDevice(Device);
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
void vw_ReleaseSound(cSound* Sound)
{
	// проверка входящих данных
	if (Sound == nullptr)
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
	if (Sound != nullptr)
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
		while (Tmp != nullptr) {
			cSound *Tmp1 = Tmp->Next;
			vw_ReleaseSound(Tmp);
			Tmp = Tmp1;
		}

		StartSoundMan = nullptr;
		EndSoundMan = nullptr;
	} else {

		cSound *Tmp = StartSoundMan;
		while (Tmp != nullptr) {
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
	if (Sound == nullptr)
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
	if (Sound == nullptr)
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
	while (Tmp != nullptr) {
		cSound *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num) return Tmp;
		Tmp = Tmp1;
	}
	return nullptr;
}


//------------------------------------------------------------------------------------
// Нахождение по имени
//------------------------------------------------------------------------------------
cSound *vw_FindSoundByName(const char *Name)
{
	if (Name == nullptr)
		return nullptr;

	cSound *Tmp = StartSoundMan;
	while (Tmp != nullptr) {
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

	while (Tmp != nullptr) {
		cSound *Tmp1 = Tmp->Next;

		// считаем сколько играем этот эффект
		float DeltaTime = CurrentGetTime - Tmp->LastUpdateTime;
		Tmp->Age += DeltaTime;
		Tmp->LastUpdateTime = CurrentGetTime;

		if ((alIsSource(Tmp->Source)) &&
		    (Tmp->DestroyTimeStart > 0.0f)) {
			if (Tmp->DestroyTime > 0.0f) {
				Tmp->DestroyTime -= DeltaTime;
				if (Tmp->DestroyTime < 0.0f)
					Tmp->DestroyTime = 0.0f;
				alSourcef (Tmp->Source, AL_GAIN, Tmp->MainVolume*Tmp->Volume*(Tmp->DestroyTime/Tmp->DestroyTimeStart));
				alGetError(); // сброс ошибок
			} else {
				// уже нулевая громкость, можем удалять
				vw_ReleaseSound(Tmp);
				Tmp = nullptr;
			}
		}

		// смотрим, если уже не играем - надо удалить
		if ((Tmp != nullptr) &&
		    (alIsSource(Tmp->Source))) {
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
	cSound *Tmp = StartSoundMan;

	while (Tmp != nullptr) {
		cSound *Tmp1 = Tmp->Next;
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
	cSound *GroupCanStop = nullptr;
	// находим звук, из этой подгруппы с меньшим или равным приоритетом + самый старый
	cSound *SubGroupCanStop = nullptr;


	cSound *Tmp = StartSoundMan;
	while (Tmp != nullptr) {
		cSound *Tmp1 = Tmp->Next;

		if ((Tmp->DestroyTimeStart == -1.0f) &&
		    (Tmp->Group == Group)) {
			if (Tmp->SubGroup == SubGroup) {
				SubGroupCurrentCount++;

				// если приоритет выше или такой же, можем его останавливать
				if (Priority <= Tmp->Priority) {
					if (SubGroupCanStop == nullptr)
						SubGroupCanStop = Tmp;// если ничего еще нет, ставим этот
					else {
						if (SubGroupCanStop->Priority < Tmp->Priority)
							SubGroupCanStop = Tmp;	// если этот с меньшим приоритетом - берем сразу
						else if (SubGroupCanStop->Age < Tmp->Age)
							SubGroupCanStop = Tmp;	// меняем на более старый
					}
				}
			}

			GroupCurrentCount++;
			// если приоритет выше или такой же, можем его останавливать
			if (Priority <= Tmp->Priority) {
				if (GroupCanStop == nullptr)
					GroupCanStop = Tmp;// если ничего еще нет, ставим этот
				else {
					if (GroupCanStop->Priority < Tmp->Priority)
						GroupCanStop = Tmp;	// если этот с меньшим приоритетом - берем сразу
					else if (GroupCanStop->Age < Tmp->Age)
						GroupCanStop = Tmp;	// меняем на более старый
				}
			}
		}

		Tmp = Tmp1;
	}



	// если в подгруппе нет места - останавливаем звук из подгруппы
	if (SubGroupCount <= SubGroupCurrentCount) {
		if (SubGroupCanStop != nullptr) {
			SubGroupCanStop->Stop(0.15f);
			GroupCurrentCount--;
			SubGroupCurrentCount--;
		}
	} else { // если уже удалили, само собой в группе будет пустое место
		// если в группе нет места - останавливаем звук из группы
		if (GroupCount <= GroupCurrentCount)
			if (GroupCanStop != nullptr) {
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
	sMusic *Tmp = StartMusicMan;

	while (Tmp != nullptr) {
		sMusic *Tmp1 = Tmp->Next;
		// смотрим, если играем что-то, передаем...
		if (alIsSource(Tmp->Source)) {
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
void vw_ReleaseMusic(sMusic* Music)
{
	// проверка входящих данных
	if (Music == nullptr)
		return;

	// отключаем от менерджера
	vw_DetachMusic(Music);

	// обязательно остановить!!!
	if (alIsSource(Music->Source)) {
		alSourceStop(Music->Source);

		// открепляем все буферы от источника
		int queued;
		alGetSourcei(Music->Source, AL_BUFFERS_QUEUED, &queued);
		alGetError(); // сброс ошибок
		while(queued--) {
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

	if (Music->mVF != nullptr) {
		ov_clear(Music->mVF);
		delete Music->mVF;
		Music->mVF = nullptr;
	}


	if (Music->MusicFile != nullptr)
		vw_fclose(Music->MusicFile);

	// освобождаем память
	if (Music != nullptr)
		delete Music;
}


//------------------------------------------------------------------------------------
// освобождаем все подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllMusic()
{
	// Чистка памяти...
	sMusic *Tmp = StartMusicMan;
	while (Tmp != nullptr) {
		sMusic *Tmp1 = Tmp->Next;
		vw_ReleaseMusic(Tmp);
		Tmp = Tmp1;
	}

	StartMusicMan = nullptr;
	EndMusicMan = nullptr;
}


//------------------------------------------------------------------------------------
// подключение звука к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachMusic(sMusic* Music)
{
	if (Music == nullptr)
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
void vw_DetachMusic(sMusic* Music)
{
	if (Music == nullptr) return;

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

	while (Tmp != nullptr) {
		sMusic *Tmp1 = Tmp->Next;
		if (!Tmp->Update()) vw_ReleaseMusic(Tmp);
		Tmp = Tmp1;
	}
}


//------------------------------------------------------------------------------------
// Установка громкости у всех
//------------------------------------------------------------------------------------
void vw_SetMusicMainVolume(float NewMainVolume)
{
	sMusic *Tmp = StartMusicMan;

	while (Tmp != nullptr) {
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
	while (Tmp != nullptr) {
		sMusic *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num) return Tmp;
		Tmp = Tmp1;
	}
	return nullptr;
}
