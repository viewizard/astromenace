/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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

*************************************************************************************/

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

#include "../core/base.h"
#include "platform.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

// Default index of the display to query. Make sure, that you clear all related arrays
// if this index changed during session, since we use cached data for more speed.
int DisplayIndex{0};

std::vector<sViewSize> FullscreenSizeArray{};
std::vector<sViewSize> WindowSizeArray{};

} // unnamed namespace


/*
 * Change index of the display to query.
 */
void ChangeDisplayIndex(int NewDisplayIndex)
{
	if (DisplayIndex == NewDisplayIndex)
		return;

	// prevent out of range index usage
	int DisplaysCount = SDL_GetNumVideoDisplays();
	if (DisplaysCount >= 1) {
		if (NewDisplayIndex >= DisplaysCount)
			NewDisplayIndex = 0; // fallback to first display
	} else
		std::cerr << __func__ << "(): " << "SDL_GetNumVideoDisplays() failed: " << SDL_GetError() << "\n";

	DisplayIndex = NewDisplayIndex;
	FullscreenSizeArray.clear();
	WindowSizeArray.clear();
}

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
const std::vector<sViewSize> &DetectFullscreenSize()
{
	if (!FullscreenSizeArray.empty())
		return FullscreenSizeArray;

	sViewSize tmpViewSize;
	if (!GetDisplaySize(tmpViewSize))
		return FullscreenSizeArray; // return empty vector

	if (!AllowedAspectRatio(tmpViewSize))
		return FullscreenSizeArray; // return empty vector

	FullscreenSizeArray.emplace_back(tmpViewSize);
	return FullscreenSizeArray;
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
const std::vector<sViewSize> &DetectWindowSizeArray()
{
	if (!WindowSizeArray.empty())
		return WindowSizeArray;

	// we need array of "well known" sizes, so, I did not find anything better,
	// than use more or less popular resolutions as default array
	static const std::vector<sViewSize> DefaultWindowSizeArray{
		sViewSize{640, 480},
		sViewSize{768, 480},
		sViewSize{800, 480},
		sViewSize{800, 600},
		sViewSize{832, 624},
		sViewSize{854, 480},
		sViewSize{960, 540},
		sViewSize{960, 544},
		sViewSize{960, 640},
		sViewSize{960, 720},
		sViewSize{1024, 576},
		sViewSize{1024, 600},
		sViewSize{1024, 640},
		sViewSize{1024, 768},
		sViewSize{1024, 800},
		sViewSize{1120, 832},
		sViewSize{1152, 720},
		sViewSize{1152, 768},
		sViewSize{1152, 864},
		sViewSize{1152, 900},
		sViewSize{1280, 720},
		sViewSize{1280, 768},
		sViewSize{1280, 800},
		sViewSize{1280, 854},
		sViewSize{1280, 960},
		sViewSize{1280, 1024},
		sViewSize{1360, 768},
		sViewSize{1366, 768},
		sViewSize{1400, 1050},
		sViewSize{1440, 900},
		sViewSize{1440, 960},
		sViewSize{1440, 1024},
		sViewSize{1440, 1080},
		sViewSize{1536, 864},
		sViewSize{1600, 900},
		sViewSize{1600, 1024},
		sViewSize{1600, 1200},
		sViewSize{1680, 1050},
		sViewSize{1792, 1344},
		sViewSize{1856, 1392},
		sViewSize{1800, 1440},
		sViewSize{1920, 1080},
		sViewSize{1920, 1200},
		sViewSize{1920, 1400},
		sViewSize{1920, 1440},
		sViewSize{2048, 1280},
		sViewSize{2048, 1536},
		sViewSize{2304, 1440},
		sViewSize{2304, 1728},
		sViewSize{2560, 1440},
		sViewSize{2560, 1600},
		sViewSize{2560, 1920},
		sViewSize{2560, 2048},
		sViewSize{2800, 2100},
		sViewSize{2880, 1800},
		sViewSize{3200, 2048},
		sViewSize{3200, 2400},
		sViewSize{3840, 2160},
		sViewSize{3840, 2400},
		sViewSize{4096, 2304},
		sViewSize{4096, 3072},
		sViewSize{5120, 3200},
		sViewSize{5120, 4096},
		sViewSize{6400, 4096},
		sViewSize{6400, 4800},
		sViewSize{7680, 4320},
		sViewSize{7680, 4800}
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

} // astromenace namespace
} // viewizard namespace
