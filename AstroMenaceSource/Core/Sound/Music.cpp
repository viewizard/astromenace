/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "Sound.h"
#include "../System/System.h"



ALboolean CheckALError();
ALboolean CheckALUTError();




//------------------------------------------------------------------------------------
//	Vorbis callback functions
//------------------------------------------------------------------------------------
size_t VorbisRead(void *ptr, size_t byteSize, size_t sizeToRead, void *datasource)
{
	// нужно возвращать кол-во прочитаных byteSize
	eFILE* vorbisData = (eFILE*)datasource;
	return vorbisData->fread(ptr, byteSize, sizeToRead);
}


int VorbisSeek(void *datasource, ogg_int64_t offset, int whence)
{
	eFILE* vorbisData = (eFILE*)datasource;
	return vorbisData->fseek(offset, whence);
}


int VorbisClose(void *UNUSED(datasource))
{// похоже, без этой функции у линукс версии vorbis проблемы, хотя по документации...
	return 1;
}


long VorbisTell(void *datasource)
{
	eFILE*	vorbisData = (eFILE*)datasource;
	return vorbisData->ftell();
}





//------------------------------------------------------------------------------------
// Читаем фрагмент ogg файла
//------------------------------------------------------------------------------------
bool eMusic::ReadOggBlock(ALuint BufID, size_t Size)
{
	// vars
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
// Проигрывание звука
//------------------------------------------------------------------------------------
bool eMusic::Play(const char * Name, float fVol, float fMainVol, bool Loop, const char *LoopFileName)
{
	Volume = fVol;
	MainVolume = fMainVol;

	if (LoopFileName == 0)
	{
		LoopPart[0] = 0;
	}
	else
	{
		strcpy(LoopPart, LoopFileName);
	}

	FadeInSw = false;
	FadeInStartVol = Volume;
	FadeInEndVol = Volume;
	FadeOutSw = false;
	FadeTime = 0.0f;
	FadeAge = 0.0f;
	LastTime = vw_GetTime();
	mVF = 0;

	// Position of the source sound.
	ALfloat SourcePos[] = { 0.0f, 0.0f, 0.0f }; // -1.0 1.0 по иксам это баланс
	// Velocity of the source sound.
	ALfloat SourceVel[] = { 0.0f, 0.0f, 0.0f };

	alGenSources(1, &Source);
	if(!CheckALError()) return AL_FALSE;

	alSourcef (Source, AL_PITCH,    1.0      ); // 1.0 only!!!
	alSourcef (Source, AL_GAIN,     fVol*fMainVol	 ); // фактически громкость
	alSourcefv(Source, AL_POSITION, SourcePos);
	alSourcefv(Source, AL_VELOCITY, SourceVel);
	alSourcei (Source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcei(Source, AL_LOOPING, AL_FALSE);
	alGetError(); // сброс ошибок
	Looped = Loop;


	// делаем буфер...

	// Open Ogg file
	MusicFile = vw_fopen(Name);
	if (MusicFile == 0) return false;

	int	 i, DynBuffs = 1, BlockSize;

	// OggVorbis specific structures
	ov_callbacks	cb;
	// Fill cb struct
	cb.close_func	= VorbisClose;
	cb.read_func	= VorbisRead;
	cb.seek_func	= VorbisSeek;
	cb.tell_func	= VorbisTell;
	// Create OggVorbis_File struct
	mVF = new OggVorbis_File;

	// Generate local buffers
	if (ov_open_callbacks(MusicFile, mVF, NULL, 0, cb) < 0)
	{
		// This is not ogg bitstream. Return
		return false;
	}



	// Check for existance of sound
	BlockSize	= DYNBUF_SIZE;
	DynBuffs	= NUM_OF_DYNBUF;

	// Return vorbis_comment and vorbis_info structures
	mComment		= ov_comment(mVF, -1);
	mInfo			= ov_info(mVF, -1);

	// Fill buffer infos
	Rate		= mInfo->rate;
	Format 		= (mInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	// Create buffers
	alGenBuffers(DynBuffs, Buffers);
	if (!CheckALError()) return false;

	for (i = 0; i < DynBuffs; i++)
	{
		// Read amount (DYNBUF_SIZE) data into each buffer
		ReadOggBlock(Buffers[i], BlockSize);
		if (!CheckALError()) return false;

		alSourceQueueBuffers(Source, 1, &Buffers[i]);
		if (!CheckALError()) return false;
	}




	// запускаем на проигрывание

	alSourcePlay(Source);
	if(!CheckALError()) return AL_FALSE;

	return AL_TRUE;
}



//------------------------------------------------------------------------------------
// обновление потока
//------------------------------------------------------------------------------------
bool eMusic::Update()
{
	int	 Processed = 0;

	alGetSourcei(Source, AL_BUFFERS_PROCESSED, &Processed);

	// We still have processed buffers
	while (Processed--)
	{
		// не создаем новые буферы, просто берем уже использованный и заполняем новыми данными
		// а потом ставим его опять в очередь
		ALuint bufferID;
		alSourceUnqueueBuffers(Source, 1, &bufferID);
		if (!CheckALError()) return true;

		if (ReadOggBlock(bufferID, DYNBUF_SIZE) != 0)
		{
			alSourceQueueBuffers(Source, 1, &bufferID);
			if (!CheckALError()) return true;
		}
		else
		{

			if (LoopPart[0] != 0)
			{
				if (mVF != 0){ov_clear(mVF);delete mVF; mVF = 0;}
				if (MusicFile != 0)
				{
					vw_fclose(MusicFile);
				}

				// Open Ogg file
				MusicFile = vw_fopen(LoopPart);
				if (MusicFile != 0)
				{
					// OggVorbis specific structures
					ov_callbacks	cb;
					// Fill cb struct
					cb.close_func	= VorbisClose;
					cb.read_func	= VorbisRead;
					cb.seek_func	= VorbisSeek;
					cb.tell_func	= VorbisTell;
					// Create OggVorbis_File struct
					mVF = new OggVorbis_File;

					// Generate local buffers
					if (ov_open_callbacks(MusicFile, mVF, NULL, 0, cb) < 0)
					{
						// This is not ogg bitstream. Return
						return false;
					}

				}
				else return false;

				Looped = true;
				LoopPart[0] = 0;
			}


			// если не нужно больше повторять - выходим
			if (!Looped) return false;

			// устанавливаем на начало
			ov_pcm_seek(mVF, 0);
			alSourceQueueBuffers(Source, 1, &bufferID);
			if (!CheckALError()) return true;

		}
	}


	// обрабатываем эффекты
	float TimeDelta = vw_GetTime() - LastTime;

	if (FadeInSw)
	if (Volume < FadeInEndVol)
	{
		FadeTime += TimeDelta;
		Volume = FadeInEndVol*(FadeTime/FadeAge);
		alSourcef(Source, AL_GAIN, MainVolume*Volume );
		alGetError(); // сброс ошибок
	}

	if (FadeOutSw)
	if (Volume > 0.0f)
	{
		FadeTime += TimeDelta;
		Volume = FadeInStartVol*((FadeAge-FadeTime)/FadeAge);
		alSourcef(Source, AL_GAIN, MainVolume*Volume);
		alGetError(); // сброс ошибок
		if (Volume <= 0.0f) return false;
	}

	LastTime = vw_GetTime();



	// если по какой-то причине перестали играть...
	ALint TMPS;
	alGetSourcei(Source, AL_SOURCE_STATE, &TMPS);
	alGetError(); // сброс ошибок
	if (TMPS == AL_STOPPED) return false;

	return true;
}





//------------------------------------------------------------------------------------
// установка громкости
//------------------------------------------------------------------------------------
void eMusic::SetMainVolume(float NewMainVolume)
{
	// если это не источник, уходим
	if (!alIsSource(Source)) return;

	MainVolume = NewMainVolume;
	alSourcef(Source, AL_GAIN,     MainVolume*Volume );
}



//------------------------------------------------------------------------------------
// плавное появление
//------------------------------------------------------------------------------------
void eMusic::FadeIn(float EndVol, float Time)
{
	FadeInSw = true;
	FadeOutSw = false;
	FadeInEndVol = EndVol;
	FadeTime = 0.0f; // начинаем с нуля!
	FadeAge = Time;
	LastTime = vw_GetTime();
}



//------------------------------------------------------------------------------------
// плавное уход на 0 с текущего
//------------------------------------------------------------------------------------
void eMusic::FadeOut(float Time)
{
	FadeOutSw = true;
	FadeInSw = false;
	FadeInStartVol = Volume;
	FadeTime = 0.0f; // начинаем с нуля!
	FadeAge = Time;
	LastTime = vw_GetTime();
}




