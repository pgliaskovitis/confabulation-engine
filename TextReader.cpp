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

#include "TextReader.h"
#include "NGramHandler.h"
#include "SentenceTokenizer.h"
#include "KnowledgeManager.h"
#include "Globals.h"
#include <memory>

TextReader::TextReader(Globals& globalsManager) : globals_manager_(globalsManager)
{}

void TextReader::InitializeFileStream(std::ifstream& file)
{
	file.clear(file.goodbit);
}

//this assumes a seed file that contains all delimiter symbols
void TextReader::HandleSymbolFile(const Symbol& seedname)
{
	std::ifstream l_file;
	Symbol symbol;

	l_file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
    try {
		l_file.open(seedname.c_str());
        InitializeFileStream(l_file);
        if (l_file.is_open()) {
            while (l_file.good()) {
				std::getline(l_file, symbol);
                all_delimiter_symbols_.append(symbol); //delimiters as single string
                delimiter_symbols_.push_back(symbol); // delimiters as separate strings
				//std::cout << "My delimiters are now: " << m_allDelimiterSymbols << "\n";
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

//this assumes a master seed file that contains the names of all other text files
void TextReader::HandleAllTextFiles(const Symbol& seedname)
{
	std::ifstream l_file;
	Symbol filename;
	
	l_file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
    try {
		l_file.open(seedname.c_str());
        InitializeFileStream(l_file);
        if (l_file.is_open()) {
            while (l_file.good()) {
				std::getline(l_file, filename);
                HandleAllSentences(filename);
                std::cout << "Just handled file " << filename << ", Multi-word count is: " << globals_manager_.get_ngram_handler().get_multi_word_count() << "\n";
				std::cout.flush();
			}
			l_file.close();
		}

    } catch (std::ifstream::failure& e) {
		if (!l_file.eofbit)
			std::cerr << "Exception opening/reading/closing file\n";
	    l_file.close();
	}

}

//this works inside each text file
void TextReader::HandleAllSentences(const Symbol& filename)
{
	std::ifstream l_file;
	Symbol sentence;

	l_file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
    try {
		l_file.open(filename.c_str());
        InitializeFileStream(l_file);
        if (l_file.is_open()) {
            std::shared_ptr<Symbol> leftOverSentence = nullptr;
            while (l_file.good()) {
				std::stringstream l_string;

                if (leftOverSentence != nullptr)
					l_string << *leftOverSentence;

				Symbol::size_type endOfSentence = Symbol::npos;
				Symbol::size_type endOfDelimiter = Symbol::npos;
				Symbol::size_type endOfPhase = Symbol::npos;

                while (endOfSentence == Symbol::npos) {
					std::getline(l_file, sentence);
                    RemoveCommonAbbreviations(sentence);
					l_string << " " << sentence;
                    endOfSentence = l_string.str().find_first_of(Globals::kSentenceDelimiters, 0);
				}

                endOfDelimiter = l_string.str().find_first_not_of(Globals::kSentenceDelimiters, endOfSentence);

                if (endOfDelimiter != Symbol::npos) {
                    //delimiter finishes in the current line
					std::shared_ptr<Symbol> tempLeftover(new Symbol(l_string.str().substr(endOfDelimiter)));
                    leftOverSentence = tempLeftover; //store the part of the line from the end of the delimiter to npos
					endOfPhase = endOfDelimiter;
                } else {
                    //delimiter does not finish in the current line
					std::shared_ptr<Symbol> tempLeftover(new Symbol(l_string.str().substr(endOfSentence + 1)));
                    leftOverSentence = tempLeftover; //store the part of the line from the start of the delimiter to npos
					endOfPhase = endOfSentence + 1;
				}

                const std::vector<Symbol>& currentSentenceTokens = ExtractSentenceTokens(l_string.str().substr(0, endOfPhase));

#ifdef DEBUG_1_H_
				std::cout << "------- New sentence confabulation symbols START -------" << "\n";
				for (std::vector<Symbol>::const_iterator it = currentSentenceTokens.begin(); it != currentSentenceTokens.end(); ++it)
                    std::cout << "---->" << (*it) << "<----\n";
				std::cout << "------- New sentence confabulation symbols END -------" << "\n";
#endif

				//const std::vector<std::shared_ptr<std::set<Symbol>>> mySymbolSet = m_globalsManager.getKnowledgeManager()->getAllSymbols();
				//std::cout << "Symbols are now: ";
				//for (std::set<Symbol>::iterator it = mySymbolSet[0]->begin(); it != mySymbolSet[0]->end(); ++it) {
				//	std::cout << *it << "\n";
				//}

                globals_manager_.get_ngram_handler().ExtractAndStoreNGrams(currentSentenceTokens);
                globals_manager_.get_knowledge_manager().EnrichKnowledge(currentSentenceTokens);
			}
			l_file.close();
		}

    } catch (std::ifstream::failure& e) {
		if (!l_file.eofbit)
			std::cerr << "Exception opening/reading/closing file\n";

		l_file.close();
	}

}

const std::vector<Symbol> TextReader::ExtractSentenceTokens(const Symbol& input)
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

void TextReader::CleanToken(Symbol& input)
{
	input.erase(std::remove(input.begin(), input.end(), '\0'), input.end());
	input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());
	input.erase(std::remove(input.begin(), input.end(), '\t'), input.end());
	std::transform(input.begin(), input.end(), input.begin(), ::tolower);
}

void TextReader::RemoveCommonAbbreviations(Symbol& input)
{
	size_t tempIndex = 0;
	size_t firstIndex = Symbol::npos;
	Symbol abbreviatedSymbol;

    while (tempIndex != Symbol::npos) {
		tempIndex = 0;
        for (std::vector<Symbol>::const_iterator it = Globals::kCommonAbbreviations.begin(); it != Globals::kCommonAbbreviations.end(); ++it) {
			/* Locate the substring to replace. */
			tempIndex = input.find(*it, 0);
            if (tempIndex < firstIndex) {
				firstIndex = tempIndex;
				abbreviatedSymbol = (*it);
			}
		}

		if (firstIndex == Symbol::npos) break;

		/* Make the replacement. */
		if ((abbreviatedSymbol != "A.D.") && (abbreviatedSymbol != "B.C."))
			input.replace(firstIndex, abbreviatedSymbol.size(), abbreviatedSymbol.substr(0, abbreviatedSymbol.size() - 1));
        else if (abbreviatedSymbol == "A.D.")
			input.replace(firstIndex, abbreviatedSymbol.size(), "AD");
		else if (abbreviatedSymbol == "B.C.")
			input.replace(firstIndex, abbreviatedSymbol.size(), "BC");
    }
}
