/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: OGL_Texture.cpp

	Copyright (c) 2003-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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


extern	PFNGLACTIVETEXTUREPROC glActiveTexture13;
extern	PFNGLGENERATEMIPMAPPROC glGenerateMipmapEXT;
extern	PFNGLTEXSTORAGE2DPROC glTexStorage2DEXT;






//------------------------------------------------------------------------------------
// Генерация текстуры
//------------------------------------------------------------------------------------
GLuint vw_CreateTexture(BYTE *ustDIB, int Width, int Height, bool MipMap, int Bytes, bool NeedCompression)
{
	// ничего не передали
	if (ustDIB == 0) return 0;

	GLuint TextureID = 0;

	glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);
	// ... It has been reported that on some ATI drivers, glGenerateMipmap(GL_TEXTURE_2D)
	// has no effect unless you precede it with a call to glEnable(GL_TEXTURE_2D) ...
	// "перестраховываемся" и включаем его постоянно при генерации текстур
	glEnable(GL_TEXTURE_2D);

	int Mode;
	int Color;
	int InternalFormat;
	eDevCaps *OpenGL_DevCaps = vw_GetDevCaps();


	if (OpenGL_DevCaps->TexturesCompression & NeedCompression)
	{
		if (Bytes == 4)
		{	// компрессия 4 к 1
			Mode = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			Color = GL_RGBA;
			InternalFormat = GL_RGBA8;
		}
		else
		{	// компрессия 6 к 1, считаем 4 слоя
			Mode = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			Color = GL_RGB;
			InternalFormat = GL_RGB8;
		}
	}
	else
	{
		if (Bytes == 4)
		{
			Mode = GL_RGBA;
			Color = GL_RGBA;
			InternalFormat = GL_RGBA8;
		}
		else
		{	// считаем 4 слоя (фактически их задействуем)
			Mode = GL_RGB;
			Color = GL_RGB;
			InternalFormat = GL_RGB8;
		}
	}


	if (MipMap)
	{	// используем по порядку наиболее новые решения при генерации мипмепов
		if ((glGenerateMipmapEXT != NULL) & (glTexStorage2DEXT != NULL))
		{
			// считаем сколько нужно создавать мипмапов
			int NeedMipMapLvl = 1;
			int MaxSize = Width;
			if (MaxSize < Height) MaxSize = Height;
			while (MaxSize > 2) {MaxSize = MaxSize/2; NeedMipMapLvl++;};
			// со стореджем уже не используем компрессию
			glTexStorage2DEXT(GL_TEXTURE_2D, NeedMipMapLvl, InternalFormat, Width, Height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, Color, GL_UNSIGNED_BYTE, ustDIB);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
		else
		if (glGenerateMipmapEXT != NULL)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, Mode, Width, Height,	0, Color, GL_UNSIGNED_BYTE, ustDIB);
			glGenerateMipmapEXT(GL_TEXTURE_2D);
		}
		else
		if (OpenGL_DevCaps->HardwareMipMapGeneration)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, Mode, Width, Height,	0, Color, GL_UNSIGNED_BYTE, ustDIB);
		}
		else
		{
			// делаем через glu...
			gluBuild2DMipmaps(GL_TEXTURE_2D, Mode, Width, Height, Color, GL_UNSIGNED_BYTE, ustDIB);
		}
	}
	else // без мипмепов
	{
		if (glTexStorage2DEXT != NULL)
		{
			// со стореджем уже не используем компрессию
			glTexStorage2DEXT(GL_TEXTURE_2D, 1, InternalFormat, Width, Height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, Color, GL_UNSIGNED_BYTE, ustDIB);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, Mode, Width, Height, 0, Color, GL_UNSIGNED_BYTE, ustDIB);
		}
	}


	glDisable(GL_TEXTURE_2D);

	return TextureID;
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_DeleteTexture(GLuint TextureID)
{
	glDeleteTextures(1, &TextureID);
}





