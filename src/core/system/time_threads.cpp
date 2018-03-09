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

/*
The idea of this code - time manipulations, what we need in the game. For example,
we could "pause" game by stop the time flowing, or just "slowdown" some objects.

This code provide "time threads" mechanism that allow manipulate time inside time
thread or even "stop" time inside time thread.

In common cases, if time manipulations feature not a goal, SDL_GetTicks()
should be preferred over then vw_GetTimeThread().
*/

#include "../base.h"
#include "../math/math.h"

namespace {

#define MAX_TIMETHREAD_COUNT 10
// current time thread status (active/paused)
bool TimeThreadStatus[MAX_TIMETHREAD_COUNT];
// last ticks, in order to care about time thread pause
uint32_t LastGetTicks[MAX_TIMETHREAD_COUNT];
// "time point", ticks of last speed change
uint32_t DiffGetTicks[MAX_TIMETHREAD_COUNT];
// current time thread speed
float TimeThreadSpeed[MAX_TIMETHREAD_COUNT];
// all "previous time" on last speed change
float TimeThreadBuffer[MAX_TIMETHREAD_COUNT];

} // unnamed namespace


/*
 * Initialize time for particular thread.
 */
void vw_InitTimeThread(int Num)
{
	TimeThreadStatus[Num] = true;
	LastGetTicks[Num] = 0;
	DiffGetTicks[Num] = 0;
	TimeThreadSpeed[Num] = 1.0f;
	TimeThreadBuffer[Num] = 0.0f;
}

/*
 * Initialize time threads.
 */
void vw_InitTimeThreads()
{
	for (int i = 0; i < MAX_TIMETHREAD_COUNT; i++) {
		vw_InitTimeThread(i);
	}
}

/*
 * Get time for particular thread.
 */
float vw_GetTimeThread(int TimeThread)
{
	// time manipulations
	if (TimeThreadSpeed[TimeThread] != 1.0f) {
		// calculate time from "time point" (DiffGetTicks), when speed was changed last time, till now
		float RealTimeThread = ((SDL_GetTicks() - DiffGetTicks[TimeThread]) * TimeThreadSpeed[TimeThread]) / 1000.0f;
		// add "previous time" from time buffer
		return RealTimeThread + TimeThreadBuffer[TimeThread];
	}

	return TimeThreadBuffer[TimeThread] + (SDL_GetTicks() - DiffGetTicks[TimeThread]) / 1000.0f;
}

/*
 * Start all time threads.
 */
void vw_StartTimeThreads()
{
	for (int i = 0; i < MAX_TIMETHREAD_COUNT; i++) {
		if (!TimeThreadStatus[i]) {
			DiffGetTicks[i] += SDL_GetTicks() - LastGetTicks[i];
			TimeThreadStatus[i] = true;
		}
	}
}

/*
 * Stop all time threads.
 */
void vw_StopTimeThreads()
{
	for (int i=0; i<MAX_TIMETHREAD_COUNT; i++) {
		if (TimeThreadStatus[i]) {
			LastGetTicks[i] = SDL_GetTicks();
			TimeThreadStatus[i] = false;
		}
	}
}

/*
 * Set time thread speed for particular thread.
 */
void vw_SetTimeThreadSpeed(int TimeThread, float NewSpeed)
{
	vw_Clamp(NewSpeed, 0.0f, 2.0f);

	// store "previous time" in the time buffer
	TimeThreadBuffer[TimeThread] += ((SDL_GetTicks() - DiffGetTicks[TimeThread]) * TimeThreadSpeed[TimeThread]) / 1000.0f;
	// store "time point", when speed was changed
	DiffGetTicks[TimeThread] = SDL_GetTicks();
	TimeThreadSpeed[TimeThread] = NewSpeed;
}

/*
 * Get time thread speed for particular thread.
 */
float vw_GetTimeThreadSpeed(int TimeThread)
{
	return TimeThreadSpeed[TimeThread];
}
