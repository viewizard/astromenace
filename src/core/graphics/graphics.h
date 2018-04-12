/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
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
	int MaxAnisotropyLevel;
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

// Texture filtering modes
#define RI_MAGFILTER		0x103F00
#define RI_MAGFILTER_POINT	0x103100
#define RI_MAGFILTER_LINEAR	0x103200
#define RI_MINFILTER		0x1030F0
#define RI_MINFILTER_POINT	0x103010
#define RI_MINFILTER_LINEAR	0x103020
#define RI_MIPFILTER		0x10300F
#define RI_MIPFILTER_NONE	0x103001
#define RI_MIPFILTER_POINT	0x103002
#define RI_MIPFILTER_LINEAR	0x103004
// Basic filters
#define RI_TEXTURE_NONE		RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_NONE
#define RI_TEXTURE_BILINEAR	RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_POINT
#define RI_TEXTURE_TRILINEAR	RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_LINEAR

// Texture address modes
#define RI_WRAP_U		0x10410
#define RI_WRAP_V		0x10401
#define RI_CLAMP_TO_EDGE	0x10400

// Texture blending factors
// Blend factor is (0, 0, 0, 0).
#define RI_BLEND_ZERO		0x1070
// Blend factor is (1, 1, 1, 1).
#define RI_BLEND_ONE		0x1071
// Blend factor is (Rs, Gs, Bs, As).
#define RI_BLEND_SRCCOLOR	0x1072
// Blend factor is (1–Rs, 1–Gs, 1–Bs, 1–As).
#define RI_BLEND_INVSRCCOLOR	0x1073
// Blend factor is (As, As, As, As).
#define RI_BLEND_SRCALPHA	0x1074
// Blend factor is (1–As, 1–As, 1–As, 1–As).
#define RI_BLEND_INVSRCALPHA	0x1075
// Blend factor is (Ad, Ad, Ad, Ad).
#define RI_BLEND_DESTALPHA	0x1076
// Blend factor is (1–Ad, 1–Ad, 1–Ad, 1–Ad).
#define RI_BLEND_INVDESTALPHA	0x1077
// Blend factor is (Rd, Gd, Bd, Ad).
#define RI_BLEND_DESTCOLOR	0x1078
// Blend factor is (1–Rd, 1–Gd, 1–Bd, 1–Ad).
#define RI_BLEND_INVDESTCOLOR	0x1079
// Blend factor is (f, f, f, 1); f = min(As, 1–Ad).
#define RI_BLEND_SRCALPHASAT	0x107A

// Texture blending modes
// Parameter name
#define RI_TBLEND_COLOROP		0x103051
#define RI_TBLEND_ALPHAOP		0x103052
#define RI_TBLEND_ALPHAARG1		0x103053
#define RI_TBLEND_ALPHAARG2		0x103054
#define RI_TBLEND_ALPHAARG3		0x103055
#define RI_TBLEND_COLORARG1		0x103056
#define RI_TBLEND_COLORARG2		0x103057
#define RI_TBLEND_COLORARG3		0x103058
// Parameter
#define RI_TBLEND_SOURCE1		0x103061
#define RI_TBLEND_SOURCE2		0x103062
#define RI_TBLEND_MODULATE		0x103063
#define RI_TBLEND_MODULATE2X		0x103064
#define RI_TBLEND_MODULATE4X		0x103065
#define RI_TBLEND_ADD			0x103066
#define RI_TBLEND_ADDSMOOTH		0x103067
#define RI_TBLEND_ADD_SIGNED		0x103068
#define RI_TBLEND_SUBTRACT		0x103069
#define RI_TBLEND_DIFFUSE_ALPHA		0x103070
#define RI_TBLEND_TEXTURE_ALPHA		0x103071
#define RI_TBLEND_CURRENT_ALPHA		0x103072
#define RI_TBLEND_DOTPRODUCT		0x103073
#define RI_TBLEND_CURRENT		0x103080
#define RI_TBLEND_TEXTURE		0x103081
#define RI_TBLEND_CONSTANT		0x103082
#define RI_TBLEND_DIFFUSE		0x103083
#define RI_TBLEND_SPECULAR		0x103084

// vw_SetTextureEnvMode() MODE
#define RI_TENV_DECAL			1
#define RI_TENV_BLEND			2
#define RI_TENV_REPLACE			3
#define RI_TENV_ADD			4
#define RI_TENV_MODULATE		5
#define RI_TENV_COMBINE			6

// vw_SetTextureCompare() MODE
#define RI_COMPARE_R_TO_TEXTURE		1
#define RI_COMPARE_NONE			2

