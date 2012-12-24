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


#include "VFS.h"
#include "../Math/Math.h"


// Список подключенных VFS файлов
struct eVFS
{
	char *FileName;		// Имя файла VFS
	SDL_RWops *File;	// Указатель на файл виртуальной системы

	// данные для записи в создаваемую VFS
	int NumberOfFilesVFS;
	int HeaderLengthVFS;
	int HeaderOffsetVFS;
	int DataStartOffsetVFS;


	eVFS*	Prev;
	eVFS*	Next;
};
eVFS *StartVFS = 0;
eVFS *EndVFS = 0;





// Список файлов, доступных в подключенных (открытых) VFS
struct eVFS_Entry
{
	BOOL	Link;		// флаг что это не реальная запись а линк на существующую в VFS
	DWORD	NameLen;	// Кол-во байт в имени...
	char	*Name;		// Имя записи (имя файла) (может быть "линком")
	int		Offset;		// Смещение начала файла относительно начала файловой системы
	int		Length;		// Длина файла в системе
	int		RealLength;	// Длина файла после распаковки
	eVFS	*Parent;
	BYTE	ArhKeyLen;	// Кол-во байт ключа упаковки
	char	*ArhKey;	// Ключ-упаковки

	eVFS_Entry*	Prev;
	eVFS_Entry*	Next;

};
eVFS_Entry *StarVFSArray = 0;
eVFS_Entry *EndVFSArray = 0;





// Список файлов (открытых)
eFILE *StartFileVFS = 0;	// Указатель на первый файл в списке...
eFILE *EndFileVFS = 0;		// Указатель на последний файл в списке...


// чтобы при работе не было сбоев в последовательности файлов... блокируем этот участок
// при создании и закрытии файлов
bool fileIObusy = false;
// при чтении из файла вфс
bool VFSfileIObusy = false;





//------------------------------------------------------------------------------------
// тест сжатия
//------------------------------------------------------------------------------------
int CheckCompression(int tmpLength, const BYTE *buffer, char *ArhKeyVFS)
{
	BYTE *tmp = 0;
	tmp = new BYTE[tmpLength];
	memcpy(tmp, buffer, tmpLength);

	// устанавливаем данные
	BYTE *dstVFS = 0;
	BYTE *srcVFS = tmp;
	srcVFS = new BYTE[tmpLength];
	memcpy(srcVFS, buffer, tmpLength);

	int dsizeVFS = tmpLength;
	int ssizeVFS = tmpLength;



	// цикл по кол-ву примененных методов сжатия в ArhKeyVFS
	for (unsigned int i=0; i<strlen(ArhKeyVFS);i++)
	{
		// находим, какой текущий метод сжатия
		char S = ArhKeyVFS[strlen(ArhKeyVFS)-i-1];

		// если RLE
		if (S == VFS_DATA_ARH_RLE)
		{
			vw_DATAtoRLE(&dstVFS, srcVFS, &dsizeVFS, ssizeVFS);
			delete [] srcVFS; srcVFS = 0;
			srcVFS = dstVFS;
			ssizeVFS = dsizeVFS;
			dstVFS = 0;
		}
		// если HAFF
		if (S == VFS_DATA_ARH_HAFF)
		{
			vw_DATAtoHAFF(&dstVFS, srcVFS, &dsizeVFS, ssizeVFS);
			delete [] srcVFS; srcVFS = 0;
			srcVFS = dstVFS;
			ssizeVFS = dsizeVFS;
			dstVFS = 0;
		}
	}

	delete [] srcVFS; srcVFS = 0;

	return dsizeVFS;
}







