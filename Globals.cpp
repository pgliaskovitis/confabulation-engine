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

#include "Globals.h"
#include "KnowledgeManager.h"
#include "TextReader.h"
#include "NGramHandler.h"

const Symbol Globals::kSentenceDelimiters = "\".!?;";
const Symbol Globals::kTokenDelimiters = " ,`\".!?;@#$%^&*()-:/\\_=+~[]{}<>0123456789";
const std::vector<Symbol> Globals::kCommonAbbreviations = {"Mr.", "Mrs.", "Dr.", "etc.", "Jr.", "Sr.", "Co.", "Ltd.", "A.D.", "B.C."};
const Symbol Globals::kDummy = "DummySymbol";
const unsigned short Globals::kLevelSize = 15;
const float Globals::kBaseProb = 0.0001;
const unsigned int Globals::kBandGap = 1;
const unsigned short Globals::kHeapResults = 3;

KnowledgeManager& Globals::get_knowledge_manager()
{
    return *knowledge_manager_;
}

void Globals::set_knowledge_manager(std::shared_ptr<KnowledgeManager> knowledge_manager)
{
    knowledge_manager_ = knowledge_manager;
}

TextReader& Globals::get_text_reader()
{
    return *text_reader_;
}

void Globals::set_text_reader(std::shared_ptr<TextReader> text_reader)
{
    text_reader_ = text_reader;
}

NGramHandler& Globals::get_ngram_handler()
{
    return *ngram_handler_;
}

void Globals::set_ngram_handler(std::shared_ptr<NGramHandler> ngram_handler)
{
    ngram_handler_ = ngram_handler;
}
