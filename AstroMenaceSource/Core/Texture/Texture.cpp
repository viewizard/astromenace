/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: Texture.cpp

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


#include "Texture.h"
#include "../System/System.h"
#include "../RendererInterface/RendererInterface.h"



extern	eDevCaps OpenGL_DevCaps;


extern int FilteringTexMan;
extern int Address_ModeTexMan;
extern BYTE ARedTexMan;
extern BYTE AGreenTexMan;
extern BYTE ABlueTexMan;

extern bool MipMap;
extern int  AFlagTexMan;
extern bool AlphaTexMan;
void AttachTexture(eTexture* Texture);
void DetachTexture(eTexture* Texture);


int ReadJPG(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels);
int ReadTGA(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels);


//------------------------------------------------------------------------------------
// Освобождение памяти и удаление текстуры
//------------------------------------------------------------------------------------
void vw_ReleaseTexture(eTexture* Texture)
{
	// проверка входящих данных
	if (Texture == 0) return;

	// отключаем текстуру от менерджера текстур
	DetachTexture(Texture);

	// освобождаем память
	vw_DeleteTexture(Texture->TextureID);
	if (Texture->Name != 0) {delete [] Texture->Name; Texture->Name = 0;}
	if (Texture != 0){delete Texture; Texture = 0;}
}






//------------------------------------------------------------------------------------
// Переработка размеров...    ближайшая, большая четная или степень 2
//------------------------------------------------------------------------------------
static int power_of_two(int Num)
{
	int value = 1;

	while (value < Num)
	{
		value <<= 1;
	}
	return value;
}
void Resize(BYTE **DIB, eTexture *Texture)
{
	// берем размеры к которым нужно "подгонять"
	int powWidth = power_of_two(Texture->Width);
	int powHeight = power_of_two(Texture->Height);

	// нужно ли обрабатывать вообще?
	if (powWidth==Texture->Width && powHeight==Texture->Height) return;

	BYTE *DIBtemp = *DIB;
	*DIB = 0;
	*DIB = new BYTE[powWidth*powHeight*Texture->Bytes]; if (*DIB == 0) return;

	// делаем все по цвету-прозначности + ставим все прозрачным
	BYTE ColorF[4];
	ColorF[0] = Texture->ARed;
	ColorF[1] = Texture->AGreen;
	ColorF[2] = Texture->ABlue;
	ColorF[3] = 0;//если Texture->Bytes == 4, его возьмем
	for (int i=0; i<powWidth*powHeight*Texture->Bytes; i+=Texture->Bytes)
	{
		memcpy(*DIB+i, ColorF, Texture->Bytes);
	}


	// находим отступ между строчками
	int stride = Texture->Width * Texture->Bytes;
	// должен быть приведен к DWORD построчно (чтобы не было проблем с нечетными данными)
	while((stride % 4) != 0) stride++;


	// вставляем исходный рисунок
	for (int y=0; y<Texture->Height; y++)
	{
		//int st1 = (y*(powWidth))*Texture->Bytes;
		// чтобы правильно делать без SDL_image
		int st1 = ((y + (powHeight - Texture->Height))*(powWidth))*Texture->Bytes;
		int st2 = (y*(stride));
		memcpy(*DIB+st1, DIBtemp+st2, stride);
	}

	// меняем значения текстуры
	Texture->Width = powWidth;
	Texture->Height = powHeight;
	// освобождаем память
	if (DIBtemp != 0){delete [] DIBtemp; DIBtemp = 0;}
}











