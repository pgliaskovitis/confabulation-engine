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

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <string>
#include <memory>
#include <vector>

//forward declarations
class KnowledgeManager;
class TextReader;
class NGramHandler;

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

class Globals
{
public:
    static const Symbol kSentenceDelimiters;
    static const Symbol kTokenDelimiters;
    static const std::vector<Symbol> kCommonAbbreviations;
    static const Symbol kDummy;

    static const unsigned short kLevelSize;
    static const float kBaseProb;
    static const unsigned int kBandGap;
    static const unsigned short kHeapResults;

    TextReader& get_text_reader();
    KnowledgeManager& get_knowledge_manager();
    NGramHandler& get_ngram_handler();

    void set_text_reader(std::shared_ptr<TextReader> text_reader);
    void set_knowledge_manager(std::shared_ptr<KnowledgeManager> knowledge_manager);
    void set_ngram_handler(std::shared_ptr<NGramHandler> ngram_handler);

private:
    std::shared_ptr<KnowledgeManager> knowledge_manager_;

    std::shared_ptr<TextReader> text_reader_;

    std::shared_ptr<NGramHandler> ngram_handler_;
};

#endif /* GLOBALS_H_ */
