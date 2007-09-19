/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: ru.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "../System.h"


// название кнопок
char RusChar[3];
char * vw_VirtualCodeNameRU(int Num)
{
	RusChar[0] = 0;
	RusChar[1] = 0;
	RusChar[2] = 0;

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
		case SDLK_EQUALS: return "Equals";
		case SDLK_BACKSPACE: return "BackSpace";
		case SDLK_TAB: return "Tab";
		case SDLK_q: RusChar[0] = 0xD0; RusChar[1] = 0x99; return RusChar; //'Й'
		case SDLK_w: RusChar[0] = 0xD0; RusChar[1] = 0xA6; return RusChar; //'Ц'
		case SDLK_e: RusChar[0] = 0xD0; RusChar[1] = 0xA3; return RusChar;//'У'
		case SDLK_r: RusChar[0] = 0xD0; RusChar[1] = 0x9A; return RusChar;//'К'
		case SDLK_t: RusChar[0] = 0xD0; RusChar[1] = 0x95; return RusChar;//'Е'
		case SDLK_y: RusChar[0] = 0xD0; RusChar[1] = 0x9D; return RusChar;//'Н'
		case SDLK_u: RusChar[0] = 0xD0; RusChar[1] = 0x93; return RusChar;//'Г'
		case SDLK_i: RusChar[0] = 0xD0; RusChar[1] = 0xA8; return RusChar;//'Ш'
		case SDLK_o: RusChar[0] = 0xD0; RusChar[1] = 0xA9; return RusChar;//'Щ'
		case SDLK_p: RusChar[0] = 0xD0; RusChar[1] = 0x97; return RusChar;//'З'
		case SDLK_LEFTBRACKET: RusChar[0] = 0xD0; RusChar[1] = 0xA5; return RusChar;//'Х'
		case SDLK_RIGHTBRACKET: RusChar[0] = 0xD0; RusChar[1] = 0xAA; return RusChar;//'Ъ'
		case SDLK_RETURN: return "Enter";
		case SDLK_LCTRL: return "LeftCtrl";
		case SDLK_a: RusChar[0] = 0xD0; RusChar[1] = 0xA4; return RusChar;//'Ф'
		case SDLK_s: RusChar[0] = 0xD0; RusChar[1] = 0xAB; return RusChar;//'Ы'
		case SDLK_d: RusChar[0] = 0xD0; RusChar[1] = 0x92; return RusChar;//'В'
		case SDLK_f: RusChar[0] = 0xD0; RusChar[1] = 0x90; return RusChar;//'А'
		case SDLK_g: RusChar[0] = 0xD0; RusChar[1] = 0x9F; return RusChar;//'П'
		case SDLK_h: RusChar[0] = 0xD0; RusChar[1] = 0xA0; return RusChar;//'Р'
		case SDLK_j: RusChar[0] = 0xD0; RusChar[1] = 0x9E; return RusChar;//'О'
		case SDLK_k: RusChar[0] = 0xD0; RusChar[1] = 0x9B; return RusChar;//'Л'
		case SDLK_l: RusChar[0] = 0xD0; RusChar[1] = 0x94; return RusChar;//'Д'
		case SDLK_SEMICOLON: RusChar[0] = 0xD0; RusChar[1] = 0x96; return RusChar;//'Ж'
		case SDLK_QUOTE: RusChar[0] = 0xD0; RusChar[1] = 0xAD; return RusChar;//'Э'
		case SDLK_QUOTEDBL: return "Quotedbl";
		case SDLK_BACKQUOTE: RusChar[0] = 0xD0; RusChar[1] = 0x81; return RusChar;//'Ё'
		case SDLK_LSHIFT: return "LeftShift";
		case SDLK_BACKSLASH: return "BackSlash";
		case SDLK_z: RusChar[0] = 0xD0; RusChar[1] = 0xAF; return RusChar;//'Я'
		case SDLK_x: RusChar[0] = 0xD0; RusChar[1] = 0xA7; return RusChar;//'Ч'
		case SDLK_c: RusChar[0] = 0xD0; RusChar[1] = 0xA1; return RusChar;//'С'
		case SDLK_v: RusChar[0] = 0xD0; RusChar[1] = 0x9C; return RusChar;//'М'
		case SDLK_b: RusChar[0] = 0xD0; RusChar[1] = 0x98; return RusChar;//'И'
		case SDLK_n: RusChar[0] = 0xD0; RusChar[1] = 0xA2; return RusChar;//'Т'
		case SDLK_m: RusChar[0] = 0xD0; RusChar[1] = 0xAC; return RusChar;//'Ь'
		case SDLK_COMMA: RusChar[0] = 0xD0; RusChar[1] = 0x91; return RusChar;//'Б';
		case SDLK_PERIOD: RusChar[0] = 0xD0; RusChar[1] = 0xAE; return RusChar;//'Ю';
		case SDLK_SLASH: return "Slash";
		case SDLK_RSHIFT: return "RightShift";
		case SDLK_ASTERISK: return "Asterisk";
		case SDLK_LALT: return "LeftAlt";
		case SDLK_SPACE: return "Space";
	//	case SDLK_CAPSLOCK: return "CapsLock";
	//	case SDLK_NUMLOCK: return "NumLock";
	//	case SDLK_SCROLLOCK: return "Scroll";
		case SDLK_KP7: return "NumPad 7";
		case SDLK_KP8: return "NumPad 8";
		case SDLK_KP9: return "NumPad 9";
		case SDLK_KP_MINUS: return "Minus";
		case SDLK_KP_MULTIPLY: return "Multiply";
		case SDLK_KP4: return "NumPad 4";
		case SDLK_KP5: return "NumPad 5";
		case SDLK_KP6: return "NumPad 6";
		case SDLK_KP_PLUS: return "Plus";
		case SDLK_KP1: return "NumPad 1";
		case SDLK_KP2: return "NumPad 2";
		case SDLK_KP3: return "NumPad 3";
		case SDLK_KP0: return "NumPad 0";
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
		case SDLK_LSUPER: return "LeftWin";
		case SDLK_RSUPER: return "RightWin";

		// забой... чтобы не показывать, пока ищем
		case 0: return "?";
	}

	return 0;
}














