/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Explosion.cpp

	Copyright (c) 2006-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.2

******************************************************************************

	AstroMenace game source code available under "dual licensing" model.
	The licensing options available are:

	* Commercial Licensing. This is the appropriate option if you are
	  creating proprietary applications and you are not prepared to
	  distribute and share the source code of your application.
	  Contact us for pricing at viewizard@viewizard.com

	* Open Source Licensing. This is the appropriate option if you want
	  to share the source code of your application with everyone you
	  distribute it to, and you also want to give them the right to share
	  who uses it. You should have received a copy of the GNU General Public
	  License version 3 with this source codes.
	  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/


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








	// если не считаем в шейдере, нужно это делать сдесь
	// работаем без индекс буфера, сдесь последовательность именно вертексов, индекс нулевой
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

							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].Stride] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride];
							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].Stride+1] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride+1];
							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].Stride+2] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride+2];

							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].Stride] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride];
							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].Stride+1] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride+1];
							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].Stride+2] = DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride+2];
						}
						else
						{
							// уменьшаем частицу, перебираем размер и текстурные координаты
							{
								float tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+1)] -DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*i];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+1)] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+1)+1] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*i+1];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+1)+1] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+1)+2] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*i+2];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+1)+2] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;

								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+2)] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*i];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+2)] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+2)+1] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*i+1];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+2)+1] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
								tmp = DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+2)+2] - DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*i+2];
								DrawObjectList[j].VertexBuffer[DrawObjectList[j].Stride*(i+2)+2] -= (tmp/ExplosionPieceData[Count].Life)*ExplosionGeometryMove;
							}

							DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride] += TMP.x;
							DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride+1] += TMP.y;
							DrawObjectList[j].VertexBuffer[i*DrawObjectList[j].Stride+2] += TMP.z;

							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].Stride] += TMP.x;
							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].Stride+1] += TMP.y;
							DrawObjectList[j].VertexBuffer[(i+1)*DrawObjectList[j].Stride+2] += TMP.z;

							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].Stride] += TMP.x;
							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].Stride+1] += TMP.y;
							DrawObjectList[j].VertexBuffer[(i+2)*DrawObjectList[j].Stride+2] += TMP.z;
						}

					}

					Count++;
				}


				// пересоздаем буферы vbo и vao
				// удаляем старые буферы, если они есть, создаем новые
				// ! индексный буфер не трогаем, его не надо пересоздавать вообще

				if (DrawObjectList[j].VertexBufferVBO != 0)
				{
					vw_DeleteVBO(*DrawObjectList[j].VertexBufferVBO); delete DrawObjectList[j].VertexBufferVBO; DrawObjectList[j].VertexBufferVBO = 0;
				}
				if (DrawObjectList[j].VAO != 0)
				{
					vw_DeleteVAO(*DrawObjectList[j].VAO); delete DrawObjectList[j].VAO; DrawObjectList[j].VAO = 0;
				}


				// делаем VBO
				DrawObjectList[j].VertexBufferVBO = new unsigned int;
				if (!vw_BuildVBO(DrawObjectList[j].VertexCount, DrawObjectList[j].VertexBuffer, DrawObjectList[j].Stride, DrawObjectList[j].VertexBufferVBO))
				{
					delete DrawObjectList[j].VertexBufferVBO; DrawObjectList[j].VertexBufferVBO=0;
				}

				// делаем индекс VBO, создаем его один раз, если его нет
				if (DrawObjectList[j].IndexBufferVBO == 0)
				{
					DrawObjectList[j].IndexBufferVBO = new unsigned int;
					if (!vw_BuildIndexVBO(DrawObjectList[j].VertexCount, DrawObjectList[j].IndexBuffer, DrawObjectList[j].IndexBufferVBO))
					{
						delete DrawObjectList[j].IndexBufferVBO; DrawObjectList[j].IndexBufferVBO=0;
					}
				}

				// делаем VAO
				DrawObjectList[j].VAO = new unsigned int;
				if (!vw_BuildVAO(DrawObjectList[j].VAO, DrawObjectList[j].VertexCount, DrawObjectList[j].FVF_Format, DrawObjectList[j].VertexBuffer,
									DrawObjectList[j].Stride*sizeof(float), DrawObjectList[j].VertexBufferVBO,
									DrawObjectList[j].RangeStart, DrawObjectList[j].IndexBuffer, DrawObjectList[j].IndexBufferVBO))
				{
					delete DrawObjectList[j].VAO; DrawObjectList[j].VAO=0;
				}

			}
		}
	}
	else
	{
		// меняем данные глобальные для шейдера, тут делаем столько столько позволяет, а не 30 раз как с изменением геометрии

		// общий коэф. расстояния
		ShaderData[1] += ShaderData[0]*TimeDelta;
		// дельта скорости
		ShaderData[0] -= ShaderData[0]*TimeDelta;
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



