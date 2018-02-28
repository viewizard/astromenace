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


#include "graphics.h"



// GL_ARB_shader_objects
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = nullptr;
PFNGLGETHANDLEARBPROC glGetHandleARB = nullptr;
PFNGLDETACHOBJECTARBPROC glDetachObjectARB = nullptr;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = nullptr;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = nullptr;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = nullptr;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = nullptr;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = nullptr;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = nullptr;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = nullptr;
PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB = nullptr;
PFNGLUNIFORM1FARBPROC glUniform1fARB = nullptr;
PFNGLUNIFORM2FARBPROC glUniform2fARB = nullptr;
PFNGLUNIFORM3FARBPROC glUniform3fARB = nullptr;
PFNGLUNIFORM4FARBPROC glUniform4fARB = nullptr;
PFNGLUNIFORM1IARBPROC glUniform1iARB = nullptr;
PFNGLUNIFORM2IARBPROC glUniform2iARB = nullptr;
PFNGLUNIFORM3IARBPROC glUniform3iARB = nullptr;
PFNGLUNIFORM4IARBPROC glUniform4iARB = nullptr;
PFNGLUNIFORM1FVARBPROC glUniform1fvARB = nullptr;
PFNGLUNIFORM2FVARBPROC glUniform2fvARB = nullptr;
PFNGLUNIFORM3FVARBPROC glUniform3fvARB = nullptr;
PFNGLUNIFORM4FVARBPROC glUniform4fvARB = nullptr;
PFNGLUNIFORM1IVARBPROC glUniform1ivARB = nullptr;
PFNGLUNIFORM2IVARBPROC glUniform2ivARB = nullptr;
PFNGLUNIFORM3IVARBPROC glUniform3ivARB = nullptr;
PFNGLUNIFORM4IVARBPROC glUniform4ivARB = nullptr;
PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fvARB = nullptr;
PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB = nullptr;
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB = nullptr;
PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB = nullptr;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = nullptr;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB = nullptr;
PFNGLGETATTACHEDOBJECTSARBPROC glGetAttachedObjectsARB = nullptr;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = nullptr;
PFNGLGETACTIVEUNIFORMARBPROC glGetActiveUniformARB = nullptr;
PFNGLGETUNIFORMFVARBPROC glGetUniformfvARB = nullptr;
PFNGLGETUNIFORMIVARBPROC glGetUniformivARB = nullptr;
PFNGLGETSHADERSOURCEARBPROC glGetShaderSourceARB = nullptr;


// GL_ARB_vertex_shader
PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB = nullptr;
PFNGLGETACTIVEATTRIBARBPROC glGetActiveAttribARB = nullptr;
PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB = nullptr;


// для менеджера
eGLSL *StartGLSLMan = nullptr;
eGLSL *EndGLSLMan = nullptr;
int NumGLSLMan = 0;















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

	// GL_ARB_vertex_shader
	glBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC) SDL_GL_GetProcAddress("glBindAttribLocationARB");
	glGetActiveAttribARB = (PFNGLGETACTIVEATTRIBARBPROC) SDL_GL_GetProcAddress("glGetActiveAttribARB");
	glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC) SDL_GL_GetProcAddress("glGetAttribLocationARB");

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
	    glGetShaderSourceARB == nullptr || glBindAttribLocationARB == nullptr || glGetActiveAttribARB == nullptr || glGetAttribLocationARB == nullptr) {
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
		glBindAttribLocationARB = nullptr;
		glGetActiveAttribARB = nullptr;
		glGetAttribLocationARB = nullptr;

		return false;
	}

	return true;
}





