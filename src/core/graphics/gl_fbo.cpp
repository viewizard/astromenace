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

// текущий работающий буфер, если 0 - фрейм буфер
sFBO *CurrentFBO = nullptr;

// указатели на функии
PFNGLGENRENDERBUFFERSEXTPROC				glGenRenderbuffersEXT = nullptr;
PFNGLBINDRENDERBUFFEREXTPROC				glBindRenderbufferEXT = nullptr;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC	glRenderbufferStorageMultisampleEXT = nullptr;
PFNGLGENRENDERBUFFERSEXTPROC				glGenFramebuffersEXT = nullptr;
PFNGLBINDFRAMEBUFFEREXTPROC					glBindFramebufferEXT = nullptr;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC			glFramebufferRenderbufferEXT = nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC			glCheckFramebufferStatusEXT = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC			glFramebufferTexture2DEXT = nullptr;
PFNGLDELETERENDERBUFFERSEXTPROC				glDeleteRenderbuffersEXT = nullptr;
PFNGLDELETEFRAMEBUFFERSEXTPROC				glDeleteFramebuffersEXT = nullptr;
PFNGLBLITFRAMEBUFFEREXTPROC					glBlitFramebufferEXT = nullptr;
PFNGLISFRAMEBUFFEREXTPROC					glIsFramebufferEXT = nullptr;
PFNGLGENERATEMIPMAPPROC 					glGenerateMipmapEXT = nullptr;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC		glGetFramebufferAttachmentParameterivEXT = nullptr;
// GL_NV_framebuffer_multisample_coverage, не обязательный
PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC	glRenderbufferStorageMultisampleCoverageNV = nullptr;




//------------------------------------------------------------------------------------
// Инициализация работы с FBO
//------------------------------------------------------------------------------------
bool vw_Internal_InitializationFBO()
{
	// GL_ARB_framebuffer_object
	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenRenderbuffers");
	glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindRenderbuffer");
	glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) SDL_GL_GetProcAddress("glRenderbufferStorageMultisample");
	glGenFramebuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenFramebuffers");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindFramebuffer");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) SDL_GL_GetProcAddress("glFramebufferRenderbuffer");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) SDL_GL_GetProcAddress("glCheckFramebufferStatus");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) SDL_GL_GetProcAddress("glFramebufferTexture2D");
	glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteRenderbuffers");
	glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteFramebuffers");
	glBlitFramebufferEXT = (PFNGLBLITFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBlitFramebuffer");
	glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glIsFramebuffer");
	glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPPROC) SDL_GL_GetProcAddress("glGenerateMipmap");
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameteriv");


	if (glGenRenderbuffersEXT == nullptr || glBindRenderbufferEXT == nullptr ||
	    glRenderbufferStorageMultisampleEXT == nullptr || glGenFramebuffersEXT == nullptr ||
	    glBindFramebufferEXT == nullptr || glFramebufferRenderbufferEXT == nullptr ||
	    glCheckFramebufferStatusEXT == nullptr || glFramebufferTexture2DEXT == nullptr ||
	    glDeleteRenderbuffersEXT == nullptr || glDeleteFramebuffersEXT == nullptr ||
	    glBlitFramebufferEXT == nullptr || glIsFramebufferEXT == nullptr ||
	    glGenerateMipmapEXT == nullptr || glGetFramebufferAttachmentParameterivEXT == nullptr) {
		// GL_EXT_framebuffer_object+GL_EXT_framebuffer_multisample+GL_EXT_framebuffer_blit
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
		glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) SDL_GL_GetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	}


	if (glGenRenderbuffersEXT == nullptr || glBindRenderbufferEXT == nullptr ||
	    glRenderbufferStorageMultisampleEXT == nullptr || glGenFramebuffersEXT == nullptr ||
	    glBindFramebufferEXT == nullptr || glFramebufferRenderbufferEXT == nullptr ||
	    glCheckFramebufferStatusEXT == nullptr || glFramebufferTexture2DEXT == nullptr ||
	    glDeleteRenderbuffersEXT == nullptr || glDeleteFramebuffersEXT == nullptr ||
	    glBlitFramebufferEXT == nullptr || glIsFramebufferEXT == nullptr ||
	    glGenerateMipmapEXT == nullptr || glGetFramebufferAttachmentParameterivEXT == nullptr) {
		glGenRenderbuffersEXT = nullptr;
		glBindRenderbufferEXT = nullptr;
		glRenderbufferStorageMultisampleEXT = nullptr;
		glGenFramebuffersEXT = nullptr;
		glBindFramebufferEXT = nullptr;
		glFramebufferRenderbufferEXT = nullptr;
		glCheckFramebufferStatusEXT = nullptr;
		glFramebufferTexture2DEXT = nullptr;
		glDeleteRenderbuffersEXT = nullptr;
		glDeleteFramebuffersEXT = nullptr;
		glBlitFramebufferEXT = nullptr;
		glIsFramebufferEXT = nullptr;
		glGenerateMipmapEXT = nullptr;
		glGetFramebufferAttachmentParameterivEXT = nullptr;

		return false;
	}

	// инициализируем GL_NV_framebuffer_multisample_coverage, как не обязательный
	glRenderbufferStorageMultisampleCoverageNV = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC) SDL_GL_GetProcAddress("glRenderbufferStorageMultisampleCoverageNV");


	return true;
}