// значения кнопок клавиатуры



int VirtualCodeCharRU1(int Num)
{

	switch (Num)
	{
		case SDLK_BACKQUOTE: return 0xD081;//'Ё'
		case SDLK_1: return '!';
		case SDLK_2: return '"';
		case SDLK_3: return 'N';
		case SDLK_4: return ';';
		case SDLK_5: return '%';
		case SDLK_6: return ':';
		case SDLK_7: return '?';
		case SDLK_8: return '*';
		case SDLK_9: return '(';
		case SDLK_0: return ')';
		case SDLK_MINUS: return '_';
		case SDLK_EQUALS: return '+';
		case SDLK_q: return 0xD099;//'Й'
		case SDLK_w: return 0xD0A6;//'Ц'
		case SDLK_e: return 0xD0A3;//'У'
		case SDLK_r: return 0xD09A;//'К'
		case SDLK_t: return 0xD095;//'Е'
		case SDLK_y: return 0xD09D;//'Н'
		case SDLK_u: return 0xD093;//'Г'
		case SDLK_i: return 0xD0A8;//'Ш'
		case SDLK_o: return 0xD0A9;//'Щ'
		case SDLK_p: return 0xD097;//'З'
		case SDLK_LEFTBRACKET: return 0xD0A5;//'Х'
		case SDLK_RIGHTBRACKET: return 0xD0AA;//'Ъ'
		case SDLK_BACKSLASH: return '|';
		case SDLK_a: return 0xD0A4;//'Ф'
		case SDLK_s: return 0xD0AB;//'Ы'
		case SDLK_d: return 0xD092;//'В'
		case SDLK_f: return 0xD090;//'А'
		case SDLK_g: return 0xD09F;//'П'
		case SDLK_h: return 0xD0A0;//'Р'
		case SDLK_j: return 0xD09E;//'О'
		case SDLK_k: return 0xD09B;//'Л'
		case SDLK_l: return 0xD094;//'Д'
		case SDLK_SEMICOLON: return 0xD096;//'Ж'
		case SDLK_QUOTE: return 0xD0AD;//'Э'
		case SDLK_z: return 0xD0AF;//'Я'
		case SDLK_x: return 0xD0A7;//'Ч'
		case SDLK_c: return 0xD0A1;//'С'
		case SDLK_v: return 0xD09C;//'М'
		case SDLK_b: return 0xD098;//'И'
		case SDLK_n: return 0xD0A2;//'Т'
		case SDLK_m: return 0xD0AC;//'Ь'
		case SDLK_COMMA: return 0xD091;//'Б';
		case SDLK_PERIOD: return 0xD0AE;//'Ю';
		case SDLK_SLASH: return ',';
		case SDLK_ASTERISK: return '*';
		case SDLK_SPACE: return ' ';

		case SDLK_KP7: return '7';
		case SDLK_KP8: return '8';
		case SDLK_KP9: return '9';
		case SDLK_KP_MINUS: return '-';
		case SDLK_KP4: return '4';
		case SDLK_KP5: return '5';
		case SDLK_KP6: return '6';
		case SDLK_KP_PLUS: return '+';
		case SDLK_KP_MULTIPLY: return '*';
		case SDLK_KP1: return '1';
		case SDLK_KP2: return '2';
		case SDLK_KP3: return '3';
		case SDLK_KP0: return '0';
		case SDLK_KP_PERIOD: return '.';
		case SDLK_KP_DIVIDE: return '/';
	}

	return 0;
}

