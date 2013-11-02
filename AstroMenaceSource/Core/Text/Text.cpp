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


#include "Text.h"


struct sCSVRow
{
	char **CellData;
	int Columns;

	sCSVRow *Next;
	sCSVRow *Prev;
};


sCSVRow *StartCSVRow;
sCSVRow *EndCSVRow;

// буфер со всем текстом
char *TextBuffer = 0;

// указатель на таблицу данных по языкам
sLanguageList *LanguageList = 0;
sLanguageList *vw_GetLanguageList(){return LanguageList;};
// кол-во языков
int LanguageListCount = 0;
int vw_GetLanguageListCount(){return LanguageListCount;};
// текущий язык
int CurrentLanguage = 0;




// присоединяем к списку
void vw_AttachCSVRow(sCSVRow* CSVRow)
{
	if (CSVRow == 0) return;

	// первый в списке...
	if (EndCSVRow == 0)
	{
		CSVRow->Prev = 0;
		CSVRow->Next = 0;
		StartCSVRow = CSVRow;
		EndCSVRow = CSVRow;
	}
	else // продолжаем заполнение...
	{
		CSVRow->Prev = EndCSVRow;
		CSVRow->Next = 0;
		EndCSVRow->Next = CSVRow;
		EndCSVRow = CSVRow;
	}
}
// удаляем из списка
void vw_DetachCSVRow(sCSVRow* CSVRow)
{
	if (CSVRow == 0) return;

	// переустанавливаем указатели...
	if (StartCSVRow == CSVRow) StartCSVRow = CSVRow->Next;
	if (EndCSVRow == CSVRow) EndCSVRow = CSVRow->Prev;

	if (CSVRow->Next != 0) CSVRow->Next->Prev = CSVRow->Prev;
		else if (CSVRow->Prev != 0) CSVRow->Prev->Next = 0;
	if (CSVRow->Prev != 0) CSVRow->Prev->Next = CSVRow->Next;
		else if (CSVRow->Next != 0) CSVRow->Next->Prev = 0;
}
// переприсоединяем как первый в списке
void vw_ReAttachCSVRowAsFirst(sCSVRow* CSVRow)
{
	if (CSVRow == 0) return;

	vw_DetachCSVRow(CSVRow);

	// если список пустой - просто присоединяем к списку и все
	if (StartCSVRow == 0)
	{
		vw_AttachCSVRow(CSVRow);
	}
	else
	{
		CSVRow->Next = StartCSVRow;
		CSVRow->Prev = 0;
		StartCSVRow->Prev = CSVRow;
		StartCSVRow = CSVRow;
	}
}








//-----------------------------------------------------------------------------
// освобождаем данные
//-----------------------------------------------------------------------------
void vw_ReleaseText()
{
	// Чистка памяти...
	sCSVRow *Tmp = StartCSVRow;
	while (Tmp != 0)
	{
		sCSVRow *Tmp1 = Tmp->Next;
		if (Tmp->CellData != 0) delete [] Tmp->CellData;
		delete Tmp;
		Tmp = Tmp1;
	}

	StartCSVRow = 0;
	EndCSVRow = 0;

	if (TextBuffer != 0) {delete [] TextBuffer; TextBuffer = 0;}

	CurrentLanguage = 0;
	LanguageListCount = 0;
	delete [] LanguageList;
	LanguageList = 0;
}





//-----------------------------------------------------------------------------
// устанавливаем язык
//-----------------------------------------------------------------------------
void vw_SetTextLanguage(int Language)
{
	CurrentLanguage = Language;
}





