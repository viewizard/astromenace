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

#include "../base.h"

/*
 * Open website URL.
 */
bool vw_OpenWebsiteURL(const std::string &WebsiteURL)
{
#ifdef __unix

#elif defined(__APPLE__) && defined(__MACH__)
	CFURLRef OpenURL = CFURLCreateWithBytes (
				nullptr,
				(UInt8*)WebsiteURL.data(),
				WebsiteURL.size(),
				kCFStringEncodingASCII,
				nullptr);
	OSStatus rc = LSOpenCFURLRef(OpenURL, 0);
	CFRelease(OpenURL);
	if (rc == noErr)
		return true;
#elif defined(WIN32)
	int rc = reinterpret_cast<int>(ShellExecuteA(nullptr, "open", WebsiteURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL));
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb762153%28v=vs.85%29.aspx
	// Return value (HINSTANCE)
	// If the function succeeds, it returns a value greater than 32. If the function fails, it returns
	// an error value that indicates the cause of the failure.
	// It can be cast only to an int and compared to either 32 or the following error codes below.
	if (rc > 32)
		return true;
#endif

	return false;
}
