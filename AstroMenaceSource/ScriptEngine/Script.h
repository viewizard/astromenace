/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Script.h

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

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


#ifndef SCRIPT_H
#define SCRIPT_H

#include "../Game.h"




//-----------------------------------------------------------------------------
// Основной класс сценария
//-----------------------------------------------------------------------------
class ScriptEngine
{
public:

	ScriptEngine();
	~ScriptEngine();

	// запустить скрипт на выполнение
	bool	RunScript(const char *FileName, float InitTime);


	// проверяем скрипт
	bool	Update(float Time);
	// доп. проверка для TimeLine
	void	UpdateTimeLine();
	// последнее время выполнения команды
	float	TimeLastOp;
	// время старта скрипта
	float	StartTime;
	// разность между текущем временем и необходимым, нужно чтобы правильно считать положение при появлении
	float	TimeOpLag;

	// основной документ
	TiXmlDocument	*xmlDoc;
	// основной элемент
	TiXmlElement	*xmlAstroMenaceScript;
	// текущий элемент
	TiXmlElement	*xmlElem;

	// включен отладочный режим или нет... по умолчанию выключен
	bool	ShowDebugModeLine;




	bool NeedCheckSpaceShip;
	bool NeedCheckGroundObject;
	float EndDelayMissionComplete;
	float LastTimeMissionComplete;

	float AsterQuant;
	float AsterW;
	float AsterH;
	float AsterXPos;
	float AsterYPos;
	float AsterZPos;
	float AsterRealNeed;
	float AsterMaxSpeed;
	float AsterMinFastSpeed;
	float AsterLastTime;
	int AsterFastCount;
	bool AsterOn;

};








// работа с текстом

void ReleaseGameText();
void InitGameText(const char *FileName);
const char *GetText(const char *ItemID);
int CheckFontCharsInText();



#endif // SCRIPT_H
