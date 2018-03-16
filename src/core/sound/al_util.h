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

#ifndef al_util_H
#define al_util_H

#include "../base.h"

#if defined(__APPLE__) && defined(__MACH__)
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#else // defined(__APPLE__) && defined(__MACH__)
#include "AL/al.h"
#include "AL/alc.h"
#endif // defined(__APPLE__) && defined(__MACH__)
#include "AL/alut.h"

#include "vorbis/vorbisfile.h"
#include "ogg/ogg.h"


/*
 * Check ALC errors.
 */
inline ALboolean CheckALCError(ALCdevice *Device, const char *FunctionName)
{
	ALenum ErrCode;
	if ((ErrCode = alcGetError(Device)) != ALC_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "ALC error: " << alcGetString(Device, ErrCode) << "\n";
		return AL_FALSE;
	}
	return AL_TRUE;
}

/*
 * Check AL errors.
 */
inline ALboolean CheckALError(const char *FunctionName)
{
	ALenum ErrCode;
	if ((ErrCode = alGetError()) != AL_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "OpenAL error: " << alGetString(ErrCode) << "\n";
		return AL_FALSE;
	}
	return AL_TRUE;
}

/*
 * Check ALUT errors.
 */
inline ALboolean CheckALUTError(const char *FunctionName)
{
	ALenum ErrCode;
	if ((ErrCode = alutGetError()) != ALUT_ERROR_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "OpenAL alut error: " << alutGetErrorString(ErrCode) << "\n";
		return AL_FALSE;
	}
	return AL_TRUE;
}

/*
 * Check OpenAL source status.
 */
inline bool CheckALSourceState(ALuint Source, ALint State)
{
	ALint tmpState;
	alGetSourcei(Source, AL_SOURCE_STATE, &tmpState);
	alGetError(); // reset errors
	return (tmpState == State);
}

#endif // al_util_H
