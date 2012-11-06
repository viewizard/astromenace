/******************************************************************************

	This source file is part of AstroMenace game
	(Hardcore 3D space shooter with spaceship upgrade possibilities.)
	For the latest info, see http://www.viewizard.com/

	File name: Text.cpp

	Copyright (c) 2006-2012 Michael Kurinnoy, Viewizard
	All Rights Reserved.

	File Version: 1.3

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


#include "../Game.h"



struct sTextNode
{
	char *id;
	char *en;
	char *de;
	char *ru;

	sTextNode *Next;
	sTextNode *Prev;
};


sTextNode *StartTextNode;
sTextNode *EndTextNode;

// буфер со всем текстом
char *TextBuffer = 0;





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
void ReleaseGameText()
{
	// Чистка памяти...
	sTextNode *Tmp = StartTextNode;
	while (Tmp != 0)
	{
		sTextNode *Tmp1 = Tmp->Next;
		delete Tmp;
		Tmp = Tmp1;
	}

	StartTextNode = 0;
	EndTextNode = 0;

	if (TextBuffer != 0) {delete [] TextBuffer; TextBuffer = 0;}
}


//-----------------------------------------------------------------------------
// иним текстовых xml
//-----------------------------------------------------------------------------
void InitGameText(const char *FileName)
{
	ReleaseGameText();

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
	// не делаем проверок формата, символы между столбцами - ';', символ конца строки '\n'
	// вся работа строится на TextBuffer, другой памяти не выделяем - только ссылаемся
	// указателями на данные в этом буфере (предварительно поставив 0 для разграничения слов и предложений)
	// ! важно - в конце файла должен быть перевод на новую строку, чтобы поставить ноль и сформировать конец строки, заменив символ '\n'
	// перевод строки символ LF делать всегда в файле

	char *Buffer = TextBuffer;

	// первую строчку пропускаем всегда, она не содержит полезной информации
	while(Buffer[0] != '\n') {Buffer++;DataLength--;}
	Buffer++;DataLength--;

	// крутим пока не обработали все строки (3 - это три ';' - минимум для строки в нашем случае)
	while(DataLength > 3)
	{
		char *id = 0;
		char *en = 0;
		char *de = 0;
		char *ru = 0;


		// проверяем id
		id = Buffer;
		// если у нас "пустой" id - значит это пустая строка, и надо переходить к следующей
		if (id[0] == ';')
		{
			while(Buffer[0] != '\n') {Buffer++;DataLength--;}
			Buffer++;DataLength--;
			continue;
		}
		// ищем маркер следующего текстового столбца
		while(Buffer[0] != ';') {Buffer++;DataLength--;}
		// нашли, ставим туда ноль, чтобы ноль-терминальная строка была завершенной
		Buffer[0] = 0;
		Buffer++;DataLength--;


		// проверяем en
		en = Buffer;
		// ищем маркер следующего текстового столбца
		while(Buffer[0] != ';') {Buffer++;DataLength--;}
		// нашли, ставим туда ноль, чтобы ноль-терминальная строка была завершенной
		Buffer[0] = 0;
		Buffer++;DataLength--;

		// проверяем de
		de = Buffer;
		// ищем маркер следующего текстового столбца
		while(Buffer[0] != ';') {Buffer++;DataLength--;}
		// нашли, ставим туда ноль, чтобы ноль-терминальная строка была завершенной
		Buffer[0] = 0;
		Buffer++;DataLength--;

		// проверяем ru
		ru = Buffer;
		// ищем маркер следующего текстового столбца
		while(Buffer[0] != '\n') {Buffer++;DataLength--;}
		// нашли, ставим туда ноль, чтобы ноль-терминальная строка была завершенной
		Buffer[0] = 0;
		Buffer++;DataLength--;


		// все компоненты собраны, делаем ноду
		sTextNode *NewTextNode;
		NewTextNode = new sTextNode;

		vw_AttachTextNode(NewTextNode);

		NewTextNode->id = id;
		NewTextNode->en = en;
		NewTextNode->de = de;
		NewTextNode->ru = ru;
	}

}




// проверяем только цифры
int strcmpIdNum(const char *a, const char *b)
{
	// если первые 2 символа одинаковые - значит по номеру 100% совпадают
	if ((a[0] == b[0]) & (a[1] == b[1])) return 0;

	return 1;
}




//-----------------------------------------------------------------------------
// получаем текст из файлы
//-----------------------------------------------------------------------------
const char *GetText(const char *ItemID)
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

			// возвращаем с учетом текущего языка
			switch(Setup.MenuLanguage)
			{
				case 1: return Tmp->en;
				case 2: return Tmp->de;
				case 3: return Tmp->ru;

				default: return 0;
			}
		}

		Tmp = Tmp1;
	}

	fprintf(stderr, "Text not found, ID: %s\n", ItemID);
	return 0;
}




//-----------------------------------------------------------------------------
// проверяем, есть ли символ в фонте, перебираем по тексту всех языков
//-----------------------------------------------------------------------------
int CheckFontCharsInText()
{
	if (TextBuffer == 0) return -1;


	printf("Font characters detection start.\n");


	sTextNode *Tmp = StartTextNode;
	while (Tmp != 0)
	{
		sTextNode *Tmp1 = Tmp->Next;

		const char *CharsList = Tmp->en;

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


		CharsList = Tmp->de;

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


		CharsList = Tmp->ru;

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


		Tmp = Tmp1;
	}



	printf("Font characters detection end.\n\n");

	return 0;
}
