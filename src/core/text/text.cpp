/****************************************************************************

    AstroMenace
    Hardcore 3D space scroll-shooter with spaceship upgrade possibilities.
    Copyright (c) 2006-2019 Mikhail Kurinnoi, Viewizard


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

// TODO since we use std::unordered_map with hashes now, work with text should be revised -
//      TextID should be removed, and English text should be used as ID and "default" text

#include "../math/math.h"
#include "../vfs/vfs.h"
#include "../font/font.h"
#include "text.h"

namespace viewizard {

namespace {

// Map of Maps with all sorted language data in utf8.
std::unordered_map<unsigned int, std::unordered_map<std::string, std::string>> TextTable;
// Map of Maps with all sorted language data in utf32.
std::unordered_map<unsigned int, std::unordered_map<std::string, std::u32string>> TextTableUTF32;
// Current default language. English, by default.
unsigned int CurrentLanguage{0};
// all characters used in text for current language
std::unordered_set<char32_t> CharsSetForLanguage;

} // unnamed namespace

/*
 * Detect element in std::unordered_map<std::unordered_map<>>.
 */
template <typename T, typename C, typename R>
static bool isElementPresentInTable(T &Table, C Column, R &Row)
{
    return (Table.find(Column) != Table.end()
            && Table[Column].find(Row) != Table[Column].end());
}

/*
 * Unconditional rehash for std::unordered_map<std::unordered_map<>>.
 */
template <typename T>
static void UnconditionalRehash(T &Table)
{
    Table.rehash(0);
    for (auto &tmpColumn : Table) {
        tmpColumn.second.rehash(0);
    }
}

/*
 * Set default language.
 */
void vw_SetTextLanguage(unsigned int Language)
{
    CurrentLanguage = Language;
}

/*
 * Get available languages count.
 */
unsigned int vw_GetLanguageListCount()
{
    if (TextTable.empty()) {
        return 1; // English only
    }

    return static_cast<unsigned>(TextTable.size());
}

/*
 * Release data.
 */
void vw_ReleaseText()
{
    TextTable.clear();
    TextTableUTF32.clear();
    CurrentLanguage = 0; // English
}

/*
 * Create text table with utf32 data (convert TextTable data for utf8 to utf32).
 */
static void CreateTextTableUTF32()
{
    for (unsigned int i = 0; i < TextTable.size(); i++) {
        for (const auto tmpData : TextTable[i]) {
            TextTableUTF32[i][tmpData.first] = ConvertUTF8.from_bytes(tmpData.second);
        }
    }
    // unconditional rehash, at this line we have not rehashed map
    UnconditionalRehash(TextTableUTF32);
}

/*
 * Parse each row's block, separated by 1.SymbolSeparator, 2.SymbolEndOfLine, 3.EOF
 */
static int GetRowTextBlock(std::string &CurrentTextBlock, uint8_t *Data, long DataSize, long &i,
                           const char SymbolSeparator, const char SymbolEndOfLine)
{
    constexpr char SymbolQuotes{'\"'};

    // check quotes, since with quotes we have another sequence of reading
    bool InsideFieldTrigger{Data[i] == SymbolQuotes};
    if (InsideFieldTrigger) {
        i++; // skip quotes
    }

    // without quotes we read text till SymbolSeparator or SymbolEndOfLine
    for (; ((!InsideFieldTrigger && (Data[i] != SymbolSeparator) && (Data[i] != SymbolEndOfLine)) ||
            // if text braced by quotes, we read till end (second) of quotes
            (InsideFieldTrigger && (Data[i] != SymbolQuotes))) &&
            (i < DataSize); i++) {

        // special case, store '\n' instead of '\'+'n'
        if (!CurrentTextBlock.empty()
            && CurrentTextBlock.back() == '\\'
            && Data[i] == 'n') {
            CurrentTextBlock.back() = '\n';
        } else {
            CurrentTextBlock += Data[i];
        }
    }

    if (InsideFieldTrigger) {
        if (i >= DataSize) {
            std::cerr << __func__ << "(): " << "file end before end (second) of quotes.\n";
            return ERR_FILE_IO;
        }
        i++; // skip quotes
    }

    return 0;
}

/*
 * Initialization. Load file with translation in .csv format (supported by LibreOffice Calc).
 */
