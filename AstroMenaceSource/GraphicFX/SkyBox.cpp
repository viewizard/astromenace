/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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


#include "../Game.h"




//-----------------------------------------------------------------------------
// local/protected variables
//-----------------------------------------------------------------------------

// SkyBox textures
eTexture *SkyBox_Texture[6]={0,0,0,0,0,0};
// SkyBox location
float SkyBox_x = 0.0f;
float SkyBox_y = 0.0f;
float SkyBox_z = 0.0f;
// SkyBox size
float SkyBox_width_2 = 100.0f;
float SkyBox_height_2 = 100.0f;
float SkyBox_length_2 = 100.0f;






//-----------------------------------------------------------------------------
// Draw all SkyBox sides
//-----------------------------------------------------------------------------
void SkyBoxDraw(void)
{
	int VFV = RI_3f_XYZ | RI_2_TEX | RI_DUBLICATE_TEX_COORD;
	float *buff = 0;
	buff = new float[5*4]; if (buff == 0) return;
	int k;


	// сразу выполняем настройку второй текстуры
	vw_SetTexture(1, vw_FindTextureByName("DATA/SKYBOX/tile_stars.tga"));
	vw_SetTextureEnvMode(RI_TENV_DECAL);
	vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
	// по умолчанию всегда трилинейная фильтрация, если надо - ставим билинейную
	if (Setup.TextureFilteringMode == 1) vw_SetTextureFiltering(RI_TEXTURE_BILINEAR);
	// корректируем текстурные координаты для второй текстуры через матрицу
	vw_MatrixMode(RI_TEXTURE_MATRIX);
	vw_LoadIdentity();
	vw_Scale(2.0f, 2.0f, 1.0f);
	vw_MatrixMode(RI_MODELVIEW_MATRIX);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The BACK side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[BACK] != 0)
	{
		k=0;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;

		vw_SetTexture(0, SkyBox_Texture[BACK]);
		vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(float));
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The FRONT side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[FRONT] != 0)
	{
		k=0;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;

		vw_SetTexture(0, SkyBox_Texture[FRONT]);
		vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(float));

	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The BOTTOM side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[BOTTOM] != 0)
	{
		k=0;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;

		vw_SetTexture(0, SkyBox_Texture[BOTTOM]);
		vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(float));
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The TOP side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[TOP] != 0)
	{
		k=0;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;

		vw_SetTexture(0, SkyBox_Texture[TOP]);
		vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(float));
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The LEFT side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[LEFT] != 0)
	{
		k=0;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x - SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;

		vw_SetTexture(0, SkyBox_Texture[LEFT]);
		vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(float));
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The RIGHT side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[RIGHT] != 0)
	{
		k=0;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y - SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 0.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z - SkyBox_length_2;
		buff[k++] = 1.0f;
		buff[k++] = 1.0f;
		buff[k++] = SkyBox_x + SkyBox_width_2;
		buff[k++] = SkyBox_y + SkyBox_height_2;
		buff[k++] = SkyBox_z + SkyBox_length_2;
		buff[k++] = 0.0f;
		buff[k++] = 1.0f;

		vw_SetTexture(0, SkyBox_Texture[RIGHT]);
		vw_SetTextureAnisotropy(Setup.AnisotropyLevel);
		vw_SendVertices(RI_TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(float));
	}


	vw_BindTexture(1, 0);
	vw_MatrixMode(RI_TEXTURE_MATRIX);
	vw_LoadIdentity();
	vw_MatrixMode(RI_MODELVIEW_MATRIX);

	vw_BindTexture(0, 0);

	if (buff != 0){delete [] buff; buff = 0;}
}





//-----------------------------------------------------------------------------
// Setup texture for each SkyBox side
//-----------------------------------------------------------------------------
void SkyBoxSetTexture(eTexture *nTexture, int Side)
{
	SkyBox_Texture[Side] = nTexture;
}



//-----------------------------------------------------------------------------
// Create SkyBox
//-----------------------------------------------------------------------------
void SkyBoxCreate(float nx, float ny, float nz,
							 float nwidth, float nheight, float nlength)
{
	SkyBox_x = nx;
	SkyBox_y = ny;
	SkyBox_z = nz;
	SkyBox_width_2 = nwidth/2.0f;
	SkyBox_height_2 = nheight/2.0f;
	SkyBox_length_2 = nlength/2.0f;
}
