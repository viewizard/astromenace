/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2018 Mikhail Kurinnoi, Viewizard


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
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/


#include "xml.h"
#include "../vfs/vfs.h"








// ищем первое вхождение подстроки в строку, передаем позицию, или -1 если не найдена
int FindSubString(char *String, const char *SubString)
{
	unsigned int lenght = strlen(SubString);
	for (int i = 0; (String+i)[0] != '\0'; i++) {
		if (!strncmp(String+i, SubString, lenght)) return i;
	}
	return -1;
}

// замена данных, с сохранением перевода на новую строку
void EraseSubString(char *String, unsigned int StartPos, unsigned int EndPos)
{
	// проверяем, если не символы конце строки \r или \n - меняем на пробелы
	for (unsigned int i=StartPos; i<EndPos; i++) {
		if ((String[i] != '\r') && (String[i] != '\n')) String[i] = ' ';
	}
}

// считаем кол-во строк до текущего положения буфера
#ifdef gamedebug
unsigned int GetLineNumber(char *String, unsigned int Pos)
{
	unsigned int LineNumber = 1;
	unsigned int CurrentPos = 0;

	while ((strlen(String) > 0) && (CurrentPos <= Pos)) {
		if (String[0] == '\n') LineNumber++;
		CurrentPos++;
		String++;
	}
	return LineNumber;
}
#else
unsigned int GetLineNumber(char *UNUSED(String), unsigned int UNUSED(Pos))
{
	return 0;
}
#endif // gamedebug

// создаем подстроку из строки с выделением памяти
char *CreateSubString(char *String, unsigned int StartPos, unsigned int EndPos)
{
	if (EndPos <= StartPos)
		return nullptr;

	char * Result = new char[EndPos-StartPos+1];
	strncpy(Result, String+StartPos, EndPos-StartPos);
	Result[EndPos-StartPos] = 0;

	return Result;
}













//-----------------------------------------------------------------------------
// Включаем в список
//-----------------------------------------------------------------------------
void cXMLDocument::AttachXMLChildEntry(cXMLEntry *ParentXMLEntry, cXMLEntry *ChildXMLEntry)
{
	if ((RootXMLEntry == nullptr) &&
	    (ParentXMLEntry == nullptr)) {
		RootXMLEntry = ChildXMLEntry;
		return;
	}

	if ((ParentXMLEntry == nullptr) ||
	    (ChildXMLEntry == nullptr))
		return;

	// первый в списке...
	if (ParentXMLEntry->LastChild == nullptr) {
		ChildXMLEntry->Prev = nullptr;
		ChildXMLEntry->Next = nullptr;
		ParentXMLEntry->FirstChild = ChildXMLEntry;
		ParentXMLEntry->LastChild = ChildXMLEntry;
	} else { // продолжаем заполнение...
		ChildXMLEntry->Prev = ParentXMLEntry->LastChild;
		ChildXMLEntry->Next = nullptr;
		ParentXMLEntry->LastChild->Next = ChildXMLEntry;
		ParentXMLEntry->LastChild = ChildXMLEntry;
	}
}

//-----------------------------------------------------------------------------
// Исключаем из списка
//-----------------------------------------------------------------------------
void cXMLDocument::DetachXMLChildEntry(cXMLEntry *ParentXMLEntry, cXMLEntry *XMLChildEntry)
{
	if ((ParentXMLEntry == nullptr) ||
	    (XMLChildEntry == nullptr))
		return;

	// переустанавливаем указатели...
	if (ParentXMLEntry->FirstChild == XMLChildEntry)
		ParentXMLEntry->FirstChild = XMLChildEntry->Next;
	if (ParentXMLEntry->LastChild == XMLChildEntry)
		ParentXMLEntry->LastChild = XMLChildEntry->Prev;

	if (XMLChildEntry->Next != nullptr)
		XMLChildEntry->Next->Prev = XMLChildEntry->Prev;
	else if (XMLChildEntry->Prev != nullptr)
		XMLChildEntry->Prev->Next = nullptr;

	if (XMLChildEntry->Prev != nullptr)
		XMLChildEntry->Prev->Next = XMLChildEntry->Next;
	else if (XMLChildEntry->Next != nullptr)
		XMLChildEntry->Next->Prev = nullptr;
}

