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

#ifndef GFX_SKYBOX_H
#define GFX_SKYBOX_H

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

enum class eSide : unsigned {
    RIGHT,  // The RIGHT side
    LEFT,   // The LEFT side
    BOTTOM, // The BOTTOM side
    TOP,    // The TOP side
    BACK,   // The BACK side
    FRONT,  // The FRONT side
    size    // last entry, enumeration size
};


// Setup texture for each SkyBox side.
void SkyBoxSetTexture(GLtexture TextureID, eSide Side);
// Create SkyBox.
void SkyBoxCreate(float X, float Y, float Z, float Width, float Height, float Length);
// Draw initialized SkyBox sides.
void SkyBoxDraw();

} // astromenace namespace
} // viewizard namespace

#endif // GFX_SKYBOX_H
