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

In common cases, if time manipulations feature not a goal, SDL_GetTicks() provide
same functionality.
*/

// TODO the main idea of time calculation should be revised for discrete time
// Since we don't know how expensive SDL_GetTicks() is (on different platforms),
// no reason call SDL_GetTicks() all the time for animation/gfx. One time update
// for thread peer loop cycle should be enough.
// * see UpdateAllObject3D() - we already use this idea for all objects update.
// (!) make sure, all vw_GetTimeThread() usage changed to SDL_GetTicks() where
// it is necessary.

#include "../base.h"
#include "../math/math.h"

namespace {

struct sTimeThread {
	// current time thread status (active/paused)
	bool Status{false};
	// last ticks, in order to care about time thread pause
	uint32_t LastGetTicks{0};
	// "time point", ticks of last speed change
	uint32_t DiffGetTicks{0};
	// current time thread speed
	float Speed{1.0f};
	// all "previous time" on last speed change
	float Buffer{0.0f};
};

// std::unordered_map - caller could use any integer thread number (negative also
// allowed), we need container with operator [], that create new element on access
// and could use any integer number as index.
std::unordered_map<int, sTimeThread> TimeThreadsMap;

} // unnamed namespace


/*
 * Initialize or re-initialize time for particular thread.
 */
void vw_InitTimeThread(int TimeThread)
{
	TimeThreadsMap[TimeThread].Status = true;
	TimeThreadsMap[TimeThread].LastGetTicks = 0;
	TimeThreadsMap[TimeThread].DiffGetTicks = 0;
	TimeThreadsMap[TimeThread].Speed = 1.0f;
	TimeThreadsMap[TimeThread].Buffer = 0.0f;
}

/*
 * Get time for particular thread.
 */
float vw_GetTimeThread(int TimeThread)
{
	if (TimeThreadsMap.find(TimeThread) == TimeThreadsMap.end()) {
		std::cerr << "TimeThread was not initialized: " << TimeThread << "\n";
		return 0.0f;
	}

	// time manipulations
	if (TimeThreadsMap[TimeThread].Speed != 1.0f) {
		// calculate time from "time point" (DiffGetTicks), when speed was changed last time, till now
		float RealTimeThread = ((SDL_GetTicks() - TimeThreadsMap[TimeThread].DiffGetTicks) * TimeThreadsMap[TimeThread].Speed) / 1000.0f;
		// add "previous time" from time buffer
		return RealTimeThread + TimeThreadsMap[TimeThread].Buffer;
	}

	return TimeThreadsMap[TimeThread].Buffer + (SDL_GetTicks() - TimeThreadsMap[TimeThread].DiffGetTicks) / 1000.0f;
}

/*
 * Start all time threads.
 */
void vw_StartTimeThreads()
{
	for (auto &TimeThread : TimeThreadsMap) {
		if (!TimeThread.second.Status) {
			TimeThread.second.DiffGetTicks += SDL_GetTicks() - TimeThread.second.LastGetTicks;
			TimeThread.second.Status = true;
		}
	}
}

/*
 * Stop all time threads.
 */
void vw_StopTimeThreads()
{
	for (auto &TimeThread : TimeThreadsMap) {
		if (TimeThread.second.Status) {
			TimeThread.second.LastGetTicks = SDL_GetTicks();
			TimeThread.second.Status = false;
		}
	}
}

/*
 * Set time thread speed for particular thread.
 */
void vw_SetTimeThreadSpeed(int TimeThread, float NewSpeed)
{
	if (TimeThreadsMap.find(TimeThread) == TimeThreadsMap.end()) {
		std::cerr << "TimeThread was not initialized: " << TimeThread << "\n";
		return;
	}

	// store "previous time" in the time buffer
	TimeThreadsMap[TimeThread].Buffer += ((SDL_GetTicks() - TimeThreadsMap[TimeThread].DiffGetTicks) * TimeThreadsMap[TimeThread].Speed) / 1000.0f;
	// store "time point", when speed was changed
	TimeThreadsMap[TimeThread].DiffGetTicks = SDL_GetTicks();
	TimeThreadsMap[TimeThread].Speed = NewSpeed;
}

/*
 * Get time thread speed for particular thread.
 */
float vw_GetTimeThreadSpeed(int TimeThread)
{
	if (TimeThreadsMap.find(TimeThread) == TimeThreadsMap.end()) {
		std::cerr << "TimeThread was not initialized: " << TimeThread << "\n";
		return 0.0f;
	}

	return TimeThreadsMap[TimeThread].Speed;
}