//-----------------------------------------------------------------------------
// Включаем в список атрибут
//-----------------------------------------------------------------------------
void cXMLDocument::AttachXMLAttribute(cXMLEntry *XMLEntry, cXMLAttribute *XMLAttribute)
{
	if ((XMLEntry == nullptr) ||
	    (XMLAttribute == nullptr))
		return;

	// первый в списке...
	if (XMLEntry->LastAttribute == nullptr) {
		XMLAttribute->Prev = nullptr;
		XMLAttribute->Next = nullptr;
		XMLEntry->FirstAttribute = XMLAttribute;
		XMLEntry->LastAttribute = XMLAttribute;
	} else { // продолжаем заполнение...
		XMLAttribute->Prev = XMLEntry->LastAttribute;
		XMLAttribute->Next = nullptr;
		XMLEntry->LastAttribute->Next = XMLAttribute;
		XMLEntry->LastAttribute = XMLAttribute;
	}
}

//-----------------------------------------------------------------------------
// Исключаем из списка атрибут
//-----------------------------------------------------------------------------
void cXMLDocument::DetachXMLAttribute(cXMLEntry *XMLEntry, cXMLAttribute *XMLAttribute)
{
	if ((XMLEntry == nullptr) ||
	    (XMLAttribute == nullptr))
		return;

	// переустанавливаем указатели...
	if (XMLEntry->FirstAttribute == XMLAttribute)
		XMLEntry->FirstAttribute = XMLAttribute->Next;
	if (XMLEntry->LastAttribute == XMLAttribute)
		XMLEntry->LastAttribute = XMLAttribute->Prev;

	if (XMLAttribute->Next != nullptr)
		XMLAttribute->Next->Prev = XMLAttribute->Prev;
	else if (XMLAttribute->Prev != nullptr)
		XMLAttribute->Prev->Next = nullptr;

	if (XMLAttribute->Prev != nullptr)
		XMLAttribute->Prev->Next = XMLAttribute->Next;
	else if (XMLAttribute->Next != nullptr)
		XMLAttribute->Next->Prev = nullptr;
}













bool cXMLDocument::ParseTagLine(char *OriginBuffer, unsigned int StartPosition, char *Buffer, cXMLEntry *XMLEntry)
{
	// 1 - получаем имя тэга (начинается сразу после символа <, а заканчивается пробелом, >, />, или символом таб)
	int TagNameEnd = FindSubString(Buffer, " ");
	if (TagNameEnd == -1 || (FindSubString(Buffer, "\t") != -1 && TagNameEnd > FindSubString(Buffer, "\t"))) TagNameEnd = FindSubString(Buffer, "\t");
	if (TagNameEnd == -1 || (FindSubString(Buffer, ">") != -1 && TagNameEnd > FindSubString(Buffer, ">"))) TagNameEnd = FindSubString(Buffer, ">");
	if (TagNameEnd == -1 || (FindSubString(Buffer, "/>") != -1 && TagNameEnd > FindSubString(Buffer, "/>"))) TagNameEnd = FindSubString(Buffer, "/>");
	XMLEntry->Name = CreateSubString(Buffer, 1, TagNameEnd);

	// 2 - проверяем наличие атрибутов и заносим их в динамический массив
	unsigned int i = TagNameEnd;
	while (((Buffer+i)[0] != '>') || ((Buffer+i)[0] != '\0')) {
		// пропускаем все пробелы и табы
		while ((((Buffer+i)[0] == ' ') || ((Buffer+i)[0] == '\t')) && ((Buffer+i)[0] != '\0')) i++;
		if ((Buffer+i)[0] == '\0') {
			fprintf(stderr, "XML file corrupted, line: %i.", GetLineNumber(OriginBuffer, StartPosition));
			break;
		}
		// еще раз проверяем, возможно завершение тэга ставили через пробел или таб
		if (((Buffer+i)[0] == '>') || (!strncmp(Buffer+i, "/>", strlen("/>")))) break;

		// находим имя атрибута
		unsigned int AttribNameStart = i;
		while (((Buffer+i)[0] != '=') && ((Buffer+i)[0] != '\0')) i++;
		if ((Buffer+i)[0] == '\0') {
			fprintf(stderr, "XML file corrupted, line: %i.", GetLineNumber(OriginBuffer, StartPosition));
			break;
		}
		unsigned int AttribNameEnd = i;
		// пропускаем все до кавычек (они у нас следующие, после знака равенства)
		i+=2;
		unsigned int AttribDataStart = i;
		while ((((Buffer+i)[0] != '\'') && ((Buffer+i)[0] != '\"')) && ((Buffer+i)[0] != '\0')) i++;
		if ((Buffer+i)[0] == '\0') {
			fprintf(stderr, "XML file corrupted, line: %i.", GetLineNumber(OriginBuffer, StartPosition));
			break;
		}
		unsigned int AttribDataEnd = i;
		i++;

		// собираем новый атрибут и подключаем его к элементу
		cXMLAttribute *XMLAttribute = new cXMLAttribute;
		AttachXMLAttribute(XMLEntry, XMLAttribute);
		XMLAttribute->Name = CreateSubString(Buffer, AttribNameStart, AttribNameEnd);
		XMLAttribute->Data = CreateSubString(Buffer, AttribDataStart, AttribDataEnd);
	}


	// 3 - определяем и номер строки
	XMLEntry->LineNumber = GetLineNumber(OriginBuffer, StartPosition);


	// 4 - определить есть ли в ней атрибут закрытия '/', или у нас есть еще и контент и закрывающий тэг
	if (FindSubString(Buffer, "/>") != -1) return false;
	return true;
}



