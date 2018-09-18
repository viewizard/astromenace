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


	Website: https://viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// NOTE GL_GENERATE_MIPMAP (since OpenGL 1.4)
//      could be used to replace GL_GENERATE_MIPMAP_SGIS and gluBuild2DMipmaps()

// NOTE GL_EXT_direct_state_access (since OpenGL 4.5)
//      glTextureStorage2DEXT() + glTextureSubImage2DEXT() + glGenerateTextureMipmapEXT()
//      could be used with texture's name directly, without bind target

// NOTE GL_EXT_direct_state_access (since OpenGL 4.5)
//      glBindTextureUnitEXT()
//      could be used to replace glActiveTexture() + glBindTexture()

// NOTE GL_EXT_direct_state_access (since OpenGL 4.5)
//      glTextureParameteri()
//      could be used to replace glTexParameteri()

// NOTE GL_TEXTURE_MAX_ANISOTROPY (since OpenGL 4.6)
//      could be used to replace GL_TEXTURE_MAX_ANISOTROPY_EXT

// NOTE glTexEnvi() deprecated in OpenGL 3.1 core profile
//      (vw_SetTextureEnvMode(), vw_SetTextureBlendMode())

#include "../texture/texture.h"
#include "graphics_internal.h"
#include "graphics.h"
#include "extensions.h"

namespace viewizard {

/*
 * Create texture.
 */
GLtexture vw_BuildTexture(const std::unique_ptr<uint8_t[]> &PixelsArray,
			  GLsizei Width, GLsizei Height, bool MipMap, int Bytes,
			  eTextureCompressionType CompressionType)
{
	if (!PixelsArray.get())
		return 0;

	GLenum Format{GL_RGB};
	if (Bytes == 4)
		Format = GL_RGBA;

	GLenum InternalFormat{GL_RGB8};
	switch (CompressionType) {
	case eTextureCompressionType::BPTC:
		if (!vw_DevCaps().ARB_texture_compression_bptc &&
		    !vw_DevCaps().OpenGL_4_2_supported) // also part of OpenGL 4.2
			return 0;
		if (Bytes == 4)
			InternalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM;
		else
			InternalFormat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
		break;

	case eTextureCompressionType::S3TC:
		if (!vw_DevCaps().EXT_texture_compression_s3tc)
			return 0;
		if (Bytes == 4)
			InternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		else
			InternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		break;

	case eTextureCompressionType::NONE:
		if (Bytes == 4)
			InternalFormat = GL_RGBA8;
		break;
	}

	GLtexture TextureID{0};
	glGenTextures(1, &TextureID);
	vw_BindTexture(0, TextureID);

	if (MipMap) {
		// use newest available first
		if (pfn_glGenerateMipmap && pfn_glTexStorage2D) {
			GLsizei NeedMipMapLvls = 1 + static_cast<GLsizei>(std::floor(std::log2(std::max(Width, Height))));
			pfn_glTexStorage2D(GL_TEXTURE_2D, NeedMipMapLvls, InternalFormat, Width, Height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, Format,
					GL_UNSIGNED_BYTE, PixelsArray.get());
			pfn_glGenerateMipmap(GL_TEXTURE_2D);
		} else if (pfn_glGenerateMipmap) {
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format,
				     GL_UNSIGNED_BYTE, PixelsArray.get());
			pfn_glGenerateMipmap(GL_TEXTURE_2D);
		} else if (vw_DevCaps().SGIS_generate_mipmap) {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format,
				     GL_UNSIGNED_BYTE, PixelsArray.get());
		} else
			gluBuild2DMipmaps(GL_TEXTURE_2D, InternalFormat, Width, Height, Format,
					  GL_UNSIGNED_BYTE, PixelsArray.get());
	} else {
		if (pfn_glTexStorage2D) {
			pfn_glTexStorage2D(GL_TEXTURE_2D, 1, InternalFormat, Width, Height);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Width, Height, Format,
					GL_UNSIGNED_BYTE, PixelsArray.get());
		} else
			glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format,
				     GL_UNSIGNED_BYTE, PixelsArray.get());
	}

	return TextureID;
}

/*
 * Select active texture unit (starts from 0, for GL_TEXTURE0 unit).
 */
void vw_SelectActiveTextureUnit(GLenum Unit)
{
	if (pfn_glActiveTexture)
		pfn_glActiveTexture(GL_TEXTURE0 + Unit);
}