//------------------------------------------------------------------------------------
// Создание VFS
//------------------------------------------------------------------------------------
int vw_CreateVFS(const char *Name, unsigned int BuildNumber)
{
	// Начальная подготовка структуры списка...
	eVFS *TempVFS = 0;
	TempVFS = new eVFS; if (TempVFS == 0) return -1;

	TempVFS->NumberOfFilesVFS = 0;
	TempVFS->HeaderLengthVFS = 0;
	TempVFS->HeaderOffsetVFS = 0;
	TempVFS->DataStartOffsetVFS = 0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// открываем файл VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	TempVFS->File = 0;
	TempVFS->File = SDL_RWFromFile(Name, "wb");
    if (TempVFS->File == NULL)
    {
        fprintf(stderr, "Can't open VFS file for write %s\n", Name);
        return -1; // ERROR
    }


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выделяем память для имени
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	TempVFS->FileName = 0;
	TempVFS->FileName = new char[strlen(Name)+1]; if (TempVFS->FileName == 0) return -1;
	strcpy(TempVFS->FileName, Name);


	// первый в списке...
	if (EndVFS == 0)
	{
		TempVFS->Prev = 0;
		TempVFS->Next = 0;
		StartVFS = TempVFS;
		EndVFS = TempVFS;
	}
	else // продолжаем заполнение...
	{
		TempVFS->Prev = EndVFS;
		TempVFS->Next = 0;
		EndVFS->Next = TempVFS;
		EndVFS = TempVFS;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// пишем VFS_ (4б) + версию (4б) + номер билда (4б)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	char tmp1[5] = "VFS_";
	SDL_RWwrite(TempVFS->File, tmp1, 4, 1);
	SDL_RWwrite(TempVFS->File, VFS_VER, 4, 1);
	SDL_RWwrite(TempVFS->File, &BuildNumber, 4, 1);




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// пишем смещение начала таблицы
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	TempVFS->HeaderOffsetVFS = 4+4+4+4+4;//VFS_ ver build offset lenght
	TempVFS->DataStartOffsetVFS = 4+4+4; //VFS_ ver build

	SDL_RWwrite(TempVFS->File, &TempVFS->HeaderOffsetVFS, 4, 1);
	SDL_RWwrite(TempVFS->File, &TempVFS->HeaderLengthVFS, 4, 1);

	printf("VFS file was created %s\n", Name);
	return 0;
}













//------------------------------------------------------------------------------------
// запись данных в VFS
//------------------------------------------------------------------------------------
int vw_WriteIntoVFSfromFile(const char *SrcName, const char *DstName)
{
	if (SrcName == 0) return -1;
	if (DstName == 0) return -1;

	// читаем данные файла в буфер
	SDL_RWops *Ftmp = SDL_RWFromFile(SrcName, "rb");
	// Если файл не найден - выходим
	if (Ftmp == NULL)
    {
		fprintf(stderr, "Can't find file %s !!!\n", SrcName);
        return -1;
    }

	// получаем размер файла
	SDL_RWseek(Ftmp, 0, SEEK_END);
	int tmpLength = SDL_RWtell(Ftmp);
	SDL_RWseek(Ftmp, 0, SEEK_SET);

	// копируем все данные файла в массив
	BYTE *tmp = 0;
	tmp = new BYTE[tmpLength];
	SDL_RWread(Ftmp, tmp, tmpLength, 1);
	SDL_RWclose(Ftmp);

	// запись в VFS
	if (0 != vw_WriteIntoVFSfromMemory(DstName, tmp, tmpLength))
	{
		// какая-то ошибка, не можем записать в VFS
		delete [] tmp; tmp = 0;
		fprintf(stderr, "Can't write into VFS from memory %s !!!\n", DstName);
		return -1;
	}

	// Освобождаем память
	delete [] tmp; tmp = 0;

	return 0;
}




//------------------------------------------------------------------------------------
// запись данных в VFS
//------------------------------------------------------------------------------------
int	vw_WriteIntoVFSfromMemory(const char *Name, const BYTE * buffer, int size)
{

	// (!) пока работаем только с одной открытой VFS в системе, собственно больше и не нужно

	eVFS *WritebleVFS = StartVFS;

	if (WritebleVFS == 0)
	{
		fprintf(stderr, "Can't find VFS opened for write\n");
		return -1;
	}



	// Начальная подготовка структуры списка для новых данных
	eVFS_Entry *NewVFS_Entry = 0;
	NewVFS_Entry = new eVFS_Entry; if (NewVFS_Entry == 0) return -1;

	// первый в списке...
	if (EndVFSArray == 0)
	{
		NewVFS_Entry->Prev = 0;
		NewVFS_Entry->Next = 0;
		StarVFSArray = NewVFS_Entry;
		EndVFSArray = NewVFS_Entry;
	}
	else // продолжаем заполнение...
	{
		NewVFS_Entry->Prev = EndVFSArray;
		NewVFS_Entry->Next = 0;
		EndVFSArray->Next = NewVFS_Entry;
		EndVFSArray = NewVFS_Entry;
	}

	NewVFS_Entry->Parent = WritebleVFS;

	NewVFS_Entry->ArhKeyLen = 0;
	NewVFS_Entry->ArhKey = 0;
	NewVFS_Entry->NameLen = (DWORD)strlen(Name);

	NewVFS_Entry->Name = 0;
	NewVFS_Entry->Name = new char[NewVFS_Entry->NameLen+1];
	strcpy(NewVFS_Entry->Name, Name);

	NewVFS_Entry->Link = false;
	NewVFS_Entry->Offset = WritebleVFS->HeaderOffsetVFS; // т.к. это будет последний файл в структуре...
	NewVFS_Entry->Length = size;
	NewVFS_Entry->RealLength = size;








	// надо составлять свою ArhKeyVFS
	// 0 - 0
	int BestMode = 0;

#ifdef compression

	int BestSize = NewVFS_Entry->Length;
	int TmpSize = 0;


	// 1 - 1
	NewVFS_Entry->ArhKey = new char[1+1];
	NewVFS_Entry->ArhKey[0] = '1';
	NewVFS_Entry->ArhKey[1] = 0;
	TmpSize = CheckCompression(NewVFS_Entry->Length, buffer, NewVFS_Entry->ArhKey);
	if (TmpSize<BestSize)
	{
		BestMode = 1;
		BestSize = TmpSize;
	}

	// 2 - 2
	if (NewVFS_Entry->ArhKey != 0){delete [] NewVFS_Entry->ArhKey; NewVFS_Entry->ArhKey = 0;}
	NewVFS_Entry->ArhKey = new char[1+1];
	NewVFS_Entry->ArhKey[0] = '2';
	NewVFS_Entry->ArhKey[1] = 0;
	TmpSize = CheckCompression(NewVFS_Entry->Length, buffer, NewVFS_Entry->ArhKey);
	if (TmpSize<BestSize)
	{
		BestMode = 2;
		BestSize = TmpSize;
	}

	// 3 - 12
	if (NewVFS_Entry->ArhKey != 0){delete [] NewVFS_Entry->ArhKey; NewVFS_Entry->ArhKey = 0;}
	NewVFS_Entry->ArhKey = new char[2+1];
	NewVFS_Entry->ArhKey[0] = '1';
	NewVFS_Entry->ArhKey[0] = '2';
	NewVFS_Entry->ArhKey[1] = 0;
	TmpSize = CheckCompression(NewVFS_Entry->Length, buffer, NewVFS_Entry->ArhKey);
	if (TmpSize<BestSize)
	{
		BestMode = 3;
		BestSize = TmpSize;
	}

	// 4 - 21
	if (NewVFS_Entry->ArhKey != 0){delete [] NewVFS_Entry->ArhKey; NewVFS_Entry->ArhKey = 0;}
	NewVFS_Entry->ArhKey = new char[2+1];
	NewVFS_Entry->ArhKey[0] = '2';
	NewVFS_Entry->ArhKey[0] = '1';
	NewVFS_Entry->ArhKey[1] = 0;
	TmpSize = CheckCompression(NewVFS_Entry->Length, buffer, NewVFS_Entry->ArhKey);
	if (TmpSize<BestSize)
	{
		BestMode = 4;
		BestSize = TmpSize;
	}

#endif // compression

	if (NewVFS_Entry->ArhKey != 0){delete [] NewVFS_Entry->ArhKey; NewVFS_Entry->ArhKey = 0;}

	switch (BestMode)
	{
		case 0:
			NewVFS_Entry->ArhKeyLen = 2;
			NewVFS_Entry->ArhKey = new char[NewVFS_Entry->ArhKeyLen];
			NewVFS_Entry->ArhKey[0] = '0';
			NewVFS_Entry->ArhKey[1] = 0;
			break;
		case 1:
			NewVFS_Entry->ArhKeyLen = 2;
			NewVFS_Entry->ArhKey = new char[NewVFS_Entry->ArhKeyLen];
			NewVFS_Entry->ArhKey[0] = '1';
			NewVFS_Entry->ArhKey[1] = 0;
			break;
		case 2:
			NewVFS_Entry->ArhKeyLen = 2;
			NewVFS_Entry->ArhKey = new char[NewVFS_Entry->ArhKeyLen];
			NewVFS_Entry->ArhKey[0] = '2';
			NewVFS_Entry->ArhKey[1] = 0;
			break;
		case 3:
			NewVFS_Entry->ArhKeyLen = 3;
			NewVFS_Entry->ArhKey = new char[NewVFS_Entry->ArhKeyLen];
			NewVFS_Entry->ArhKey[0] = '1';
			NewVFS_Entry->ArhKey[1] = '2';
			NewVFS_Entry->ArhKey[2] = 0;
			break;
		case 4:
			NewVFS_Entry->ArhKeyLen = 3;
			NewVFS_Entry->ArhKey = new char[NewVFS_Entry->ArhKeyLen];
			NewVFS_Entry->ArhKey[0] = '2';
			NewVFS_Entry->ArhKey[1] = '1';
			NewVFS_Entry->ArhKey[2] = 0;
			break;
	}








	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если используется сжатие - запаковываем данные
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// устанавливаем данные
	BYTE *dstVFS = 0;
	BYTE *srcVFS = 0;
	srcVFS = new BYTE[NewVFS_Entry->Length];
	memcpy(srcVFS, buffer, NewVFS_Entry->Length);
	int ssizeVFS = NewVFS_Entry->Length;

	if (!((NewVFS_Entry->ArhKey[0]=='0')&(strlen(NewVFS_Entry->ArhKey)==1)))
	{
		// цикл по кол-ву примененных методов сжатия в ArhKeyVFS
		for (unsigned int i=0; i<strlen(NewVFS_Entry->ArhKey);i++)
		{
			// находим, какой текущий метод сжатия
			char S = NewVFS_Entry->ArhKey[strlen(NewVFS_Entry->ArhKey)-i-1];

			// если RLE
			if (S == VFS_DATA_ARH_RLE)
			{
				vw_DATAtoRLE(&dstVFS, srcVFS, &NewVFS_Entry->Length, ssizeVFS);
				delete [] srcVFS; srcVFS = 0;
				srcVFS = dstVFS;
				ssizeVFS = NewVFS_Entry->Length;
				dstVFS = 0;

			}
			// если HAFF
			if (S == VFS_DATA_ARH_HAFF)
			{
				vw_DATAtoHAFF(&dstVFS, srcVFS, &NewVFS_Entry->Length, ssizeVFS);
				delete [] srcVFS; srcVFS = 0;
				srcVFS = dstVFS;
				ssizeVFS = NewVFS_Entry->Length;
				dstVFS = 0;
			}
		}
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// пишем данные в VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SDL_RWseek(WritebleVFS->File, WritebleVFS->HeaderOffsetVFS, SEEK_SET);
	SDL_RWwrite(WritebleVFS->File, srcVFS, NewVFS_Entry->Length, 1);
	delete [] srcVFS; srcVFS = 0;




	// переписывать все ентри, принадлежащие этому вфс файлу, в конец
	eVFS_Entry *Tmp = StarVFSArray;
	while (Tmp != 0)
	{
		eVFS_Entry *Tmp1 = Tmp->Next;

		// пишем только файлы (не линки)
		if ((!Tmp->Link) && (NewVFS_Entry->Parent == WritebleVFS))
		{
			SDL_RWwrite(WritebleVFS->File, &Tmp->ArhKeyLen, 1, 1);
			if (Tmp->ArhKeyLen > 0)
				SDL_RWwrite(WritebleVFS->File, Tmp->ArhKey, Tmp->ArhKeyLen, 1);
			SDL_RWwrite(WritebleVFS->File, &Tmp->NameLen, 2, 1);
			SDL_RWwrite(WritebleVFS->File, Tmp->Name, Tmp->NameLen, 1);
			SDL_RWwrite(WritebleVFS->File, &Tmp->Offset, 4, 1);
			SDL_RWwrite(WritebleVFS->File, &Tmp->Length, 4, 1);
			SDL_RWwrite(WritebleVFS->File, &Tmp->RealLength, 4, 1);
		}

		Tmp = Tmp1;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// меняем данные
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	WritebleVFS->HeaderOffsetVFS += NewVFS_Entry->Length;
	WritebleVFS->HeaderLengthVFS += 1+NewVFS_Entry->ArhKeyLen+2+NewVFS_Entry->NameLen+4+4+4;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// переписываем смещение начала таблицы
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SDL_RWseek(WritebleVFS->File, WritebleVFS->DataStartOffsetVFS, SEEK_SET);
	SDL_RWwrite(WritebleVFS->File, &WritebleVFS->HeaderOffsetVFS, 4, 1);
	SDL_RWwrite(WritebleVFS->File, &WritebleVFS->HeaderLengthVFS, 4, 1);


	printf("%s file added to VFS.\n", Name);

	return 0;
}




//------------------------------------------------------------------------------------
// Открытие (подключение) VFS
//------------------------------------------------------------------------------------
int vw_OpenVFS(const char *Name, unsigned int BuildNumber)
{
	unsigned int vfsBuildNumber = 0;
	int POS = 0; // указатель позиции в буфере.
	BYTE *buff = 0;
	int HeaderOffsetVFS;
	int HeaderLengthVFS;


	// Начальная подготовка структуры списка...
	eVFS *TempVFS = 0;
	TempVFS = new eVFS; if (TempVFS == 0) return -1;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// открываем файл VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	TempVFS->File = 0;
	TempVFS->File = SDL_RWFromFile(Name, "rb");
    if (TempVFS->File == NULL)
    {
        fprintf(stderr, "Can't find VFS file %s\n", Name);
        return -1; // ERROR
    }



    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выделяем память для имени
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	TempVFS->FileName = 0;
	TempVFS->FileName = new char[strlen(Name)+1]; if (TempVFS->FileName == 0) return -1;
	strcpy(TempVFS->FileName, Name);




	// первый в списке...
	if (EndVFS == 0)
	{
		TempVFS->Prev = 0;
		TempVFS->Next = 0;
		StartVFS = TempVFS;
		EndVFS = TempVFS;
	}
	else // продолжаем заполнение...
	{
		TempVFS->Prev = EndVFS;
		TempVFS->Next = 0;
		EndVFS->Next = TempVFS;
		EndVFS = TempVFS;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Проверяем сигнатуру "VFS_" - 4 байт
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	char Sign[4]; Sign[4] = 0;
	if(SDL_RWread(TempVFS->File, &Sign, 4, 1) == -1)
	{
		// если файл меньше, значит ошибка
		fprintf(stderr, "VFS file size error %s\n", Name);
		goto vw_OpenVFS_Error;
	}
	if (strncmp(Sign, "VFS_", 4) != 0)
	{
		// нет сигнатуры
		fprintf(stderr, "VFS file header error %s\n", Name);
		goto vw_OpenVFS_Error;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Bерсия - 4 байт
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	char Version[4]; Version[4] = 0;
	if(SDL_RWread(TempVFS->File, &Version, 4, 1) == -1)
	{
		fprintf(stderr, "VFS file corrupted: %s\n", Name);
		goto vw_OpenVFS_Error;
	}
	if (strncmp(Version, "v1.5", 4) != 0)
	{
		fprintf(stderr, "VFS file has wrong version, version %s not supported: %s\n", Version, Name);
		goto vw_OpenVFS_Error;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Билд - 4 байт (если предали ноль - проверять билды не нужно)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if(SDL_RWread(TempVFS->File, &vfsBuildNumber, 4, 1) == -1)
	{
		fprintf(stderr, "VFS file corrupted: %s\n", Name);
		goto vw_OpenVFS_Error;
	}
	// если передали ноль - проверка не нужна
	if (BuildNumber != 0)
	{
		if (vfsBuildNumber == 0)
		{
			printf("VFS file build number was not set (0).\n");
		}
		else
		{
			if (BuildNumber != vfsBuildNumber)
			{
				fprintf(stderr, "VFS file has wrong build number (%i), you need VFS with build number %i\n", vfsBuildNumber, BuildNumber);
				goto vw_OpenVFS_Error;
			}
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// читаем смещение таблицы данных VFS - 4 байт
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	HeaderOffsetVFS = SDL_ReadLE32(TempVFS->File);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// читаем длину таблицы данных VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	HeaderLengthVFS = SDL_ReadLE32(TempVFS->File);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Читаем в буфер всю таблицу
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SDL_RWseek(TempVFS->File, HeaderOffsetVFS, SEEK_SET);
	buff = new BYTE[HeaderLengthVFS]; if (buff == 0) return -1;
	SDL_RWread(TempVFS->File, buff, HeaderLengthVFS, 1);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Составляем (дополняем) список доступных файлов
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SDL_RWseek(TempVFS->File, HeaderOffsetVFS, SEEK_SET);
	while (POS<HeaderLengthVFS)
	{
		// Начальная подготовка структуры списка...
		eVFS_Entry *Temp = 0;
		Temp = new eVFS_Entry; if (Temp == 0) return -1;

		// первый в списке...
		if (EndVFSArray == 0)
		{
			Temp->Prev = 0;
			Temp->Next = 0;
			StarVFSArray = Temp;
			EndVFSArray = Temp;
		}
		else // продолжаем заполнение...
		{
			Temp->Prev = EndVFSArray;
			Temp->Next = 0;
			EndVFSArray->Next = Temp;
			EndVFSArray = Temp;
		}

		Temp->Parent = TempVFS;

		SDL_RWseek(TempVFS->File, 1, SEEK_CUR);
		Temp->ArhKeyLen = buff[POS];
		Temp->ArhKey = new char[Temp->ArhKeyLen+1]; if (Temp->ArhKey == 0) return -1;
		Temp->ArhKey[Temp->ArhKeyLen] = 0;// последний символ всегда ноль - конец строки
		SDL_RWread(TempVFS->File, Temp->ArhKey, Temp->ArhKeyLen, 1);

		SDL_RWseek(TempVFS->File, 2, SEEK_CUR);
		Temp->NameLen = buff[POS+1+Temp->ArhKeyLen+1]*0x100+buff[POS+Temp->ArhKeyLen+1];
		Temp->Name = new char[Temp->NameLen+1]; if (Temp->Name == 0) return -1;
		Temp->Name[Temp->NameLen] = 0;// последний символ всегда ноль - конец строки
		SDL_RWread(TempVFS->File, Temp->Name, Temp->NameLen, 1);
		Temp->Link = false;
		Temp->Offset = SDL_ReadLE32(TempVFS->File);
		Temp->Length = SDL_ReadLE32(TempVFS->File);
		Temp->RealLength = SDL_ReadLE32(TempVFS->File);
		POS = POS + 1 + Temp->ArhKeyLen + 2 + Temp->NameLen + 4 + 4 + 4;
	}

	// Освобождаем память...
	if (buff != 0){delete [] buff; buff = 0;}



	// выходим, все хорошо
	printf("VFS file was opened %s\n", Name);
	return 0;


	// была ошибка
vw_OpenVFS_Error:
	SDL_RWclose(TempVFS->File);
	if (TempVFS->FileName != 0){delete [] TempVFS->FileName; TempVFS->FileName = 0;}
	return -1;
}






//------------------------------------------------------------------------------------
// Закрываем VFS
//------------------------------------------------------------------------------------
void vw_CloseVFS(void)
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// даже если файла нет, все равно освобождаем память
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	eVFS_Entry *Tmp = StarVFSArray;
	while (Tmp != 0)
	{
		eVFS_Entry *Tmp1 = Tmp->Next;

		if (Tmp->Name != 0) {delete [] Tmp->Name; Tmp->Name = 0;}
		if (Tmp->ArhKey != 0) {delete [] Tmp->ArhKey; Tmp->ArhKey = 0;}

		// переустанавливаем указатели...
		if (StarVFSArray == Tmp) StarVFSArray = Tmp->Next;
		if (EndVFSArray == Tmp) EndVFSArray = Tmp->Prev;


		if (Tmp->Next != 0) Tmp->Next->Prev = Tmp->Prev;
			else if (Tmp->Prev != 0) Tmp->Prev->Next = 0;
		if (Tmp->Prev != 0) Tmp->Prev->Next = Tmp->Next;
			else if (Tmp->Next != 0) Tmp->Next->Prev = 0;

		delete Tmp;
		Tmp = Tmp1;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем указатели в исходное состояние
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	StarVFSArray = 0;
	EndVFSArray = 0;



	eVFS *TempVFS = StartVFS;
	while (TempVFS != 0)
	{
		eVFS *TempVFS1 = TempVFS->Next;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// если VFS файл открыт, закрываем его
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (TempVFS->File != NULL)
		{
			SDL_RWclose(TempVFS->File);
			TempVFS->File = 0;
		}



		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// освобождаем пямять в VFSFileName и пишем сообщение о закрытии
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		if (TempVFS->FileName != 0)
		{
			printf("VFS file was closed %s\n", TempVFS->FileName);
			delete [] TempVFS->FileName; TempVFS->FileName = 0;
		}

		TempVFS = TempVFS1;
	}

	StartVFS = 0;
	EndVFS = 0;
}






//------------------------------------------------------------------------------------
// Завершаем работу с VFS
//------------------------------------------------------------------------------------
void vw_ShutdownVFS(void)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// закрываем все открытые файлы
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (StartFileVFS != 0)
	{
		// берем указатель на первый файл
		eFILE *Tmp = StartFileVFS;
		// просматриваем все файлы
		while (Tmp != 0)
		{
			eFILE *Tmp1 = Tmp->Next;
			vw_fclose(Tmp);
			Tmp = Tmp1;
		}
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем указатели в исходное состояние
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	StartFileVFS = 0;
	EndFileVFS = 0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Закрываем VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	vw_CloseVFS();
}









//------------------------------------------------------------------------------------
// создаем "симлинк" внутри VFS
//------------------------------------------------------------------------------------
bool vw_CreateEntryLinkVFS(const char *FileName, const char *FileNameLink)
{
	// проверяем, если уже есть такой файл или линк - выходим
	eVFS_Entry *Tmp = StarVFSArray;
	while (Tmp != 0)
	{
		eVFS_Entry *Tmp1 = Tmp->Next;
		if (vw_strcmp(Tmp->Name, FileNameLink) == 0)
		{
			// нашли, такой файл или симлинк уже есть в системе
			fprintf(stderr, "VFS link creation error, file or link already present: %s\n", FileNameLink);
			return false;
		}
		Tmp = Tmp1;
	}


	// находим запись с соотв. RealName и меняем Name
	eVFS_Entry *OutputEntry = 0;
	Tmp = StarVFSArray;
	while (Tmp != 0 && OutputEntry == 0)
	{
		eVFS_Entry *Tmp1 = Tmp->Next;
		if (vw_strcmp(Tmp->Name, FileName) == 0)
		{
			OutputEntry = Tmp;

			// добавляем новую eVFS_Entry запись в конец списка
			eVFS_Entry *NewTemp = 0;
			NewTemp = new eVFS_Entry;

			NewTemp->Link = true;

			NewTemp->NameLen = strlen(FileNameLink)+1;
			NewTemp->Name = new char[NewTemp->NameLen];
			strcpy(NewTemp->Name, FileNameLink);

			NewTemp->Offset = OutputEntry->Offset;
			NewTemp->Length = OutputEntry->Length;
			NewTemp->RealLength = OutputEntry->RealLength;
			NewTemp->Parent = OutputEntry->Parent;

			NewTemp->ArhKeyLen = OutputEntry->ArhKeyLen;
			NewTemp->ArhKey = new char[NewTemp->ArhKeyLen];
			strcpy(NewTemp->ArhKey, OutputEntry->ArhKey);

			// добавляем запись в конец списка
			NewTemp->Prev = EndVFSArray;
			NewTemp->Next = 0;
			EndVFSArray->Next = NewTemp;
			EndVFSArray = NewTemp;


			printf("VFS link created: %s > %s\n", FileName, FileNameLink);
			return true;
		}
		Tmp = Tmp1;
	}


	// не нашли ничего
	fprintf(stderr, "VFS link creation error, file not found: %s\n", FileName);
	return false;
}




//------------------------------------------------------------------------------------
// удаляем "симлинк" внутри VFS
//------------------------------------------------------------------------------------
bool vw_DeleteEntryLinkVFS(const char *FileNameLink)
{

	// проверяем, если уже есть такой файл или линк - выходим
	eVFS_Entry *Tmp = StarVFSArray;
	while (Tmp != 0)
	{
		eVFS_Entry *Tmp1 = Tmp->Next;
		if (vw_strcmp(Tmp->Name, FileNameLink) == 0)
		{
			if (Tmp->Link)
			{

				if (Tmp->Name != 0) {delete [] Tmp->Name; Tmp->Name = 0;}
				if (Tmp->ArhKey != 0) {delete [] Tmp->ArhKey; Tmp->ArhKey = 0;}

				// переустанавливаем указатели...
				if (StarVFSArray == Tmp) StarVFSArray = Tmp->Next;
				if (EndVFSArray == Tmp) EndVFSArray = Tmp->Prev;

				if (Tmp->Next != 0) Tmp->Next->Prev = Tmp->Prev;
					else if (Tmp->Prev != 0) Tmp->Prev->Next = 0;
				if (Tmp->Prev != 0) Tmp->Prev->Next = Tmp->Next;
					else if (Tmp->Next != 0) Tmp->Next->Prev = 0;

				delete Tmp;

				printf("VFS link deleted: %s\n", FileNameLink);
				return true;

			}
			else
			{
				fprintf(stderr, "VFS link deletion error, can not delete file: %s\n", FileNameLink);
				return false;
			}
		}


		Tmp = Tmp1;
	}


	// не нашли ничего
	fprintf(stderr, "VFS link deletion error, link not found: %s\n", FileNameLink);
	return false;
}






//------------------------------------------------------------------------------------
// удаляем все "симлинки" из VFS
//------------------------------------------------------------------------------------
bool vw_DeleteAllLinksVFS()
{

	// проверяем, если уже есть такой файл или линк - выходим
	eVFS_Entry *Tmp = StarVFSArray;
	while (Tmp != 0)
	{
		eVFS_Entry *Tmp1 = Tmp->Next;

		if (Tmp->Link)
		{
			if (Tmp->Name != 0) printf("VFS link deleted: %s\n", Tmp->Name);

			if (Tmp->Name != 0) {delete [] Tmp->Name; Tmp->Name = 0;}
			if (Tmp->ArhKey != 0) {delete [] Tmp->ArhKey; Tmp->ArhKey = 0;}

			// переустанавливаем указатели...
			if (StarVFSArray == Tmp) StarVFSArray = Tmp->Next;
			if (EndVFSArray == Tmp) EndVFSArray = Tmp->Prev;

			if (Tmp->Next != 0) Tmp->Next->Prev = Tmp->Prev;
				else if (Tmp->Prev != 0) Tmp->Prev->Next = 0;
			if (Tmp->Prev != 0) Tmp->Prev->Next = Tmp->Next;
				else if (Tmp->Next != 0) Tmp->Next->Prev = 0;

			delete Tmp;
		}

		Tmp = Tmp1;
	}


	return true;
}

















//------------------------------------------------------------------------------------
// Проверка наличия файла в VFS или FS
//------------------------------------------------------------------------------------
int FileDetect(const char *FileName)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// поиск в VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	eVFS_Entry *Tmp = StarVFSArray;
	while (Tmp != 0)
	{
		eVFS_Entry *Tmp1 = Tmp->Next;

		if(vw_strcmp(FileName, Tmp->Name) == 0)
		{
			// нашли
			return VFS_FILE_VFS;
		}

		Tmp = Tmp1;
	}




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// поиск в FS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SDL_RWops *temp = SDL_RWFromFile(FileName, "rb");
	// смогли открыть файл
	if (temp != NULL)
	{
		SDL_RWclose(temp);
        return VFS_FILE_FS;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Файл не найден
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	return -1;
}










//------------------------------------------------------------------------------------
// Открытие файла в VFS (или подгонка файла FS к классу VFS)
//------------------------------------------------------------------------------------
extern char ProgrammDir[MAX_PATH];
eFILE *vw_fopen(const char *FileName)
{


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверяем наличие файла в системе
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int ResTMP = FileDetect(FileName);


	if (ResTMP == -1)
	{
		fprintf(stderr, "Can't find the file %s\n", FileName);
		goto FOP_ERR;
	}


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// файл в FS , подготовка данных и передача указателя на структуру
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (ResTMP == VFS_FILE_FS)
	{
		// Начальная подготовка структуры списка...
		eFILE *Temp = 0;
		Temp = new eFILE; if (Temp == 0) return 0;

		// возможно используется многопоточность, смотрим чтобы не портить данные
		while (fileIObusy) SDL_Delay(0);
		// говорим, что работаем с это процедурой
		fileIObusy = true;

		// первый в списке...
		if (EndFileVFS == 0)
		{
			Temp->Prev = 0;
			Temp->Next = 0;
			StartFileVFS = Temp;
			EndFileVFS = Temp;
		}
		else // продолжаем заполнение...
		{
			Temp->Prev = EndFileVFS;
			Temp->Next = 0;
			EndFileVFS->Next = Temp;
			EndFileVFS = Temp;
		}

		fileIObusy = false;

		Temp->PackLength = 0;
		Temp->Pos = 0;
		Temp->RealLength = 0;
		Temp->VFS_Offset = 0;

		// заносим имя файла
		Temp->Name = 0;
		Temp->Name = new char[strlen(FileName)+1]; if (Temp->Name == 0) return 0;
		strcpy(Temp->Name, FileName);

		// открываем физ. файл, проверка в детекте...
		SDL_RWops * fTEMP = SDL_RWFromFile(FileName, "rb");

		// получаем размер файла
		SDL_RWseek(fTEMP, 0, SEEK_END);
		Temp->RealLength = SDL_RWtell(fTEMP);
		Temp->PackLength = Temp->RealLength;
		SDL_RWseek(fTEMP, 0, SEEK_SET);

		// резервируем память
		Temp->Data = 0;
		Temp->Data = new BYTE[Temp->RealLength]; if (Temp->Data == 0) return 0;

		// плолучаем данные...
		SDL_RWread(fTEMP, Temp->Data, 1, Temp->RealLength);
		SDL_RWclose(fTEMP);

		// передаем указатель на структуру...
		return Temp;
	}


    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// файл в VFS , подготовка данных и передача указателя на структуру
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ResTMP == VFS_FILE_VFS)
	{
		// Начальная подготовка структуры списка...
		eFILE *Temp = 0;
		Temp = new eFILE; if (Temp == 0) return 0;

		// возможно используется многопоточность, смотрим чтобы не портить данные
		while (fileIObusy) SDL_Delay(0);
		// говорим, что работаем с это процедурой
		fileIObusy = true;

		// первый в списке...
		if (EndFileVFS == 0)
		{
			Temp->Prev = 0;
			Temp->Next = 0;
			StartFileVFS = Temp;
			EndFileVFS = Temp;
		}
		else // продолжаем заполнение...
		{
			Temp->Prev = EndFileVFS;
			Temp->Next = 0;
			EndFileVFS->Next = Temp;
			EndFileVFS = Temp;
		}

		fileIObusy = false;

		Temp->PackLength = 0;
		Temp->Pos = 0;
		Temp->RealLength = 0;
		Temp->VFS_Offset = 0;

		Temp->Name = 0;
		Temp->Name = new char[strlen(FileName)+1]; if (Temp->Name == 0) return 0;
		strcpy(Temp->Name, FileName);



		// находим указатель на запись в таблице файлов VFS
		eVFS_Entry *OutputFile = 0;
		eVFS_Entry *Tmp = StarVFSArray;
		while (Tmp != 0 && OutputFile == 0)
		{
			eVFS_Entry *Tmp1 = Tmp->Next;

			if(vw_strcmp(Temp->Name, Tmp->Name) == 0)
			{
				// нашли
				OutputFile = Tmp;
			}

			Tmp = Tmp1;
		}
		if (OutputFile == 0) return 0;


		Temp->PackLength = OutputFile->Length;
		Temp->VFS_Offset = OutputFile->Offset;

		// резервируем память
		Temp->Data = 0;
		Temp->Data = new BYTE[Temp->PackLength]; if (Temp->Data == 0) return 0;


		// возможно используется многопоточность, смотрим чтобы не портить данные
		while (VFSfileIObusy) SDL_Delay(0);
		// говорим, что работаем с это процедурой
		VFSfileIObusy = true;

		// плолучаем данные...
		SDL_RWseek(OutputFile->Parent->File, Temp->VFS_Offset, SEEK_SET);
		SDL_RWread(OutputFile->Parent->File, Temp->Data, Temp->PackLength, 1);
		Temp->RealLength = Temp->PackLength;

		VFSfileIObusy = false;



		// используем сжатие?
		BYTE *srcVFS = 0;
		BYTE *dstVFS = 0;
		int ssizeVFS = 0;
		int dsizeVFS = 0;

		if (!((OutputFile->ArhKey[0]=='0')&(strlen(OutputFile->ArhKey)==1)))
		{
			for (size_t i=0; i<OutputFile->ArhKeyLen;i++)
			{
				char S = OutputFile->ArhKey[i];
				size_t cur_i = OutputFile->ArhKeyLen-1;

				if (S == VFS_DATA_ARH_RLE)
				{
					ssizeVFS = Temp->RealLength;
					srcVFS = Temp->Data;

					if (i == cur_i)
						vw_RLEtoDATA(OutputFile->RealLength, &dstVFS, srcVFS, &dsizeVFS, ssizeVFS);
					else
						vw_RLEtoDATA(0, &dstVFS, srcVFS, &dsizeVFS, ssizeVFS);
					srcVFS = 0;
					Temp->RealLength = dsizeVFS;
					if (Temp->Data != 0){delete [] Temp->Data; Temp->Data = 0;}
					Temp->Data = dstVFS;
					dstVFS = 0;
				}
				if (S == VFS_DATA_ARH_HAFF)
				{
					ssizeVFS = Temp->RealLength;
					srcVFS = Temp->Data;

					if (i == cur_i)
						vw_HAFFtoDATA(OutputFile->RealLength, &dstVFS, srcVFS, &dsizeVFS, ssizeVFS);
					else
						vw_HAFFtoDATA(0, &dstVFS, srcVFS, &dsizeVFS, ssizeVFS);
					srcVFS = 0;
					Temp->RealLength = dsizeVFS;
					if (Temp->Data != 0){delete [] Temp->Data; Temp->Data = 0;}
					Temp->Data = dstVFS;
					dstVFS = 0;
				}
			}
		}

		// передаем указатель на структуру...
		return Temp;
	}



    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// в системе нет такого файла, или какие-то проблемы
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FOP_ERR:
	fileIObusy = false;
	return 0;
}










//------------------------------------------------------------------------------------
// Закрытие файла в VFS
//------------------------------------------------------------------------------------
int vw_fclose(eFILE *stream)
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// файл не открыли...закрывать нечего...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (stream == 0) return -1;

	// возможно используется многопоточность, смотрим чтобы не портить данные
	while (fileIObusy) SDL_Delay(0);
	// говорим, что работаем с это процедурой
	fileIObusy = true;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Освобождаем память
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (stream->Data != 0) {delete [] stream->Data; stream->Data = 0;}
	if (stream->Name != 0) {delete [] stream->Name; stream->Name = 0;}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Устанавливаем все у начальное состояние
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    stream->PackLength = 0;
	stream->Pos = 0;
	stream->RealLength = 0;
	stream->VFS_Offset =0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// переустанавливаем указатели
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	if (StartFileVFS == stream) StartFileVFS = stream->Next;
	if (EndFileVFS == stream) EndFileVFS = stream->Prev;

	if (stream->Next != 0) stream->Next->Prev = stream->Prev;
		else if (stream->Prev != 0) stream->Prev->Next = 0;
	if (stream->Prev != 0) stream->Prev->Next = stream->Next;
		else if (stream->Next != 0) stream->Next->Prev = 0;

	if (stream != 0){delete stream; stream = 0;}


	fileIObusy = false;
	return 0;
}

















//------------------------------------------------------------------------------------
// чтение блока данных из файла в VFS
//------------------------------------------------------------------------------------
int eFILE::fread(void *buffer, size_t size, size_t count)
{
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// проверки
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (buffer == 0) return -1;
	if (Data == 0) return -1;

	// Кол-во успешно считанных раз
	int SecCount = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// считываем данные
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (size_t i=0; i<count; i++)
	{
		// если можем прочитать следующий блок size размера - читаем
		if (size <= (unsigned int)(RealLength - Pos))
		{
			// считываем байты
			memcpy((BYTE*)buffer + SecCount*size, Data + Pos, size);
			// передвигаем текущий указатель
			Pos += size;
			// устанавливаем успешное чтение
			SecCount++;
		}
		// иначе, уже достигли конца файла - выходим из цикла
		else break;
	}

	return SecCount;
}






//------------------------------------------------------------------------------------
// поиск (установка текущей позиции)
//------------------------------------------------------------------------------------
int eFILE::fseek(long offset, int origin)
{
	// изменяем значение указателя, согласно origin...
	switch (origin)
	{
		case SEEK_CUR:
		{
			if (Pos + offset > RealLength) return 1;
			Pos += offset;
			break;
		}
		case SEEK_END:
		{
			if (RealLength-offset < 0) return 1;
			Pos = RealLength-offset;
			break;
		}
		case SEEK_SET:
		{
			if (offset < 0 || offset > RealLength) return 1;
			Pos = offset;
			break;
		}
	}

	return 0;
}




//------------------------------------------------------------------------------------
// получить текущую позицию указателя
//------------------------------------------------------------------------------------
long eFILE::ftell()
{
	return Pos;
}




//------------------------------------------------------------------------------------
// узнаем, достигли конца файла или нет
//------------------------------------------------------------------------------------
int eFILE::feof()
{
	// если указатель больше или равен длине файла - значит он уже за массивом данных файла
	if (Pos >= RealLength-1) return 1;
	// иначе - все ок, еще можем читать файл
	return 0;
}




//------------------------------------------------------------------------------------
// Get string from stream
//------------------------------------------------------------------------------------
char *eFILE::fgets(char *str, int num)
{
	// проверка первая, не читаем больше, чем у нас длина файла
	if (Pos+num >= RealLength-1) num = (RealLength-1)-Pos;

	for (int i=0; i<num; i++)
	{
		str[i] = Data[Pos];
		Pos++;
		// конец строки, нужно выйти
		if (Data[Pos-1] == '\n') return str;
	}

	return str;
}





