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

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

// NOTE since OpenGL 3.1 core profile, OpenGL must render with a program,
//      vw_StopShaderProgram() should be removed

#include "../vfs/vfs.h"
#include "graphics.h"
#include "extensions.h"

struct cGLSL {
	friend std::weak_ptr<cGLSL> vw_CreateShader(const std::string &ShaderName,
						    const std::string &VertexShaderFileName,
						    const std::string &FragmentShaderFileName);
public:
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

private:
	// Don't allow direct new/delete usage in code, only vw_CreateShader()
	// allowed for shaders creation and release setup (deleter must be provided).
	cGLSL() = default;
	~cGLSL() {
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
};

namespace {

// all shaders
std::unordered_map<std::string, std::shared_ptr<cGLSL>> ShadersMap{};

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
std::weak_ptr<cGLSL> vw_FindShaderByName(const std::string &Name)
{
	if (Name.empty())
		return std::weak_ptr<cGLSL>{};

	auto tmpShader = ShadersMap.find(Name);
	if (tmpShader != ShadersMap.end())
		return tmpShader->second;

	return std::weak_ptr<cGLSL>{};
}

/*
 * Create shader program.
 */
std::weak_ptr<cGLSL> vw_CreateShader(const std::string &ShaderName,
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
		return std::weak_ptr<cGLSL>{};

	ShadersMap.emplace(ShaderName, std::shared_ptr<cGLSL>{new cGLSL, [](cGLSL *p) {delete p;}});

	// create empty objects
	ShadersMap[ShaderName]->VertexShader = _glCreateShaderObject(GL_VERTEX_SHADER_ARB);
	ShadersMap[ShaderName]->FragmentShader = _glCreateShaderObject(GL_FRAGMENT_SHADER_ARB);

	// load vertex shader
	if (!VertexShaderFileName.empty()) {
		std::unique_ptr<sFILE> VertexFile = vw_fopen(VertexShaderFileName);

		if (VertexFile) {
			const GLcharARB *TmpGLcharARB = (const GLcharARB *)VertexFile->Data.get();
			GLint TmpGLint = (GLint)VertexFile->Size;
			_glShaderSource(ShadersMap[ShaderName]->VertexShader, 1, &TmpGLcharARB, &TmpGLint);
			vw_fclose(VertexFile);
			ShadersMap[ShaderName]->VertexShaderUse = true;
		}
	}
	// load fragment shader
	if (!FragmentShaderFileName.empty()) {
		std::unique_ptr<sFILE> FragmentFile = vw_fopen(FragmentShaderFileName);

		if (FragmentFile) {
			const GLcharARB *TmpGLcharARB = (const GLcharARB *)FragmentFile->Data.get();
			GLint TmpGLint = (GLint)FragmentFile->Size;
			_glShaderSource(ShadersMap[ShaderName]->FragmentShader, 1, &TmpGLcharARB, &TmpGLint);
			vw_fclose(FragmentFile);
			ShadersMap[ShaderName]->FragmentShaderUse = true;
		}
	}

	// compile shaders
	if (ShadersMap[ShaderName]->VertexShaderUse) {
		_glCompileShader(ShadersMap[ShaderName]->VertexShader);
		CheckOGLError(__func__);
		GLint vertCompiled{0};
		_glGetObjectParameteriv(ShadersMap[ShaderName]->VertexShader, GL_COMPILE_STATUS, &vertCompiled);
		PrintShaderInfoLog(ShadersMap[ShaderName]->VertexShader, VertexShaderFileName);

		if (!vertCompiled) {
			ShadersMap.erase(ShaderName);
			return std::weak_ptr<cGLSL>{};
		}
	}
	if (ShadersMap[ShaderName]->FragmentShaderUse) {
		_glCompileShader(ShadersMap[ShaderName]->FragmentShader);
		CheckOGLError(__func__);
		GLint fragCompiled{0};
		_glGetObjectParameteriv(ShadersMap[ShaderName]->FragmentShader, GL_COMPILE_STATUS, &fragCompiled);
		PrintShaderInfoLog(ShadersMap[ShaderName]->FragmentShader, FragmentShaderFileName);

		if (!fragCompiled) {
			ShadersMap.erase(ShaderName);
			return std::weak_ptr<cGLSL>{};
		}
	}

	// create program
	ShadersMap[ShaderName]->Program = _glCreateProgramObject();
	if (ShadersMap[ShaderName]->VertexShaderUse)
		_glAttachObject(ShadersMap[ShaderName]->Program, ShadersMap[ShaderName]->VertexShader);
	if (ShadersMap[ShaderName]->FragmentShaderUse)
		_glAttachObject(ShadersMap[ShaderName]->Program, ShadersMap[ShaderName]->FragmentShader);

	std::cout << "Shader ... " << VertexShaderFileName << " " << FragmentShaderFileName << "\n";

	return ShadersMap[ShaderName];
}

/*
 * Links a program object.
 */
bool vw_LinkShaderProgram(std::weak_ptr<cGLSL> &GLSL)
{
	if (!_glLinkProgram ||
	    !_glGetObjectParameteriv)
		return false;

	auto sharedGLSL = GLSL.lock();
	if (!sharedGLSL)
		return false;

	_glLinkProgram(sharedGLSL->Program);
	CheckOGLError(__func__);

	GLint Linked{false};
	_glGetObjectParameteriv(sharedGLSL->Program, GL_LINK_STATUS, &Linked);
	PrintProgramInfoLog(sharedGLSL->Program);

	return Linked;
}

/*
 * Installs a program object as part of current rendering state.
 */
bool vw_UseShaderProgram(std::weak_ptr<cGLSL> &GLSL)
{
	if (!_glUseProgramObject)
		return false;

	auto sharedGLSL = GLSL.lock();
	if (!sharedGLSL)
		return false;

	_glUseProgramObject(sharedGLSL->Program);
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
GLint vw_GetUniformLocation(std::weak_ptr<cGLSL> &GLSL, const std::string &Name)
{
	if (Name.empty() || !_glGetUniformLocation)
		return -1;

	auto sharedGLSL = GLSL.lock();
	if (!sharedGLSL)
		return -1;

	int tmpLocation = _glGetUniformLocation(sharedGLSL->Program, Name.c_str());
	CheckOGLError(__func__);

	if (tmpLocation == -1)
		std::cerr << __func__ << "(): " << "No such uniform named: " << Name << "\n";

	return tmpLocation;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform1i(GLint UniformLocation, int data)
{
	if (!_glUniform1i)
		return false;

	_glUniform1i(UniformLocation, data);
	CheckOGLError(__func__);

	return true;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform1f(GLint UniformLocation, float data)
{
	if (!_glUniform1f)
		return false;

	_glUniform1f(UniformLocation, data);
	CheckOGLError(__func__);

	return true;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform3f(GLint UniformLocation, float data1, float data2, float data3)
{
	if (!_glUniform3f)
		return false;

	_glUniform3f(UniformLocation, data1, data2, data3);
	CheckOGLError(__func__);

	return true;
}
