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


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

/*
Should be used for 'sound' system internal purposes only. Don't include into other
sources, you are free to change or add methods to sound.h header file instead.
*/

#ifndef CORE_AUDIO_BUFFER_H
#define CORE_AUDIO_BUFFER_H

#include "../base.h"
#include "../vfs/vfs.h"
#include "openal.h"
#include "vorbis/vorbisfile.h"
#include "ogg/ogg.h"

namespace viewizard {

struct sStreamBuffer;

// Create stream buffer from OGG file.
sStreamBuffer *vw_CreateStreamBufferFromOGG(const std::string &Name, const std::string &LoopFileName);
// Update all stream buffers.
void vw_UpdateStreamBuffer(sStreamBuffer *StreamBuffer, ALuint Source, bool &Looped, std::string &LoopPart);
// Queue stream buffer.
bool vw_QueueStreamBuffer(sStreamBuffer *StreamBuffer, ALuint Source);
// Unqueue stream buffer.
bool vw_UnqueueStreamBuffer(sStreamBuffer *StreamBuffer, ALuint Source);
// Release all stream buffers.
void vw_ReleaseAllStreamBuffers();

// Create sound buffer from OGG file.
ALuint vw_CreateSoundBufferFromWAV(const std::string &Name);
// Create sound buffer from WAV file.
ALuint vw_CreateSoundBufferFromOGG(const std::string &Name);
// Find sound buffer by name.
ALuint vw_FindSoundBufferIDByName(const std::string &Name);
// Release all sound buffers.
void vw_ReleaseAllSoundBuffers();

} // viewizard namespace

#endif // CORE_AUDIO_BUFFER_H
