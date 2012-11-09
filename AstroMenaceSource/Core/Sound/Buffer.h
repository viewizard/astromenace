/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Buffer.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#ifndef Buffer_H
#define Buffer_H

#include "Sound.h"


//------------------------------------------------------------------------------------
// Структура буфера звука
//------------------------------------------------------------------------------------
struct eBuffer
{
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
