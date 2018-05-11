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

// TODO translate comments

#ifndef SCRIPT_H
#define SCRIPT_H

#include "../core/core.h"

struct sTimeSheet {
	// флаг, показывает что установлен и задействован этот режим
	bool InUse{false};
	// кол-во времени (оставшееся), которое работает этот режим
	float Time{0.0f};
	// включена ли автоматическая настройка поведения по предустановкам
	int AI_Mode{0};
	std::shared_ptr<cXMLDocument> xmlAI{};

	// данные, скорость
	float Speed{0.0f};
	float Acceler{1.0f}; // [0.0f, 1.0f]
	float SpeedLR{0.0f};
	float AccelerLR{1.0f}; // [0.0f, 1.0f]
	float SpeedUD{0.0f};
	float AccelerUD{1.0f}; // [0.0f, 1.0f]

	float SpeedByCamFB{0.0f};
	float AccelerByCamFB{1.0f}; // [0.0f, 1.0f]
	float SpeedByCamLR{0.0f};
	float AccelerByCamLR{1.0f}; // [0.0f, 1.0f]
	float SpeedByCamUD{0.0f};
	float AccelerByCamUD{1.0f}; // [0.0f, 1.0f]

	// поворот
	sVECTOR3D Rotation{0.0f, 0.0f, 0.0f};
	sVECTOR3D RotationAcceler{1.0f, 1.0f, 1.0f}; // [0.0f, 1.0f]
	// стрельба
	bool Fire{false};
	// стрельба спец оружием боса
	bool BossFire{false};
	// наведение на цель (для турелей)
	bool Targeting{false};
};

class cMissionScript
{
public:
	cMissionScript();

	// запустить скрипт на выполнение
	bool RunScript(const char *FileName, float InitTime);

	// проверяем скрипт
	bool Update(float Time);
	// доп. проверка для TimeLine
	void UpdateTimeLine();
	// последнее время выполнения команды
	float TimeLastOp{0};
	// время старта скрипта
	float StartTime{0};
	// разность между текущем временем и необходимым, нужно чтобы правильно считать положение при появлении
	float TimeOpLag{0};

	// основной документ
	std::unique_ptr<cXMLDocument> xmlDoc{};
	// текущий итератор (положение в скрипте)
	std::list<sXMLEntry>::iterator xmlEntryIter{};

	// включен отладочный режим или нет... по умолчанию выключен
	bool ShowLineNumber{false};

	bool NeedCheckSpaceShip{false};
	bool NeedCheckGroundObject{false};
	float EndDelayMissionComplete{0.0f};
	float LastTimeMissionComplete{-1.0f};

	float AsterQuant{2.0f};
	float AsterW{280.0f};
	float AsterH{7.5f};
	float AsterXPos{0.0f};
	float AsterYPos{-10.0f};
	float AsterZPos{340.0f};
	float AsterRealNeed{0.0f};
	float AsterMaxSpeed{5.0f};
	float AsterMinFastSpeed{35.0f};
	float AsterLastTime{-1.0f};
	int AsterFastCount{0};
	bool AsterOn{false};

private:
	std::shared_ptr<cXMLDocument> xmlAI{};
};


// работа с распаковкой sTimeSheet
void InterAIMode(std::list<sTimeSheet> &TimeSheetList);

#endif // SCRIPT_H
