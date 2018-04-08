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

#ifndef MODEL3D_H
#define MODEL3D_H

#include "../base.h"

struct sVECTOR3D;

enum class ObjectDrawType {
	Normal,
	Blend	// with blend (for planet's sky)
};

struct sObjectBlock {
	~sObjectBlock();

	// GLSL-related
	int ShaderType{1};
	float ShaderData[16]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	sVECTOR3D Rotation{0.0f, 0.0f, 0.0f};

	// animation (rotation)
	bool NeedGeometryAnimation{false};
	sVECTOR3D GeometryAnimation{0.0f, 0.0f, 0.0f};

	// animation (tile animation)
	bool NeedTextureAnimation{false};
	sVECTOR3D TextureAnimation{0.0f, 0.0f, 0.0f};

	ObjectDrawType DrawType{ObjectDrawType::Normal};

	// initial index in global/local index array/indexbo, if we don't have
	// indexes at all, initial vertex in global/local vertex array/vertexbo
	unsigned int RangeStart{0};

	// if we allocate memory, bind vbo/ibo/vao personally for this object - release/delete this resources
	bool NeedDestroyDataInObjectBlock{false};

	// vertex-related
	float *VertexArray{nullptr};
	int VertexFormat{0};
	unsigned int VertexStride{0};	// in bytes
	unsigned int VertexCount{0};
	unsigned int VBO{0};

	// index-related
	unsigned int *IndexArray{nullptr};
	unsigned int IBO{0};

	// vao
	unsigned int VAO{0};

	// for explosion we need pre-generated vertex array with small triangles,
	// in this case, we could create cool looking effects, when enemies disintegrate
	// into 'dust' pieces during explosion
	float *VertexArrayWithSmallTriangles{nullptr};
	unsigned int VertexArrayWithSmallTrianglesCount{0};
};

struct sModel3D {
	virtual ~sModel3D();

	// Load VW3D 3D models format.
	bool LoadVW3D(const std::string &FileName);
	// Save VW3D 3D models format.
	bool SaveVW3D(const std::string &FileName);

	// attached objects
	sObjectBlock *ObjectsList{nullptr};
	unsigned int ObjectsListCount{0};

	// vertex-related
	float *GlobalVertexArray{nullptr};
	unsigned int GlobalVertexArrayCount{0};
	unsigned int GlobalVBO{0};

	// index-related
	unsigned int *GlobalIndexArray{nullptr};
	unsigned int GlobalIndexArrayCount{0};
	unsigned int GlobalIBO{0};

	// vao
	unsigned int GlobalVAO{0};
};


// Load 3D model.
sModel3D *vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal);
// Release all 3D models.
void vw_ReleaseAllModel3D();

#endif // MODEL3D_H
