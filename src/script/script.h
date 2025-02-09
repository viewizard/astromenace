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

// TODO structures sTimeSheet and cMissionScript should use encapsulation (switch to class)

// TODO move all sTimeSheet related stuff from 'object3d' into this 'script' sources

#ifndef SCRIPT_SCRIPT_H
#define SCRIPT_SCRIPT_H

#include "../core/core.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

struct sTimeSheet {
    bool InUse{false};
    float Time{0.0f};

    int AI_Mode{0}; // packed TimeSheet
    std::shared_ptr<cXMLDocument> xmlAI{}; // xml with packed TimeSheets

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

    sVECTOR3D Rotation{0.0f, 0.0f, 0.0f};
    sVECTOR3D RotationAcceler{1.0f, 1.0f, 1.0f}; // [0.0f, 1.0f]
    sVECTOR3D SetAngle{0.0f, 0.0f, 0.0f};
    bool NeedSetAngle{false};
    bool Fire{false};
    bool BossFire{false};
    bool Targeting{false};
};

class cMissionScript
{
public:
    bool RunScript(const std::string &FileName, float InitTime);
    bool Update(float Time);
    void UpdateTimeLine();

    float TimeLastOp{0}; // last operational time
    float StartTime{0}; // script start time
    float TimeOpLag{0}; // care about time lag

    std::unique_ptr<cXMLDocument> xmlDoc{};
    std::list<sXMLEntry>::iterator xmlEntryIter{};

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


/*
 * Unpack TimeSheet to the list.
 */
void UnpackAIMode(std::list<sTimeSheet> &TimeSheetList);

} // astromenace namespace
} // viewizard namespace

#endif // SCRIPT_SCRIPT_H
