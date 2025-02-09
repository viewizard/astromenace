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

#include "../math/math.h"
#include "SDL2/SDL.h"

namespace viewizard {

namespace {

std::unordered_map<int, bool> KeyStatus{};
// unicode character for current pressed button
std::u32string CurrentUnicodeChar{};

} // unnamed namespace


/*
 * Get key status (pressed or not).
 */
bool vw_GetKeyStatus(int Key)
{
    if (KeyStatus.find(Key) == KeyStatus.end())
        return false;

    return KeyStatus[Key];
}

/*
 * Set key status released.
 */
void vw_SetKeyReleased(int Key)
{
    KeyStatus[Key] = false;
}

/*
 * Key status update. Should be called on SDL_KEYUP and SDL_KEYDOWN events.
 */
void vw_KeyStatusUpdate(int Key, bool StatusDown)
{
    KeyStatus[Key] = StatusDown;
}

/*
 * Set current unicode (UTF8) character.
 */
void vw_SetCurrentUnicodeChar(const char *NewUnicodeChar)
{
    if (NewUnicodeChar) {
        CurrentUnicodeChar = ConvertUTF8.from_bytes(NewUnicodeChar);
    } else {
        CurrentUnicodeChar.clear();
    }
}

/*
 * Get current unicode (UTF32) character.
 */
const std::u32string &vw_GetCurrentUnicodeChar()
{
    return CurrentUnicodeChar;
}

} // viewizard namespace
