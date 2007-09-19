/////////////////////////////////////////////////
// Viewizard Game Engine
/////////////////////////////////////////////////
// File: VFS.cpp
/////////////////////////////////////////////////
// Copyright (c) 2001-2006, Michael Kurinnoy
// All Rights Reserved.
/////////////////////////////////////////////////
// Ver : 2.0
/////////////////////////////////////////////////


//------------------------------------------------------------------------------------
// include и lib...
//------------------------------------------------------------------------------------
#include "vfs_var.h"




//------------------------------------------------------------------------------------
// кодируем-декодируем
//------------------------------------------------------------------------------------
int XORCount = 0;
char XORKey[] = "VASW%YTgbsbnj9243itmwreGFGRHWHTGGFNWASSGSHGmkwoy245i5hykjlgsajnqiot403wigunwrewg-3940514342";
void CodeXOR(char *Text, int Size)
{
	for (int i=0; i < Size; i++)
	{
		Text[i] = Text[i]^XORKey[XORCount];
		XORCount++;
		// чтобы не было переполнения
		if (XORCount >= (int)strlen(XORKey)) XORCount=0;
	}
}






//------------------------------------------------------------------------------------
// Создание VFS
//------------------------------------------------------------------------------------
int vw_CreateVFS(const char *Name, const char *nArhKey, bool WriteArhKey, int Ver)
{
	XORCount = 0;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выделяем память для имени
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	VFSFileName = new char[strlen(Name)+1];
	// не можем выделить память
	if (VFSFileName == 0) return 0;
	strcpy_s(VFSFileName, strlen(Name)+1, Name);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// открываем (создаем) на чтение-запись...
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// не можем записать
	if (fopen_s(&VFSFile, VFSFileName, "wb+") != 0) return 0;
	VFSFileWritable = true;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// выделяем память для ArhKeyVFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	ArhKeyVFS = new char[strlen(nArhKey)+1];
	if (ArhKeyVFS == 0) return 0;
	strcpy_s(ArhKeyVFS, strlen(nArhKey)+1, nArhKey);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// пишем VFS_ (4б)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	char tmp1[5] = "VFS_";
	fwrite(tmp1,4,1,VFSFile);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// пишем версию (4б)
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	VFSversion = Ver;
	if (VFSversion == 3)
		fwrite("v1.3",4,1,VFSFile);
	if (VFSversion == 2)
		fwrite("v1.2",4,1,VFSFile);
	if (VFSversion == 1)
		fwrite("v1.1",4,1,VFSFile);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Пишем (если нужно) последовательность упаковки
	// 1-й байт - сколько символов в упаков.послед. 0 - если ее вообще нет
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (VFSversion == 1 || VFSversion == 2)
	{
		if ((!WriteArhKey) | ((ArhKeyVFS[0]=='0')&(strlen(ArhKeyVFS)==1)) )
		{
			// ничего писать не нужно, пишем 0 в длину ArhKeyVFS
			byte tmp3 = 0;
			fwrite(&tmp3,1,1,VFSFile);
			// находим значения смещений для записи данных и файловой таблици
			HeaderOffsetVFS = 4+4+1+4+4;//VFS v offset lenght
			DataStartOffsetVFS = 4+4+1;

		}
		else
		{
			// Пишем последовательность...
			byte tmp3 = (byte)strlen(ArhKeyVFS);
			// пишем, какова длина ArhKeyVFS
			fwrite(&tmp3,1,1,VFSFile);
			// пишем ArhKeyVFS
			fwrite(ArhKeyVFS,strlen(ArhKeyVFS),1,VFSFile);
			// находим значения смещений для записи данных и файловой таблици
			HeaderOffsetVFS = 4+4+1+(int)strlen(ArhKeyVFS)+4+4;//VFS ver offset lenght
			DataStartOffsetVFS = 4+4+1+(int)strlen(ArhKeyVFS);
		}
	}
	if (VFSversion == 3)
	{
		HeaderOffsetVFS = 4+4+4+4;//VFS ver offset lenght
		DataStartOffsetVFS = 4+4;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// пишем смещение начала таблицы
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	NumberOfFilesVFS = 0;
	HeaderLengthVFS = 0;
	fwrite(&HeaderOffsetVFS,4,1,VFSFile);
	fwrite(&HeaderLengthVFS,4,1,VFSFile);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// VFS подключен - можно работать
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	VFSFileOpen=true;  

	return 0;
}









//------------------------------------------------------------------------------------
// Закрываем VFS
//------------------------------------------------------------------------------------
void vw_CloseVFS(void)
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если VFS файл открыт, закрываем его 
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (VFSFileOpen)
	{
		fclose(VFSFile);
		VFSFileWritable = false;
		VFSFileOpen = false;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// даже если файла не, все равно освобождаем память 
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (int j = 0; j<NumberOfFilesVFS; j++)
	{ 
		if (VFSdirArray[j].Name != 0) delete [] VFSdirArray[j].Name; 
		VFSdirArray[j].Name = 0; 
		/*if (VFSversion == 3)
		{
			if (VFSdirArray[j].ArhKey != 0) delete [] VFSdirArray[j].ArhKey;
			VFSdirArray[j].ArhKey = 0;
		}*/
	}
	if (VFSdirArray != 0) 
	{ delete [] VFSdirArray; VFSdirArray = 0; }




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// освобождаем память в ArhKeyVFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (ArhKeyVFS != 0)
	{	delete [] ArhKeyVFS; ArhKeyVFS = 0;	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// установка значений в исходные состояния
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	SetUpperVFS = false;
	HeaderOffsetVFS = 0;
	DataStartOffsetVFS = 0;
	HeaderLengthVFS = 0;
	NumberOfFilesVFS = 0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// освобождаем пямять в VFSFileName и пишем сообщение о закрытии
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (VFSFileName != 0)
	{
		delete [] VFSFileName; VFSFileName = 0;
	}
}







//------------------------------------------------------------------------------------
// Запись файла из FS в VFS 
//------------------------------------------------------------------------------------
int vw_CopyFStoVFS(const char *SrcFileName, const char *DstFileName)
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Если VFS файл не открыт - нам тут делать нечего
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!VFSFileOpen) return 0;
	if (!VFSFileWritable) return 0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// читаем данные файла в буфер
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	FILE *Ftmp = 0;
	// Если файл не найден - выходим
	if(fopen_s(&Ftmp, SrcFileName, "rb") != 0) return 0; 


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// получаем размер файла
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    fseek(Ftmp, 0, SEEK_END);
    int tmpLength = ftell(Ftmp);
    fseek(Ftmp, 0, SEEK_SET);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// копируем все данные файла в массив
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	byte *tmp = 0;
	tmp = new byte[tmpLength];
	// не можем выделить память - выходим
	if (tmp == 0) return 0;
	fread(tmp,tmpLength,1,Ftmp);
	fclose(Ftmp);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// запись в VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (0 != vw_WriteIntoVFS(DstFileName, tmp, tmpLength))
	{
		// какая-то ошибка, не можем записать в VFS
		delete [] tmp; tmp = 0;
		return 0;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Освобождаем память
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	delete [] tmp; tmp = 0;

	return 0;
}






int CheckCompression(int tmpLength, const byte *buffer)
{
	byte *tmp = 0;
	tmp = new byte[tmpLength];
	memcpy(tmp, buffer, tmpLength);

	// устанавливаем данные
	ssizeVFS = tmpLength;
	dsizeVFS = tmpLength;
	srcVFS = tmp;
	tmp = 0;
		
	// цикл по кол-ву примененных методов сжатия в ArhKeyVFS
	for (unsigned int i=0; i<strlen(ArhKeyVFS);i++)
	{
		// находим, какой текущий метод сжатия	
		char S = ArhKeyVFS[strlen(ArhKeyVFS)-i-1];

		// если RLE
		if (S == VFS_DATA_ARH_RLE)
		{
			vw_DATAtoRLE();
			delete [] srcVFS; srcVFS = 0;
			srcVFS = dstVFS;
			ssizeVFS = dsizeVFS;
			dstVFS = 0;
		}
		// если HAFF
		if (S == VFS_DATA_ARH_HAFF)
		{
			vw_DATAtoHAFF();
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
// запись данных в VFS
//------------------------------------------------------------------------------------
int	vw_WriteIntoVFS(const char *Name, const byte * buffer, int size)
{

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Если VFS файл не открыт - нам тут делать нечего
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!VFSFileOpen) return 0;
	if (!VFSFileWritable) return 0;
	if (buffer == 0) return 0;




	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// получаем размер файла
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int RealLength = size;
	int tmpLength = size;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// копируем все данные в массив
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	byte *tmp = 0;
	tmp = new byte[tmpLength];
	// не можем выделить память - выходим
	if (tmp == 0) return 0;
	memcpy(tmp, buffer, size);







	// для версии 1.3 вот тут надо составлять свою ArhKeyVFS
	if (VFSversion == 3)
	{
		// 0 - 0
		int BestMode = 0;
		int BestSize = tmpLength;
		int TmpSize = 0;


		// 1 - 1
		if (ArhKeyVFS != 0){delete [] ArhKeyVFS; ArhKeyVFS = 0;}
		ArhKeyVFS = new char[1+1];
		ArhKeyVFS[0] = '1';
		ArhKeyVFS[1] = 0;
		TmpSize = CheckCompression(tmpLength, buffer);
		if (TmpSize<BestSize)
		{
			BestMode = 1;
			BestSize = TmpSize;
		}
/*
		// 2 - 2
		if (ArhKeyVFS != 0){delete [] ArhKeyVFS; ArhKeyVFS = 0;}
		ArhKeyVFS = new char[1+1];
		ArhKeyVFS[0] = '2';
		ArhKeyVFS[1] = 0;
		TmpSize = CheckCompression(tmpLength, buffer);
		if (TmpSize<BestSize)
		{
			BestMode = 2;
			BestSize = TmpSize;
		}

		// 3 - 12
		if (ArhKeyVFS != 0){delete [] ArhKeyVFS; ArhKeyVFS = 0;}
		ArhKeyVFS = new char[2+1];
		ArhKeyVFS[0] = '1';
		ArhKeyVFS[0] = '2';
		ArhKeyVFS[1] = 0;
		TmpSize = CheckCompression(tmpLength, buffer);
		if (TmpSize<BestSize)
		{
			BestMode = 3;
			BestSize = TmpSize;
		}

		// 4 - 21
		if (ArhKeyVFS != 0){delete [] ArhKeyVFS; ArhKeyVFS = 0;}
		ArhKeyVFS = new char[2+1];
		ArhKeyVFS[0] = '2';
		ArhKeyVFS[0] = '1';
		ArhKeyVFS[1] = 0;
		TmpSize = CheckCompression(tmpLength, buffer);
		if (TmpSize<BestSize)
		{
			BestMode = 3;
			BestSize = TmpSize;
		}
*/

		// для текстовых файлов принудительно ставим хаффмана
		const char *Extension = strrchr(Name,'.');
		if (!strcmp(Extension, ".xml") || 
			!strcmp(Extension, ".vert") || 
			!strcmp(Extension, ".frag"))
		{
			BestMode = 2;
		}





		if (ArhKeyVFS != 0){delete [] ArhKeyVFS; ArhKeyVFS = 0;}

		switch (BestMode)
		{
			case 0:
				ArhKeyVFS = new char[1+1];
				ArhKeyVFS[0] = '0';
				ArhKeyVFS[1] = 0;
				break;
			case 1:
				ArhKeyVFS = new char[1+1];
				ArhKeyVFS[0] = '1';
				ArhKeyVFS[1] = 0;
				break;
			case 2:
				ArhKeyVFS = new char[1+1];
				ArhKeyVFS[0] = '2';
				ArhKeyVFS[1] = 0;
				break;
			case 3:
				ArhKeyVFS = new char[2+1];
				ArhKeyVFS[0] = '1';
				ArhKeyVFS[1] = '2';
				ArhKeyVFS[2] = 0;
				break;
			case 4:
				ArhKeyVFS = new char[2+1];
				ArhKeyVFS[0] = '2';
				ArhKeyVFS[1] = '1';
				ArhKeyVFS[2] = 0;
				break;
		}
	
	}



	










	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// если используется сжатие - запаковываем данные
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (!((ArhKeyVFS[0]=='0')&(strlen(ArhKeyVFS)==1)))
	{
		// устанавливаем данные
		ssizeVFS = tmpLength;
		dsizeVFS = tmpLength;
		srcVFS = tmp;
		tmp = 0;
		
		// цикл по кол-ву примененных методов сжатия в ArhKeyVFS
		for (unsigned int i=0; i<strlen(ArhKeyVFS);i++)
		{
			// находим, какой текущий метод сжатия	
			char S = ArhKeyVFS[strlen(ArhKeyVFS)-i-1];

			// если RLE
			if (S == VFS_DATA_ARH_RLE)
			{
				vw_DATAtoRLE();
				delete [] srcVFS; srcVFS = 0;
				srcVFS = dstVFS;
				ssizeVFS = dsizeVFS;
				dstVFS = 0;

			}
			// если HAFF
			if (S == VFS_DATA_ARH_HAFF)
			{
				vw_DATAtoHAFF();
				delete [] srcVFS; srcVFS = 0;
				srcVFS = dstVFS;
				ssizeVFS = dsizeVFS;
				dstVFS = 0;
			}
		}

		// возвращаем данные в исходный массив
		tmp = new byte[dsizeVFS];
		if (tmp == 0) return 0;
		tmpLength = dsizeVFS;
		memcpy(tmp, srcVFS, tmpLength);
		delete [] srcVFS; srcVFS = 0;
	}



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// пишем данные в VFS
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	fseek(VFSFile, HeaderOffsetVFS, SEEK_SET);
	fwrite(tmp, tmpLength, 1, VFSFile);
	delete [] tmp; tmp = 0;



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// переписываем данные таблицы VFSdirArray
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	XORCount = 0;
	for (int i = 0; i<NumberOfFilesVFS; i++)
	{
		if (VFSversion == 3)
		{
			fwrite(&VFSdirArray[i].ArhKeyLen, 1, 1, VFSFile);
			if (VFSdirArray[i].ArhKeyLen > 0)
				fwrite(VFSdirArray[i].ArhKey, VFSdirArray[i].ArhKeyLen, 1, VFSFile);
		}
		fwrite(&VFSdirArray[i].NameLen, 2, 1, VFSFile);
		if (VFSversion == 3) CodeXOR(VFSdirArray[i].Name, VFSdirArray[i].NameLen);
		fwrite(VFSdirArray[i].Name, VFSdirArray[i].NameLen, 1, VFSFile);
		fwrite(&VFSdirArray[i].Offset, 4, 1, VFSFile);
		fwrite(&VFSdirArray[i].Length, 4, 1, VFSFile);
		if (VFSversion == 2 || VFSversion == 3) fwrite(&VFSdirArray[i].RealLength, 4, 1, VFSFile);
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// дописываем новую запись в таблицу файла
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int lenArhKeyVFS=0;
	if (VFSversion == 3)
	{
		if (ArhKeyVFS[0]=='0' && (strlen(ArhKeyVFS) == 1))
		{
			// ничего писать не нужно, пишем 0 в длину ArhKeyVFS
			byte tmp3 = 0;
			fwrite(&tmp3,1,1,VFSFile);
			lenArhKeyVFS = 1;
		}
		else
		{
			// Пишем последовательность...
			byte tmp3 = (byte)strlen(ArhKeyVFS);
			// пишем, какова длина ArhKeyVFS
			fwrite(&tmp3,1,1,VFSFile);
			// пишем ArhKeyVFS
			fwrite(ArhKeyVFS,strlen(ArhKeyVFS),1,VFSFile);
			lenArhKeyVFS = (int)strlen(ArhKeyVFS)+1;
		}
	}
	DWORD len = (DWORD)strlen(Name);
	fwrite(&len, 2, 1, VFSFile);
	char FileName[MAX_PATH];
	strcpy(FileName, Name);
	if (VFSversion == 3) CodeXOR(FileName, len);
	fwrite(FileName, len, 1, VFSFile);
	fwrite(&HeaderOffsetVFS, 4, 1, VFSFile);
	fwrite(&tmpLength, 4, 1, VFSFile);
	if (VFSversion == 2 || VFSversion == 3) fwrite(&RealLength, 4, 1, VFSFile);


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// меняем данные
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	HeaderOffsetVFS += tmpLength;
	if (VFSversion == 2 || VFSversion == 3)
		HeaderLengthVFS += lenArhKeyVFS+2+len+4+4+4;
	if (VFSversion == 1)
		HeaderLengthVFS += 2+len+4+4;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// переписываем смещение начала таблицы
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	fseek( VFSFile, DataStartOffsetVFS, SEEK_SET );
	fwrite(&HeaderOffsetVFS, 4, 1, VFSFile);	
	fwrite(&HeaderLengthVFS, 4, 1, VFSFile);



	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// удаляем старый VFSdirArray
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (int j = 0; j<NumberOfFilesVFS; j++)
	{
		delete [] VFSdirArray[j].Name;
		VFSdirArray[j].Name = 0;
	}
	delete [] VFSdirArray; VFSdirArray = 0; 
		

	// т.к. дописали один файл
	NumberOfFilesVFS++;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Резервируем память
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	VFSdirArray = new eVFS_Entry[ NumberOfFilesVFS ];
	if (VFSdirArray == 0) return 0;


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Заполняем таблицу
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	fseek( VFSFile, HeaderOffsetVFS, SEEK_SET );
	XORCount = 0;
	for (int i=0; i<NumberOfFilesVFS; i++)
	{
		if (VFSversion == 3)
		{
			fread( &VFSdirArray[i].ArhKeyLen, 1, 1, VFSFile );
			VFSdirArray[i].ArhKey = 0;
			if (VFSdirArray[i].ArhKeyLen > 0)
			{
				VFSdirArray[i].ArhKey = new char[VFSdirArray[i].ArhKeyLen+1];
				VFSdirArray[i].ArhKey[VFSdirArray[i].ArhKeyLen+1] = 0;
				fread( VFSdirArray[i].ArhKey, VFSdirArray[i].ArhKeyLen, 1, VFSFile );
			}
		}
		VFSdirArray[i].NameLen = 0;
		fread( &VFSdirArray[i].NameLen, 2, 1, VFSFile );
		VFSdirArray[i].Name = new char[VFSdirArray[i].NameLen+1];
		VFSdirArray[i].Name[VFSdirArray[i].NameLen] = 0;
		fread( VFSdirArray[i].Name, VFSdirArray[i].NameLen, 1, VFSFile );
		if (VFSversion == 3) CodeXOR(VFSdirArray[i].Name, VFSdirArray[i].NameLen);
		fread( &VFSdirArray[i].Offset, 4, 1, VFSFile );
		fread( &VFSdirArray[i].Length, 4, 1, VFSFile );

		if (VFSversion == 2 || VFSversion == 3) 
		{
			fread( &VFSdirArray[i].RealLength, 4, 1, VFSFile );
		}
	}



	return 0;
}