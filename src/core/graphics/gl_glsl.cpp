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

// TODO shaders uniforms should be managed inside 'gl_glsl'

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
	GLuint Program{0};
	GLuint VertexShader{0};
	GLuint FragmentShader{0};
	std::vector<GLint> UniformLocations{};

private:
	// Don't allow direct new/delete usage in code, only vw_CreateShader()
	// allowed for shaders creation and release setup (deleter must be provided).
	cGLSL() = default;
	~cGLSL() {
		if (_glDetachShader && _glDeleteShader &&
		    _glIsShader && _glIsProgram && _glDeleteProgram) {
			if (Program && _glIsProgram(Program)) {
				if (VertexShader && _glIsShader(VertexShader))
					_glDetachShader(Program, VertexShader);
				if (FragmentShader && _glIsShader(FragmentShader))
					_glDetachShader(Program, FragmentShader);
				_glDeleteProgram(Program);
			}
			if (VertexShader && _glIsShader(VertexShader))
				_glDeleteShader(VertexShader);
			if (FragmentShader && _glIsShader(FragmentShader))
				_glDeleteShader(FragmentShader);
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
static void PrintShaderInfoLog(GLuint shader, const std::string &ShaderName)
{
	if (!_glGetShaderiv ||
	    !_glGetShaderInfoLog)
		return;

	CheckOGLError(__func__);

	int infologLength{0};
	_glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError(__func__);

	if (infologLength > 0) {
		std::unique_ptr<GLchar[]> infoLog{new GLchar[infologLength]};
		int charsWritten{0};
		_glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog.get());
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
static void PrintProgramInfoLog(GLuint program)
{
	if (!_glGetProgramiv ||
	    !_glGetProgramInfoLog)
		return;

	CheckOGLError(__func__);

	int infologLength{0};
	_glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError(__func__);

	if (infologLength > 0) {
		std::unique_ptr<GLchar[]> infoLog{new GLchar[infologLength]};
		int charsWritten{0};
		_glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog.get());
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
 * Find and store uniform location in shader. Return internal storage number for future use.
 */
int vw_FindShaderUniformLocation(std::weak_ptr<cGLSL> &GLSL, const std::string &UniformName)
{
	// lock shader before vw_GetUniformLocation() call
	if (auto sharedGLSL = GLSL.lock()) {
		GLint tmpUniformLocation = vw_GetUniformLocation(GLSL, UniformName);
		if (tmpUniformLocation != -1) {
			sharedGLSL.get()->UniformLocations.emplace_back(tmpUniformLocation);
			// we are safe with static_cast here, since shader's uniforms number low
			return static_cast<int>(sharedGLSL.get()->UniformLocations.size() - 1);
		} else
			std::cerr << __func__ << "(): " << "Can't find uniform name " << UniformName << "\n";
	}

	return -1;
}

/*
 *  Get previously found in shader uniform's location by internal storage number.
 */
GLint vw_GetShaderUniformLocation(std::shared_ptr<cGLSL> sharedGLSL, int UniformNumber)
{
	if (!sharedGLSL)
		return -1;

	// no checks here for better speed
	return sharedGLSL.get()->UniformLocations[UniformNumber];
}

/*
 * Create shader program.
 */
std::weak_ptr<cGLSL> vw_CreateShader(const std::string &ShaderName,
				     const std::string &VertexShaderFileName,
				     const std::string &FragmentShaderFileName)
{
	if (ShaderName.empty() ||
	    !_glCreateShader ||
	    !_glShaderSource ||
	    !_glCompileShader ||
	    !_glCreateProgram ||
	    !_glAttachShader ||
	    !_glGetShaderiv ||
	    (VertexShaderFileName.empty() && FragmentShaderFileName.empty()))
		return std::weak_ptr<cGLSL>{};

	ShadersMap.emplace(ShaderName, std::shared_ptr<cGLSL>{new cGLSL, [](cGLSL *p) {delete p;}});

	// load vertex shader
	if (!VertexShaderFileName.empty()) {
		// create empty object
		ShadersMap[ShaderName]->VertexShader = _glCreateShader(GL_VERTEX_SHADER);

		std::unique_ptr<sFILE> VertexFile = vw_fopen(VertexShaderFileName);

		if (!VertexFile) {
			ShadersMap.erase(ShaderName);
			std::cerr << __func__ << "(): " << "Can't find file " << VertexShaderFileName << "\n";
			return std::weak_ptr<cGLSL>{};
		}

		const GLchar *TmpGLchar = (const GLchar *)VertexFile->Data.get();
		GLint TmpGLint = (GLint)VertexFile->Size;
		_glShaderSource(ShadersMap[ShaderName]->VertexShader, 1, &TmpGLchar, &TmpGLint);
		vw_fclose(VertexFile);
	}
	// load fragment shader
	if (!FragmentShaderFileName.empty()) {
		// create empty object
		ShadersMap[ShaderName]->FragmentShader = _glCreateShader(GL_FRAGMENT_SHADER);

		std::unique_ptr<sFILE> FragmentFile = vw_fopen(FragmentShaderFileName);

		if (!FragmentFile) {
			ShadersMap.erase(ShaderName);
			std::cerr << __func__ << "(): " << "Can't find file " << FragmentShaderFileName << "\n";
			return std::weak_ptr<cGLSL>{};
		}

		const GLchar *TmpGLchar = (const GLchar *)FragmentFile->Data.get();
		GLint TmpGLint = (GLint)FragmentFile->Size;
		_glShaderSource(ShadersMap[ShaderName]->FragmentShader, 1, &TmpGLchar, &TmpGLint);
		vw_fclose(FragmentFile);
	}

	// compile shaders
	if (ShadersMap[ShaderName]->VertexShader) {
		_glCompileShader(ShadersMap[ShaderName]->VertexShader);
		CheckOGLError(__func__);
		GLint vertCompiled{0};
		_glGetShaderiv(ShadersMap[ShaderName]->VertexShader, GL_COMPILE_STATUS, &vertCompiled);
		PrintShaderInfoLog(ShadersMap[ShaderName]->VertexShader, VertexShaderFileName);

		if (!vertCompiled) {
			ShadersMap.erase(ShaderName);
			return std::weak_ptr<cGLSL>{};
		}
	}
	if (ShadersMap[ShaderName]->FragmentShader) {
		_glCompileShader(ShadersMap[ShaderName]->FragmentShader);
		CheckOGLError(__func__);
		GLint fragCompiled{0};
		_glGetShaderiv(ShadersMap[ShaderName]->FragmentShader, GL_COMPILE_STATUS, &fragCompiled);
		PrintShaderInfoLog(ShadersMap[ShaderName]->FragmentShader, FragmentShaderFileName);

		if (!fragCompiled) {
			ShadersMap.erase(ShaderName);
			return std::weak_ptr<cGLSL>{};
		}
	}

	// create program
	ShadersMap[ShaderName]->Program = _glCreateProgram();
	if (ShadersMap[ShaderName]->VertexShader)
		_glAttachShader(ShadersMap[ShaderName]->Program, ShadersMap[ShaderName]->VertexShader);
	if (ShadersMap[ShaderName]->FragmentShader)
		_glAttachShader(ShadersMap[ShaderName]->Program, ShadersMap[ShaderName]->FragmentShader);

	std::cout << "Shader ... " << VertexShaderFileName << " " << FragmentShaderFileName << "\n";

	return ShadersMap[ShaderName];
}

/*
 * Links a program object.
 */
bool vw_LinkShaderProgram(std::weak_ptr<cGLSL> &GLSL)
{
	if (!_glLinkProgram ||
	    !_glGetProgramiv)
		return false;

	auto sharedGLSL = GLSL.lock();
	if (!sharedGLSL)
		return false;

	_glLinkProgram(sharedGLSL->Program);
	CheckOGLError(__func__);

	GLint Linked{false};
	_glGetProgramiv(sharedGLSL->Program, GL_LINK_STATUS, &Linked);
	PrintProgramInfoLog(sharedGLSL->Program);

	return Linked;
}

/*
 * Installs a program object as part of current rendering state.
 */
bool vw_UseShaderProgram(std::shared_ptr<cGLSL> &sharedGLSL)
{
	if (!_glUseProgram ||
	    !sharedGLSL)
		return false;

	_glUseProgram(sharedGLSL->Program);
	CheckOGLError(__func__);

	return true;
}

/*
 * Installs a program object as part of current rendering state.
 */
bool vw_UseShaderProgram(std::weak_ptr<cGLSL> &GLSL)
{
	auto sharedGLSL = GLSL.lock();
	return vw_UseShaderProgram(sharedGLSL);
}

/*
 * Switch to fixed-function program object as part of current rendering state.
 */
bool vw_StopShaderProgram()
{
	if (!_glUseProgram)
		return false;

	_glUseProgram(0);
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
