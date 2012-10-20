/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: ShadowMap.cpp

	Copyright (c) 2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

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


#include "../Game.h"




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
float ShadowMapViewPort_znear, ShadowMapViewPort_zfar;






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
	//if (vw_BuildFBO(ShadowMapFBO, Width, Height, true, true))
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

	// сохраняем данные вьюпорта
	vw_GetViewport(&ShadowMapViewPort_x, &ShadowMapViewPort_y, &ShadowMapViewPort_width, &ShadowMapViewPort_height, &ShadowMapViewPort_znear, &ShadowMapViewPort_zfar);

	vw_SetViewport(0, 0, ShadowMapFBO->Width, ShadowMapFBO->Height, 0.045f, fFarClip, RI_BL_CORNER);

	// сохраняем данные текущего фбо или фб
	CurrentSystemFBO = vw_GetCurrentFBO();

	// устанавливаем фбо
	vw_BindFBO(ShadowMapFBO);

	vw_Clear(RI_DEPTH_BUFFER);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//vw_Clear(RI_COLOR_BUFFER | RI_DEPTH_BUFFER);

	vw_ResizeScene(45.0f, (ShadowMapFBO->Width*1.0f)/(ShadowMapFBO->Height*1.0f), 0.045f, fFarClip);
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
	//vw_CullFace(RI_NONE);
}


//-----------------------------------------------------------------------------
// завершение рендеринга в шадовмеп фбо
//-----------------------------------------------------------------------------
void ShadowMap_EndRenderToFBO(float AR)
{
	if (ShadowMapFBO == 0) return;
	if (ShadowMapFBO->DepthTexture == 0) return;

	vw_CullFace(RI_BACK);

	// устанавливаем на место основной фбо или фб
	vw_BindFBO(CurrentSystemFBO);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//--------- рисуем цвет сост. для теста
//	vw_DrawColorFBO(ShadowMapFBO, CurrentSystemFBO);

	// устанавливаем первоначальный вьюпорт
	vw_SetViewport(ShadowMapViewPort_x, ShadowMapViewPort_y, ShadowMapViewPort_width, ShadowMapViewPort_height, ShadowMapViewPort_znear, ShadowMapViewPort_zfar, RI_BL_CORNER);
	vw_ResizeScene(45.0f, AR, 1.0f, 2000.0f);

	// восстанавливаем модельвью матрицу
	vw_PopMatrix();
}





//-----------------------------------------------------------------------------
// начало рендеринга моделей с тенями
//-----------------------------------------------------------------------------
void ShadowMap_StartFinalRender(unsigned int TextureStage)
{
	if (ShadowMapFBO == 0) return;
	if (ShadowMapFBO->DepthTexture == 0) return;

	vw_BindTexture(TextureStage, ShadowMapFBO->DepthTexture);
	// т.е. будем использовать shadow2DProj
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	// ставим линеар сглаживание, чтобы PCF делало более плавные переходы
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


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
void ShadowMap_EndFinalRender(unsigned int TextureStage)
{
	if (ShadowMapFBO == 0) return;
	if (ShadowMapFBO->DepthTexture == 0) return;

	vw_BindTexture(TextureStage, 0);
}

