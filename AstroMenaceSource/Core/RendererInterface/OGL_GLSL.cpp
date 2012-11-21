/********************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#include "RendererInterface.h"
extern eDevCaps OpenGL_DevCaps;



// GL_ARB_shader_objects
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLGETHANDLEARBPROC glGetHandleARB = NULL;
PFNGLDETACHOBJECTARBPROC glDetachObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
PFNGLUNIFORM2IARBPROC glUniform2iARB = NULL;
PFNGLUNIFORM3IARBPROC glUniform3iARB = NULL;
PFNGLUNIFORM4IARBPROC glUniform4iARB = NULL;
PFNGLUNIFORM1FVARBPROC glUniform1fvARB = NULL;
PFNGLUNIFORM2FVARBPROC glUniform2fvARB = NULL;
PFNGLUNIFORM3FVARBPROC glUniform3fvARB = NULL;
PFNGLUNIFORM4FVARBPROC glUniform4fvARB = NULL;
PFNGLUNIFORM1IVARBPROC glUniform1ivARB = NULL;
PFNGLUNIFORM2IVARBPROC glUniform2ivARB = NULL;
PFNGLUNIFORM3IVARBPROC glUniform3ivARB = NULL;
PFNGLUNIFORM4IVARBPROC glUniform4ivARB = NULL;
PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fvARB = NULL;
PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB = NULL;
PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
PFNGLGETATTACHEDOBJECTSARBPROC glGetAttachedObjectsARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLGETACTIVEUNIFORMARBPROC glGetActiveUniformARB = NULL;
PFNGLGETUNIFORMFVARBPROC glGetUniformfvARB = NULL;
PFNGLGETUNIFORMIVARBPROC glGetUniformivARB = NULL;
PFNGLGETSHADERSOURCEARBPROC glGetShaderSourceARB = NULL;


// GL_ARB_vertex_shader
PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB = NULL;
PFNGLGETACTIVEATTRIBARBPROC glGetActiveAttribARB = NULL;
PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB = NULL;


// для менеджера
eGLSL *StartGLSLMan = 0;
eGLSL *EndGLSLMan = 0;
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
	StartGLSLMan = 0;
	EndGLSLMan = 0;
	NumGLSLMan = 0;


	if (glDeleteObjectARB == NULL || glGetHandleARB == NULL || glDetachObjectARB == NULL || glCreateShaderObjectARB == NULL ||
		glShaderSourceARB == NULL || glCompileShaderARB == NULL || glCreateProgramObjectARB == NULL || glAttachObjectARB == NULL ||
		glLinkProgramARB == NULL || glUseProgramObjectARB == NULL || glValidateProgramARB == NULL || glUniform1fARB == NULL ||
		glUniform2fARB == NULL || glUniform3fARB == NULL || glUniform4fARB == NULL || glUniform1iARB == NULL ||
		glUniform2iARB == NULL || glUniform3iARB == NULL || glUniform4iARB == NULL || glUniform1fvARB == NULL ||
		glUniform2fvARB == NULL || glUniform3fvARB == NULL || glUniform4fvARB == NULL || glUniform1ivARB == NULL ||
		glUniform2ivARB == NULL || glUniform3ivARB == NULL || glUniform4ivARB == NULL || glUniformMatrix2fvARB == NULL ||
		glUniformMatrix3fvARB == NULL || glUniformMatrix4fvARB == NULL || glGetObjectParameterfvARB == NULL ||
		glGetObjectParameterivARB == NULL || glGetInfoLogARB == NULL || glGetAttachedObjectsARB == NULL ||
		glGetUniformLocationARB == NULL || glGetActiveUniformARB == NULL || glGetUniformfvARB == NULL || glGetUniformivARB == NULL ||
		glGetShaderSourceARB == NULL || glBindAttribLocationARB == NULL || glGetActiveAttribARB == NULL || glGetAttribLocationARB == NULL)
	{
		glDeleteObjectARB = NULL;
		glGetHandleARB = NULL;
		glDetachObjectARB = NULL;
		glCreateShaderObjectARB = NULL;
		glShaderSourceARB = NULL;
		glCompileShaderARB = NULL;
		glCreateProgramObjectARB = NULL;
		glAttachObjectARB = NULL;
		glLinkProgramARB = NULL;
		glUseProgramObjectARB = NULL;
		glValidateProgramARB = NULL;
		glUniform1fARB = NULL;
		glUniform2fARB = NULL;
		glUniform3fARB = NULL;
		glUniform4fARB = NULL;
		glUniform1iARB = NULL;
		glUniform2iARB = NULL;
		glUniform3iARB = NULL;
		glUniform4iARB = NULL;
		glUniform1fvARB = NULL;
		glUniform2fvARB = NULL;
		glUniform3fvARB = NULL;
		glUniform4fvARB = NULL;
		glUniform1ivARB = NULL;
		glUniform2ivARB = NULL;
		glUniform3ivARB = NULL;
		glUniform4ivARB = NULL;
		glUniformMatrix2fvARB = NULL;
		glUniformMatrix3fvARB = NULL;
		glUniformMatrix4fvARB = NULL;
		glGetObjectParameterfvARB = NULL;
		glGetObjectParameterivARB = NULL;
		glGetInfoLogARB = NULL;
		glGetAttachedObjectsARB = NULL;
		glGetUniformLocationARB = NULL;
		glGetActiveUniformARB = NULL;
		glGetUniformfvARB = NULL;
		glGetUniformivARB = NULL;
		glGetShaderSourceARB = NULL;
		glBindAttribLocationARB = NULL;
		glGetActiveAttribARB = NULL;
		glGetAttribLocationARB = NULL;

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
    while (glErr != GL_NO_ERROR)
    {
        fprintf(stderr, "glError 0x%x %s\n", glErr, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


//------------------------------------------------------------------------------------
// Print out the information log for a shader object
//------------------------------------------------------------------------------------
void vw_PrintShaderInfoLog(GLuint shader, const char *ShaderName)
{
	if (glGetObjectParameterivARB == NULL) return;
	if (glGetInfoLogARB == NULL) return;

    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog;

    CheckOGLError();  // Check for OpenGL errors

    glGetObjectParameterivARB(shader, GL_INFO_LOG_LENGTH, &infologLength);

    CheckOGLError();  // Check for OpenGL errors

    if (infologLength > 0)
    {
        infoLog = (GLchar *)malloc(infologLength);
        if (infoLog == NULL)
        {
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


void vw_PrintProgramInfoLog(GLuint program)
{
	if (glGetObjectParameterivARB == NULL) return;
	if (glGetInfoLogARB == NULL) return;

    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog;

    CheckOGLError();  // Check for OpenGL errors

    glGetObjectParameterivARB(program, GL_INFO_LOG_LENGTH, &infologLength);

    CheckOGLError();  // Check for OpenGL errors

    if (infologLength > 0)
    {
        infoLog = (GLchar *)malloc(infologLength);
        if (infoLog == NULL)
        {
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
	if (GLSL == 0) return;
	if (glDetachObjectARB == NULL) return;
	if (glDeleteObjectARB == NULL) return;

	vw_DetachShader(GLSL);

	// открепляем хидеры шейдеров
	if (GLSL->VertexShaderUse) glDetachObjectARB(GLSL->Program, GLSL->VertexShader);
	if (GLSL->FragmentShaderUse) glDetachObjectARB(GLSL->Program, GLSL->FragmentShader);
	// удаляем
	glDeleteObjectARB(GLSL->VertexShader);
	glDeleteObjectARB(GLSL->FragmentShader);
	glDeleteObjectARB(GLSL->Program);

	if (GLSL->Name != 0){delete [] GLSL->Name; GLSL->Name = 0;}

	// удаляем указатель
	delete GLSL; GLSL = 0;
}








//------------------------------------------------------------------------------------
// освобождаем все шейдеры подключенные к менеджеру
//------------------------------------------------------------------------------------
void vw_ReleaseAllShaders()
{
	// Чистка памяти...
	eGLSL *Tmp = StartGLSLMan;
	while (Tmp != 0)
	{
		eGLSL *Tmp1 = Tmp->Next;
		vw_ReleaseShader(Tmp);
		Tmp = Tmp1;
	}

	StartGLSLMan = 0;
	EndGLSLMan = 0;
	NumGLSLMan = 0;
}







//------------------------------------------------------------------------------------
// подключение к менеджеру
//------------------------------------------------------------------------------------
void vw_AttachShader(eGLSL* GLSL)
{
	if (GLSL == 0) return;

	// первый в списке...
	if (EndGLSLMan == 0)
	{
		GLSL->Prev = 0;
		GLSL->Next = 0;
		NumGLSLMan += 1;
		GLSL->Num = NumGLSLMan;
		StartGLSLMan = GLSL;
		EndGLSLMan = GLSL;
	}
	else // продолжаем заполнение...
	{
		GLSL->Prev = EndGLSLMan;
		GLSL->Next = 0;
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
	if (GLSL == 0) return;

	// переустанавливаем указатели...
	if (StartGLSLMan == GLSL) StartGLSLMan = GLSL->Next;
	if (EndGLSLMan == GLSL) EndGLSLMan = GLSL->Prev;


	if (GLSL->Next != 0) GLSL->Next->Prev = GLSL->Prev;
		else if (GLSL->Prev != 0) GLSL->Prev->Next = 0;
	if (GLSL->Prev != 0) GLSL->Prev->Next = GLSL->Next;
		else if (GLSL->Next != 0) GLSL->Next->Prev = 0;
}





//------------------------------------------------------------------------------------
// Нахождение по уникальному номеру...
//------------------------------------------------------------------------------------
eGLSL* vw_FindShaderByNum(int Num)
{
	eGLSL *Tmp = StartGLSLMan;

	while (Tmp != 0)
	{
		eGLSL *Tmp1 = Tmp->Next;
		if (Tmp->Num == Num) return Tmp;
		Tmp = Tmp1;
	}

	return 0;
}






//------------------------------------------------------------------------------------
// Нахождение по имени...
//------------------------------------------------------------------------------------
eGLSL* vw_FindShaderByName(const char *Name)
{
	eGLSL *Tmp = StartGLSLMan;

	while (Tmp != 0)
	{
		eGLSL *Tmp1 = Tmp->Next;
		if( vw_strcmp(Tmp->Name, Name) == 0 ) return Tmp;
		Tmp = Tmp1;
	}

	return 0;
}


























//------------------------------------------------------------------------------------
// создаем шейдерную программу
//------------------------------------------------------------------------------------
eGLSL *vw_CreateShader(const char *ShaderName, const char *VertexShaderFileName, const char *FragmentShaderFileName)
{
	if (glCreateShaderObjectARB == NULL) return 0;
	if (glShaderSourceARB == NULL) return 0;
	if (glCompileShaderARB == NULL) return 0;
	if (glCreateProgramObjectARB == NULL) return 0;
	if (glAttachObjectARB == NULL) return 0;
	if (glGetObjectParameterivARB == NULL) return 0;
	if (VertexShaderFileName == 0 && FragmentShaderFileName == 0) return 0;


	GLint	vertCompiled, fragCompiled;    // status values

	// создаем структуру в памяти
	eGLSL *GLSLtmp = 0;
	GLSLtmp = new eGLSL;
	if (GLSLtmp == 0) return 0;

	// создаем пустые объекты и получаем хидеры на них
    GLSLtmp->VertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    GLSLtmp->FragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);


	// загружаем данные в пустые шейдеры

	// вертекстный шейдер
	GLSLtmp->VertexShaderUse = false;
	if (VertexShaderFileName != 0)
	{
		eFILE *VertexFile = 0;
		VertexFile = vw_fopen(VertexShaderFileName);

		if (VertexFile != 0)
		{
			const GLcharARB *TmpGLcharARB =  (const GLcharARB *)VertexFile->Data;
			glShaderSourceARB(GLSLtmp->VertexShader, 1, &TmpGLcharARB, &VertexFile->RealLength);
			vw_fclose(VertexFile);
			GLSLtmp->VertexShaderUse = true;
		}
	}
	// фрагментный шейдер
	GLSLtmp->FragmentShaderUse = false;
	if (FragmentShaderFileName != 0)
	{
		eFILE *FragmentFile = 0;
		FragmentFile = vw_fopen(FragmentShaderFileName);

		if (FragmentFile != 0)
		{
			const GLcharARB *TmpGLcharARB =  (const GLcharARB *)FragmentFile->Data;
			glShaderSourceARB(GLSLtmp->FragmentShader, 1, &TmpGLcharARB, &FragmentFile->RealLength);
			vw_fclose(FragmentFile);
			GLSLtmp->FragmentShaderUse = true;
		}
	}


	// компилируем шейдеры

	if (GLSLtmp->VertexShaderUse)
	{
		glCompileShaderARB(GLSLtmp->VertexShader);
		CheckOGLError();  // Check for OpenGL errors
		glGetObjectParameterivARB(GLSLtmp->VertexShader, GL_COMPILE_STATUS, &vertCompiled);
		vw_PrintShaderInfoLog(GLSLtmp->VertexShader, VertexShaderFileName);

		if (!vertCompiled)	return 0;
	}
	if (GLSLtmp->FragmentShaderUse)
	{
		glCompileShaderARB(GLSLtmp->FragmentShader);
		CheckOGLError();  // Check for OpenGL errors
		glGetObjectParameterivARB(GLSLtmp->FragmentShader, GL_COMPILE_STATUS, &fragCompiled);
		vw_PrintShaderInfoLog(GLSLtmp->FragmentShader, FragmentShaderFileName);

		if (!fragCompiled)	return 0;
	}


    // создаем программу, чтобы подключить эти шейдеры
    GLSLtmp->Program = glCreateProgramObjectARB();
    if (GLSLtmp->VertexShaderUse) glAttachObjectARB(GLSLtmp->Program, GLSLtmp->VertexShader);
    if (GLSLtmp->FragmentShaderUse) glAttachObjectARB(GLSLtmp->Program, GLSLtmp->FragmentShader);


	if (VertexShaderFileName == 0)
		printf("Shader ... %s\n", FragmentShaderFileName);
	else
		if (FragmentShaderFileName == 0)
			printf("Shader ... %s\n", VertexShaderFileName);
		else
			printf("Shader ... %s %s\n", VertexShaderFileName, FragmentShaderFileName);


	if (ShaderName != 0)
	{
		GLSLtmp->Name = new char[strlen(ShaderName)+1];
		strcpy(GLSLtmp->Name, ShaderName);
	}
	else
	{
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
	if (GLSL == 0) return false;
	if (glLinkProgramARB == NULL) return false;
	if (glGetObjectParameterivARB == NULL) return false;

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
	if (GLSL == 0) return false;
	if (glUseProgramObjectARB == NULL) return false;

    glUseProgramObjectARB(GLSL->Program);
    CheckOGLError();

	return true;
}





//------------------------------------------------------------------------------------
// останавливаем работу шейдера
//------------------------------------------------------------------------------------
bool vw_StopShaderProgram()
{
	if (glUseProgramObjectARB == NULL) return false;

    glUseProgramObjectARB(0);
    CheckOGLError();

	return true;
}





//------------------------------------------------------------------------------------
// Get the location of a uniform variable
//------------------------------------------------------------------------------------
int vw_GetUniformLocation(eGLSL *GLSL, const char *name)
{
	if (glGetUniformLocationARB == NULL) return -1;

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
	if (GLSL == 0) return false;
	if (glUniform1iARB == NULL) return false;

	glUniform1iARB(UniformLocation, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform1i(eGLSL *GLSL, const char *name, int data)
{
	if (GLSL == 0) return false;
	if (name == 0) return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1) return false;

	return vw_Uniform1i(GLSL, Loc, data);
}


bool vw_Uniform1f(eGLSL *GLSL, int UniformLocation, float data)
{
	if (GLSL == 0) return false;
	if (glUniform3fARB == NULL) return false;

	glUniform1fARB(UniformLocation, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform1f(eGLSL *GLSL, const char *name, float data)
{
	if (GLSL == 0) return false;
	if (name == 0) return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1) return false;

	return vw_Uniform1f(GLSL, Loc, data);
}


bool vw_Uniform3f(eGLSL *GLSL, int UniformLocation, float data1, float data2, float data3)
{
	if (GLSL == 0) return false;
	if (glUniform3fARB == NULL) return false;

	glUniform3fARB(UniformLocation, data1, data2, data3);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform3f(eGLSL *GLSL, const char *name, float data1, float data2, float data3)
{
	if (GLSL == 0) return false;
	if (name == 0) return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1) return false;

	return vw_Uniform3f(GLSL, Loc, data1, data2, data3);
}


bool vw_Uniform1fv(eGLSL *GLSL, int UniformLocation, int count, float *data)
{
	if (GLSL == 0) return false;
	if (glUniform1fvARB == NULL) return false;

	glUniform1fvARB(UniformLocation, count, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform1fv(eGLSL *GLSL, const char *name, int count, float *data)
{
	if (GLSL == 0) return false;
	if (name == 0) return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1) return false;

	return vw_Uniform1fv(GLSL, Loc, count, data);
}


bool vw_Uniform4fv(eGLSL *GLSL, int UniformLocation, int count, float *data)
{
	if (GLSL == 0) return false;
	if (glUniform4fvARB == NULL) return false;

	glUniform4fvARB(UniformLocation, count, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_Uniform4fv(eGLSL *GLSL, const char *name, int count, float *data)
{
	if (GLSL == 0) return false;
	if (name == 0) return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1) return false;

	return vw_Uniform4fv(GLSL, Loc, count, data);
}


bool vw_UniformMatrix4fv(eGLSL *GLSL, int UniformLocation, bool transpose, int count, float *data)
{
	if (GLSL == 0) return false;
	if (glUniformMatrix4fvARB == NULL) return false;

	glUniformMatrix4fvARB(UniformLocation, count, transpose, data);

	CheckOGLError();  // Check for OpenGL errors

	return true;
}
bool vw_UniformMatrix4fv(eGLSL *GLSL, const char *name, bool transpose, int count, float *data)
{
	if (GLSL == 0) return false;
	if (name == 0) return false;

	int Loc = vw_GetUniformLocation(GLSL, name);
	if (Loc == -1) return false;

	return vw_UniformMatrix4fv(GLSL, Loc, transpose, count, data);
}

