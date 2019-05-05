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

#include "../base.h"
#include "opengl.h"

namespace viewizard {

// OpenGL 1.3 (only what we need or would need in future)
PFNGLACTIVETEXTUREPROC pfn_glActiveTexture{nullptr};
PFNGLCLIENTACTIVETEXTUREPROC pfn_glClientActiveTexture{nullptr};

// OpenGL 1.5 (only what we need or would need in future)
PFNGLBINDBUFFERPROC pfn_glBindBuffer{nullptr};
PFNGLDELETEBUFFERSPROC pfn_glDeleteBuffers{nullptr};
PFNGLGENBUFFERSPROC pfn_glGenBuffers{nullptr};
PFNGLISBUFFERPROC pfn_glIsBuffer{nullptr};
PFNGLBUFFERDATAPROC pfn_glBufferData{nullptr};

// OpenGL 2.0 (only what we need or would need in future)
PFNGLATTACHSHADERPROC pfn_glAttachShader{nullptr};
PFNGLBINDATTRIBLOCATIONPROC pfn_glBindAttribLocation{nullptr};
PFNGLCOMPILESHADERPROC pfn_glCompileShader{nullptr};
PFNGLCREATEPROGRAMPROC pfn_glCreateProgram{nullptr};
PFNGLCREATESHADERPROC pfn_glCreateShader{nullptr};
PFNGLDELETEPROGRAMPROC pfn_glDeleteProgram{nullptr};
PFNGLDELETESHADERPROC pfn_glDeleteShader{nullptr};
PFNGLDETACHSHADERPROC pfn_glDetachShader{nullptr};
PFNGLGETACTIVEATTRIBPROC pfn_glGetActiveAttrib{nullptr};
PFNGLGETACTIVEUNIFORMPROC pfn_glGetActiveUniform{nullptr};
PFNGLGETATTACHEDSHADERSPROC pfn_glGetAttachedShaders{nullptr};
PFNGLGETATTRIBLOCATIONPROC pfn_glGetAttribLocation{nullptr};
PFNGLGETPROGRAMIVPROC pfn_glGetProgramiv{nullptr};
PFNGLGETPROGRAMINFOLOGPROC pfn_glGetProgramInfoLog{nullptr};
PFNGLGETSHADERIVPROC pfn_glGetShaderiv{nullptr};
PFNGLGETSHADERINFOLOGPROC pfn_glGetShaderInfoLog{nullptr};
PFNGLGETSHADERSOURCEPROC pfn_glGetShaderSource{nullptr};
PFNGLGETUNIFORMLOCATIONPROC pfn_glGetUniformLocation{nullptr};
PFNGLGETUNIFORMFVPROC pfn_glGetUniformfv{nullptr};
PFNGLGETUNIFORMIVPROC pfn_glGetUniformiv{nullptr};
PFNGLISPROGRAMPROC pfn_glIsProgram{nullptr};
PFNGLISSHADERPROC pfn_glIsShader{nullptr};
PFNGLLINKPROGRAMPROC pfn_glLinkProgram{nullptr};
PFNGLSHADERSOURCEPROC pfn_glShaderSource{nullptr};
PFNGLUSEPROGRAMPROC pfn_glUseProgram{nullptr};
PFNGLUNIFORM1FPROC pfn_glUniform1f{nullptr};
PFNGLUNIFORM2FPROC pfn_glUniform2f{nullptr};
PFNGLUNIFORM3FPROC pfn_glUniform3f{nullptr};
PFNGLUNIFORM4FPROC pfn_glUniform4f{nullptr};
PFNGLUNIFORM1IPROC pfn_glUniform1i{nullptr};
PFNGLUNIFORM2IPROC pfn_glUniform2i{nullptr};
PFNGLUNIFORM3IPROC pfn_glUniform3i{nullptr};
PFNGLUNIFORM4IPROC pfn_glUniform4i{nullptr};
PFNGLUNIFORM1FVPROC pfn_glUniform1fv{nullptr};
PFNGLUNIFORM2FVPROC pfn_glUniform2fv{nullptr};
PFNGLUNIFORM3FVPROC pfn_glUniform3fv{nullptr};
PFNGLUNIFORM4FVPROC pfn_glUniform4fv{nullptr};
PFNGLUNIFORM1IVPROC pfn_glUniform1iv{nullptr};
PFNGLUNIFORM2IVPROC pfn_glUniform2iv{nullptr};
PFNGLUNIFORM3IVPROC pfn_glUniform3iv{nullptr};
PFNGLUNIFORM4IVPROC pfn_glUniform4iv{nullptr};
PFNGLVALIDATEPROGRAMPROC pfn_glValidateProgram{nullptr};

// OpenGL 2.1 (only what we need or would need in future)
PFNGLUNIFORMMATRIX2X3FVPROC pfn_glUniformMatrix2x3fv{nullptr};
PFNGLUNIFORMMATRIX3X2FVPROC pfn_glUniformMatrix3x2fv{nullptr};
PFNGLUNIFORMMATRIX2X4FVPROC pfn_glUniformMatrix2x4fv{nullptr};
PFNGLUNIFORMMATRIX4X2FVPROC pfn_glUniformMatrix4x2fv{nullptr};
PFNGLUNIFORMMATRIX3X4FVPROC pfn_glUniformMatrix3x4fv{nullptr};
PFNGLUNIFORMMATRIX4X3FVPROC pfn_glUniformMatrix4x3fv{nullptr};

// OpenGL 3.0 (only what we need or would need in future)
PFNGLBINDRENDERBUFFERPROC pfn_glBindRenderbuffer{nullptr};
PFNGLDELETERENDERBUFFERSPROC pfn_glDeleteRenderbuffers{nullptr};
PFNGLGENRENDERBUFFERSPROC pfn_glGenRenderbuffers{nullptr};
PFNGLISFRAMEBUFFERPROC pfn_glIsFramebuffer{nullptr};
PFNGLBINDFRAMEBUFFERPROC pfn_glBindFramebuffer{nullptr};
PFNGLDELETEFRAMEBUFFERSPROC pfn_glDeleteFramebuffers{nullptr};
PFNGLGENFRAMEBUFFERSPROC pfn_glGenFramebuffers{nullptr};
PFNGLCHECKFRAMEBUFFERSTATUSPROC pfn_glCheckFramebufferStatus{nullptr};
PFNGLFRAMEBUFFERTEXTURE2DPROC pfn_glFramebufferTexture2D{nullptr};
PFNGLFRAMEBUFFERRENDERBUFFERPROC pfn_glFramebufferRenderbuffer{nullptr};
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC pfn_glGetFramebufferAttachmentParameteriv{nullptr};
PFNGLGENERATEMIPMAPPROC pfn_glGenerateMipmap{nullptr};
PFNGLBLITFRAMEBUFFERPROC pfn_glBlitFramebuffer{nullptr};
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC pfn_glRenderbufferStorageMultisample{nullptr};
PFNGLBINDVERTEXARRAYPROC pfn_glBindVertexArray{nullptr};
PFNGLDELETEVERTEXARRAYSPROC pfn_glDeleteVertexArrays{nullptr};
PFNGLGENVERTEXARRAYSPROC pfn_glGenVertexArrays{nullptr};
PFNGLISVERTEXARRAYPROC pfn_glIsVertexArray{nullptr};

// OpenGL 4.2 (only what we need or would need in future)
PFNGLTEXSTORAGE2DPROC pfn_glTexStorage2D{nullptr};

// GL_NV_framebuffer_multisample_coverage
PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC pfn_glRenderbufferStorageMultisampleCoverageNV{nullptr};


/*
 * OpenGL 1.3 initialization (only what we need or would need in future).
 */
bool Initialize_OpenGL_1_3()
{
    pfn_glActiveTexture = (PFNGLACTIVETEXTUREPROC) SDL_GL_GetProcAddress("glActiveTexture");
    pfn_glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC) SDL_GL_GetProcAddress("glClientActiveTexture");

