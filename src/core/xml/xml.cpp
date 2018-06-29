/************************************************************************************

	AstroMenace
	Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
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
	along with AstroMenace. If not, see <https://www.gnu.org/licenses/>.


	Website: https://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO revise ParseTagContent() in order to use main buffer with begin/end position
//      don't create temporary buffers string

#include "../vfs/vfs.h"
#include "xml.h"

namespace viewizard {

namespace {

// For more compatibility, we use 'Windows' end line symbols
// in saved XML files.
const std::string EndLine{"\r\n"};

} // unnamed namespace


/*
 * Line number in XML file.
 */
#ifdef NDEBUG
static unsigned int GetLineNumber(const std::string &UNUSED(Buffer), unsigned long UNUSED(Pos))
{
	return 0;
}
#else
static unsigned int GetLineNumber(const std::string &Buffer, unsigned long Pos)
{
	unsigned int LineNumber = 1;

	for (unsigned int i = 0; i < Pos; i++) {
		if (Buffer[i] == '\n')
			LineNumber++;
	}

	return LineNumber;
}
#endif // NDEBUG

/*
 * Parce tag line.
 */
bool cXMLDocument::ParseTagLine(unsigned int LineNumber, const std::string &Buffer, sXMLEntry *XMLEntry)
{
	// 1 - tag's name (starts after '<' and ends by ' ', '>', '/' (/>), '\t')
	unsigned long TagNameEnd = Buffer.find_first_of(" />\t");
	XMLEntry->Name = Buffer.substr(1, TagNameEnd - 1);
	if (CalculateHash) {
		XMLEntry->NameHash = constexpr_hash_djb2a(XMLEntry->Name.c_str());
		auto tmpHash = HashCheckMap.find(XMLEntry->NameHash);
		if (tmpHash == HashCheckMap.end())
			HashCheckMap.emplace(XMLEntry->NameHash, XMLEntry->Name);
		else if (tmpHash->second != XMLEntry->Name)
			std::cerr << "Warning! Detected hash collision for tag name: "
				  << XMLEntry->Name << " and tag name: "
				  << tmpHash->second << "\n";
	}

	// 2 - check for attributes
	unsigned long i = TagNameEnd;
	while ((Buffer[i] != '>') || (Buffer[i] != '\0')) {
		// skip all ' ' and '\t'
		while (((Buffer[i] == ' ') || (Buffer[i] == '\t')) && (Buffer[i] != '\0')) {
			i++;
		}
		if (Buffer[i] == '\0') {
			std::cerr << __func__ << "(): " << "XML file corrupted, line: "
				  << LineNumber << "\n";
			break;
		}
		// check one more time for tag's close symbol
		if ((Buffer[i] == '>') || (!strncmp(Buffer.data() + i, "/>", strlen("/>"))))
			break;

		// attribute name
		unsigned long AttribNameStart = i;
		while ((Buffer[i] != '=') && (Buffer[i] != '\0')) {
			i++;
		}
		if (Buffer[i] == '\0') {
			std::cerr << __func__ << "(): " << "XML file corrupted, line: "
				  << LineNumber << "\n";
			break;
		}
		unsigned long AttribNameEnd = i;
		// skip all symbols till '\'' or '\"', should be next after '='
		i += 2;
		unsigned long AttribDataStart = i;
		while ((Buffer[i] != '\'') && (Buffer[i] != '\"') && (Buffer[i] != '\0')) {
			i++;
		}
		if (Buffer[i] == '\0') {
			std::cerr << __func__ << "(): " << "XML file corrupted, line: "
				  << LineNumber << "\n";
			break;
		}
		unsigned long AttribDataEnd = i;
		i++;

		// add attribute to entry
		std::string tmpAttrIndex = Buffer.substr(AttribNameStart, AttribNameEnd - AttribNameStart);
		XMLEntry->Attributes[tmpAttrIndex] = Buffer.substr(AttribDataStart, AttribDataEnd - AttribDataStart);
	}

	// 3 - store current tag's line number for
	XMLEntry->LineNumber = LineNumber;

	// 4 - check tag's close symbol
	if (Buffer.find("/>") != std::string::npos)
		return false;

	return true;
}

