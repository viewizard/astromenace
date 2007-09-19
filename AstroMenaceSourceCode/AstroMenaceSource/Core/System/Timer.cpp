/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Timer.cpp

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


#include "System.h"


// максимальное кол-во отслеживаемых веток времени
#define MAX_TIMETHREAD_COUNT	10


// текущий статус счета времени - считаем-нет
bool TimeCurrentStatus[MAX_TIMETHREAD_COUNT];
// последний подсчет тиков
Uint32 LastGetTicks[MAX_TIMETHREAD_COUNT];
// разница в тиках, чтобы учитывать простой или другие моменты
Uint32 DiffGetTicks[MAX_TIMETHREAD_COUNT];
// скорость 0-1  приращения, нужно для замедления-ускорение учета времени
float TimeThreadSpeed[MAX_TIMETHREAD_COUNT];
// накопление времени с другой соростью
float TimeThreadBuffer[MAX_TIMETHREAD_COUNT];


//------------------------------------------------------------------------------------
// инициализация внутреннего счета времени
//------------------------------------------------------------------------------------
void vw_InitTime()
{
	for (int i=0; i<MAX_TIMETHREAD_COUNT; i++)
	{
		TimeCurrentStatus[i] = true;
		LastGetTicks[i] = 0;
		DiffGetTicks[i] = 0;
		TimeThreadSpeed[i] = 1.0f;
		TimeThreadBuffer[i] = 0.0f;
	}
}
void vw_InitTimeNum(int Num)
{
	TimeCurrentStatus[Num] = true;
	LastGetTicks[Num] = 0;
	DiffGetTicks[Num] = 0;
	TimeThreadSpeed[Num] = 1.0f;
	TimeThreadBuffer[Num] = 0.0f;
}


//------------------------------------------------------------------------------------
// получение времени с начала запуска SDL
//------------------------------------------------------------------------------------
float vw_GetTime(int TimeThread)
{
	// если скорость не 1.0, нужно учитывать поправку
	if (TimeThreadSpeed[TimeThread] != 1.0f)
	{
		// находим время с учетом скорости
		float RealTimeThread = ((SDL_GetTicks() - DiffGetTicks[TimeThread])*TimeThreadSpeed[TimeThread])/1000.0f;
		// выдаем уже правильные данные
		return RealTimeThread + TimeThreadBuffer[TimeThread];
	}

	return TimeThreadBuffer[TimeThread] + (SDL_GetTicks() - DiffGetTicks[TimeThread])/1000.0f;
}




//------------------------------------------------------------------------------------
// старт счета времени
//------------------------------------------------------------------------------------
void vw_StartTime()
{
	for (int i=0; i<MAX_TIMETHREAD_COUNT; i++)
	if (!TimeCurrentStatus[i])
	{
		DiffGetTicks[i] += SDL_GetTicks() - LastGetTicks[i];
		TimeCurrentStatus[i] = true;
	}
}



//------------------------------------------------------------------------------------
// остановка счета времени
//------------------------------------------------------------------------------------
void vw_StopTime()
{
	for (int i=0; i<MAX_TIMETHREAD_COUNT; i++)
	if (TimeCurrentStatus[i])
	{
		LastGetTicks[i] = SDL_GetTicks();
		TimeCurrentStatus[i] = false;
	}
}


//------------------------------------------------------------------------------------
// установка текущей скорости приращения
//------------------------------------------------------------------------------------
void vw_SetTimeThreadSpeed(int TimeThread, float NewSpeed)
{
	if (NewSpeed > 2.0f) NewSpeed = 2.0f;
	if (NewSpeed < 0.0f) NewSpeed = 0.0f;

	// правка данных, чтобы все сходилось
	TimeThreadBuffer[TimeThread] += ((SDL_GetTicks() - DiffGetTicks[TimeThread])*TimeThreadSpeed[TimeThread])/1000.0f;
	DiffGetTicks[TimeThread] = SDL_GetTicks();
	TimeThreadSpeed[TimeThread] = NewSpeed;
}


//------------------------------------------------------------------------------------
// получение текущей скорости приращения
//------------------------------------------------------------------------------------
float vw_GetTimeThreadSpeed(int TimeThread)
{
	return TimeThreadSpeed[TimeThread];
}



