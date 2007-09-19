/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: OGL_Matrix.cpp

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








//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_LoadIdentity()
{
	glLoadIdentity();
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_Translate(VECTOR3D Location)
{
	glTranslatef(Location.x, Location.y, Location.z);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_Rotate(float fX, float fY, float fZ)
{
	if (fX != 0.0f) glRotatef(fX, 1.0f, 0.0f, 0.0f);
	if (fY != 0.0f) glRotatef(fY, 0.0f, 1.0f, 0.0f);
	if (fZ != 0.0f) glRotatef(fZ, 0.0f, 0.0f, 1.0f);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_Scale(float fX, float fY, float fZ)
{
	glScalef(fX, fY, fZ);
}




//------------------------------------------------------------------------------------
// получение матриц
//------------------------------------------------------------------------------------
void vw_GetMatrix(int pname, float *params)
{
	switch (pname)
	{
		case RI_PROJECTION_MATRIX: glGetFloatv(GL_PROJECTION_MATRIX, params); break;
		case RI_MODELVIEW_MATRIX: glGetFloatv(GL_MODELVIEW_MATRIX, params); break;
	}
}
void vw_SetMatrix(int pname, float *params)
{
	int MatrixMode;
	glGetIntegerv(GL_MATRIX_MODE, &MatrixMode);
	switch (pname)
	{
		case RI_PROJECTION_MATRIX: glMatrixMode(GL_PROJECTION); break;
		case RI_MODELVIEW_MATRIX: glMatrixMode(GL_MODELVIEW); break;
	}
	glLoadMatrixf(params);
	glMatrixMode(MatrixMode);
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_PushMatrix(void)
{
	glPushMatrix();
}



//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_PopMatrix(void)
{
	glPopMatrix();
}
