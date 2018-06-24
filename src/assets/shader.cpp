/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../core/core.h"

std::weak_ptr<cGLSL> GLSLShaderType1{};
std::weak_ptr<cGLSL> GLSLShaderType2{};
std::weak_ptr<cGLSL> GLSLShaderType3{};

struct sGLSLMetadata {
	std::string Name;
	std::string VertexShaderFileName;
	std::string FragmentShaderFileName;
};
const sGLSLMetadata GLSLLoadList[] = {
	{"ParticleSystem",		"glsl/particle.vert",		"glsl/particle.frag"},
	{"PerPixelLight",		"glsl/light.vert",		"glsl/light.frag"},
	{"PerPixelLight_ShadowMap",	"glsl/light_shadowmap.vert",	"glsl/light_shadowmap.frag"},
	{"PerPixelLight_Explosion",	"glsl/light_explosion.vert",	"glsl/light_explosion.frag"},
};
#define GLSLLoadListCount sizeof(GLSLLoadList)/sizeof(GLSLLoadList[0])


/*
 * Setup shader's uniform locations.
 */
static void SetupUniformLocations()
{
	// find all shaders by name
	GLSLShaderType1 = vw_FindShaderByName("PerPixelLight");
	GLSLShaderType2 = vw_FindShaderByName("PerPixelLight_Explosion");
	GLSLShaderType3 = vw_FindShaderByName("PerPixelLight_ShadowMap");

	// find and store uniform location for all shaders, in real, we don't need
	// store internal storage number for uniforms usage, since we load them
	// one-by-one, and we know sequence, we could use 0-1-2-3-4 numbers directly
	vw_FindShaderUniformLocation(GLSLShaderType1, "Texture1");
	vw_FindShaderUniformLocation(GLSLShaderType1, "Texture2");
	vw_FindShaderUniformLocation(GLSLShaderType1, "DirectLightCount");
	vw_FindShaderUniformLocation(GLSLShaderType1, "PointLightCount");
	vw_FindShaderUniformLocation(GLSLShaderType1, "NeedMultitexture");
	vw_FindShaderUniformLocation(GLSLShaderType1, "NormalMap");
	vw_FindShaderUniformLocation(GLSLShaderType1, "NeedNormalMapping");

	vw_FindShaderUniformLocation(GLSLShaderType2, "Texture1");
	vw_FindShaderUniformLocation(GLSLShaderType2, "DirectLightCount");
	vw_FindShaderUniformLocation(GLSLShaderType2, "PointLightCount");
	vw_FindShaderUniformLocation(GLSLShaderType2, "SpeedData1");
	vw_FindShaderUniformLocation(GLSLShaderType2, "SpeedData2");

	vw_FindShaderUniformLocation(GLSLShaderType3, "Texture1");
	vw_FindShaderUniformLocation(GLSLShaderType3, "Texture2");
	vw_FindShaderUniformLocation(GLSLShaderType3, "DirectLightCount");
	vw_FindShaderUniformLocation(GLSLShaderType3, "PointLightCount");
	vw_FindShaderUniformLocation(GLSLShaderType3, "NeedMultitexture");
	vw_FindShaderUniformLocation(GLSLShaderType3, "ShadowMap");
	vw_FindShaderUniformLocation(GLSLShaderType3, "xPixelOffset");
	vw_FindShaderUniformLocation(GLSLShaderType3, "yPixelOffset");
	vw_FindShaderUniformLocation(GLSLShaderType3, "NormalMap");
	vw_FindShaderUniformLocation(GLSLShaderType3, "NeedNormalMapping");
	vw_FindShaderUniformLocation(GLSLShaderType3, "PCFMode");
}

/*
 * Cycle with function callback on each shader asset load.
 */
bool ForEachShaderAssetLoad(std::function<void ()> function)
{
	if (!vw_GetDevCaps().OpenGL_2_0_supported ||
	    !vw_GetDevCaps().OpenGL_2_1_supported)
		return false;

	for (unsigned i = 0; i < GLSLLoadListCount; i++) {
		std::weak_ptr<cGLSL> Program = vw_CreateShader(GLSLLoadList[i].Name,
							       GLSLLoadList[i].VertexShaderFileName,
							       GLSLLoadList[i].FragmentShaderFileName);
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

	SetupUniformLocations();
	return true;
}
