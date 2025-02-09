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

#ifndef OBJECT3D_SHAKE_H
#define OBJECT3D_SHAKE_H

#include "../core/core.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

class cShake {
public:
    cShake (const sVECTOR3D &_Direction, const unsigned _ChunkNum,
            const float _ShiftPerSecond, const std::function<float ()> &_GenerateRand) :
        Direction{_Direction},
        ChunkNum{_ChunkNum},
        ShiftPerSecond{_ShiftPerSecond},
        GenerateRand{_GenerateRand}
    {
        Need = GenerateRand();
    }

    void Update(float TimeDelta, std::function<void (const float ShakeIncrement,
                const sVECTOR3D &ShakeDirection,
                const unsigned ShakeChunkNum)> function)
    {
        float Sign{1.0f};
        if (Need < Current) {
            Sign = -1.0f;
        }

        float tmpIncrement = Sign * ShiftPerSecond * TimeDelta;

        if ((Sign > 0.0f && Need <= Current + tmpIncrement) || (Sign < 0.0f && Need >= Current + tmpIncrement)) {
            tmpIncrement += Current + tmpIncrement - Need;
            Need = GenerateRand();
        }
        Current += tmpIncrement;
        function(tmpIncrement, Direction, ChunkNum);
    }

private:
    sVECTOR3D Direction{};
    unsigned ChunkNum{0};
    float ShiftPerSecond{0.0f};
    float Need{0.0f};
    float Current{0.0f};
    std::function<float ()> GenerateRand{[] () {return 0.0f;}};
};

} // astromenace namespace
} // viewizard namespace

#endif // OBJECT3D_SHAKE_H