//------------------------------------------------------------------------------------
// ошибка
//------------------------------------------------------------------------------------
int CheckOGLError()
{
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		fprintf(stderr, "glError 0x%x\n", glErr);
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
	if (glGetObjectParameterivARB == nullptr) return;
	if (glGetInfoLogARB == nullptr) return;

	int infologLength = 0;
	int charsWritten  = 0;

	CheckOGLError();  // Check for OpenGL errors

	glGetObjectParameterivARB(shader, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError();  // Check for OpenGL errors

	if (infologLength > 0) {
		GLchar *infoLog = (GLchar *)malloc(infologLength);
		if (infoLog == nullptr) {
			fprintf(stderr, "ERROR: Could not allocate InfoLog buffer\n");
			return;
		}
		glGetInfoLogARB(shader, infologLength, &charsWritten, infoLog);
		if (strlen(infoLog) >1)
			printf("Shader InfoLog %s:\n%s\n\n", ShaderName, infoLog);
		free(infoLog);
	}
	CheckOGLError();  // Check for OpenGL errors
}


void vw_PrintProgramInfoLog(GLhandleARB program)
{
	if (glGetObjectParameterivARB == nullptr) return;
	if (glGetInfoLogARB == nullptr) return;

	int infologLength = 0;
	int charsWritten  = 0;

	CheckOGLError();  // Check for OpenGL errors

	glGetObjectParameterivARB(program, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError();  // Check for OpenGL errors

	if (infologLength > 0) {
		GLchar *infoLog = (GLchar *)malloc(infologLength);
		if (infoLog == nullptr) {
			printf("ERROR: Could not allocate InfoLog buffer\n");
			exit(1);
		}
		glGetInfoLogARB(program, infologLength, &charsWritten, infoLog);
		if (strlen(infoLog) >1)
			printf("Program InfoLog:\n%s\n\n", infoLog);
		free(infoLog);
	}
	CheckOGLError();  // Check for OpenGL errors
}





//------------------------------------------------------------------------------------
// освобождаем память
//------------------------------------------------------------------------------------
void vw_ReleaseShader(eGLSL *GLSL)
{
	if ((GLSL == nullptr) ||
	    (glDetachObjectARB == nullptr) ||
	    (glDeleteObjectARB == nullptr))
		return;

	vw_DetachShader(GLSL);

	// открепляем хидеры шейдеров
	if (GLSL->VertexShaderUse)
		glDetachObjectARB(GLSL->Program, GLSL->VertexShader);
	if (GLSL->FragmentShaderUse)
		glDetachObjectARB(GLSL->Program, GLSL->FragmentShader);
	// удаляем
	glDeleteObjectARB(GLSL->VertexShader);
	glDeleteObjectARB(GLSL->FragmentShader);
	glDeleteObjectARB(GLSL->Program);

	if (GLSL->Name != nullptr) {
		delete [] GLSL->Name;
		GLSL->Name = nullptr;
	}

	// удаляем указатель
	delete GLSL;
}








//------------------------------------------------------------------------------------
// освобождаем все шейдеры подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllShaders()
{
	// Чистка памяти...
	eGLSL *Tmp = StartGLSLMan;
	while (Tmp != nullptr) {
		eGLSL *Tmp1 = Tmp->Next;
		vw_ReleaseShader(Tmp);
		Tmp = Tmp1;
	}

	StartGLSLMan = nullptr;
	EndGLSLMan = nullptr;
	NumGLSLMan = 0;
}







//------------------------------------------------------------------------------------
// подключение к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachShader(eGLSL* GLSL)
{
	if (GLSL == nullptr)
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
void vw_DetachShader(eGLSL* GLSL)
{
	if (GLSL == nullptr)
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
eGLSL* vw_FindShaderByNum(int Num)
{
	eGLSL *Tmp = StartGLSLMan;

	while (Tmp != nullptr) {
		eGLSL *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num)
			return Tmp;
		Tmp = Tmp1;
	}

	return nullptr;
}






//------------------------------------------------------------------------------------
// Нахождение по имени...
//------------------------------------------------------------------------------------
eGLSL* vw_FindShaderByName(const char *Name)
{
	eGLSL *Tmp = StartGLSLMan;

	while (Tmp != nullptr) {
		eGLSL *Tmp1 = Tmp->Next;
		if (strcmp(Tmp->Name, Name) == 0)
			return Tmp;
		Tmp = Tmp1;
	}

	return nullptr;
}


























//------------------------------------------------------------------------------------
// создаем шейдерную программу
//------------------------------------------------------------------------------------
eGLSL *vw_CreateShader(const char *ShaderName, const char *VertexShaderFileName, const char *FragmentShaderFileName)
{
	if ((glCreateShaderObjectARB == nullptr) ||
	    (glShaderSourceARB == nullptr) ||
	    (glCompileShaderARB == nullptr) ||
	    (glCreateProgramObjectARB == nullptr) ||
	    (glAttachObjectARB == nullptr) ||
	    (glGetObjectParameterivARB == nullptr) ||
	    (VertexShaderFileName == nullptr && FragmentShaderFileName == nullptr))
		return nullptr;


	GLint	vertCompiled, fragCompiled;    // status values

	// создаем структуру в памяти
	eGLSL *GLSLtmp = new eGLSL;

	// создаем пустые объекты и получаем хидеры на них
	GLSLtmp->VertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	GLSLtmp->FragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);


	// загружаем данные в пустые шейдеры

	// вертекстный шейдер
	GLSLtmp->VertexShaderUse = false;
	if (VertexShaderFileName != nullptr) {
		std::unique_ptr<eFILE> VertexFile = vw_fopen(VertexShaderFileName);

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
	if (FragmentShaderFileName != nullptr) {
		std::unique_ptr<eFILE> FragmentFile = vw_fopen(FragmentShaderFileName);

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
		CheckOGLError();  // Check for OpenGL errors
		glGetObjectParameterivARB(GLSLtmp->VertexShader, GL_COMPILE_STATUS, &vertCompiled);
		vw_PrintShaderInfoLog(GLSLtmp->VertexShader, VertexShaderFileName);

		if (!vertCompiled)
			return nullptr;
	}
	if (GLSLtmp->FragmentShaderUse) {
		glCompileShaderARB(GLSLtmp->FragmentShader);
		CheckOGLError();  // Check for OpenGL errors
		glGetObjectParameterivARB(GLSLtmp->FragmentShader, GL_COMPILE_STATUS, &fragCompiled);
		vw_PrintShaderInfoLog(GLSLtmp->FragmentShader, FragmentShaderFileName);

		if (!fragCompiled)
			return nullptr;
	}


	// создаем программу, чтобы подключить эти шейдеры
	GLSLtmp->Program = glCreateProgramObjectARB();
	if (GLSLtmp->VertexShaderUse) glAttachObjectARB(GLSLtmp->Program, GLSLtmp->VertexShader);
	if (GLSLtmp->FragmentShaderUse) glAttachObjectARB(GLSLtmp->Program, GLSLtmp->FragmentShader);


	if (VertexShaderFileName == nullptr)
		printf("Shader ... %s\n", FragmentShaderFileName);
	else if (FragmentShaderFileName == nullptr)
		printf("Shader ... %s\n", VertexShaderFileName);
	else
		printf("Shader ... %s %s\n", VertexShaderFileName, FragmentShaderFileName);


	if (ShaderName != nullptr) {
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
bool vw_LinkShaderProgram(eGLSL *GLSL)
{
	if ((GLSL == nullptr) ||
	    (glLinkProgramARB == nullptr) ||
	    (glGetObjectParameterivARB == nullptr))
		return false;

	GLint linked;

	glLinkProgramARB(GLSL->Program);
	CheckOGLError();  // Check for OpenGL errors
	glGetObjectParameterivARB(GLSL->Program, GL_LINK_STATUS, &linked);
	vw_PrintProgramInfoLog(GLSL->Program);

	if (!linked)
		return false;

	return true;
}





//------------------------------------------------------------------------------------
// запускаем шейдер на исполнение
//------------------------------------------------------------------------------------
bool vw_UseShaderProgram(eGLSL *GLSL)
{
	if ((GLSL == nullptr) || (glUseProgramObjectARB == nullptr))
		return false;

	glUseProgramObjectARB(GLSL->Program);
	CheckOGLError();

	return true;
}





//------------------------------------------------------------------------------------
// останавливаем работу шейдера
//------------------------------------------------------------------------------------
bool vw_StopShaderProgram()
{
	if (glUseProgramObjectARB == nullptr) return false;

#ifdef __APPLE__
	glUseProgramObjectARB(nullptr);	// typedef void *GLhandleARB; - see glext.h for more declaration
#else
	glUseProgramObjectARB(0);	// typedef unsigned int GLhandleARB; - see glext.h for more declaration
#endif
	CheckOGLError();

	return true;
}





//------------------------------------------------------------------------------------
// Get the location of a uniform variable
//------------------------------------------------------------------------------------
int vw_GetUniformLocation(eGLSL *GLSL, const char *name)
{
	if (glGetUniformLocationARB == nullptr) return -1;

	int loc;

	loc = glGetUniformLocationARB(GLSL->Program, name);

	if (loc == -1)
		fprintf(stderr, "No such uniform named \"%s\"\n", name);

	CheckOGLError();  // Check for OpenGL errors
	return loc;
}




//------------------------------------------------------------------------------------
// установка значения параметра
//------------------------------------------------------------------------------------
bool vw_Uniform1i(eGLSL *GLSL, int UniformLocation, int data)
{
	if ((GLSL == nullptr) || (glUniform1iARB == nullptr))
		return false;

	glUniform1iARB(UniformLocation, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform1i(eGLSL *GLSL, const char *name, int data)
{
	if ((GLSL == nullptr) || (name == nullptr))
		return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1)
		return false;

	return vw_Uniform1i(GLSL, Loc, data);
}


bool vw_Uniform1f(eGLSL *GLSL, int UniformLocation, float data)
{
	if ((GLSL == nullptr) || (glUniform3fARB == nullptr))
		return false;

	glUniform1fARB(UniformLocation, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform1f(eGLSL *GLSL, const char *name, float data)
{
	if ((GLSL == nullptr) || (name == nullptr))
		return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1)
		return false;

	return vw_Uniform1f(GLSL, Loc, data);
}


bool vw_Uniform3f(eGLSL *GLSL, int UniformLocation, float data1, float data2, float data3)
{
	if ((GLSL == nullptr) || (glUniform3fARB == nullptr))
		return false;

	glUniform3fARB(UniformLocation, data1, data2, data3);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform3f(eGLSL *GLSL, const char *name, float data1, float data2, float data3)
{
	if ((GLSL == nullptr) || (name == nullptr))
		return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1)
		return false;

	return vw_Uniform3f(GLSL, Loc, data1, data2, data3);
}


bool vw_Uniform1fv(eGLSL *GLSL, int UniformLocation, int count, float *data)
{
	if ((GLSL == nullptr) || (glUniform1fvARB == nullptr))
		return false;

	glUniform1fvARB(UniformLocation, count, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform1fv(eGLSL *GLSL, const char *name, int count, float *data)
{
	if ((GLSL == nullptr) || (name == nullptr))
		return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1)
		return false;

	return vw_Uniform1fv(GLSL, Loc, count, data);
}


bool vw_Uniform4fv(eGLSL *GLSL, int UniformLocation, int count, float *data)
{
	if ((GLSL == nullptr) || (glUniform4fvARB == nullptr))
		return false;

	glUniform4fvARB(UniformLocation, count, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform4fv(eGLSL *GLSL, const char *name, int count, float *data)
{
	if ((GLSL == nullptr) || (name == nullptr))
		return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1)
		return false;

	return vw_Uniform4fv(GLSL, Loc, count, data);
}


bool vw_UniformMatrix4fv(eGLSL *GLSL, int UniformLocation, bool transpose, int count, float *data)
{
	if ((GLSL == nullptr) || (glUniformMatrix4fvARB == nullptr))
		return false;

	glUniformMatrix4fvARB(UniformLocation, count, transpose, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_UniformMatrix4fv(eGLSL *GLSL, const char *name, bool transpose, int count, float *data)
{
	if ((GLSL == nullptr) || (name == nullptr))
		return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1)
		return false;

	return vw_UniformMatrix4fv(GLSL, Loc, transpose, count, data);
}

