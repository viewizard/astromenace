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

// TODO translate comments

// TODO check sources, direct OpenGL usage outside 'graphics' are prohibited,
//      for 'graphics' code - switch to "vw_*" functions-wrapers for OpenGL if possible.

/*
Direct OpenGL usage outside 'graphics' are prohibited.
'graphics' code should also use "vw_*" functions-wrapers for OpenGL if possible.

Since OpenGL is evolving, more and more features allowed for new hardware.
'graphics' provide "vw_*" functions-wrapers for OpenGL, that use best solutions
for current hardware (and supported by code at this moment).
*/

#ifndef Graphics_H
#define Graphics_H

#include "../base.h"
#include "opengl.h"

struct sRECT;
struct sVECTOR3D;
class cTexture;

enum class eOrigin {
	upper_left,
	bottom_left
};

enum class eCompareFunc : GLint {
	LEQUAL = GL_LEQUAL,
	GEQUAL = GL_GEQUAL,
	LESS = GL_LESS,
	GREATER = GL_GREATER,
	EQUAL = GL_EQUAL,
	NOTEQUAL = GL_NOTEQUAL,
	ALWAYS = GL_ALWAYS,
	NEVER = GL_NEVER
};

enum class eMatrixMode : GLenum {
	PROJECTION = GL_PROJECTION,
	MODELVIEW = GL_MODELVIEW,
	TEXTURE = GL_TEXTURE
};

enum class eMatrixPname : GLenum {
	PROJECTION = GL_PROJECTION_MATRIX,
	MODELVIEW = GL_MODELVIEW_MATRIX,
	TEXTURE = GL_TEXTURE_MATRIX
};

enum class eTextureBlendFactor : GLenum {
	ZERO = GL_ZERO,
	ONE = GL_ONE,
	SRC_COLOR = GL_SRC_COLOR,
	ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
	DST_COLOR = GL_DST_COLOR,
	MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
	SRC_ALPHA = GL_SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
	DST_ALPHA = GL_DST_ALPHA,
	ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
	CONSTANT_COLOR = GL_CONSTANT_COLOR,
	ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR,
	CONSTANT_ALPHA = GL_CONSTANT_ALPHA,
	ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA,
	SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE,
	SRC1_COLOR = GL_SRC1_COLOR,
	ONE_MINUS_SRC1_COLOR = GL_ONE_MINUS_SRC1_COLOR,
	SRC1_ALPHA = GL_SRC1_ALPHA,
	ONE_MINUS_SRC1_ALPHA = GL_ONE_MINUS_SRC1_ALPHA
};

enum class eTextureEnvMode : GLint {
	DECAL = GL_DECAL,
	BLEND = GL_BLEND,
	REPLACE = GL_REPLACE,
	ADD = GL_ADD,
	COMBINE = GL_COMBINE,
	MODULATE = GL_MODULATE
};

enum class eTextureDepthMode : GLint {
	LUMINANCE = GL_LUMINANCE,
	INTENSITY = GL_INTENSITY,
	ALPHA = GL_ALPHA
};

enum class eTextureCompareMode : GLint {
	REF_TO_TEXTURE = GL_COMPARE_REF_TO_TEXTURE,
	NONE = GL_NONE
};

enum class eTextureWrapCoord : GLenum {
	S = GL_TEXTURE_WRAP_S,
	T = GL_TEXTURE_WRAP_T,
	R = GL_TEXTURE_WRAP_R
};

enum class eTextureWrapMode : GLint {
	CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
	MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
	REPEAT = GL_REPEAT,
	MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE
};

struct sTextureWrap {
	eTextureWrapMode S{eTextureWrapMode::REPEAT};
	eTextureWrapMode T{eTextureWrapMode::REPEAT};
	eTextureWrapMode R{eTextureWrapMode::REPEAT};

	sTextureWrap() = default;
	sTextureWrap(eTextureWrapMode param) :
		S{param},
		T{param},
		R{param}
	{}
};

