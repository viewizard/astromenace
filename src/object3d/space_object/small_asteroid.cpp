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
#include "../../assets/texture.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

/*
 * Constructor.
 */
cSmallAsteroid::cSmallAsteroid()
{
    ObjectType = eObjectType::SmallAsteroid;

    static unsigned Model3DFileNameHashes[]{
        constexpr_hash_djb2a("models/space/asteroid-010.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-011.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-012.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-013.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-014.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-015.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-016.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-017.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-018.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-019.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0110.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0111.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0112.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0113.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0114.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0115.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0116.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0117.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0118.vw3d"),
        constexpr_hash_djb2a("models/space/asteroid-0119.vw3d")
    };

    LoadObjectData(Model3DFileNameHashes[vw_uRandNum(19)], *this);

    constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/space/asteroid-01.tga");
    GLtexture tmpTexture = GetPreloadedTextureAsset(tmpTextureHash);
    for (unsigned int i = 0; i < Chunks.size(); i++) {
        Texture[i] = tmpTexture;
    }

    RotationSpeed.x = 100.0f + 50.0f * vw_fRand0();
    RotationSpeed.y = 20.0f + 20.0f * vw_fRand0();

    ArmorCurrentStatus = ArmorInitialStatus = Radius * 10.0f / GameEnemyArmorPenalty;
    Radius = Width / 2.0f;
}

} // astromenace namespace
} // viewizard namespace