// vw_SetTextureDepthMode() MODE
#define RI_DEPTH_TEXTURE_MODE_LUMINANCE	1
#define RI_DEPTH_TEXTURE_MODE_INTENSITY	2
#define RI_DEPTH_TEXTURE_MODE_ALPHA	3


// Primitives types
#define RI_POINTS			0x1020
#define RI_LINES			0x1021
#define RI_TRIANGLES			0x1022
#define RI_TRIANGLE_STRIP		0x1023
#define RI_TRIANGLE_FAN			0x1024
#define RI_QUADS			0x1025
// Data format
#define RI_3f_XYZ			0x0001000
#define RI_2f_XY			0x0002000
#define RI_3s_XYZ			0x0003000
#define RI_2s_XY			0x0004000
#define RI_3i_XYZ			0x0005000
#define RI_3f_NORMAL			0x0000100
#define RI_4f_COLOR			0x0000010
#define RI_4ub_COLOR			0x0000020
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
#define RI_1s_TEX			0x0500000
#define RI_2s_TEX			0x0600000
#define RI_3s_TEX			0x0700000
#define RI_4s_TEX			0x0800000
// тип работы с координатами текстуры
#define RI_SEPARATE_TEX_COORD		0x0000000
#define RI_DUBLICATE_TEX_COORD		0x1000000

// Polygon Mode
#define RI_POINT			0x10B1
#define RI_LINE				0x10B2
#define RI_FILL				0x10B3

// Cull Face
#define RI_NONE				0x10C1
#define RI_BACK				0x10C2
#define RI_FRONT			0x10C3

// Set depth buffer status and texture compare function
#define RI_NEVER			1
#define RI_LESS				2
#define RI_EQUAL			3
#define RI_LESSEQUAL			4
#define RI_GREATER			5
#define RI_NOTEQUAL			6
#define RI_GREATEREQUAL			7
#define RI_ALWAYS			8

// Matrix types
#define RI_PROJECTION_MATRIX		0x1080
#define RI_MODELVIEW_MATRIX		0x1081
#define RI_TEXTURE_MATRIX		0x1082

// VBO
#define RI_ARRAY_BUFFER			1
#define RI_ELEMENT_ARRAY_BUFFER		2


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

// Set gamma ramp.
void vw_SetGammaRamp(float Gamma, float Contrast, float Brightness);
// Get gamma ramp.
void vw_GetGammaRamp(float *Gamma, float *Contrast, float *Brightness);

// Set viewport data.
void vw_SetViewport(GLint x, GLint y, GLsizei width, GLsizei height,
		    GLdouble near = 0, GLdouble far = 1, eOrigin Origin = eOrigin::upper_left);
// Get viewport data.
void vw_GetViewport(int *x = nullptr, int *y = nullptr, int *width = nullptr, int *height = nullptr,
		    float *znear = nullptr, float *zfar = nullptr);
// Resize scene.
void vw_ResizeScene(float nfAngle, float AR, float nfNearClip, float nfFarClip);
// Function for window WM_SIZE message only.
void vw_OnChangeSize(int nWidth, int nHeight);

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
GLtexture vw_BuildTexture(uint8_t *ustDIB, int Width, int Height, bool MipMap, int Bytes, int CompressionType);
// Select active texture unit (starts from 0, for GL_TEXTURE0 unit).
void vw_SelectActiveTextureUnit(GLenum Unit);
// Bind texture for particular texture unit (starts from 0, for GL_TEXTURE0 unit).
void vw_BindTexture(GLenum Unit, GLtexture TextureID);
// Delete texture.
void vw_DeleteTexture(GLtexture TextureID);
// Set texture filtering mode.
void vw_SetTextureFiltering(int nFiltering);
// Set texture Anisotropy Level.
void vw_SetTextureAnisotropy(int AnisotropyLevel);
// Set texture address mode.
void vw_SetTextureAddressMode(int nAddressMode);
// Set texture Alpha Test value that specifies a reference alpha value against which pixels are tested.
void vw_SetTextureAlphaTest(bool Flag, float Value);
// Set texture blending factor.
void vw_SetTextureBlend(bool Flag, int Src, int Dst);
// Set texture blending mode.
void vw_SetTextureBlendMode(int pname, int param);
// Set texture env mode.
void vw_SetTextureEnvMode(int MODE);
// Set texture compare mode.
void vw_SetTextureCompare(int MODE, int FUNC);
// Set texture depth mode.
void vw_SetTextureDepthMode(int MODE);

/*
 * 3D rendering.
 */

