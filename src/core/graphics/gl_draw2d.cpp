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

// NOTE GL_EXT_direct_state_access (since OpenGL 4.5)
//      glGetTextureLevelParameterfv()
//      could be used to replace glGetTexLevelParameterfv()

#include "../math/math.h"
#include "../texture/texture.h"
#include "graphics.h"

namespace {

constexpr unsigned int DrawBufferSize{16}; // RI_2f_XYZ | RI_2f_TEX = (2 + 2) * 4 vertices = 16
float DrawBuffer[DrawBufferSize];
unsigned int DrawBufferCurrentPosition{0};

} // unnamed namespace


/*
 * Switch to 2D rendering mode. Origin is upper left corner.
 */
void vw_Start2DMode(GLdouble zNear, GLdouble zFar)
{
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	// get current viewport start point and size
	int ViewportX, ViewportY, ViewportWidth, ViewportHeight;
	vw_GetViewport(&ViewportX, &ViewportY, &ViewportWidth, &ViewportHeight);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	float AW{0.0f};
	float AH{0.0f};

	// care about fixed aspect ratio, that could be setted up
	// change origin to upper left corner
	if (vw_GetAspectWH(&AW, &AH))
		glOrtho(ViewportX * AW / ViewportWidth, (ViewportX + ViewportWidth) * AW / ViewportWidth,
			(ViewportY + ViewportHeight) * AH / ViewportHeight, ViewportY * AH / ViewportHeight,
			zNear, zFar);
	else
		glOrtho(0, ViewportWidth, ViewportHeight, 0, zNear, zFar);

	// change textures origin to upper left corner
	vw_SelectActiveTextureUnit(0); // switch to 0 unit, for proper texture matrix
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glScalef(1.0, -1.0, 1.0);
	glTranslatef(0.0f, -1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

/*
 * Switch back to 3D rendering mode.
 * Restore matrices and attributes in reverse to vw_Start2DMode() sequence.
 */
void vw_End2DMode()
{
	// we don't switch to 0 unit, in 2D mode only 0 unit should be used
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

/*
 * Add data to local draw buffer.
 */
static inline void AddToDrawBuffer(float CoordX, float CoordY, float TextureU, float TextureV)
{
	DrawBuffer[DrawBufferCurrentPosition++] = CoordX;
	DrawBuffer[DrawBufferCurrentPosition++] = CoordY;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureU;
	DrawBuffer[DrawBufferCurrentPosition++] = TextureV;
}

/*
 * Draw transparent. Origin is upper left corner.
 */
void vw_Draw2D(const sRECT &DstRect, const sRECT &SrcRect, GLtexture Texture, bool Alpha,
	       float Transp, float RotateAngle, const sRGBCOLOR &Color)
{
	if (!Texture || (Transp <= 0.0f))
		return;

	// bind texture before glGetTexLevelParameterfv() call
	vw_BindTexture(0, Texture);

	float ImageHeight{0.0f};
	float ImageWidth{0.0f};
	// if texture loaded via textures manager, get data from it
	if (!vw_FindTextureSizeByID(Texture, &ImageWidth, &ImageHeight)) {
		// get Width and Height for 0 mipmap level
		// call glGetTexLevelParameterfv() is generally not recommended,
		// since it could stall the OpenGL pipeline
		glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &ImageWidth);
		glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &ImageHeight);
	}
	if (!ImageHeight || !ImageWidth) {
		std::cerr << __func__ << "(): " << "zero image height or width not allowed.\n";
		return;
	}

	// texture's UV coordinates
	float U_left = (SrcRect.left * 1.0f) / ImageWidth;
	float V_top = (SrcRect.top * 1.0f) / ImageHeight;
	float U_right = (SrcRect.right * 1.0f) / ImageWidth;
	float V_bottom = (SrcRect.bottom * 1.0f) / ImageHeight;

	// 'reset' buffer
	DrawBufferCurrentPosition = 0;

	// RI_TRIANGLE_STRIP (2 triangles)
	AddToDrawBuffer(DstRect.left, DstRect.top, U_left, V_top);
	AddToDrawBuffer(DstRect.left, DstRect.bottom, U_left, V_bottom);
	AddToDrawBuffer(DstRect.right, DstRect.top, U_right, V_top);
	AddToDrawBuffer(DstRect.right, DstRect.bottom, U_right, V_bottom);

	// setup OpenGL
	vw_SetTextureBlend(Alpha, eTextureBlendFactor::SRC_ALPHA, eTextureBlendFactor::ONE_MINUS_SRC_ALPHA);
	vw_Clamp(Transp, 0.0f, 1.0f);
	vw_SetColor(Color.r, Color.g, Color.b, Transp);
	glPushMatrix();
	glRotatef(RotateAngle, 0, 0, 1);

	vw_SendVertices(RI_TRIANGLE_STRIP, 4, RI_2f_XY | RI_1_TEX, DrawBuffer, DrawBufferSize);

	// restore previous OpenGL states
	glPopMatrix();
	vw_SetTextureBlend(false, eTextureBlendFactor::ONE, eTextureBlendFactor::ZERO);
	vw_SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	vw_BindTexture(0, 0);
}
