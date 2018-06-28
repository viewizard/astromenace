/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "graphics.h"

namespace viewizard {

/*
 * Set lighting status.
 */
void vw_Lighting(bool param)
{
	if (param)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
}

/*
 * Enable light.
 */
void vw_LightEnable(GLenum light, bool param)
{
	if (param)
		glEnable(GL_LIGHT0 + light);
	else
		glDisable(GL_LIGHT0 + light);
}

/*
 * Set light parameter.
 */
void vw_SetLight(GLenum light, eLightParameter pname, GLfloat param)
{
	glLightf(GL_LIGHT0 + light, static_cast<GLenum>(pname), param);
}

/*
 * Set light parameter by array.
 */
void vw_SetLightV(GLenum light, eLightVParameter pname, const GLfloat *param)
{
	glLightfv(GL_LIGHT0 + light, static_cast<GLenum>(pname), param);
}

/*
 *  Set material parameter by array.
 */
void vw_MaterialV(eMaterialParameter pname, const GLfloat *param)
{
	glMaterialfv(GL_FRONT_AND_BACK, static_cast<GLenum>(pname), param);
}

} // viewizard namespace
