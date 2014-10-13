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

#ifndef SENTENCETOKENIZER_H_
#define SENTENCETOKENIZER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <locale>

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

class SentenceTokenizer
{
public:

	explicit SentenceTokenizer(const Symbol& source);
	SentenceTokenizer(const SentenceTokenizer& stok);

	//Extract the next token.
	//Returns whether or not it was successful.
    bool Tokenize(const Symbol& delimiters);
	
	//Extract the next token of a (structured) persisted knowledgebase file
	// Returns whether or not it was successful
    const std::vector<Symbol> KnowledgeTokenize(const std::vector<Symbol>& persistence_delimiters);

	//Get the last-read token
    const Symbol& Str() const;

	//Get the delimiter following the last-read token
    const Symbol& Delim() const;

    static bool IsSymbolAlphanumeric(const Symbol& input);

private:
    Symbol source_; //our copy of the source
    Symbol::size_type index_; //where we are at in our copy of the source
    Symbol token_; //a copy of the last token found in our copy of the source
    Symbol delimiter_; //a copy of the last delimiter symbol, i.e., the one following m_token

    unsigned int knowledge_extraction_phase_;

    Symbol ExtractDelimiterToken(size_t begin, size_t end);

};

#endif /* SENTENCETOKENIZER_H_ */
