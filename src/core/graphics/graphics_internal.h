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

/*
Should be for graphic's internal use only.
*/

#ifndef CORE_GRAPHICS_GRAPHICSINTERNAL_H
#define CORE_GRAPHICS_GRAPHICSINTERNAL_H

#include "opengl.h"

namespace viewizard {

struct sDevCaps;

/*
 * gl_main
 */

// Internal access to DevCaps, with write access.
sDevCaps &ChangeDevCaps();

/*
 * gl_draw3d
 */

void Draw3D_EnableStates(int DataFormat, GLvoid *VertexArray,
                         GLsizei stride, GLuint VertexBO, GLuint IndexBO);
void Draw3D_DisableStates(int DataFormat, GLuint VertexBO, GLuint IndexBO);

} // viewizard namespace

#endif // CORE_GRAPHICS_GRAPHICSINTERNAL_H
