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


#ifndef RendererInterface_H
#define RendererInterface_H


#include "../Base.h"
#include "../Math/Math.h"
#include "../Texture/Texture.h"



struct eCoverageModes
{
	int ColorSamples;
	int CoverageSamples;
};


struct eDevCaps
{
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
	eCoverageModes MultisampleCoverageModes[32]; // собственно сам список режимов CSAA+MSAA, ставим просто 32 штуки, чтобы не заморачиваться с освобождением памяти
	// есть ли возможность включить сжатие текстур
	bool TexturesCompression;
	// GL_ARB_texture_storage
	bool TextureStorage;
	// поддержка FBO
	bool FramebufferObject;
	// глубина depth буфера в битах, получаем ее при первой генерации fbo с буфером глубины, по умолчанию 0
	// если работаем с fbo, то еще на этапе инициализации основного fbo прорисовки будут получены данные максимально поддерживаемой глубины
	int FramebufferObjectDepthSize;
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
	// поддержка Occlusion Query (попадает в видимую область прорисовки)
	bool OcclusionQuerySupported;
	// поддержка генерации мипмеп в железе
	bool HardwareMipMapGeneration;
};










// Point 0-0 in the upper left corner
#define RI_UL_CORNER						0x1011
// Point 0-0 in the bottom left corner
#define RI_BL_CORNER						0x1012

// Buffer clear bit
#define RI_COLOR_BUFFER		0x1000
#define RI_DEPTH_BUFFER		0x0100
#define RI_ACCUM_BUFFER		0x0010
#define RI_STENCIL_BUFFER	0x0001

// Texture filtering modes
#define RI_MAGFILTER_POINT					0x103000
#define RI_MAGFILTER_LINEAR					0x103100
#define RI_MINFILTER_POINT					0x103000
#define RI_MINFILTER_LINEAR					0x103010
#define RI_MIPFILTER_NONE					0x103000
#define RI_MIPFILTER_POINT					0x103001
#define RI_MIPFILTER_LINEAR					0x103002
// Basic filters
#define RI_TEXTURE_NONE			RI_MAGFILTER_POINT | RI_MINFILTER_POINT | RI_MIPFILTER_NONE
#define RI_TEXTURE_BILINEAR		RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_POINT
#define RI_TEXTURE_TRILINEAR	RI_MAGFILTER_LINEAR | RI_MINFILTER_LINEAR | RI_MIPFILTER_LINEAR

// Texture address modes
#define RI_WRAP_U							0x10410
#define RI_WRAP_V							0x10401
#define RI_CLAMP_TO_EDGE					0x10400

// Texture blending factors
// Blend factor is (0, 0, 0, 0).
#define RI_BLEND_ZERO						0x1070
// Blend factor is (1, 1, 1, 1).
#define RI_BLEND_ONE						0x1071
// Blend factor is (Rs, Gs, Bs, As).
#define RI_BLEND_SRCCOLOR					0x1072
// Blend factor is (1–Rs, 1–Gs, 1–Bs, 1–As).
#define RI_BLEND_INVSRCCOLOR				0x1073
// Blend factor is (As, As, As, As).
#define RI_BLEND_SRCALPHA					0x1074
// Blend factor is (1–As, 1–As, 1–As, 1–As).
#define RI_BLEND_INVSRCALPHA				0x1075
// Blend factor is (Ad, Ad, Ad, Ad).
#define RI_BLEND_DESTALPHA					0x1076
// Blend factor is (1–Ad, 1–Ad, 1–Ad, 1–Ad).
#define RI_BLEND_INVDESTALPHA				0x1077
// Blend factor is (Rd, Gd, Bd, Ad).
#define RI_BLEND_DESTCOLOR					0x1078
// Blend factor is (1–Rd, 1–Gd, 1–Bd, 1–Ad).
#define RI_BLEND_INVDESTCOLOR				0x1079
// Blend factor is (f, f, f, 1); f = min(As, 1–Ad).
#define RI_BLEND_SRCALPHASAT				0x107A

