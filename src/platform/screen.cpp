/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../core/base.h"
#include "platform.h"

/*
Note, we use view size instead of window size (or screen size),
in the meaning of game's rendering rectangle size.
For fullscreen this will be also window size (and screen size),
for windowed mode this will be internal size that don't include
window's decor size.

Highly important to care about aspect ratio, only standard (4:3 and 5:4)
and wide (16:9 and 16:10) view size are supported now.
Note, for more wide aspect ratio support, all missions should be
revised first in order to prevent objects creation in the visible
part of scene.
In real, we allow any view size with aspect ratio from 5:4 to 16:9.
*/

namespace {

// Default screen number. Make sure, you clear all related arrays on
// change it during one session, since we use cached data for more speed.
int ScreenNumber{0};

std::vector<sViewSize> FullScreenSizeArray{};

} // unnamed namespace


/*
 * Check view size's aspect ratio.
 */
static bool AllowedAspectRatio(const sViewSize &ViewSize)
{
	float tmpAspectRatio = static_cast<float>(ViewSize.Width) / static_cast<float>(ViewSize.Height);

	// only aspect ratio from 5:4 to 16:9 are allowed
	if ((tmpAspectRatio > 1.24f) ||
	    (tmpAspectRatio < 1.78f))
		return true;

	return false;
}

/*
 * Detect current screen size for fullscreen mode.
 * Note, we work with one screen only now.
 * If current screen size is not appropriate, returned vector is empty.
 */
std::vector<sViewSize> &DetectFullScreenSize()
{
	if (!FullScreenSizeArray.empty())
		return FullScreenSizeArray;

	// get current screen size
	SDL_DisplayMode CurrentDisplayMode;
	if (SDL_GetDesktopDisplayMode(ScreenNumber, &CurrentDisplayMode) != 0) {
		std::cerr << __func__ << "(): " << "SDL_GetDesktopDisplayMode() failed: " << SDL_GetError() << "\n";
		return FullScreenSizeArray; // return empty vector
	}

	sViewSize tmpViewSize{CurrentDisplayMode.w, CurrentDisplayMode.h};
	if (!AllowedAspectRatio(tmpViewSize))
		return FullScreenSizeArray; // return empty vector

	FullScreenSizeArray.emplace_back(tmpViewSize);
	return FullScreenSizeArray;
}
