/************************************************************************************

	AstroMenace (Hardcore 3D space shooter with spaceship upgrade possibilities)
	Copyright (c) 2006-2016 Mikhail Kurinnoi, Viewizard


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


#ifndef XML_H
#define XML_H

#include "../Base.h"


// реализация всех необходимых игре функций по считыванию/записи XML файла
// (!) это не полноценный парсер XML, а ограниченный по функционалу код (+ по ряду причин не самый быстрый)
// исключительно для работы с файлом конфигурации игры и прочими текстовыми данным (фиксированными), где скорость не играет большого значения

// основные отличия:
// 1) не используем std string
// 2) связываем номер строки в документе с открывающим тэгом (нам это нужно для отладки уровней и вывода информации об ошибках хмл)
// (!) для получения номера строки необходимо раскоментрировать #define gamedebug в config.h (для увеличения скорости парсинга не включаем)
// 3) проверяем структуру xml документа

// особенности работы:
// 1) игнорируем комментарии при загрузке файла
// 2) работаем только с utf8 (и ascii), особенности других кодировок не учитываем
// 3) тэги не должны разбиваться на несколько строк (c < до > все должно быть на одной строке)
// 4) не работаем с CDATA






class cXMLAttribute
{
public:

	cXMLAttribute()
	{
		Name = Data = 0; Next = Prev = 0;
	};
	~cXMLAttribute()
	{
		if (Name != 0) delete [] Name;
		if (Data != 0) delete [] Data;
	};

	// данные атрибута
	char *Name;
	char *Data;

	// указатели на массив атрибутов
	cXMLAttribute *Next;
	cXMLAttribute *Prev;
};





class cXMLEntry
{
public:

	cXMLEntry()
	{
		EntryType = 0;
		Name = Content = 0;
		LineNumber = 0;
		FirstAttribute = LastAttribute = 0;
		Next = Prev = FirstChild = LastChild = 0;
	}
	~cXMLEntry()
	{
		if (Name != 0) delete [] Name;
		if (Content != 0) delete [] Content;

		cXMLAttribute *TmpAttribute = FirstAttribute;
		while (TmpAttribute != 0)
		{
			cXMLAttribute *TmpAttribute1 = TmpAttribute->Next;
			delete TmpAttribute;
			TmpAttribute = TmpAttribute1;
		}

		cXMLEntry *TmpEntry = FirstChild;
		while (TmpEntry != 0)
		{
			cXMLEntry *TmpEntry1 = TmpEntry->Next;
			delete TmpEntry;
			TmpEntry = TmpEntry1;
		}
	}

	// тип записи 0-обычная, 1-коментарий
	int EntryType;
	// имя элемента (если коментарий - текст)
	char *Name;
	// содержимое элемента
	char *Content;
	// номер строки в документе (если читаем из документа, если создаем - всегда ноль)
	int LineNumber;


	// указатели на динамический массив атрибутов данной записи
	cXMLAttribute *FirstAttribute;
	cXMLAttribute *LastAttribute;

	// указатели на массив дочерних элементов
	cXMLEntry *FirstChild;
	cXMLEntry *LastChild;

	// указатели на массив элементов текущего уровня вложенности
	cXMLEntry *Next;
	cXMLEntry *Prev;
};





class cXMLDocument
{
public:

	cXMLDocument(){	RootXMLEntry = 0; };
	~cXMLDocument(){ ReleaseXMLDocument(); };
	void ReleaseXMLDocument()
	{
		if (RootXMLEntry != 0) delete RootXMLEntry;
		RootXMLEntry = 0;
	};


	// указатель на корневой элемент
	cXMLEntry *RootXMLEntry;


	bool Load(const char *XMLFileName);
	bool ParseTagLine(char *OriginBuffer, unsigned int StartPosition, char *Buffer, cXMLEntry *XMLEntry);
	bool ParseTagContent(char *OriginBuffer, unsigned int StartPosition, char *Buffer, cXMLEntry *ParentXMLEntry);


	bool Save(const char *XMLFileName);
	void SaveRecursive(cXMLEntry *XMLEntry, SDL_RWops *File, unsigned int Level);



	cXMLEntry *AddEntry(cXMLEntry *ParentXMLEntry, const char *EntryName)
	{
		if (ParentXMLEntry == 0)
		{
			RootXMLEntry = new cXMLEntry;
			RootXMLEntry->Name = new char[strlen(EntryName)+1];
			strcpy(RootXMLEntry->Name, EntryName);
			RootXMLEntry->Name[strlen(EntryName)] = 0;
			return RootXMLEntry;
		}
		else
		{
			cXMLEntry *NewXMLEntry = new cXMLEntry;
			NewXMLEntry->Name = new char[strlen(EntryName)+1];
			strcpy(NewXMLEntry->Name, EntryName);
			NewXMLEntry->Name[strlen(EntryName)] = 0;
			AttachXMLChildEntry(ParentXMLEntry, NewXMLEntry);
			return NewXMLEntry;
		}
	};


	void AddEntryContent(cXMLEntry *XMLEntry, char *EntryData)
	{
		XMLEntry->Content = new char[strlen(EntryData)+1];
		strcpy(XMLEntry->Content, EntryData);
		XMLEntry->Content[strlen(EntryData)] = 0;
	};


	void AddEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName, const char *AttributeData)
	{
		cXMLAttribute *NewXMLAttribute = new cXMLAttribute;

		NewXMLAttribute->Name = new char[strlen(AttributeName)+1];
		strcpy(NewXMLAttribute->Name, AttributeName);
		NewXMLAttribute->Name[strlen(AttributeName)] = 0;

		NewXMLAttribute->Data = new char[strlen(AttributeData)+1];
		strcpy(NewXMLAttribute->Data, AttributeData);
		NewXMLAttribute->Data[strlen(AttributeData)] = 0;

		AttachXMLAttribute(XMLEntry, NewXMLAttribute);
	};
	void AddEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName, int AttributeData)
	{
		char buffer[1024];
		sprintf(buffer, "%i", AttributeData);
		AddEntryAttribute(XMLEntry, AttributeName, buffer);
	};
	void AddEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName, float AttributeData)
	{
		char buffer[1024];
		sprintf(buffer, "%f", AttributeData);
		AddEntryAttribute(XMLEntry, AttributeName, buffer);
	};
	void AddEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName, bool AttributeData)
	{
		if (AttributeData) AddEntryAttribute(XMLEntry, AttributeName, "on");
		else AddEntryAttribute(XMLEntry, AttributeName, "off");
	};


	void AddComment(cXMLEntry *ParentXMLEntry, const char *Text)
	{
		cXMLEntry *NewXMLEntry = new cXMLEntry;
		NewXMLEntry->EntryType = 1;
		NewXMLEntry->Name = new char[strlen(Text)+1];
		strcpy(NewXMLEntry->Name, Text);
		NewXMLEntry->Name[strlen(Text)] = 0;
		AttachXMLChildEntry(ParentXMLEntry, NewXMLEntry);
	};









	cXMLEntry *FindEntryByName(cXMLEntry *ParentXMLEntry, const char *Name)
	{
		if (ParentXMLEntry == 0) return 0;
		cXMLEntry *TmpEntry = ParentXMLEntry->FirstChild;
		while (TmpEntry != 0)
		{
			if (!strcmp(Name, TmpEntry->Name)) return TmpEntry;
			TmpEntry = TmpEntry->Next;
		}
		return 0;
	};

	char *GetEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName)
	{
		if (XMLEntry == 0) return 0;
		cXMLAttribute *TmpAttribute = XMLEntry->FirstAttribute;
		while (TmpAttribute != 0)
		{
			if (!strcmp(AttributeName, TmpAttribute->Name)) return TmpAttribute->Data;
			TmpAttribute = TmpAttribute->Next;
		}
		return 0;
	};

	int iGetEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName)
	{
		if (XMLEntry == 0) return 0;
		return atoi(GetEntryAttribute(XMLEntry, AttributeName));
	};

	float fGetEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName)
	{
		if (XMLEntry == 0) return 0.0f;
		return (float)atof(GetEntryAttribute(XMLEntry, AttributeName));
	};

	bool bGetEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName)
	{
		if (XMLEntry == 0) return false;
		char *Data = GetEntryAttribute(XMLEntry, AttributeName);
		if ((!strcmp(Data, "on")) || (!strcmp(Data, "true")) || (!strcmp(Data, "yes")) || (!strcmp(Data, "1"))) return true;
		return false;
	};



	cXMLEntry *FindFirstChildEntryByName(cXMLEntry *ParentXMLEntry, const char *ChildEntryName)
	{
		if (ParentXMLEntry == 0) return 0;

		cXMLEntry *TmpEntry = ParentXMLEntry->FirstChild;
		while (TmpEntry != 0)
		{
			if (!strcmp(ChildEntryName, TmpEntry->Name)) return TmpEntry;
			TmpEntry = TmpEntry->Next;
		}
		return 0;
	}




	void AttachXMLChildEntry(cXMLEntry *ParentXMLEntry, cXMLEntry *ChildXMLEntry);
	void DetachXMLChildEntry(cXMLEntry *ParentXMLEntry, cXMLEntry *ChildXMLEntry);
	void AttachXMLAttribute(cXMLEntry *XMLEntry, cXMLAttribute *XMLAttribute);
	void DetachXMLAttribute(cXMLEntry *XMLEntry, cXMLAttribute *XMLAttribute);
};


#endif // XML_H