    if (!pfn_glActiveTexture
        || !pfn_glClientActiveTexture) {
        pfn_glActiveTexture = nullptr;
        pfn_glClientActiveTexture = nullptr;

        return false;
    }

    return true;
}

/*
 * OpenGL 1.5 initialization (only what we need or would need in future).
 */
bool Initialize_OpenGL_1_5()
{
    // get pointers to the GL functions
    pfn_glBindBuffer = (PFNGLBINDBUFFERPROC) SDL_GL_GetProcAddress("glBindBuffer");
    pfn_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteBuffers");
    pfn_glGenBuffers = (PFNGLGENBUFFERSPROC) SDL_GL_GetProcAddress("glGenBuffers");
    pfn_glIsBuffer = (PFNGLISBUFFERPROC) SDL_GL_GetProcAddress("glIsBuffer");
    pfn_glBufferData = (PFNGLBUFFERDATAPROC) SDL_GL_GetProcAddress("glBufferData");

    if (!pfn_glBindBuffer
        || !pfn_glDeleteBuffers
        || !pfn_glGenBuffers
        || !pfn_glIsBuffer
        || !pfn_glBufferData) {
        pfn_glBindBuffer = nullptr;
        pfn_glDeleteBuffers = nullptr;
        pfn_glGenBuffers = nullptr;
        pfn_glIsBuffer = nullptr;
        pfn_glBufferData = nullptr;

        return false;
    }

    return true;
}