//-----------------------------------------------------------------------------
// загружаем текстовый .csv
//-----------------------------------------------------------------------------
int vw_InitText(const char *FileName, const char SymbolSeparator, const char SymbolEndOfLine)
{
	vw_ReleaseText();

	// читаем данные
	eFILE *TempF = vw_fopen(FileName);

	if (TempF == NULL) return -1;

	TempF->fseek(0, SEEK_END);
	int DataLength = TempF->ftell();
	TempF->fseek(0, SEEK_SET);
	TextBuffer = new char[DataLength];
	TempF->fread(TextBuffer, DataLength, 1);
	vw_fclose(TempF);


	// парсим формат .csv (табличный формат, поддерживаемый офисными редакторами)
	// не делаем проверок формата на наличие в файле повреждений структуры
	// вся работа строится на TextBuffer, другой памяти не выделяем - только ссылаемся указателями
	// на данные в этом буфере (предварительно поставив 0 для разграничения текста в ячейках)
	// ! важно - в конце файла должен быть перевод на новую строку, чтобы поставить ноль и сформировать конец строки, заменив символ SymbolEndOfLine

	char *Buffer = TextBuffer;

	// по первой строче считаем сколько у нас в файле столбцов
	int ColumnsCount = 0;
	while(Buffer[0] != SymbolEndOfLine)
	{
		if (Buffer[0] == SymbolSeparator) ColumnsCount++;
		Buffer++;
	}
	ColumnsCount++; // для последнего столбца конечный символ - SymbolEndOfLine, который мы получили в цикле

	// если столбцов менее 2-х - данных недостаточно для работы
	if (ColumnsCount < 2) return -1;

	Buffer = TextBuffer; // восстанавливаем указатель

	// крутим пока не обработали все строки
	while(DataLength > ColumnsCount-1)
	{
		// создаем строку
		sCSVRow *NewCSVRow;
		NewCSVRow = new sCSVRow;

		NewCSVRow->Columns = ColumnsCount;
		NewCSVRow->CellData = new char*[NewCSVRow->Columns];

		for (int i=0; i<NewCSVRow->Columns; i++)
		{
			NewCSVRow->CellData[i] = Buffer;
			// ищем маркер следующего столбца или конца строки
			while((Buffer[0] != SymbolEndOfLine) && (Buffer[0] != SymbolSeparator)) {Buffer++;DataLength--;}
			// нашли, ставим туда ноль, чтобы ноль-терминальная строка была завершенной
			Buffer[0] = 0;
			Buffer++;DataLength--;
		}

		vw_AttachCSVRow(NewCSVRow);
	}


	// для работы с языками, создаем таблицу
	LanguageListCount = ColumnsCount-1;
	LanguageList = new sLanguageList[LanguageListCount];

	for (int i=0; i<LanguageListCount; i++)
	{
		LanguageList[i].code = vw_GetText("0_code", i+1);
		if (LanguageList[i].code == 0) return -2;
		LanguageList[i].title = vw_GetText("0_title", i+1);
		if (LanguageList[i].title == 0) return -2;
	}

	return 0;
}




// проверяем только цифры
int strcmpIdNum(const char *a, const char *b)
{
	// если первые 2 символа одинаковые - значит по номеру 100% совпадают
	if ((a[0] == b[0]) && (a[1] == b[1])) return 0;

	return 1;
}




//-----------------------------------------------------------------------------
// получаем текст из файла
//-----------------------------------------------------------------------------
const char *vw_GetText(const char *ItemID, int Language)
{
	if (TextBuffer == 0) return 0;
	if (ItemID == 0) return 0;
	if (Language < 1 || Language > LanguageListCount) Language = CurrentLanguage;

	sCSVRow *Tmp = StartCSVRow;
	while (Tmp != 0)
	{
		sCSVRow *Tmp1 = Tmp->Next;

		// в первом столбце у нас записан идентификатор текста с 1 или 2-х значным номером
		// не работаем со строками без идентификатора
		if (Tmp->CellData[0][0] != 0)
		if (!strcmpIdNum(Tmp->CellData[0], ItemID))
		if (!vw_strcmp(ItemID, Tmp->CellData[0]))
		{
			// перемещаем его на первое место в списке для ускорения поиска в след. проходах
			vw_ReAttachCSVRowAsFirst(Tmp);

			// возвращаем указатель на нужный столбец
			return Tmp->CellData[Language];
		}

		Tmp = Tmp1;
	}

	fprintf(stderr, "Text not found, ID: %s\n", ItemID);
	return 0;
}




//-----------------------------------------------------------------------------
// проверяем, есть ли символ в фонте, перебираем по тексту всех языков
//-----------------------------------------------------------------------------
int vw_CheckFontCharsInText()
{
	if (TextBuffer == 0) return -1;


	printf("Font characters detection start.\n");


	sCSVRow *Tmp = StartCSVRow;
	while (Tmp != 0)
	{
		sCSVRow *Tmp1 = Tmp->Next;

		for (int i=0; i<Tmp->Columns; i++)
		{
			const char *CharsList = Tmp->CellData[i];

			if (CharsList!=0)
			{
				// перебираем всю строку
				while (strlen(CharsList) > 0)
				{
					unsigned CurrentChar;
					// преобразуем в утф32 и "сдвигаемся" на следующий символ в строке
					CharsList = utf8_to_utf32(CharsList, &CurrentChar);
					// загружаем символ и все необходимые данные для него
					if (!vw_FindFontCharByUTF32(CurrentChar))
					{
						printf("!!! FontChar was not created, Unicode: " );
						if ( CurrentChar < 0x80 && CurrentChar > 0 )
						{
							printf( "%c (0x%04X)\n", (char)CurrentChar, CurrentChar );
						}
						else
						{
							printf( "? (0x%04X)\n", CurrentChar );
						}
					}
				}
			}
		}


		Tmp = Tmp1;
	}



	printf("Font characters detection end.\n\n");

	return 0;
}
