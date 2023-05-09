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

#include "../assets/model3d.h"
#include "object3d.h"

// NOTE switch to nested namespace definition (namespace A::B::C { ... }) (since C++17)
namespace viewizard {
namespace astromenace {

// FIXME should be fixed, don't allow global scope interaction for local variables
std::weak_ptr<cGLSL> GLSLShaderType1{};
std::weak_ptr<cGLSL> GLSLShaderType2{};
std::weak_ptr<cGLSL> GLSLShaderType3{};


/*
 * Check for "mortal" objects, that could be used for collision detection.
 */
// TODO probably, we should use additional field with this info instead of call NeedCheckCollision() all the time
bool NeedCheckCollision(const cObject3D &Object3D)
{
    // don't use 'default' case here, we need compiler's warning if anyone was missed
    switch (Object3D.ObjectType) {
    case eObjectType::none:
        return false;

    case eObjectType::EarthFighter:
        return true;

    case eObjectType::AlienFighter:
        return true;

    case eObjectType::AlienMotherShip:
        return true;

    case eObjectType::PirateShip:
        return true;

    case eObjectType::PirateVehicle:
        return true;

    case eObjectType::PirateBuilding:
        return true;

    case eObjectType::SmallAsteroid:
        return true;

    case eObjectType::SpaceDebris:
        return true;

    case eObjectType::ShipWeapon:
        return true;

    case eObjectType::Projectile:
        return true;

    case eObjectType::Explosion:
        return false;

    case eObjectType::CivilianBuilding:
        return false;

    case eObjectType::BasePart:
        return false;

    case eObjectType::Planet:
        return false;

    case eObjectType::Planetoid:
        return false;

    case eObjectType::BigAsteroid:
        return false;
    }

    return false;
}

/*
 * Load 3D model data.
 */
void LoadObjectData(unsigned FileNameHash, cObject3D &Object3D)
{
    std::weak_ptr<sModel3D> Model = GetPreloadedModel3DAsset(FileNameHash);
    auto sharedModel = Model.lock();
    if (!sharedModel) {
        return;
    }

    Object3D.GlobalVertexArray = sharedModel->GlobalVertexArray;
    Object3D.GlobalVertexArrayCount = sharedModel->GlobalVertexArrayCount;
    Object3D.GlobalVBO = sharedModel->GlobalVBO;
    Object3D.GlobalIndexArray = sharedModel->GlobalIndexArray;
    Object3D.GlobalIndexArrayCount = sharedModel->GlobalIndexArrayCount;
    Object3D.GlobalIBO = sharedModel->GlobalIBO;
    Object3D.GlobalVAO = sharedModel->GlobalVAO;
    Object3D.Chunks = sharedModel->Chunks;

    Object3D.Texture.resize(Object3D.Chunks.size(), 0);
    Object3D.TextureIllum.resize(Object3D.Chunks.size(), 0);
    Object3D.NormalMap.resize(Object3D.Chunks.size(), 0);

    Object3D.AABB = sharedModel->AABB;
    Object3D.OBB = sharedModel->OBB;
    Object3D.HitBB = sharedModel->HitBB;
    Object3D.GeometryCenter = sharedModel->GeometryCenter;
    Object3D.Radius = sharedModel->Radius;
    Object3D.Width = sharedModel->Width;
    Object3D.Length = sharedModel->Length;
    Object3D.Height = sharedModel->Height;
}

/*
 * Setup shader's uniform location.
 */
static bool SetupShaderUniformLocation(std::weak_ptr<cGLSL> &Shader,
                                       const std::string &ShaderName,
                                       const std::vector<std::string> &UniformLocationNameArray)
{
    // find and store uniform location for all shaders, we don't really need
    // store internal storage number for uniforms usage, since we load them
    // one-by-one, and we know sequence, we could use 0-1-2-3-4 numbers directly
    for (const auto &tmpName : UniformLocationNameArray) {
        if (vw_FindShaderUniformLocation(Shader, tmpName) < 0) {
            std::cerr << __func__ << "(): " << "failed to find uniform location " << tmpName
                      << " in shader " << ShaderName << ".\n";
            return false;
        }
    }
    return true;
}

/*
 * Setup shader.
 */
static bool SetupShader(std::weak_ptr<cGLSL> &Shader,
                        const std::string &ShaderName,
                        const std::vector<std::string> &UniformLocationNameArray)
{
    Shader = vw_FindShaderByName(ShaderName);
    if (Shader.expired()) {
        std::cerr << __func__ << "(): " << "failed to find " << ShaderName << " shader.\n";
        return false;
    }
    if (!SetupShaderUniformLocation(Shader, ShaderName, UniformLocationNameArray)) {
        return false;
    }

    return true;
}

/*
 * Setup shaders.
 */
bool SetupObject3DShaders()
{
    const std::vector<std::string> GLSLShaderType1UniformLocationNames{
        {"Texture1"},
        {"Texture2"},
        {"NeedMultitexture"},
        {"NormalMap"},
        {"NeedNormalMapping"}
    };
    if (!SetupShader(GLSLShaderType1, "PerPixelLight", GLSLShaderType1UniformLocationNames)) {
        return false;
    }

    const std::vector<std::string> GLSLShaderType2UniformLocationNames{
        {"Texture1"},
        {"SpeedData1"},
        {"SpeedData2"}
    };
    if (!SetupShader(GLSLShaderType2, "PerPixelLight_Explosion", GLSLShaderType2UniformLocationNames)) {
        return false;
    }

    const std::vector<std::string> GLSLShaderType3UniformLocationNames{
        {"Texture1"},
        {"Texture2"},
        {"NeedMultitexture"},
        {"ShadowMap"},
        {"xPixelOffset"},
        {"yPixelOffset"},
        {"NormalMap"},
        {"NeedNormalMapping"},
    };
    if (!SetupShader(GLSLShaderType3, "PerPixelLight_ShadowMap", GLSLShaderType3UniformLocationNames)) {
        return false;
    }

    return true;
}

} // astromenace namespace
} // viewizard namespace
