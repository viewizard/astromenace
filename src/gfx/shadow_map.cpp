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


#include "../game.h"




//-----------------------------------------------------------------------------
// local/protected variables
//-----------------------------------------------------------------------------
eFBO *ShadowMapFBO = 0;
float ShadowMap_LightProjectionMatrix[16];
float ShadowMap_LightModelViewMatrix[16];

float xPixelOffset = 0;
float yPixelOffset = 0;
float ShadowMap_Get_xPixelOffset(){return xPixelOffset;}
float ShadowMap_Get_yPixelOffset(){return yPixelOffset;}


eFBO *CurrentSystemFBO = 0;
int ShadowMapViewPort_x, ShadowMapViewPort_y, ShadowMapViewPort_width, ShadowMapViewPort_height;






//-----------------------------------------------------------------------------
// инициализация шадов меп
//-----------------------------------------------------------------------------
bool ShadowMap_Init(int Width, int Height)
{
	if (ShadowMapFBO != 0) ShadowMap_Release();

	xPixelOffset = 1.0f/(Setup.Width * ((Width*1.0f)/Setup.Width));
	yPixelOffset = 1.0f/(Setup.Height * ((Height*1.0f)/Setup.Height));

	ShadowMapFBO = new eFBO;

	// для нормальной работы нам нужно 24 бита или больше, проверяем это
	//if (vw_BuildFBO(ShadowMapFBO, Width, Height, true, true)) // тест, для вывода цветовой составляющей на экран
	if (vw_BuildFBO(ShadowMapFBO, Width, Height, false, true))
		if (ShadowMapFBO->DepthSize >= 24)
			return true;

	// если неудачно создали, или недостаточная точность буфера глубины - уходим
	ShadowMap_Release();
	fprintf(stderr, "\n(!) You need at least 24 bits Depth Size for Shadow Mapping support.\n\n");
	return false;
}


//-----------------------------------------------------------------------------
// освобождаем память
//-----------------------------------------------------------------------------
void ShadowMap_Release()
{
	if (ShadowMapFBO == 0) return;

	vw_DeleteFBO(ShadowMapFBO);
	delete ShadowMapFBO;
	ShadowMapFBO = 0;
}







//-----------------------------------------------------------------------------
// подготовка рендеринга в шадовмеп фбо
//-----------------------------------------------------------------------------
void ShadowMap_StartRenderToFBO(VECTOR3D FocusPointCorrection, float Distance, float fFarClip)
{
	if (ShadowMapFBO == 0) return;
	if (ShadowMapFBO->DepthTexture == 0) return;

	// сохраняем модельвью матрицу
	vw_PushMatrix();

	// сохраняем данные вьюпорта (параметры буфера глубины не получаем, всегда используем его полностью, 0-1)
	vw_GetViewport(&ShadowMapViewPort_x, &ShadowMapViewPort_y, &ShadowMapViewPort_width, &ShadowMapViewPort_height);
	// устанавливаем вьюпорт согласно нашему фбо для шадовмепинга
	vw_SetViewport(0, 0, ShadowMapFBO->Width, ShadowMapFBO->Height, 0.005f, 1.0f, RI_BL_CORNER);

	// сохраняем данные текущего фбо или фб
	CurrentSystemFBO = vw_GetCurrentFBO();

	// устанавливаем фбо
	vw_BindFBO(ShadowMapFBO);

	vw_Clear(RI_DEPTH_BUFFER);
	vw_SetColorMask(false, false, false, false);
	//vw_Clear(RI_COLOR_BUFFER | RI_DEPTH_BUFFER); // тест, для вывода цветовой составляющей на экран

	// сохраняем матрицу проекции
	vw_MatrixMode(RI_PROJECTION_MATRIX);
	vw_PushMatrix();
	vw_MatrixMode(RI_MODELVIEW_MATRIX);

	vw_ResizeScene(45.0f, (ShadowMapFBO->Width*1.0f)/(ShadowMapFBO->Height*1.0f), 1.0f, fFarClip);
	vw_GetMatrix(RI_PROJECTION_MATRIX, ShadowMap_LightProjectionMatrix);


	// получаем данные направленного источника света
	eLight 	*CurrentDirectLight = vw_GetMainDirectLight();
	VECTOR3D LightPosition = CurrentDirectLight->Direction;
	LightPosition.Normalize();
	LightPosition = LightPosition^(-Distance);

	VECTOR3D CurrentCameraFocusPoint = vw_GetCameraFocusPoint();
	CurrentCameraFocusPoint += FocusPointCorrection;
	// т.к. у нас направленный свет, надо смещаться относительно точки куда светим
	LightPosition += CurrentCameraFocusPoint;

	gluLookAt(LightPosition.x, LightPosition.y, LightPosition.z,
				CurrentCameraFocusPoint.x, CurrentCameraFocusPoint.y, CurrentCameraFocusPoint.z,
				0.0f, 1.0f, 0.0f);

	vw_GetMatrix(RI_MODELVIEW_MATRIX, ShadowMap_LightModelViewMatrix);

	vw_CullFace(RI_FRONT);

	vw_PolygonOffset(true, 2.0f, 2.0f);
}


