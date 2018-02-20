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

#include "sound.h"


//------------------------------------------------------------------------------------
// Структура буфера звука
//------------------------------------------------------------------------------------
struct eBuffer {
	char 		*Name;		// фактически имя файла из которого сделали буфер

	ALuint 		Buffer;

	eBuffer*	Prev;		// Pointer to the previous loaded Sound in the memory
	eBuffer*	Next;		// Pointer to the next loaded Sound in the memory
};


//------------------------------------------------------------------------------------
// Функции для работы с буфером
//------------------------------------------------------------------------------------
ALuint vw_CreateSoundBufferFromWAV(const char *Name);
ALuint vw_CreateSoundBufferFromOGG(const char *Name);
void vw_ReleaseBuffer(eBuffer *Buffer);

eBuffer *vw_FindBufferByName(const char *Name);
ALuint vw_FindBufferIDByName(const char *Name);

void vw_ReleaseAllBuffers();
void vw_AttachBuffer(eBuffer* Buffer);
void vw_DetachBuffer(eBuffer* Buffer);





#endif // Buffer_H
