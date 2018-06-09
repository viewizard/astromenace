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
Note, we use view size instead of window size (or display size),
in the meaning of game's rendering area size.
For fullscreen this will be also window size (and display size),
for windowed mode this will be internal size that don't include
window's decor size.

Highly important to care about aspect ratio, only standard (4:3 and 5:4)
and wide (16:9 and 16:10) view size are supported now.
Note, for more wide aspect ratio support, all missions should be
revised first in order to prevent objects creation in the visible
part of scene.
In real, we allow any view size with aspect ratio from 5:4 (1.25) to 16:9 (1.77).
*/

namespace {

// Default index of the display to query. Make sure, that you clear all related arrays
// if this index changed during session, since we use cached data for more speed.
int DisplayIndex{0};

std::vector<sViewSize> FullScreenSizeArray{};
std::vector<sViewSize> WindowSizeArray{};

} // unnamed namespace


/*
 * Check for standard aspect ratio.
 */
bool StandardAspectRation(const sViewSize &ViewSize)
{
	float tmpAspectRatio = static_cast<float>(ViewSize.Width) / static_cast<float>(ViewSize.Height);

	if ((tmpAspectRatio > 1.24f) &&
	    (tmpAspectRatio < 1.4f))
		return true;

	return false;
}

/*
 * Check view size's aspect ratio.
 */
static bool AllowedAspectRatio(const sViewSize &ViewSize)
{
	float tmpAspectRatio = static_cast<float>(ViewSize.Width) / static_cast<float>(ViewSize.Height);

	// only aspect ratio from 5:4 to 16:9 are allowed
	if ((tmpAspectRatio > 1.24f) &&
	    (tmpAspectRatio < 1.78f))
		return true;

	return false;
}

/*
 * Get display size.
 */
static bool GetDisplaySize(sViewSize &ViewSize)
{
	SDL_Rect DisplayBounds;
	if (SDL_GetDisplayBounds(DisplayIndex, &DisplayBounds) == 0) {
		ViewSize.Width = DisplayBounds.w;
		ViewSize.Height = DisplayBounds.h;
		return true;
	} else
		std::cerr << __func__ << "(): " << "SDL_GetDisplayBounds() failed: " << SDL_GetError() << "\n";

	SDL_DisplayMode DisplayMode;
	if (SDL_GetDesktopDisplayMode(DisplayIndex, &DisplayMode) == 0) {
		ViewSize.Width = DisplayMode.w;
		ViewSize.Height = DisplayMode.h;
		return true;
	} else
		std::cerr << __func__ << "(): " << "SDL_GetDesktopDisplayMode() failed: " << SDL_GetError() << "\n";

	return false;
}

/*
 * Detect current display size for fullscreen mode.
 * Note, we work with one display only now.
 * If current display size is not appropriate, returned vector is empty.
 */
std::vector<sViewSize> &DetectFullScreenSize()
{
	if (!FullScreenSizeArray.empty())
		return FullScreenSizeArray;

	sViewSize tmpViewSize;
	if (!GetDisplaySize(tmpViewSize))
		return FullScreenSizeArray; // return empty vector

	if (!AllowedAspectRatio(tmpViewSize))
		return FullScreenSizeArray; // return empty vector

	FullScreenSizeArray.emplace_back(tmpViewSize);
	return FullScreenSizeArray;
}

/*
 * Get display usable size.
 */
static bool GetDisplayUsableSize(sViewSize &ViewSize)
{
	SDL_Rect DisplayUsableBounds;
	if (SDL_GetDisplayUsableBounds(DisplayIndex, &DisplayUsableBounds) == 0) {
		ViewSize.Width = DisplayUsableBounds.w;
		ViewSize.Height = DisplayUsableBounds.h;
		return true;
	} else
		std::cerr << __func__ << "(): " << "SDL_GetDisplayUsableBounds() failed: " << SDL_GetError() << "\n";

	// fallback
	return GetDisplaySize(ViewSize);
}

/*
 * Detect allowed view size array for windowed mode.
 */
std::vector<sViewSize> &DetectWindowSizeArray()
{
	if (!WindowSizeArray.empty())
		return WindowSizeArray;

	// we need array of "well known" sizes, so, I did not find anything better,
	// than use more or less popular resolutions as default array
	static const std::vector<sViewSize> DefaultWindowSizeArray{
		{640, 480},
		{768, 480},
		{800, 480},
		{800, 600},
		{832, 624},
		{854, 480},
		{960, 540},
		{960, 544},
		{960, 640},
		{960, 720},
		{1024, 576},
		{1024, 600},
		{1024, 640},
		{1024, 768},
		{1024, 800},
		{1120, 832},
		{1152, 720},
		{1152, 768},
		{1152, 864},
		{1152, 900},
		{1280, 720},
		{1280, 768},
		{1280, 800},
		{1280, 854},
		{1280, 960},
		{1280, 1024},
		{1360, 768},
		{1366, 768},
		{1400, 1050},
		{1440, 900},
		{1440, 960},
		{1440, 1024},
		{1440, 1080},
		{1536, 864},
		{1600, 900},
		{1600, 1024},
		{1600, 1200},
		{1680, 1050},
		{1792, 1344},
		{1856, 1392},
		{1800, 1440},
		{1920, 1080},
		{1920, 1200},
		{1920, 1400},
		{1920, 1440},
		{2048, 1280},
		{2048, 1536},
		{2304, 1440},
		{2304, 1728},
		{2560, 1440},
		{2560, 1600},
		{2560, 1920},
		{2560, 2048},
		{2800, 2100},
		{2880, 1800},
		{3200, 2048},
		{3200, 2400},
		{3840, 2160},
		{3840, 2400},
		{4096, 2304},
		{4096, 3072},
		{5120, 3200},
		{5120, 4096},
		{6400, 4096},
		{6400, 4800},
		{7680, 4320},
		{7680, 4800},
	};

	sViewSize DisplayUsableSize;
	if (!GetDisplayUsableSize(DisplayUsableSize))
		return WindowSizeArray; // return empty vector

	// we have DefaultWindowSizeArray, but we need sizes less then display usable bounds only
	auto UnaryPredicate = [&DisplayUsableSize] (const sViewSize &a) {
		return ((a.Width >= DisplayUsableSize.Width) ||
			(a.Height >= DisplayUsableSize.Height));
	};
	// make sure std::back_inserter() are used, since we do not resize() our vector,
	// moreover, we don't even know what size we need
	std::remove_copy_if(DefaultWindowSizeArray.begin(), DefaultWindowSizeArray.end(),
			    std::back_inserter(WindowSizeArray), UnaryPredicate);

	// TODO probably, we also need SDL_GetDisplayMode() here, this is not exactly
	//      what we need, but in this way we could extend the array

	return WindowSizeArray;
}
