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

// TODO don't call GetPreloadedTextureAsset() all the time, use cached texture instead

#include "space_object.h"
#include "../../config/config.h"
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Constructor.
 */
cPlanetoid::cPlanetoid(const int PlanetoidNum)
{
    ObjectStatus = eObjectStatus::none;
    ObjectType = eObjectType::Planetoid;

    // we could create random planetoid, or provide particular planetoid id
    if (PlanetoidNum < 1 || PlanetoidNum > 5) {
        InternalType = 1 + vw_uRandNum(4);
    } else {
        InternalType = PlanetoidNum;
    }

    switch (InternalType) {
    case 1:
        {
            constexpr unsigned tmpHash = constexpr_hash_djb2a("models/space/bigasteroid-01.vw3d");
            LoadObjectData(tmpHash, *this);
        }
        break;
    case 2:
        {
            constexpr unsigned tmpHash = constexpr_hash_djb2a("models/space/bigasteroid-02.vw3d");
            LoadObjectData(tmpHash, *this);
        }
        break;
    case 3:
        {
            constexpr unsigned tmpHash = constexpr_hash_djb2a("models/space/bigasteroid-03.vw3d");
            LoadObjectData(tmpHash, *this);
        }
        break;
    case 4:
        {
            constexpr unsigned tmpHash = constexpr_hash_djb2a("models/space/bigasteroid-04.vw3d");
            LoadObjectData(tmpHash, *this);
        }
        break;
    case 5:
        {
            constexpr unsigned tmpHash = constexpr_hash_djb2a("models/space/bigasteroid-05.vw3d");
            LoadObjectData(tmpHash, *this);
        }
        break;
    }

    RotationSpeed.x = 10.0f + 10.0f * vw_fRand0();
    RotationSpeed.y = 2.0f + 20.0f * vw_fRand0();

    constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/d_class3.tga");
    Texture[0] = GetPreloadedTextureAsset(tmpTextureHash);
    TextureIllum[0] = 0;
    constexpr unsigned tmpNormalMapHash = constexpr_hash_djb2a("models/normalmap/d_class3_nm.tga");
    NormalMap[0] = GetPreloadedTextureAsset(tmpNormalMapHash);
}

} // astromenace namespace
} // viewizard namespace