/*
 * OpenGL 2.0 initialization (only what we need or would need in future).
 */
bool Initialize_OpenGL_2_0()
{
    pfn_glAttachShader = (PFNGLATTACHSHADERPROC) SDL_GL_GetProcAddress("glAttachShader");
    pfn_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) SDL_GL_GetProcAddress("glBindAttribLocation");
    pfn_glCompileShader = (PFNGLCOMPILESHADERPROC) SDL_GL_GetProcAddress("glCompileShader");
    pfn_glCreateProgram = (PFNGLCREATEPROGRAMPROC) SDL_GL_GetProcAddress("glCreateProgram");
    pfn_glCreateShader = (PFNGLCREATESHADERPROC) SDL_GL_GetProcAddress("glCreateShader");
    pfn_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) SDL_GL_GetProcAddress("glDeleteProgram");
    pfn_glDeleteShader = (PFNGLDELETESHADERPROC) SDL_GL_GetProcAddress("glDeleteShader");
    pfn_glDetachShader = (PFNGLDETACHSHADERPROC) SDL_GL_GetProcAddress("glDetachShader");
    pfn_glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC) SDL_GL_GetProcAddress("glGetActiveAttrib");
    pfn_glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC) SDL_GL_GetProcAddress("glGetActiveUniform");
    pfn_glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC) SDL_GL_GetProcAddress("glGetAttachedShaders");
    pfn_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) SDL_GL_GetProcAddress("glGetAttribLocation");
    pfn_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) SDL_GL_GetProcAddress("glGetProgramiv");
    pfn_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) SDL_GL_GetProcAddress("glGetProgramInfoLog");
    pfn_glGetShaderiv = (PFNGLGETSHADERIVPROC) SDL_GL_GetProcAddress("glGetShaderiv");
    pfn_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) SDL_GL_GetProcAddress("glGetShaderInfoLog");
    pfn_glGetShaderSource = (PFNGLGETSHADERSOURCEPROC) SDL_GL_GetProcAddress("glGetShaderSource");
    pfn_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) SDL_GL_GetProcAddress("glGetUniformLocation");
    pfn_glGetUniformfv = (PFNGLGETUNIFORMFVPROC) SDL_GL_GetProcAddress("glGetUniformfv");
    pfn_glGetUniformiv = (PFNGLGETUNIFORMIVPROC) SDL_GL_GetProcAddress("glGetUniformiv");
    pfn_glIsProgram = (PFNGLISPROGRAMPROC) SDL_GL_GetProcAddress("glIsProgram");
    pfn_glIsShader = (PFNGLISSHADERPROC) SDL_GL_GetProcAddress("glIsShader");
    pfn_glLinkProgram = (PFNGLLINKPROGRAMPROC) SDL_GL_GetProcAddress("glLinkProgram");
    pfn_glShaderSource = (PFNGLSHADERSOURCEPROC) SDL_GL_GetProcAddress("glShaderSource");
    pfn_glUseProgram = (PFNGLUSEPROGRAMPROC) SDL_GL_GetProcAddress("glUseProgram");
    pfn_glUniform1f = (PFNGLUNIFORM1FPROC) SDL_GL_GetProcAddress("glUniform1f");
    pfn_glUniform2f = (PFNGLUNIFORM2FPROC) SDL_GL_GetProcAddress("glUniform2f");
    pfn_glUniform3f = (PFNGLUNIFORM3FPROC) SDL_GL_GetProcAddress("glUniform3f");
    pfn_glUniform4f = (PFNGLUNIFORM4FPROC) SDL_GL_GetProcAddress("glUniform4f");
    pfn_glUniform1i = (PFNGLUNIFORM1IPROC) SDL_GL_GetProcAddress("glUniform1i");
    pfn_glUniform2i = (PFNGLUNIFORM2IPROC) SDL_GL_GetProcAddress("glUniform2i");
    pfn_glUniform3i = (PFNGLUNIFORM3IPROC) SDL_GL_GetProcAddress("glUniform3i");
    pfn_glUniform4i = (PFNGLUNIFORM4IPROC) SDL_GL_GetProcAddress("glUniform4i");
    pfn_glUniform1fv = (PFNGLUNIFORM1FVPROC) SDL_GL_GetProcAddress("glUniform1fv");
    pfn_glUniform2fv = (PFNGLUNIFORM2FVPROC) SDL_GL_GetProcAddress("glUniform2fv");
    pfn_glUniform3fv = (PFNGLUNIFORM3FVPROC) SDL_GL_GetProcAddress("glUniform3fv");
    pfn_glUniform4fv = (PFNGLUNIFORM4FVPROC) SDL_GL_GetProcAddress("glUniform4fv");
    pfn_glUniform1iv = (PFNGLUNIFORM1IVPROC) SDL_GL_GetProcAddress("glUniform1iv");
    pfn_glUniform2iv = (PFNGLUNIFORM2IVPROC) SDL_GL_GetProcAddress("glUniform2iv");
    pfn_glUniform3iv = (PFNGLUNIFORM3IVPROC) SDL_GL_GetProcAddress("glUniform3iv");
    pfn_glUniform4iv = (PFNGLUNIFORM4IVPROC) SDL_GL_GetProcAddress("glUniform4iv");
    pfn_glValidateProgram = (PFNGLVALIDATEPROGRAMPROC) SDL_GL_GetProcAddress("glValidateProgram");

    if (!pfn_glAttachShader
        || !pfn_glBindAttribLocation
        || !pfn_glCompileShader
        || !pfn_glCreateProgram
        || !pfn_glCreateShader
        || !pfn_glDeleteProgram
        || !pfn_glDeleteShader
        || !pfn_glDetachShader
        || !pfn_glGetActiveAttrib
        || !pfn_glGetActiveUniform
        || !pfn_glGetAttachedShaders
        || !pfn_glGetAttribLocation
        || !pfn_glGetProgramiv
        || !pfn_glGetProgramInfoLog
        || !pfn_glGetShaderiv
        || !pfn_glGetShaderInfoLog
        || !pfn_glGetShaderSource
        || !pfn_glGetUniformLocation
        || !pfn_glGetUniformfv
        || !pfn_glGetUniformiv
        || !pfn_glIsProgram
        || !pfn_glIsShader
        || !pfn_glLinkProgram
        || !pfn_glShaderSource
        || !pfn_glUseProgram
        || !pfn_glUniform1f
        || !pfn_glUniform2f
        || !pfn_glUniform3f
        || !pfn_glUniform4f
        || !pfn_glUniform1i
        || !pfn_glUniform2i
        || !pfn_glUniform3i
        || !pfn_glUniform4i
        || !pfn_glUniform1fv
        || !pfn_glUniform2fv
        || !pfn_glUniform3fv
        || !pfn_glUniform4fv
        || !pfn_glUniform1iv
        || !pfn_glUniform2iv
        || !pfn_glUniform3iv
        || !pfn_glUniform4iv
        || !pfn_glValidateProgram) {
        pfn_glAttachShader = nullptr;
        pfn_glBindAttribLocation = nullptr;
        pfn_glCompileShader = nullptr;
        pfn_glCreateProgram = nullptr;
        pfn_glCreateShader = nullptr;
        pfn_glDeleteProgram = nullptr;
        pfn_glDeleteShader = nullptr;
        pfn_glDetachShader = nullptr;
        pfn_glGetActiveAttrib = nullptr;
        pfn_glGetActiveUniform = nullptr;
        pfn_glGetAttachedShaders = nullptr;
        pfn_glGetAttribLocation = nullptr;
        pfn_glGetProgramiv = nullptr;
        pfn_glGetProgramInfoLog = nullptr;
        pfn_glGetShaderiv = nullptr;
        pfn_glGetShaderInfoLog = nullptr;
        pfn_glGetShaderSource = nullptr;
        pfn_glGetUniformLocation = nullptr;
        pfn_glGetUniformfv = nullptr;
        pfn_glGetUniformiv = nullptr;
        pfn_glIsProgram = nullptr;
        pfn_glIsShader = nullptr;
        pfn_glLinkProgram = nullptr;
        pfn_glShaderSource = nullptr;
        pfn_glUseProgram = nullptr;
        pfn_glUniform1f = nullptr;
        pfn_glUniform2f = nullptr;
        pfn_glUniform3f = nullptr;
        pfn_glUniform4f = nullptr;
        pfn_glUniform1i = nullptr;
        pfn_glUniform2i = nullptr;
        pfn_glUniform3i = nullptr;
        pfn_glUniform4i = nullptr;
        pfn_glUniform1fv = nullptr;
        pfn_glUniform2fv = nullptr;
        pfn_glUniform3fv = nullptr;
        pfn_glUniform4fv = nullptr;
        pfn_glUniform1iv = nullptr;
        pfn_glUniform2iv = nullptr;
        pfn_glUniform3iv = nullptr;
        pfn_glUniform4iv = nullptr;
        pfn_glValidateProgram = nullptr;

        return false;
    }

    return true;
}

