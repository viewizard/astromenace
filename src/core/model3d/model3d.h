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


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

#ifndef CORE_MODEL3D_MODEL3D_H
#define CORE_MODEL3D_MODEL3D_H

#include "../base.h"
#include "../math/math.h"
#include "../graphics/opengl.h"

namespace viewizard {

using bounding_box = std::array<sVECTOR3D, 8>;

// Oriented Bounding Box, coordinates are related to model's center
struct sOBB {
	bounding_box Box{};
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
};

// Hit Bounding Box, coordinates are related to model chunk's center
struct sHitBB {
	bounding_box Box{};
	sVECTOR3D Location{0.0f, 0.0f, 0.0f};
	float Radius2{0.0f}; // square of the radius of HitBB
	sVECTOR3D Size{0.0f, 0.0f, 0.0f}; // HitBB's size
};

enum class eModel3DDrawType {
	Normal,
	Blend	// with blend (for planet's sky)
};

struct sChunk3D {
	~sChunk3D();

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

	eModel3DDrawType DrawType{eModel3DDrawType::Normal};

	// initial index in global/local index array/indexbo, if we don't have
	// indexes at all, initial vertex in global/local vertex array/vertexbo
	unsigned int RangeStart{0};

	// if vbo/ibo/vao was built personally for this chunk - release this resources
	bool NeedReleaseOpenGLBuffers{false};

	// vertex quantity, that should be rendered
	unsigned int VertexQuantity{0};
	// vertex-related
	std::shared_ptr<float> VertexArray{}; // float[], make sure, that custom deleter are used
	int VertexFormat{0};
	unsigned int VertexStride{0}; // in bytes
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

struct sModel3D {
	virtual ~sModel3D() = default;

	// attached chunks
	std::vector<sChunk3D> Chunks{};

	// vertex-related
	std::shared_ptr<float> GlobalVertexArray{}; // float[], make sure, that custom deleter are used
	unsigned int GlobalVertexArrayCount{0}; // vertex quantity in GlobalVertexArray
	GLuint GlobalVBO{0};
	// index-related
	std::shared_ptr<unsigned> GlobalIndexArray{}; // unsigned[], make sure, that custom deleter are used
	unsigned int GlobalIndexArrayCount{0}; // GlobalIndexArray's size
	GLuint GlobalIBO{0};
	// vao-related
	GLuint GlobalVAO{0};

	// Axis-Aligned Bounding Box, coordinates are related to model's location
	bounding_box AABB{};
	// Oriented Bounding Box, coordinates are related to model's center
	sOBB OBB{};
	// Hit Bounding Box, same as OBB, but for each model chunk's in the 3D model
	std::vector<sHitBB> HitBB{};
	// geometry center of all vertices in the model, related to model's location
	sVECTOR3D GeometryCenter{0.0f, 0.0f, 0.0f};
	float Radius{0.0f}; // Radius, for fast collisions check
	float Width{1.0f}; // calculated from 3D model actual size
	float Length{1.0f}; // calculated from 3D model actual size
	float Height{1.0f}; // calculated from 3D model actual size

	// 3D model's metadata initialization (AABB, OBB, HitBB, size, etc).
	void MetadataInitialization();
};

// Load 3D model.
// Note, we don't provide shared_ptr, only weak_ptr, since all memory management
// should be internal only. Caller should operate with weak_ptr and use lock()
// (shared_ptr) only during access to model's data.
// Note, FileName used as a key in ModelsMap, and should not be used with different
// TriangleSizeLimit or NeedTangentAndBinormal.
std::weak_ptr<sModel3D> vw_LoadModel3D(const std::string &FileName, float TriangleSizeLimit, bool NeedTangentAndBinormal);
// Release all 3D models.
void vw_ReleaseAllModel3D();

} // viewizard namespace

#endif // CORE_MODEL3D_MODEL3D_H