//------------------------------------------------------------------------------------
// Растягивание картинки, нужно устанавливать дополнительно...
//------------------------------------------------------------------------------------
void ResizeImage(int width, int height, BYTE **DIB, eTexture *Texture)
{
	if (width == Texture->Width && height == Texture->Height) return;

	int		i, j, x, y, offset_y, offset_x;

	// переносим во временный массив данные...
	BYTE *src = *DIB;
	BYTE *dst = 0;
	dst = new BYTE[width*height*Texture->Bytes]; if (dst == 0) return;

	// растягиваем исходный массив (или сжимаем)
	for (j=0; j<height; j++)
	{
		y = (j * Texture->Height) / height;
		offset_y = y * Texture->Width;

		for (i=0; i<width; i++)
		{
			x = (i * Texture->Width) / width;
			offset_x = (offset_y + x) * Texture->Bytes;

			dst[(i+j*width)*Texture->Bytes] = src[(x+y*Texture->Width)*Texture->Bytes];
			dst[(i+j*width)*Texture->Bytes+1] = src[(x+y*Texture->Width)*Texture->Bytes+1];
			dst[(i+j*width)*Texture->Bytes+2] = src[(x+y*Texture->Width)*Texture->Bytes+2];
			if (Texture->Bytes == 4)
				dst[(i+j*width)*Texture->Bytes+3] = src[(x+y*Texture->Width)*Texture->Bytes+3];
		}
	}

	// меняем значения текстуры
	Texture->Width = width;
	Texture->Height = height;
	// освобождаем память
	if (src != 0){delete [] src; src = 0;}
	// устанавливаем указатель на новый блок памяти
	*DIB = dst;
}









//------------------------------------------------------------------------------------
// Создание альфа канала
//------------------------------------------------------------------------------------
void CreateAlpha(BYTE **DIBRESULT, eTexture *Texture, int AlphaFlag)
{
	// находим отступ между строчками
	int stride = Texture->Width * 3;
	if (Texture->Width != 2 && Texture->Width != 1)
		while((stride % 4) != 0) stride++;
	int stride2 = Texture->Width * 4;
	while((stride2 % 4) != 0) stride2++;

	// сохраняем во временном указателе
	BYTE *DIBtemp  = *DIBRESULT;
	BYTE *DIB = 0;
	DIB = new BYTE[stride2*Texture->Height]; if (DIB == 0) return;

	int k1=0;
	int k2=0;

	// Формируем данные по цветам...
	BYTE GreyRedC = (BYTE)(((float)Texture->ARed / 255) * 76);
	BYTE GreyGreenC = (BYTE)(((float)Texture->AGreen / 255) * 150);
	BYTE GreyBlueC = (BYTE)(((float)Texture->ABlue / 255) * 28);
	BYTE GreyC = GreyBlueC+GreyGreenC+GreyRedC;

	for(int j1 = 0; j1 < Texture->Height;j1++)
	{

		k1 = stride*j1;// делаем правильное смещение при переходе
		k2 = stride2*j1;

		for(int j2 = 0; j2 < Texture->Width;j2++)
		{
			DIB[k2] = DIBtemp[k1];
			DIB[k2 + 1] = DIBtemp[k1 + 1];
			DIB[k2 + 2] = DIBtemp[k1 + 2];

			switch(AlphaFlag)
			{
				case TX_ALPHA_GREYSC:
				{
					// Формируем данные по цветам...
					BYTE GreyRed = (BYTE)(((float)DIB[k2+2] / 255) * 76);
					BYTE GreyGreen = (BYTE)(((float)DIB[k2+1] / 255) * 150);
					BYTE GreyBlue = (BYTE)(((float)DIB[k2] / 255) * 28);
					DIB[k2 + 3] = GreyBlue+GreyGreen+GreyRed;
					break;
				}
				case TX_ALPHA_EQUAL:
				{
					if ((Texture->ABlue==DIB[k2])&(Texture->AGreen==DIB[k2+1])&(Texture->ARed==DIB[k2+2])) DIB[k2+3] = 0;//Alpha
						else DIB[k2 + 3] = 255;
					break;
				}
				case TX_ALPHA_GEQUAL:
				{
					// Формируем данные по цветам...
					BYTE GreyRed = (BYTE)(((float)DIB[k2+2] / 255) * 76);
					BYTE GreyGreen = (BYTE)(((float)DIB[k2+1] / 255) * 150);
					BYTE GreyBlue = (BYTE)(((float)DIB[k2] / 255) * 28);
					BYTE Grey = GreyBlue+GreyGreen+GreyRed;

					if (GreyC >= Grey) DIB[k2+3] = 0;//Alpha
						else DIB[k2 + 3] = 255;
					break;
				}
				case TX_ALPHA_LEQUAL:
				{
					// Формируем данные по цветам...
					BYTE GreyRed = (BYTE)(((float)DIB[k2+2] / 255) * 76);
					BYTE GreyGreen = (BYTE)(((float)DIB[k2+1] / 255) * 150);
					BYTE GreyBlue = (BYTE)(((float)DIB[k2] / 255) * 28);
					BYTE Grey = GreyBlue+GreyGreen+GreyRed;

					if (GreyC <= Grey) DIB[k2+3] = 0;//Alpha
						else DIB[k2 + 3] = 255;
					break;
				}
				case TX_ALPHA_GREAT:
				{
					// Формируем данные по цветам...
					BYTE GreyRed = (BYTE)(((float)DIB[k2+2] / 255) * 76);
					BYTE GreyGreen = (BYTE)(((float)DIB[k2+1] / 255) * 150);
					BYTE GreyBlue = (BYTE)(((float)DIB[k2] / 255) * 28);
					BYTE Grey = GreyBlue+GreyGreen+GreyRed;

					if (GreyC > Grey) DIB[k2+3] = 0;//Alpha
						else DIB[k2 + 3] = 255;
					break;
				}
				case TX_ALPHA_LESS:
				{
					// Формируем данные по цветам...
					BYTE GreyRed = (BYTE)(((float)DIB[k2+2] / 255) * 76);
					BYTE GreyGreen = (BYTE)(((float)DIB[k2+1] / 255) * 150);
					BYTE GreyBlue = (BYTE)(((float)DIB[k2] / 255) * 28);
					BYTE Grey = GreyBlue+GreyGreen+GreyRed;

					if (GreyC < Grey) DIB[k2+3] = 0;//Alpha
						else DIB[k2 + 3] = 255;
					break;
				}
				default:
				{
					DIB[k2 + 3] = 255;
					break;
				}

			}

			k2 += 4;
			k1 += 3;
		}
	}

	if (DIBtemp != 0){delete [] DIBtemp; DIBtemp = 0;}
	*DIBRESULT = DIB;
	Texture->Bytes = 4;
}