//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTextureV(DWORD Stage, eTexture *Texture)
{
	if (Texture == 0) return;

	Texture->TexturePrior = 1.0f;


	glActiveTexture13(GL_TEXTURE0 + Stage);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture->TextureID);

	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

	if (Stage > 0)
	{
		// данные для смешение цвета - добавляем к существ.
		glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD_SIGNED_EXT);
		glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
		glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
		// данные для смешение альфы - всегда 1-й слой...
		glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_ADD);
		glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
		glTexEnvf (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
		glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT);
		glTexEnvf (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);
	}
	else
	{
		// первая текстура, нужно заменить все
		glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
		glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
		glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);

		glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_MODULATE);
		glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
		glTexEnvf (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
		glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT);
		glTexEnvf (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);
	}
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTextureBlendMode(DWORD stage, int pname, int param)
{
	GLenum  cmd = GL_COMBINE_RGB;
	GLenum  arg = GL_REPLACE;

	switch (pname)
	{
		case RI_TBLEND_COLOROP:
			cmd = GL_COMBINE_RGB;
			break;
		case RI_TBLEND_ALPHAOP:
			cmd = GL_COMBINE_ALPHA;
			break;
		case RI_TBLEND_ALPHAARG1:
			cmd = GL_OPERAND0_ALPHA;
			break;
		case RI_TBLEND_ALPHAARG2:
			cmd = GL_OPERAND1_ALPHA;
			break;
		case RI_TBLEND_ALPHAARG3:
			cmd = GL_OPERAND2_ALPHA;
			break;
		case RI_TBLEND_COLORARG1:
			cmd = GL_SOURCE0_RGB;
			break;
		case RI_TBLEND_COLORARG2:
			cmd = GL_SOURCE1_RGB;
			break;
		case RI_TBLEND_COLORARG3:
			cmd = GL_SOURCE2_RGB;
			break;
	}

	switch (param)
	{
		case RI_TBLEND_SOURCE1:
			arg = GL_REPLACE;
			break;
		case RI_TBLEND_SOURCE2:
			arg = GL_REPLACE;
			break;
		case RI_TBLEND_MODULATE:
			arg = GL_MODULATE;
			break;
		case RI_TBLEND_MODULATE2X:
			arg = GL_MODULATE;
			break;
		case RI_TBLEND_MODULATE4X:
			arg = GL_MODULATE;
			break;
		case RI_TBLEND_ADD:
			arg = GL_ADD;
			break;
		case RI_TBLEND_ADDSMOOTH:
			arg = GL_ADD;
			break;
		case RI_TBLEND_ADD_SIGNED:
			arg = GL_ADD_SIGNED;
			break;
		case RI_TBLEND_SUBTRACT:
			arg = GL_SUBTRACT;
			break;
		case RI_TBLEND_DIFFUSE_ALPHA:
		case RI_TBLEND_TEXTURE_ALPHA:
		case RI_TBLEND_CURRENT_ALPHA:
			arg = GL_INTERPOLATE;
			break;
		case RI_TBLEND_DOTPRODUCT:
			arg = GL_DOT3_RGB;
			break;

		case RI_TBLEND_CURRENT:
			arg = GL_PREVIOUS;
			break;
		case RI_TBLEND_TEXTURE:
			arg = GL_TEXTURE;
			break;
		case RI_TBLEND_CONSTANT:
			arg = GL_CONSTANT;
			break;
		case RI_TBLEND_DIFFUSE:
			arg = GL_PRIMARY_COLOR;
			break;
		case RI_TBLEND_SPECULAR: // ---
			arg = 0;
			break;
	}


	glActiveTexture13(GL_TEXTURE0 + stage);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

	// работаем с MODULATE
	switch (param)
	{
		case RI_TBLEND_MODULATE2X:
			if (pname == RI_TBLEND_COLOROP) glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);
			else glTexEnvi(GL_TEXTURE_ENV, 	GL_ALPHA_SCALE , 2);
			break;
		case RI_TBLEND_MODULATE4X:
			if (pname == RI_TBLEND_COLOROP) glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);
			else glTexEnvi(GL_TEXTURE_ENV, 	GL_ALPHA_SCALE , 4);
			break;
		default:
			if (pname == RI_TBLEND_COLOROP) glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
			else glTexEnvi(GL_TEXTURE_ENV, 	GL_ALPHA_SCALE , 1);
			break;
	}


	glTexEnvi(GL_TEXTURE_ENV, cmd, arg);
}




