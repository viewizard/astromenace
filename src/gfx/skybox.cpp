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


	Web Site: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO add initialization via XML file, hard coded initialization should be removed
// TODO remove GetPreloadedTextureAsset() call from main loop
// TODO move to one 'static' VBO + VAO
//      no IBO need since we use triangle strip, use range+count for each side (6 textures with 1 VBO)
//      re-create on init and size changes only + care about release on game restart

/*
SkyBox should be rendered in special way, with 2 textures for each side.

First textures - space, unique for each side, without any stars (small elements),
with 'nebulae' of galactics.

Second texture - small elements (stars). One texture for all sides, tiled, with larger size than first
textures. Since this one is tiled, we could scale it in 2 times for each side for better look.
*/

#include "../core/core.h"
#include "../assets/texture.h"
#include "skybox.h"

namespace {

// SkyBox textures
std::array<GLtexture, static_cast<unsigned>(eSide::size)> SkyBox_Texture{};
// SkyBox location
float SkyBoxX{0.0f};
float SkyBoxY{0.0f};
float SkyBoxZ{0.0f};
// SkyBox size
float SkyBoxHalfWidth{100.0f};
float SkyBoxHalfHeight{100.0f};
float SkyBoxHalfLength{100.0f};

// vertex array for rendering
float VertexArray[20]; // 4 * 5 = 4 vertices * (RI_3f_XYZ + RI_2_TEX);
unsigned int VertexArrayPosition{0};

} // unnamed namespace


/*
 * Draw local vertex array.
 */
static inline void DrawVertexArray(eSide Side)
{
	vw_BindTexture(0, SkyBox_Texture[static_cast<unsigned>(Side)]);
	vw_Draw3D(ePrimitiveType::TRIANGLE_STRIP, 4,
		  RI_3f_XYZ | RI_2_TEX | RI_DUBLICATE_TEX_COORD,
		  VertexArray, 5 * sizeof(VertexArray[0]));
}

/*
 * Add vertex to  to local vertex array.
 */
static inline void AddToVertexArray(float CoordX, float CoordY, float CoordZ,
				    float TextureU, float TextureV)
{
	VertexArray[VertexArrayPosition++] = CoordX;
	VertexArray[VertexArrayPosition++] = CoordY;
	VertexArray[VertexArrayPosition++] = CoordZ;
	VertexArray[VertexArrayPosition++] = TextureU;
	VertexArray[VertexArrayPosition++] = TextureV;
}

/*
 * Draw initialized SkyBox sides.
 */
void SkyBoxDraw()
{
	// setup second texture unit (with small stars)
	vw_BindTexture(1, GetPreloadedTextureAsset("skybox/tile_stars.tga"));
	vw_SetTextureEnvMode(eTextureEnvMode::DECAL);
	vw_MatrixMode(eMatrixMode::TEXTURE);
	vw_LoadIdentity();
	vw_Scale(2.0f, 2.0f, 1.0f);
	vw_MatrixMode(eMatrixMode::MODELVIEW);

	if (SkyBox_Texture[static_cast<unsigned>(eSide::BACK)]) {
		VertexArrayPosition = 0;
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 0.0f, 0.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 0.0f, 1.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 1.0f, 0.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 1.0f, 1.0f);
		DrawVertexArray(eSide::BACK);
	}

	if (SkyBox_Texture[static_cast<unsigned>(eSide::FRONT)]) {
		VertexArrayPosition = 0;
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 0.0f, 0.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 0.0f, 1.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 1.0f, 0.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 1.0f, 1.0f);
		DrawVertexArray(eSide::FRONT);
	}

	if (SkyBox_Texture[static_cast<unsigned>(eSide::BOTTOM)]) {
		VertexArrayPosition = 0;
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 0.0f, 0.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 0.0f, 1.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 1.0f, 0.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 1.0f, 1.0f);
		DrawVertexArray(eSide::BOTTOM);
	}

	if (SkyBox_Texture[static_cast<unsigned>(eSide::TOP)]) {
		VertexArrayPosition = 0;
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 1.0f, 1.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 1.0f, 0.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 0.0f, 1.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 0.0f, 0.0f);
		DrawVertexArray(eSide::TOP);
	}

	if (SkyBox_Texture[static_cast<unsigned>(eSide::LEFT)]) {
		VertexArrayPosition = 0;
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 0.0f, 1.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 1.0f, 1.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 0.0f, 0.0f);
		AddToVertexArray(SkyBoxX - SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 1.0f, 0.0f);
		DrawVertexArray(eSide::LEFT);
	}

	if (SkyBox_Texture[static_cast<unsigned>(eSide::RIGHT)]) {
		VertexArrayPosition = 0;
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 1.0f, 0.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY - SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 0.0f, 0.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ - SkyBoxHalfLength,
				 1.0f, 1.0f);
		AddToVertexArray(SkyBoxX + SkyBoxHalfWidth, SkyBoxY + SkyBoxHalfHeight, SkyBoxZ + SkyBoxHalfLength,
				 0.0f, 1.0f);
		DrawVertexArray(eSide::RIGHT);
	}

	vw_BindTexture(1, 0);
	vw_MatrixMode(eMatrixMode::TEXTURE);
	vw_LoadIdentity();
	vw_MatrixMode(eMatrixMode::MODELVIEW);
	vw_BindTexture(0, 0);
}

/*
 * Setup texture for each SkyBox side.
 */
void SkyBoxSetTexture(GLtexture TextureID, eSide Side)
{
	SkyBox_Texture[static_cast<unsigned>(Side)] = TextureID;
}

/*
 * Create SkyBox.
 */
void SkyBoxCreate(float X, float Y, float Z, float Width, float Height, float Length)
{
	SkyBoxX = X;
	SkyBoxY = Y;
	SkyBoxZ = Z;
	SkyBoxHalfWidth = Width / 2.0f;
	SkyBoxHalfHeight = Height / 2.0f;
	SkyBoxHalfLength = Length / 2.0f;
}
