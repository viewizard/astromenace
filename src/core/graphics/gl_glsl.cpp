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

// TODO shaders uniforms should be managed inside 'gl_glsl'

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

// NOTE since OpenGL 3.1 core profile, OpenGL must render with a program,
//      vw_StopShaderProgram() should be removed

// TODO struct cGLSL should use encapsulation (switch to class)

#include "../vfs/vfs.h"
#include "graphics.h"
#include "extensions.h"

namespace viewizard {

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
    ~cGLSL()
    {
        if (pfn_glDetachShader && pfn_glDeleteShader && pfn_glIsShader && pfn_glIsProgram && pfn_glDeleteProgram) {
            if (Program && pfn_glIsProgram(Program)) {
                if (VertexShader && pfn_glIsShader(VertexShader)) {
                    pfn_glDetachShader(Program, VertexShader);
                }
                if (FragmentShader && pfn_glIsShader(FragmentShader)) {
                    pfn_glDetachShader(Program, FragmentShader);
                }
                pfn_glDeleteProgram(Program);
            }
            if (VertexShader && pfn_glIsShader(VertexShader)) {
                pfn_glDeleteShader(VertexShader);
            }
            if (FragmentShader && pfn_glIsShader(FragmentShader)) {
                pfn_glDeleteShader(FragmentShader);
            }
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
    if (!pfn_glGetShaderiv || !pfn_glGetShaderInfoLog) {
        return;
    }

    CheckOGLError(__func__);

    int infologLength{0};
    pfn_glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

    CheckOGLError(__func__);

    if (infologLength > 0) {
        std::unique_ptr<GLchar[]> infoLog{new GLchar[infologLength]};
        int charsWritten{0};
        pfn_glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog.get());
        // at this line, infoLog.get() should contains null-terminated string
        if (charsWritten) {
            std::cout << "Shader InfoLog " << ShaderName << ":\n" << infoLog.get() << "\n\n";
        } else {
            std::cout << "Shader InfoLog:\n" << "no log provided" << "\n\n";
        }
    }
    CheckOGLError(__func__);
}

/*
 * Print out the information log.
 */
static void PrintProgramInfoLog(GLuint program)
{
    if (!pfn_glGetProgramiv || !pfn_glGetProgramInfoLog) {
        return;
    }

    CheckOGLError(__func__);

    int infologLength{0};
    pfn_glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

    CheckOGLError(__func__);

    if (infologLength > 0) {
        std::unique_ptr<GLchar[]> infoLog{new GLchar[infologLength]};
        int charsWritten{0};
        pfn_glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog.get());
        // at this line, infoLog.get() should contains null-terminated string
        if (charsWritten) {
            std::cout << "Program InfoLog:\n" << infoLog.get() << "\n\n";
        } else {
            std::cout << "Program InfoLog:\n" << "no log provided" << "\n\n";
        }
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
    if (Name.empty()) {
        return std::weak_ptr<cGLSL>{};
    }

    auto tmpShader = ShadersMap.find(Name);
    if (tmpShader != ShadersMap.end()) {
        return tmpShader->second;
    }

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
        } else {
            std::cerr << __func__ << "(): " << "Can't find uniform name " << UniformName << "\n";
        }
    }

    return -1;
}

/*
 *  Get previously found in shader uniform's location by internal storage number.
 */
