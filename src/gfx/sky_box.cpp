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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#include "../game.h"

namespace {

enum eSide {
	RIGHT	= 0,	// The RIGHT side
	LEFT	= 1,	// The LEFT side
	BOTTOM	= 2,	// The BOTTOM side
	TOP	= 3,	// The TOP side
	BACK	= 4,	// The BACK side
	FRONT	= 5	// The FRONT side
};

} // unnamed namespace


//-----------------------------------------------------------------------------
// local/protected variables
//-----------------------------------------------------------------------------

// SkyBox textures
GLtexture SkyBox_Texture[6]{0, 0, 0, 0, 0, 0};
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
void SkyBoxDraw()
{
	int VFV = RI_3f_XYZ | RI_2_TEX | RI_DUBLICATE_TEX_COORD;
	float *buff = new float[5*4];
	int k;


	// сразу выполняем настройку второй текстуры
	vw_BindTexture(1, vw_FindTextureByName("skybox/tile_stars.tga"));
	vw_SetTextureEnvMode(eTextureEnvMode::DECAL);
	// корректируем текстурные координаты для второй текстуры через матрицу
	vw_MatrixMode(eMatrixMode::TEXTURE);
	vw_LoadIdentity();
	vw_Scale(2.0f, 2.0f, 1.0f);
	vw_MatrixMode(eMatrixMode::MODELVIEW);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The BACK side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[BACK]) {
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

		vw_BindTexture(0, SkyBox_Texture[BACK]);
		vw_SendVertices(ePrimitiveType::TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(buff[0]));
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The FRONT side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[FRONT]) {
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

		vw_BindTexture(0, SkyBox_Texture[FRONT]);
		vw_SendVertices(ePrimitiveType::TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(buff[0]));

	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The BOTTOM side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[BOTTOM]) {
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

		vw_BindTexture(0, SkyBox_Texture[BOTTOM]);
		vw_SendVertices(ePrimitiveType::TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(buff[0]));
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The TOP side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[TOP]) {
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

		vw_BindTexture(0, SkyBox_Texture[TOP]);
		vw_SendVertices(ePrimitiveType::TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(buff[0]));
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The LEFT side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[LEFT]) {
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

		vw_BindTexture(0, SkyBox_Texture[LEFT]);
		vw_SendVertices(ePrimitiveType::TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(buff[0]));
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// The RIGHT side
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (SkyBox_Texture[RIGHT]) {
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

		vw_BindTexture(0, SkyBox_Texture[RIGHT]);
		vw_SendVertices(ePrimitiveType::TRIANGLE_STRIP, 4, VFV, buff, 5*sizeof(buff[0]));
	}


	vw_BindTexture(1, 0);
	vw_MatrixMode(eMatrixMode::TEXTURE);
	vw_LoadIdentity();
	vw_MatrixMode(eMatrixMode::MODELVIEW);

	vw_BindTexture(0, 0);

	if (buff != nullptr)
		delete [] buff;
}





//-----------------------------------------------------------------------------
// Setup texture for each SkyBox side
//-----------------------------------------------------------------------------
void SkyBoxSetTexture(GLtexture TextureID, int Side)
{
	SkyBox_Texture[Side] = TextureID;
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