//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTexFiltering(DWORD Stage, int nFiltering, int AnisotropyLevel)
{
	glActiveTexture13(GL_TEXTURE0 + Stage);

	// перебираем по мип-меп фильтру
	switch (nFiltering & 0x00000F)
	{
		case 0: // RI_MIPFILTER_NONE
			if ((nFiltering & 0x1030F0) == RI_MINFILTER_POINT)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			if ((nFiltering & 0x1030F0) == RI_MINFILTER_LINEAR)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			break;
		case 1: // RI_MIPFILTER_POINT
			if ((nFiltering & 0x1030F0) == RI_MINFILTER_POINT)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			if ((nFiltering & 0x1030F0) == RI_MINFILTER_LINEAR)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			break;
		case 2: // RI_MIPFILTER_LINEAR
			if ((nFiltering & 0x1030F0) == RI_MINFILTER_POINT)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			if ((nFiltering & 0x1030F0) == RI_MINFILTER_LINEAR)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			break;

	}

	// ставим MAG фильтр
	if ((nFiltering & 0x103F00) == RI_MAGFILTER_POINT)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if ((nFiltering & 0x103F00) == RI_MAGFILTER_LINEAR)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);




	// ставим ANISOTROPY
	if (AnisotropyLevel > 1)
	{
		eDevCaps *OpenGL_DevCaps = vw_GetDevCaps();

		if (OpenGL_DevCaps->MaxAnisotropyLevel > 1)
		{
			if (AnisotropyLevel > OpenGL_DevCaps->MaxAnisotropyLevel) AnisotropyLevel = OpenGL_DevCaps->MaxAnisotropyLevel;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, AnisotropyLevel);
		}
	}
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTexAddressMode(DWORD Stage, int nAddressMode)
{
	glActiveTexture13(GL_TEXTURE0 + Stage);

	if ((nAddressMode & 0x10410) == RI_WRAP_U)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // не ставить просто GL_CLAMP, нвидия все равно ставит GL_CLAMP_TO_EDGE

	if ((nAddressMode & 0x10401) == RI_WRAP_V)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // не ставить просто GL_CLAMP, нвидия все равно ставит GL_CLAMP_TO_EDGE
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTexAlpha(bool Flag, float Value)
{
	if (Flag)
	{
		glAlphaFunc(GL_GREATER, Value);
		glEnable(GL_ALPHA_TEST);
	}
	else
	{
		glAlphaFunc(GL_GREATER, 1.0f);
		glDisable(GL_ALPHA_TEST);
	}
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTexBlend(int Src, int Dst)
{
	int tmpSRC,tmpDST;

	switch(Src)
	{
		case RI_BLEND_ZERO:
			tmpSRC = GL_ZERO;
			break;
		case RI_BLEND_ONE:
			tmpSRC = GL_ONE;
			break;
		case RI_BLEND_SRCCOLOR:
			tmpSRC = GL_SRC_COLOR;
			break;
		case RI_BLEND_INVSRCCOLOR:
			tmpSRC = GL_ONE_MINUS_SRC_COLOR;
			break;
		case RI_BLEND_INVSRCALPHA:
			tmpSRC = GL_ONE_MINUS_SRC_ALPHA;
			break;
		case RI_BLEND_DESTALPHA:
			tmpSRC = GL_DST_ALPHA;
			break;
		case RI_BLEND_INVDESTALPHA:
			tmpSRC = GL_ONE_MINUS_DST_ALPHA;
			break;
		case RI_BLEND_SRCALPHA:
			tmpSRC = GL_SRC_ALPHA;
			break;
		case RI_BLEND_DESTCOLOR:
			tmpSRC = GL_DST_COLOR;
			break;
		case RI_BLEND_INVDESTCOLOR:
			tmpSRC = GL_ONE_MINUS_DST_COLOR;
			break;
		case RI_BLEND_SRCALPHASAT:
			tmpSRC = GL_SRC_ALPHA_SATURATE;
			break;
		default:
			tmpSRC = GL_ONE;
			break;
	}
	switch(Dst)
	{
		case RI_BLEND_ZERO:
			tmpDST = GL_ZERO;
			break;
		case RI_BLEND_ONE:
			tmpDST = GL_ONE;
			break;
		case RI_BLEND_SRCCOLOR:
			tmpDST = GL_SRC_COLOR;
			break;
		case RI_BLEND_INVSRCCOLOR:
			tmpDST = GL_ONE_MINUS_SRC_COLOR;
			break;
		case RI_BLEND_INVSRCALPHA:
			tmpDST = GL_ONE_MINUS_SRC_ALPHA;
			break;
		case RI_BLEND_DESTALPHA:
			tmpDST = GL_DST_ALPHA;
			break;
		case RI_BLEND_INVDESTALPHA:
			tmpDST = GL_ONE_MINUS_DST_ALPHA;
			break;
		case RI_BLEND_SRCALPHA:
			tmpDST = GL_SRC_ALPHA;
			break;
		case RI_BLEND_DESTCOLOR:
			tmpDST = GL_DST_COLOR;
			break;
		case RI_BLEND_INVDESTCOLOR:
			tmpDST = GL_ONE_MINUS_DST_COLOR;
			break;
		case RI_BLEND_SRCALPHASAT:
			tmpDST = GL_SRC_ALPHA_SATURATE;
			break;
		default:
			tmpDST = GL_ZERO;
			break;
	}

	glEnable(GL_BLEND);
	glBlendFunc(tmpSRC, tmpDST);

}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_SetTextureDef(DWORD Stage)
{
	eDevCaps *OpenGL_DevCaps = vw_GetDevCaps();

    vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);


	glActiveTexture13(GL_TEXTURE0 + Stage);

	if (OpenGL_DevCaps->MaxAnisotropyLevel > 1)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);

	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
	// данные для смешение цвета - заменяем...
	glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

	if (Stage > 0)
	{
		// данные для смешение цвета - добавляем к существ.
		glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);
		// данные для смешение альфы - всегда 1-й слой...
		glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_ADD);
	}

	glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_MODULATE);
	glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_TEXTURE);
	glTexEnvf (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT, GL_SRC_ALPHA);
	glTexEnvf (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, GL_PREVIOUS_EXT);
	glTexEnvf (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT, GL_SRC_ALPHA);

	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture13(GL_TEXTURE0);
}







//------------------------------------------------------------------------------------
// получение GL_RGBA-GL_RGB набора из текстуры...
//------------------------------------------------------------------------------------
void vw_GetTextureImage(eTexture *Texture, void *bits, int BPP)
{
	if (Texture == 0) return;


	glBindTexture(GL_TEXTURE_2D, Texture->TextureID);
	if (BPP == 4)
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
	else
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);
}



//------------------------------------------------------------------------------------
// установка приоритета текстуры 0-1
//------------------------------------------------------------------------------------
void vw_SetPrioritizeTextures(GLuint TextureID, float Prior)
{
	glPrioritizeTextures( 1, &TextureID, &Prior);
}



//------------------------------------------------------------------------------------
// получение текущего приоритета
//------------------------------------------------------------------------------------
void vw_GetPrioritizeTextures(GLuint TextureID, float *Prior)
{
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glGetTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, Prior);
}




