/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: RLE.cpp

	Copyright (c) 2001-2007 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.0

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


#include "VFS.h"




//------------------------------------------------------------------------------------
// разарх данных методом RLE
//------------------------------------------------------------------------------------
int vw_RLEtoDATA(int size, BYTE **dstVFS, BYTE *srcVFS, int *dsizeVFS, int ssizeVFS)
{
	// считаем кол-во памяти...
	int i = 0;
	*dsizeVFS = 0;

	if (size == 0)
	{
		while (i<ssizeVFS)
		{
			// если повторяющаяся последовательность...
			if (srcVFS[i] != 0)
			{
				(*dsizeVFS) += srcVFS[i];
				i++;
				// пропускаем байт информации...
				i++;
			}
			else // идет последовательность неповт. данных.
			{
				// пропускаем 0
				i++;
				BYTE temp = srcVFS[i] ;
				i++;
				// отсчитываем колличество символов
				(*dsizeVFS) += temp;
				// переходим на новую позицию...
				i += temp;
			}
		}
	}
	else (*dsizeVFS) = size;

	// выделяем память...
	*dstVFS = new BYTE[*dsizeVFS]; if ((*dstVFS) == 0) return 0;



	// распаковываем...
	i = 0;
	int j = 0; // текущее состояние в распакованном массиве...
	while (i<ssizeVFS)
	{
		// если повторяющаяся последовательность...
		if (srcVFS[i] != 0)
		{
			// узнаем кол-во повторов...
			BYTE rep = srcVFS[i];
			i++;
			// узнаем байт информации...
			BYTE tmp = srcVFS[i];
			i++;
			// заносим информацию...
			memset((*dstVFS) + j, tmp, rep);
			j += rep;
		}
		else // идет последовательность неповт. данных.
		{
			// пропускаем 0
			i++;
			// количество данных...
			BYTE temp = srcVFS[i];
			i++;
			// копируем данные....
			memcpy((*dstVFS) + j, srcVFS + i, temp);
			// переходим на новую позицию...
			i += temp;
			j += temp;
		}
	}

	return 0;
}