/*
 * Bind texture for particular texture unit (starts from 0, for GL_TEXTURE0 unit).
 */
void vw_BindTexture(GLenum Unit, GLtexture TextureID)
{
	vw_SelectActiveTextureUnit(Unit);

	if (TextureID) {
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glEnable(GL_TEXTURE_2D);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
}

/*
 * Delete texture.
 */
void vw_DeleteTexture(GLtexture TextureID)
{
	glDeleteTextures(1, &TextureID);
}

/*
 * Set texture blending mode.
 * This is deprecated part (since OpenGL 3.1), no reason make it fully functional.
 */
void vw_SetTextureBlendMode(eTextureCombinerName name, eTextureCombinerOp param)
{
	glTexEnvi(GL_TEXTURE_ENV, static_cast<GLenum>(name),  static_cast<GLint>(param));
}

/*
 * Set texture filtering mode.
 */
void vw_SetTextureFiltering(eTextureMinFilter MinFilter, eTextureMagFilter MagFilter)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(MinFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(MagFilter));
}

/*
 * Set texture filtering mode.
 */
void vw_SetTextureFiltering(const sTextureFilter &Filter)
{
	vw_SetTextureFiltering(Filter.Min, Filter.Mag);
}

/*
 * Set texture Anisotropy Level.
 */
void vw_SetTextureAnisotropy(GLint AnisotropyLevel)
{
	if (vw_DevCaps().MaxAnisotropyLevel > 0) {
		if (AnisotropyLevel < 1) {
			std::cerr << __func__ << "(): " << "anisotropy level " << AnisotropyLevel
				  << " not supported, changed to 1\n";
			AnisotropyLevel = 1;
		} else if (AnisotropyLevel > vw_DevCaps().MaxAnisotropyLevel) {
			std::cerr << __func__ << "(): " << "anisotropy level " << AnisotropyLevel
				  << " not supported, reduced to " << vw_DevCaps().MaxAnisotropyLevel
				  << "\n";
			AnisotropyLevel = vw_DevCaps().MaxAnisotropyLevel;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, AnisotropyLevel);
	}
}

/*
 * Set texture address mode.
 */
void vw_SetTextureAddressMode(eTextureWrapCoord coord, eTextureWrapMode mode)
{
	glTexParameteri(GL_TEXTURE_2D, static_cast<GLenum>(coord), static_cast<GLint>(mode));
}

/*
 * Set texture address mode.
 */
void vw_SetTextureAddressMode(const sTextureWrap &wrap)
{
	vw_SetTextureAddressMode(eTextureWrapCoord::S, wrap.S);
	vw_SetTextureAddressMode(eTextureWrapCoord::T, wrap.T);
	vw_SetTextureAddressMode(eTextureWrapCoord::R, wrap.R);
}

/*
 * Set texture Alpha Test value that specifies a reference alpha value against which pixels are tested.
 */
void vw_SetTextureAlphaTest(bool flag, eCompareFunc func, GLclampf ref)
{
	if (flag) {
		glAlphaFunc(static_cast<GLenum>(func), ref);
		glEnable(GL_ALPHA_TEST);
	} else {
		// ignore parameters and setup initial values
		glAlphaFunc(GL_ALWAYS, 0);
		glDisable(GL_ALPHA_TEST);
	}
}

/*
 * Set texture blending factor.
 */
void vw_SetTextureBlend(bool flag, eTextureBlendFactor sfactor, eTextureBlendFactor dfactor)
{
	if (!flag) {
		glDisable(GL_BLEND);
		// ignore parameters and setup initial values
		glBlendFunc(GL_ONE, GL_ZERO);
		return;
	}

	glEnable(GL_BLEND);
	glBlendFunc(static_cast<GLenum>(sfactor), static_cast<GLenum>(dfactor));
}

/*
 * Set texture compare mode.
 */
void vw_SetTextureCompare(eTextureCompareMode mode, eCompareFunc func)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, static_cast<GLint>(mode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, static_cast<GLint>(func));
}

/*
 * Set texture depth mode.
 */
void vw_SetTextureDepthMode(eTextureDepthMode mode)
{
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, static_cast<GLint>(mode));
}

/*
 * Set texture env mode.
 */
void vw_SetTextureEnvMode(eTextureEnvMode mode)
{
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, static_cast<GLint>(mode));
}

} // viewizard namespace