enum class eTextureMinFilter : GLint {
	NEAREST = GL_NEAREST,
	LINEAR = GL_LINEAR,
	NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

enum class eTextureMagFilter : GLint {
	NEAREST = GL_NEAREST,
	LINEAR = GL_LINEAR
};

enum class eTextureBasicFilter {
	NONE,
	BILINEAR,
	TRILINEAR
};

struct sTextureFilter {
	eTextureMinFilter Min{eTextureMinFilter::NEAREST_MIPMAP_LINEAR};
	eTextureMagFilter Mag{eTextureMagFilter::LINEAR};

	sTextureFilter() = default;
	sTextureFilter(eTextureBasicFilter Filter)
	{
		switch (Filter) {
		case eTextureBasicFilter::NONE:
			Min = eTextureMinFilter::NEAREST;
			Mag = eTextureMagFilter::NEAREST;
			break;
		case eTextureBasicFilter::BILINEAR:
			Min = eTextureMinFilter::LINEAR;
			Mag = eTextureMagFilter::LINEAR;
			break;
		case eTextureBasicFilter::TRILINEAR:
			Min = eTextureMinFilter::LINEAR_MIPMAP_LINEAR;
			Mag = eTextureMagFilter::LINEAR;
			break;
		}
	}
};

enum class eTextureCombinerName : GLenum {
	COMBINE_RGB = GL_COMBINE_RGB
};

enum class eTextureCombinerOp : GLint {
	ADD = GL_ADD
};

enum class eTextureCompressionType : int {
	NONE = 0,
	S3TC = 1,
	BPTC = 2
};

enum class eMaterialParameter : GLenum {
	AMBIENT = GL_AMBIENT,
	DIFFUSE = GL_DIFFUSE,
	SPECULAR = GL_SPECULAR,
	EMISSION = GL_EMISSION,
	SHININESS = GL_SHININESS
};

enum class eLightParameter : GLenum {
	SPOT_EXPONENT = GL_SPOT_EXPONENT,
	SPOT_CUTOFF = GL_SPOT_CUTOFF,
	CONSTANT_ATTENUATION = GL_CONSTANT_ATTENUATION,
	LINEAR_ATTENUATION = GL_LINEAR_ATTENUATION,
	QUADRATIC_ATTENUATION = GL_QUADRATIC_ATTENUATION
};

enum class eLightVParameter : GLenum {
	SPOT_EXPONENT = GL_SPOT_EXPONENT,
	SPOT_CUTOFF = GL_SPOT_CUTOFF,
	CONSTANT_ATTENUATION = GL_CONSTANT_ATTENUATION,
	LINEAR_ATTENUATION = GL_LINEAR_ATTENUATION,
	QUADRATIC_ATTENUATION = GL_QUADRATIC_ATTENUATION,
	AMBIENT = GL_AMBIENT,
	DIFFUSE = GL_DIFFUSE,
	SPECULAR = GL_SPECULAR,
	POSITION = GL_POSITION,
	SPOT_DIRECTION = GL_SPOT_DIRECTION
};

enum class eBufferObject : GLenum {
	Vertex = GL_ARRAY_BUFFER,
	Index = GL_ELEMENT_ARRAY_BUFFER
};

struct sCoverageModes {
	int ColorSamples;
	int CoverageSamples;
};

struct sDevCaps {
	// версия OpenGL
	int OpenGLmajorVersion;
	int OpenGLminorVersion;
	// Max multtextures
	int MaxMultTextures;
	// Max texture width
	int MaxTextureWidth;
	// Max texture height
	int MaxTextureHeight;
	// макс. кол-во одновременно обрабатываемых источников света
	int MaxActiveLights;
	// максимальный уровень анизотропии
	GLint MaxAnisotropyLevel;
	// макс. уровень антиалиасинга, 0-нет, 2+ - есть
	int MaxSamples; // MSAA
	int MaxMultisampleCoverageModes; // кол-во профайлов антиалиасинга с CSAA+MSAA
	sCoverageModes MultisampleCoverageModes[32]; // собственно сам список режимов CSAA+MSAA, ставим просто 32 штуки, чтобы не заморачиваться с освобождением памяти
	// есть ли возможность включить сжатие текстур
	bool TexturesCompression;
	// поддержка GL_ARB_texture_compression_bptc
	bool TexturesCompressionBPTC;
	// GL_ARB_texture_storage
	bool TextureStorage;
	// поддержка FBO
	bool FramebufferObject;
	// глубина depth буфера в битах, получаем ее при первой генерации fbo с буфером глубины, по умолчанию 0
	// если работаем с fbo, то еще на этапе инициализации основного fbo прорисовки будут получены данные максимально поддерживаемой глубины
	GLint FramebufferObjectDepthSize;
	// поддержка VBO
	bool VBOSupported;
	// поддержка VAO
	bool VAOSupported;
	// поддержка загрузки текстур со сторонами не кратные степени двойки
	bool TextureNPOTSupported;
	// поддержка шейдеров GLSL 1.00
	bool GLSL100Supported;
	// шейдерная модель
	float ShaderModel;
	// поддержка генерации мипмеп в железе
	bool HardwareMipMapGeneration;
};

// Buffer clear bit
#define RI_COLOR_BUFFER		0x1000
#define RI_DEPTH_BUFFER		0x0100
#define RI_ACCUM_BUFFER		0x0010
#define RI_STENCIL_BUFFER	0x0001

// Primitives types
#define RI_POINTS			0x1020
#define RI_LINES			0x1021
#define RI_TRIANGLES			0x1022
#define RI_TRIANGLE_STRIP		0x1023
#define RI_TRIANGLE_FAN			0x1024
// Data format
#define RI_3f_XYZ			0x0001000
#define RI_2f_XY			0x0002000
#define RI_3f_NORMAL			0x0000100
#define RI_4f_COLOR			0x0000010
// кол-во текстур
#define RI_1_TEX			0x0000001
#define RI_2_TEX			0x0000002
#define RI_3_TEX			0x0000003
#define RI_4_TEX			0x0000004
#define RI_5_TEX			0x0000005
#define RI_6_TEX			0x0000006
#define RI_7_TEX			0x0000007
#define RI_8_TEX			0x0000008
// размер данных на каждую текстуру
#define RI_1f_TEX			0x0100000
#define RI_2f_TEX			0x0200000 // по умолчанию
#define RI_3f_TEX			0x0300000
#define RI_4f_TEX			0x0400000
// тип работы с координатами текстуры
#define RI_SEPARATE_TEX_COORD		0x0000000
#define RI_DUBLICATE_TEX_COORD		0x1000000

// Cull Face
#define RI_NONE				0x10C1
#define RI_BACK				0x10C2
#define RI_FRONT			0x10C3


/*
 * Common functions.
 */

// Initialization renderer.
int vw_InitWindow(const char *Title, int Width, int Height, int *Bits, bool FullScreenFlag,
		  int CurrentVideoModeX, int CurrentVideoModeY, int VSync);
SDL_Window *vw_GetSDL2Windows();
void vw_InitOpenGL(int Width, int Height, int *MSAA, int *CSAA);
// Shutdown renderer.
void vw_ShutdownRenderer();
// Get device capability.
const sDevCaps *vw_GetDevCaps();

void vw_SetAspectRatio(float nWidth, float nHeight, bool Value);
bool vw_GetAspectWH(float *ARWidth, float *ARHeight);

// Get rendered primitives count.
int vw_GetPrimCount();

// Set viewport data.
void vw_SetViewport(GLint x, GLint y, GLsizei width, GLsizei height,
		    GLdouble near = 0, GLdouble far = 1, eOrigin Origin = eOrigin::upper_left);
// Get viewport data.
void vw_GetViewport(int *x = nullptr, int *y = nullptr, int *width = nullptr, int *height = nullptr,
		    float *znear = nullptr, float *zfar = nullptr);
// Resize scene.
void vw_ResizeScene(float nfAngle, float AR, float nfNearClip, float nfFarClip);

/*
 * Rendering.
 */

// Begin rendering.
void vw_BeginRendering(int  mask);
// End rendering.
void vw_EndRendering();
// Clear buffer.
void vw_Clear(int mask);
// Set scene clear color.
void vw_SetClearColor(float nRed, float nGreen, float nBlue, float nAlpha);
// Set scene color mask.
void vw_SetColorMask(bool red, bool green, bool blue, bool alpha);

/*
 * Textures.
 */

// Create texture.
GLtexture vw_BuildTexture(const std::unique_ptr<uint8_t[]> &PixelsArray,
			  GLsizei Width, GLsizei Height, bool MipMap, int Bytes,
			  eTextureCompressionType CompressionType);
// Select active texture unit (starts from 0, for GL_TEXTURE0 unit).
void vw_SelectActiveTextureUnit(GLenum Unit);
// Bind texture for particular texture unit (starts from 0, for GL_TEXTURE0 unit).
void vw_BindTexture(GLenum Unit, GLtexture TextureID);
// Delete texture.
void vw_DeleteTexture(GLtexture TextureID);
// Set texture filtering mode.
void vw_SetTextureFiltering(eTextureMinFilter MinFilter, eTextureMagFilter MagFilter);
// Set texture filtering mode.
void vw_SetTextureFiltering(const sTextureFilter &Filter);
// Set texture Anisotropy Level.
void vw_SetTextureAnisotropy(GLint AnisotropyLevel);
// Set texture address mode.
void vw_SetTextureAddressMode(eTextureWrapCoord coord, eTextureWrapMode mode);
// Set texture address mode.
void vw_SetTextureAddressMode(const sTextureWrap &wrap);
// Set texture Alpha Test value that specifies a reference alpha value against which pixels are tested.
void vw_SetTextureAlphaTest(bool flag, eCompareFunc func, GLclampf ref);
// Set texture blending factor.
void vw_SetTextureBlend(bool flag, eTextureBlendFactor sfactor, eTextureBlendFactor dfactor);
// Set texture blending mode.
void vw_SetTextureBlendMode(eTextureCombinerName name, eTextureCombinerOp param);
// Set texture env mode.
void vw_SetTextureEnvMode(eTextureEnvMode mode);
// Set texture compare mode.
void vw_SetTextureCompare(eTextureCompareMode mode, eCompareFunc func);
// Set texture depth mode.
void vw_SetTextureDepthMode(eTextureDepthMode mode);

/*
 * 3D rendering.
 */

// Send (draw) vertices.
void vw_SendVertices(int PrimitiveType, int NumVertices, int DataFormat, void *VertexArray, int Stride,
		     GLuint VertexBO = 0, unsigned int RangeStart = 0, unsigned int *IndexArray = nullptr,
		     GLuint IndexBO = 0, GLuint VAO = 0);
// Set color.
void vw_SetColor(float nRed, float nGreen, float nBlue, float nAlpha);
// Set what facets can be culled.
void vw_CullFace(int face);
// Set depth buffer.
void vw_DepthTest(bool mode, eCompareFunc func);
// Set polygon offset mode.
void vw_PolygonOffset(bool enable, float factor, float units);

/*
 * Matrix.
 */

// Replace the current matrix with the identity matrix.
void vw_LoadIdentity();
// Produce a translation by sVECTOR3D.
void vw_Translate(sVECTOR3D Location);
// Produce a rotation of angle degrees around the vector x, y and z.
void vw_Rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
// Produce a nonuniform scaling along the x, y, and z axes.
void vw_Scale(GLfloat x, GLfloat y, GLfloat z);
// Push the current matrix stack.
void vw_PushMatrix();
// Pop the current matrix stack.
void vw_PopMatrix();
// Get matrix from the top of the matrix stack.
void vw_GetMatrix(eMatrixPname pname, GLfloat *params);
// Replace the current matrix with an arbitrary matrix.
void vw_SetMatrix(const GLfloat *matrix);
// Sets the current matrix mode.
void vw_MatrixMode(eMatrixMode mode);
// Multiply the current matrix by an arbitrary matrix.
void vw_MultMatrix(const GLfloat *matrix);

/*
 * Light.
 */

// Set lighting status.
void vw_Lighting(bool param);
// Enable light.
void vw_LightEnable(GLenum light, bool param);
// Set light parameter.
void vw_SetLight(GLenum light, eLightParameter pname, GLfloat param);
// Set light parameter by array.
void vw_SetLightV(GLenum light, eLightVParameter pname, const GLfloat *param);
// Set material parameter by array.
void vw_MaterialV(eMaterialParameter pname, const GLfloat *param);

/*
 * Buffer objects.
 */

// Build buffer object (size in bytes).
bool vw_BuildBufferObject(eBufferObject target, GLsizeiptr size, const GLvoid *data, GLuint &buffer);
// Bind buffer object.
void vw_BindBufferObject(eBufferObject target, GLuint buffer);
// Delete buffer object.
void vw_DeleteBufferObject(GLuint &buffer);

/*
 * Vertex array objects.
 */

// Build vertex array object.
bool vw_BuildVAO(GLuint &VAO, int NumVertices, int DataFormat, void *VertexArray, int Stride, GLuint VertexBO,
		 unsigned int RangeStart, unsigned int *IndexArray, GLuint IndexBO);
// Bind vertex array object.
void vw_BindVAO(GLuint VAO);
// Delete vertex array object.
void vw_DeleteVAO(GLuint &VAO);

/*
 * FBO.
 */

struct sFBO {
	GLrenderbuffer ColorBuffer;
	GLrenderbuffer DepthBuffer;
	GLtexture ColorTexture;
	GLtexture DepthTexture;
	GLint DepthSize;
	GLframebuffer FrameBufferObject;
	GLsizei Width;
	GLsizei Height;
};

// Build FBO. Caller should allocate mamory (FBO).
bool vw_BuildFBO(sFBO *FBO, GLsizei Width, GLsizei Height, bool NeedColor,
		 bool NeedDepth, GLsizei MSAA = 0, GLsizei *CSAA = nullptr);
// Bind FBO.
void vw_BindFBO(sFBO *FBO);
// Get current FBO (null if FrameBuffer).
sFBO *vw_GetCurrentFBO();
// Blit color part of source FBO to target FBO (need this one in order to work with multi samples).
void vw_BlitFBO(sFBO *SourceFBO, sFBO *TargetFBO);
// Draw source FBO (color texture) to target FBO (if null, to FrameBuffer).
void vw_DrawColorFBO(sFBO *SourceFBO, sFBO *TargetFBO);
// Delete FBO.
void vw_DeleteFBO(sFBO *FBO);

/*
 * GLSL.
 */

struct cGLSL;

// Create shader program.
std::weak_ptr<cGLSL> vw_CreateShader(const std::string &ShaderName,
				     const std::string &VertexShaderFileName,
				     const std::string &FragmentShaderFileName);
// Release all shaders.
void vw_ReleaseAllShaders();
// Check, is shaders Map empty.
bool vw_ShadersMapEmpty();
// Find shader by name.
std::weak_ptr<cGLSL> vw_FindShaderByName(const std::string &Name);
// Links a program object.
bool vw_LinkShaderProgram(std::weak_ptr<cGLSL> &GLSL);
// Installs a program object as part of current rendering state.
bool vw_UseShaderProgram(std::weak_ptr<cGLSL> &GLSL);
// Switch to fixed-function program object as part of current rendering state.
bool vw_StopShaderProgram();
// Returns the location of a uniform variable.
GLint vw_GetUniformLocation(std::weak_ptr<cGLSL> &GLSL, const std::string &Name);
// Specify the value of a uniform variable for the current program object.
bool vw_Uniform1i(GLint UniformLocation, int data);
// Specify the value of a uniform variable for the current program object.
bool vw_Uniform1f(GLint UniformLocation, float data);
// Specify the value of a uniform variable for the current program object.
bool vw_Uniform3f(GLint UniformLocation, float data1, float data2, float data3);

/*
 * 2D rendering.
 */

// Switch to 2D rendering mode. Origin is upper left corner.
void vw_Start2DMode(GLdouble zNear, GLdouble zFar);
// Switch back to 3D rendering mode.
void vw_End2DMode();
// Draw transparent. Origin is upper left corner.
void vw_Draw2D(const sRECT &DstRect, const sRECT &SrcRect, GLtexture Texture, bool Alpha, float Transp = 1.0f,
	       float RotateAngle = 0.0f, const sRGBCOLOR &Color = {1.0f, 1.0f, 1.0f});

/*
 * Misc.
 */

// Create screenshot from current OpenGL surface.
int vw_Screenshot(int Width, int Height, const std::string &FileName);

#endif // Graphics_H