/*
 * OpenGL 2.1 initialization (only what we need or would need in future).
 */
bool Initialize_OpenGL_2_1()
{
    pfn_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC) SDL_GL_GetProcAddress("glUniformMatrix2x3fv");
    pfn_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC) SDL_GL_GetProcAddress("glUniformMatrix3x2fv");
    pfn_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC) SDL_GL_GetProcAddress("glUniformMatrix2x4fv");
    pfn_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC) SDL_GL_GetProcAddress("glUniformMatrix4x2fv");
    pfn_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC) SDL_GL_GetProcAddress("glUniformMatrix3x4fv");
    pfn_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC) SDL_GL_GetProcAddress("glUniformMatrix4x3fv");

    if (!pfn_glUniformMatrix2x3fv
        || !pfn_glUniformMatrix3x2fv
        || !pfn_glUniformMatrix2x4fv
        || !pfn_glUniformMatrix4x2fv
        || !pfn_glUniformMatrix3x4fv
        || !pfn_glUniformMatrix4x3fv) {
        pfn_glUniformMatrix2x3fv = nullptr;
        pfn_glUniformMatrix3x2fv = nullptr;
        pfn_glUniformMatrix2x4fv = nullptr;
        pfn_glUniformMatrix4x2fv = nullptr;
        pfn_glUniformMatrix3x4fv = nullptr;
        pfn_glUniformMatrix4x3fv = nullptr;

        return false;
    }

    return true;
}

