/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
	Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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

// TODO check sources, direct OpenGL usage outside 'graphics' are prohibited,
//      for 'graphics' code - switch to "vw_*" functions-wrappers for OpenGL if possible.

/*
Direct OpenGL usage outside 'graphics' are prohibited.
'graphics' code should also use "vw_*" functions-wrappers for OpenGL if possible.

Since OpenGL is evolving, more and more features allowed for new hardware.
'graphics' provide "vw_*" functions-wrappers for OpenGL, that use best solutions
for current hardware (and supported by code at this moment).
*/

#ifndef CORE_GRAPHICS_GRAPHICS_H
#define CORE_GRAPHICS_GRAPHICS_H

#include "../base.h"
#include "opengl.h"

namespace viewizard {

struct sRECT;
struct sVECTOR3D;
class cTexture;

enum class eOrigin {
	upper_left,
	bottom_left
};

enum class ePrimitiveType : GLenum {
	LINES = GL_LINES,
	TRIANGLES = GL_TRIANGLES,
	TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	TRIANGLE_FAN = GL_TRIANGLE_FAN
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
	explicit sTextureWrap(eTextureWrapMode param) :
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
	explicit sTextureFilter(eTextureBasicFilter Filter)
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

enum class eBufferObjectUsage : GLenum {
	STREAM = GL_STREAM_DRAW,	// The data store contents will be modified once and used at most a few times.
	STATIC = GL_STATIC_DRAW,	// The data store contents will be modified once and used many times.
	DYNAMIC = GL_DYNAMIC_DRAW	// The data store contents will be modified repeatedly and used many times.
};

enum class eCullFace : GLenum {
	NONE = GL_NONE,
	BACK = GL_BACK,
	FRONT = GL_FRONT,
	FRONT_AND_BACK = GL_FRONT_AND_BACK
};

struct sCoverageModes {
	int ColorSamples{0};
	int CoverageSamples{0};

	sCoverageModes() = default;
	explicit sCoverageModes(int _ColorSamples, int _CoverageSamples) :
		ColorSamples{_ColorSamples},
		CoverageSamples{_CoverageSamples}
	{}
};

enum class eRGBCOLOR {
	white,
	black,
	red,
	green,
	blue,
	yellow,
	orange
};

struct sRGBCOLOR {
	float r{0.0f};
	float g{0.0f};
	float b{0.0f};

	sRGBCOLOR() = default;
	explicit sRGBCOLOR(float _r, float _g, float _b) :
		r{_r},
		g{_g},
		b{_b}
	{}
	explicit sRGBCOLOR(eRGBCOLOR color)
	{
		switch (color) {
		case eRGBCOLOR::white:
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
			break;
		case eRGBCOLOR::black:
			r = 0.0f;
			g = 0.0f;
			b = 0.0f;
			break;
		case eRGBCOLOR::red:
			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
			break;
		case eRGBCOLOR::green:
			r = 0.0f;
			g = 1.0f;
			b = 0.0f;
			break;
		case eRGBCOLOR::blue:
			r = 0.0f;
			g = 0.0f;
			b = 1.0f;
			break;
		case eRGBCOLOR::yellow:
			r = 1.0f;
			g = 1.0f;
			b = 0.0f;
			break;
		case eRGBCOLOR::orange:
			r = 1.0f;
			g = 0.5f;
			b = 0.0f;
			break;
		}
	}
};

struct sDevCaps {
	int OpenGLmajorVersion{0};
	int OpenGLminorVersion{0};
	int MaxTextureWidth{0};
	int MaxTextureHeight{0};
	int MaxActiveLights{0};
	GLint MaxAnisotropyLevel{0}; // could be 0 (not supported by hardware/drivers)
	// MSAA + CSAA modes
	std::vector<sCoverageModes> MultisampleCoverageModes{};
	// available depth's size for FBO
	GLint FramebufferObjectDepthSize{0};

	bool OpenGL_1_3_supported{false};
	bool OpenGL_1_5_supported{false};
	bool OpenGL_2_0_supported{false};
	bool OpenGL_2_1_supported{false};
	bool OpenGL_3_0_supported{false};
	bool OpenGL_4_2_supported{false};

