/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


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
	cXMLDocument	*xmlDoc;
	// текущий элемент
	cXMLEntry		*xmlEntry;

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




#endif // SCRIPT_H
