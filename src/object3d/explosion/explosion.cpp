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

#include "explosion.h"
#include "../../config/config.h"


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
cExplosion::cExplosion()
{
	ObjectStatus = eObjectStatus::Enemy;
	ObjectType = eObjectType::Explosion;

	// нужно рисовать без оптимизации
	NeedCullFaces = false;

	// нет взрыва, сразу уничтожаем
	Lifetime = 0.0f;

	// подключаем к своему списку
	AttachExplosion(this);
}



//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
cExplosion::~cExplosion()
{
	if (ExplosionPieceData != nullptr) {
		delete [] ExplosionPieceData;
		ExplosionPieceData = nullptr;
	}

	for (auto tmpGFX : GraphicFX) {
		if (auto sharedGFX = tmpGFX.lock()) {
			sharedGFX->IsSuppressed = true;
			sharedGFX->DestroyIfNoParticles = true;
		}
	}


	DetachExplosion(this);
}








//-----------------------------------------------------------------------------
// Обновление данных объектa
//-----------------------------------------------------------------------------
bool cExplosion::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!cObject3D::Update(Time)) {
		// делаем правильную остановку частиц...
		for (auto tmpGFX : GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock()) {
				sharedGFX->StopAllParticles();

				if (!(ExplosionTypeByClass == 2 && (ExplosionType == 16 || ExplosionType == 17
								    || ExplosionType == 18 || ExplosionType == 19
								    || ExplosionType == 205 || ExplosionType == 206
								    || ExplosionType == 209 || ExplosionType == 210))) {
					vw_ReleaseParticleSystem(sharedGFX);
				}
			}
		}
		return false;
	}




	// остановка испускания частиц... для взрывов
	if (ExplosionTypeByClass == 1 ||
	    (ExplosionTypeByClass == 2 && (ExplosionType == 16 || ExplosionType == 17
					   || ExplosionType == 18 || ExplosionType == 19
					   || ExplosionType == 205 || ExplosionType == 206 || ExplosionType == 209 || ExplosionType == 210
					   || ExplosionType == 214 || ExplosionType == 215 || ExplosionType == 216
					   || ExplosionType == 217)) ||
	    (ExplosionTypeByClass == 2 && (ExplosionType == -16 || ExplosionType == -17
					   || ExplosionType == -18 || ExplosionType == -19
					   || ExplosionType == -205 || ExplosionType == -206 || ExplosionType == -209 || ExplosionType == -210
					   || ExplosionType == -214 || ExplosionType == -215 || ExplosionType == -216
					   || ExplosionType == -217))) {
		for (auto tmpGFX : GraphicFX) {
			if (auto sharedGFX = tmpGFX.lock()) {
				if (Lifetime < sharedGFX->Life) {
					// только говорим, чтобы больше не создавал частиц!!!
					// не удаляем и не зануляем - иначе не сможем им управлять
					sharedGFX->IsSuppressed = true;
				}
			}
		}
	}

	// работа с эффектами
	if (ExplosionTypeByClass == 2) {
		if (ExplosionType == 16 || ExplosionType == 17 || ExplosionType == 205 || ExplosionType == 206) {
			if (auto sharedGFX = GraphicFX[0].lock()) {
				sharedGFX->CreationSize.x -= sharedGFX->CreationSize.x * TimeDelta;
				sharedGFX->CreationSize.z -= sharedGFX->CreationSize.z * TimeDelta;
				sharedGFX->DeadZone -= sharedGFX->DeadZone * TimeDelta;
			}
		}
		if (ExplosionType == 18 || ExplosionType == 209) {
			if (auto sharedGFX = GraphicFX[0].lock()) {
				sharedGFX->CreationSize.x -= sharedGFX->CreationSize.x * TimeDelta;
				sharedGFX->CreationSize.z -= sharedGFX->CreationSize.z * TimeDelta;
				sharedGFX->DeadZone -= sharedGFX->DeadZone * TimeDelta;
			}
		}
		if (ExplosionType == 19 || ExplosionType == 210) {
			if (auto sharedGFX = GraphicFX[0].lock()) {
				sharedGFX->CreationSize.x += 10.0f * TimeDelta;
				sharedGFX->CreationSize.z += 10.0f * TimeDelta;
				sharedGFX->DeadZone += 10.0f * TimeDelta;
			}
			if (auto sharedGFX = GraphicFX[1].lock()) {
				sharedGFX->CreationSize.x -= sharedGFX->CreationSize.x * TimeDelta;
				sharedGFX->CreationSize.z -= sharedGFX->CreationSize.z * TimeDelta;
				sharedGFX->DeadZone -= sharedGFX->DeadZone * TimeDelta;
			}
		}
	}








	// если не считаем в шейдере, нужно перебрать геометрию и собрать новые буферы
	if (!GameConfig().UseGLSL120) {
		// первый раз - просто запоминаем время
		if (ExplosionGeometryMoveLastTime == -1)
			ExplosionGeometryMoveLastTime = Time;

		// если время подошло - делаем анимацию, иначе - пропускаем этот цикл
		if (ExplosionGeometryMoveLastTime + 0.035f < Time) {
			// поворачиваем все объекты
			float ExplosionGeometryMove = Time-ExplosionGeometryMoveLastTime;
			ExplosionGeometryMoveLastTime = Time;


			// расчет физики для каждой частицы
			if (!Model3DBlocks.empty()) {
				sVECTOR3D TMP;
				int Count = 0;

				for (auto &tmpModel3DBlock : Model3DBlocks) {
					for (unsigned int i = 0; i < tmpModel3DBlock.VertexQuantity; i+=3) {
						if (ExplosionPieceData[Count].Life > 0.0f) {
							// получаем текущий вектор движения данного треугольника
							TMP = ExplosionPieceData[Count].Velocity^ExplosionGeometryMove;
							ExplosionPieceData[Count].Velocity -= TMP;

							// вычисляем время жизни
							ExplosionPieceData[Count].Life -= ExplosionGeometryMove;


							if (ExplosionPieceData[Count].Life<=0.001f) {
								ExplosionPieceData[Count].Life = 0.0f;

								tmpModel3DBlock.VertexArray.get()[(i + 1) * tmpModel3DBlock.VertexStride] = tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride];
								tmpModel3DBlock.VertexArray.get()[(i + 1) * tmpModel3DBlock.VertexStride + 1] = tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride + 1];
								tmpModel3DBlock.VertexArray.get()[(i + 1) * tmpModel3DBlock.VertexStride + 2] = tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride + 2];

								tmpModel3DBlock.VertexArray.get()[(i + 2) * tmpModel3DBlock.VertexStride] = tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride];
								tmpModel3DBlock.VertexArray.get()[(i + 2) * tmpModel3DBlock.VertexStride + 1] = tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride + 1];
								tmpModel3DBlock.VertexArray.get()[(i + 2) * tmpModel3DBlock.VertexStride + 2] = tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride + 2];
							} else {
								// уменьшаем частицу, перебираем размер и текстурные координаты
								{
									float tmp = tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 1)] - tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * i];
									tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 1)] -= (tmp / ExplosionPieceData[Count].Life) * ExplosionGeometryMove;
									tmp = tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride*(i + 1) + 1] - tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * i + 1];
									tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 1) + 1] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
									tmp = tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 1) + 2] - tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * i + 2];
									tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 1) + 2] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;

									tmp = tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 2)] - tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * i];
									tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 2)] -= (tmp / ExplosionPieceData[Count].Life) * ExplosionGeometryMove;
									tmp = tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 2) + 1] - tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * i + 1];
									tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 2) + 1] -= (tmp / ExplosionPieceData[Count].Life) * ExplosionGeometryMove;
									tmp = tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 2) + 2] - tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * i + 2];
									tmpModel3DBlock.VertexArray.get()[tmpModel3DBlock.VertexStride * (i + 2) + 2] -= (tmp / ExplosionPieceData[Count].Life) * ExplosionGeometryMove;
								}

								tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride] += TMP.x;
								tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride + 1] += TMP.y;
								tmpModel3DBlock.VertexArray.get()[i * tmpModel3DBlock.VertexStride + 2] += TMP.z;

								tmpModel3DBlock.VertexArray.get()[(i + 1) * tmpModel3DBlock.VertexStride] += TMP.x;
								tmpModel3DBlock.VertexArray.get()[(i + 1) * tmpModel3DBlock.VertexStride + 1] += TMP.y;
								tmpModel3DBlock.VertexArray.get()[(i + 1) * tmpModel3DBlock.VertexStride + 2] += TMP.z;

								tmpModel3DBlock.VertexArray.get()[(i + 2) * tmpModel3DBlock.VertexStride] += TMP.x;
								tmpModel3DBlock.VertexArray.get()[(i + 2) * tmpModel3DBlock.VertexStride + 1] += TMP.y;
								tmpModel3DBlock.VertexArray.get()[(i + 2) * tmpModel3DBlock.VertexStride + 2] += TMP.z;
							}

						}

						Count++;
					}


					// пересоздаем буферы vbo и vao
					// удаляем старые буферы, если они есть, создаем новые
					// ! индексный буфер не трогаем, его не надо пересоздавать вообще

					if (tmpModel3DBlock.VBO)
						vw_DeleteBufferObject(tmpModel3DBlock.VBO);
					if (tmpModel3DBlock.VAO)
						vw_DeleteVAO(tmpModel3DBlock.VAO);


					// делаем VBO
					if (!vw_BuildBufferObject(eBufferObject::Vertex, tmpModel3DBlock.VertexQuantity * tmpModel3DBlock.VertexStride * sizeof(float), tmpModel3DBlock.VertexArray.get(), tmpModel3DBlock.VBO))
						tmpModel3DBlock.VBO = 0;

					// делаем IBO, создаем его один раз, если его нет
					if (!tmpModel3DBlock.IBO) {
						if (!vw_BuildBufferObject(eBufferObject::Index, tmpModel3DBlock.VertexQuantity * sizeof(unsigned), tmpModel3DBlock.IndexArray.get(), tmpModel3DBlock.IBO))
							tmpModel3DBlock.IBO = 0;
					}

					// делаем VAO
					if (!vw_BuildVAO(tmpModel3DBlock.VAO, tmpModel3DBlock.VertexFormat,
							 tmpModel3DBlock.VertexStride * sizeof(float),
							 tmpModel3DBlock.VBO, tmpModel3DBlock.IBO))
						tmpModel3DBlock.VAO = 0;
				}
			}
		}
	} else {
		// меняем данные глобальные для шейдера, тут делаем столько столько позволяет, а не 30 раз как с изменением геометрии
		for (auto &tmpModel3DBlock : Model3DBlocks) {
			// общий коэф. расстояния
			tmpModel3DBlock.ShaderData[1] += tmpModel3DBlock.ShaderData[0]*TimeDelta;
			// дельта скорости
			tmpModel3DBlock.ShaderData[0] -= tmpModel3DBlock.ShaderData[0]*TimeDelta;
		}
	}






	// делаем AABB по упращенной схеме, главное для нас - скорость
	float AABBSpeedTmp = AABBSpeed*TimeDelta;
	AABBSpeed -= AABBSpeedTmp;
	if (!Model3DBlocks.empty()) {
		float MinX = AABB[6].x - AABBSpeedTmp;
		float MaxX = AABB[0].x + AABBSpeedTmp;
		float MinY = AABB[6].y - AABBSpeedTmp;
		float MaxY = AABB[0].y + AABBSpeedTmp;
		float MinZ = AABB[6].z - AABBSpeedTmp;
		float MaxZ = AABB[0].z + AABBSpeedTmp;

		AABB[0] = sVECTOR3D(MaxX, MaxY, MaxZ);
		AABB[1] = sVECTOR3D(MinX, MaxY, MaxZ);
		AABB[2] = sVECTOR3D(MinX, MaxY, MinZ);
		AABB[3] = sVECTOR3D(MaxX, MaxY, MinZ);
		AABB[4] = sVECTOR3D(MaxX, MinY, MaxZ);
		AABB[5] = sVECTOR3D(MinX, MinY, MaxZ);
		AABB[6] = sVECTOR3D(MinX, MinY, MinZ);
		AABB[7] = sVECTOR3D(MaxX, MinY, MinZ);
	}








	sVECTOR3D TMP2 = VelocityOrientation^(OldSpeed*TimeDelta);


	// перемещаем эффекты
	for (unsigned int i = 0; i < GraphicFX.size(); i++) {
		if (auto sharedGFX = GraphicFX[i].lock()) {
			sVECTOR3D tmpLocation{sharedGFX->GetLocation()};
			tmpLocation += TMP2;

			// взрыв пришельцев
			if ((ExplosionTypeByClass == 1) && (ExplosionType == 2)) {
				if (i == 1)
					sharedGFX->MoveSystemLocation(tmpLocation);
				else
					sharedGFX->MoveSystem(tmpLocation);
			} else
				sharedGFX->MoveSystem(tmpLocation);

			// всегда подтягиваем данные
			sharedGFX->SetStartLocation(tmpLocation);
		}
	}


	// плавно замедляем...
	if (NeedStop) OldSpeed -= OldSpeed*TimeDelta;
	if (OldSpeed<0.0f) OldSpeed = 0.0f;

	// перемещаем объект
	SetLocation(Location+TMP2);





	// объект в порядке - удалять не нужно
	return true;
}



