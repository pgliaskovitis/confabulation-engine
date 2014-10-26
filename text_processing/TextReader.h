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

#ifndef TEXTREADERN_H
#define TEXTREADERN_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#ifndef DEBUG_1_H_
//#define DEBUG_1_H_
#endif

#ifndef STOREDELIMITERS_
//#define STOREDELIMITERS_
#endif

class TextReader
{
public:
    TextReader(const std::string& symbol_file, const std::string& master_file);

    void Initialize();

    const std::vector<std::string> GetNextSentenceTokens(bool& finished_reading);
    const std::vector<std::string> ExtractTokens(const std::string& input);

private:
    void HandleSymbolFile();
    void HandleMasterFile();

    void CleanToken(std::string& input);
    void RemoveCommonAbbreviations(std::string& input);

    std::string symbol_file_name_;
    std::string master_file_name_;

    std::string all_delimiter_symbols_;
    std::vector<std::string> delimiter_symbols_;

    std::vector<std::string> text_file_names_;
    std::vector<std::string>::iterator current_text_file_it_;
    std::unique_ptr<std::ifstream> current_text_file_;

    std::shared_ptr<std::string> left_over_sentence_;
};

#endif // TEXTREADERN_H
