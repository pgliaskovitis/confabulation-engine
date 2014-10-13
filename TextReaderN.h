/*
 * Copyright 2014 Periklis Liaskovitis
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

#ifndef TEXTREADERN_H
#define TEXTREADERN_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
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

class TextReaderN
{
public:
    TextReaderN(const std::string& symbol_file, const std::string& master_file);

    void Initialize();

    const std::vector<Symbol> GetNextSentenceTokens();

private:
    void HandleSymbolFile();
    void HandleMasterFile();

    const std::vector<Symbol> ExtractTokens(const Symbol& input);
    void InitializeFileStream(std::ifstream& file);
    void CleanToken(Symbol& input);
    void RemoveCommonAbbreviations(Symbol& input);

    std::string symbol_file_name_;
    std::string master_file_name_;

    std::string all_delimiter_symbols_;
    std::vector<Symbol> delimiter_symbols_;

    std::vector<std::string> text_file_names_;
    std::vector<std::string>::iterator current_text_file_it_;
    std::unique_ptr<std::ifstream> current_text_file_;

    std::shared_ptr<Symbol> left_over_sentence_;
};

#endif // TEXTREADERN_H
