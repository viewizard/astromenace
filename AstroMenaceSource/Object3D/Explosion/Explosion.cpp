/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright © 2006-2012 Michael Kurinnoy, Viewizard


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


/// подключаем нужные файлы
#include "Explosion.h"
#include "../../Game.h"


//-----------------------------------------------------------------------------
// Конструктор, инициализация всех переменных
//-----------------------------------------------------------------------------
CExplosion::CExplosion(void)
{

	// ставим тот, что нужно... т.е. того кого взорвали
	ObjectStatus = 1; // чужой
	ObjectType = 11;

	// нужно рисовать без оптимизации
	NeedCullFaces = false;

	ExplosionPieceData = 0;

	ExplosionGeometryMoveLastTime = -1;

	// нет взрыва, сразу уничтожаем
	Lifetime = 0.0f;

	// нет графических эффектов
	GraphicFXQuantity = 0;
	GraphicFX = 0;

	ExplosionType = 0;

	OldSpeed = 0.0f;
	VelocityOrientation = VECTOR3D(0,0,0);
	NeedStop = true;
	AABBSpeed = 0.0f;

	// подключаем к своему списку
	Next = Prev = 0;
	AttachExplosion(this);
}



//-----------------------------------------------------------------------------
// Деструктор
//-----------------------------------------------------------------------------
CExplosion::~CExplosion(void)
{
	if (ExplosionPieceData!=0){delete [] ExplosionPieceData; ExplosionPieceData = 0;}

	if (GraphicFX != 0)
	{
		for (int i=0; i<GraphicFXQuantity; i++)
		if (GraphicFX[i] != 0)
		{
			GraphicFX[i]->IsSuppressed = true;
			GraphicFX[i]->DestroyIfNoParticles = true;
		}
		delete [] GraphicFX; GraphicFX = 0;
	}


	DetachExplosion(this);
}