bool cXMLDocument::ParseTagContent(char *OriginBuffer, unsigned int StartPosition, char *Buffer, cXMLEntry *ParentXMLEntry)
{
	// проверяем наличие вложенных тэгов
	bool ChildsFound = true;
	int DetectTagOpenSymbol = FindSubString(Buffer, "<");
	// если символа открытия в строке нет - это просто данные, иначе проверяем, что стоит до этого символа
	if (DetectTagOpenSymbol > 0) {
		int CurrentPos = 0;
		while(CurrentPos != DetectTagOpenSymbol) {
			// если до открывающего тэга идут не " ", "\t", "\r", "\n", значит у нас просто данные
			if (((Buffer+CurrentPos)[0] != ' ') &&
			    ((Buffer+CurrentPos)[0] != '\t') &&
			    ((Buffer+CurrentPos)[0] != '\r') &&
			    ((Buffer+CurrentPos)[0] != '\n')) {
				ChildsFound = false;
				break;
			}

			CurrentPos++;
		}

	} else ChildsFound = false;


	// 1 - это просто контент, заносим данные и выходи из рекурсии
	if (!ChildsFound) {
		ParentXMLEntry->Content = CreateSubString(Buffer, 0, strlen(Buffer));
		return true;
	}
	// 2 - если в строке нашли открывающий символ тэга - идем на рекурсивную обработку строки с хмл данными
	else {
		// в цикле, пока не достигнем конца обрабатываемой строки:
		unsigned int CurrentBufferPosition = 0;
		while(strlen(Buffer) > 0) {
			// находим положение открывающего тэг символа и закрывающего
			DetectTagOpenSymbol = FindSubString(Buffer, "<");

			// это может быть комментарий, проверяем
			if (!strncmp(Buffer+DetectTagOpenSymbol, "<!--", strlen("<!--"))) {
				// ищем завершающую часть, и сразу перемещаемся к ней
				int DetectCommentCloseSymbol = FindSubString(Buffer, "-->");
				if (DetectCommentCloseSymbol == -1) {
					fprintf(stderr, "XML file corrupted, can't find comment end in line %i.\n", GetLineNumber(OriginBuffer, StartPosition+DetectTagOpenSymbol+CurrentBufferPosition));
					return false;
				}
				Buffer += DetectCommentCloseSymbol + strlen("-->");
				CurrentBufferPosition += DetectCommentCloseSymbol + strlen("-->");
				continue;
			}

			// если в строке уже нет открывающих символов - просто выходим, все проверили
			if (DetectTagOpenSymbol == -1) return true;
			int DetectTagCloseSymbol = FindSubString(Buffer, ">");
			// если был открывающий символ, но нет закрывающего - это ошибка структуры документа
			if (DetectTagCloseSymbol == -1) {
				fprintf(stderr, "XML file corrupted, can't find element end for element in line %i.\n", GetLineNumber(OriginBuffer, StartPosition+DetectTagOpenSymbol+CurrentBufferPosition));
				return false;
			}
			DetectTagCloseSymbol += strlen(">");

			// создаем новый элемент и подключаем его к родительскому
			cXMLEntry *XMLEntry = new cXMLEntry;
			AttachXMLChildEntry(ParentXMLEntry, XMLEntry);

			// полученные данные передаем на обработку и анализ строки элемента
			char *TagString = CreateSubString(Buffer, DetectTagOpenSymbol, DetectTagCloseSymbol);
			bool ElementHaveContent = ParseTagLine(OriginBuffer, StartPosition+DetectTagOpenSymbol+CurrentBufferPosition, TagString, XMLEntry);
			delete [] TagString;

			// если у нас закрытый тэг - с этим элементом закончили, идем искать дальше
			if (!ElementHaveContent) {
				Buffer += DetectTagCloseSymbol;
				CurrentBufferPosition += DetectTagCloseSymbol;
				continue;
			}

			// если тэг открытый - ищем завершающий тэг </имя>
			char *CloseElement = new char[strlen("</>")+strlen(XMLEntry->Name)+1];
			strcpy(CloseElement, "</");
			strcat(CloseElement, XMLEntry->Name);
			strcat(CloseElement, ">");
			CloseElement[strlen("</>")+strlen(XMLEntry->Name)] = 0;
			int CloseElementPosition = FindSubString(Buffer, CloseElement);
			delete [] CloseElement;
			// если закрывающего элемента нет - значит файл поврежден
			if (CloseElementPosition == -1) {
				fprintf(stderr, "XML file corrupted, can't find element end: %s in line: %i\n", XMLEntry->Name, GetLineNumber(OriginBuffer, StartPosition+DetectTagOpenSymbol+CurrentBufferPosition));
				return false;
			}

			// передаем данные на рекурсивную обработку (если закрывающий тэг не стоит сразу после открывающего)
			if (DetectTagCloseSymbol < CloseElementPosition) {
				char *ElementContent = CreateSubString(Buffer, DetectTagCloseSymbol, CloseElementPosition);
				if (!ParseTagContent(OriginBuffer, DetectTagCloseSymbol+StartPosition+CurrentBufferPosition, ElementContent, XMLEntry)) {
					// вернули с ошибкой, выходим
					delete [] ElementContent;
					return false;
				}
				delete [] ElementContent;
			}

			// смещаем буфер
			Buffer += CloseElementPosition + strlen(XMLEntry->Name) + strlen("</>");
			CurrentBufferPosition += CloseElementPosition + strlen(XMLEntry->Name) + strlen("</>");
		}
	}


	return false;
}












