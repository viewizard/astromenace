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
	along with AstroMenace. If not, see <http://www.gnu.org/licenses/>.


	Web Site: http://www.viewizard.com/
	Project: https://github.com/viewizard/astromenace
	E-mail: viewizard@viewizard.com

*************************************************************************************/

// TODO translate comments
// TODO revise ParseTagContent() in order to use main buffer and begin/end position
//      don't create temporary string buffers

// FIXME GetLineNumber() should work with current parser changes

#include "../vfs/vfs.h"
#include "xml.h"

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
	// 1 - получаем имя тэга (начинается сразу после символа <, а заканчивается пробелом, >, />, или символом таб)
	unsigned long TagNameEnd = Buffer.find_first_of(" />\t");
	XMLEntry->Name = Buffer.substr(1, TagNameEnd - 1);

	// 2 - проверяем наличие атрибутов и заносим их в динамический массив
	unsigned long i = TagNameEnd;
	while ((Buffer[i] != '>') || (Buffer[i] != '\0')) {
		// пропускаем все пробелы и табы
		while (((Buffer[i] == ' ') || (Buffer[i] == '\t')) && (Buffer[i] != '\0')) {
			i++;
		}
		if (Buffer[i] == '\0') {
			std::cerr << __func__ << "(): " << "XML file corrupted, line: "
				  << LineNumber << "\n";
			break;
		}
		// еще раз проверяем, возможно завершение тэга ставили через пробел или таб
		if ((Buffer[i] == '>') || (!strncmp(Buffer.data() + i, "/>", strlen("/>"))))
			break;

		// находим имя атрибута
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
		// пропускаем все до кавычек (они у нас следующие, после знака равенства)
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

		// собираем новый атрибут и подключаем его к элементу
		std::string tmpAttrIndex = Buffer.substr(AttribNameStart, AttribNameEnd - AttribNameStart);
		XMLEntry->Attributes[tmpAttrIndex] = Buffer.substr(AttribDataStart, AttribDataEnd - AttribDataStart);
	}

	// 3 - определяем и номер строки
	XMLEntry->LineNumber = LineNumber;

	// 4 - определить есть ли в ней атрибут закрытия '/', или у нас есть еще и контент и закрывающий тэг
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
	// 1 - это просто контент, заносим данные и выходим из рекурсии
	if (Buffer.find("<") == std::string::npos) {
		ParentXMLEntry->Content = Buffer;
		return true;
	}
	// 2 - если в строке нашли открывающий символ тэга - идем на рекурсивную обработку строки с хмл данными

	// в цикле, пока не достигнем конца обрабатываемой строки:
	unsigned long CurrentBufferPosition{0};
	while (CurrentBufferPosition < Buffer.size()) {
		// находим положение открывающего тэг символа и закрывающего
		unsigned long DetectTagOpenSymbol = Buffer.find("<", CurrentBufferPosition);

		// если в строке уже нет открывающих символов - просто выходим, все проверили
		if (DetectTagOpenSymbol == std::string::npos)
			return true;

		// это может быть комментарий, проверяем
		if (!strncmp(Buffer.data() + DetectTagOpenSymbol, "<!--", strlen("<!--"))) {
			// ищем завершающую часть, и сразу перемещаемся к ней
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
		// если был открывающий символ, но нет закрывающего - это ошибка структуры документа
		if (DetectTagCloseSymbol == std::string::npos) {
			std::cerr << __func__ << "(): "
				  << "XML file corrupted, can't find element end for element in line "
				  << GetLineNumber(OriginBuffer,
						   StartPosition + CurrentBufferPosition)
				  << "\n";
			return false;
		}
		DetectTagCloseSymbol += strlen(">");

		// создаем новый элемент и подключаем его к родительскому
		sXMLEntry *XMLEntry{nullptr};
		if (!RootXMLEntry.get() && !ParentXMLEntry) {
			RootXMLEntry.reset(new sXMLEntry);
			XMLEntry = RootXMLEntry.get();
		} else {
			// NOTE emplace_back() return reference to the inserted element (since C++17)
			ParentXMLEntry->ChildrenList.emplace_back();
			XMLEntry = &ParentXMLEntry->ChildrenList.back();
		}

		// полученные данные передаем на обработку и анализ строки элемента
		std::string TagString = Buffer.substr(DetectTagOpenSymbol,
						      DetectTagCloseSymbol - DetectTagOpenSymbol);
		unsigned int LineNumber = GetLineNumber(OriginBuffer,
							StartPosition + CurrentBufferPosition);
		bool ElementHaveContent = ParseTagLine(LineNumber, TagString, XMLEntry);

		// если у нас закрытый тэг - с этим элементом закончили, идем искать дальше
		if (!ElementHaveContent) {
			CurrentBufferPosition = DetectTagCloseSymbol;
			// FIXME continue in the middle of the cycle
			continue;
		}

		// если тэг открытый - ищем завершающий тэг </имя>
		std::string CloseElement{"</" + XMLEntry->Name + ">"};
		auto CloseElementPosition = Buffer.find(CloseElement, CurrentBufferPosition);
		// если закрывающего элемента нет - значит файл поврежден
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

		// передаем данные на рекурсивную обработку (если закрывающий тэг не стоит сразу после открывающего)
		if (DetectTagCloseSymbol < CloseElementPosition) {
			std::string ElementContent = Buffer.substr(DetectTagCloseSymbol,
								   CloseElementPosition - DetectTagCloseSymbol);
			if (!ParseTagContent(OriginBuffer,
					     StartPosition + CurrentBufferPosition,
					     ElementContent, XMLEntry)) {
				// вернули с ошибкой, выходим
				return false;
			}
		}

		// смещаем буфер
		CurrentBufferPosition = CloseElementPosition + CloseElement.size();
	}

	return false;
}