//-----------------------------------------------------------------------------
// Обновление данных объектa
//-----------------------------------------------------------------------------
bool CExplosion::Update(float Time)
{
	// вызываем родительскую функцию
	// если там передали удалить - выходим
	if (!::CObject3D::Update(Time))
	{
		// делаем правильную остановку частиц...
		if (GraphicFX != 0)
		for (int i=0; i<GraphicFXQuantity; i++)
		if (GraphicFX[i] != 0)
		{
			GraphicFX[i]->StopAllParticles();

			if (!(ExplosionTypeByClass == 2 && (ExplosionType == 16 || ExplosionType == 17
			|| ExplosionType == 18 || ExplosionType == 19
			|| ExplosionType == 205 || ExplosionType == 206 || ExplosionType == 209 || ExplosionType == 210)))
			{
                delete GraphicFX[i]; GraphicFX[i] = 0;
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
			|| ExplosionType == -217))
        )
	{
        if (GraphicFX != 0)
        for (int i=0; i<GraphicFXQuantity; i++)
        if (GraphicFX[i] != 0)
        if (Lifetime < GraphicFX[i]->Life)
        {
            // только говорим, чтобы больше не создавал частиц!!!
            // не удаляем и не зануляем - иначе не сможем им управлять
            GraphicFX[i]->IsSuppressed = true;
        }
    }

	// работа с эффектами
    if (ExplosionTypeByClass == 2)
    {
        if (ExplosionType == 16 || ExplosionType == 17 || ExplosionType == 205 || ExplosionType == 206)
        {
            GraphicFX[0]->CreationSize.x -= GraphicFX[0]->CreationSize.x*TimeDelta;
            GraphicFX[0]->CreationSize.z -= GraphicFX[0]->CreationSize.z*TimeDelta;
            GraphicFX[0]->DeadZone -= GraphicFX[0]->DeadZone*TimeDelta;
        }
        if (ExplosionType == 18 || ExplosionType == 209)
        {
            GraphicFX[0]->CreationSize.x -= GraphicFX[0]->CreationSize.x*TimeDelta;
            GraphicFX[0]->CreationSize.z -= GraphicFX[0]->CreationSize.z*TimeDelta;
            GraphicFX[0]->DeadZone -= GraphicFX[0]->DeadZone*TimeDelta;
        }
        if (ExplosionType == 19 || ExplosionType == 210)
        {
            GraphicFX[0]->CreationSize.x += 10.0f*TimeDelta;
            GraphicFX[0]->CreationSize.z += 10.0f*TimeDelta;
            GraphicFX[0]->DeadZone += 10.0f*TimeDelta;

            GraphicFX[1]->CreationSize.x -= GraphicFX[1]->CreationSize.x*TimeDelta;
            GraphicFX[1]->CreationSize.z -= GraphicFX[1]->CreationSize.z*TimeDelta;
            GraphicFX[1]->DeadZone -= GraphicFX[1]->DeadZone*TimeDelta;
        }
    }








	// если не считаем в шейдере, нужно перебрать геометрию и собрать новые буферы
	if (!Setup.UseGLSL)
	{
		// первый раз - просто запоминаем время
		if (ExplosionGeometryMoveLastTime == -1) ExplosionGeometryMoveLastTime = Time;

		// если время подошло - делаем анимацию, иначе - пропускаем этот цикл
		if (ExplosionGeometryMoveLastTime + 0.035f < Time)
		{
			// поворачиваем все объекты
			float ExplosionGeometryMove = Time-ExplosionGeometryMoveLastTime;
			ExplosionGeometryMoveLastTime = Time;



			// расчет физики для каждой частицы
			VECTOR3D TMP;
			int Count = 0;
			if (DrawObjectList != 0)
			for (int j=0; j<DrawObjectQuantity; j++)
			{
				for (int i=0; i<DrawObjectList[j].VertexCount; i+=3)
				{
					if (ExplosionPieceData[Count].Life > 0.0f)
					{
						// получаем текущий вектор движения данного треугольника
						TMP = ExplosionPieceData[Count].Velocity^ExplosionGeometryMove;
						ExplosionPieceData[Count].Velocity -= TMP;

						// вычисляем время жизни
						ExplosionPieceData[Count].Life -= ExplosionGeometryMove;


						if (ExplosionPieceData[Count].Life<=0.001f)
						{
							ExplosionPieceData[Count].Life = 0.0f;

							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].VertexStride] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride];
							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].VertexStride+1] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride+1];
							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].VertexStride+2] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride+2];

							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].VertexStride] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride];
							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].VertexStride+1] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride+1];
							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].VertexStride+2] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride+2];
						}
						else
						{
							// уменьшаем частицу, перебираем размер и текстурные координаты
							{
								float tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)] -DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+1] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+1];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+1] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+2] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+2];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+1)+2] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;

								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+1] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+1];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+1] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+2] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*i+2];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].VertexStride*(i+2)+2] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
							}

							DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride] += TMP.x;
							DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride+1] += TMP.y;
							DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].VertexStride+2] += TMP.z;

							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].VertexStride] += TMP.x;
							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].VertexStride+1] += TMP.y;
							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].VertexStride+2] += TMP.z;

							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].VertexStride] += TMP.x;
							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].VertexStride+1] += TMP.y;
							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].VertexStride+2] += TMP.z;
						}

					}

					Count++;
				}


				// пересоздаем буферы vbo и vao
				// удаляем старые буферы, если они есть, создаем новые
				// ! индексный буфер не трогаем, его не надо пересоздавать вообще

				if (DrawObjectList[j].VBO != 0)
				{
					vw_DeleteVBO(*DrawObjectList[j].VBO); delete DrawObjectList[j].VBO; DrawObjectList[j].VBO = 0;
				}
				if (DrawObjectList[j].VAO != 0)
				{
					vw_DeleteVAO(*DrawObjectList[j].VAO); delete DrawObjectList[j].VAO; DrawObjectList[j].VAO = 0;
				}


				// делаем VBO
				DrawObjectList[j].VBO = new unsigned int;
				if (!vw_BuildVBO(DrawObjectList[j].VertexCount, DrawObjectList[j].VertexBuffer, DrawObjectList[j].VertexStride, DrawObjectList[j].VBO))
				{
					delete DrawObjectList[j].VBO; DrawObjectList[j].VBO=0;
				}

				// делаем IBO, создаем его один раз, если его нет
				if (DrawObjectList[j].IBO == 0)
				{
					DrawObjectList[j].IBO = new unsigned int;
					if (!vw_BuildIBO(DrawObjectList[j].VertexCount, DrawObjectList[j].IndexBuffer, DrawObjectList[j].IBO))
					{
						delete DrawObjectList[j].IBO; DrawObjectList[j].IBO=0;
					}
				}

				// делаем VAO
				DrawObjectList[j].VAO = new unsigned int;
				if (!vw_BuildVAO(DrawObjectList[j].VAO, DrawObjectList[j].VertexCount, DrawObjectList[j].VertexFormat, DrawObjectList[j].VertexBuffer,
									DrawObjectList[j].VertexStride*sizeof(float), DrawObjectList[j].VBO,
									DrawObjectList[j].RangeStart, DrawObjectList[j].IndexBuffer, DrawObjectList[j].IBO))
				{
					delete DrawObjectList[j].VAO; DrawObjectList[j].VAO=0;
				}

			}
		}
	}
	else
	{
		// меняем данные глобальные для шейдера, тут делаем столько столько позволяет, а не 30 раз как с изменением геометрии
		for (int j=0; j<DrawObjectQuantity; j++)
		{
			// общий коэф. расстояния
			DrawObjectList[j].ShaderData[1] += DrawObjectList[j].ShaderData[0]*TimeDelta;
			// дельта скорости
			DrawObjectList[j].ShaderData[0] -= DrawObjectList[j].ShaderData[0]*TimeDelta;
		}
	}






	// делаем AABB по упращенной схеме, главное для нас - скорость
	float AABBSpeedTmp = AABBSpeed*TimeDelta;
	AABBSpeed -= AABBSpeedTmp;
	if (DrawObjectList != 0)
	{
		float MinX = AABB[6].x - AABBSpeedTmp;
		float MaxX = AABB[0].x + AABBSpeedTmp;
		float MinY = AABB[6].y - AABBSpeedTmp;
		float MaxY = AABB[0].y + AABBSpeedTmp;
		float MinZ = AABB[6].z - AABBSpeedTmp;
		float MaxZ = AABB[0].z + AABBSpeedTmp;

		AABB[0] = VECTOR3D(MaxX, MaxY, MaxZ);
		AABB[1] = VECTOR3D(MinX, MaxY, MaxZ);
		AABB[2] = VECTOR3D(MinX, MaxY, MinZ);
		AABB[3] = VECTOR3D(MaxX, MaxY, MinZ);
		AABB[4] = VECTOR3D(MaxX, MinY, MaxZ);
		AABB[5] = VECTOR3D(MinX, MinY, MaxZ);
		AABB[6] = VECTOR3D(MinX, MinY, MinZ);
		AABB[7] = VECTOR3D(MaxX, MinY, MinZ);
	}








	VECTOR3D TMP2 = VelocityOrientation^(OldSpeed*TimeDelta);


	// перемещаем эффекты
	if (GraphicFX != 0)
	for (int i=0; i<GraphicFXQuantity; i++)
	if (GraphicFX[i] != 0)
	{
		VECTOR3D Loc;
		GraphicFX[i]->GetLocation(&Loc);

        // взрыв пришельцев
        if (ExplosionTypeByClass == 1 && ExplosionType == 2)
        {
			if (i==1)
				GraphicFX[i]->MoveSystemLocation(Loc + TMP2);
			else
				GraphicFX[i]->MoveSystem(Loc + TMP2);
		}
        else
            GraphicFX[i]->MoveSystem(Loc + TMP2);


		// всегда подтягиваем данные
		GraphicFX[i]->SetStartLocation(Loc + TMP2);
	}




	// плавно замедляем...
	if (NeedStop) OldSpeed -= OldSpeed*TimeDelta;
	if (OldSpeed<0.0f) OldSpeed = 0.0f;

	// перемещаем объект
	SetLocation(Location+TMP2);





	// объект в порядке - удалять не нужно
	return true;
}