//------------------------------------------------------------------------------------
// Создаем FBO с заданными параметрами (FBO - уже заранее подготовленный объект, в функции память не выделяем)
//------------------------------------------------------------------------------------
bool vw_BuildFBO(sFBO *FBO, int Width, int Height, bool NeedColor, bool NeedDepth, int MSAA, int *CSAA)
{
	if ((FBO == nullptr) ||
	    (glGenRenderbuffersEXT == nullptr) ||
	    (glBindRenderbufferEXT == nullptr) ||
	    (glRenderbufferStorageMultisampleEXT == nullptr) ||
	    (glGenFramebuffersEXT == nullptr) ||
	    (glBindFramebufferEXT == nullptr) ||
	    (glFramebufferRenderbufferEXT == nullptr) ||
	    (glCheckFramebufferStatusEXT == nullptr))
		return false;

	// если не поддерживаем ковередж - просто ставим MSAA
	if (glRenderbufferStorageMultisampleCoverageNV == nullptr)
		if (CSAA != nullptr)
			*CSAA = MSAA;

	int InternalCSAA = MSAA;
	if (CSAA != nullptr)
		InternalCSAA = *CSAA;


	FBO->Width = Width;
	FBO->Height = Height;
	FBO->ColorBuffer = 0;
	FBO->DepthBuffer = 0;
	FBO->ColorTexture = 0;
	FBO->DepthTexture = 0;
	FBO->FrameBufferObject = 0;
	FBO->DepthSize = 0;

	std::cout << "Frame Buffer Object creation start with parameters ";
	std::cout << Width << " " << Height << " " << NeedColor << " " << NeedDepth << " " << MSAA << " " << InternalCSAA << "\n";

	glGenFramebuffersEXT(1, &FBO->FrameBufferObject);
	glBindFramebufferEXT(GL_FRAMEBUFFER, FBO->FrameBufferObject);


	if (NeedColor) {
		// если есть мультисемплы - делаем буфером, если нет - текстурой
		if (MSAA >= 2) {
			glGenRenderbuffersEXT(1, &FBO->ColorBuffer);
			glBindRenderbufferEXT(GL_RENDERBUFFER, FBO->ColorBuffer);
			if ((InternalCSAA == MSAA) || (InternalCSAA == 0))
				glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, MSAA, GL_RGBA, FBO->Width, FBO->Height);
			else
				glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, InternalCSAA, MSAA, GL_RGBA, FBO->Width, FBO->Height);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, FBO->ColorBuffer);
			if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "Can't create FRAMEBUFFER.\n\n";
				return false;
			}
		} else {
			glGenTextures(1, &FBO->ColorTexture);
			glBindTexture(GL_TEXTURE_2D, FBO->ColorTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FBO->Width, FBO->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBO->ColorTexture, 0);
			if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "Can't create FRAMEBUFFER.\n\n";
				return false;
			}
		}
	}


	if (NeedDepth) {
		// если есть мультисемплы - делаем буфером, если нет - текстурой
		if (MSAA >= 2) {
			glGenRenderbuffersEXT(1, &FBO->DepthBuffer);
			glBindRenderbufferEXT(GL_RENDERBUFFER, FBO->DepthBuffer);
			if ((InternalCSAA == MSAA) || (InternalCSAA == 0))
				// Ставим "GL_DEPTH_COMPONENT" (No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available ...)
				glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, MSAA, GL_DEPTH_COMPONENT, FBO->Width, FBO->Height);
			else
				// Ставим "GL_DEPTH_COMPONENT" (No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available ...)
				glRenderbufferStorageMultisampleCoverageNV(GL_RENDERBUFFER, InternalCSAA, MSAA, GL_DEPTH_COMPONENT, FBO->Width, FBO->Height);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBO->DepthBuffer);
			// получаем назначенную драйвером глубину depth буфера
			glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &FBO->DepthSize);
			if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "Can't create FRAMEBUFFER.\n\n";
				return false;
			}
		} else {
			glGenTextures(1, &FBO->DepthTexture);
			glBindTexture(GL_TEXTURE_2D, FBO->DepthTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Ставим "GL_DEPTH_COMPONENT" (No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available ...)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, FBO->Width, FBO->Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBO->DepthTexture, 0);
			// получаем назначенную драйвером глубину depth буфера
			glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &FBO->DepthSize);
			if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "Can't create FRAMEBUFFER.\n\n";
				return false;
			}
		}
	}


	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	std::cout << "Frame Buffer Object created. Depth Size: " << FBO->DepthSize << "\n";

	// запоминаем максимальные полученные данные по буферу глубины
	if (vw_GetDevCaps()->FramebufferObjectDepthSize < FBO->DepthSize) vw_GetDevCaps()->FramebufferObjectDepthSize = FBO->DepthSize;

	return true;
}







