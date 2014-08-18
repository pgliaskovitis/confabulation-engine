#include "TextReaderN.h"
#include "Globals.h"

TextReaderN::TextReaderN(const std::string& symbol_file, const std::string& master_file) :
    symbol_file_name_(symbol_file),
    master_file_name_(master_file),
    current_text_file_index_(0)
{
}

void TextReaderN::Initialize()
{
    HandleSymbolFile();
    HandleMasterFile();
}

const std::vector<Symbol> TextReaderN::ExtractNextSentenceTokens()
{

}

void TextReaderN::HandleSymbolFile()
{
    std::ifstream l_file;
    Symbol symbol;

    l_file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
    try {
        l_file.open(symbol_file_name_);
        InitializeFileStream(l_file);
        if (l_file.is_open()) {
            while (l_file.good()) {
                std::getline(l_file, symbol);
                all_delimiter_symbols_.append(symbol); //delimiters as single string
                delimiter_symbols_.push_back(symbol); // delimiters as separate strings
            }
            l_file.close();
        }

    } catch (std::ifstream::failure& e)	{
        if (!l_file.eofbit)
            std::cerr << "Exception opening/reading/closing file\n";

        l_file.close();
    }

    //sort delimiters
    std::sort(delimiter_symbols_.begin(), delimiter_symbols_.end());
}

void TextReaderN::HandleMasterFile()
{
    std::ifstream l_file;
    Symbol filename;

    l_file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
    try {
        l_file.open(master_file_name_);
        InitializeFileStream(l_file);
        if (l_file.is_open()) {
            while (l_file.good()) {
                std::getline(l_file, filename);
                text_file_names_.push_back(filename);
            }
            l_file.close();
        }

    } catch (std::ifstream::failure& e) {
        if (!l_file.eofbit)
            std::cerr << "Exception opening/reading/closing file\n";
        l_file.close();
    }
}

void TextReaderN::InitializeFileStream(std::ifstream &file)
{
    file.clear(file.goodbit);
}

void TextReaderN::CleanToken(Symbol& input)
{
    input.erase(std::remove(input.begin(), input.end(), '\0'), input.end());
    input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());
    input.erase(std::remove(input.begin(), input.end(), '\t'), input.end());
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
}

void TextReaderN::RemoveCommonAbbreviations(Symbol& input)
{
    size_t temp_index = 0;
    size_t first_index = Symbol::npos;
    Symbol abbreviated_symbol;

    while (temp_index != Symbol::npos) {
        temp_index = 0;
        for (std::vector<Symbol>::const_iterator it = Globals::kCommonAbbreviations.begin(); it != Globals::kCommonAbbreviations.end(); ++it) {
            /* Locate the substring to replace. */
            temp_index = input.find(*it, 0);
            if (temp_index < first_index) {
                first_index = temp_index;
                abbreviated_symbol = (*it);
            }
        }

        if (first_index == Symbol::npos) break;

        /* Make the replacement. */
        if ((abbreviated_symbol != "A.D.") && (abbreviated_symbol != "B.C."))
            input.replace(first_index, abbreviated_symbol.size(), abbreviated_symbol.substr(0, abbreviated_symbol.size() - 1));
        else if (abbreviated_symbol == "A.D.")
            input.replace(first_index, abbreviated_symbol.size(), "AD");
        else if (abbreviated_symbol == "B.C.")
            input.replace(first_index, abbreviated_symbol.size(), "BC");
    }
}
