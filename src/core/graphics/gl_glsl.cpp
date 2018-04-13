/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO move from pointers to std::shared_ptr/std::weak_ptr

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

// NOTE since OpenGL 3.1 core profile, OpenGL must render with a program,
//      vw_StopShaderProgram() should be removed

#include "../vfs/vfs.h"
#include "graphics.h"
#include "extensions.h"

struct sGLSL {
	~sGLSL() {
		if (_glDetachObject && _glDeleteObject) {
			if (VertexShaderUse)
				_glDetachObject(Program, VertexShader);
			if (FragmentShaderUse)
				_glDetachObject(Program, FragmentShader);

			_glDeleteObject(VertexShader);
			_glDeleteObject(FragmentShader);
			_glDeleteObject(Program);
		}
	}

#ifdef __APPLE__
	// typedef void *GLhandleARB, see glext.h for more declaration
	GLhandleARB Program{nullptr};
	GLhandleARB VertexShader{nullptr};
	GLhandleARB FragmentShader{nullptr};
#else
	// typedef unsigned int GLhandleARB, see glext.h for more declaration
	GLhandleARB Program{0};
	GLhandleARB VertexShader{0};
	GLhandleARB FragmentShader{0};
#endif
	bool VertexShaderUse{false};
	bool FragmentShaderUse{false};
};

namespace {

// all shaders
std::unordered_map<std::string, std::shared_ptr<sGLSL>> ShadersMap{};

} // unnamed namespace


/*
 * Print (and reset) OpenGL errors.
 */
static void CheckOGLError(const char *FunctionName)
{
	GLenum glErr{0};

	while ((glErr = glGetError()) != GL_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "glError " << glErr << "\n";
	}
}

/*
 * Print out the information log for a shader object.
 */