// Texture blending modes
// Parametre name
#define RI_TBLEND_COLOROP					0x103051
#define RI_TBLEND_ALPHAOP					0x103052
#define RI_TBLEND_ALPHAARG1					0x103053
#define RI_TBLEND_ALPHAARG2					0x103054
#define RI_TBLEND_ALPHAARG3					0x103055
#define RI_TBLEND_COLORARG1					0x103056
#define RI_TBLEND_COLORARG2					0x103057
#define RI_TBLEND_COLORARG3					0x103058
// Parametre
#define RI_TBLEND_SOURCE1					0x103061
#define RI_TBLEND_SOURCE2					0x103062
#define RI_TBLEND_MODULATE					0x103063
#define RI_TBLEND_MODULATE2X				0x103064
#define RI_TBLEND_MODULATE4X				0x103065
#define RI_TBLEND_ADD						0x103066
#define RI_TBLEND_ADDSMOOTH					0x103067
#define RI_TBLEND_ADD_SIGNED				0x103068
#define RI_TBLEND_SUBTRACT					0x103069
#define RI_TBLEND_DIFFUSE_ALPHA				0x103070
#define RI_TBLEND_TEXTURE_ALPHA				0x103071
#define RI_TBLEND_CURRENT_ALPHA				0x103072
#define RI_TBLEND_DOTPRODUCT				0x103073
#define RI_TBLEND_CURRENT					0x103080
#define RI_TBLEND_TEXTURE					0x103081
#define RI_TBLEND_CONSTANT					0x103082
#define RI_TBLEND_DIFFUSE					0x103083
#define RI_TBLEND_SPECULAR					0x103084
// vw_SetTextureEnvMode GL_TEXTURE_ENV_MODE
#define RI_TENV_DECAL		1
#define RI_TENV_BLEND		2
#define RI_TENV_REPLACE		3
#define RI_TENV_ADD			4
#define RI_TENV_MODULATE	5
#define RI_TENV_COMBINE		6

// vw_SetTextureCompare MODE
#define RI_COMPARE_R_TO_TEXTURE		1
#define RI_COMPARE_NONE				2

// vw_SetTextureDepthMode MODE
#define RI_DEPTH_TEXTURE_MODE_LUMINANCE		1
#define RI_DEPTH_TEXTURE_MODE_INTENSITY		2
#define RI_DEPTH_TEXTURE_MODE_ALPHA			3


// Primitives types
#define RI_POINTS							0x1020
#define RI_LINES							0x1021
#define RI_TRIANGLES						0x1022
#define RI_TRIANGLE_STRIP					0x1023
#define RI_TRIANGLE_FAN						0x1024
#define RI_QUADS							0x1025
// Data format
#define RI_3f_XYZ							0x0001000
#define RI_2f_XY							0x0002000
#define RI_3s_XYZ							0x0003000
#define RI_2s_XY							0x0004000
#define RI_3i_XYZ							0x0005000
#define RI_3f_NORMAL						0x0000100
#define RI_4f_COLOR							0x0000010
#define RI_4ub_COLOR						0x0000020
// кол-во текстур
#define RI_1_TEX							0x0000001
#define RI_2_TEX							0x0000002
#define RI_3_TEX							0x0000003
#define RI_4_TEX							0x0000004
#define RI_5_TEX							0x0000005
#define RI_6_TEX							0x0000006
#define RI_7_TEX							0x0000007
#define RI_8_TEX							0x0000008
// размер данных на каждую текстуру
#define RI_1f_TEX							0x0100000
#define RI_2f_TEX							0x0200000 // по умолчанию
#define RI_3f_TEX							0x0300000
#define RI_4f_TEX							0x0400000
#define RI_1s_TEX							0x0500000
#define RI_2s_TEX							0x0600000
#define RI_3s_TEX							0x0700000
#define RI_4s_TEX							0x0800000
// тип работы с координатами текстуры
#define RI_SEPARATE_TEX_COORD				0x0000000
#define RI_DUBLICATE_TEX_COORD				0x1000000

// Polygon Mode
#define RI_POINT							0x10B1
#define RI_LINE								0x10B2
#define RI_FILL								0x10B3

// Cull Face
#define RI_NONE								0x10C1
#define RI_BACK								0x10C2
#define RI_FRONT							0x10C3

