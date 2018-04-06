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

#include "../math/math.h"
#include "../texture/texture.h"
#include "graphics.h"






//------------------------------------------------------------------------------------
// Переход на 2D режима вывода
//------------------------------------------------------------------------------------
void vw_Start2DMode(float nZ1, float nZ2)
{
	// запоминаем состояние флагов
	glPushAttrib(GL_ENABLE_BIT);
	// и выключаем "ненужные"
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	// берем размер вьюпорта
	int X, Y, W, H;
	vw_GetViewport(&X, &Y, &W, &H);

	// переводим его в флоат для расчетов (нужно чтобы результаты были флоат)
	float AWw = W*1.0f;
	float AHw = H*1.0f;

	glMatrixMode(GL_PROJECTION);								//select the projection matrix
	glPushMatrix();												//store the projection matrix
	glLoadIdentity();											//reset the projection matrix

	// смотрим, была ли установка на фиксированный внутренний размер
	float AW;
	float AH;
	bool ASpresent = vw_GetAspectWH(&AW, &AH);


	if (ASpresent)
		glOrtho(X*(AW/AWw), (X+W)*(AW/AWw), Y*(AH/AHw), (Y+H)*(AH/AHw), nZ1, nZ2);
	else
		glOrtho(0, AWw, 0, AHw, nZ1, nZ2);	//set up an ortho screen

	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix
	glPushMatrix();
	glLoadIdentity();

}



//------------------------------------------------------------------------------------
// Возвращение в обычный (3D) режим вывода
//------------------------------------------------------------------------------------
void vw_End2DMode()
{
	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);			//select the projection matrix
	glPopMatrix();							//restore the old projection matrix

	glMatrixMode(GL_MODELVIEW);				//select the modelview matrix

	// восстанавливаем флаги
	glPopAttrib();
}






//------------------------------------------------------------------------------------
// Прорисовка в 2д
//------------------------------------------------------------------------------------
void vw_Draw(int X, int Y, sRECT *SrcRect, GLtexture Texture, bool Alpha, float RotateAngle, int DrawCorner)
{
	if (!Texture)
		return;

	float AW;
	float AH;
	bool ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	float AHw = H*1.0f;

	// Установка текстуры и ее свойств...
	vw_BindTexture(0, Texture);
	vw_SetTextureBlend(Alpha, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);


	// Вычисление поправки по У в зависимости от DrawCorner
	// - расположения угла начала координат
	float tmpPosY = 0;
	// изменяем только в случае RI_UL_CORNER
	if (DrawCorner == RI_UL_CORNER) {
		if (ASpresent) tmpPosY = (AH - Y - Y - (SrcRect->bottom - SrcRect->top));
		else tmpPosY = (AHw - Y - Y - (SrcRect->bottom - SrcRect->top));
	}


	float ImageHeight{0.0f};
	float ImageWidth{0.0f};
	vw_FindTextureSizeByID(Texture, &ImageWidth, &ImageHeight);

	float FrameHeight = (SrcRect->bottom*1.0f)/ImageHeight;
	float FrameWidth = (SrcRect->right*1.0f)/ImageWidth;

	float Yst = (SrcRect->top)/ImageHeight;
	float Xst = (SrcRect->left)/ImageWidth;


	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX
	float *tmp = new float[(2+2)*4];
	int k = 0;

	tmp[k++] = X;
	tmp[k++] = Y +tmpPosY + (SrcRect->bottom - SrcRect->top);
	tmp[k++] = Xst;
	tmp[k++] = 1.0f-Yst;

	tmp[k++] = X;
	tmp[k++] = Y +tmpPosY;
	tmp[k++] = Xst;
	tmp[k++] = 1.0f-FrameHeight;

	tmp[k++] = X + (SrcRect->right - SrcRect->left);
	tmp[k++] = Y +tmpPosY + (SrcRect->bottom - SrcRect->top);
	tmp[k++] = FrameWidth;
	tmp[k++] = 1.0f-Yst;

	tmp[k++] = X + (SrcRect->right - SrcRect->left);
	tmp[k++] = Y +tmpPosY;
	tmp[k++] = FrameWidth;
	tmp[k++] = 1.0f-FrameHeight;

	glPushMatrix();
	glRotatef(RotateAngle, 0, 0, 1);

	vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, tmp, 4*sizeof(tmp[0]));
	glPopMatrix();


	delete [] tmp;
	vw_SetTextureBlend(false, 0, 0);
	vw_BindTexture(0, 0);
}





