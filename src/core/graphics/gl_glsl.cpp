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

// TODO translate comments
// TODO move from pointers to std::shared_ptr/std::weak_ptr
// TODO move to std::string usage in code
// TODO move to std::unordered_map usage for shaders management

#include "../vfs/vfs.h"
#include "graphics.h"

namespace {

// GL_ARB_shader_objects
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB{nullptr};
PFNGLGETHANDLEARBPROC glGetHandleARB{nullptr};
PFNGLDETACHOBJECTARBPROC glDetachObjectARB{nullptr};
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB{nullptr};
PFNGLSHADERSOURCEARBPROC glShaderSourceARB{nullptr};
PFNGLCOMPILESHADERARBPROC glCompileShaderARB{nullptr};
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB{nullptr};
PFNGLATTACHOBJECTARBPROC glAttachObjectARB{nullptr};
PFNGLLINKPROGRAMARBPROC glLinkProgramARB{nullptr};
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB{nullptr};
PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB{nullptr};
PFNGLUNIFORM1FARBPROC glUniform1fARB{nullptr};
PFNGLUNIFORM2FARBPROC glUniform2fARB{nullptr};
PFNGLUNIFORM3FARBPROC glUniform3fARB{nullptr};
PFNGLUNIFORM4FARBPROC glUniform4fARB{nullptr};
PFNGLUNIFORM1IARBPROC glUniform1iARB{nullptr};
PFNGLUNIFORM2IARBPROC glUniform2iARB{nullptr};
PFNGLUNIFORM3IARBPROC glUniform3iARB{nullptr};
PFNGLUNIFORM4IARBPROC glUniform4iARB{nullptr};
PFNGLUNIFORM1FVARBPROC glUniform1fvARB{nullptr};
PFNGLUNIFORM2FVARBPROC glUniform2fvARB{nullptr};
PFNGLUNIFORM3FVARBPROC glUniform3fvARB{nullptr};
PFNGLUNIFORM4FVARBPROC glUniform4fvARB{nullptr};
PFNGLUNIFORM1IVARBPROC glUniform1ivARB{nullptr};
PFNGLUNIFORM2IVARBPROC glUniform2ivARB{nullptr};
PFNGLUNIFORM3IVARBPROC glUniform3ivARB{nullptr};
PFNGLUNIFORM4IVARBPROC glUniform4ivARB{nullptr};
PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fvARB{nullptr};
PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB{nullptr};
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB{nullptr};
PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB{nullptr};
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB{nullptr};
PFNGLGETINFOLOGARBPROC glGetInfoLogARB{nullptr};
PFNGLGETATTACHEDOBJECTSARBPROC glGetAttachedObjectsARB{nullptr};
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB{nullptr};
PFNGLGETACTIVEUNIFORMARBPROC glGetActiveUniformARB{nullptr};
PFNGLGETUNIFORMFVARBPROC glGetUniformfvARB{nullptr};
PFNGLGETUNIFORMIVARBPROC glGetUniformivARB{nullptr};
PFNGLGETSHADERSOURCEARBPROC glGetShaderSourceARB{nullptr};

// для менеджера
sGLSL *StartGLSLMan{nullptr};
sGLSL *EndGLSLMan{nullptr};
int NumGLSLMan{0};

} // unnamed namespace