int VirtualCodeCharRU2(int Num)
{

	switch (Num)
	{
		case SDLK_BACKQUOTE: return 0xD191;//'ё'
		case SDLK_1: return '1';
		case SDLK_2: return '2';
		case SDLK_3: return '3';
		case SDLK_4: return '4';
		case SDLK_5: return '5';
		case SDLK_6: return '6';
		case SDLK_7: return '7';
		case SDLK_8: return '8';
		case SDLK_9: return '9';
		case SDLK_0: return '0';
		case SDLK_MINUS: return '-';
		case SDLK_EQUALS: return '=';
		case SDLK_q: return 0xD0B9;//'й'
		case SDLK_w: return 0xD186;//'ц'
		case SDLK_e: return 0xD183;//'у'
		case SDLK_r: return 0xD0BA;//'к'
		case SDLK_t: return 0xD0B5;//'е'
		case SDLK_y: return 0xD0BD;//'н'
		case SDLK_u: return 0xD0B3;//'г'
		case SDLK_i: return 0xD188;//'ш'
		case SDLK_o: return 0xD189;//'щ'
		case SDLK_p: return 0xD0B7;//'з'
		case SDLK_LEFTBRACKET: return 0xD185;//'х'
		case SDLK_RIGHTBRACKET: return 0xD18A;//'ъ'
		case SDLK_BACKSLASH: return '\\';
		case SDLK_a: return 0xD184;//'ф'
		case SDLK_s: return 0xD18B;//'ы'
		case SDLK_d: return 0xD0B2;//'в'
		case SDLK_f: return 0xD0B0;//'а'
		case SDLK_g: return 0xD0BF;//'п'
		case SDLK_h: return 0xD180;//'р'
		case SDLK_j: return 0xD0BE;//'о'
		case SDLK_k: return 0xD0BB;//'л'
		case SDLK_l: return 0xD0B4;//'д'
		case SDLK_SEMICOLON: return 0xD0B6;//'ж'
		case SDLK_QUOTE: return 0xD18D;//'э'
		case SDLK_z: return 0xD18F;//'я'
		case SDLK_x: return 0xD187;//'ч'
		case SDLK_c: return 0xD181;//'с'
		case SDLK_v: return 0xD0BC;//'м'
		case SDLK_b: return 0xD0B8;//'и'
		case SDLK_n: return 0xD182;//'т'
		case SDLK_m: return 0xD18C;//'ь'
		case SDLK_COMMA: return 0xD0B1;//'б'
		case SDLK_PERIOD: return 0xD18E;//'ю'
		case SDLK_SLASH: return '/';
		case SDLK_ASTERISK: return '*';
		case SDLK_SPACE: return ' ';

		case SDLK_KP7: return '7';
		case SDLK_KP8: return '8';
		case SDLK_KP9: return '9';
		case SDLK_KP_MINUS: return '-';
		case SDLK_KP4: return '4';
		case SDLK_KP5: return '5';
		case SDLK_KP6: return '6';
		case SDLK_KP_PLUS: return '+';
		case SDLK_KP_MULTIPLY: return '*';
		case SDLK_KP1: return '1';
		case SDLK_KP2: return '2';
		case SDLK_KP3: return '3';
		case SDLK_KP0: return '0';
		case SDLK_KP_PERIOD: return '.';
		case SDLK_KP_DIVIDE: return '/';
	}

	return 0;
}