//------------------------------------------------------------------------------------
// Установка FBO
//------------------------------------------------------------------------------------
void vw_BindFBO(sFBO *FBO)
{
	if ((glBindFramebufferEXT == nullptr) ||
	    ((FBO != nullptr) && (FBO->FrameBufferObject == 0)))
		return;

	// проверяем текущий
	if ((CurrentFBO != nullptr) && // если текущий был с мультисемплами - надо их выключить
	    ((CurrentFBO->ColorBuffer != 0) || (CurrentFBO->DepthBuffer != 0)))
		glDisable(GL_MULTISAMPLE);

	if (FBO != nullptr) {
		if ((FBO->ColorBuffer != 0) || (FBO->DepthBuffer != 0))
			glEnable(GL_MULTISAMPLE);
		glBindFramebufferEXT(GL_FRAMEBUFFER, FBO->FrameBufferObject);
	} else {
		// ставим основной фрейм буфер
		glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
	}

	CurrentFBO = FBO;
}







//------------------------------------------------------------------------------------
// получаем текущий установленный FBO, 0 - если фрейм буфер
//------------------------------------------------------------------------------------
sFBO *vw_GetCurrentFBO()
{
	return CurrentFBO;
}








//------------------------------------------------------------------------------------
// Блит FBO
//------------------------------------------------------------------------------------
void vw_BlitFBO(sFBO *SourceFBO, sFBO *TargetFBO)
{
	if ((SourceFBO == nullptr) || (TargetFBO == nullptr) ||
	    (glBindFramebufferEXT == nullptr) || (glBlitFramebufferEXT == nullptr) ||
	    (SourceFBO->FrameBufferObject == 0) || (TargetFBO->FrameBufferObject == 0))
		return;

	glBindFramebufferEXT(GL_READ_FRAMEBUFFER, SourceFBO->FrameBufferObject);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, TargetFBO->FrameBufferObject);
	glBlitFramebufferEXT( 0, 0, SourceFBO->Width, SourceFBO->Height,
			      0, 0, TargetFBO->Width, TargetFBO->Height,
			      GL_COLOR_BUFFER_BIT, GL_LINEAR);
}










