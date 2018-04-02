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

struct sXMLEntry {
	eEntryType EntryType{eEntryType::Regular};
	std::string Name; // name, if entry type is comment - comment's text
	std::string Content;
	std::unordered_map<std::string, std::string> Attributes;
	std::list<sXMLEntry> ChildrenList{};

	int LineNumber{0}; // line number in file (for new created, 0)
};

class cXMLDocument {
public:
	cXMLDocument() = default;
	// Load XML from file.
	cXMLDocument(const std::string &XMLFileName);

	// Save XML to file (libSDL RWops).
	bool Save(const std::string &XMLFileName);

	sXMLEntry *CreateRootEntry(const std::string &EntryName)
	{
		RootXMLEntry.reset(new sXMLEntry);
		RootXMLEntry->Name = EntryName;
		return RootXMLEntry.get();
	}

	sXMLEntry *GetRootEntry()
	{
		return RootXMLEntry.get();
	}

	sXMLEntry *AddEntry(sXMLEntry &ParentXMLEntry, const std::string &EntryName)
	{
		// NOTE emplace_back() return reference to the inserted element (since C++17)
		ParentXMLEntry.ChildrenList.emplace_back();
		ParentXMLEntry.ChildrenList.back().Name = EntryName;
		return &ParentXMLEntry.ChildrenList.back();
	}

	void AddEntryContent(sXMLEntry *XMLEntry, const std::string &EntryData)
	{
		if (!XMLEntry)
			return;

		XMLEntry->Content = EntryData;
	}

	void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, const std::string &AttributeData)
	{
		if (!XMLEntry)
			return;

		XMLEntry->Attributes[AttributeName] = AttributeData;
	}

	// string literal matches bool overload instead of "std::string&", forced to use "const char*" here too
	void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, const char *AttributeData)
	{
		if (!XMLEntry || !AttributeData)
			return;

		XMLEntry->Attributes[AttributeName] = AttributeData;
	}

	void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, int AttributeData)
	{
		if (!XMLEntry)
			return;

		AddEntryAttribute(XMLEntry, AttributeName, std::to_string(AttributeData));
	}

	void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, float AttributeData)
	{
		if (!XMLEntry)
			return;

		AddEntryAttribute(XMLEntry, AttributeName, std::to_string(AttributeData));
	}

	void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, bool AttributeData)
	{
		if (!XMLEntry)
			return;

		if (AttributeData)
			AddEntryAttribute(XMLEntry, AttributeName, "on");
		else
			AddEntryAttribute(XMLEntry, AttributeName, "off");
	}

	void AddComment(sXMLEntry &ParentXMLEntry, const std::string &Text)
	{
		// NOTE emplace_back() return reference to the inserted element (since C++17)
		ParentXMLEntry.ChildrenList.emplace_back();
		ParentXMLEntry.ChildrenList.back().Name = Text;
		ParentXMLEntry.ChildrenList.back().EntryType = eEntryType::Comment;
	}

	// find first children element by name
	sXMLEntry *FindEntryByName(sXMLEntry &ParentXMLEntry, const std::string &Name)
	{
		for (auto &tmpEntry : ParentXMLEntry.ChildrenList) {
			if (tmpEntry.Name == Name)
				return &tmpEntry;
		}

		return nullptr;
	}

	bool GetEntryAttribute(const sXMLEntry &XMLEntry, const std::string &AttributeName, std::string &Result)
	{
		auto tmpAttr = XMLEntry.Attributes.find(AttributeName);
		if (tmpAttr == XMLEntry.Attributes.end())
			return false;

		Result = tmpAttr->second;
		return true;
	}

	bool iGetEntryAttribute(const sXMLEntry &XMLEntry, const std::string &AttributeName, int &Result)
	{
		auto tmpAttr = XMLEntry.Attributes.find(AttributeName);
		if (tmpAttr == XMLEntry.Attributes.end())
			return false;

		Result = atoi(tmpAttr->second.c_str());
		return true;
	}

	bool fGetEntryAttribute(const sXMLEntry &XMLEntry, const std::string &AttributeName, float &Result)
	{
		auto tmpAttr = XMLEntry.Attributes.find(AttributeName);
		if (tmpAttr == XMLEntry.Attributes.end())
			return false;

		Result = (float)atof(tmpAttr->second.c_str());
		return true;
	}

	bool bGetEntryAttribute(const sXMLEntry &XMLEntry, const std::string &AttributeName, bool &Result)
	{
		auto tmpAttr = XMLEntry.Attributes.find(AttributeName);
		if (tmpAttr == XMLEntry.Attributes.end())
			return false;

		Result = false;
		if ((tmpAttr->second == "on") ||
		    (tmpAttr->second == "true") ||
		    (tmpAttr->second == "yes") ||
		    (tmpAttr->second == "1"))
			Result = true;

		return true;
	}

private:
	// Save XML elements to file recursively.
	void SaveRecursive(const sXMLEntry &XMLEntry, SDL_RWops *File, unsigned int Level);
	// Parce tag line.
	bool ParseTagLine(unsigned int LineNumber, const std::string &Buffer, sXMLEntry *XMLEntry);
	// Parse tag content.
	bool ParseTagContent(const std::string &OriginBuffer, unsigned int StartPosition,
			     const std::string &Buffer, sXMLEntry *ParentXMLEntry);

	// Accordinately to https://www.w3schools.com/XML/xml_syntax.asp
	// "XML documents must contain one root element that is the parent of all other elements".
	std::unique_ptr<sXMLEntry> RootXMLEntry{};
};

#endif // XML_H