//------------------------------------------------------------------------------------
// Удаляем альфа канал
//------------------------------------------------------------------------------------
void DeleteAlpha(BYTE **DIBRESULT, eTexture *Texture)
{

	// находим отступ между строчками
	int stride = Texture->Width * 3;
	while((stride % 4) != 0) stride++;
	int stride2 = Texture->Width * 4;
	while((stride2 % 4) != 0) stride2++;

	// сохраняем во временном указателе
	BYTE *DIBtemp  = *DIBRESULT;
	BYTE *DIB = 0;
	DIB = new BYTE[stride*Texture->Height]; if (DIB == 0) return;

	int k1=0;
	int k2=0;

	for(int j1 = 0; j1 < Texture->Height;j1++)
	{
		k1 = stride*j1;
		k2 = stride2*j1;

		for(int j2 = 0; j2 < Texture->Width;j2++)
		{
			DIB[k1] = DIBtemp[k2];
			DIB[k1 + 1] = DIBtemp[k2 + 1];
			DIB[k1 + 2] = DIBtemp[k2 + 2];

			k2 += 4;
			k1 += 3;
		}
	}

	if (DIBtemp != 0){delete [] DIBtemp; DIBtemp = 0;}
	*DIBRESULT = DIB;
	Texture->Bytes = 3;
}









