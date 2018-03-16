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

#include "../system/system.h"
#include "buffer.h"
#include "sound.h"


//------------------------------------------------------------------------------------
// Получение ошибок
//------------------------------------------------------------------------------------
ALboolean CheckALCError(ALCdevice *Device, const char *FunctionName)
{
	ALenum ErrCode;
	if ((ErrCode = alcGetError(Device)) != ALC_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "ALC error: " << alcGetString(Device, ErrCode) << "\n";
		return AL_FALSE;
	}
	return AL_TRUE;
}
ALboolean CheckALError(const char *FunctionName)
{
	ALenum ErrCode;
	if ((ErrCode = alGetError()) != AL_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "OpenAL error: " << alGetString(ErrCode) << "\n";
		return AL_FALSE;
	}
	return AL_TRUE;
}

ALboolean CheckALUTError(const char *FunctionName)
{
	ALenum ErrCode;
	if ((ErrCode = alutGetError()) != ALUT_ERROR_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "OpenAL alut error: " << alutGetErrorString(ErrCode) << "\n";
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
	ALfloat ListenerPos[] = {0.0f, 0.0f, 0.0f};
	// Velocity of the Listener.
	ALfloat ListenerVel[] = {0.0f, 0.0f, 0.0f};
	// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
	// Also note that these should be units of '1'.
	ALfloat ListenerOri[] = {0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f};

	alutInitWithoutContext(nullptr, nullptr);
	if (!CheckALUTError(__func__))
		return false;

	ALCcontext *Context{nullptr};
	ALCdevice *Device{nullptr};

	// Open default sound device
	Device = alcOpenDevice(nullptr); // "Generic Software"
	// Check for errors
	if (!Device) {
		std::cerr << "Default sound device not found.\n";
		return false;
	}

	// Creating rendering context
	Context = alcCreateContext(Device, nullptr);
	if (!CheckALCError(Device, __func__))
		return false;

	alcMakeContextCurrent(Context);
	if (!CheckALCError(Device, __func__))
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
	vw_ReleaseAllSounds();
	vw_ReleaseAllMusic();
	// убираем все буферы
	vw_ReleaseAllStreamBuffers();
	vw_ReleaseAllSoundBuffers();

	ALCcontext *Context{nullptr};
	ALCdevice *Device{nullptr};

	//Get active context
	Context = alcGetCurrentContext();
	//Get device for active context
	Device = alcGetContextsDevice(Context);

	// Если можем убрать Context, чтобы он не был текущим
	if (alcMakeContextCurrent(nullptr)) {
		alcGetError(Device);

		// Destroy context
		if (Context && (alcGetCurrentContext() != Context))
			alcDestroyContext(Context);
		alcGetError(Device);

		// Close sound device
		if (Device)
			alcCloseDevice(Device);
	}

	alutExit();
	CheckALUTError(__func__);
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
