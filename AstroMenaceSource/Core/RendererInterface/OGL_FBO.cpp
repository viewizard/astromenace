/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: OGL_FBO.cpp

	Copyright (c) 2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


#include "RendererInterface.h"



// данные для буферов, чтобы сделать MSAA
GLuint ColorRenderbufferName = 0;
GLuint DepthRenderbufferName = 0;
GLuint ColorTextureName = 0;
GLuint FramebufferRenderName = 0;
GLuint FramebufferResolveName = 0;
GLuint FBO_Width, FBO_Height;

// указатели на функии
PFNGLGENRENDERBUFFERSEXTPROC				glGenRenderbuffersEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC				glBindRenderbufferEXT = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC	glRenderbufferStorageMultisampleEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC				glGenFramebuffersEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC					glBindFramebufferEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC			glFramebufferRenderbufferEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC			glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC			glFramebufferTexture2DEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC				glDeleteRenderbuffersEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC				glDeleteFramebuffersEXT = NULL;
PFNGLBLITFRAMEBUFFEREXTPROC					glBlitFramebufferEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC					glIsFramebufferEXT = NULL;
PFNGLGENERATEMIPMAPPROC 					glGenerateMipmapEXT = NULL;





//------------------------------------------------------------------------------------
// Инициализация работы с FBO
//------------------------------------------------------------------------------------
bool vw_Internal_InitializationFBO()
{
	// Get Pointers To The GL Functions
	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenRenderbuffersEXT");
	glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindRenderbufferEXT");
	glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) SDL_GL_GetProcAddress("glRenderbufferStorageMultisampleEXT");
	glGenFramebuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenFramebuffersEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindFramebufferEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glFramebufferRenderbufferEXT");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
	glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteRenderbuffersEXT");
	glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
	glBlitFramebufferEXT = (PFNGLBLITFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBlitFramebufferEXT");
	glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glIsFramebufferEXT");
	glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPPROC) SDL_GL_GetProcAddress("glGenerateMipmapEXT");


	if (glGenRenderbuffersEXT == NULL || glBindRenderbufferEXT == NULL ||
		glRenderbufferStorageMultisampleEXT == NULL || glGenFramebuffersEXT == NULL ||
		glBindFramebufferEXT == NULL || glFramebufferRenderbufferEXT == NULL ||
		glCheckFramebufferStatusEXT == NULL || glFramebufferTexture2DEXT == NULL ||
		glDeleteRenderbuffersEXT == NULL || glDeleteFramebuffersEXT == NULL ||
		glBlitFramebufferEXT == NULL || glIsFramebufferEXT == NULL ||
		glGenerateMipmapEXT == NULL)
	{
		glGenRenderbuffersEXT = NULL;
		glBindRenderbufferEXT = NULL;
		glRenderbufferStorageMultisampleEXT = NULL;
		glGenFramebuffersEXT = NULL;
		glBindFramebufferEXT = NULL;
		glFramebufferRenderbufferEXT = NULL;
		glCheckFramebufferStatusEXT = NULL;
		glFramebufferTexture2DEXT = NULL;
		glDeleteRenderbuffersEXT = NULL;
		glDeleteFramebuffersEXT = NULL;
		glBlitFramebufferEXT = NULL;
		glIsFramebufferEXT = NULL;
		glGenerateMipmapEXT = NULL;

		return false;
	}

	return true;
}






//------------------------------------------------------------------------------------
// Инициализация буферов
//------------------------------------------------------------------------------------
bool vw_Internal_MSAA_FBO_Create(int Width, int Height, int MSAA)
{
	if (glGenRenderbuffersEXT == NULL) return false;
	if (glBindRenderbufferEXT == NULL) return false;
	if (glRenderbufferStorageMultisampleEXT == NULL) return false;
	if (glGenFramebuffersEXT == NULL) return false;
	if (glBindFramebufferEXT == NULL) return false;
	if (glFramebufferRenderbufferEXT == NULL) return false;
	if (glCheckFramebufferStatusEXT == NULL) return false;


	FBO_Width = Width;
	FBO_Height = Height;


	// создаем наш буфер с мультисемплами (цвет+глубина)
	glGenRenderbuffersEXT(1, &ColorRenderbufferName);
	glBindRenderbufferEXT(GL_RENDERBUFFER, ColorRenderbufferName);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, MSAA, GL_RGBA, FBO_Width, FBO_Height);

	glGenRenderbuffersEXT(1, &DepthRenderbufferName);
  	glBindRenderbufferEXT(GL_RENDERBUFFER, DepthRenderbufferName);
  	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, MSAA, GL_DEPTH_COMPONENT24, FBO_Width, FBO_Height);

	glGenFramebuffersEXT(1, &FramebufferRenderName);
	glBindFramebufferEXT(GL_FRAMEBUFFER, FramebufferRenderName);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ColorRenderbufferName);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderbufferName);
	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Can't create Render FRAMEBUFFER.\n\n");
		return false;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);


	// создаем обычный буфер без мультисемплов, чтобы в него потом вывести (только цвет делаем)
    glGenTextures(1, &ColorTextureName);
	glBindTexture(GL_TEXTURE_2D, ColorTextureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FBO_Width, FBO_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenFramebuffersEXT(1, &FramebufferResolveName);
	glBindFramebufferEXT(GL_FRAMEBUFFER, FramebufferResolveName);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTextureName, 0);
	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(stderr, "Can't create Resolve FRAMEBUFFER.\n\n");
		return false;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	return true;
}







