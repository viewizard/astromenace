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

// TODO in future, use make_unique() to make unique_ptr-s (C++14)
// TODO CheckALError() and CheckALUTError() should be braced by namespace
// TODO remove code duplication (vorbis callback functions) in buffer/music

#include "../vfs/vfs.h"
#include "buffer.h"

namespace {

std::unordered_map<std::string, ALuint> SoundBuffersMap;

} // unnamed namespace

ALboolean CheckALError();
ALboolean CheckALUTError();


/*
 * Vorbis callback functions
 */
static size_t VorbisRead(void *ptr, size_t byteSize, size_t sizeToRead, void *datasource)
{
	sFILE *vorbisData = (sFILE*)datasource;
	return vorbisData->fread(ptr, byteSize, sizeToRead);
}
static int VorbisSeek(void *datasource, ogg_int64_t offset, int whence)
{
	sFILE *vorbisData = (sFILE*)datasource;
	return vorbisData->fseek(offset, whence);
}
static int VorbisClose(void *UNUSED(datasource))
{
	return 1;
}
static long VorbisTell(void *datasource)
{
	sFILE *vorbisData = (sFILE*)datasource;
	return vorbisData->ftell();
}

/*
 * Read OGG block.
 */
static bool ReadOggBlockSFX(ALuint BufID, size_t Size, OggVorbis_File *mVF, ALsizei Rate, ALenum Format)
{
	if (!mVF || !Size)
		return false;

	std::vector<char> PCM(Size);
	int current_section;
	unsigned int TotalRet{0};
	long ret{0};
	// read loop
	while (TotalRet < Size) {
		ret = ov_read(mVF, PCM.data() + TotalRet, Size - TotalRet, 0, 2, 1, &current_section);

		// if end of file or read limit exceeded
		if (ret == 0)
			break;
		else if (ret > 0)
			TotalRet += ret;
	}

	if (TotalRet > 0) {
		alBufferData(BufID, Format, PCM.data(), TotalRet, Rate);
		CheckALError(); // reset errors
	}

	return (ret > 0);
}

/*
 * Create sound buffer from OGG file.
 */
ALuint vw_CreateSoundBufferFromOGG(const std::string &Name)
{
	ALuint Buffer = vw_FindSoundBufferIDByName(Name);
	if (Buffer) {
		std::cout << "Buffer already created ... " << Name << "\n";
		return Buffer;
	}

	std::unique_ptr<sFILE> file = vw_fopen(Name);
	if (!file)
		return 0;

	// OggVorbis specific structures
	ov_callbacks cb;
	// fill cb struct
	cb.close_func = VorbisClose;
	cb.read_func = VorbisRead;
	cb.seek_func = VorbisSeek;
	cb.tell_func = VorbisTell;
	// create OggVorbis_File struct
	std::unique_ptr<OggVorbis_File> mVF{new OggVorbis_File};

	// generate local buffers
	if (ov_open_callbacks(file.get(), mVF.get(), nullptr, 0, cb) < 0)
		return 0; // this is not ogg bitstream

	// return vorbis_info structures
	vorbis_info *mInfo = ov_info(mVF.get(), -1);

	// create buffers
	alGenBuffers(1, &Buffer);
	if (!CheckALError())
		return 0;

	// read all data into buffer
	int BlockSize = ov_pcm_total(mVF.get(), -1) * 4;
	ReadOggBlockSFX(Buffer, BlockSize, mVF.get(), mInfo->rate,
			(mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
	if (!CheckALError())
		return 0;

	vw_fclose(file);
	if (!mVF.get())
		ov_clear(mVF.get());

	if (Buffer) {
		SoundBuffersMap.emplace(Name, Buffer);
		std::cout << "Buffer ... " << Name << "\n";
	}

	return Buffer;
}

/*
 * Create sound buffer from WAV file.
 */
ALuint vw_CreateSoundBufferFromWAV(const std::string &Name)
{
	ALuint Buffer = vw_FindSoundBufferIDByName(Name);
	if (Buffer) {
		std::cout << "Buffer already created ... " << Name << "\n";
		return Buffer;
	}

	std::unique_ptr<sFILE> file = vw_fopen(Name);
	if (!file)
		return 0;

	Buffer = alutCreateBufferFromFileImage(file->Data.get(), file->Size);
	if (!CheckALUTError())
		return 0;

	vw_fclose(file);

	if (Buffer) {
		SoundBuffersMap.emplace(Name, Buffer);
		std::cout << "Buffer ... " << Name << "\n";
	}

	return Buffer;
}

/*
 * Find sound buffer by name.
 */
ALuint vw_FindSoundBufferIDByName(const std::string &Name)
{
	auto tmpBuffer = SoundBuffersMap.find(Name);
	if (tmpBuffer != SoundBuffersMap.end())
		return tmpBuffer->second;

	return 0;
}

/*
 * Release all sound buffers.
 */
void vw_ReleaseAllSoundBuffers()
{
	for (auto &tmpBuffer : SoundBuffersMap) {
		if (tmpBuffer.second)
			alDeleteBuffers(1, &tmpBuffer.second);
	}
	SoundBuffersMap.clear();
	alGetError(); // reset errors
}
