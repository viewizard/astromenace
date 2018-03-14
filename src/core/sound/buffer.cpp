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

// TODO CheckALError() and CheckALUTError() should be braced by namespace
// TODO all this returns on errors should be more helpful, add more std::cerr output

#include "../vfs/vfs.h"
#include "buffer.h"

namespace {

std::unordered_map<std::string, ALuint> SoundBuffersMap;
std::unordered_map<std::string, sStreamBuffer> StreamBuffersMap;

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
static bool ReadOggBlock(ALuint BufID, size_t Size, OggVorbis_File *mVF, ALsizei Rate, ALenum Format)
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
 * Find stream buffer by name.
 */
static sStreamBuffer *FindStreamBufferByName(const std::string &Name)
{
	auto tmpBuffers = StreamBuffersMap.find(Name);
	if (tmpBuffers != StreamBuffersMap.end())
		return &tmpBuffers->second;

	return nullptr;
}

/*
 * Reset queued stream buffers by stream source.
 */
static sStreamBuffer *ResetStreamBuffers(sStreamBuffer *StreamBuffer)
{
	// set current position to 0
	ov_pcm_seek(&StreamBuffer->mVF, 0);
	// fill all buffers with proper data
	for (int i = 0; i < NUM_OF_DYNBUF; i++) {
		ReadOggBlock(StreamBuffer->Buffers[i], DYNBUF_SIZE,
			     &StreamBuffer->mVF, StreamBuffer->mInfo->rate,
			     (StreamBuffer->mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
		if (!CheckALError())
			return nullptr;
	}
	return StreamBuffer;
}

/*
 * Set stream buffer source.
 */
static bool SetStreamBufferSource(sStreamBuffer *StreamBuffer, const std::string &FileName)
{
	vw_fclose(StreamBuffer->File);

	StreamBuffer->File = vw_fopen(FileName);
	if (!StreamBuffer->File)
		return false;

	// OggVorbis specific structures
	ov_callbacks cb;
	// fill cb struct
	cb.close_func = VorbisClose;
	cb.read_func = VorbisRead;
	cb.seek_func = VorbisSeek;
	cb.tell_func = VorbisTell;
	// generate local buffers
	if (ov_open_callbacks(StreamBuffer->File.get(), &StreamBuffer->mVF, nullptr, 0, cb) < 0)
		return false; // this is not ogg bitstream

	return true;
}

/*
 * Create stream buffers from OGG file.
 */
sStreamBuffer *vw_CreateStreamBufferFromOGG(const std::string &Name, const std::string &LoopFileName)
{
	sStreamBuffer *StreamBuffer = FindStreamBufferByName(Name);
	if (StreamBuffer) {
		// we could have an issue, if stream switched to 'loop' part
		// so, if call have a LoopFileName, reset stream source just in case
		if (!LoopFileName.empty())
			SetStreamBufferSource(StreamBuffer, Name);
		// caller wait from us new stream buffer, that start playing from 0
		return ResetStreamBuffers(StreamBuffer);
	}

	if (!SetStreamBufferSource(&StreamBuffersMap[Name], Name)) { // create entry on first access
		StreamBuffersMap.erase(Name);
		return nullptr;
	}

	// return vorbis_info structures
	StreamBuffersMap[Name].mInfo = ov_info(&StreamBuffersMap[Name].mVF, -1);

	// create buffers
	alGenBuffers(NUM_OF_DYNBUF, StreamBuffersMap[Name].Buffers.data());
	if (!CheckALError()) {
		ov_clear(&StreamBuffersMap[Name].mVF);
		StreamBuffersMap.erase(Name);
		return nullptr;
	}

	for (int i = 0; i < NUM_OF_DYNBUF; i++) {
		ReadOggBlock(StreamBuffersMap[Name].Buffers[i], DYNBUF_SIZE,
			     &StreamBuffersMap[Name].mVF, StreamBuffersMap[Name].mInfo->rate,
			     (StreamBuffersMap[Name].mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
		if (!CheckALError()) {
			ov_clear(&StreamBuffersMap[Name].mVF);
			StreamBuffersMap.erase(Name);
			return nullptr;
		}
	}

	return &StreamBuffersMap[Name];
}

/*
 * Read and queue stream buffer.
 */
static bool ReadAndQueueStreamBuffer(sStreamBuffer *StreamBuffer, ALuint Source, ALuint bufferID)
{
	if (ReadOggBlock(bufferID, DYNBUF_SIZE, &StreamBuffer->mVF, StreamBuffer->mInfo->rate,
			 (StreamBuffer->mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16)) {
		alSourceQueueBuffers(Source, 1, &bufferID);
		CheckALError();
		return true;
	}

	return false;
}

/*
 * Update stream buffer.
 */
void vw_UpdateStreamBuffer(sStreamBuffer *StreamBuffer, ALuint Source, bool &Looped, std::string &LoopPart)
{
	// get info, how many buffers were used and should be refilled now
	int Processed{0};
	alGetSourcei(Source, AL_BUFFERS_PROCESSED, &Processed);

	while (Processed--) {
		// re-use previous buffers
		ALuint bufferID;
		alSourceUnqueueBuffers(Source, 1, &bufferID);
		if (!CheckALError())
			return;

		if (!ReadAndQueueStreamBuffer(StreamBuffer, Source, bufferID)) {
			// we don't have data from our stream source to fill buffers any more,
			// for looped music - change the current stream source position to 0
			if (Looped) {
				ov_pcm_seek(&StreamBuffer->mVF, 0);
				// we unqueued buffer, should again read and queue it
				if (!ReadAndQueueStreamBuffer(StreamBuffer, Source, bufferID))
					return;
			} else if (!LoopPart.empty()) {
				// if we have "main" part and "loop" part of music (2 files) -
				// switch to "loop" part and make it looped
				SetStreamBufferSource(StreamBuffer, LoopPart);
				Looped = true; // "loop" part always looped
				LoopPart.clear(); // from now "loop" part is current
				// we unqueued buffer, should again read and queue it
				if (!ReadAndQueueStreamBuffer(StreamBuffer, Source, bufferID))
					return;
			}
		}
	}
}

/*
 * Release all stream buffers.
 */
void vw_ReleaseAllStreamBuffers()
{
	for (auto &tmpStream : StreamBuffersMap) {
		ov_clear(&tmpStream.second.mVF);
		alDeleteBuffers(NUM_OF_DYNBUF, tmpStream.second.Buffers.data());
	}
	StreamBuffersMap.clear();
	alGetError(); // reset errors
}

/*
 * Create sound buffer from OGG file.
 */
ALuint vw_CreateSoundBufferFromOGG(const std::string &Name)
{
	ALuint Buffer = vw_FindSoundBufferIDByName(Name);
	if (Buffer)
		return Buffer;

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
	// OggVorbis_File struct
	OggVorbis_File mVF;
	// generate local buffers
	if (ov_open_callbacks(file.get(), &mVF, nullptr, 0, cb) < 0)
		return 0; // this is not ogg bitstream

	// return vorbis_info structures
	vorbis_info *mInfo = ov_info(&mVF, -1);

	// create buffers
	alGenBuffers(1, &Buffer);
	if (!CheckALError()) {
		ov_clear(&mVF);
		return 0;
	}

	// read all data into buffer
	int BlockSize = ov_pcm_total(&mVF, -1) * 4;
	ReadOggBlock(Buffer, BlockSize, &mVF, mInfo->rate,
		     (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
	if (!CheckALError()) {
		ov_clear(&mVF);
		return 0;
	}

	vw_fclose(file);
	ov_clear(&mVF);

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
	if (Buffer)
		return Buffer;

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