//------------------------------------------------------------------------------------
// Рисуем FBO в указанный FBO
//------------------------------------------------------------------------------------
void vw_DrawColorFBO(sFBO *SourceFBO, sFBO *TargetFBO)
{
	if ((SourceFBO == nullptr) || (SourceFBO->ColorTexture == 0))
		return;


	vw_BindFBO(TargetFBO);
	glViewport(0, 0, SourceFBO->Width, SourceFBO->Height);


	// рисуем текстуру на весь экран

	// запоминаем состояние флагов
	glPushAttrib(GL_ENABLE_BIT);
	// и выключаем "ненужные"
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);								//select the projection matrix
	glPushMatrix();												//store the projection matrix
	glLoadIdentity();											//reset the projection matrix

	glOrtho(0, SourceFBO->Width, 0, SourceFBO->Height, -1, 1);

	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix
	glPushMatrix();
	glLoadIdentity();


	// RI_2f_XY | RI_1_TEX
	float *buff = new float[4*4];

	int k = 0;

	buff[k++] = 0.0f;
	buff[k++] = 0.0f;
	buff[k++] = 0.0f;
	buff[k++] = 0.0f;

	buff[k++] = 0.0f;
	buff[k++] = SourceFBO->Height;
	buff[k++] = 0.0f;
	buff[k++] = 1.0f;

	buff[k++] = SourceFBO->Width;
	buff[k++] = 0.0f;
	buff[k++] = 1.0f;
	buff[k++] = 0.0f;

	buff[k++] = SourceFBO->Width;
	buff[k++] = SourceFBO->Height;
	buff[k++] = 1.0f;
	buff[k++] = 1.0f;


	vw_BindTexture(0, SourceFBO->ColorTexture);

	vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, buff, 4*sizeof(buff[0]));

	vw_BindTexture(0, 0);
	delete [] buff;


	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);			//select the projection matrix
	glPopMatrix();							//restore the old projection matrix

	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix

	// восстанавливаем флаги
	glPopAttrib();

}











//------------------------------------------------------------------------------------
// Удаление данных FBO
//------------------------------------------------------------------------------------
void vw_DeleteFBO(sFBO *FBO)
{
	if ((FBO == nullptr) ||
	    (glDeleteRenderbuffersEXT == nullptr) ||
	    (glDeleteFramebuffersEXT == nullptr))
		return;


	if (FBO->ColorTexture != 0) {
		glDeleteTextures(1, &FBO->ColorTexture);
		FBO->ColorTexture=0;
	};
	if (FBO->DepthTexture != 0) {
		glDeleteTextures(1, &FBO->DepthTexture);
		FBO->DepthTexture=0;
	};

	if (FBO->ColorBuffer != 0) {
		glDeleteRenderbuffersEXT(1, &FBO->ColorBuffer);
		FBO->ColorBuffer=0;
	};
	if (FBO->DepthBuffer != 0) {
		glDeleteRenderbuffersEXT(1, &FBO->DepthBuffer);
		FBO->DepthBuffer=0;
	};

	if (FBO->FrameBufferObject != 0) {
		glDeleteFramebuffersEXT(1, &FBO->FrameBufferObject);
		FBO->FrameBufferObject=0;
	};
}

