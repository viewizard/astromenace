/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (C) 2006-2025 Mikhail Kurinnoi, Viewizard


    AstroMenace is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AstroMenace is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


    Website: https://viewizard.com/
    Project: https://github.com/viewizard/astromenace
    E-mail: viewizard@viewizard.com

*****************************************************************************/

// NOTE in future, use make_unique() to make unique_ptr-s (since C++14)

#ifndef CORE_XML_XML_H
#define CORE_XML_XML_H

#include "../base.h"

/*
 * This is not a full featured XML parser, only features AstoMenace needs are supported.
 * This is not the fast one XML parser, we don't use this code for time-critical parts.
 * The main reason for this XML parser code - reduce external dependencies and avoid
 * license-related issues for included code.
 *
 * Limitations:
 * 1. Only ascii are supported (utf8 for comments).
 * 2. Tags between '<' and '>' characters could not be wrapped.
 * 3. CDATA are not supported.
 *
 * Special features:
 * 1. For game scripts debug, XML will care about lines numbers and open tag.
 * 2. Test XML file structure (in the way we need).
 * 3. Hash calculation for tags names (for fast search, or switch statement).
 */

namespace viewizard {

enum class eEntryType {
    Regular,    // regular, could contain attributes and sub-entries
    Comment     // <--! comment -->
};

struct sXMLEntry {
    [[gnu::noinline, clang::noinline, msvc::noinline]]
    sXMLEntry() = default;
    [[gnu::noinline, clang::noinline, msvc::noinline]]
    ~sXMLEntry() = default;

    eEntryType EntryType{eEntryType::Regular};
    std::string Name{}; // name, if entry type is comment - comment's text
    unsigned NameHash{0}; // name's hash
    std::string Content{};
    std::unordered_map<std::string, std::string> Attributes{};
    std::list<sXMLEntry> ChildrenList{};

    int LineNumber{0}; // line number in file (for new created, 0)
};

class cXMLDocument
{
public:
    cXMLDocument() = default;
    // Load XML from file.
    explicit cXMLDocument(const std::string &XMLFileName, bool Hash = false);

    // Save XML to file (libSDL RWops).
    bool Save(const std::string &XMLFileName);

    sXMLEntry *CreateRootEntry(const std::string &EntryName);
    sXMLEntry *GetRootEntry();

    sXMLEntry *AddEntry(sXMLEntry &ParentXMLEntry, const std::string &EntryName);
    void AddEntryContent(sXMLEntry *XMLEntry, const std::string &EntryData);
    void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, const std::string &AttributeData);
    // string literal matches bool overload instead of "std::string&", forced to use "const char*" here too
    void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, const char *AttributeData);
    void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, int AttributeData);
    void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, float AttributeData);
    void AddEntryAttribute(sXMLEntry *XMLEntry, const std::string &AttributeName, bool AttributeData);
    void AddComment(sXMLEntry &ParentXMLEntry, const std::string &Text);

    // find first children element by name
    sXMLEntry *FindEntryByName(sXMLEntry &ParentXMLEntry, const std::string &Name);

    bool GetEntryAttribute(const sXMLEntry &XMLEntry, const std::string &AttributeName, std::string &Result);
    bool iGetEntryAttribute(const sXMLEntry &XMLEntry, const std::string &AttributeName, int &Result);
    bool fGetEntryAttribute(const sXMLEntry &XMLEntry, const std::string &AttributeName, float &Result);
    bool bGetEntryAttribute(const sXMLEntry &XMLEntry, const std::string &AttributeName, bool &Result);

private:
    // Save XML elements to file recursively.
    void SaveRecursive(const sXMLEntry &XMLEntry, std::ofstream &File, unsigned int Level);
    // Parce tag line.
    bool ParseTagLine(unsigned int LineNumber, const std::string &Buffer, sXMLEntry *XMLEntry);
    // Parse tag content.
    bool ParseTagContent(const std::string &OriginBuffer, std::string::size_type StartPosition,
                         const std::string &Buffer, sXMLEntry *ParentXMLEntry);

    // Accordinately to https://www.w3schools.com/XML/xml_syntax.asp
    // "XML documents must contain one root element that is the parent of all other elements".
    std::unique_ptr<sXMLEntry> RootXMLEntry{};
    // calculate hash for tags names
    bool CalculateHash{false};
    // hash check for collisions
    std::map<unsigned, std::string> HashCheckMap{};
};

} // viewizard namespace

#endif // CORE_XML_XML_H
