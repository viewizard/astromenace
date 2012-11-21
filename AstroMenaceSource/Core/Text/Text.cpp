/********************************************************************************

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


	Web Site:		http://www.viewizard.com/
	E-mail:			viewizard@viewizard.com
	Postal address:	kvartal Zhukova, 10/79, Lugansk city, 91051, Ukraine


*********************************************************************************/


#include "Text.h"


struct sTextNode
{
	char *id;
	char **text;
	int count;

	sTextNode *Next;
	sTextNode *Prev;
};


sTextNode *StartTextNode;
sTextNode *EndTextNode;

// буфер со всем текстом
char *TextBuffer = 0;

// текущий язык
int CurrentLanguage = 0;




// присоединяем к списку
void vw_AttachTextNode(sTextNode* TextNode)
{
	if (TextNode == 0) return;

	// первый в списке...
	if (EndTextNode == 0)
	{
		TextNode->Prev = 0;
		TextNode->Next = 0;
		StartTextNode = TextNode;
		EndTextNode = TextNode;
	}
	else // продолжаем заполнение...
	{
		TextNode->Prev = EndTextNode;
		TextNode->Next = 0;
		EndTextNode->Next = TextNode;
		EndTextNode = TextNode;
	}
}
// удаляем из списка
void vw_DetachTextNode(sTextNode* TextNode)
{
	if (TextNode == 0) return;

	// переустанавливаем указатели...
	if (StartTextNode == TextNode) StartTextNode = TextNode->Next;
	if (EndTextNode == TextNode) EndTextNode = TextNode->Prev;

	if (TextNode->Next != 0) TextNode->Next->Prev = TextNode->Prev;
		else if (TextNode->Prev != 0) TextNode->Prev->Next = 0;
	if (TextNode->Prev != 0) TextNode->Prev->Next = TextNode->Next;
		else if (TextNode->Next != 0) TextNode->Next->Prev = 0;
}
// переприсоединяе как первый в списке
void vw_ReAttachTextNodeAsFirst(sTextNode* TextNode)
{
	if (TextNode == 0) return;

	vw_DetachTextNode(TextNode);

	// если список пустой - просто присоединяем к списку и все
	if (StartTextNode == 0)
	{
		vw_AttachTextNode(TextNode);
	}
	else
	{
		TextNode->Next = StartTextNode;
		TextNode->Prev = 0;
		StartTextNode->Prev = TextNode;
		StartTextNode = TextNode;
	}
}








//-----------------------------------------------------------------------------
// освобождаем данные
//-----------------------------------------------------------------------------
void vw_ReleaseText()
{
	// Чистка памяти...
	sTextNode *Tmp = StartTextNode;
	while (Tmp != 0)
	{
		sTextNode *Tmp1 = Tmp->Next;
		if (Tmp->text != 0) delete [] Tmp->text;
		delete Tmp;
		Tmp = Tmp1;
	}

	StartTextNode = 0;
	EndTextNode = 0;

	if (TextBuffer != 0) {delete [] TextBuffer; TextBuffer = 0;}

	CurrentLanguage = 0;
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
void vw_InitText(const char *FileName, const char SymbolSeparator, const char SymbolEndOfLine)
{
	vw_ReleaseText();

	// читаем данные
	eFILE *TempF = vw_fopen(FileName);

	if (TempF == NULL) return;

	TempF->fseek(0, SEEK_END);
	int DataLength = TempF->ftell();
	TempF->fseek(0, SEEK_SET);
	TextBuffer = new char[DataLength];
	TempF->fread(TextBuffer, DataLength, 1);
	vw_fclose(TempF);


	// парсим формат .csv
	// не делаем проверок формата
	// вся работа строится на TextBuffer, другой памяти не выделяем - только ссылаемся указателями
	// на данные в этом буфере (предварительно поставив 0 для разграничения слов и предложений)
	// ! важно - в конце файла должен быть перевод на новую строку, чтобы поставить ноль и сформировать конец строки, заменив символ SymbolEndOfLine
	// ! важно - первая строка с идентификаторами столбцов обязательно должна присутствовать, по ней считаем кол-во языков в файле

	char *Buffer = TextBuffer;

	// по первой строчке считаем сколько у нас в файле языков
	int LanguageCount = 0;
	while(Buffer[0] != SymbolEndOfLine)
	{
		if (Buffer[0] == SymbolSeparator) LanguageCount++;
		Buffer++;DataLength--;
	}
	Buffer++;DataLength--;

	// крутим пока не обработали все строки
	while(DataLength > LanguageCount)
	{
		// делаем ноду
		sTextNode *NewTextNode;
		NewTextNode = new sTextNode;

		NewTextNode->count = LanguageCount;

		// проверяем id
		NewTextNode->id = Buffer;
		// если у нас "пустой" id - значит это пустая строка, и надо переходить к следующей
		if (NewTextNode->id[0] == SymbolSeparator)
		{
			while(Buffer[0] != SymbolEndOfLine) {Buffer++;DataLength--;}
			Buffer++;DataLength--;

			delete NewTextNode;
			continue;
		}
		// ищем маркер следующего текстового столбца
		while(Buffer[0] != SymbolSeparator) {Buffer++;DataLength--;}
		// нашли, ставим туда ноль, чтобы ноль-терминальная строка была завершенной
		Buffer[0] = 0;
		Buffer++;DataLength--;

		NewTextNode->text = new char*[NewTextNode->count];

		for (int i=0; i<NewTextNode->count; i++)
		{
			NewTextNode->text[i] = Buffer;
			// ищем маркер следующего текстового столбца или конец строки
			while((Buffer[0] != SymbolEndOfLine) && (Buffer[0] != SymbolSeparator)) {Buffer++;DataLength--;}
			// нашли, ставим туда ноль, чтобы ноль-терминальная строка была завершенной
			Buffer[0] = 0;
			Buffer++;DataLength--;
		}

		vw_AttachTextNode(NewTextNode);
	}
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
const char *vw_GetText(const char *ItemID)
{
	if (TextBuffer == 0) return 0;
	if (ItemID == 0) return 0;

	sTextNode *Tmp = StartTextNode;
	while (Tmp != 0)
	{
		sTextNode *Tmp1 = Tmp->Next;

		if (!strcmpIdNum(Tmp->id, ItemID))
		if (!vw_strcmp(ItemID, Tmp->id))
		{
			// перемещаем его на первое место в списке для ускорения поиска в след. проходах
			vw_ReAttachTextNodeAsFirst(Tmp);

			return Tmp->text[CurrentLanguage];
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


	sTextNode *Tmp = StartTextNode;
	while (Tmp != 0)
	{
		sTextNode *Tmp1 = Tmp->Next;


		for (int i=0; i<Tmp->count; i++)
		{

			const char *CharsList = Tmp->text[i];

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
