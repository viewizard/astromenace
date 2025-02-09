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

// NOTE ARB_vertex_attrib_binding (since OpenGL 4.3)
//      specify the attribute format and the attribute data separately

// NOTE ARB_direct_state_access (since OpenGL 4.5)
//      no longer need to call glBindBuffer() or glBindVertexArray()

#include "graphics_internal.h"
#include "graphics.h"
#include "extensions.h"

namespace viewizard {

/*
 * Build vertex array object.
 */
bool vw_BuildVAO(GLuint &VAO, int DataFormat, GLsizei Stride, GLuint VertexBO, GLuint IndexBO)
{
    if (!pfn_glGenVertexArrays || !pfn_glIsVertexArray) {
        return false;
    }

    // we can not generate VAO without VBO
    if (!VertexBO) {
        std::cerr << __func__ << "(): " << "VertexBO must be provided.\n";
        return false;
    }

    pfn_glGenVertexArrays(1, &VAO);

    vw_BindVAO(VAO);
    Draw3D_EnableStates(DataFormat, nullptr, Stride, VertexBO, IndexBO);

    vw_BindVAO(0);
    Draw3D_DisableStates(DataFormat, VertexBO, IndexBO);

    if (!pfn_glIsVertexArray(VAO)) {
        return false;
    }

    return true;
}

/*
 * Bind vertex array object.
 */
void vw_BindVAO(GLuint VAO)
{
    // don't call glIsVertexArray() here, for best speed
    if (!pfn_glBindVertexArray) {
        return;
    }

    pfn_glBindVertexArray(VAO);
}

/*
 * Delete vertex array object.
 */
void vw_DeleteVAO(GLuint &VAO)
{
    if (!pfn_glIsVertexArray || !pfn_glDeleteVertexArrays || !pfn_glIsVertexArray(VAO)) {
        return;
    }

    pfn_glDeleteVertexArrays(1, &VAO);
    VAO = 0;
}

} // viewizard namespace
