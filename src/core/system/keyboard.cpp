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

#include "../math/math.h"

namespace {

// libSDL2 keystate array size
int KeyStateArraySize{0};
// unicode character for current pressed button
std::u32string CurrentUnicodeChar;

} // unnamed namespace


/*
 * Get key status (pressed or not).
 */
bool vw_GetKeyStatus(int Key)
{
	const uint8_t *KeyState = SDL_GetKeyboardState(&KeyStateArraySize);
	if (KeyState[SDL_GetScancodeFromKey(Key)])
		return true;

	return false;
}

/*
 * Set key status (pressed or released).
 */
void vw_SetKeyStatus(int Key, bool NewKeyStatus)
{
	uint8_t *KeyState = (uint8_t *)SDL_GetKeyboardState(&KeyStateArraySize);
	KeyState[SDL_GetScancodeFromKey(Key)] = NewKeyStatus;
}

/*
 * Get libSDL2 keystate array size.
 */
int vw_GetKeyStateArraySize()
{
	if (!KeyStateArraySize)
		SDL_GetKeyboardState(&KeyStateArraySize);

	return KeyStateArraySize;
}

/*
 * Set current unicode (UTF8) character.
 */
void vw_SetCurrentUnicodeChar(const char *NewUnicodeChar)
{
	if (NewUnicodeChar)
		CurrentUnicodeChar = ConvertUTF8.from_bytes(NewUnicodeChar);
	else
		CurrentUnicodeChar.clear();
}

/*
 * Get current unicode (UTF32) character.
 */
const std::u32string &vw_GetCurrentUnicodeChar()
{
	return CurrentUnicodeChar;
}
