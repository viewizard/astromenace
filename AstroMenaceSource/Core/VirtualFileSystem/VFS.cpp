/******************************************************************************

	This source file is part of Viewizard Game Engine
	For the latest info, see http://www.viewizard.com/

	File name: VFS.cpp

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
#include "../Math/Math.h"


// Список подключенных VFS файлов
struct eVFS
{
	char *FileName;		// Имя файла VFS
	SDL_RWops *File;	// Указатель на файл виртуальной системы

	eVFS*	Prev;
	eVFS*	Next;
};
eVFS *StarVFS = 0;
eVFS *EndVFS = 0;





// Список файлов, доступных в подключенных (открытых) VFS
struct eVFS_Entry
{
	DWORD	NameLen;	// Кол-во байт в имени...
	char	*Name;		// Имя записи (имя файла)
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
// кодируем-декодируем
//------------------------------------------------------------------------------------
int XORCount = 0;
char XORKey[] = "VASW%YTgbsbnj9243itmwreGFGRHWHTGGFNWASSGSHGmkwoy245i5hykjlgsajnqiot403wigunwrewg-3940514342";
void VFSCodeXOR(char *Text, int Size)
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
// Открытие (подключение) VFS
//------------------------------------------------------------------------------------
int vw_OpenVFS(const char *Name)
{
	XORCount = 0;
	int POS = 0; // указатель позиции в буфере.
	BYTE *buff = 0;
	int DataStartOffsetVFS = 0;
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
        printf("Can't find VFS file %s\n", Name);
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
		StarVFS = TempVFS;
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
	char Sign[4];
	if(SDL_RWread(TempVFS->File, &Sign, 4, 1) == -1)
	{
		// если файл меньше, значит ошибка
		printf("VFS file size error %s\n", Name);
		goto vw_OpenVFS_Error;
	}
	if (strncmp(Sign, "VFS_", 4) != 0)
	{
		// нет сигнатуры
		printf("VFS file header error %s\n", Name);
		goto vw_OpenVFS_Error;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Bерсия - 4 байт
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	char Version[4];
	if(SDL_RWread(TempVFS->File, &Version, 4, 1) == -1)
	{
		printf("VFS file has wrong version or corrupted %s\n", Name);
		goto vw_OpenVFS_Error;
	}
	if (strncmp(Version, VFS_VER, 4) != 0)
	{
		printf("VFS file has wrong version %s\n", Name);
		goto vw_OpenVFS_Error;
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
		VFSCodeXOR(Temp->Name, Temp->NameLen);
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

		Tmp = Tmp1;
	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// устанавливаем указатели в исходное состояние
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	StarVFSArray = 0;
	EndVFSArray = 0;



	eVFS *TempVFS = StarVFS;
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

	// если в линуксе, проверяем и полный путь тоже
	// возможно, не открыт файл вфс, или файла в нем нет
#ifdef __unix
	if (ResTMP == -1)
	{
		char *TmpFileName;
		TmpFileName = new char[MAX_PATH]; if (TmpFileName == 0) return 0;
		strcpy(TmpFileName, ProgrammDir);
		strcat(TmpFileName, FileName);
		ResTMP = FileDetect(TmpFileName);
		// если нет и тут, выходим
		if (ResTMP == -1)
		{
			fprintf(stderr, "Can't find the file %s\n", FileName);
			if (TmpFileName != 0){delete [] TmpFileName; TmpFileName = 0;}
			goto FOP_ERR;
		}

		// открываем файл сами

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
		SDL_RWops * fTEMP = SDL_RWFromFile(TmpFileName, "rb");

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
		if (TmpFileName != 0){delete [] TmpFileName; TmpFileName = 0;}
		return Temp;

	}
#endif // unix

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
		if (size <= RealLength - Pos)
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





