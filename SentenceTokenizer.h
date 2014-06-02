/*
 * SentenceTokenizer.h
 *
 *  Created on: Jun 1, 2013
 *      Author: Periklis G. Liaskovitis
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