int vw_InitText(const char *FileName, const char SymbolSeparator, const char SymbolEndOfLine)
{
    vw_ReleaseText();

    // open and don't call vw_fclose(), use tmpFile->Data directly
    std::unique_ptr<cFILE> tmpFile = vw_fopen(FileName);
    if (!tmpFile) {
        return ERR_FILE_NOT_FOUND;
    }
    std::cout << "Load and parse .csv file " << FileName << "\n";

    // plain .csv file format parser
    // parse data by each byte, in order to use string as RowCode - build
    // RowCode first, only after that, initialize new TextTable row
    bool NeedBuildCurrentRowCode = true;
    std::string CurrentRowCode;
    unsigned int CurrentColumnNumber{0};
    unsigned int LineNumber{1}; // line number for error message
    for (long i = 0; i < tmpFile->GetSize(); i++) {
        // parse each row
        for (; (tmpFile->GetData()[i] != SymbolEndOfLine) && (i < tmpFile->GetSize()); i++) {
            // read text block in line, .csv line looks like:
            // text_block;text_block;...;text_blockSymbolEndOfLine
            // if text braced by quotes:
            // "text_block";"text_block";...;"text_block"SymbolEndOfLine
            std::string CurrentRowTextBlock{};
            if (GetRowTextBlock(CurrentRowTextBlock, tmpFile->GetData(), tmpFile->GetSize(), i,
                                SymbolSeparator, SymbolEndOfLine)) {
                std::cerr << __func__ << "(): " << "file corrupted.";
                vw_ReleaseText();
                return ERR_FILE_IO;
            }
            if (NeedBuildCurrentRowCode) {
                CurrentRowCode = CurrentRowTextBlock;
                // RowCode built, next blocks in this row contain data
                NeedBuildCurrentRowCode = false;
            }
            // we use column 0 with same row code, in order to make code simple and clear
            // plus, for UTF32 we need a column to store English UTF8 -> UTF32 conversion results
            TextTable[CurrentColumnNumber][CurrentRowCode] = CurrentRowTextBlock;
            CurrentColumnNumber++;
            // detect and skip duplicate line (if we already have this element created => this is duplicate)
            if (isElementPresentInTable(TextTable, CurrentColumnNumber, CurrentRowCode)) {
                std::cerr << __func__ << "(): " << "* Duplicate line detected, line number "
                          << LineNumber << "\n";
                for (; (tmpFile->GetData()[i] != SymbolEndOfLine) && (i < tmpFile->GetSize()); i++) {}
            }
            // we found SymbolEndOfLine in previous cycle, in order to prevent "i" changes, break cycle
            if (tmpFile->GetData()[i] == SymbolEndOfLine) {
                break;
            }
        }
        // move to next row
        CurrentColumnNumber = 0;
        NeedBuildCurrentRowCode = true;
        CurrentRowCode.clear();
        LineNumber++;
    }
    // unconditional rehash, at this line we have not rehashed map
    UnconditionalRehash(TextTable);

    CreateTextTableUTF32();

    std::cout << "Detected " << vw_GetLanguageListCount() << " languages:";
    for (unsigned int i = 0; i < TextTable.size(); i++) {
        std::cout << " " << vw_GetText("en", i);
    }
    std::cout << "\n\n";
    return 0;
}

/*
 * Get UTF8 text for particular language.
 */
const std::string &vw_GetText(const std::string &ItemID, unsigned int Language)
{
    if (Language > vw_GetLanguageListCount()) {
        Language = CurrentLanguage;
    }

    auto tmpText = TextTable[Language].find(ItemID);
    if (tmpText != TextTable[Language].end()) {
        return tmpText->second;
    }

    // ItemID should be added to TextTable for key and all languages, and
    // we should return pointer to this new entry, but not ItemID
    // make sure, vw_GetLanguageListCount() is called instead of size(),
    // since we could have an empty table (languages was not loaded)
    for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
        TextTable[i][ItemID] = ItemID;
    }

    std::cout << "Added to text table for all languages: \"" << ItemID << "\"\n";

    return TextTable[Language][ItemID];
}

/*
 * Get UTF32 text for particular language.
 */
const std::u32string &vw_GetTextUTF32(const std::string &ItemID, unsigned int Language)
{
    if (Language > vw_GetLanguageListCount()) {
        Language = CurrentLanguage;
    }

    auto tmpText = TextTableUTF32[Language].find(ItemID);
    if (tmpText != TextTableUTF32[Language].end()) {
        return tmpText->second;
    }

    // ItemID should be converted to UTF32 and added to TextTableUTF32 for all languages,
    // and we should return pointer to this new entry, but not TextTableUTF32Error
    std::u32string tmpTextUTF32 = ConvertUTF8.from_bytes(ItemID);
    // make sure, vw_GetLanguageListCount() is called instead of size(),
    // since we could have an empty table (languages was not loaded)
    for (unsigned int i = 0; i < vw_GetLanguageListCount(); i++) {
        TextTableUTF32[i][ItemID] = tmpTextUTF32;
    }

    std::cout << "Added to text table for all languages: \"" << ItemID << "\"\n";

    return TextTableUTF32[Language][ItemID];
}

/*
 * Find all used in text characters for current language.
 */
std::unordered_set<char32_t> &vw_FindCharsSetForLanguage()
{
    CharsSetForLanguage.clear();

// TODO provide additional sort by character's use frequency in text
//      We need this, since vw_GenerateFontChars() generate texture limited by size. Some
//      characters could be skipped and will use personal texture. In this way we will
//      forced to switch textures and draw small vertex buffers all the time (if character
//      frequently used in text and use personal texture for rendering).

    if (TextTableUTF32.empty()) {
        // default symbols for English, since we don't have text loaded
        std::string tmpSymbols{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
                               " .,!?-+\():;%&`'*#$=[]@^{}_~><–—«»“”|/©"};
        std::u32string tmpSymbolsUTF32 = ConvertUTF8.from_bytes(tmpSymbols);
        for (const auto &UTF32 : tmpSymbolsUTF32) {
            CharsSetForLanguage.insert(UTF32);
        }
    } else {
        for (const auto &tmpWords : TextTableUTF32[CurrentLanguage]) {
            for (const auto &UTF32 : tmpWords.second) {
                CharsSetForLanguage.insert(UTF32);
            }
        }
    }
    // unconditional rehash, at this line we have not rehashed set
    CharsSetForLanguage.rehash(0);

#ifndef NDEBUG
    for (const auto UTF32 : CharsSetForLanguage) {
        std::cout << "Detected character:  "
                  << ConvertUTF8.to_bytes(UTF32) << "  "
                  << "0x" << std::uppercase << std::hex << UTF32 << std::dec << "\n";
    }
#endif // NDEBUG

    return CharsSetForLanguage;
}

} // viewizard namespace
