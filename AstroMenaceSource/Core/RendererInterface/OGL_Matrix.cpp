/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2013 Michael Kurinnoy, Viewizard


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
	Project: http://sourceforge.net/projects/openastromenace/
	E-mail: viewizard@viewizard.com

*************************************************************************************/


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
void vw_Rotate(float fAngle, float fX, float fY, float fZ)
{
	glRotatef(fAngle, fX, fY, fZ);
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
		case RI_TEXTURE_MATRIX: glGetFloatv(GL_TEXTURE, params); break;
		default: fprintf(stderr, "Error in vw_GetMatrix function call, wrong pname.\n"); break;
	}
}
void vw_SetMatrix(float *params)
{
	glLoadMatrixf(params);
}





//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_MatrixMode(int pname)
{
	switch (pname)
	{
		case RI_PROJECTION_MATRIX: glMatrixMode(GL_PROJECTION); break;
		case RI_MODELVIEW_MATRIX: glMatrixMode(GL_MODELVIEW); break;
		case RI_TEXTURE_MATRIX: glMatrixMode(GL_TEXTURE); break;
		default: fprintf(stderr, "Error in vw_MatrixMode function call, wrong pname.\n"); break;
	}
}




//------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void vw_MultMatrix(float *params)
{
	glMultMatrixf(params);
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