//-----------------------------------------------------------------------------
// Загрузка
//-----------------------------------------------------------------------------
bool cXMLDocument::Load(const char *XMLFileName)
{
	printf("Open XML file: %s\n", XMLFileName);

	// если что-то было загружено ранее - освобождаем
	ReleaseXMLDocument();

	// читаем данные
	eFILE *XMLFile = vw_fopen(XMLFileName);

	if (XMLFile == nullptr) {
		fprintf(stderr, "XML file not found: %s\n", XMLFileName);
		return false;
	}

	// читаем все данные в буфер
	XMLFile->fseek(0, SEEK_END);
	unsigned int DataLength = XMLFile->ftell();
	XMLFile->fseek(0, SEEK_SET);
	char *Buffer = new char[DataLength+1];
	Buffer[DataLength] = 0;
	XMLFile->fread(Buffer, DataLength, 1);
	vw_fclose(XMLFile);


	// проверяем заголовок
	if (FindSubString(Buffer, "<?xml") == -1) {
		fprintf(stderr, "XML file corrupted: %s\n", XMLFileName);
		return false;
	}
	if (FindSubString(Buffer, "?>") == -1) {
		fprintf(stderr, "XML file corrupted: %s\n", XMLFileName);
		return false;
	}


	// идем на рекурсивную обработку
	if (!ParseTagContent(Buffer, FindSubString(Buffer, "?>")+strlen("?>"), Buffer+FindSubString(Buffer, "?>")+strlen("?>"), nullptr)) {
		fprintf(stderr, "XML file corrupted: %s\n", XMLFileName);
		delete [] Buffer;
		return false;
	}

	if (RootXMLEntry == nullptr) {
		fprintf(stderr, "XML file corrupted, root element not found: %s\n", XMLFileName);
		delete [] Buffer;
		return false;
	}

	delete [] Buffer;
	return true;
}









