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


#include "buffer.h"

ALboolean CheckALError();
ALboolean CheckALUTError();


sBuffer *StartBufferMan = nullptr;
sBuffer *EndBufferMan = nullptr;






//------------------------------------------------------------------------------------
// Vorbis callback functions
//------------------------------------------------------------------------------------
size_t VorbisReadSFX(void *ptr, size_t byteSize, size_t sizeToRead, void *datasource)
{
	// нужно возвращать кол-во прочитаных byteSize
	sFILE* vorbisData = (sFILE*)datasource;
	return vorbisData->fread(ptr, byteSize, sizeToRead);
}
int VorbisSeekSFX(void *datasource, ogg_int64_t offset, int whence)
{
	sFILE* vorbisData = (sFILE*)datasource;
	return vorbisData->fseek(offset, whence);
}
int VorbisCloseSFX(void *UNUSED(datasource))
{
	// похоже, без этой функции у линукс версии vorbis проблемы, хотя по документации...
	return 1;
}
long VorbisTellSFX(void *datasource)
{
	sFILE*	vorbisData = (sFILE*)datasource;
	return vorbisData->ftell();
}



//------------------------------------------------------------------------------------
// Читаем фрагмент ogg файла
//------------------------------------------------------------------------------------
bool ReadOggBlockSFX(ALuint BufID, size_t Size, OggVorbis_File *mVF, ALsizei Rate, ALenum Format)
{
	if (mVF == nullptr)
		return false;

	// var
	int			current_section;
	unsigned int TotalRet = 0;
	long		ret = 0;
	char		*PCM;

	if (Size < 1) return false;
	PCM = new char[Size];

	// Read loop
	while (TotalRet < Size) {
		ret = ov_read(mVF, PCM + TotalRet, Size - TotalRet, 0, 2, 1, &current_section);

		// if end of file or read limit exceeded
		if (ret == 0) break;
		else if (ret < 0) {	// Error in bitstream
			//
		} else {
			TotalRet += ret;
		}
	}
	if (TotalRet > 0) {
		alBufferData(BufID, Format, PCM, TotalRet, Rate);
		CheckALError();
	}
	delete [] PCM;
	return (ret > 0);
}



