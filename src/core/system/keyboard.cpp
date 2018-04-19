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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../math/math.h"

namespace {

// libSDL2 keystate array size
int KeyStateArraySize{0};
// since we can't prevent SDL_KEYDOWN event repeats from hardware in libSDL2, we are
// forced to use additional array with locks in order to be sure, that we will not
// handle one real key press more that one time per press, just because keyboard repeats
// SDL_KEYDOWN every 30 ms
// KeyStatus[N] == true  - we can get "real" key status
// KeyStatus[N] == false - we locked "key released" manually by vw_SetKeyStatus() call
std::vector<bool> KeyStatus{};
// unicode character for current pressed button
std::u32string CurrentUnicodeChar{};

} // unnamed namespace


/*
 * Resize key status array.
 */
static void ResizeKeyStatus(unsigned int NeedStoreElement)
{
	// since we start from 0 (first element), in order to access
	// NeedStoreElement element, we need at least "NeedStoreElement + 1"
	// elements in KeyStatus array
	if (KeyStatus.size() + 1 < NeedStoreElement)
		KeyStatus.resize(NeedStoreElement + 1, true); // fill it with 'true' by default
}

/*
 * Get key status (pressed or not).
 */
bool vw_GetKeyStatus(int Key)
{
	const uint8_t *KeyState = SDL_GetKeyboardState(&KeyStateArraySize);
	ResizeKeyStatus(SDL_GetScancodeFromKey(Key));
	// handle "key down" only if KeyStatus[] also 'true', mean, we don't "key up"
	// it manually by vw_SetKeyStatus() call
	if (KeyState[SDL_GetScancodeFromKey(Key)] && KeyStatus[SDL_GetScancodeFromKey(Key)])
		return true;

	return false;
}

/*
 * Set key status (pressed or released).
 */
void vw_SetKeyStatus(int Key, bool NewKeyStatus)
{
	ResizeKeyStatus(SDL_GetScancodeFromKey(Key));
	KeyStatus[SDL_GetScancodeFromKey(Key)] = NewKeyStatus;
}

/*
 * Key status update. Should be called on SDL_KEYUP event.
 */
void vw_KeyStatusUpdate(int Key)
{
	const uint8_t *KeyState = SDL_GetKeyboardState(&KeyStateArraySize);
	ResizeKeyStatus(SDL_GetScancodeFromKey(Key));
	// reset KeyStatus[] to "true", since we have event SDL_KEYUP
	if (!KeyState[SDL_GetScancodeFromKey(Key)])
		KeyStatus[SDL_GetScancodeFromKey(Key)] = true;
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
