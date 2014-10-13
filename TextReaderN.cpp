#include <sstream>
#include "TextReaderN.h"
#include "Globals.h"
#include "SentenceTokenizer.h"

TextReaderN::TextReaderN(const std::string& symbol_file, const std::string& master_file) :
    symbol_file_name_(symbol_file),
    master_file_name_(master_file),
    current_text_file_it_(text_file_names_.begin()),
    left_over_sentence_(nullptr)
{
}

void TextReaderN::Initialize()
{
    HandleSymbolFile();
    HandleMasterFile();
}

const std::vector<Symbol> TextReaderN::GetNextSentenceTokens()
{
    if (current_text_file_ != nullptr) {

        while (current_text_file_->good()) {
            std::string sentence;
            std::stringstream l_string;

            if (left_over_sentence_ != nullptr)
                l_string << *left_over_sentence_;

            Symbol::size_type end_of_sentence = Symbol::npos;
            Symbol::size_type end_of_delimiter = Symbol::npos;
            Symbol::size_type end_of_phase = Symbol::npos;

            while (end_of_sentence == Symbol::npos) {
                std::getline(*current_text_file_, sentence);
                RemoveCommonAbbreviations(sentence);
                l_string << " " << sentence;
                end_of_sentence = l_string.str().find_first_of(Globals::kSentenceDelimiters, 0);
            }

            end_of_delimiter = l_string.str().find_first_not_of(Globals::kSentenceDelimiters, end_of_sentence);

            if (end_of_delimiter != Symbol::npos) {
                //delimiter finishes in the current line
                std::shared_ptr<Symbol> tempLeftover(new Symbol(l_string.str().substr(end_of_delimiter)));
                left_over_sentence_ = tempLeftover; //store the part of the line from the end of the delimiter to npos
                end_of_phase = end_of_delimiter;
            } else {
                //delimiter does not finish in the current line
                std::shared_ptr<Symbol> tempLeftover(new Symbol(l_string.str().substr(end_of_sentence + 1)));
                left_over_sentence_ = tempLeftover; //store the part of the line from the start of the delimiter to npos
                end_of_phase = end_of_sentence + 1;
            }

            const std::vector<Symbol>& current_sentence_tokens = ExtractTokens(l_string.str().substr(0, end_of_phase));

#ifdef DEBUG_1_H_
            std::cout << "------- New sentence confabulation symbols START -------" << "\n";
            for (std::vector<Symbol>::const_iterator it = currentSentenceTokens.begin(); it != currentSentenceTokens.end(); ++it)
                std::cout << "---->" << (*it) << "<----\n";
            std::cout << "------- New sentence confabulation symbols END -------" << "\n";
#endif
            return current_sentence_tokens;
        }

        current_text_file_->close();
        current_text_file_.reset(nullptr);
        ++current_text_file_it_;
        left_over_sentence_ = nullptr;

        return GetNextSentenceTokens();
    } else {
        if (current_text_file_it_ != text_file_names_.end()) {
            current_text_file_.reset(new std::ifstream(*current_text_file_it_));
            current_text_file_->exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
            InitializeFileStream(*current_text_file_);

            return GetNextSentenceTokens();
        } else
            return {""};
    }
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

const std::vector<Symbol> TextReaderN::ExtractTokens(const Symbol &input)
{
    std::vector<Symbol> output;

    SentenceTokenizer tok(input);

    while (tok.Tokenize(Globals::kTokenDelimiters)) {
#ifdef STOREDELIMITERS_
        if (tok.delim() != "") {
            output.push_back(tok.delim());
            //std::cout << "Found non-empty delimiter \"" << tok.delim() << "\"\n";
        }
#endif
        Symbol newToken = tok.Str();
        //std::cout << newToken << std::endl;
        CleanToken(newToken);
        output.push_back(newToken);
    }

    return output;
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