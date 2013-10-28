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


#include "Texture.h"
#include "../System/System.h"
#include "../RendererInterface/RendererInterface.h"



extern	eDevCaps OpenGL_DevCaps;

extern eTexture *StartTexMan;
extern int FilteringTexMan;
extern int Address_ModeTexMan;
extern BYTE ARedTexMan;
extern BYTE AGreenTexMan;
extern BYTE ABlueTexMan;

extern bool MipMap;
extern int  AFlagTexMan;
extern bool AlphaTexMan;
void vw_AttachTexture(eTexture* Texture);
void vw_DetachTexture(eTexture* Texture);


int ReadJPG(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels);
int ReadTGA(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels);
int ReadPNG(BYTE **DIB, eFILE *pFile, int *DWidth, int *DHeight, int *DChanels);


//------------------------------------------------------------------------------------
// Освобождение памяти и удаление текстуры
//------------------------------------------------------------------------------------
void vw_ReleaseTexture(eTexture* Texture)
{
	// проверка входящих данных
	if (Texture == 0) return;

	// отключаем текстуру от менерджера текстур
	vw_DetachTexture(Texture);

	// освобождаем память
	vw_DeleteTexture(Texture->TextureID);
	if (Texture->Name != 0) delete [] Texture->Name;
	delete Texture;
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

	// переносим во временный массив данные...
	BYTE *src = *DIB;
	BYTE *dst = 0;
	dst = new BYTE[width*height*Texture->Bytes]; if (dst == 0) return;

	// растягиваем исходный массив (или сжимаем)
	for (int j=0; j<height; j++)
	{
		int y = (j * Texture->Height) / height;
		int offset_y = y * Texture->Width;

		for (int i=0; i<width; i++)
		{
			int x = (i * Texture->Width) / width;
			int offset_x = (offset_y + x) * Texture->Bytes;

			dst[(i+j*width)*Texture->Bytes] = src[offset_x];
			dst[(i+j*width)*Texture->Bytes+1] = src[offset_x+1];
			dst[(i+j*width)*Texture->Bytes+2] = src[offset_x+2];
			if (Texture->Bytes == 4)
				dst[(i+j*width)*Texture->Bytes+3] = src[offset_x+3];
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

	// Формируем данные по цветам...
	BYTE GreyRedC = (BYTE)(((float)Texture->ARed / 255) * 76);
	BYTE GreyGreenC = (BYTE)(((float)Texture->AGreen / 255) * 150);
	BYTE GreyBlueC = (BYTE)(((float)Texture->ABlue / 255) * 28);
	BYTE GreyC = GreyBlueC+GreyGreenC+GreyRedC;

	for(int j1 = 0; j1 < Texture->Height;j1++)
	{

		int k1 = stride*j1;// делаем правильное смещение при переходе
		int k2 = stride2*j1;

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

	if (DIBtemp != 0) delete [] DIBtemp;
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

	for(int j1 = 0; j1 < Texture->Height;j1++)
	{
		int k1 = stride*j1;
		int k2 = stride2*j1;

		for(int j2 = 0; j2 < Texture->Width;j2++)
		{
			DIB[k1] = DIBtemp[k2];
			DIB[k1 + 1] = DIBtemp[k2 + 1];
			DIB[k1 + 2] = DIBtemp[k2 + 2];

			k2 += 4;
			k1 += 3;
		}
	}

	if (DIBtemp != 0) delete [] DIBtemp;
	*DIBRESULT = DIB;
	Texture->Bytes = 3;
}








//------------------------------------------------------------------------------------
// конвертирование в VW2D
//------------------------------------------------------------------------------------
void vw_ConvertImageToVW2D(const char *SrcName, const char *DestName)
{
	eFILE *pFile = 0;
	int DWidth = 0;
	int DHeight = 0;
	int DChanels = 0;
	BYTE *tmp_image = 0;

	int LoadAs = TGA_FILE;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Открываем файл
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	pFile = vw_fopen(SrcName);
	if (pFile == 0)
	{
		fprintf(stderr, "Unable to found %s\n", SrcName);
		return;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Ищем как грузить по расширению
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем, вообще есть расширение или нет, плюс, получаем указатель на последнюю точку
	const char *file_ext = strrchr(SrcName, '.');
	if (file_ext)
	{
		if (!strcasecmp(".tga", file_ext)) LoadAs = TGA_FILE;
		else
			if (!strcasecmp(".jpg", file_ext)) LoadAs = JPG_FILE;
			else
				if (!strcasecmp(".png", file_ext)) LoadAs = PNG_FILE;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Загружаем
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	switch(LoadAs)
	{
		case TGA_FILE:
			ReadTGA(&tmp_image, pFile, &DWidth, &DHeight, &DChanels);
			break;

		case JPG_FILE:
			ReadJPG(&tmp_image, pFile, &DWidth, &DHeight, &DChanels);
			break;

		case PNG_FILE:
			ReadPNG(&tmp_image, pFile, &DWidth, &DHeight, &DChanels);
			break;

		default:
			fprintf(stderr, "Unable to load %s\n", SrcName);
			return;
			break;
	}

	if (tmp_image == 0)
	{
		fprintf(stderr, "Unable to load %s\n", SrcName);
		return;
	}

	// все, файл нам больше не нужен
	vw_fclose(pFile);



	// записываем данные на диск

	SDL_RWops *FileVW2D;
	FileVW2D = SDL_RWFromFile(DestName, "wb");
	// если не можем создать файл на запись - уходим
    if (FileVW2D == NULL)
    {
        fprintf(stderr, "Can't create %s file on disk.\n", DestName);
        return;
    }

	// маркер файла 4 байта
	char tmp1[5] = "VW2D";
	SDL_RWwrite(FileVW2D, tmp1, 4, 1);

	SDL_RWwrite(FileVW2D, &DWidth, sizeof(int), 1);
	SDL_RWwrite(FileVW2D, &DHeight, sizeof(int), 1);
	SDL_RWwrite(FileVW2D, &DChanels, sizeof(int), 1);
	SDL_RWwrite(FileVW2D, tmp_image, DWidth*DHeight*DChanels, 1);

	SDL_RWclose(FileVW2D);

	// освобождаем память
	delete [] tmp_image;
}








//------------------------------------------------------------------------------------
// загрузка текстуры из файла и подключение к менеджеру текстур
//------------------------------------------------------------------------------------
eTexture* vw_LoadTexture(const char *nName, const char *RememberAsName, int CompressionType, int LoadAs, int NeedResizeW, int NeedResizeH)
{
	// временно, файл текстуры
	eFILE *pFile = 0;

	int DWidth = 0;
	int DHeight = 0;
	int DChanels = 0;
	BYTE *tmp_image = 0;

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
		// проверяем, вообще есть расширение или нет, плюс, получаем указатель на последнюю точку
		const char *file_ext = strrchr(nName, '.');
		if (file_ext)
		{
			if (!strcasecmp(".tga", file_ext)) LoadAs = TGA_FILE;
			else
				if (!strcasecmp(".vw2d", file_ext)) LoadAs = VW2D_FILE;
				else
					if (!strcasecmp(".jpg", file_ext)) LoadAs = JPG_FILE;
					else
						if (!strcasecmp(".png", file_ext)) LoadAs = PNG_FILE;
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

		case PNG_FILE:
			ReadPNG(&tmp_image, pFile, &DWidth, &DHeight, &DChanels);
			break;

		case VW2D_FILE:
			// пропускаем заголовок "VW2D"
			pFile->fseek(4, SEEK_SET);
			// считываем ширину
			pFile->fread(&DWidth, sizeof(int), 1);
			// считываем высоту
			pFile->fread(&DHeight, sizeof(int), 1);
			// считываем кол-во каналов
			pFile->fread(&DChanels, sizeof(int), 1);
			// резервируем память
			tmp_image = new BYTE[DWidth*DHeight*DChanels];
			// считываем уже готовый к созданию текстуры массив
			pFile->fread(tmp_image, DWidth*DHeight*DChanels, 1);
			break;

		default:
			return 0;
			break;
	}

	if (tmp_image == 0)
	{
		fprintf(stderr, "Unable to load %s\n", nName);
		return 0;
	}

	// все, файл нам больше не нужен
	vw_fclose(pFile);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Сохраняем имя текстуры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	eTexture *Result = 0;

	if (RememberAsName == NULL)
	{
		Result = vw_CreateTextureFromMemory(nName, tmp_image, DWidth, DHeight, DChanels, CompressionType, NeedResizeW, NeedResizeH);
	}
	else // иначе, есть имя под которым надо запомнить
	{
		Result = vw_CreateTextureFromMemory(RememberAsName, tmp_image, DWidth, DHeight, DChanels, CompressionType, NeedResizeW, NeedResizeH);
	}


	// освобождаем память
	if (tmp_image != 0){delete [] tmp_image; tmp_image = 0;}


	return Result;
}








//------------------------------------------------------------------------------------
// создание текстуры из памяти
//------------------------------------------------------------------------------------
eTexture* vw_CreateTextureFromMemory(const char *TextureName, BYTE * DIB, int DWidth, int DHeight, int DChanels, int CompressionType, int NeedResizeW, int NeedResizeH, bool NeedDuplicateCheck)
{
	// проверяем в списке, если уже создавали ее - просто возвращаем указатель
	if (NeedDuplicateCheck)
	{
		eTexture *Tmp = StartTexMan;
		while (Tmp != 0)
		{
			eTexture *Tmp1 = Tmp->Next;
			if(vw_strcmp(Tmp->Name, TextureName) == 0)
			{
				printf("Texture already loaded: %s\n", TextureName);
				return Tmp;
			}
			Tmp = Tmp1;
		}
	}


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
	Texture->Width = DWidth;
	Texture->Height = DHeight;
	Texture->Bytes = DChanels;

	// временный массив данных
	BYTE *tmp_image = 0;
	tmp_image = new BYTE[DWidth*DHeight*DChanels]; if (tmp_image == 0) { delete Texture; return 0; }
	memcpy(tmp_image, DIB, DWidth*DHeight*DChanels);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Сохраняем имя текстуры
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Texture->Name = new char[strlen(TextureName)+1]; if (Texture->Name == 0) { delete Texture; delete [] tmp_image; return 0; }
	strcpy(Texture->Name, TextureName);


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
	if (NeedResizeW!=0 && NeedResizeH!=0)
		ResizeImage(NeedResizeW, NeedResizeH, &tmp_image, Texture);


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
	Texture->TextureID = vw_BuildTexture(tmp_image, Texture->Width, Texture->Height, MipMap, Texture->Bytes, CompressionType);
	// устанавливаем параметры
	vw_SetTextureFiltering(FilteringTexMan);
	vw_SetTextureAddressMode(Address_ModeTexMan);
	// анбиндим
	vw_BindTexture(0, 0);

	// освобождаем память
	if (tmp_image != 0) delete [] tmp_image;

	// присоединяем текстуру к менеджеру текстур
	vw_AttachTexture(Texture);
	printf("Ok ... %s\n", TextureName);
	return Texture;
}