static void PrintShaderInfoLog(GLhandleARB shader, const std::string &ShaderName)
{
	if (!_glGetObjectParameteriv ||
	    !_glGetInfoLog)
		return;

	CheckOGLError(__func__);

	int infologLength{0};
	_glGetObjectParameteriv(shader, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError(__func__);

	if (infologLength > 0) {
		std::unique_ptr<GLchar []> infoLog{new GLchar[infologLength]};
		int charsWritten{0};
		_glGetInfoLog(shader, infologLength, &charsWritten, infoLog.get());
		// at this line, infoLog.get() should contains null-terminated string
		if (charsWritten)
			std::cout << "Shader InfoLog " << ShaderName << ":\n" << infoLog.get() << "\n\n";
		else
			std::cout << "Shader InfoLog:\n" << "no log provided" << "\n\n";
	}
	CheckOGLError(__func__);
}

/*
 * Print out the information log.
 */
static void PrintProgramInfoLog(GLhandleARB program)
{
	if (!_glGetObjectParameteriv ||
	    !_glGetInfoLog)
		return;

	CheckOGLError(__func__);

	int infologLength{0};
	_glGetObjectParameteriv(program, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError(__func__);

	if (infologLength > 0) {
		std::unique_ptr<GLchar []> infoLog{new GLchar[infologLength]};
		int charsWritten{0};
		_glGetInfoLog(program, infologLength, &charsWritten, infoLog.get());
		// at this line, infoLog.get() should contains null-terminated string
		if (charsWritten)
			std::cout << "Program InfoLog:\n" << infoLog.get() << "\n\n";
		else
			std::cout << "Program InfoLog:\n" << "no log provided" << "\n\n";
	}
	CheckOGLError(__func__);
}

/*
 * Release all shaders.
 */
void vw_ReleaseAllShaders()
{
	ShadersMap.clear();
}

/*
 * Check, is shaders Map empty.
 */
bool vw_ShadersMapEmpty()
{
	return ShadersMap.empty();
}

/*
 * Find shader by name.
 */
sGLSL *vw_FindShaderByName(const std::string &Name)
{
	if (Name.empty())
		return nullptr;

	auto tmpShader = ShadersMap.find(Name);
	if (tmpShader != ShadersMap.end())
		return tmpShader->second.get();

	return nullptr;
}

/*
 * Create shader program.
 */
sGLSL *vw_CreateShader(const std::string &ShaderName,
		       const std::string &VertexShaderFileName,
		       const std::string &FragmentShaderFileName)
{
	if (ShaderName.empty() ||
	    !_glCreateShaderObject ||
	    !_glShaderSource ||
	    !_glCompileShader ||
	    !_glCreateProgramObject ||
	    !_glAttachObject ||
	    !_glGetObjectParameteriv ||
	    (VertexShaderFileName.empty() && FragmentShaderFileName.empty()))
		return nullptr;

	GLint vertCompiled{0}, fragCompiled{0}; // status values

	ShadersMap.emplace(ShaderName, new sGLSL);

	// create empty objects
	ShadersMap[ShaderName].get()->VertexShader = _glCreateShaderObject(GL_VERTEX_SHADER_ARB);
	ShadersMap[ShaderName].get()->FragmentShader = _glCreateShaderObject(GL_FRAGMENT_SHADER_ARB);

	// load vertex shader
	if (!VertexShaderFileName.empty()) {
		std::unique_ptr<sFILE> VertexFile = vw_fopen(VertexShaderFileName);

		if (VertexFile) {
			const GLcharARB *TmpGLcharARB = (const GLcharARB *)VertexFile->Data.get();
			GLint TmpGLint = (GLint)VertexFile->Size;
			_glShaderSource(ShadersMap[ShaderName].get()->VertexShader, 1, &TmpGLcharARB, &TmpGLint);
			vw_fclose(VertexFile);
			ShadersMap[ShaderName].get()->VertexShaderUse = true;
		}
	}
	// load fragment shader
	if (!FragmentShaderFileName.empty()) {
		std::unique_ptr<sFILE> FragmentFile = vw_fopen(FragmentShaderFileName);

		if (FragmentFile) {
			const GLcharARB *TmpGLcharARB = (const GLcharARB *)FragmentFile->Data.get();
			GLint TmpGLint = (GLint)FragmentFile->Size;
			_glShaderSource(ShadersMap[ShaderName].get()->FragmentShader, 1, &TmpGLcharARB, &TmpGLint);
			vw_fclose(FragmentFile);
			ShadersMap[ShaderName].get()->FragmentShaderUse = true;
		}
	}

	// compile shaders
	if (ShadersMap[ShaderName].get()->VertexShaderUse) {
		_glCompileShader(ShadersMap[ShaderName].get()->VertexShader);
		CheckOGLError(__func__);
		_glGetObjectParameteriv(ShadersMap[ShaderName].get()->VertexShader, GL_COMPILE_STATUS, &vertCompiled);
		PrintShaderInfoLog(ShadersMap[ShaderName].get()->VertexShader, VertexShaderFileName);

		if (!vertCompiled) {
			ShadersMap.erase(ShaderName);
			return nullptr;
		}
	}
	if (ShadersMap[ShaderName].get()->FragmentShaderUse) {
		_glCompileShader(ShadersMap[ShaderName].get()->FragmentShader);
		CheckOGLError(__func__);
		_glGetObjectParameteriv(ShadersMap[ShaderName].get()->FragmentShader, GL_COMPILE_STATUS, &fragCompiled);
		PrintShaderInfoLog(ShadersMap[ShaderName].get()->FragmentShader, FragmentShaderFileName);

		if (!fragCompiled) {
			ShadersMap.erase(ShaderName);
			return nullptr;
		}
	}

	// create program
	ShadersMap[ShaderName].get()->Program = _glCreateProgramObject();
	if (ShadersMap[ShaderName].get()->VertexShaderUse)
		_glAttachObject(ShadersMap[ShaderName].get()->Program, ShadersMap[ShaderName].get()->VertexShader);
	if (ShadersMap[ShaderName].get()->FragmentShaderUse)
		_glAttachObject(ShadersMap[ShaderName].get()->Program, ShadersMap[ShaderName].get()->FragmentShader);

	std::cout << "Shader ... " << VertexShaderFileName << " " << FragmentShaderFileName << "\n";

	return ShadersMap[ShaderName].get();
}

/*
 * Links a program object.
 */
bool vw_LinkShaderProgram(sGLSL *GLSL)
{
	if (!GLSL ||
	    !_glLinkProgram ||
	    !_glGetObjectParameteriv)
		return false;

	_glLinkProgram(GLSL->Program);
	CheckOGLError(__func__);

	GLint Linked{false};
	_glGetObjectParameteriv(GLSL->Program, GL_LINK_STATUS, &Linked);
	PrintProgramInfoLog(GLSL->Program);

	return Linked;
}

/*
 * Installs a program object as part of current rendering state.
 */
bool vw_UseShaderProgram(sGLSL *GLSL)
{
	if (!GLSL || !_glUseProgramObject)
		return false;

	_glUseProgramObject(GLSL->Program);
	CheckOGLError(__func__);

	return true;
}

/*
 * Switch to fixed-function program object as part of current rendering state.
 */
bool vw_StopShaderProgram()
{
	if (!_glUseProgramObject)
		return false;

	_glUseProgramObject(
#ifdef __APPLE__
			      nullptr	/* typedef void *GLhandleARB, see glext.h for more declaration */
#else
			      0		/* typedef unsigned int GLhandleARB, see glext.h for more declaration */
#endif
			      );
	CheckOGLError(__func__);

	return true;
}

/*
 * Returns the location of a uniform variable.
 */
int vw_GetUniformLocation(sGLSL *GLSL, const std::string &Name)
{
	if (!GLSL || Name.empty() || !_glGetUniformLocation)
		return -1;

	int tmpLocation = _glGetUniformLocation(GLSL->Program, Name.c_str());
	CheckOGLError(__func__);

	if (tmpLocation == -1)
		std::cerr << __func__ << "(): " << "No such uniform named: " << Name << "\n";

	return tmpLocation;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform1i(sGLSL *GLSL, int UniformLocation, int data)
{
	if (!GLSL || !_glUniform1i)
		return false;

	_glUniform1i(UniformLocation, data);
	CheckOGLError(__func__);

	return true;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform1f(sGLSL *GLSL, int UniformLocation, float data)
{
	if (!GLSL || !_glUniform1f)
		return false;

	_glUniform1f(UniformLocation, data);
	CheckOGLError(__func__);

	return true;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform3f(sGLSL *GLSL, int UniformLocation, float data1, float data2, float data3)
{
	if (!GLSL || !_glUniform3f)
		return false;

	_glUniform3f(UniformLocation, data1, data2, data3);
	CheckOGLError(__func__);

	return true;
}
