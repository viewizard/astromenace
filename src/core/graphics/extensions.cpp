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

#include "../base.h"
#include "opengl.h"
#include "SDL2/SDL.h"

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
    pfn_glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(SDL_GL_GetProcAddress("glActiveTexture"));
    pfn_glClientActiveTexture = reinterpret_cast<PFNGLCLIENTACTIVETEXTUREPROC>(SDL_GL_GetProcAddress("glClientActiveTexture"));

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
    pfn_glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(SDL_GL_GetProcAddress("glBindBuffer"));
    pfn_glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(SDL_GL_GetProcAddress("glDeleteBuffers"));
    pfn_glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(SDL_GL_GetProcAddress("glGenBuffers"));
    pfn_glIsBuffer = reinterpret_cast<PFNGLISBUFFERPROC>(SDL_GL_GetProcAddress("glIsBuffer"));
    pfn_glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(SDL_GL_GetProcAddress("glBufferData"));

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
    pfn_glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(SDL_GL_GetProcAddress("glAttachShader"));
    pfn_glBindAttribLocation = reinterpret_cast<PFNGLBINDATTRIBLOCATIONPROC>(SDL_GL_GetProcAddress("glBindAttribLocation"));
    pfn_glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(SDL_GL_GetProcAddress("glCompileShader"));
    pfn_glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(SDL_GL_GetProcAddress("glCreateProgram"));
    pfn_glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(SDL_GL_GetProcAddress("glCreateShader"));
    pfn_glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(SDL_GL_GetProcAddress("glDeleteProgram"));
    pfn_glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(SDL_GL_GetProcAddress("glDeleteShader"));
    pfn_glDetachShader = reinterpret_cast<PFNGLDETACHSHADERPROC>(SDL_GL_GetProcAddress("glDetachShader"));
    pfn_glGetActiveAttrib = reinterpret_cast<PFNGLGETACTIVEATTRIBPROC>(SDL_GL_GetProcAddress("glGetActiveAttrib"));
    pfn_glGetActiveUniform = reinterpret_cast<PFNGLGETACTIVEUNIFORMPROC>(SDL_GL_GetProcAddress("glGetActiveUniform"));
    pfn_glGetAttachedShaders = reinterpret_cast<PFNGLGETATTACHEDSHADERSPROC>(SDL_GL_GetProcAddress("glGetAttachedShaders"));
    pfn_glGetAttribLocation = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC>(SDL_GL_GetProcAddress("glGetAttribLocation"));
    pfn_glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(SDL_GL_GetProcAddress("glGetProgramiv"));
    pfn_glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(SDL_GL_GetProcAddress("glGetProgramInfoLog"));
    pfn_glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(SDL_GL_GetProcAddress("glGetShaderiv"));
    pfn_glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(SDL_GL_GetProcAddress("glGetShaderInfoLog"));
    pfn_glGetShaderSource = reinterpret_cast<PFNGLGETSHADERSOURCEPROC>(SDL_GL_GetProcAddress("glGetShaderSource"));
    pfn_glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(SDL_GL_GetProcAddress("glGetUniformLocation"));
    pfn_glGetUniformfv = reinterpret_cast<PFNGLGETUNIFORMFVPROC>(SDL_GL_GetProcAddress("glGetUniformfv"));
    pfn_glGetUniformiv = reinterpret_cast<PFNGLGETUNIFORMIVPROC>(SDL_GL_GetProcAddress("glGetUniformiv"));
    pfn_glIsProgram = reinterpret_cast<PFNGLISPROGRAMPROC>(SDL_GL_GetProcAddress("glIsProgram"));
    pfn_glIsShader = reinterpret_cast<PFNGLISSHADERPROC>(SDL_GL_GetProcAddress("glIsShader"));
    pfn_glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(SDL_GL_GetProcAddress("glLinkProgram"));
    pfn_glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(SDL_GL_GetProcAddress("glShaderSource"));
    pfn_glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(SDL_GL_GetProcAddress("glUseProgram"));
    pfn_glUniform1f = reinterpret_cast<PFNGLUNIFORM1FPROC>(SDL_GL_GetProcAddress("glUniform1f"));
    pfn_glUniform2f = reinterpret_cast<PFNGLUNIFORM2FPROC>(SDL_GL_GetProcAddress("glUniform2f"));
    pfn_glUniform3f = reinterpret_cast<PFNGLUNIFORM3FPROC>(SDL_GL_GetProcAddress("glUniform3f"));
    pfn_glUniform4f = reinterpret_cast<PFNGLUNIFORM4FPROC>(SDL_GL_GetProcAddress("glUniform4f"));
    pfn_glUniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(SDL_GL_GetProcAddress("glUniform1i"));
    pfn_glUniform2i = reinterpret_cast<PFNGLUNIFORM2IPROC>(SDL_GL_GetProcAddress("glUniform2i"));
    pfn_glUniform3i = reinterpret_cast<PFNGLUNIFORM3IPROC>(SDL_GL_GetProcAddress("glUniform3i"));
    pfn_glUniform4i = reinterpret_cast<PFNGLUNIFORM4IPROC>(SDL_GL_GetProcAddress("glUniform4i"));
    pfn_glUniform1fv = reinterpret_cast<PFNGLUNIFORM1FVPROC>(SDL_GL_GetProcAddress("glUniform1fv"));
    pfn_glUniform2fv = reinterpret_cast<PFNGLUNIFORM2FVPROC>(SDL_GL_GetProcAddress("glUniform2fv"));
    pfn_glUniform3fv = reinterpret_cast<PFNGLUNIFORM3FVPROC>(SDL_GL_GetProcAddress("glUniform3fv"));
    pfn_glUniform4fv = reinterpret_cast<PFNGLUNIFORM4FVPROC>(SDL_GL_GetProcAddress("glUniform4fv"));
    pfn_glUniform1iv = reinterpret_cast<PFNGLUNIFORM1IVPROC>(SDL_GL_GetProcAddress("glUniform1iv"));
    pfn_glUniform2iv = reinterpret_cast<PFNGLUNIFORM2IVPROC>(SDL_GL_GetProcAddress("glUniform2iv"));
    pfn_glUniform3iv = reinterpret_cast<PFNGLUNIFORM3IVPROC>(SDL_GL_GetProcAddress("glUniform3iv"));
    pfn_glUniform4iv = reinterpret_cast<PFNGLUNIFORM4IVPROC>(SDL_GL_GetProcAddress("glUniform4iv"));
    pfn_glValidateProgram = reinterpret_cast<PFNGLVALIDATEPROGRAMPROC>(SDL_GL_GetProcAddress("glValidateProgram"));

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
    pfn_glUniformMatrix2x3fv = reinterpret_cast<PFNGLUNIFORMMATRIX2X3FVPROC>(SDL_GL_GetProcAddress("glUniformMatrix2x3fv"));
    pfn_glUniformMatrix3x2fv = reinterpret_cast<PFNGLUNIFORMMATRIX3X2FVPROC>(SDL_GL_GetProcAddress("glUniformMatrix3x2fv"));
    pfn_glUniformMatrix2x4fv = reinterpret_cast<PFNGLUNIFORMMATRIX2X4FVPROC>(SDL_GL_GetProcAddress("glUniformMatrix2x4fv"));
    pfn_glUniformMatrix4x2fv = reinterpret_cast<PFNGLUNIFORMMATRIX4X2FVPROC>(SDL_GL_GetProcAddress("glUniformMatrix4x2fv"));
    pfn_glUniformMatrix3x4fv = reinterpret_cast<PFNGLUNIFORMMATRIX3X4FVPROC>(SDL_GL_GetProcAddress("glUniformMatrix3x4fv"));
    pfn_glUniformMatrix4x3fv = reinterpret_cast<PFNGLUNIFORMMATRIX4X3FVPROC>(SDL_GL_GetProcAddress("glUniformMatrix4x3fv"));

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
    pfn_glBindRenderbuffer = reinterpret_cast<PFNGLBINDRENDERBUFFERPROC>(SDL_GL_GetProcAddress("glBindRenderbuffer"));
    pfn_glDeleteRenderbuffers = reinterpret_cast<PFNGLDELETERENDERBUFFERSPROC>(SDL_GL_GetProcAddress("glDeleteRenderbuffers"));
    pfn_glGenRenderbuffers = reinterpret_cast<PFNGLGENRENDERBUFFERSPROC>(SDL_GL_GetProcAddress("glGenRenderbuffers"));
    pfn_glIsFramebuffer = reinterpret_cast<PFNGLISFRAMEBUFFERPROC>(SDL_GL_GetProcAddress("glIsFramebuffer"));
    pfn_glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(SDL_GL_GetProcAddress("glBindFramebuffer"));
    pfn_glDeleteFramebuffers = reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(SDL_GL_GetProcAddress("glDeleteFramebuffers"));
    pfn_glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(SDL_GL_GetProcAddress("glGenFramebuffers"));
    pfn_glCheckFramebufferStatus =
        reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(SDL_GL_GetProcAddress("glCheckFramebufferStatus"));
    pfn_glFramebufferTexture2D =
        reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(SDL_GL_GetProcAddress("glFramebufferTexture2D"));
    pfn_glFramebufferRenderbuffer =
        reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(SDL_GL_GetProcAddress("glFramebufferRenderbuffer"));
    pfn_glGetFramebufferAttachmentParameteriv =
        reinterpret_cast<PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC>(
            SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameteriv"));
    pfn_glGenerateMipmap = reinterpret_cast<PFNGLGENERATEMIPMAPPROC>(SDL_GL_GetProcAddress("glGenerateMipmap"));
    pfn_glBlitFramebuffer = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(SDL_GL_GetProcAddress("glBlitFramebuffer"));
    pfn_glRenderbufferStorageMultisample =
        reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC>(SDL_GL_GetProcAddress("glRenderbufferStorageMultisample"));
    pfn_glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(SDL_GL_GetProcAddress("glBindVertexArray"));
    pfn_glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(SDL_GL_GetProcAddress("glDeleteVertexArrays"));
    pfn_glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(SDL_GL_GetProcAddress("glGenVertexArrays"));
    pfn_glIsVertexArray = reinterpret_cast<PFNGLISVERTEXARRAYPROC>(SDL_GL_GetProcAddress("glIsVertexArray"));

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
    pfn_glTexStorage2D = reinterpret_cast<PFNGLTEXSTORAGE2DPROC>(SDL_GL_GetProcAddress("glTexStorage2D"));

    return pfn_glTexStorage2D;
}

/*
 * GL_NV_framebuffer_multisample_coverage initialization.
 */
bool Initialize_GL_NV_framebuffer_multisample_coverage()
{
    pfn_glRenderbufferStorageMultisampleCoverageNV =
        reinterpret_cast<PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC>(
            SDL_GL_GetProcAddress("glRenderbufferStorageMultisampleCoverageNV"));

    return pfn_glRenderbufferStorageMultisampleCoverageNV;
}

} // viewizard namespace
