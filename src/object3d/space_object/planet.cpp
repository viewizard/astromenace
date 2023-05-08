/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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
cPlanet::cPlanet(const int PlanetNum)
{
    ObjectStatus = eObjectStatus::none;
    ObjectType = eObjectType::Planet;

    InternalType = PlanetNum;

    switch (InternalType) {
    case 1:
        NeedCullFaces = false;
        NeedAlphaTest = true;
        LoadObjectData("models/planet/aplanet.vw3d", *this);
        {
            constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/d_class3.tga");
            Texture[0] = GetPreloadedTextureAsset(tmpTextureHash);
        }
        TextureIllum[0] = 0;
        {
            constexpr unsigned tmpNormalMapHash = constexpr_hash_djb2a("models/normalmap/d_class3_nm.tga");
            NormalMap[0] = GetPreloadedTextureAsset(tmpNormalMapHash);
        }
        {
            constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/asteroid.tga");
            Texture[1] = Texture[2] = Texture[3] = GetPreloadedTextureAsset(tmpTextureHash);
        }
        TextureIllum[1] = TextureIllum[2] = TextureIllum[3] = 0;
        {
            constexpr unsigned tmpNormalMapHash = constexpr_hash_djb2a("models/normalmap/planet_asteroids_nm.tga");
            NormalMap[1] = NormalMap[2] = NormalMap[3] = GetPreloadedTextureAsset(tmpNormalMapHash);
        }
        break;

    case 2:
        PromptDrawDist2 = 100.0f;
        LoadObjectData("models/planet/dplanet.vw3d", *this);
        {
            constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/d_class3.tga");
            GLtexture tmpTexture = GetPreloadedTextureAsset(tmpTextureHash);
            constexpr unsigned tmpNormalMapHash = constexpr_hash_djb2a("models/normalmap/d_class3_nm.tga");
            GLtexture tmpNormalMap = GetPreloadedTextureAsset(tmpNormalMapHash);
            for (unsigned int i = 0; i < Chunks.size(); i++) {
                Texture[i] = tmpTexture;
                TextureIllum[i] = 0;
                NormalMap[i] = tmpNormalMap;
            }
        }
        break;

    case 3:
        NeedCullFaces = false;
        LoadObjectData("models/planet/gplanet.vw3d", *this);
        {
            constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/m_class7.tga");
            Texture[0] = GetPreloadedTextureAsset(tmpTextureHash);
        }
        TextureIllum[0] = 0;
        {
            constexpr unsigned tmpNormalMapHash = constexpr_hash_djb2a("models/normalmap/m_class7_nm.tga");
            NormalMap[0] = GetPreloadedTextureAsset(tmpNormalMapHash);
        }
        {
            constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/clouds.tga");
            Texture[1] = GetPreloadedTextureAsset(tmpTextureHash);
        }
        TextureIllum[1] = 0;
        Chunks[1].DrawType = eModel3DDrawType::Blend;
        break;

    case 4:
        LoadObjectData("models/planet/moon.vw3d", *this);
        {
            constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/q_class2.tga");
            Texture[0] = GetPreloadedTextureAsset(tmpTextureHash);
        }
        TextureIllum[0] = 0;
        {
            constexpr unsigned tmpNormalMapHash = constexpr_hash_djb2a("models/normalmap/q_class2_nm.tga");
            NormalMap[0] = GetPreloadedTextureAsset(tmpNormalMapHash);
        }
        break;

    case 5:
        LoadObjectData("models/planet/planet5.vw3d", *this);
        {
            constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/a_class4.tga");
            Texture[0] = GetPreloadedTextureAsset(tmpTextureHash);
        }
        TextureIllum[0] = 0;
        {
            constexpr unsigned tmpNormalMapHash = constexpr_hash_djb2a("models/normalmap/a_class4_nm.tga");
            NormalMap[0] = GetPreloadedTextureAsset(tmpNormalMapHash);
        }
        break;

    case 6:
        LoadObjectData("models/planet/planet6.vw3d", *this);
        {
            constexpr unsigned tmpTextureHash = constexpr_hash_djb2a("models/planet/d_class3.tga");
            Texture[0] = GetPreloadedTextureAsset(tmpTextureHash);
        }
        TextureIllum[0] = 0;
        {
            constexpr unsigned tmpNormalMapHash = constexpr_hash_djb2a("models/normalmap/d_class3_nm.tga");
            NormalMap[0] = GetPreloadedTextureAsset(tmpNormalMapHash);
        }
        break;
    }
}

} // astromenace namespace
} // viewizard namespace