/*
 * Parse tag content.
 * We start from root and parse all children tags recursively.
 */
bool cXMLDocument::ParseTagContent(const std::string &OriginBuffer, unsigned long StartPosition,
				   const std::string &Buffer, sXMLEntry *ParentXMLEntry)
{
	// 1 - text content
	if (Buffer.find("<") == std::string::npos) {
		ParentXMLEntry->Content = Buffer;
		return true;
	}

	// 2 - if tag's open symbol found, parse recursively
	unsigned long CurrentBufferPosition{0};
	while (CurrentBufferPosition < Buffer.size()) {
		// locate tag open and close symbols
		unsigned long DetectTagOpenSymbol = Buffer.find("<", CurrentBufferPosition);

		// if no tag's open symbols - nothing to do
		if (DetectTagOpenSymbol == std::string::npos)
			return true;

		// check for comment
		if (!strncmp(Buffer.data() + DetectTagOpenSymbol, "<!--", strlen("<!--"))) {
			// find tag's close symbol
			unsigned long DetectCommentCloseSymbol = Buffer.find("-->", CurrentBufferPosition);
			if (DetectCommentCloseSymbol == std::string::npos) {
				std::cerr << __func__ << "(): "
					  << "XML file corrupted, can't find comment end in line "
					  << GetLineNumber(OriginBuffer,
							   StartPosition + CurrentBufferPosition)
					  << "\n";
				return false;
			}
			CurrentBufferPosition = DetectCommentCloseSymbol + strlen("-->");
			// FIXME continue in the middle of the cycle
			continue;
		}

		unsigned long DetectTagCloseSymbol = Buffer.find(">", CurrentBufferPosition);
		// something wrong, xml don't have a tag's close symbol
		if (DetectTagCloseSymbol == std::string::npos) {
			std::cerr << __func__ << "(): "
				  << "XML file corrupted, can't find element end for element in line "
				  << GetLineNumber(OriginBuffer,
						   StartPosition + CurrentBufferPosition)
				  << "\n";
			return false;
		}
		DetectTagCloseSymbol += strlen(">");

		// create new entry
		sXMLEntry *XMLEntry{nullptr};
		if (!ParentXMLEntry) {
			RootXMLEntry.reset(new sXMLEntry);
			XMLEntry = RootXMLEntry.get();
		} else {
			// NOTE emplace_back() return reference to the inserted element (since C++17)
			ParentXMLEntry->ChildrenList.emplace_back();
			XMLEntry = &ParentXMLEntry->ChildrenList.back();
		}

		// parce tag's line
		std::string TagString = Buffer.substr(DetectTagOpenSymbol,
						      DetectTagCloseSymbol - DetectTagOpenSymbol);
		unsigned int LineNumber = GetLineNumber(OriginBuffer,
							StartPosition + DetectTagOpenSymbol);
		bool ElementHaveContent = ParseTagLine(LineNumber, TagString, XMLEntry);

		// if we have tag's close element, we done here
		if (!ElementHaveContent) {
			CurrentBufferPosition = DetectTagCloseSymbol;
			// FIXME continue in the middle of the cycle
			continue;
		}

		// tag opened, look at tag's close element - </tag>
		std::string CloseElement{"</" + XMLEntry->Name + ">"};
		auto CloseElementPosition = Buffer.find(CloseElement, CurrentBufferPosition);
		// something wrong, xml don't have tag's close element
		if (CloseElementPosition == std::string::npos) {
			std::cerr << __func__ << "(): "
				  << "XML file corrupted, can't find element end: "
				  << XMLEntry->Name
				  << " in line: "
				  << GetLineNumber(OriginBuffer,
						   StartPosition + CurrentBufferPosition)
				  << "\n";
			return false;
		}

		// if we have some content inside tag - parse it
		if (DetectTagCloseSymbol < CloseElementPosition) {
			std::string ElementContent = Buffer.substr(DetectTagCloseSymbol,
								   CloseElementPosition - DetectTagCloseSymbol);
			if (!ParseTagContent(OriginBuffer,
					     StartPosition + DetectTagCloseSymbol,
					     ElementContent, XMLEntry)) {
				return false;
			}
		}

		CurrentBufferPosition = CloseElementPosition + CloseElement.size();
	}

	return false;
}

