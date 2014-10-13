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

#include "SentenceTokenizer.h"

SentenceTokenizer::SentenceTokenizer(const Symbol& source) : source_(source), index_(0)
{
}

SentenceTokenizer::SentenceTokenizer(const SentenceTokenizer& stok) : source_(stok.source_), index_(stok.index_)
{
}

bool SentenceTokenizer::IsSymbolAlphanumeric(const Symbol& input)
{

      if (input.size() == 0)
		  return false;

      for (std::string::const_iterator it = input.begin(); it != input.end(); ++it)
        if (!std::isalpha(*it))
	    	return false;

	  return true;
}

const Symbol& SentenceTokenizer::Str() const
{
    return token_;
}

const Symbol& SentenceTokenizer::Delim() const
{
    return delimiter_;
}

bool SentenceTokenizer::Tokenize(const Symbol& delimiters)
{
	size_t index0 = 0;

    /* _DEBUG
     *
	 *
	size_t n = 0;
	std::cout << "Delimiter list for tokenizer: " << "\n";
	for (vector<Symbol>::const_iterator it = delimiterSymbols.begin(); it != delimiterSymbols.end(); ++it) {
	  	std::cout << (*it) << " or " << delimiterSymbols.at(n++) << "\n";
	}
	       *
	       *
	_DEBUG */

	// Find the beginning of the next token, if any
    if (index_ == Symbol::npos) return false;

	/* _DEBUG
	 *
	 *
	if (m_source.at(m_index) != ' ') {
		std::cout << "Current cursor shows " << m_source.at(m_index) << " at " << m_index << "\n";
	}
	 	  *
	 	  *
	_DEBUG*/

    index0 = source_.find_first_not_of(delimiters, index_);

    // Find the end of the token, if any
    if (index0 == Symbol::npos) return false;

	/* _DEBUG
	 *
	 *
	if (m_source.at(m_index) != ' ') {
		std::cout << "Next cursor shows " << m_source.at(index0) << " at " << index0 << "\n";
	}
	 	  *
	 	  *
	_DEBUG*/

	//Save the result on the delimiter
    delimiter_ = ExtractDelimiterToken(index_, index0);

	/* _DEBUG
	 *
	 *
	if (m_delimiter != " ") {
		std::cout << "Non-alphanumeric token = " << m_delimiter << "\n";
	}
	 	  *
	 	  *
	_DEBUG*/

    index_ = source_.find_first_of(delimiters, index0);

	/* _DEBUG
	 *
	 *
	std::cout << "m_index = " << m_index << " index0 = " << index0 << "\n";
	 	  *
	 	  *
	_DEBUG*/

    // Save the result on the token
    if (index_ == std::string::npos)
        token_ = source_.substr(index0);
        //std::cout << "m_token infinite is " << m_token << "\n";
    else {
        size_t length_token = index_ - index0;
        token_ = source_.substr(index0, length_token);
        //std::cout << "m_token finite is " << m_token << "\n";
    }

    return true;
}

const std::vector<Symbol> SentenceTokenizer::KnowledgeTokenize(const std::vector<Symbol>& persistence_delimiters)
{
	size_t index0 = 0;
    int knowledge_extraction_phase = -1;
    std::vector<Symbol> extracted_tokens;

    std::string current_delimiter;
    std::string next_delimiter;

    while (true) {
		// Find the beginning of the next token, if any
        if (index_ == Symbol::npos) break;

		//this is the bulk of the logic, for selecting the correct structured delimiters
		//structured knowledge format is
		//source_symbol:::target_symbol_1(link_count|||total_target_count)___target_symbol_2
        switch (knowledge_extraction_phase) {
            case -1: { //source_symbol
                current_delimiter = "";
                next_delimiter = persistence_delimiters[knowledge_extraction_phase + 1];
                knowledge_extraction_phase++;
				break;
			}
            case 0: { //":::"
                current_delimiter = persistence_delimiters[knowledge_extraction_phase];
                next_delimiter = persistence_delimiters[knowledge_extraction_phase + 1];
                knowledge_extraction_phase++;
				break;
			}
            case 1: { //"{"
                current_delimiter = persistence_delimiters[knowledge_extraction_phase];
                next_delimiter = persistence_delimiters[knowledge_extraction_phase + 1];
                knowledge_extraction_phase++;
				break;
			}
            case 2: { //"|||"
                current_delimiter = persistence_delimiters[knowledge_extraction_phase];
                next_delimiter = persistence_delimiters[knowledge_extraction_phase + 1];
                knowledge_extraction_phase++;
				break;
			}
            case 3: { //}---->"
                current_delimiter = persistence_delimiters[knowledge_extraction_phase];
                next_delimiter = persistence_delimiters[1];
                knowledge_extraction_phase = 1;
				break;
			}
		}

        index0 = source_.find_first_not_of(current_delimiter, index_);

		// Find the end of the token, if any
		if (index0 == Symbol::npos) break;
        index_ = source_.find_first_of(next_delimiter, index0);

	    // Save the result on the token
        if (index_ == std::string::npos) {
            token_ = source_.substr(index0);
            extracted_tokens.push_back(token_);
	        //std::cout << "m_token infinite is " << m_token << std::endl;
        } else {
            size_t length_token = index_ - index0;
            token_ = source_.substr(index0, length_token);
            extracted_tokens.push_back(token_);
	        //std::cout << "m_token finite is " << m_token << std::endl;
	    }
	}

    return extracted_tokens;
}

Symbol SentenceTokenizer::ExtractDelimiterToken(size_t begin, size_t end)
{
	Symbol delimiter_partial;

	Symbol::size_type length_delim = (end == Symbol::npos) ? Symbol::npos : (end - begin);
    delimiter_partial = source_.substr(begin, length_delim);

    size_t str_begin = delimiter_partial.find_first_not_of(' ');
    if (str_begin == std::string::npos)
		return ""; // no content in the delimiter other than white space

    size_t str_end = delimiter_partial.find_last_not_of(' ');
    size_t str_range = str_end - str_begin + 1;

    return delimiter_partial.substr(str_begin, str_range);
}
