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

#ifndef XML_H
#define XML_H

// TODO translate comments

#include "../base.h"

/*
 * This is not a full featured XML parser, only features AstoMenace needs are supported.
 * This is not the fast one XML parser, we don't use this code for time-critical parts.
 * The main reason for this XML parser code - reduce external dependencies and avoid
 * license-related issues for included code.
 *
 * Limitations:
 * 1. Only utf8 and ascii are supported.
 * 2. Tags between '<' and '>' characters could not be wrapped.
 * 3. CDATA are not supported.
 *
 * Special features:
 * 1. For game scripts debug, XML will care about lines numbers and open tag.
 * 2. Test XML file structure (in the way we need).
 */

class cXMLAttribute
{
public:

	cXMLAttribute()
	{};
	~cXMLAttribute()
	{
		if (Name != nullptr)
			delete [] Name;
		if (Data != nullptr)
			delete [] Data;
	};

	// данные атрибута
	char *Name{nullptr};
	char *Data{nullptr};

	// указатели на массив атрибутов
	cXMLAttribute *Next{nullptr};
	cXMLAttribute *Prev{nullptr};
};

class cXMLEntry
{
public:

	cXMLEntry() {};
	~cXMLEntry()
	{
		if (Name != nullptr)
			delete [] Name;
		if (Content != nullptr)
			delete [] Content;

		cXMLAttribute *TmpAttribute = FirstAttribute;
		while (TmpAttribute != nullptr) {
			cXMLAttribute *TmpAttribute1 = TmpAttribute->Next;
			delete TmpAttribute;
			TmpAttribute = TmpAttribute1;
		}

		cXMLEntry *TmpEntry = FirstChild;
		while (TmpEntry != nullptr) {
			cXMLEntry *TmpEntry1 = TmpEntry->Next;
			delete TmpEntry;
			TmpEntry = TmpEntry1;
		}
	};

	// тип записи 0-обычная, 1-коментарий
	int EntryType{0};
	// имя элемента (если коментарий - текст)
	char *Name{nullptr};
	// содержимое элемента
	char *Content{nullptr};
	// номер строки в документе (если читаем из документа, если создаем - всегда ноль)
	int LineNumber{0};


	// указатели на динамический массив атрибутов данной записи
	cXMLAttribute *FirstAttribute{nullptr};
	cXMLAttribute *LastAttribute{nullptr};

	// указатели на массив дочерних элементов
	cXMLEntry *FirstChild{nullptr};
	cXMLEntry *LastChild{nullptr};

	// указатели на массив элементов текущего уровня вложенности
	cXMLEntry *Next{nullptr};
	cXMLEntry *Prev{nullptr};
};

class cXMLDocument
{
public:

	cXMLDocument() {};
	~cXMLDocument()
	{
		ReleaseXMLDocument();
	};
	void ReleaseXMLDocument()
	{
		if (RootXMLEntry != nullptr)
			delete RootXMLEntry;
		RootXMLEntry = nullptr;
	};

	// указатель на корневой элемент
	cXMLEntry *RootXMLEntry{nullptr};

	bool Load(const char *XMLFileName);
	bool ParseTagLine(char *OriginBuffer, unsigned int StartPosition, char *Buffer, cXMLEntry *XMLEntry);
	bool ParseTagContent(char *OriginBuffer, unsigned int StartPosition, char *Buffer, cXMLEntry *ParentXMLEntry);

	bool Save(const char *XMLFileName);
	void SaveRecursive(cXMLEntry *XMLEntry, SDL_RWops *File, unsigned int Level);

	cXMLEntry *AddEntry(cXMLEntry *ParentXMLEntry, const char *EntryName)
	{
		if (ParentXMLEntry == nullptr) {
			RootXMLEntry = new cXMLEntry;
			RootXMLEntry->Name = new char[strlen(EntryName)+1];
			strcpy(RootXMLEntry->Name, EntryName);
			RootXMLEntry->Name[strlen(EntryName)] = 0;
			return RootXMLEntry;
		} else {
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
		if (AttributeData)
			AddEntryAttribute(XMLEntry, AttributeName, "on");
		else
			AddEntryAttribute(XMLEntry, AttributeName, "off");
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
		if (ParentXMLEntry == nullptr)
			return nullptr;
		cXMLEntry *TmpEntry = ParentXMLEntry->FirstChild;
		while (TmpEntry != nullptr) {
			if (!strcmp(Name, TmpEntry->Name))
				return TmpEntry;
			TmpEntry = TmpEntry->Next;
		}
		return nullptr;
	};

	char *GetEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName)
	{
		if (XMLEntry == nullptr)
			return nullptr;
		cXMLAttribute *TmpAttribute = XMLEntry->FirstAttribute;
		while (TmpAttribute != nullptr) {
			if (!strcmp(AttributeName, TmpAttribute->Name))
				return TmpAttribute->Data;
			TmpAttribute = TmpAttribute->Next;
		}
		return nullptr;
	};

	int iGetEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName)
	{
		if (XMLEntry == nullptr)
			return 0;
		return atoi(GetEntryAttribute(XMLEntry, AttributeName));
	};

	float fGetEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName)
	{
		if (XMLEntry == nullptr)
			return 0.0f;
		return (float)atof(GetEntryAttribute(XMLEntry, AttributeName));
	};

	bool bGetEntryAttribute(cXMLEntry *XMLEntry, const char *AttributeName)
	{
		if (XMLEntry == nullptr)
			return false;
		char *Data = GetEntryAttribute(XMLEntry, AttributeName);
		if ((!strcmp(Data, "on")) ||
		    (!strcmp(Data, "true")) ||
		    (!strcmp(Data, "yes")) ||
		    (!strcmp(Data, "1")))
			return true;
		return false;
	};

	cXMLEntry *FindFirstChildEntryByName(cXMLEntry *ParentXMLEntry, const char *ChildEntryName)
	{
		if (ParentXMLEntry == nullptr)
			return nullptr;

		cXMLEntry *TmpEntry = ParentXMLEntry->FirstChild;
		while (TmpEntry != nullptr) {
			if (!strcmp(ChildEntryName, TmpEntry->Name))
				return TmpEntry;
			TmpEntry = TmpEntry->Next;
		}
		return nullptr;
	}

	void AttachXMLChildEntry(cXMLEntry *ParentXMLEntry, cXMLEntry *ChildXMLEntry);
	void DetachXMLChildEntry(cXMLEntry *ParentXMLEntry, cXMLEntry *ChildXMLEntry);
	void AttachXMLAttribute(cXMLEntry *XMLEntry, cXMLAttribute *XMLAttribute);
	void DetachXMLAttribute(cXMLEntry *XMLEntry, cXMLAttribute *XMLAttribute);
};

#endif // XML_H