// Set depth buffer status and texture compare function
#define RI_NEVER          		1
#define RI_LESS                 2
#define RI_EQUAL                3
#define RI_LESSEQUAL            4
#define RI_GREATER              5
#define RI_NOTEQUAL             6
#define RI_GREATEREQUAL         7
#define RI_ALWAYS               8

// Matrix types
#define RI_PROJECTION_MATRIX				0x1080
#define RI_MODELVIEW_MATRIX					0x1081
#define RI_TEXTURE_MATRIX					0x1082

// Light parametres
#define RI_SPOT_EXPONENT					0x1090
#define RI_SPOT_CUTOFF						0x1091
#define RI_CONSTANT_ATTENUATION				0x1092
#define RI_LINEAR_ATTENUATION				0x1093
#define RI_QUADRATIC_ATTENUATION			0x1094
#define RI_AMBIENT							0x1095
#define RI_DIFFUSE							0x1096
#define RI_SPECULAR							0x1097
#define RI_POSITION							0x1098
#define RI_DIRECTION						0x1099
#define RI_EMISSION							0x109A
#define RI_SHININESS						0x109B

// VBO
#define RI_ARRAY_BUFFER						1
#define RI_ELEMENT_ARRAY_BUFFER				2










// Common functions

// Initialization renderer
int vw_InitWindow(const char* Title, int Width, int Height, int *Bits, BOOL FullScreenFlag, int CurrentVideoModeX, int CurrentVideoModeY, int CurrentVideoModeW, int CurrentVideoModeH, int VSync);
void vw_InitOpenGL(int Width, int Height, int *MSAA, int *CSAA);
// Shutdown renderer dll
void vw_ShutdownRenderer(void);
// Get device capability
eDevCaps *vw_GetDevCaps();

void vw_SetAspectRatio(float nWidth, float nHeight, bool Value);
bool vw_GetAspectWH(float *ARWidth, float *ARHeight);





// Set viewport data
void vw_SetViewport(int x, int y, int width, int height, float znear, float zfar, int Corner = RI_UL_CORNER);
// Get viewport data
void vw_GetViewport(int *x=0, int *y=0, int *width=0, int *height=0, float *znear=0, float *zfar=0);
// Resize scene
void vw_ResizeScene(float nfAngle, float AR, float nfNearClip, float nfFarClip);
// Function for window WM_SIZE message only
void vw_OnChangeSize(int nWidth, int nHeight);





// Basic rendering functions

// Begin rendering
void vw_BeginRendering(int  mask);
// End rendering
void vw_EndRendering(void);
// Clear buffer
void vw_Clear(int mask);
// Start 2D rendering mode
void vw_Start2DMode(float nZ1, float nZ2);
// End 2D rendering mode
void vw_End2DMode(void);
// Set scene clear color
void vw_SetClearColor(float nRed, float nGreen, float nBlue, float nAlpha);
// Set scene color mask
void vw_SetColorMask(bool red, bool green, bool blue, bool alpha);





// Texture functions

// Create texture
GLuint vw_BuildTexture(BYTE *ustDIB, int Width, int Height, bool MipMap, int Bytes, bool NeedCompression);
// Bind texture
void vw_BindTexture(DWORD Stage, GLuint TextureID);
// Delete texture
void vw_DeleteTexture(GLuint TextureID);
// Set texture filtering mode
void vw_SetTextureFiltering(int nFiltering);
// Set texture Anisotropy Level
void vw_SetTextureAnisotropy(int AnisotropyLevel);
// Set texture address mode
void vw_SetTextureAddressMode(int nAddressMode);
// Set texture Alpha Test value that specifies a reference alpha value against which pixels are tested
void vw_SetTextureAlphaTest(bool Flag, float Value);
// Set texture blending factor
void vw_SetTextureBlend(bool Flag, int Src, int Dst);
// Set texture blending mode
void vw_SetTextureBlendMode(int pname, int param);
// Set texture env mode
void vw_SetTextureEnvMode(int param);
// Set texture compare mode
void vw_SetTextureCompare(int MODE, int FUNC);
// Set texture depth mode
void vw_SetTextureDepthMode(int MODE);
// Set texture by pointer
void vw_SetTexture(DWORD Stage, eTexture *Texture);
// Get texture image bitmap (RGBA) by void pointer
void vw_GetTextureImage(eTexture *Texture, void *bits, int BPP);
// Set texture priority
void vw_SetPrioritizeTextures(GLuint TextureID, float Prior);
// Get texture priority
void vw_GetPrioritizeTextures(GLuint TextureID, float *Prior);









