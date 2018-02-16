/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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


#include "../System.h"





const char * vw_VirtualCodeNameDE(int Num)
{
	switch (Num)
	{
		case SDLK_1: return "1";
		case SDLK_2: return "2";
		case SDLK_3: return "3";
		case SDLK_4: return "4";
		case SDLK_5: return "5";
		case SDLK_6: return "6";
		case SDLK_7: return "7";
		case SDLK_8: return "8";
		case SDLK_9: return "9";
		case SDLK_0: return "0";
		case SDLK_MINUS: return "Minus";
		case SDLK_EQUALS: return "Gleich";
		case SDLK_BACKSPACE: return "BackSpace";
		case SDLK_TAB: return "Tab";
		case SDLK_q: return "Q";
		case SDLK_w: return "W";
		case SDLK_e: return "E";
		case SDLK_r: return "R";
		case SDLK_t: return "T";
		case SDLK_y: return "Z";
		case SDLK_u: return "U";
		case SDLK_i: return "I";
		case SDLK_o: return "O";
		case SDLK_p: return "P";
		case SDLK_LEFTBRACKET: return "Ü";
		case SDLK_RIGHTBRACKET: return "+";
		case SDLK_RETURN: return "Enter";
		case SDLK_LCTRL: return "LeftCtrl";
		case SDLK_a: return "A";
		case SDLK_s: return "S";
		case SDLK_d: return "D";
		case SDLK_f: return "F";
		case SDLK_g: return "G";
		case SDLK_h: return "H";
		case SDLK_j: return "J";
		case SDLK_k: return "K";
		case SDLK_l: return "L";
		case SDLK_SEMICOLON: return "Ö";
		case SDLK_QUOTE: return "Ä";
		case SDLK_QUOTEDBL: return "Quotedbl";
		case SDLK_BACKQUOTE: return "BackQuote";
		case SDLK_LSHIFT: return "LeftShift";
		case SDLK_BACKSLASH: return "BackSlash";
		case SDLK_z: return "Y";
		case SDLK_x: return "X";
		case SDLK_c: return "C";
		case SDLK_v: return "V";
		case SDLK_b: return "B";
		case SDLK_n: return "N";
		case SDLK_m: return "M";
		case SDLK_COMMA: return "Comma";
		case SDLK_PERIOD: return "Period";
		case SDLK_SLASH: return "Minus";
		case SDLK_RSHIFT: return "RightShift";
		case SDLK_ASTERISK: return "Asterisk";
		case SDLK_LALT: return "LeftAlt";
		case SDLK_SPACE: return "Space";
	//	case SDLK_CAPSLOCK: return "CapsLock";
	//	case SDLK_NUMLOCK: return "NumLock";
	//	case SDLK_SCROLLOCK: return "Scroll";
		case SDLK_KP_MINUS: return "Minus";
		case SDLK_KP_MULTIPLY: return "Multiply";
		case SDLK_KP_PLUS: return "Plus";
		case SDLK_KP_PERIOD: return "Period";
		case SDLK_KP_ENTER: return "Enter";
		case SDLK_RCTRL: return "RightCtrl";
		case SDLK_KP_DIVIDE: return "Divide";
		case SDLK_RALT: return "RightAlt";
		case SDLK_PAUSE: return "Pause";
		case SDLK_HOME: return "Home";
		case SDLK_UP: return "Up";
		case SDLK_PAGEUP: return "PgUp";
		case SDLK_LEFT: return "Left";
		case SDLK_RIGHT: return "Right";
		case SDLK_END: return "End";
		case SDLK_DOWN: return "Down";
		case SDLK_PAGEDOWN: return "PgDn";
		case SDLK_INSERT: return "Insert";
		case SDLK_DELETE: return "Delete";
#ifdef use_SDL2
		case SDLK_KP_7: return "NumPad 7";
		case SDLK_KP_8: return "NumPad 8";
		case SDLK_KP_9: return "NumPad 9";
		case SDLK_KP_4: return "NumPad 4";
		case SDLK_KP_5: return "NumPad 5";
		case SDLK_KP_6: return "NumPad 6";
		case SDLK_KP_1: return "NumPad 1";
		case SDLK_KP_2: return "NumPad 2";
		case SDLK_KP_3: return "NumPad 3";
		case SDLK_KP_0: return "NumPad 0";
		case SDLK_LGUI: return "LeftWin";
		case SDLK_RGUI: return "RightWin";
#else
		case SDLK_KP7: return "NumPad 7";
		case SDLK_KP8: return "NumPad 8";
		case SDLK_KP9: return "NumPad 9";
		case SDLK_KP4: return "NumPad 4";
		case SDLK_KP5: return "NumPad 5";
		case SDLK_KP6: return "NumPad 6";
		case SDLK_KP1: return "NumPad 1";
		case SDLK_KP2: return "NumPad 2";
		case SDLK_KP3: return "NumPad 3";
		case SDLK_KP0: return "NumPad 0";
		case SDLK_LSUPER: return "LeftWin";
		case SDLK_RSUPER: return "RightWin";
#endif
		// забой... чтобы не показывать, пока ищем
		case 0: return "?";
	}

	return 0;
}