//-----------------------------------------------------------------------------
// завершение рендеринга в шадовмеп фбо
//-----------------------------------------------------------------------------
void ShadowMap_EndRenderToFBO()
{
	if (ShadowMapFBO == 0) return;
	if (ShadowMapFBO->DepthTexture == 0) return;

	vw_PolygonOffset(false, 0.0f, 0.0f);

	vw_CullFace(RI_BACK);

	// устанавливаем на место основной фбо или фб
	vw_BindFBO(CurrentSystemFBO);

	vw_SetColorMask(true, true, true, true);
//--------- рисуем цвет сост. для теста
//	vw_DrawColorFBO(ShadowMapFBO, CurrentSystemFBO);

	// устанавливаем первоначальный вьюпорт
	vw_SetViewport(ShadowMapViewPort_x, ShadowMapViewPort_y, ShadowMapViewPort_width, ShadowMapViewPort_height, 0.0f, 1.0f, RI_BL_CORNER);
	// восстанавливаем матрицу проекции
	vw_MatrixMode(RI_PROJECTION_MATRIX);
	vw_PopMatrix();
	vw_MatrixMode(RI_MODELVIEW_MATRIX);

	// восстанавливаем модельвью матрицу
	vw_PopMatrix();
}





//-----------------------------------------------------------------------------
// начало рендеринга моделей с тенями
//-----------------------------------------------------------------------------
void ShadowMap_StartFinalRender()
{
	if (ShadowMapFBO == 0) return;
	if (ShadowMapFBO->DepthTexture == 0) return;

	vw_BindTexture(2, ShadowMapFBO->DepthTexture);
	// т.к. будем использовать shadow2DProj, ставим правильный режим работы
	vw_SetTextureCompare(RI_COMPARE_R_TO_TEXTURE, RI_LESSEQUAL);
	vw_SetTextureDepthMode(RI_DEPTH_TEXTURE_MODE_INTENSITY);
	// ставим линеар сглаживание, чтобы PCF делало более плавные переходы
	vw_SetTextureFiltering(RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_NONE);


	vw_MatrixMode(RI_TEXTURE_MATRIX);
	vw_LoadIdentity();
	vw_Translate(VECTOR3D(0.5f, 0.5f, 0.5f)); // remap from [-1,1]^2 to [0,1]^2
	vw_Scale(0.5f, 0.5f, 0.5f);

	vw_MultMatrix(ShadowMap_LightProjectionMatrix);
	vw_MultMatrix(ShadowMap_LightModelViewMatrix);

	float CurrentInvModelView[16];
	vw_GetMatrix(RI_MODELVIEW_MATRIX, CurrentInvModelView);
	Matrix44InverseRotate(CurrentInvModelView);
	vw_MultMatrix(CurrentInvModelView);

	vw_MatrixMode(RI_MODELVIEW_MATRIX);
}




//-----------------------------------------------------------------------------
// завершение рендеринга моделей с тенями
//-----------------------------------------------------------------------------
void ShadowMap_EndFinalRender()
{
	if (ShadowMapFBO == 0) return;
	if (ShadowMapFBO->DepthTexture == 0) return;

	vw_BindTexture(2, 0);
}