int VirtualCodeCharRU3(int Num)
{

	switch (Num)
	{
		case SDLK_BACKQUOTE: return 0xD081;//'Ё'
		case SDLK_1: return '1';
		case SDLK_2: return '2';
		case SDLK_3: return '3';
		case SDLK_4: return '4';
		case SDLK_5: return '5';
		case SDLK_6: return '6';
		case SDLK_7: return '7';
		case SDLK_8: return '8';
		case SDLK_9: return '9';
		case SDLK_0: return '0';
		case SDLK_MINUS: return '-';
		case SDLK_EQUALS: return '=';
		case SDLK_q: return 0xD099;//'Й'
		case SDLK_w: return 0xD0A6;//'Ц'
		case SDLK_e: return 0xD0A3;//'У'
		case SDLK_r: return 0xD09A;//'К'
		case SDLK_t: return 0xD095;//'Е'
		case SDLK_y: return 0xD09D;//'Н'
		case SDLK_u: return 0xD093;//'Г'
		case SDLK_i: return 0xD0A8;//'Ш'
		case SDLK_o: return 0xD0A9;//'Щ'
		case SDLK_p: return 0xD097;//'З'
		case SDLK_LEFTBRACKET: return 0xD0A5;//'Х'
		case SDLK_RIGHTBRACKET: return 0xD0AA;//'Ъ'
		case SDLK_BACKSLASH: return '\\';
		case SDLK_a: return 0xD0A4;//'Ф'
		case SDLK_s: return 0xD0AB;//'Ы'
		case SDLK_d: return 0xD092;//'В'
		case SDLK_f: return 0xD090;//'А'
		case SDLK_g: return 0xD09F;//'П'
		case SDLK_h: return 0xD0A0;//'Р'
		case SDLK_j: return 0xD09E;//'О'
		case SDLK_k: return 0xD09B;//'Л'
		case SDLK_l: return 0xD094;//'Д'
		case SDLK_SEMICOLON: return 0xD096;//'Ж'
		case SDLK_QUOTE: return 0xD0AD;//'Э'
		case SDLK_z: return 0xD0AF;//'Я'
		case SDLK_x: return 0xD0A7;//'Ч'
		case SDLK_c: return 0xD0A1;//'С'
		case SDLK_v: return 0xD09C;//'М'
		case SDLK_b: return 0xD098;//'И'
		case SDLK_n: return 0xD0A2;//'Т'
		case SDLK_m: return 0xD0AC;//'Ь'
		case SDLK_COMMA: return 0xD091;//'Б';
		case SDLK_PERIOD: return 0xD0AE;//'Ю';
		case SDLK_SLASH: return '.';
		case SDLK_ASTERISK: return '*';
		case SDLK_SPACE: return ' ';

		case SDLK_KP7: return '7';
		case SDLK_KP8: return '8';
		case SDLK_KP9: return '9';
		case SDLK_KP_MINUS: return '-';
		case SDLK_KP4: return '4';
		case SDLK_KP5: return '5';
		case SDLK_KP6: return '6';
		case SDLK_KP_PLUS: return '+';
		case SDLK_KP_MULTIPLY: return '*';
		case SDLK_KP1: return '1';
		case SDLK_KP2: return '2';
		case SDLK_KP3: return '3';
		case SDLK_KP0: return '0';
		case SDLK_KP_PERIOD: return '.';
		case SDLK_KP_DIVIDE: return '/';
	}

	return 0;
}



