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

// TODO add support for SDL_INIT_HAPTIC (force feedback)

// NOTE SDL2 also provide SDL_INIT_GAMECONTROLLER now

/*
At this time we don't provide menu options to choose joystick, first one (with index 0) will be used by default.
Which Joystick should be used, could be configured via config file, "JoystickNum" parameter.
*/

#include "../core/core.h"
#include "../config/config.h"

namespace {

SDL_Joystick *Joystick{nullptr};
int JoystickAxisX{0};
int JoystickAxisY{0};
int JoystickButtonsQuantity{0};
std::vector<bool> JoystickButtons{};

float JoystickCurentTime{0.0f};
float JoystickTimeDelta{0.0f};

} // unnamed namespace


/*
 * Joystick initialization.
 */
bool JoystickInit(float InitialTime)
{
	if (SDL_NumJoysticks() <= 0)
		return false;

	std::cout << "Found Joystick(s):\n";
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		std::cout << "Joystick Name " << i << ": " << SDL_JoystickNameForIndex(i) << "\n";
	}

	if (GameConfig().JoystickNum >= SDL_NumJoysticks())
		ChangeGameConfig().JoystickNum = 0;

	Joystick = SDL_JoystickOpen(GameConfig().JoystickNum);

	if (Joystick) {
		std::cout << "Opened Joystick " << GameConfig().JoystickNum << "\n"
			  << "Joystick Name: " << SDL_JoystickNameForIndex(GameConfig().JoystickNum) << "\n"
			  << "Joystick Number of Axes: " << SDL_JoystickNumAxes(Joystick) << "\n"
			  << "Joystick Number of Buttons: " << SDL_JoystickNumButtons(Joystick) << "\n"
			  << "Joystick Number of Balls: " << SDL_JoystickNumBalls(Joystick) << "\n\n";
	} else {
		std::cout << "Couldn't open Joystick " << GameConfig().JoystickNum << "\n\n";
		return false;
	}

	JoystickCurentTime = InitialTime;
	JoystickTimeDelta = 0.0f;

	JoystickAxisX = SDL_JoystickGetAxis(Joystick, 0);
	JoystickAxisY = SDL_JoystickGetAxis(Joystick, 1);
	JoystickButtonsQuantity = SDL_JoystickNumButtons(Joystick);
	JoystickButtons.resize(JoystickButtonsQuantity, false);

	return true;
}

/*
 * Close joystick.
 */
void JoystickClose()
{
	if ((SDL_NumJoysticks() > 0) &&
	    Joystick &&
	    SDL_JoystickGetAttached(Joystick))
		SDL_JoystickClose(Joystick);
}

/*
 * Check current joystick status (opened or not).
 */
bool isJoystickAvailable()
{
	return Joystick;
}

/*
 * Get current opened joystick buttons quantity.
 */
int GetJoystickButtonsQuantity()
{
	if (!Joystick)
		return 0;

	return JoystickButtonsQuantity;
}

/*
 * Set joystick button status.
 */
void SetJoystickButton(int ButtonNumber, bool ButtonStatus)
{
	if (!Joystick ||
	    // we may have config that was made previously with another joystick
	    // don't change game's config, just ignore this buttons check
	    (ButtonNumber < 0) ||
	    (ButtonNumber >= JoystickButtonsQuantity))
		return;

	JoystickButtons[ButtonNumber] = ButtonStatus;
}

/*
 * Get joystick button status.
 */
bool GetJoystickButton(int ButtonNumber)
{
	if (!Joystick ||
	    // we may have config that was made previously with another joystick
	    // don't change game's config, just ignore this buttons check
	    (ButtonNumber < 0) ||
	    (ButtonNumber >= JoystickButtonsQuantity))
		return false;

	return JoystickButtons[ButtonNumber];
}

/*
 * Emulate mouse movements.
 */
void JoystickEmulateMouseMovement(float Time)
{
	if (!Joystick)
		return;

	JoystickTimeDelta = Time - JoystickCurentTime;
	JoystickCurentTime = Time;

	int X = SDL_JoystickGetAxis(Joystick, 0);
	int Y = SDL_JoystickGetAxis(Joystick, 1);

	// JoystickDeadZone: [0, 10]
	// min/max joystick axis value: -32768/32767
	// we are using 3000 here, since dead zone should not be same small/big as min/max joystick axis values
	if (abs(X) < (GameConfig().JoystickDeadZone * 3000))
		X = 0;
	if (abs(Y) < (GameConfig().JoystickDeadZone * 3000))
		Y = 0;

	if ((JoystickAxisX != X) || (JoystickAxisY != Y)) {
		JoystickAxisX = 0;
		JoystickAxisY = 0;

		// InternalWidth here, since during one second we should move cursor from left to right
		int Xsm{static_cast<int>(GameConfig().InternalWidth * (X / 32768.0f) * JoystickTimeDelta)};
		int Ysm{static_cast<int>(GameConfig().InternalWidth * (Y / 32768.0f) * JoystickTimeDelta)};

		vw_SetMousePosRel(Xsm, Ysm);
	}
}

/*
 * Provide joystick button's name - "ButtonN", where N is number.
 */
std::string JoystickButtonName(int ButtonNum)
{
	if (ButtonNum < 0)
		return "?";

	// ButtonNum + 1, since buttons index start from 0
	return vw_GetText("Button") + std::to_string(ButtonNum + 1);
}
