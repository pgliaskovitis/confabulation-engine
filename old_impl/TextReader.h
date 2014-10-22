/*
 * Copyright 2014 Periklis G. Liaskovitis
 *
 * This file is part of confab-engine.
 *
 * confab-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * confab-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with confab-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEXTREADER_H_
#define TEXTREADER_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>

class Globals;

#ifndef DEBUG_1_H_
//#define DEBUG_1_H_
#endif

#ifndef STOREDELIMITERS_
//#define STOREDELIMITERS_
#endif

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

// StringTokenizer takes a sentence and breaks it into words
class TextReader
{
public:

	TextReader(Globals& globalsManager);

	//Reads all symbols from a symbol file
    void HandleSymbolFile(const Symbol& seedname);

	//Reads all file names from a single seed file
    void HandleAllTextFiles(const Symbol& seedname);

	//Reads all sentences from a single input file, named in the argument
    void HandleAllSentences(const Symbol& filename);

	//Breaks a single sentence into words
    const std::vector<Symbol> ExtractSentenceTokens(const Symbol& input);

private:
    Globals& globals_manager_;
    Symbol all_delimiter_symbols_;
    std::vector<Symbol> delimiter_symbols_;

    void InitializeFileStream(std::ifstream& file);
    void CleanToken(Symbol& input);
    void RemoveCommonAbbreviations(Symbol& input);
};

#endif /*TEXTREADER_H_*/