//------------------------------------------------------------------------------------
// Создаем буфер, для ogg SFX
//------------------------------------------------------------------------------------
ALuint vw_CreateSoundBufferFromOGG(const char *Name)
{
	ALuint Buffer;

	std::unique_ptr<sFILE> file = vw_fopen(Name);
	if (file == nullptr)
		return 0;

	// OggVorbis specific structures
	ov_callbacks	cb;
	// Fill cb struct
	cb.close_func	= VorbisCloseSFX;
	cb.read_func	= VorbisReadSFX;
	cb.seek_func	= VorbisSeekSFX;
	cb.tell_func	= VorbisTellSFX;
	// Create OggVorbis_File struct
	OggVorbis_File *mVF = new OggVorbis_File;

	// Generate local buffers
	if (ov_open_callbacks(file.get(), mVF, nullptr, 0, cb) < 0)
		// This is not ogg bitstream. Return
		return 0;

	// Return vorbis_info structures
	vorbis_info *mInfo = ov_info(mVF, -1);

	// Create buffers
	alGenBuffers(1, &Buffer);
	if (!CheckALError()) return 0;

	// читаем все сразу, в один буфер
	int BlockSize = ov_pcm_total(mVF, -1) * 4;
	ReadOggBlockSFX(Buffer, BlockSize, mVF, mInfo->rate, (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
	if (!CheckALError()) return 0;

	vw_fclose(file);
	if (!mVF) {
		ov_clear(mVF);
		delete mVF;
	}


	// вот теперь создаем объект
	if (Buffer != 0) {
		sBuffer *TMPBuffer = new sBuffer;
		vw_AttachBuffer(TMPBuffer);

		TMPBuffer->Name = new char[strlen(Name)+1];
		strcpy(TMPBuffer->Name, Name);
		TMPBuffer->Buffer = Buffer;

		std::cout << "Buffer ... " << Name << "\n";
	}

	return Buffer;
}









//------------------------------------------------------------------------------------
// Создаем буфер, для wav SFX
//------------------------------------------------------------------------------------
ALuint vw_CreateSoundBufferFromWAV(const char *Name)
{
	ALuint Buffer;

	std::unique_ptr<sFILE> file = vw_fopen(Name);
	if (file == nullptr)
		return 0;

	Buffer = alutCreateBufferFromFileImage(file->Data.get(), file->Size);
	if (!CheckALUTError())
		return 0;

	vw_fclose(file);


	// вот теперь создаем объект
	if (Buffer != 0) {
		sBuffer *TMPBuffer = new sBuffer;
		vw_AttachBuffer(TMPBuffer);

		TMPBuffer->Name = new char[strlen(Name) + 1];
		strcpy(TMPBuffer->Name, Name);
		TMPBuffer->Buffer = Buffer;

		std::cout << "Buffer ... " << Name << "\n";
	}

	return Buffer;
}
//------------------------------------------------------------------------------------
// Удаляем буфер
//------------------------------------------------------------------------------------
void vw_ReleaseBuffer(sBuffer *Buffer)
{
	if (Buffer == nullptr)
		return;

	vw_DetachBuffer(Buffer);

	if (Buffer->Name != nullptr) {
		delete [] Buffer->Name;
		Buffer->Name = nullptr;
	}

	if (Buffer->Buffer != 0) {
		alDeleteBuffers(1, &Buffer->Buffer);
		Buffer->Buffer = 0;
	}
	alGetError(); // сброс ошибок

	delete Buffer;
}



sBuffer *vw_FindBufferByName(const char *Name)
{
	sBuffer *Tmp = StartBufferMan;

	while (Tmp != nullptr) {
		sBuffer *Tmp1 = Tmp->Next;
		if(strcmp(Tmp->Name, Name) == 0) return Tmp;
		Tmp = Tmp1;
	}

	return nullptr;
}

ALuint vw_FindBufferIDByName(const char *Name)
{
	sBuffer *Tmp = StartBufferMan;

	while (Tmp != nullptr) {
		sBuffer *Tmp1 = Tmp->Next;
		if(strcmp(Tmp->Name, Name) == 0) return Tmp->Buffer;
		Tmp = Tmp1;
	}

	return 0;
}

void vw_ReleaseAllBuffers()
{
	// Чистка памяти...
	sBuffer *Tmp = StartBufferMan;
	while (Tmp != nullptr) {
		sBuffer *Tmp1 = Tmp->Next;
		vw_ReleaseBuffer(Tmp);
		Tmp = Tmp1;
	}

	StartBufferMan = nullptr;
	EndBufferMan = nullptr;
}

void vw_AttachBuffer(sBuffer* Buffer)
{
	if (Buffer == nullptr)
		return;

	// первый в списке...
	if (EndBufferMan == nullptr) {
		Buffer->Prev = nullptr;
		Buffer->Next = nullptr;
		StartBufferMan = Buffer;
		EndBufferMan = Buffer;
	} else { // продолжаем заполнение...
		Buffer->Prev = EndBufferMan;
		Buffer->Next = nullptr;
		EndBufferMan->Next = Buffer;
		EndBufferMan = Buffer;
	}
}

void vw_DetachBuffer(sBuffer* Buffer)
{
	if (Buffer == nullptr)
		return;

	// переустанавливаем указатели...
	if (StartBufferMan == Buffer)
		StartBufferMan = Buffer->Next;
	if (EndBufferMan == Buffer)
		EndBufferMan = Buffer->Prev;

	if (Buffer->Next != nullptr)
		Buffer->Next->Prev = Buffer->Prev;
	else if (Buffer->Prev != nullptr)
		Buffer->Prev->Next = nullptr;
	if (Buffer->Prev != nullptr)
		Buffer->Prev->Next = Buffer->Next;
	else if (Buffer->Next != nullptr)
		Buffer->Next->Prev = nullptr;
}