//------------------------------------------------------------------------------------
// иним и подключаем все указатели
//------------------------------------------------------------------------------------
bool vw_Internal_InitializationGLSL()
{
	// GL_ARB_shader_objects
	glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) SDL_GL_GetProcAddress("glDeleteObjectARB");
	glGetHandleARB = (PFNGLGETHANDLEARBPROC) SDL_GL_GetProcAddress("glGetHandleARB");
	glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC) SDL_GL_GetProcAddress("glDetachObjectARB");
	glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) SDL_GL_GetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glShaderSourceARB");
	glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) SDL_GL_GetProcAddress("glCompileShaderARB");
	glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glCreateProgramObjectARB");
	glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) SDL_GL_GetProcAddress("glAttachObjectARB");
	glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) SDL_GL_GetProcAddress("glLinkProgramARB");
	glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");
	glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC) SDL_GL_GetProcAddress("glValidateProgramARB");
	glUniform1fARB = (PFNGLUNIFORM1FARBPROC) SDL_GL_GetProcAddress("glUniform1fARB");
	glUniform2fARB = (PFNGLUNIFORM2FARBPROC) SDL_GL_GetProcAddress("glUniform2fARB");
	glUniform3fARB = (PFNGLUNIFORM3FARBPROC) SDL_GL_GetProcAddress("glUniform3fARB");
	glUniform4fARB = (PFNGLUNIFORM4FARBPROC) SDL_GL_GetProcAddress("glUniform4fARB");
	glUniform1iARB = (PFNGLUNIFORM1IARBPROC) SDL_GL_GetProcAddress("glUniform1iARB");
	glUniform2iARB = (PFNGLUNIFORM2IARBPROC) SDL_GL_GetProcAddress("glUniform2iARB");
	glUniform3iARB = (PFNGLUNIFORM3IARBPROC) SDL_GL_GetProcAddress("glUniform3iARB");
	glUniform4iARB = (PFNGLUNIFORM4IARBPROC) SDL_GL_GetProcAddress("glUniform4iARB");
	glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC) SDL_GL_GetProcAddress("glUniform1fvARB");
	glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC) SDL_GL_GetProcAddress("glUniform2fvARB");
	glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC) SDL_GL_GetProcAddress("glUniform3fvARB");
	glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC) SDL_GL_GetProcAddress("glUniform4fvARB");
	glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC) SDL_GL_GetProcAddress("glUniform1ivARB");
	glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC) SDL_GL_GetProcAddress("glUniform2ivARB");
	glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC) SDL_GL_GetProcAddress("glUniform3ivARB");
	glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC) SDL_GL_GetProcAddress("glUniform4ivARB");
	glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC) SDL_GL_GetProcAddress("glUniformMatrix2fvARB");
	glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC) SDL_GL_GetProcAddress("glUniformMatrix3fvARB");
	glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC) SDL_GL_GetProcAddress("glUniformMatrix4fvARB");
	glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterfvARB");
	glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterivARB");
	glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) SDL_GL_GetProcAddress("glGetInfoLogARB");
	glGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC) SDL_GL_GetProcAddress("glGetAttachedObjectsARB");
	glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) SDL_GL_GetProcAddress("glGetUniformLocationARB");
	glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC) SDL_GL_GetProcAddress("glGetActiveUniformARB");
	glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC) SDL_GL_GetProcAddress("glGetUniformfvARB");
	glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC) SDL_GL_GetProcAddress("glGetUniformivARB");
	glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glGetShaderSourceARB");

	// инициализация менеджера
	StartGLSLMan = nullptr;
	EndGLSLMan = nullptr;
	NumGLSLMan = 0;

	if (glDeleteObjectARB == nullptr || glGetHandleARB == nullptr || glDetachObjectARB == nullptr || glCreateShaderObjectARB == nullptr ||
	    glShaderSourceARB == nullptr || glCompileShaderARB == nullptr || glCreateProgramObjectARB == nullptr || glAttachObjectARB == nullptr ||
	    glLinkProgramARB == nullptr || glUseProgramObjectARB == nullptr || glValidateProgramARB == nullptr || glUniform1fARB == nullptr ||
	    glUniform2fARB == nullptr || glUniform3fARB == nullptr || glUniform4fARB == nullptr || glUniform1iARB == nullptr ||
	    glUniform2iARB == nullptr || glUniform3iARB == nullptr || glUniform4iARB == nullptr || glUniform1fvARB == nullptr ||
	    glUniform2fvARB == nullptr || glUniform3fvARB == nullptr || glUniform4fvARB == nullptr || glUniform1ivARB == nullptr ||
	    glUniform2ivARB == nullptr || glUniform3ivARB == nullptr || glUniform4ivARB == nullptr || glUniformMatrix2fvARB == nullptr ||
	    glUniformMatrix3fvARB == nullptr || glUniformMatrix4fvARB == nullptr || glGetObjectParameterfvARB == nullptr ||
	    glGetObjectParameterivARB == nullptr || glGetInfoLogARB == nullptr || glGetAttachedObjectsARB == nullptr ||
	    glGetUniformLocationARB == nullptr || glGetActiveUniformARB == nullptr || glGetUniformfvARB == nullptr || glGetUniformivARB == nullptr ||
	    glGetShaderSourceARB == nullptr) {
		glDeleteObjectARB = nullptr;
		glGetHandleARB = nullptr;
		glDetachObjectARB = nullptr;
		glCreateShaderObjectARB = nullptr;
		glShaderSourceARB = nullptr;
		glCompileShaderARB = nullptr;
		glCreateProgramObjectARB = nullptr;
		glAttachObjectARB = nullptr;
		glLinkProgramARB = nullptr;
		glUseProgramObjectARB = nullptr;
		glValidateProgramARB = nullptr;
		glUniform1fARB = nullptr;
		glUniform2fARB = nullptr;
		glUniform3fARB = nullptr;
		glUniform4fARB = nullptr;
		glUniform1iARB = nullptr;
		glUniform2iARB = nullptr;
		glUniform3iARB = nullptr;
		glUniform4iARB = nullptr;
		glUniform1fvARB = nullptr;
		glUniform2fvARB = nullptr;
		glUniform3fvARB = nullptr;
		glUniform4fvARB = nullptr;
		glUniform1ivARB = nullptr;
		glUniform2ivARB = nullptr;
		glUniform3ivARB = nullptr;
		glUniform4ivARB = nullptr;
		glUniformMatrix2fvARB = nullptr;
		glUniformMatrix3fvARB = nullptr;
		glUniformMatrix4fvARB = nullptr;
		glGetObjectParameterfvARB = nullptr;
		glGetObjectParameterivARB = nullptr;
		glGetInfoLogARB = nullptr;
		glGetAttachedObjectsARB = nullptr;
		glGetUniformLocationARB = nullptr;
		glGetActiveUniformARB = nullptr;
		glGetUniformfvARB = nullptr;
		glGetUniformivARB = nullptr;
		glGetShaderSourceARB = nullptr;

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------
// ошибка
//------------------------------------------------------------------------------------
static int CheckOGLError(const char *FunctionName)
{
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	GLenum glErr;
	int retCode{0};

	glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "glError " << glErr << "\n";
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}

//------------------------------------------------------------------------------------
// Print out the information log for a shader object
//------------------------------------------------------------------------------------
void vw_PrintShaderInfoLog(GLhandleARB shader, const char *ShaderName)
{
	if (!glGetObjectParameterivARB ||
	    !glGetInfoLogARB)
		return;

	int infologLength{0};
	int charsWritten{0};

	CheckOGLError(__func__);  // Check for OpenGL errors

	glGetObjectParameterivARB(shader, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError(__func__);  // Check for OpenGL errors

	if (infologLength > 0) {
		GLchar *infoLog = (GLchar *)malloc(infologLength);
		if (!infoLog) {
			std::cerr << __func__ << "(): " << "Could not allocate InfoLog buffer.\n";
			return;
		}
		glGetInfoLogARB(shader, infologLength, &charsWritten, infoLog);
		if (strlen(infoLog) >1)
			std::cout << "Shader InfoLog " << ShaderName << ":\n" << infoLog << "\n\n";
		free(infoLog);
	}
	CheckOGLError(__func__);  // Check for OpenGL errors
}

void vw_PrintProgramInfoLog(GLhandleARB program)
{
	if (!glGetObjectParameterivARB ||
	    !glGetInfoLogARB)
		return;

	int infologLength{0};
	int charsWritten{0};

	CheckOGLError(__func__);  // Check for OpenGL errors

	glGetObjectParameterivARB(program, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError(__func__);  // Check for OpenGL errors

	if (infologLength > 0) {
		GLchar *infoLog = (GLchar *)malloc(infologLength);
		if (!infoLog) {
			std::cerr << __func__ << "(): " << "Could not allocate InfoLog buffer.\n";
			return;
		}
		glGetInfoLogARB(program, infologLength, &charsWritten, infoLog);
		if (strlen(infoLog) >1)
			std::cout << "Program InfoLog:\n" << infoLog << "\n\n";
		free(infoLog);
	}
	CheckOGLError(__func__);  // Check for OpenGL errors
}

//------------------------------------------------------------------------------------
// освобождаем все шейдеры подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllShaders()
{
	if (!glDetachObjectARB ||
	    !glDeleteObjectARB)
		return;

	// Чистка памяти...
	sGLSL *Tmp = StartGLSLMan;
	while (Tmp) {
		sGLSL *Tmp1 = Tmp->Next;

		vw_DetachShader(Tmp);

		// открепляем хидеры шейдеров
		if (Tmp->VertexShaderUse)
			glDetachObjectARB(Tmp->Program, Tmp->VertexShader);
		if (Tmp->FragmentShaderUse)
			glDetachObjectARB(Tmp->Program, Tmp->FragmentShader);
		// удаляем
		glDeleteObjectARB(Tmp->VertexShader);
		glDeleteObjectARB(Tmp->FragmentShader);
		glDeleteObjectARB(Tmp->Program);

		if (Tmp->Name) {
			delete [] Tmp->Name;
			Tmp->Name = nullptr;
		}

		// удаляем указатель
		delete Tmp;

		Tmp = Tmp1;
	}

	StartGLSLMan = nullptr;
	EndGLSLMan = nullptr;
	NumGLSLMan = 0;
}

//------------------------------------------------------------------------------------
// подключение к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachShader(sGLSL* GLSL)
{
	if (!GLSL)
		return;

	// первый в списке...
	if (EndGLSLMan == nullptr) {
		GLSL->Prev = nullptr;
		GLSL->Next = nullptr;
		NumGLSLMan += 1;
		GLSL->Num = NumGLSLMan;
		StartGLSLMan = GLSL;
		EndGLSLMan = GLSL;
	} else {
		GLSL->Prev = EndGLSLMan;
		GLSL->Next = nullptr;
		EndGLSLMan->Next = GLSL;
		NumGLSLMan += 1;
		GLSL->Num = NumGLSLMan;
		EndGLSLMan = GLSL;
	}
}

//------------------------------------------------------------------------------------
// отключение от менеджера
//------------------------------------------------------------------------------------
void vw_DetachShader(sGLSL* GLSL)
{
	if (!GLSL)
		return;

	// переустанавливаем указатели...
	if (StartGLSLMan == GLSL)
		StartGLSLMan = GLSL->Next;
	if (EndGLSLMan == GLSL)
		EndGLSLMan = GLSL->Prev;

	if (GLSL->Next != nullptr)
		GLSL->Next->Prev = GLSL->Prev;
	else if (GLSL->Prev != nullptr)
		GLSL->Prev->Next = nullptr;

	if (GLSL->Prev != nullptr)
		GLSL->Prev->Next = GLSL->Next;
	else if (GLSL->Next != nullptr)
		GLSL->Next->Prev = nullptr;
}

//------------------------------------------------------------------------------------
// Нахождение по уникальному номеру...
//------------------------------------------------------------------------------------
// TODO change to empty() request function
sGLSL* vw_FindShaderByNum(int Num)
{
	sGLSL *Tmp = StartGLSLMan;

	while (Tmp) {
		sGLSL *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num)
			return Tmp;
		Tmp = Tmp1;
	}

	return nullptr;
}

//------------------------------------------------------------------------------------
// Нахождение по имени...
//------------------------------------------------------------------------------------
sGLSL* vw_FindShaderByName(const char *Name)
{
	if (!Name)
		return nullptr;

	sGLSL *Tmp = StartGLSLMan;

	while (Tmp) {
		sGLSL *Tmp1 = Tmp->Next;
		if (strcmp(Tmp->Name, Name) == 0)
			return Tmp;
		Tmp = Tmp1;
	}

	return nullptr;
}

//------------------------------------------------------------------------------------
// создаем шейдерную программу
//------------------------------------------------------------------------------------
sGLSL *vw_CreateShader(const char *ShaderName, const char *VertexShaderFileName, const char *FragmentShaderFileName)
{
	if (!glCreateShaderObjectARB ||
	    !glShaderSourceARB ||
	    !glCompileShaderARB ||
	    !glCreateProgramObjectARB ||
	    !glAttachObjectARB ||
	    !glGetObjectParameterivARB ||
	    (!VertexShaderFileName && !FragmentShaderFileName))
		return nullptr;

	GLint vertCompiled{0}, fragCompiled{0}; // status values

	// создаем структуру в памяти
	sGLSL *GLSLtmp = new sGLSL;

	// создаем пустые объекты и получаем хидеры на них
	GLSLtmp->VertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	GLSLtmp->FragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	// загружаем данные в пустые шейдеры

	// вертекстный шейдер
	GLSLtmp->VertexShaderUse = false;
	if (VertexShaderFileName) {
		std::unique_ptr<sFILE> VertexFile = vw_fopen(VertexShaderFileName);

		if (VertexFile != nullptr) {
			const GLcharARB *TmpGLcharARB = (const GLcharARB *)VertexFile->Data.get();
			GLint TmpGLint = (GLint)VertexFile->Size;
			glShaderSourceARB(GLSLtmp->VertexShader, 1, &TmpGLcharARB, &TmpGLint);
			vw_fclose(VertexFile);
			GLSLtmp->VertexShaderUse = true;
		}
	}
	// фрагментный шейдер
	GLSLtmp->FragmentShaderUse = false;
	if (FragmentShaderFileName) {
		std::unique_ptr<sFILE> FragmentFile = vw_fopen(FragmentShaderFileName);

		if (FragmentFile != nullptr) {
			const GLcharARB *TmpGLcharARB = (const GLcharARB *)FragmentFile->Data.get();
			GLint TmpGLint = (GLint)FragmentFile->Size;
			glShaderSourceARB(GLSLtmp->FragmentShader, 1, &TmpGLcharARB, &TmpGLint);
			vw_fclose(FragmentFile);
			GLSLtmp->FragmentShaderUse = true;
		}
	}

	// компилируем шейдеры

	if (GLSLtmp->VertexShaderUse) {
		glCompileShaderARB(GLSLtmp->VertexShader);
		CheckOGLError(__func__);  // Check for OpenGL errors
		glGetObjectParameterivARB(GLSLtmp->VertexShader, GL_COMPILE_STATUS, &vertCompiled);
		vw_PrintShaderInfoLog(GLSLtmp->VertexShader, VertexShaderFileName);

		if (!vertCompiled)
			return nullptr;
	}
	if (GLSLtmp->FragmentShaderUse) {
		glCompileShaderARB(GLSLtmp->FragmentShader);
		CheckOGLError(__func__);  // Check for OpenGL errors
		glGetObjectParameterivARB(GLSLtmp->FragmentShader, GL_COMPILE_STATUS, &fragCompiled);
		vw_PrintShaderInfoLog(GLSLtmp->FragmentShader, FragmentShaderFileName);

		if (!fragCompiled)
			return nullptr;
	}

	// создаем программу, чтобы подключить эти шейдеры
	GLSLtmp->Program = glCreateProgramObjectARB();
	if (GLSLtmp->VertexShaderUse)
		glAttachObjectARB(GLSLtmp->Program, GLSLtmp->VertexShader);
	if (GLSLtmp->FragmentShaderUse)
		glAttachObjectARB(GLSLtmp->Program, GLSLtmp->FragmentShader);

	if (!VertexShaderFileName)
		std::cout << "Shader ... " <<  FragmentShaderFileName << "\n";
	else if (!FragmentShaderFileName)
		std::cout <<"Shader ... " << VertexShaderFileName << "\n";
	else
		std::cout <<"Shader ... " << VertexShaderFileName << " " << FragmentShaderFileName << "\n";

	if (ShaderName) {
		GLSLtmp->Name = new char[strlen(ShaderName)+1];
		strcpy(GLSLtmp->Name, ShaderName);
	} else {
		GLSLtmp->Name = new char[strlen(VertexShaderFileName)+1];
		strcpy(GLSLtmp->Name, VertexShaderFileName);
	}

	vw_AttachShader(GLSLtmp);

	return GLSLtmp;
}

//------------------------------------------------------------------------------------
// линкуем программу, с учетом отбинденных данных
//------------------------------------------------------------------------------------
bool vw_LinkShaderProgram(sGLSL *GLSL)
{
	if (!GLSL ||
	    !glLinkProgramARB ||
	    !glGetObjectParameterivARB)
		return false;

	glLinkProgramARB(GLSL->Program);
	CheckOGLError(__func__);

	GLint Linked{false};
	glGetObjectParameterivARB(GLSL->Program, GL_LINK_STATUS, &Linked);
	vw_PrintProgramInfoLog(GLSL->Program);

	return Linked;
}

//------------------------------------------------------------------------------------
// запускаем шейдер на исполнение
//------------------------------------------------------------------------------------
bool vw_UseShaderProgram(sGLSL *GLSL)
{
	if (!GLSL || !glUseProgramObjectARB)
		return false;

	glUseProgramObjectARB(GLSL->Program);
	CheckOGLError(__func__);

	return true;
}

//------------------------------------------------------------------------------------
// останавливаем работу шейдера
//------------------------------------------------------------------------------------
bool vw_StopShaderProgram()
{
	if (!glUseProgramObjectARB)
		return false;

	glUseProgramObjectARB(
#ifdef __APPLE__
			      nullptr	/* typedef void *GLhandleARB, see glext.h for more declaration */
#else
			      0		/* typedef unsigned int GLhandleARB, see glext.h for more declaration */
#endif
			      );

	CheckOGLError(__func__);

	return true;
}

//------------------------------------------------------------------------------------
// Get the location of a uniform variable
//------------------------------------------------------------------------------------
int vw_GetUniformLocation(sGLSL *GLSL, const char *name)
{
	if (!GLSL || !glGetUniformLocationARB)
		return -1;

	int tmpLocation = glGetUniformLocationARB(GLSL->Program, name);

	if (tmpLocation == -1)
		std::cerr << __func__ << "(): " << "No such uniform named: " << name << "\n";

	CheckOGLError(__func__);
	return tmpLocation;
}

//------------------------------------------------------------------------------------
// установка значения параметра
//------------------------------------------------------------------------------------
bool vw_Uniform1i(sGLSL *GLSL, int UniformLocation, int data)
{
	if (!GLSL || !glUniform1iARB)
		return false;

	glUniform1iARB(UniformLocation, data);

	CheckOGLError(__func__);

	return true;
}

bool vw_Uniform1f(sGLSL *GLSL, int UniformLocation, float data)
{
	if (!GLSL || !glUniform1fARB)
		return false;

	glUniform1fARB(UniformLocation, data);

	CheckOGLError(__func__);

	return true;
}

bool vw_Uniform3f(sGLSL *GLSL, int UniformLocation, float data1, float data2, float data3)
{
	if (!GLSL || !glUniform3fARB)
		return false;

	glUniform3fARB(UniformLocation, data1, data2, data3);

	CheckOGLError(__func__);

	return true;
}