//-----------------------------------------------------------------------------
// Сохранение
//-----------------------------------------------------------------------------
void cXMLDocument::SaveRecursive(cXMLEntry *XMLEntry, SDL_RWops *File, unsigned int Level)
{
	// если это комментарий
	if (XMLEntry->EntryType == 1) {
		for (unsigned int i=0; i<Level; i++) {
			SDL_RWwrite(File, "    ", strlen("    "), 1);
		}
		SDL_RWwrite(File, "<!--", strlen("<!--"), 1);
		SDL_RWwrite(File, XMLEntry->Name, strlen(XMLEntry->Name), 1);
		SDL_RWwrite(File, "-->\r\n", strlen("-->\r\n"), 1);
	} else {
		// пишем имя
		for (unsigned int i=0; i<Level; i++) {
			SDL_RWwrite(File, "    ", strlen("    "), 1);
		}
		SDL_RWwrite(File, "<", strlen("<"), 1);
		SDL_RWwrite(File, XMLEntry->Name, strlen(XMLEntry->Name), 1);

		// пишем атрибуты
		if (XMLEntry->FirstAttribute != nullptr) {
			cXMLAttribute *TmpAttrib = XMLEntry->FirstAttribute;
			while (TmpAttrib != nullptr) {
				SDL_RWwrite(File, " ", strlen(" "), 1);
				SDL_RWwrite(File, TmpAttrib->Name, strlen(TmpAttrib->Name), 1);
				SDL_RWwrite(File, "=\"", strlen("=\""), 1);
				SDL_RWwrite(File, TmpAttrib->Data, strlen(TmpAttrib->Data), 1);
				SDL_RWwrite(File, "\"", strlen("\""), 1);
				TmpAttrib = TmpAttrib->Next;
			}
			SDL_RWwrite(File, " ", strlen(" "), 1);
		}


		// пишем данные
		if ((XMLEntry->FirstChild != nullptr) || (XMLEntry->Content != nullptr)) {
			if (XMLEntry->Content != nullptr) {
				SDL_RWwrite(File, ">", strlen(">"), 1);
				SDL_RWwrite(File, XMLEntry->Content, strlen(XMLEntry->Content), 1);
				SDL_RWwrite(File, "</", strlen("</"), 1);
				SDL_RWwrite(File, XMLEntry->Name, strlen(XMLEntry->Name), 1);
				SDL_RWwrite(File, ">\r\n", strlen(">\r\n"), 1);
			} else {
				SDL_RWwrite(File, ">\r\n", strlen(">\r\n"), 1);
				cXMLEntry *Tmp = XMLEntry->FirstChild;
				while (Tmp != nullptr) {
					SaveRecursive(Tmp, File, Level+1);
					Tmp = Tmp->Next;
				}
				for (unsigned int i=0; i<Level; i++) SDL_RWwrite(File, "    ", strlen("    "), 1);
				SDL_RWwrite(File, "</", strlen("</"), 1);
				SDL_RWwrite(File, XMLEntry->Name, strlen(XMLEntry->Name), 1);
				SDL_RWwrite(File, ">\r\n", strlen(">\r\n"), 1);
			}
		} else {
			SDL_RWwrite(File, "/>\r\n", strlen("/>\r\n"), 1);
		}
	}
}
bool cXMLDocument::Save(const char *XMLFileName)
{
	printf("Save XML file: %s\n", XMLFileName);

	SDL_RWops *File = SDL_RWFromFile(XMLFileName, "wb");
	if (File == nullptr) {
		fprintf(stderr, "Can't open XML file for write %s\n", XMLFileName);
		return false;
	}


	// пишем заголовок
	SDL_RWwrite(File, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n", strlen("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n"), 1);

	if (RootXMLEntry == nullptr) {
		SDL_RWclose(File);
		return true;
	}

	// рекурсивно пишем все данные
	SaveRecursive(RootXMLEntry, File, 0);

	SDL_RWclose(File);
	return true;
}
