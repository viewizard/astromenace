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

// TODO translate comments
// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#ifndef XML_H
#define XML_H

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

enum class eEntryType {
	Regular,	// regular, could contain attributes and sub-entries
	Comment		// <--! comment -->
};

class cXMLEntry
{
public:

	cXMLEntry() = default;
	~cXMLEntry()
	{
		cXMLEntry *TmpEntry = FirstChild;
		while (TmpEntry != nullptr) {
			cXMLEntry *TmpEntry1 = TmpEntry->Next;
			delete TmpEntry;
			TmpEntry = TmpEntry1;
		}
	}

	// тип записи
	eEntryType EntryType{eEntryType::Regular};
	// имя элемента (если коментарий - текст)
	std::string Name;
	// содержимое элемента
	std::string Content;
	// номер строки в документе (если читаем из документа, если создаем - всегда ноль)
	int LineNumber{0};

	// массив атрибутов данной записи
	std::unordered_map<std::string, std::string> Attributes;

	// указатели на массив дочерних элементов
	// Children;
	cXMLEntry *FirstChild{nullptr};
	cXMLEntry *LastChild{nullptr};

	// указатели на массив элементов текущего уровня вложенности
	cXMLEntry *Next{nullptr};
	cXMLEntry *Prev{nullptr};
};

class cXMLDocument
{
public:

	cXMLDocument() = default;
	cXMLDocument(const std::string &XMLFileName);

	bool ParseTagLine(const char *OriginBuffer, unsigned int StartPosition,
			  const char *Buffer, cXMLEntry *XMLEntry);
	bool ParseTagContent(const char *OriginBuffer, unsigned int StartPosition,
			     const char *Buffer, cXMLEntry *ParentXMLEntry);

	// Save XML to file (libSDL RWops).
	bool Save(const std::string &XMLFileName);

	cXMLEntry *GetRootEntry()
	{
		return RootXMLEntry.get();
	}

	cXMLEntry *AddEntry(cXMLEntry *ParentXMLEntry, const std::string &EntryName)
	{
		if (ParentXMLEntry == nullptr) {
			RootXMLEntry.reset(new cXMLEntry);
			RootXMLEntry->Name = EntryName;
			return RootXMLEntry.get();
		} else {
			cXMLEntry *NewXMLEntry = new cXMLEntry;
			NewXMLEntry->Name = EntryName;
			AttachXMLChildEntry(ParentXMLEntry, NewXMLEntry);
			return NewXMLEntry;
		}
	}

	void AddEntryContent(cXMLEntry *XMLEntry, const std::string &EntryData)
	{
		XMLEntry->Content = EntryData;
	}

	void AddEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName, const std::string &AttributeData)
	{
		if (!XMLEntry)
			return;

		XMLEntry->Attributes[AttributeName] = AttributeData;
	}

	// string literal matches bool overload instead of "std::string&", forced to use "const char*" here too
	void AddEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName, const char *AttributeData)
	{
		if (!XMLEntry || !AttributeData)
			return;

		XMLEntry->Attributes[AttributeName] = AttributeData;
	}

	void AddEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName, int AttributeData)
	{
		AddEntryAttribute(XMLEntry, AttributeName, std::to_string(AttributeData));
	}

	void AddEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName, float AttributeData)
	{
		AddEntryAttribute(XMLEntry, AttributeName, std::to_string(AttributeData));
	}

	void AddEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName, bool AttributeData)
	{
		if (AttributeData)
			AddEntryAttribute(XMLEntry, AttributeName, "on");
		else
			AddEntryAttribute(XMLEntry, AttributeName, "off");
	}

	void AddComment(cXMLEntry *ParentXMLEntry, const std::string &Text)
	{
		cXMLEntry *NewXMLEntry = new cXMLEntry;
		NewXMLEntry->EntryType = eEntryType::Comment;
		NewXMLEntry->Name = Text;
		AttachXMLChildEntry(ParentXMLEntry, NewXMLEntry);
	}

	cXMLEntry *FindEntryByName(cXMLEntry *ParentXMLEntry, const std::string &Name)
	{
		if (ParentXMLEntry == nullptr)
			return nullptr;
		cXMLEntry *TmpEntry = ParentXMLEntry->FirstChild;
		while (TmpEntry != nullptr) {
			if (Name == TmpEntry->Name)
				return TmpEntry;
			TmpEntry = TmpEntry->Next;
		}
		return nullptr;
	}

	bool TestEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName)
	{
		if (XMLEntry->Attributes.find(AttributeName) != XMLEntry->Attributes.end())
			return true;

		return false;
	}

	const std::string &GetEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName)
	{
		return XMLEntry->Attributes[AttributeName];
	}

	int iGetEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName)
	{
		if (XMLEntry == nullptr)
			return 0;
		return atoi(GetEntryAttribute(XMLEntry, AttributeName).c_str());
	}

	float fGetEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName)
	{
		if (XMLEntry == nullptr)
			return 0.0f;
		return (float)atof(GetEntryAttribute(XMLEntry, AttributeName).c_str());
	}

	bool bGetEntryAttribute(cXMLEntry *XMLEntry, const std::string &AttributeName)
	{
		if (XMLEntry == nullptr)
			return false;
		std::string Data = GetEntryAttribute(XMLEntry, AttributeName);
		if ((Data == "on") ||
		    (Data == "true") ||
		    (Data == "yes") ||
		    (Data == "1"))
			return true;
		return false;
	}

	cXMLEntry *FindFirstChildEntryByName(cXMLEntry *ParentXMLEntry, const std::string &ChildEntryName)
	{
		if (ParentXMLEntry == nullptr)
			return nullptr;

		cXMLEntry *TmpEntry = ParentXMLEntry->FirstChild;
		while (TmpEntry != nullptr) {
			if (ChildEntryName == TmpEntry->Name)
				return TmpEntry;
			TmpEntry = TmpEntry->Next;
		}
		return nullptr;
	}

	void AttachXMLChildEntry(cXMLEntry *ParentXMLEntry, cXMLEntry *ChildXMLEntry);
	void DetachXMLChildEntry(cXMLEntry *ParentXMLEntry, cXMLEntry *ChildXMLEntry);

private:
	// Save XML elements to file recursively.
	void SaveRecursive(cXMLEntry *XMLEntry, SDL_RWops *File, unsigned int Level);

	// Accordinately to https://www.w3schools.com/XML/xml_syntax.asp
	// "XML documents must contain one root element that is the parent of all other elements".
	std::unique_ptr<cXMLEntry> RootXMLEntry{};
};

#endif // XML_H
