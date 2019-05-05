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

#include "../core/core.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

namespace {

struct sShaderMetadata {
    std::string Name;
    std::string VertexShaderFileName;
    std::string FragmentShaderFileName;
};

const std::vector<sShaderMetadata> ShaderArray{
    {"ParticleSystem",          "glsl/particle.vert",           "glsl/particle.frag"},
    {"PerPixelLight",           "glsl/light.vert",              "glsl/light.frag"},
    {"PerPixelLight_ShadowMap", "glsl/light_shadowmap.vert",    "glsl/light_shadowmap.frag"},
    {"PerPixelLight_Explosion", "glsl/light_explosion.vert",    "glsl/light_explosion.frag"},
};

} // unnamed namespace


/*
 * Cycle with function callback on each shader asset load.
 */
bool ForEachShaderAssetLoad(std::function<void ()> function)
{
    if (!vw_DevCaps().OpenGL_2_0_supported || !vw_DevCaps().OpenGL_2_1_supported) {
        return false;
    }

    for (auto &tmpAsset : ShaderArray) {
        std::weak_ptr<cGLSL> Program = vw_CreateShader(tmpAsset.Name,
                                       tmpAsset.VertexShaderFileName,
                                       tmpAsset.FragmentShaderFileName);
        if (Program.expired()) {
            vw_ReleaseAllShaders();
            return false;
        }
        if (!vw_LinkShaderProgram(Program)) {
            vw_ReleaseAllShaders();
            return false;
        }

        function();
    }

    return true;
}

} // astromenace namespace
} // viewizard namespace