/*
 * Load XML from file.
 */
cXMLDocument::cXMLDocument(const std::string &XMLFileName, bool Hash)
{
	CalculateHash = Hash;

	std::cout << "Open XML file: " << XMLFileName << "\n";

	// just to be sure, release any previous data
	RootXMLEntry.reset();

	// read all data into buffer
	std::string Buffer;
	if (vw_VFStoBuffer(XMLFileName, Buffer)) {
		std::cerr << __func__ << "(): " << "XML file not found: " << XMLFileName << "\n";
		return;
	}

	// check header
	if (Buffer.find("<?xml") == std::string::npos) {
		std::cerr << __func__ << "(): " << "XML file corrupted: " << XMLFileName << "\n";
		return;
	}
	if (Buffer.find("?>") == std::string::npos) {
		std::cerr << __func__ << "(): " << "XML file corrupted: " << XMLFileName << "\n";
		return;
	}

	// start recursive load
	std::string tmpBuffer{Buffer.substr(Buffer.find("?>") + strlen("?>"))};
	if (!ParseTagContent(Buffer, Buffer.find("?>") + strlen("?>"), tmpBuffer, nullptr)) {
		std::cerr << __func__ << "(): " << "XML file corrupted: " << XMLFileName << "\n";
		return;
	}

	if (!RootXMLEntry.get())
		std::cerr << __func__ << "(): " << "XML file corrupted, root element not found: " << XMLFileName << "\n";

	if (CalculateHash) {
		std::cout << "Hash table for XML file " << XMLFileName << "\n";
		for (const auto &tmpHash : HashCheckMap) {
			std::cout << tmpHash.first << " : " << tmpHash.second << "\n";
		}
		std::cout << "\n";
		HashCheckMap.clear();
	}
}

/*
 * Save XML elements to file recursively.
 */
void cXMLDocument::SaveRecursive(const sXMLEntry &XMLEntry, std::ofstream &File, unsigned int Level)
{
	if (XMLEntry.EntryType == eEntryType::Comment) {
		// comment
		for (unsigned int i = 0; i < Level; i++) {
			File << "    ";
		}
		File << "<!--" << XMLEntry.Name << "-->" << EndLine;
	} else {
		// regular element

		// name
		for (unsigned int i = 0; i < Level; i++) {
			File << "    ";
		}
		File << "<" << XMLEntry.Name;

		// attributes
		if (!XMLEntry.Attributes.empty()) {
			for (const auto &TmpAttrib : XMLEntry.Attributes) {
				File << " " << TmpAttrib.first << "=\"" << TmpAttrib.second << "\"";
			}
			File << " ";
		}

		// data
		if (!XMLEntry.ChildrenList.empty() || !XMLEntry.Content.empty()) {
			if (!XMLEntry.Content.empty()) {
				File << ">" << XMLEntry.Content << "</" << XMLEntry.Name << ">" << EndLine.data();
			} else {
				File << ">" << EndLine;
				for (auto &tmpEntry : XMLEntry.ChildrenList) {
					SaveRecursive(tmpEntry, File, Level + 1);
				}
				for (unsigned int i = 0; i < Level; i++) {
					File << "    ";
				}
				File << "</" << XMLEntry.Name << ">" << EndLine;
			}
		} else
			File << "/>" << EndLine;
	}
}

/*
 * Save XML to file (libSDL RWops).
 */
bool cXMLDocument::Save(const std::string &XMLFileName)
{
	std::cout << "Save XML file: " << XMLFileName << "\n";

	std::ofstream File(XMLFileName);
	if (File.fail()) {
		std::cerr << __func__ << "(): " << "Can't open XML file for write " << XMLFileName << "\n";
		return false;
	}

	// XML header
	File << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << EndLine;

	if (!RootXMLEntry.get())
		return true;

	// save all data recursively
	SaveRecursive(*RootXMLEntry.get(), File, 0);

	return true;
}

} // viewizard namespace
