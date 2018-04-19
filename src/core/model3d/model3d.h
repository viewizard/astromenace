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
#include "../graphics/opengl.h"

struct sVECTOR3D;

enum class ObjectDrawType {
	Normal,
	Blend	// with blend (for planet's sky)
};

struct sObjectBlock {
	~sObjectBlock();

	// GLSL-related
	int ShaderType{1};
	std::array<float, 16> ShaderData{};

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
	std::shared_ptr<float> VertexArray{}; // float[], make sure, that custom deleter are used
	int VertexFormat{0};
	unsigned int VertexStride{0};	// in bytes
	unsigned int VertexCount{0};
	GLuint VBO{0};
	// index-related
	std::shared_ptr<unsigned> IndexArray{}; // unsigned[], make sure, that custom deleter are used
	GLuint IBO{0};
	// vao-related
	GLuint VAO{0};

	// for explosion we need pre-generated vertex array with small triangles,
	// in this case, we could create cool looking effects, when enemies disintegrate
	// into 'dust' pieces during explosion
	std::shared_ptr<float> VertexArrayWithSmallTriangles{}; // float[], make sure, that custom deleter are used
	unsigned int VertexArrayWithSmallTrianglesCount{0};
};

class cModel3D {
	friend std::weak_ptr<cModel3D> vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit,
						      bool NeedTangentAndBinormal);

public:
	// Load VW3D 3D models format.
	bool LoadVW3D(const std::string &FileName);
	// Save VW3D 3D models format.
	bool SaveVW3D(const std::string &FileName);

	// attached objects
	std::vector<sObjectBlock> ObjectBlocks{};

	// vertex-related
	std::shared_ptr<float> GlobalVertexArray{}; // float[], make sure, that custom deleter are used
	unsigned int GlobalVertexArrayCount{0};
	GLuint GlobalVBO{0};
	// index-related
	std::shared_ptr<unsigned> GlobalIndexArray{}; // unsigned[], make sure, that custom deleter are used
	unsigned int GlobalIndexArrayCount{0};
	GLuint GlobalIBO{0};
	// vao-related
	GLuint GlobalVAO{0};

private:
	// Don't allow direct new/delete usage in code, only vw_LoadModel3D()
	// allowed for Model3D creation and release setup (deleter must be provided).
	cModel3D() = default;
	~cModel3D();
};


// Load 3D model.
std::weak_ptr<cModel3D> vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal);
// Release all 3D models.
void vw_ReleaseAllModel3D();

#endif // MODEL3D_H
