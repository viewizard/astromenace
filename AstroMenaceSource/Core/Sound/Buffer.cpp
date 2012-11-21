/********************************************************************************

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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#include "Buffer.h"

ALboolean CheckALError();
ALboolean CheckALUTError();


eBuffer *StartBufferMan = 0;
eBuffer *EndBufferMan = 0;






//------------------------------------------------------------------------------------
// Vorbis callback functions
//------------------------------------------------------------------------------------
size_t VorbisReadSFX(void *ptr, size_t byteSize, size_t sizeToRead, void *datasource)
{
	// нужно возвращать кол-во прочитаных byteSize
	eFILE* vorbisData = (eFILE*)datasource;
	return vorbisData->fread(ptr, byteSize, sizeToRead);
}
int VorbisSeekSFX(void *datasource, ogg_int64_t offset, int whence)
{
	eFILE* vorbisData = (eFILE*)datasource;
	return vorbisData->fseek(offset, whence);
}
int VorbisCloseSFX(void *UNUSED(datasource))
{// похоже, без этой функции у линукс версии vorbis проблемы, хотя по документации...
	return 1;
}
long VorbisTellSFX(void *datasource)
{
	eFILE*	vorbisData = (eFILE*)datasource;
	return vorbisData->ftell();
}



//------------------------------------------------------------------------------------
// Читаем фрагмент ogg файла
//------------------------------------------------------------------------------------
bool ReadOggBlockSFX(ALuint BufID, size_t Size, OggVorbis_File *mVF, ALsizei Rate, ALenum Format)
{
	if (mVF == 0) return false;

	// var
	int			current_section;
	unsigned int TotalRet = 0;
	long		ret = 0;
	char		*PCM;

	if (Size < 1) return false;
	PCM = new char[Size];

	// Read loop
	while (TotalRet < Size)
	{
		ret = ov_read(mVF, PCM + TotalRet, Size - TotalRet, 0, 2, 1, &current_section);

		// if end of file or read limit exceeded
		if (ret == 0) break;
		else if (ret < 0) 		// Error in bitstream
		{
			//
		}
		else
		{
			TotalRet += ret;
		}
	}
	if (TotalRet > 0)
	{
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

	eFILE *file = 0;
	file = vw_fopen(Name);
	if (file == 0) return 0;

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
	if (ov_open_callbacks(file, mVF, NULL, 0, cb) < 0)
	{
		// This is not ogg bitstream. Return
		return 0;
	}

	// Return vorbis_info structures
	vorbis_info		*mInfo		= ov_info(mVF, -1);

	// Create buffers
	alGenBuffers(1, &Buffer);
	if (!CheckALError()) return 0;

	// читаем все сразу, в один буфер
	int BlockSize = ov_pcm_total(mVF, -1) * 4;
	ReadOggBlockSFX(Buffer, BlockSize, mVF, mInfo->rate, (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
	if (!CheckALError()) return 0;

	vw_fclose(file);
	if (!mVF){ov_clear(mVF);delete mVF;}


	// вот теперь создаем объект
	if (Buffer != 0)
	{
		eBuffer *TMPBuffer = 0;
		TMPBuffer = new eBuffer;
		vw_AttachBuffer(TMPBuffer);

		TMPBuffer->Name = new char[strlen(Name)+1]; if (TMPBuffer->Name == 0) return 0;
		strcpy(TMPBuffer->Name, Name);
		TMPBuffer->Buffer = Buffer;

		printf("Buffer ... %s\n", Name);
	}

	return Buffer;
}









//------------------------------------------------------------------------------------
// Создаем буфер, для wav SFX
//------------------------------------------------------------------------------------
ALuint vw_CreateSoundBufferFromWAV(const char *Name)
{
	ALuint Buffer;

	eFILE *file = 0;
	file = vw_fopen(Name);
	if (file == 0) return 0;

	Buffer = alutCreateBufferFromFileImage(file->Data, file->RealLength);
	if (!CheckALUTError()) return 0;

	vw_fclose(file);


	// вот теперь создаем объект
	if (Buffer != 0)
	{
		eBuffer *TMPBuffer = 0;
		TMPBuffer = new eBuffer;
		vw_AttachBuffer(TMPBuffer);

		TMPBuffer->Name = new char[strlen(Name)+1]; if (TMPBuffer->Name == 0) return 0;
		strcpy(TMPBuffer->Name, Name);
		TMPBuffer->Buffer = Buffer;

		printf("Buffer ... %s\n", Name);
	}

	return Buffer;
}
//------------------------------------------------------------------------------------
// Удаляем буфер
//------------------------------------------------------------------------------------
void vw_ReleaseBuffer(eBuffer *Buffer)
{
	if (Buffer == 0) return;

	vw_DetachBuffer(Buffer);

	if (Buffer->Name != 0){delete [] Buffer->Name; Buffer->Name = 0;}

	if (Buffer->Buffer != 0){alDeleteBuffers(1, &Buffer->Buffer); Buffer->Buffer = 0;}
	alGetError(); // сброс ошибок

	delete Buffer; Buffer = 0;
}



eBuffer *vw_FindBufferByName(const char *Name)
{
	eBuffer *Tmp = StartBufferMan;

	while (Tmp != 0)
	{
		eBuffer *Tmp1 = Tmp->Next;
		if(vw_strcmp(Tmp->Name, Name) == 0) return Tmp;
		Tmp = Tmp1;
	}

	return 0;
}

ALuint vw_FindBufferIDByName(const char *Name)
{
	eBuffer *Tmp = StartBufferMan;

	while (Tmp != 0)
	{
		eBuffer *Tmp1 = Tmp->Next;
		if(vw_strcmp(Tmp->Name, Name) == 0) return Tmp->Buffer;
		Tmp = Tmp1;
	}

	return 0;
}

void vw_ReleaseAllBuffers()
{
	// Чистка памяти...
	eBuffer *Tmp = StartBufferMan;
	while (Tmp != 0)
	{
		eBuffer *Tmp1 = Tmp->Next;
		vw_ReleaseBuffer(Tmp);
		Tmp = Tmp1;
	}

	StartBufferMan = 0;
	EndBufferMan = 0;
}

void vw_AttachBuffer(eBuffer* Buffer)
{
	if (Buffer == 0) return;

	// первый в списке...
	if (EndBufferMan == 0)
	{
		Buffer->Prev = 0;
		Buffer->Next = 0;
		StartBufferMan = Buffer;
		EndBufferMan = Buffer;
	}
	else // продолжаем заполнение...
	{
		Buffer->Prev = EndBufferMan;
		Buffer->Next = 0;
		EndBufferMan->Next = Buffer;
		EndBufferMan = Buffer;
	}
}

void vw_DetachBuffer(eBuffer* Buffer)
{
	if (Buffer == 0) return;

	// переустанавливаем указатели...
	if (StartBufferMan == Buffer) StartBufferMan = Buffer->Next;
	if (EndBufferMan == Buffer) EndBufferMan = Buffer->Prev;

	if (Buffer->Next != 0) Buffer->Next->Prev = Buffer->Prev;
		else if (Buffer->Prev != 0) Buffer->Prev->Next = 0;
	if (Buffer->Prev != 0) Buffer->Prev->Next = Buffer->Next;
		else if (Buffer->Next != 0) Buffer->Next->Prev = 0;
}