/*
 * OpenGL 3.0 initialization (only what we need or would need in future).
 */
bool Initialize_OpenGL_3_0()
{
    pfn_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) SDL_GL_GetProcAddress("glBindRenderbuffer");
    pfn_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteRenderbuffers");
    pfn_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) SDL_GL_GetProcAddress("glGenRenderbuffers");
    pfn_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) SDL_GL_GetProcAddress("glIsFramebuffer");
    pfn_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) SDL_GL_GetProcAddress("glBindFramebuffer");
    pfn_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) SDL_GL_GetProcAddress("glDeleteFramebuffers");
    pfn_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) SDL_GL_GetProcAddress("glGenFramebuffers");
    pfn_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) SDL_GL_GetProcAddress("glCheckFramebufferStatus");
    pfn_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) SDL_GL_GetProcAddress("glFramebufferTexture2D");
    pfn_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) SDL_GL_GetProcAddress("glFramebufferRenderbuffer");
    pfn_glGetFramebufferAttachmentParameteriv =
        (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameteriv");
    pfn_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) SDL_GL_GetProcAddress("glGenerateMipmap");
    pfn_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC) SDL_GL_GetProcAddress("glBlitFramebuffer");
    pfn_glRenderbufferStorageMultisample =
        (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) SDL_GL_GetProcAddress("glRenderbufferStorageMultisample");
    pfn_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) SDL_GL_GetProcAddress("glBindVertexArray");
    pfn_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) SDL_GL_GetProcAddress("glDeleteVertexArrays");
    pfn_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) SDL_GL_GetProcAddress("glGenVertexArrays");
    pfn_glIsVertexArray = (PFNGLISVERTEXARRAYPROC) SDL_GL_GetProcAddress("glIsVertexArray");

    if (!pfn_glBindRenderbuffer
        || !pfn_glDeleteRenderbuffers
        || !pfn_glGenRenderbuffers
        || !pfn_glIsFramebuffer
        || !pfn_glBindFramebuffer
        || !pfn_glDeleteFramebuffers
        || !pfn_glGenFramebuffers
        || !pfn_glCheckFramebufferStatus
        || !pfn_glFramebufferTexture2D
        || !pfn_glFramebufferRenderbuffer
        || !pfn_glGetFramebufferAttachmentParameteriv
        || !pfn_glGenerateMipmap
        || !pfn_glBlitFramebuffer
        || !pfn_glRenderbufferStorageMultisample
        || !pfn_glBindVertexArray
        || !pfn_glDeleteVertexArrays
        || !pfn_glGenVertexArrays
        || !pfn_glIsVertexArray) {
        pfn_glBindRenderbuffer = nullptr;
        pfn_glDeleteRenderbuffers = nullptr;
        pfn_glGenRenderbuffers = nullptr;
        pfn_glIsFramebuffer = nullptr;
        pfn_glBindFramebuffer = nullptr;
        pfn_glDeleteFramebuffers = nullptr;
        pfn_glGenFramebuffers = nullptr;
        pfn_glCheckFramebufferStatus = nullptr;
        pfn_glFramebufferTexture2D = nullptr;
        pfn_glFramebufferRenderbuffer = nullptr;
        pfn_glGetFramebufferAttachmentParameteriv = nullptr;
        pfn_glGenerateMipmap = nullptr;
        pfn_glBlitFramebuffer = nullptr;
        pfn_glRenderbufferStorageMultisample = nullptr;
        pfn_glBindVertexArray = nullptr;
        pfn_glDeleteVertexArrays = nullptr;
        pfn_glGenVertexArrays = nullptr;
        pfn_glIsVertexArray = nullptr;

        return false;
    }

    return true;
}

/*
 * OpenGL 4.2 initialization (only what we need or would need in future).
 */
bool Initialize_OpenGL_4_2()
{
    pfn_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC) SDL_GL_GetProcAddress("glTexStorage2D");

    return pfn_glTexStorage2D;
}

/*
 * GL_NV_framebuffer_multisample_coverage initialization.
 */
bool Initialize_GL_NV_framebuffer_multisample_coverage()
{
    pfn_glRenderbufferStorageMultisampleCoverageNV =
        (PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC)
            SDL_GL_GetProcAddress("glRenderbufferStorageMultisampleCoverageNV");

    return pfn_glRenderbufferStorageMultisampleCoverageNV;
}

} // viewizard namespace
