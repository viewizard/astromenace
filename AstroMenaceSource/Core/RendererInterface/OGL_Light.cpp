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
// Вкл-выкл освещения (глобально)
//------------------------------------------------------------------------------------
void vw_Lighting(bool param)
{
	if (param) glEnable(GL_LIGHTING);
		else glDisable(GL_LIGHTING);

}




//------------------------------------------------------------------------------------
// Получение состояния
//------------------------------------------------------------------------------------
bool vw_GetLighting(void)
{
	if (glIsEnabled(GL_LIGHTING)) return true;
	return false;
}



//------------------------------------------------------------------------------------
// Включение источника света
//------------------------------------------------------------------------------------
void vw_LightEnable(int light, bool param)
{
	if (param) glEnable(GL_LIGHT0+light);
	else glDisable(GL_LIGHT0+light);
}



//------------------------------------------------------------------------------------
// Получение состояния источника света
//------------------------------------------------------------------------------------
bool vw_GetLightEnable(int light)
{
	if (glIsEnabled(GL_LIGHT0+light)) return true;
	return false;
}



//------------------------------------------------------------------------------------
// Установка атрибутов источнику света
//------------------------------------------------------------------------------------
void vw_SetLight(int light, int pname, float param)
{
	int tmpPNAME=0;

	switch (pname)
	{
		case RI_SPOT_EXPONENT:
			tmpPNAME = GL_SPOT_EXPONENT;
			break;

		case RI_SPOT_CUTOFF:
			tmpPNAME = GL_SPOT_CUTOFF;
			break;

		case RI_CONSTANT_ATTENUATION:
			tmpPNAME = GL_CONSTANT_ATTENUATION;
			break;

		case RI_LINEAR_ATTENUATION:
			tmpPNAME = GL_LINEAR_ATTENUATION;
			break;

		case RI_QUADRATIC_ATTENUATION:
			tmpPNAME = GL_QUADRATIC_ATTENUATION;
			break;

		default: 
			fprintf(stderr, "Error in vw_SetLight function call, wrong pname.\n");
			return;
 	}

	glLightf(GL_LIGHT0+light, tmpPNAME, param);
}



//------------------------------------------------------------------------------------
// Установка атрибутов источника света через указатель
//------------------------------------------------------------------------------------
void vw_SetLightV(int light, int pname, const float *param)
{
	int tmpPNAME=0;

	switch (pname)
	{
		case RI_SPOT_EXPONENT:
			tmpPNAME = GL_SPOT_EXPONENT;
			break;

		case RI_SPOT_CUTOFF:
			tmpPNAME = GL_SPOT_CUTOFF;
			break;

		case RI_CONSTANT_ATTENUATION:
			tmpPNAME = GL_CONSTANT_ATTENUATION;
			break;

		case RI_LINEAR_ATTENUATION:
			tmpPNAME = GL_LINEAR_ATTENUATION;
			break;

		case RI_QUADRATIC_ATTENUATION:
			tmpPNAME = GL_QUADRATIC_ATTENUATION;
			break;

		case RI_AMBIENT:
			tmpPNAME = GL_AMBIENT;
			break;

		case RI_DIFFUSE:
			tmpPNAME = GL_DIFFUSE;
			break;

		case RI_SPECULAR:
			tmpPNAME = GL_SPECULAR;
			break;

		case RI_POSITION:
			tmpPNAME = GL_POSITION;
			break;

		case RI_DIRECTION:
			tmpPNAME = GL_SPOT_DIRECTION;
			break;

		default:
			fprintf(stderr, "Error in vw_SetLightV function call, wrong pname.\n");
			return;
	}

	glLightfv(GL_LIGHT0+light, tmpPNAME, param);
}



//------------------------------------------------------------------------------------
// Получение атрибутов источника света через указатель
//------------------------------------------------------------------------------------
void vw_GetLightV(int light, int pname, float *param)
{
	int tmpPNAME=0;

	switch (pname)
	{
		case RI_SPOT_EXPONENT:
			tmpPNAME = GL_SPOT_EXPONENT;
			break;

		case RI_SPOT_CUTOFF:
			tmpPNAME = GL_SPOT_CUTOFF;
			break;

		case RI_CONSTANT_ATTENUATION:
			tmpPNAME = GL_CONSTANT_ATTENUATION;
			break;

		case RI_LINEAR_ATTENUATION:
			tmpPNAME = GL_LINEAR_ATTENUATION;
			break;

		case RI_QUADRATIC_ATTENUATION:
			tmpPNAME = GL_QUADRATIC_ATTENUATION;
			break;

		case RI_AMBIENT:
			tmpPNAME = GL_AMBIENT;
			break;

		case RI_DIFFUSE:
			tmpPNAME = GL_DIFFUSE;
			break;

		case RI_SPECULAR:
			tmpPNAME = GL_SPECULAR;
			break;

		case RI_POSITION:
			tmpPNAME = GL_POSITION;
			break;

		case RI_DIRECTION:
			tmpPNAME = GL_SPOT_DIRECTION;
			break;

		default:
			fprintf(stderr, "Error in vw_GetLightV function call, wrong pname.\n");
			return;
	}

	glGetLightfv(GL_LIGHT0+light, tmpPNAME, param);
}





//------------------------------------------------------------------------------------
// Установка атрибутов материала через указатель
//------------------------------------------------------------------------------------
void vw_MaterialV(int pname, const float *param)
{
	int tmpPNAME=0;

	switch (pname)
	{
		case RI_AMBIENT:
			tmpPNAME = GL_AMBIENT;
			break;

		case RI_DIFFUSE:
			tmpPNAME = GL_DIFFUSE;
			break;

		case RI_SPECULAR:
			tmpPNAME = GL_SPECULAR;
			break;

		case RI_EMISSION:
			tmpPNAME = GL_EMISSION;
			break;

		case RI_SHININESS:
			tmpPNAME = GL_SHININESS;
			break;

		default:
			fprintf(stderr, "Error in vw_MaterialV function call, wrong pname.\n");
			return;
	}

	glMaterialfv(GL_FRONT_AND_BACK, tmpPNAME, param);
}



//------------------------------------------------------------------------------------
// Получение атрибутов материала через указатель
//------------------------------------------------------------------------------------
void vw_GetMaterialV(int pname, float *param)
{
	int tmpPNAME=0;

	switch (pname)
	{
		case RI_AMBIENT:
			tmpPNAME = GL_AMBIENT;
			break;

		case RI_DIFFUSE:
			tmpPNAME = GL_DIFFUSE;
			break;

		case RI_SPECULAR:
			tmpPNAME = GL_SPECULAR;
			break;

		case RI_EMISSION:
			tmpPNAME = GL_EMISSION;
			break;

		case RI_SHININESS:
			tmpPNAME = GL_SHININESS;
			break;

		default:
			fprintf(stderr, "Error in vw_GetMaterialV function call, wrong pname.\n");
			return;
	}

	glGetMaterialfv(GL_FRONT_AND_BACK, tmpPNAME, param);
}




