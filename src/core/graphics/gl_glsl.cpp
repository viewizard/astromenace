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
#include "extensions.h"

namespace {

// для менеджера
sGLSL *StartGLSLMan{nullptr};
sGLSL *EndGLSLMan{nullptr};
int NumGLSLMan{0};

} // unnamed namespace


//------------------------------------------------------------------------------------
// ошибка
//------------------------------------------------------------------------------------
static void CheckOGLError(const char *FunctionName)
{
	GLenum glErr{0};

	while ((glErr = glGetError()) != GL_NO_ERROR) {
		std::cerr << FunctionName << "(): " << "glError " << glErr << "\n";
	}
}

//------------------------------------------------------------------------------------
// Print out the information log for a shader object
//------------------------------------------------------------------------------------
void vw_PrintShaderInfoLog(GLhandleARB shader, const char *ShaderName)
{
	if (!_glGetObjectParameteriv ||
	    !_glGetInfoLog)
		return;

	int infologLength{0};
	int charsWritten{0};

	CheckOGLError(__func__);  // Check for OpenGL errors

	_glGetObjectParameteriv(shader, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError(__func__);  // Check for OpenGL errors

	if (infologLength > 0) {
		GLchar *infoLog = (GLchar *)malloc(infologLength);
		if (!infoLog) {
			std::cerr << __func__ << "(): " << "Could not allocate InfoLog buffer.\n";
			return;
		}
		_glGetInfoLog(shader, infologLength, &charsWritten, infoLog);
		if (strlen(infoLog) >1)
			std::cout << "Shader InfoLog " << ShaderName << ":\n" << infoLog << "\n\n";
		free(infoLog);
	}
	CheckOGLError(__func__);  // Check for OpenGL errors
}

void vw_PrintProgramInfoLog(GLhandleARB program)
{
	if (!_glGetObjectParameteriv ||
	    !_glGetInfoLog)
		return;

	int infologLength{0};
	int charsWritten{0};

	CheckOGLError(__func__);  // Check for OpenGL errors

	_glGetObjectParameteriv(program, GL_INFO_LOG_LENGTH, &infologLength);

	CheckOGLError(__func__);  // Check for OpenGL errors

	if (infologLength > 0) {
		GLchar *infoLog = (GLchar *)malloc(infologLength);
		if (!infoLog) {
			std::cerr << __func__ << "(): " << "Could not allocate InfoLog buffer.\n";
			return;
		}
		_glGetInfoLog(program, infologLength, &charsWritten, infoLog);
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
	if (!_glDetachObject ||
	    !_glDeleteObject)
		return;

	// Чистка памяти...
	sGLSL *Tmp = StartGLSLMan;
	while (Tmp) {
		sGLSL *Tmp1 = Tmp->Next;

		vw_DetachShader(Tmp);

		// открепляем хидеры шейдеров
		if (Tmp->VertexShaderUse)
			_glDetachObject(Tmp->Program, Tmp->VertexShader);
		if (Tmp->FragmentShaderUse)
			_glDetachObject(Tmp->Program, Tmp->FragmentShader);
		// удаляем
		_glDeleteObject(Tmp->VertexShader);
		_glDeleteObject(Tmp->FragmentShader);
		_glDeleteObject(Tmp->Program);

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
	if (!_glCreateShaderObject ||
	    !_glShaderSource ||
	    !_glCompileShader ||
	    !_glCreateProgramObject ||
	    !_glAttachObject ||
	    !_glGetObjectParameteriv ||
	    (!VertexShaderFileName && !FragmentShaderFileName))
		return nullptr;

	GLint vertCompiled{0}, fragCompiled{0}; // status values

	// создаем структуру в памяти
	sGLSL *GLSLtmp = new sGLSL;

	// создаем пустые объекты и получаем хидеры на них
	GLSLtmp->VertexShader = _glCreateShaderObject(GL_VERTEX_SHADER_ARB);
	GLSLtmp->FragmentShader = _glCreateShaderObject(GL_FRAGMENT_SHADER_ARB);

	// загружаем данные в пустые шейдеры

	// вертекстный шейдер
	GLSLtmp->VertexShaderUse = false;
	if (VertexShaderFileName) {
		std::unique_ptr<sFILE> VertexFile = vw_fopen(VertexShaderFileName);

		if (VertexFile != nullptr) {
			const GLcharARB *TmpGLcharARB = (const GLcharARB *)VertexFile->Data.get();
			GLint TmpGLint = (GLint)VertexFile->Size;
			_glShaderSource(GLSLtmp->VertexShader, 1, &TmpGLcharARB, &TmpGLint);
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
			_glShaderSource(GLSLtmp->FragmentShader, 1, &TmpGLcharARB, &TmpGLint);
			vw_fclose(FragmentFile);
			GLSLtmp->FragmentShaderUse = true;
		}
	}

	// компилируем шейдеры

	if (GLSLtmp->VertexShaderUse) {
		_glCompileShader(GLSLtmp->VertexShader);
		CheckOGLError(__func__);  // Check for OpenGL errors
		_glGetObjectParameteriv(GLSLtmp->VertexShader, GL_COMPILE_STATUS, &vertCompiled);
		vw_PrintShaderInfoLog(GLSLtmp->VertexShader, VertexShaderFileName);

		if (!vertCompiled)
			return nullptr;
	}
	if (GLSLtmp->FragmentShaderUse) {
		_glCompileShader(GLSLtmp->FragmentShader);
		CheckOGLError(__func__);  // Check for OpenGL errors
		_glGetObjectParameteriv(GLSLtmp->FragmentShader, GL_COMPILE_STATUS, &fragCompiled);
		vw_PrintShaderInfoLog(GLSLtmp->FragmentShader, FragmentShaderFileName);

		if (!fragCompiled)
			return nullptr;
	}

	// создаем программу, чтобы подключить эти шейдеры
	GLSLtmp->Program = _glCreateProgramObject();
	if (GLSLtmp->VertexShaderUse)
		_glAttachObject(GLSLtmp->Program, GLSLtmp->VertexShader);
	if (GLSLtmp->FragmentShaderUse)
		_glAttachObject(GLSLtmp->Program, GLSLtmp->FragmentShader);

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
	    !_glLinkProgram ||
	    !_glGetObjectParameteriv)
		return false;

	_glLinkProgram(GLSL->Program);
	CheckOGLError(__func__);

	GLint Linked{false};
	_glGetObjectParameteriv(GLSL->Program, GL_LINK_STATUS, &Linked);
	vw_PrintProgramInfoLog(GLSL->Program);

	return Linked;
}

//------------------------------------------------------------------------------------
// запускаем шейдер на исполнение
//------------------------------------------------------------------------------------
bool vw_UseShaderProgram(sGLSL *GLSL)
{
	if (!GLSL || !_glUseProgramObject)
		return false;

	_glUseProgramObject(GLSL->Program);
	CheckOGLError(__func__);

	return true;
}

//------------------------------------------------------------------------------------
// останавливаем работу шейдера
//------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------
// Get the location of a uniform variable
//------------------------------------------------------------------------------------
int vw_GetUniformLocation(sGLSL *GLSL, const char *name)
{
	if (!GLSL || !_glGetUniformLocation)
		return -1;

	int tmpLocation = _glGetUniformLocation(GLSL->Program, name);

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
	if (!GLSL || !_glUniform1i)
		return false;

	_glUniform1i(UniformLocation, data);

	CheckOGLError(__func__);

	return true;
}

bool vw_Uniform1f(sGLSL *GLSL, int UniformLocation, float data)
{
	if (!GLSL || !_glUniform1f)
		return false;

	_glUniform1f(UniformLocation, data);

	CheckOGLError(__func__);

	return true;
}

bool vw_Uniform3f(sGLSL *GLSL, int UniformLocation, float data1, float data2, float data3)
{
	if (!GLSL || !_glUniform3f)
		return false;

	_glUniform3f(UniformLocation, data1, data2, data3);

	CheckOGLError(__func__);

	return true;
}