// 3D rendering functions

// Send (draw) vertices
void vw_SendVertices(int PrimitiveType, int NumVertices, int DataFormat, void *Data, int Stride, unsigned int *VBO=0,
						unsigned int RangeStart=0,	unsigned int *DataIndex=0, unsigned int *DataIndexVBO=0, unsigned int *VAO=0);
// Set color
void vw_SetColor(float nRed, float nGreen, float nBlue, float nAlpha);
// Set polygon rasterization mode
void vw_PolygonMode(int mode);
// Set what facets can be culled
void vw_CullFace(int face);
// Set depth buffer
void vw_DepthTest(bool mode, int funct);
// Set polygon offset mode
void vw_PolygonOffset(bool enable, float factor, float units);

// Loads identity in the current matrix
void vw_LoadIdentity(void);
// Determines the product of the current matrix and the computed translation matrix determined by the given factors
void vw_Translate(VECTOR3D Location);
// Determines the product of the current matrix and the computed rotation matrix
void vw_Rotate(float fAngle, float fX, float fY, float fZ);
// Determines the product of the current matrix and the computed scale matrix composed from the given point
void vw_Scale(float fX, float fY, float fZ);
// Adds a matrix to the stack
void vw_PushMatrix(void);
// Removes the current matrix from the top of the stack
void vw_PopMatrix(void);
// Get matrix
void vw_GetMatrix(int pname, float *params);
void vw_SetMatrix(float *params);
// Set Matrix Mode
void vw_MatrixMode(int pname);
// Mult matrix
void vw_MultMatrix(float *params);





// Light

// Set lighting status
void vw_Lighting(bool param);
// Get lighting status
bool vw_GetLighting(void);
// Enable light
void vw_LightEnable(int light, bool param);
// Get light status
bool vw_GetLightEnable(int light);
// Set light parametre by float
void vw_SetLight(int light, int pname, float param);
// Set light parametre by vector
void vw_SetLightV(int light, int pname, const float *param);
// Get light parametre by vector
void vw_GetLightV(int light, int pname, float *param);
// Set matirial parametre by vector
void vw_MaterialV(int pname, const float *param);
// Get matirial parametre by vector
void vw_GetMaterialV(int pname, float *param);







// VBO+IBO

// создаем буфер
bool vw_BuildVBO(int NumVertices, void *Data, int Stride, unsigned int *VBO);
bool vw_BuildIBO(int NumIndex, void *Data, unsigned int *IBO);
// устанавливаем буфер
void vw_BindVBO(int target, unsigned int VBO);
// удаляем буфер
void vw_DeleteVBO(unsigned int VBO);







// VAO

// создаем
bool vw_BuildVAO(unsigned int *VAO, int NumVertices, int DataFormat, void *Data, int Stride, unsigned int *VBO,
						unsigned int RangeStart, unsigned int *DataIndex, unsigned int *DataIndexVBO);
// устанавливаем
void vw_BindVAO(unsigned int VAO);
// удаляем
void vw_DeleteVAO(unsigned int VAO);







// FBO

// структура FBO
struct eFBO
{
	unsigned int 	ColorBuffer;
	unsigned int 	DepthBuffer;
	unsigned int 	ColorTexture;
	unsigned int 	DepthTexture;
	int				DepthSize;
	unsigned int 	FrameBufferObject;
	unsigned int 	Width;
	unsigned int 	Height;
};

// создаем (FBO - уже заранее подготовленный объект, в функции память не выделяем)
bool vw_BuildFBO(eFBO *FBO, int Width, int Height, bool NeedColor, bool NeedDepth, int MSAA=0, int *CSAA=0);
// устанавливаем
void vw_BindFBO(eFBO *FBO);
// получаем текущий установленный FBO, 0 - если фрейм буфер
eFBO *vw_GetCurrentFBO();
// блит цветовой составляющей фбо источника в фбо приемник (нужно для вывода фбо с мультисемплами)
void vw_BlitFBO(eFBO *SourceFBO, eFBO *TargetFBO);
// рисуем цветовую текстуру источника, на весь приемник, если 0 - во фрейм буфер
void vw_DrawColorFBO(eFBO *SourceFBO, eFBO *TargetFBO);
// удаление данных в структуре
void vw_DeleteFBO(eFBO *FBO);







