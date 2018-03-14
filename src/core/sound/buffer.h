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

#ifndef Buffer_H
#define Buffer_H

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


#define NUM_OF_DYNBUF	20	// (stream) num buffers in queue
#define DYNBUF_SIZE	16384	// (stream) buffer size

struct sFILE;

struct sStreamBuffer {
	std::array<ALuint, NUM_OF_DYNBUF> Buffers;
	std::unique_ptr<sFILE> File;
	OggVorbis_File mVF;
	vorbis_info *mInfo{nullptr};
};

// Create stream buffer from OGG file.
sStreamBuffer *vw_CreateStreamBufferFromOGG(const std::string &Name, const std::string &LoopFileName);
// Release all stream buffers.
void vw_UpdateStreamBuffer(sStreamBuffer *StreamBuffer, ALuint Source, bool &Looped, std::string &LoopPart);
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

#endif // Buffer_H