int VirtualCodeCharRU4(int Num)
{

	switch (Num)
	{
		case SDLK_BACKQUOTE: return 0xD191;//'ё'
		case SDLK_1: return '!';
		case SDLK_2: return '"';
		case SDLK_3: return 'N';
		case SDLK_4: return ';';
		case SDLK_5: return '%';
		case SDLK_6: return ':';
		case SDLK_7: return '?';
		case SDLK_8: return '*';
		case SDLK_9: return '(';
		case SDLK_0: return ')';
		case SDLK_MINUS: return '_';
		case SDLK_EQUALS: return '+';
		case SDLK_q: return 0xD0B9;//'й'
		case SDLK_w: return 0xD186;//'ц'
		case SDLK_e: return 0xD183;//'у'
		case SDLK_r: return 0xD0BA;//'к'
		case SDLK_t: return 0xD0B5;//'е'
		case SDLK_y: return 0xD0BD;//'н'
		case SDLK_u: return 0xD0B3;//'г'
		case SDLK_i: return 0xD188;//'ш'
		case SDLK_o: return 0xD189;//'щ'
		case SDLK_p: return 0xD0B7;//'з'
		case SDLK_LEFTBRACKET: return 0xD185;//'х'
		case SDLK_RIGHTBRACKET: return 0xD18A;//'ъ'
		case SDLK_BACKSLASH: return '|';
		case SDLK_a: return 0xD184;//'ф'
		case SDLK_s: return 0xD18B;//'ы'
		case SDLK_d: return 0xD0B2;//'в'
		case SDLK_f: return 0xD0B0;//'а'
		case SDLK_g: return 0xD0BF;//'п'
		case SDLK_h: return 0xD180;//'р'
		case SDLK_j: return 0xD0BE;//'о'
		case SDLK_k: return 0xD0BB;//'л'
		case SDLK_l: return 0xD0B4;//'д'
		case SDLK_SEMICOLON: return 0xD0B6;//'ж'
		case SDLK_QUOTE: return 0xD18D;//'э'
		case SDLK_z: return 0xD18F;//'я'
		case SDLK_x: return 0xD187;//'ч'
		case SDLK_c: return 0xD181;//'с'
		case SDLK_v: return 0xD0BC;//'м'
		case SDLK_b: return 0xD0B8;//'и'
		case SDLK_n: return 0xD182;//'т'
		case SDLK_m: return 0xD18C;//'ь'
		case SDLK_COMMA: return 0xD0B1;//'б'
		case SDLK_PERIOD: return 0xD18E;//'ю'
		case SDLK_SLASH: return ',';
		case SDLK_ASTERISK: return '*';
		case SDLK_SPACE: return ' ';

		case SDLK_KP7: return '7';
		case SDLK_KP8: return '8';
		case SDLK_KP9: return '9';
		case SDLK_KP_MINUS: return '-';
		case SDLK_KP4: return '4';
		case SDLK_KP5: return '5';
		case SDLK_KP6: return '6';
		case SDLK_KP_PLUS: return '+';
		case SDLK_KP_MULTIPLY: return '*';
		case SDLK_KP1: return '1';
		case SDLK_KP2: return '2';
		case SDLK_KP3: return '3';
		case SDLK_KP0: return '0';
		case SDLK_KP_PERIOD: return '.';
		case SDLK_KP_DIVIDE: return '/';
	}

	return 0;
}