	bool EXT_texture_compression_s3tc{false};
	bool ARB_texture_compression_bptc{false}; // note, bptc also part of OpenGL 4.2
	bool ARB_texture_non_power_of_two{false};
	bool SGIS_generate_mipmap{false};
};

// Buffer clear bit
#define RI_COLOR_BUFFER		0x1000
#define RI_DEPTH_BUFFER		0x0100
#define RI_ACCUM_BUFFER		0x0010
#define RI_STENCIL_BUFFER	0x0001

// Data format
#define RI_COORD			0x000F000
#define RI_3f_XYZ			0x0001000
#define RI_2f_XY			0x0002000
#define RI_NORMAL			0x0000F00
#define RI_3f_NORMAL			0x0000100
#define RI_COLOR			0x00000F0
#define RI_4f_COLOR			0x0000010
#define RI_TEXTURE			0x0F00000
#define RI_2f_TEX			0x0200000
// кол-во текстур
#define RI_TEX_COUNT			0x000000F
#define RI_1_TEX			0x0000001
#define RI_2_TEX			0x0000002
#define RI_3_TEX			0x0000003
#define RI_4_TEX			0x0000004
#define RI_5_TEX			0x0000005
#define RI_6_TEX			0x0000006
#define RI_7_TEX			0x0000007
#define RI_8_TEX			0x0000008
// тип работы с координатами текстуры
#define RI_TEX_COORD_TYPE		0xF000000
#define RI_SEPARATE_TEX_COORD		0x0000000
#define RI_DUBLICATE_TEX_COORD		0x1000000


/*
 * gl_main
 */

// Create window.
bool vw_CreateWindow(const char *Title, int Width, int Height, bool Fullscreen, int DisplayIndex);
// Destroy window.
void vw_DestroyWindow();
// Get SDL window handle.
SDL_Window *vw_GetSDLWindow();
// Create OpenGL context.
bool vw_CreateOpenGLContext(int VSync);
// Delete OpenGL context.
void vw_DeleteOpenGLContext();
// Initialize (or reinitialize) and setup OpenGL related stuff.
void vw_InitOpenGLStuff(int Width, int Height, int *MSAA, int *CSAA);
// Release OpenGL related stuff.
void vw_ReleaseOpenGLStuff();
// Get device capability.
const sDevCaps &vw_DevCaps();

// Set virtual internal resolution size and status.
void vw_SetInternalResolution(float Width, float Height, bool Status);
// Get virtual internal resolution.
bool vw_GetInternalResolution(float *Width, float *Height);

// Set viewport data.
void vw_SetViewport(GLint x, GLint y, GLsizei width, GLsizei height, eOrigin Origin = eOrigin::upper_left);
// Get viewport data.
void vw_GetViewport(float *x = nullptr, float *y = nullptr, float *width = nullptr, float *height = nullptr);
// Get viewport data.
void vw_GetViewport(int *x = nullptr, int *y = nullptr, int *width = nullptr, int *height = nullptr);
// Set depth range.
void vw_DepthRange(GLdouble zNear, GLdouble zFar);
// Resize scene.
void vw_ResizeScene(float FieldOfViewAngle, float AspectRatio, float zNearClip, float zFarClip);

// Begin rendering.
void vw_BeginRendering(int  mask);
// End rendering.
void vw_EndRendering();
// Clear buffers.
void vw_Clear(int mask);
// Specify clear values for the color buffers.
void vw_SetClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
// Specifies whether the individual color components in the frame buffer can or cannot be written.
void vw_SetColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
// Set color.
void vw_SetColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
// Set what facets can be culled.
void vw_CullFace(eCullFace mode);
// Set depth buffer.
void vw_DepthTest(bool mode, eCompareFunc func);
// Set the scale and units used to calculate depth values.
void vw_PolygonOffset(bool status, GLfloat factor, GLfloat units);

/*
 * gl_texture
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
 * gl_draw3d
 */

// Draw 3D primitives.
void vw_Draw3D(ePrimitiveType mode, GLsizei count, int DataFormat, GLvoid *VertexArray,
	       GLsizei Stride, GLuint VertexBO = 0, unsigned int RangeStart = 0,
	       unsigned int *IndexArray = nullptr, GLuint IndexBO = 0, GLuint VAO = 0);

/*
 * gl_matrix
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
 * gl_light
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
 * gl_vbo
 */

// Build buffer object (size in bytes).
bool vw_BuildBufferObject(eBufferObject target, GLsizeiptr size, const GLvoid *data,
			  GLuint &buffer, eBufferObjectUsage usage = eBufferObjectUsage::STATIC);
// Bind buffer object.
void vw_BindBufferObject(eBufferObject target, GLuint buffer);
// Delete buffer object.
void vw_DeleteBufferObject(GLuint &buffer);

/*
 * gl_vao
 */

// Build vertex array object.
bool vw_BuildVAO(GLuint &VAO, int DataFormat, GLsizei Stride, GLuint VertexBO, GLuint IndexBO);
// Bind vertex array object.
void vw_BindVAO(GLuint VAO);
// Delete vertex array object.
void vw_DeleteVAO(GLuint &VAO);

/*
 * gl_fbo
 */

struct sFBO {
	friend std::shared_ptr<sFBO> vw_BuildFBO(GLsizei Width, GLsizei Height, bool NeedColor,
						 bool NeedDepth, GLsizei MSAA, GLsizei *CSAA);
public:
	GLrenderbuffer ColorBuffer{0};
	GLrenderbuffer DepthBuffer{0};
	GLtexture ColorTexture{0};
	GLtexture DepthTexture{0};
	GLint DepthSize{0};
	GLframebuffer FrameBufferObject{0};
	// we are safe with sIF_dual_type here, since Width and Height not exceed 'float'
	sIF_dual_type<GLsizei, float> Width{0};
	sIF_dual_type<GLsizei, float> Height{0};

private:
	// Don't allow direct new/delete usage in code, only vw_BuildFBO()
	// allowed for FBO creation and release setup (deleter must be provided).
	sFBO() = default;
	~sFBO();
};

// Build FBO.
std::shared_ptr<sFBO> vw_BuildFBO(GLsizei Width, GLsizei Height, bool NeedColor,
				   bool NeedDepth, GLsizei MSAA = 0, GLsizei *CSAA = nullptr);
// Bind FBO.
void vw_BindFBO(std::shared_ptr<sFBO> &FBO);
// Get current FBO (nullptr if FrameBuffer).
std::weak_ptr<sFBO> &vw_GetCurrentFBO();
// Blit color part of source FBO to target FBO (need this one in order to work with multi samples).
void vw_BlitFBO(std::shared_ptr<sFBO> &SourceFBO, std::shared_ptr<sFBO> &TargetFBO);
// Draw source FBO (color texture) to target FBO (if null, to FrameBuffer).
void vw_DrawColorFBO(std::shared_ptr<sFBO> &SourceFBO, std::shared_ptr<sFBO> &TargetFBO);

/*
 * gl_glsl
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
// Find and store uniform location in shader. Return internal storage number for future use.
int vw_FindShaderUniformLocation(std::weak_ptr<cGLSL> &GLSL, const std::string &UniformName);
// Get previously found in shader uniform's location by internal storage number.
GLint vw_GetShaderUniformLocation(std::shared_ptr<cGLSL> sharedGLSL, int UniformNumber);
// Links a program object.
bool vw_LinkShaderProgram(std::weak_ptr<cGLSL> &GLSL);
// Installs a program object as part of current rendering state.
bool vw_UseShaderProgram(std::shared_ptr<cGLSL> &sharedGLSL);
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
 * gl_draw2d
 */

// Switch to 2D rendering mode. Origin is upper left corner.
void vw_Start2DMode(GLdouble zNear, GLdouble zFar);
// Switch back to 3D rendering mode.
void vw_End2DMode();
// Draw transparent. Origin is upper left corner.
void vw_Draw2D(const sRECT &DstRect, const sRECT &SrcRect, GLtexture Texture, bool Alpha, float Transp = 1.0f,
	       float RotateAngle = 0.0f, const sRGBCOLOR &Color = sRGBCOLOR{eRGBCOLOR::white});

/*
 * misc
 */

// Create screenshot from current OpenGL surface.
int vw_Screenshot(int Width, int Height, const std::string &FileName);

} // viewizard namespace

#endif // CORE_GRAPHICS_GRAPHICS_H
