/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "buffer.h"
#include "audio.h"

namespace viewizard {

namespace {

/*
From freealut documentation:

alutInit and alutInitWithoutContext put ALUT into the initialized state.
Those functions must only be called when the state is uninitialized.
...
The following functions must only be called in an initialized state and with a current context: alutExit,...
*/
bool AlutInitStatus{false};

} // unnamed namespace


/*
 * Get audio initialization status.
 */
bool vw_GetAudioStatus()
{
	return AlutInitStatus;
}

/*
 * Initialize audio.
 */
bool vw_InitAudio()
{
	// position of the Listener
	constexpr ALfloat ListenerPos[]{0.0f, 0.0f, 0.0f};
	// velocity of the Listener
	constexpr ALfloat ListenerVel[]{0.0f, 0.0f, 0.0f};
	// orientation of the Listener (first 3 elements are "at", second 3 are "up")
	// note, that these should be units of '1'
	constexpr ALfloat ListenerOri[]{0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};

	if (AlutInitStatus)
		return true;

	alutInitWithoutContext(nullptr, nullptr);
	if (!CheckALUTError(__func__))
		return false;

	// open default sound device
	ALCdevice *Device = alcOpenDevice(nullptr);
	// check for errors
	if (!Device) {
		std::cerr << __func__ << "(): " << "Default sound device not found.\n";
		return false;
	}

	// creating rendering context
	ALCcontext *Context = alcCreateContext(Device, nullptr);
	if (!CheckALCError(Device, __func__))
		return false;

	alcMakeContextCurrent(Context);
	if (!CheckALCError(Device, __func__))
		return false;

	// set listener properties
	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

	std::cout << "Vendor     : " << alGetString(AL_VENDOR) << "\n";
	std::cout << "Renderer   : " << alGetString(AL_RENDERER) << "\n";
	std::cout << "Version    : " << alGetString(AL_VERSION) << "\n";
	std::cout << "ALut ver   : " << alutGetMajorVersion() << "." << alutGetMinorVersion() << "\n";

#ifndef NDEBUG
	// print all supported OpenAL extensions (one per line)
	if (alGetString(AL_EXTENSIONS)) {
		std::string extensions{(char *)alGetString(AL_EXTENSIONS)};
		if (!extensions.empty()) {
			// replace all ' ' to '\n'
			std::replace(extensions.begin(), extensions.end(), ' ', '\n');
			std::cout << "Supported OpenAL extensions:\n" << extensions.c_str() << "\n";
		}
	}
#endif // NDEBUG

	std::cout << "\n";

	ResetALError();

	AlutInitStatus = true;
	return true;
}

/*
 * Shutdown audio.
 */
void vw_ShutdownAudio()
{
	if (!AlutInitStatus)
		return;

	vw_ReleaseAllSounds();
	vw_ReleaseAllMusic();
	vw_ReleaseAllStreamBuffers();
	vw_ReleaseAllSoundBuffers();

	// get active context
	ALCcontext *Context = alcGetCurrentContext();
	// get device for active context
	ALCdevice *Device = alcGetContextsDevice(Context);
	CheckALCError(Device, __func__);

	// change current context (remove our context)
	if (alcMakeContextCurrent(nullptr)) {
		// destroy context
		if (Context && (alcGetCurrentContext() != Context)) {
			alcDestroyContext(Context);
			CheckALCError(Device, __func__);
		}

		// close sound device
		if (Device) {
			alcCloseDevice(Device);
			CheckALCError(Device, __func__);
		}
	}

	alutExit();
	CheckALUTError(__func__);
	AlutInitStatus = false;
}

/*
 * Setup listener in 3D space.
 * Note, orientation expressed as "at" and "up" vectors.
 */
void vw_Listener(float (&ListenerPosition)[3], float (&ListenerVelocity)[3], float (&ListenerOrientation)[6])
{
	alListenerfv(AL_POSITION, ListenerPosition);
	alListenerfv(AL_VELOCITY, ListenerVelocity);
	alListenerfv(AL_ORIENTATION, ListenerOrientation);
	ResetALError();
}

} // viewizard namespace