//------------------------------------------------------------------------------------
// архивир данных методом RLE
//------------------------------------------------------------------------------------
int vw_DATAtoRLE(BYTE **dstVFS, BYTE *srcVFS, int *dsizeVFS, int ssizeVFS)
{
	// составить таблицу повторных участков,
	// текущий уч-к..
	int block = 0;
	*dsizeVFS = 0;
	int *Start = 0;
	int *Stop = 0;
	bool rep = false; // есть-нет повтора...
    BYTE PRactiv = srcVFS[0]; // предыдущий активный (проверяемый) байт

    // резервируем память...
    Start = new int[ssizeVFS / 2];
	if (Start == 0) return 0;
    Stop = new int[ssizeVFS / 2];
	if (Stop == 0) return 0;

	for (int i=1; i<ssizeVFS; i++)
	{
		if (PRactiv == srcVFS[i])
		{
			if (!rep) {Start[block] = i - 1; rep=true;}
			Stop[block] = i;
		}
		else
			if (rep) {rep=false;block++;}

		PRactiv = srcVFS[i];
	}
	if (rep) {rep=false;block++;} //если до конца...

    // найти размер результ. последов.

	if (block > 0)
	{
		int Sr=0;// начельный адрес последовательности...
		int St;  // конечный адрес последовательности...
		int dl;  // длина полседовательности...

		for (int i=0; i<block; i++)
		{
			St = Start[i] - 1;
			dl = St - Sr + 1;
			// пишем с разбиением на части по FF ...если последовательность не идентична
			while (dl >= 0xFF)
			{
				(*dsizeVFS) += 2;
				(*dsizeVFS) += 0xFF;
				dl -= 0xFF;
			}
			// пишем...то, что осталось...
			if (dl > 0) (*dsizeVFS) += 2 + dl;

			Sr = Start[i];
			St = Stop[i];
			dl = St - Sr + 1;
			// пишем...последовательность повторяющихся данных...
			while (dl >= 0xFF)
			{
				(*dsizeVFS) += 2;
				dl -= 0xFF;
			}
			// пишем...то, что осталось...
			if (dl > 0) (*dsizeVFS) += 2;

			Sr = St + 1;
		}
		St = ssizeVFS-1;
		dl = St - Sr + 1;
		// пишем с разбиением на части по FF ...если последовательность не идентична
		while (dl >= 0xFF)
		{
			(*dsizeVFS) += 2;
			(*dsizeVFS) += 0xFF;
			dl -= 0xFF;
		}
		// пишем...то, что осталось...
		if (dl > 0) (*dsizeVFS) += 2 + dl;
	}
	else
	{
		int dl;  // длина полседовательности...
		dl = ssizeVFS - 0;
        // пишем с разбиением на части по FF ...если последовательность не идентична
		while (dl >= 0xFF)
		{
			(*dsizeVFS) += 2;
			(*dsizeVFS) += 0xFF;
			dl -= 0xFF;
		}
		// пишем...то, что осталось...
		if (dl > 0) (*dsizeVFS) += 2 + dl;
	}

    // резервируем память...
	*dstVFS = 0;
    *dstVFS = new BYTE[*dsizeVFS];
	if (*dstVFS == 0)
	{
		delete [] Start; Start = 0;
		delete [] Stop; Stop = 0;
		return 0;
	}


    // пакуем...
    int j = 0; // указатель на позицию в преобразованных данных.

	if (block > 0)
	{
		int Sr=0; //(предустановка на начало - бит 0)
		int St;
		int dl;
		int popr=j;

		for (int i=0; i<block; i++)
		{
			St = Start[i] - 1;
			dl = St - Sr + 1;
			popr=0;
			// пишем с разбиением на части по FF ...если последовательность не идентична
			while (dl >= 0xFF)
			{
				(*dstVFS)[j] = 0;
				j++;
				(*dstVFS)[j] = 0xFF;
				j++;
				dl -= 0xFF;
				memcpy( (*dstVFS) + j, srcVFS + Sr + popr, 0xFF );
				popr += 0xFF;
				j += 0xFF;
			}
			// пишем...то, что осталось...
			if (dl > 0)
			{
				(*dstVFS)[j] = 0;
				j++;
				(*dstVFS)[j] = (BYTE)dl;
				j++;
				memcpy( (*dstVFS) + j, srcVFS + Sr +  popr, dl );
				popr += (BYTE)dl;
				j += dl;
			};

			Sr = Start[i];
			St = Stop[i];
			dl = St - Sr + 1;
			// пишем...последовательность повторяющихся данных...
			while (dl >= 0xFF)
			{
				(*dstVFS)[j] = 0xFF;
				j++;
				(*dstVFS)[j] = srcVFS[Start[i]];
				j++;
				dl -= 0xFF;
			}
			// пишем...то, что осталось...
			if (dl > 0)
			{
				(*dstVFS)[j] = dl;
				j++;
				(*dstVFS)[j] = srcVFS[Start[i]];
				j++;
			}

			Sr = St + 1;
		}
		St = ssizeVFS-1;
		dl = St - Sr + 1;
		popr=0;
        // пишем с разбиением на части по FF ...если последовательность не идентична
        // концовка (до сомого последнего бита)
		while (dl >= 0xFF)
		{
			(*dstVFS)[j] = 0;
			j++;
			(*dstVFS)[j] = 0xFF;
			j++;
			dl -= 0xFF;
			memcpy( (*dstVFS) + j, srcVFS + Sr + popr, 0xFF );
		    popr += 0xFF;
			j += 0xFF;
		}
		// пишем...то, что осталось...
		if (dl > 0)
		{
			(*dstVFS)[j] = 0;
			j++;
			(*dstVFS)[j] = (BYTE)dl;
			j++;
			memcpy( (*dstVFS) + j, srcVFS + Sr + popr, (BYTE)dl );
		    popr += dl;
			j += dl;
		};

	}
	else
	{
		int Sr=0;// начельный адрес последовательности...
		int dl;  // длина полседовательности...
		dl = ssizeVFS - 0;
		int popr=j;
		while (dl >= 0xFF)
		{
			(*dstVFS)[j] = 0;
			j++;
			(*dstVFS)[j] = 0xFF;
			j++;
			dl -= 0xFF;
			memcpy( (*dstVFS) + j, srcVFS + Sr + popr, 0xFF );
		    popr += 0xFF;
			j += 0xFF;
		}
		// пишем...то, что осталось...
		if (dl > 0)
		{
			(*dstVFS)[j] = 0;
			j++;
			(*dstVFS)[j] = (BYTE)dl;
			j++;
			memcpy( (*dstVFS) + j, srcVFS + Sr + popr, (BYTE)dl );
		    popr += dl;
			j += dl;
		};
	}

    delete [] Start; Start = 0;
	delete [] Stop; Stop = 0;

	return 0;
}