// шейдеры GLSL

// структура шейдера
struct eGLSL
{
	// хендлы
	GLhandleARB Program;
	GLhandleARB VertexShader;
	bool		VertexShaderUse;
	GLhandleARB FragmentShader;
	bool		FragmentShaderUse;

	// для менеждера
	char*	Name;
	eGLSL*	Prev;
	eGLSL*	Next;
	int		Num;
};

void vw_ReleaseAllShaders();
void vw_AttachShader(eGLSL* GLSL);
void vw_DetachShader(eGLSL* GLSL);
eGLSL* vw_FindShaderByNum(int Num);
eGLSL* vw_FindShaderByName(const char *Name);

void vw_ReleaseShader(eGLSL *GLSL);

eGLSL *vw_CreateShader(const char *ShaderName, const char *VertexShaderFileName, const char *FragmentShaderFileName);
bool vw_LinkShaderProgram(eGLSL *GLSL);
bool vw_UseShaderProgram(eGLSL *GLSL);
bool vw_StopShaderProgram();

int vw_GetUniformLocation(eGLSL *GLSL, const char *name);

bool vw_Uniform1i(eGLSL *GLSL, int UniformLocation, int data);
bool vw_Uniform1i(eGLSL *GLSL, const char *name, int data);
bool vw_Uniform1fv(eGLSL *GLSL, int UniformLocation, int count, float *data);
bool vw_Uniform1fv(eGLSL *GLSL, const char *name, int count, float *data);
bool vw_Uniform1f(eGLSL *GLSL, int UniformLocation, float data);
bool vw_Uniform1f(eGLSL *GLSL, const char *name, float data);
bool vw_Uniform3f(eGLSL *GLSL, int UniformLocation, float data1, float data2, float data3);
bool vw_Uniform3f(eGLSL *GLSL, const char *name, float data1, float data2, float data3);
bool vw_Uniform4fv(eGLSL *GLSL, int UniformLocation, int count, float *data);
bool vw_Uniform4fv(eGLSL *GLSL, const char *name, int count, float *data);
bool vw_UniformMatrix4fv(eGLSL *GLSL, int UniformLocation, bool transpose, int count, float *data);
bool vw_UniformMatrix4fv(eGLSL *GLSL, const char *name, bool transpose, int count, float *data);





// Occlusion Query
void vw_GenQueries(int n, unsigned int *ids);
void vw_DeleteQueries(int n, const unsigned int *ids);
bool vw_IsQuery(unsigned int id);
void vw_BeginQuery(unsigned int id);
void vw_EndQuery();
void vw_GetQueryiv(unsigned int target, unsigned int pname, int *params);
void vw_GetQueryObjectiv(unsigned int id, unsigned int pname, int *params);
void vw_GetQueryObjectuiv(unsigned int id, unsigned int pname, unsigned int *params);







// Get rendered primitives count
int vw_GetPrimCount(void);

// Set gamma ramp
void vw_SetGammaRamp(float Gamma, float Contrast, float Brightness);
// Get gamma ramp
void vw_GetGammaRamp(float *Gamma, float *Contrast, float *Brightness);








// 2D rendering functions

// Draw
void vw_Draw(int X, int Y, RECT *SrcRect, eTexture *Tex, bool Alpha, float RotateAngle=0, int DrawCorner = RI_UL_CORNER);
// Draw transparent
void vw_DrawTransparent(RECT *DstRect, RECT *SrcRect, eTexture *Tex, bool Alpha, float Transp, float RotateAngle=0, int DrawCorner = RI_UL_CORNER, float R=1.0f, float G=1.0f, float B=1.0f);






// Дополнительные функции

// Создание скриншота
int vw_Screenshot(SDL_Surface *screen, char *filename);











#endif // RendererInterface_H
