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

    const std::vector<Symbol> ExtractNextSentenceTokens();

private:
    void HandleSymbolFile();
    void HandleMasterFile();

    void InitializeFileStream(std::ifstream& file);
    void CleanToken(Symbol& input);
    void RemoveCommonAbbreviations(Symbol& input);

    std::string symbol_file_name_;
    std::string master_file_name_;

    std::string all_delimiter_symbols_;
    std::vector<Symbol> delimiter_symbols_;

    std::vector<std::string> text_file_names_;
    unsigned int current_text_file_index_;
    std::unique_ptr<std::ifstream> current_text_file_;
};

#endif // TEXTREADERN_H
