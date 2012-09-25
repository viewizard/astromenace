/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: System.h

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


#ifndef CoreSystem_H
#define CoreSystem_H

#include "../Base.h"


// проверка расширения файла (нужен для детекта wav-ogg, tga-jpg...)
bool 	vw_TestFileExtension(const char *name, const char *extension);

// открываем браузер
bool	vw_OpenBrouser(const char *url);



// работа со временем
void 	vw_InitTime();
void 	vw_InitTimeNum(int Num);
float 	vw_GetTime(int TimeThread = 0);
void 	vw_StopTime();
void 	vw_StartTime();
void 	vw_SetTimeThreadSpeed(int TimeThread=0, float NewSpeed=1.0f);
float 	vw_GetTimeThreadSpeed(int TimeThread=0);



// работа с клавиатурой
bool	vw_GetKeys(int Num);
void	vw_SetKeys(int Num, bool NewKeyStatus);
int 	GetMaxKeys();
const char* 	vw_VirtualCodeName(int KeyboardLayout, int Num);
int 	vw_VirtualCodeChar(int KeyboardLayout, int Num);
const char* 	vw_KeyboardCodeName(int Num);
int 	vw_KeyboardNameCode(const char * Name);


// работа с мышкой
int		vw_GetMousePos(int *X, int *Y);
bool	vw_GetWindowLBMouse(bool ResetStatus);
bool	vw_GetWindowRBMouse(bool ResetStatus);
void	vw_SetWindowLBMouse(bool NewStatus);
void	vw_SetWindowRBMouse(bool NewStatus);
void 	vw_SetWindowLBDoubleMouse(bool NewStatus);
bool 	vw_GetWindowLBDoubleMouse(bool ResetStatus);
void	vw_SetMousePos(int X, int Y);
void 	vw_SetMousePosRel(int X, int Y);
bool	vw_OnRect(RECT *MDetect);




#endif // CoreSystem_H