//------------------------------------------------------------------------------------
// Переключаемся на нужный буфер
//------------------------------------------------------------------------------------
void vw_Internal_MSAA_FBO_BeginRendering(float fClearRed, float fClearGreen, float fClearBlue, float fClearAlpha)
{
	if (glBindFramebufferEXT == NULL) return;
	if (FramebufferRenderName == 0) return;

	// Clear the framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
	glClearColor(fClearRed, fClearGreen, fClearBlue, fClearAlpha);

	glEnable(GL_MULTISAMPLE);
	glBindFramebufferEXT(GL_FRAMEBUFFER, FramebufferRenderName);
	glViewport(0, 0, FBO_Width, FBO_Height);
	glClearColor(fClearRed, fClearGreen, fClearBlue, fClearAlpha);
}








//------------------------------------------------------------------------------------
// Завершаем прорисовку и рисуем в основной буфер
//------------------------------------------------------------------------------------
void vw_Internal_MSAA_FBO_EndRendering()
{
	if (glBindFramebufferEXT == NULL) return;
	if (glBlitFramebufferEXT == NULL) return;
	if (FramebufferRenderName == 0) return;
	if (FramebufferResolveName == 0) return;


	glDisable(GL_MULTISAMPLE);

	// Resolved multisampling
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER, FramebufferRenderName);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, FramebufferResolveName);
	glBlitFramebufferEXT(
		0, 0, FBO_Width, FBO_Height,
		0, 0, FBO_Width, FBO_Height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, FBO_Width, FBO_Height);


	// рисуем текстуру на весь экран

    // запоминаем состояние флагов
	glPushAttrib(GL_ENABLE_BIT);
    // и выключаем "ненужные"
	glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);								//select the projection matrix
	glPushMatrix();												//store the projection matrix
	glLoadIdentity();											//reset the projection matrix

	glOrtho(0, FBO_Width, 0, FBO_Height, -1, 1);

	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix
	glPushMatrix();
	glLoadIdentity();


		float *buff = 0;
		// RI_2f_XY | RI_1_TEX
		buff = new float[4*4]; if (buff == 0) return;

		int k=0;

		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;

		buff[k++] = 0.0f;
		buff[k++] = FBO_Height;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;

		buff[k++] = FBO_Width;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;

		buff[k++] = FBO_Width;
		buff[k++] = FBO_Height;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;


		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ColorTextureName);

		vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, buff, 4*sizeof(float));

		vw_SetTextureDef(0);
		if (buff != 0){delete [] buff; buff = 0;}


	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);			//select the projection matrix
	glPopMatrix();							//restore the old projection matrix

	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix

    // восстанавливаем флаги
	glPopAttrib();
}










//------------------------------------------------------------------------------------
// Освобождаем память
//------------------------------------------------------------------------------------
void vw_Internal_ReleaseFBO()
{
	if (glDeleteRenderbuffersEXT == NULL) return;
	if (glDeleteFramebuffersEXT == NULL) return;

	if (ColorTextureName != 0) {glDeleteTextures(1, &ColorTextureName); ColorTextureName=0;};
	if (ColorRenderbufferName != 0) {glDeleteRenderbuffersEXT(1, &ColorRenderbufferName); ColorRenderbufferName=0;};
	if (DepthRenderbufferName != 0) {glDeleteRenderbuffersEXT(1, &DepthRenderbufferName); DepthRenderbufferName=0;};
	if (FramebufferRenderName != 0) {glDeleteFramebuffersEXT(1, &FramebufferRenderName); FramebufferRenderName=0;};
	if (FramebufferResolveName != 0) {glDeleteFramebuffersEXT(1, &FramebufferResolveName); FramebufferResolveName=0;};
}