//------------------------------------------------------------------------------------
// загрузка текстуры их файла и подключение к менеджеру текстур
//------------------------------------------------------------------------------------
eTexture* vw_LoadTexture(const char *nName, const char *RememberAsName, bool NeedCompression, int LoadAs, int NeedResizeX, int NeedResizeY)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Cоздаем объект
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	eTexture *Texture = 0;
	Texture = new eTexture; if (Texture == 0) return 0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Начальные установки текстуры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Texture->ARed = ARedTexMan;
	Texture->AGreen = AGreenTexMan;
	Texture->ABlue = ABlueTexMan;
	Texture->Prev = 0;
	Texture->Next = 0;
	Texture->Num = 0;
	Texture->Name = 0;
	Texture->TextureID = 0;
	Texture->Filtering = FilteringTexMan;
	Texture->Address_Mode = Address_ModeTexMan;
	Texture->Width = 0;
	Texture->Height = 0;
	Texture->TexturePrior = 0;
	Texture->MipMap = MipMap;

	// временно, файл текстуры
	eFILE *pFile = 0;

	int DWidth = 0;
	int DHeight = 0;
	int DChanels = 0;
	BYTE *tmp_image = 0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Сохраняем имя текстуры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (RememberAsName == NULL)
	{
		Texture->Name = new char[strlen(nName)+1]; if (Texture->Name == 0) return 0;
		strcpy(Texture->Name, nName);
	}
	else // иначе, есть имя под которым надо запомнить
	{
		Texture->Name = new char[strlen(RememberAsName)+1]; if (Texture->Name == 0) return 0;
		strcpy(Texture->Name, RememberAsName);
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Открываем файл
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	pFile = vw_fopen(nName);
	if (pFile == 0)
	{
		fprintf(stderr, "Unable to found %s\n", nName);
		return 0;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Ищем как грузить текстуру по расширению
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (LoadAs == AUTO_FILE)
	{
		if( vw_TestFileExtension( nName, "tga" ) || vw_TestFileExtension( nName, "TGA" ))
		{
			LoadAs = TGA_FILE;
		}
		else
		{
			if( vw_TestFileExtension( nName, "jpg" ) || vw_TestFileExtension( nName, "JPG" ))
				LoadAs = JPG_FILE;
		}
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Загружаем текстуру
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch(LoadAs)
	{
		case TGA_FILE:
			ReadTGA(&tmp_image, pFile, &DWidth, &DHeight, &DChanels);
			break;

		case JPG_FILE:
			ReadJPG(&tmp_image, pFile, &DWidth, &DHeight, &DChanels);
			break;

		default:
			return 0;
			break;
	}

	if (tmp_image == 0)
	{
		fprintf(stderr, "Unable to load %s\n", nName, SDL_GetError());
		return 0;
	}


	// вот теперь все готово, идем дальше
	Texture->Width = DWidth;
	Texture->Height = DHeight;
	Texture->Bytes = DChanels;

	// все, файлы нам больше не нужны
	vw_fclose(pFile);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Делаем альфа канал
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (Texture->Bytes == 4)
	{
		if (!AlphaTexMan)
			DeleteAlpha(&tmp_image, Texture);

	}
	if (Texture->Bytes == 3)
	{
		if (AlphaTexMan)
			CreateAlpha(&tmp_image, Texture, AFlagTexMan);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Растягиваем, если есть запрос
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (NeedResizeX!=0 && NeedResizeY!=0)
		ResizeImage(NeedResizeX, NeedResizeY, &tmp_image, Texture);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Сохраняем размеры картинки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Texture->SrcWidth = Texture->Width;
	Texture->SrcHeight = Texture->Height;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Делаем подгонку по размерам, с учетом необходимости железа
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!OpenGL_DevCaps.TextureNPOTSupported) // если не поддерживаем - берем и сами растягиваем до степени двойки
		Resize(&tmp_image, Texture);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Создаем текстуру
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Texture->TextureID = vw_CreateTexture(tmp_image, Texture->Width, Texture->Height, MipMap, Texture->Bytes, NeedCompression);



	// освобождаем память
	if (tmp_image != 0){delete [] tmp_image; tmp_image = 0;}

	// присоединяем текстуру к менеджеру текстур
	AttachTexture(Texture);
	printf("Ok ... %s\n", nName);
	return Texture;
}







//------------------------------------------------------------------------------------
// быстрая установка текстуры и ее параметров
//------------------------------------------------------------------------------------
void vw_SetTextureT(DWORD Stage, eTexture *Tex, int AnisotropyLevel)
{
	if (Tex == 0) return;

	vw_SetTextureV(Stage, Tex);
	vw_SetTexFiltering(Stage, Tex->Filtering, AnisotropyLevel);
	vw_SetTexAddressMode(Stage, Tex->Address_Mode);
	vw_SetTexAlpha(true, 0.1f);
}
