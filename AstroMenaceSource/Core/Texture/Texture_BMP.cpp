/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Texture_BMP.cpp

	Copyright (c) 2001-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 3.1

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


#include "Texture.h"

// в виндовсе эти структуры есть, для линукса - объявляем
#ifndef WIN32
typedef struct tagBITMAPFILEHEADER {
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  long  biWidth;
  long  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  long  biXPelsPerMeter;
  long  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD;
#endif // WIN32


//------------------------------------------------------------------------------------
// загрузка данных из BMP файла в DIB массив
//------------------------------------------------------------------------------------
int ReadBMP(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels)
{

	BITMAPFILEHEADER	header;
	BITMAPINFOHEADER	bitmapInfo;
	RGBQUAD				bmiColors[256];

	// читаем BITMAPFILEHEADER
	pFile->fread(&header, sizeof(BITMAPFILEHEADER), 1);
	// читаем BITMAPINFO
	pFile->fread(&bitmapInfo, sizeof(BITMAPINFOHEADER), 1);


	*DWidth = bitmapInfo.biWidth;
	*DHeight = bitmapInfo.biHeight;
	// Устанавливаем отступ между строчками
	int bmp_stride = (*DWidth) * bitmapInfo.biBitCount / 8;
	// должен быть приведен к DWORD построчно
	while((bmp_stride % 4) != 0) bmp_stride++;


	// если нужно - читаем таблицу цветов...
	if (bitmapInfo.biBitCount == 8)
		pFile->fread(&bmiColors, sizeof(RGBQUAD)*256, 1);

	// отступаем от заголовка
	pFile->fseek(header.bfOffBits, SEEK_SET);

	// Читаем данные из файла используем bmp_stride, фотошоп не передает размера!!!
	BYTE *DIBtemp = 0;
	DIBtemp = new BYTE[bmp_stride*(*DHeight)];
	if (DIBtemp == 0) return 0;
	pFile->fread(DIBtemp, bmp_stride*(*DHeight), 1);


	// если 32 битный рисунок...
	if (bitmapInfo.biBitCount == 32)
	{
		// Выделяем память...
		*DIB = 0;
		*DIB = new BYTE[(*DWidth)*(*DHeight)*4];
		if (*DIB == 0) return 0;

		for (int y=0; y<(*DHeight); y++)
		{
			int strtmp = bmp_stride*y;
			for (int x=0; x<(*DWidth); x++)
				memcpy(*DIB+x*4+(y*(*DWidth)*4), DIBtemp+strtmp+x*4, 4);
		}

		*DChanels = 4;
	}


	// если 24 битный рисунок...
	if (bitmapInfo.biBitCount == 24)
	{
		// Выделяем память...
		*DIB = 0;
		*DIB = new BYTE[(*DWidth)*(*DHeight)*3];
		if (*DIB == 0) return 0;

		for (int y=0; y<(*DHeight); y++)
		{
			int strtmp = bmp_stride*y;
			for (int x=0; x<(*DWidth); x++)
				memcpy(*DIB+x*3+(y*(*DWidth)*3), DIBtemp+strtmp+x*3, 3);
		}

		*DChanels = 3;
	}


	// если 8 битный рисунок...
	if (bitmapInfo.biBitCount == 8)
	{
		// Выделяем память...
		*DIB = 0;
		*DIB = new BYTE[(*DWidth)*(*DHeight)*3];
		if (*DIB == 0) return 0;

		// Преобразуем в нормальный вид ...
		for (int y=0; y<(*DHeight); y++)
		{
			int strtmp = bmp_stride*y;
			for (int x=0; x<(*DWidth); x++)
			{
				memcpy(*DIB+x*3+(y*(*DWidth)*3), &bmiColors[DIBtemp[strtmp+x]].rgbBlue, sizeof(BYTE));
				memcpy(*DIB+x*3+(y*(*DWidth)*3)+1, &bmiColors[DIBtemp[strtmp+x]].rgbGreen, sizeof(BYTE));
				memcpy(*DIB+x*3+(y*(*DWidth)*3)+2, &bmiColors[DIBtemp[strtmp+x]].rgbRed, sizeof(BYTE));
			}
		}

		*DChanels = 3;
	}


	delete [] DIBtemp;

	return 1;
}