/*
 * Load XML from file.
 */
cXMLDocument::cXMLDocument(const std::string &XMLFileName)
{
	std::cout << "Open XML file: " << XMLFileName << "\n";

	// just to be sure we release any previous data
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
}

/*
 * Save XML elements to file recursively.
 */
void cXMLDocument::SaveRecursive(const sXMLEntry &XMLEntry, SDL_RWops *File, unsigned int Level)
{
	if (XMLEntry.EntryType == eEntryType::Comment) {
		// comment
		for (unsigned int i = 0; i < Level; i++) {
			SDL_RWwrite(File, "    ", strlen("    "), 1);
		}
		SDL_RWwrite(File, "<!--", strlen("<!--"), 1);
		SDL_RWwrite(File, XMLEntry.Name.data(), XMLEntry.Name.size(), 1);
		SDL_RWwrite(File, "-->", strlen("-->"), 1);
		SDL_RWwrite(File, EndLine.data(), EndLine.size(), 1);
	} else {
		// regular element

		// name
		for (unsigned int i = 0; i < Level; i++) {
			SDL_RWwrite(File, "    ", strlen("    "), 1);
		}
		SDL_RWwrite(File, "<", strlen("<"), 1);
		SDL_RWwrite(File, XMLEntry.Name.data(), XMLEntry.Name.size(), 1);

		// attributes
		if (!XMLEntry.Attributes.empty()) {
			for (const auto &TmpAttrib : XMLEntry.Attributes) {
				SDL_RWwrite(File, " ", strlen(" "), 1);
				SDL_RWwrite(File, TmpAttrib.first.data(), TmpAttrib.first.size(), 1);
				SDL_RWwrite(File, "=\"", strlen("=\""), 1);
				SDL_RWwrite(File, TmpAttrib.second.data(), TmpAttrib.second.size(), 1);
				SDL_RWwrite(File, "\"", strlen("\""), 1);
			}
			SDL_RWwrite(File, " ", strlen(" "), 1);
		}

		// data
		if (!XMLEntry.ChildrenList.empty() || !XMLEntry.Content.empty()) {
			if (!XMLEntry.Content.empty()) {
				SDL_RWwrite(File, ">", strlen(">"), 1);
				SDL_RWwrite(File, XMLEntry.Content.data(), XMLEntry.Content.size(), 1);
				SDL_RWwrite(File, "</", strlen("</"), 1);
				SDL_RWwrite(File, XMLEntry.Name.data(), XMLEntry.Name.size(), 1);
				SDL_RWwrite(File, ">", strlen(">"), 1);
				SDL_RWwrite(File, EndLine.data(), EndLine.size(), 1);
			} else {
				SDL_RWwrite(File, ">", strlen(">"), 1);
				SDL_RWwrite(File, EndLine.data(), EndLine.size(), 1);
				for (auto &tmpEntry : XMLEntry.ChildrenList) {
					SaveRecursive(tmpEntry, File, Level + 1);
				}
				for (unsigned int i = 0; i < Level; i++) {
					SDL_RWwrite(File, "    ", strlen("    "), 1);
				}
				SDL_RWwrite(File, "</", strlen("</"), 1);
				SDL_RWwrite(File, XMLEntry.Name.data(), XMLEntry.Name.size(), 1);
				SDL_RWwrite(File, ">", strlen(">"), 1);
				SDL_RWwrite(File, EndLine.data(), EndLine.size(), 1);
			}
		} else {
			SDL_RWwrite(File, "/>", strlen("/>"), 1);
			SDL_RWwrite(File, EndLine.data(), EndLine.size(), 1);
		}
	}
}

/*
 * Save XML to file (libSDL RWops).
 */
bool cXMLDocument::Save(const std::string &XMLFileName)
{
	std::cout << "Save XML file: " << XMLFileName << "\n";

	SDL_RWops *File = SDL_RWFromFile(XMLFileName.c_str(), "wb");
	if (!File) {
		std::cerr << __func__ << "(): " << "Can't open XML file for write " << XMLFileName << "\n";
		return false;
	}

	// XML header
	const std::string tmpXMLHeader{"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"};
	SDL_RWwrite(File, tmpXMLHeader.data(), tmpXMLHeader.size(), 1);
	SDL_RWwrite(File, EndLine.data(), EndLine.size(), 1);

	if (!RootXMLEntry.get()) {
		SDL_RWclose(File);
		return true;
	}

	// save all data recursively
	SaveRecursive(*RootXMLEntry.get(), File, 0);

	SDL_RWclose(File);
	return true;
}
