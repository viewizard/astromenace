/****************************************************************************

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

*****************************************************************************/

#include "../base.h"
#ifdef __unix
#include <unistd.h>
#include <sys/wait.h>
#endif // __unix
#if defined(__APPLE__) && defined(__MACH__)
#include <CoreFoundation/CFBundle.h>
#include <ApplicationServices/ApplicationServices.h>
#endif // __APPLE__

namespace viewizard {

/*
 * Open website URL.
 * Note, this is wrapper for ASCII and ANSI functions, don't use URL with Unicode.
 */
bool vw_OpenWebsiteURL(const std::string &WebsiteURL)
{
    if (WebsiteURL.empty()) {
        return false;
    }

#ifdef __unix
    // we might have a blocked process issue with our fork, in order to
    // avoid this issue, we exec xdg-open in second fork (double-fork)
    pid_t pid1 = fork();
    if (pid1 == 0) {
        pid_t pid2 = vfork();
        if (pid2 == 0) {
            execlp("xdg-open", "xdg-open", WebsiteURL.c_str(), nullptr);
            _exit(EXIT_FAILURE);
        } else if (pid2 < 0) {
            _exit(EXIT_FAILURE);
        } else {
            _exit(EXIT_SUCCESS);
        }
    } else if (pid1 > 0) {
        int status;
        if (waitpid(pid1, &status, 0) == pid1
            && WIFEXITED(status)
            && WEXITSTATUS(status) == 0) {
            return true;
        }
    }
#elif defined(__APPLE__) && defined(__MACH__)
    CFURLRef OpenURL = CFURLCreateWithBytes (
                           nullptr,
                           (UInt8*)WebsiteURL.data(),
                           WebsiteURL.size(),
                           kCFStringEncodingASCII,
                           nullptr);
    OSStatus rc = LSOpenCFURLRef(OpenURL, 0);
    CFRelease(OpenURL);
    if (rc == noErr) {
        return true;
    }
#elif defined(WIN32)
    int rc = reinterpret_cast<int>(ShellExecuteA(nullptr, "open", WebsiteURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL));
    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb762153%28v=vs.85%29.aspx
    // Return value (HINSTANCE)
    // If the function succeeds, it returns a value greater than 32. If the function fails, it returns
    // an error value that indicates the cause of the failure.
    // It can be cast only to an int and compared to either 32 or the following error codes below.
    if (rc > 32) {
        return true;
    }
#endif

    return false;
}

} // viewizard namespace