GLint vw_GetShaderUniformLocation(std::shared_ptr<cGLSL> sharedGLSL, int UniformNumber)
{
    if (!sharedGLSL) {
        return -1;
    }

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
    if (ShaderName.empty()
        || !pfn_glCreateShader
        || !pfn_glShaderSource
        || !pfn_glCompileShader
        || !pfn_glCreateProgram
        || !pfn_glAttachShader
        || !pfn_glGetShaderiv
        || (VertexShaderFileName.empty() && FragmentShaderFileName.empty())) {
        return std::weak_ptr<cGLSL>{};
    }

    ShadersMap.emplace(ShaderName, std::shared_ptr<cGLSL>{new cGLSL, [](cGLSL *p) {delete p;}});

    // load vertex shader
    if (!VertexShaderFileName.empty()) {
        // create empty object
        ShadersMap[ShaderName]->VertexShader = pfn_glCreateShader(GL_VERTEX_SHADER);

        std::unique_ptr<cFILE> VertexFile = vw_fopen(VertexShaderFileName);

        if (!VertexFile) {
            ShadersMap.erase(ShaderName);
            std::cerr << __func__ << "(): " << "Can't find file " << VertexShaderFileName << "\n";
            return std::weak_ptr<cGLSL>{};
        }

        const GLchar *TmpGLchar = (const GLchar *)VertexFile->GetData();
        GLint TmpGLint = (GLint)VertexFile->GetSize();
        pfn_glShaderSource(ShadersMap[ShaderName]->VertexShader, 1, &TmpGLchar, &TmpGLint);
        vw_fclose(VertexFile);
    }
    // load fragment shader
    if (!FragmentShaderFileName.empty()) {
        // create empty object
        ShadersMap[ShaderName]->FragmentShader = pfn_glCreateShader(GL_FRAGMENT_SHADER);

        std::unique_ptr<cFILE> FragmentFile = vw_fopen(FragmentShaderFileName);

        if (!FragmentFile) {
            ShadersMap.erase(ShaderName);
            std::cerr << __func__ << "(): " << "Can't find file " << FragmentShaderFileName << "\n";
            return std::weak_ptr<cGLSL>{};
        }

        const GLchar *TmpGLchar = (const GLchar *)FragmentFile->GetData();
        GLint TmpGLint = (GLint)FragmentFile->GetSize();
        pfn_glShaderSource(ShadersMap[ShaderName]->FragmentShader, 1, &TmpGLchar, &TmpGLint);
        vw_fclose(FragmentFile);
    }

    // compile shaders
    if (ShadersMap[ShaderName]->VertexShader) {
        pfn_glCompileShader(ShadersMap[ShaderName]->VertexShader);
        CheckOGLError(__func__);
        GLint vertCompiled{0};
        pfn_glGetShaderiv(ShadersMap[ShaderName]->VertexShader, GL_COMPILE_STATUS, &vertCompiled);
        PrintShaderInfoLog(ShadersMap[ShaderName]->VertexShader, VertexShaderFileName);

        if (!vertCompiled) {
            ShadersMap.erase(ShaderName);
            return std::weak_ptr<cGLSL>{};
        }
    }
    if (ShadersMap[ShaderName]->FragmentShader) {
        pfn_glCompileShader(ShadersMap[ShaderName]->FragmentShader);
        CheckOGLError(__func__);
        GLint fragCompiled{0};
        pfn_glGetShaderiv(ShadersMap[ShaderName]->FragmentShader, GL_COMPILE_STATUS, &fragCompiled);
        PrintShaderInfoLog(ShadersMap[ShaderName]->FragmentShader, FragmentShaderFileName);

        if (!fragCompiled) {
            ShadersMap.erase(ShaderName);
            return std::weak_ptr<cGLSL>{};
        }
    }

    // create program
    ShadersMap[ShaderName]->Program = pfn_glCreateProgram();
    if (ShadersMap[ShaderName]->VertexShader) {
        pfn_glAttachShader(ShadersMap[ShaderName]->Program, ShadersMap[ShaderName]->VertexShader);
    }
    if (ShadersMap[ShaderName]->FragmentShader) {
        pfn_glAttachShader(ShadersMap[ShaderName]->Program, ShadersMap[ShaderName]->FragmentShader);
    }

    std::cout << "Shader ... " << VertexShaderFileName << " " << FragmentShaderFileName << "\n";

    return ShadersMap[ShaderName];
}

/*
 * Links a program object.
 */
bool vw_LinkShaderProgram(std::weak_ptr<cGLSL> &GLSL)
{
    if (!pfn_glLinkProgram || !pfn_glGetProgramiv) {
        return false;
    }

    auto sharedGLSL = GLSL.lock();
    if (!sharedGLSL) {
        return false;
    }

    pfn_glLinkProgram(sharedGLSL->Program);
    CheckOGLError(__func__);

    GLint Linked{false};
    pfn_glGetProgramiv(sharedGLSL->Program, GL_LINK_STATUS, &Linked);
    PrintProgramInfoLog(sharedGLSL->Program);

    return Linked;
}

/*
 * Installs a program object as part of current rendering state.
 */
bool vw_UseShaderProgram(std::shared_ptr<cGLSL> &sharedGLSL)
{
    if (!pfn_glUseProgram || !sharedGLSL) {
        return false;
    }

    pfn_glUseProgram(sharedGLSL->Program);
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
    if (!pfn_glUseProgram) {
        return false;
    }

    pfn_glUseProgram(0);
    CheckOGLError(__func__);

    return true;
}

/*
 * Returns the location of a uniform variable.
 */
GLint vw_GetUniformLocation(std::weak_ptr<cGLSL> &GLSL, const std::string &Name)
{
    if (Name.empty() || !pfn_glGetUniformLocation) {
        return -1;
    }

    auto sharedGLSL = GLSL.lock();
    if (!sharedGLSL) {
        return -1;
    }

    int tmpLocation = pfn_glGetUniformLocation(sharedGLSL->Program, Name.c_str());
    CheckOGLError(__func__);

    if (tmpLocation == -1) {
        std::cerr << __func__ << "(): " << "No such uniform named: " << Name << "\n";
    }

    return tmpLocation;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform1i(GLint UniformLocation, int data)
{
    if (!pfn_glUniform1i) {
        return false;
    }

    pfn_glUniform1i(UniformLocation, data);
    CheckOGLError(__func__);

    return true;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform1f(GLint UniformLocation, float data)
{
    if (!pfn_glUniform1f) {
        return false;
    }

    pfn_glUniform1f(UniformLocation, data);
    CheckOGLError(__func__);

    return true;
}

/*
 * Specify the value of a uniform variable for the current program object.
 */
bool vw_Uniform3f(GLint UniformLocation, float data1, float data2, float data3)
{
    if (!pfn_glUniform3f) {
        return false;
    }

    pfn_glUniform3f(UniformLocation, data1, data2, data3);
    CheckOGLError(__func__);

    return true;
}

} // viewizard namespace