// Send (draw) vertices.
void vw_SendVertices(int PrimitiveType, int NumVertices, int DataFormat, void *VertexArray, int Stride,
		     unsigned int VertexBO = 0, unsigned int RangeStart = 0, unsigned int *IndexArray = nullptr,
		     unsigned int IndexBO = 0, unsigned int VAO = 0);
// Set color.
void vw_SetColor(float nRed, float nGreen, float nBlue, float nAlpha);
// Set polygon rasterization mode.
void vw_PolygonMode(int mode);
// Set what facets can be culled.
void vw_CullFace(int face);
// Set depth buffer.
void vw_DepthTest(bool mode, int funct);
// Set polygon offset mode.
void vw_PolygonOffset(bool enable, float factor, float units);

// Loads identity in the current matrix.
void vw_LoadIdentity();
// Determines the product of the current matrix and the computed translation matrix determined by the given factors.
void vw_Translate(sVECTOR3D Location);
// Determines the product of the current matrix and the computed rotation matrix.
void vw_Rotate(float fAngle, float fX, float fY, float fZ);
// Determines the product of the current matrix and the computed scale matrix composed from the given point.
void vw_Scale(float fX, float fY, float fZ);
// Adds a matrix to the stack.
void vw_PushMatrix();
// Removes the current matrix from the top of the stack.
void vw_PopMatrix();
// Get matrix.
void vw_GetMatrix(int pname, float *params);
void vw_SetMatrix(const float *params);
// Set Matrix Mode.
void vw_MatrixMode(int pname);
// Mult matrix.
void vw_MultMatrix(float *params);

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

// Build vertex buffer object.
bool vw_BuildVertexBufferObject(int NumVertices, void *Data, int Stride, unsigned int &Buffer);
// Build index buffer object.
bool vw_BuildIndexBufferObject(int NumIndex, void *Data, unsigned int &Buffer);
// Bind buffer object.
void vw_BindBufferObject(int target, unsigned int Buffer);
// Delete buffer object.
void vw_DeleteBufferObject(unsigned int &Buffer);

/*
 * Vertex array objects.
 */

// Build vertex array object.
bool vw_BuildVAO(unsigned int &VAO, int NumVertices, int DataFormat, void *VertexArray, int Stride, unsigned int VBO,
		 unsigned int RangeStart, unsigned int *DataIndex, unsigned int DataIndexVBO);
// Bind vertex array object.
void vw_BindVAO(unsigned int VAO);
// Delete vertex array object.
void vw_DeleteVAO(unsigned int &VAO);

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

// структура шейдера
struct sGLSL {
	// хендлы
	GLhandleARB Program;
	GLhandleARB VertexShader;
	bool VertexShaderUse;
	GLhandleARB FragmentShader;
	bool FragmentShaderUse;

	// для менеждера
	char *Name;
	sGLSL *Prev;
	sGLSL *Next;
	int Num;
};

void vw_ReleaseAllShaders();
void vw_AttachShader(sGLSL *GLSL);
void vw_DetachShader(sGLSL *GLSL);
sGLSL *vw_FindShaderByNum(int Num);
sGLSL *vw_FindShaderByName(const char *Name);

void vw_ReleaseShader(sGLSL *GLSL);

sGLSL *vw_CreateShader(const char *ShaderName, const char *VertexShaderFileName, const char *FragmentShaderFileName);
bool vw_LinkShaderProgram(sGLSL *GLSL);
bool vw_UseShaderProgram(sGLSL *GLSL);
bool vw_StopShaderProgram();

int vw_GetUniformLocation(sGLSL *GLSL, const char *name);

bool vw_Uniform1i(sGLSL *GLSL, int UniformLocation, int data);
bool vw_Uniform1i(sGLSL *GLSL, const char *name, int data);
bool vw_Uniform1fv(sGLSL *GLSL, int UniformLocation, int count, float *data);
bool vw_Uniform1fv(sGLSL *GLSL, const char *name, int count, float *data);
bool vw_Uniform1f(sGLSL *GLSL, int UniformLocation, float data);
bool vw_Uniform1f(sGLSL *GLSL, const char *name, float data);
bool vw_Uniform3f(sGLSL *GLSL, int UniformLocation, float data1, float data2, float data3);
bool vw_Uniform3f(sGLSL *GLSL, const char *name, float data1, float data2, float data3);
bool vw_Uniform4fv(sGLSL *GLSL, int UniformLocation, int count, float *data);
bool vw_Uniform4fv(sGLSL *GLSL, const char *name, int count, float *data);
bool vw_UniformMatrix4fv(sGLSL *GLSL, int UniformLocation, bool transpose, int count, float *data);
bool vw_UniformMatrix4fv(sGLSL *GLSL, const char *name, bool transpose, int count, float *data);

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
