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

#include "../base.h"

// максимальное кол-во отслеживаемых веток времени
#define MAX_TIMETHREAD_COUNT 10


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
	for (int i=0; i<MAX_TIMETHREAD_COUNT; i++) {
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
	if (TimeThreadSpeed[TimeThread] != 1.0f) {
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
		if (!TimeCurrentStatus[i]) {
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
		if (TimeCurrentStatus[i]) {
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