//------------------------------------------------------------------------------------
// Прорисовка в 2д с прозрачностью
//------------------------------------------------------------------------------------
void vw_DrawTransparent(sRECT *DstRect, sRECT *SrcRect, GLtexture Texture, bool Alpha,
			float Transp, float RotateAngle, int DrawCorner, float R, float G, float B)
{

	if (!Texture || (Transp <= 0.0f))
		return;
	if (Transp > 1.0f)
		Transp = 1.0f;

	float AW;
	float AH;
	bool ASpresent = vw_GetAspectWH(&AW, &AH);

	int W, H;
	vw_GetViewport(nullptr, nullptr, &W, &H);
	float AHw = H*1.0f;

	int X = DstRect->left;
	int Y = DstRect->top;

	// Установка текстуры и ее свойств...
	vw_BindTexture(0, Texture);
	vw_SetTextureBlend(Alpha, RI_BLEND_SRCALPHA, RI_BLEND_INVSRCALPHA);

	// Вычисление поправки по У в зависимости от DrawCorner
	// - расположения угла начала координат
	float tmpPosY = 0;
	// изменяем только в случае RI_UL_CORNER
	if (DrawCorner == RI_UL_CORNER) {
		if (ASpresent) tmpPosY = (AH - Y - Y - (DstRect->bottom - DstRect->top));
		else tmpPosY = (AHw - Y - Y - (DstRect->bottom - DstRect->top));
	}



	float ImageHeight{0.0f};
	float ImageWidth{0.0f};
	vw_FindTextureSizeByID(Texture, &ImageWidth, &ImageHeight);

	float FrameHeight = (SrcRect->bottom*1.0f )/ImageHeight;
	float FrameWidth = (SrcRect->right*1.0f )/ImageWidth;

	float Yst = (SrcRect->top*1.0f)/ImageHeight;
	float Xst = (SrcRect->left*1.0f)/ImageWidth;


	vw_SetColor(R, G, B, Transp);

	// буфер для последовательности RI_TRIANGLE_STRIP
	// войдет RI_2f_XYZ | RI_2f_TEX
	float *tmp = new float[(2+2)*4];
	int k=0;

	tmp[k++] = X;
	tmp[k++] = Y +tmpPosY + (DstRect->bottom - DstRect->top);
	tmp[k++] = Xst;
	tmp[k++] = 1.0f-Yst;

	tmp[k++] = X;
	tmp[k++] = Y +tmpPosY;
	tmp[k++] = Xst;
	tmp[k++] = 1.0f-FrameHeight;

	tmp[k++] = X + (DstRect->right - DstRect->left);
	tmp[k++] = Y +tmpPosY + (DstRect->bottom - DstRect->top);
	tmp[k++] = FrameWidth;
	tmp[k++] = 1.0f-Yst;

	tmp[k++] = X + (DstRect->right - DstRect->left);
	tmp[k++] = Y +tmpPosY;
	tmp[k++] = FrameWidth;
	tmp[k++] = 1.0f-FrameHeight;


	glPushMatrix();
	glRotatef(RotateAngle, 0, 0, 1);

	vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, tmp, 4*sizeof(tmp[0]));

	glPopMatrix();



	delete [] tmp;

	vw_SetTextureBlend(false, 0, 0);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BindTexture(0, 0);

}
