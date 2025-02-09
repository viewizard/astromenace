/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (C) 2006-2025 Mikhail Kurinnoi, Viewizard


    AstroMenace is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AstroMenace is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

#ifndef CORE_TIMETHREAD_TIMETHREAD_H
#define CORE_TIMETHREAD_TIMETHREAD_H

#include "../base.h"

namespace viewizard {

// Initialize or re-initialize time for particular thread.
void vw_InitTimeThread(int TimeThread);
// Release all threads.
void vw_ReleaseAllTimeThread();
// Get time for particular thread.
float vw_GetTimeThread(int TimeThread);
// Pause all time threads.
void vw_PauseTimeThreads();
// Resume all previously paused time threads.
void vw_ResumeTimeThreads();
// Set time thread speed for particular thread.
void vw_SetTimeThreadSpeed(int TimeThread, float NewSpeed = 1.0f);

} // viewizard namespace

#endif // CORE_TIMETHREAD_TIMETHREAD_H
