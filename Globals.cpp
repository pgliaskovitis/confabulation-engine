/*
 * Globals.cpp
 *
 *  Created on: Jun 1, 2013
 *      Author: Periklis G. Liaskovitis
 */

#include "Globals.h"
#include "KnowledgeManager.h"
#include "TextReader.h"

const Symbol Globals::kSentenceDelimiters = "\".!?;";
const Symbol Globals::kTokenDelimiters = " ,`\".!?;@#$%^&*()-:/\\_=+~[]{}<>0123456789";
const std::vector<Symbol> Globals::kCommonAbbreviations = {"Mr.", "Mrs.", "Dr.", "etc.", "Jr.", "Sr.", "Co.", "Ltd.", "A.D.", "B.C."};
const unsigned short Globals::kLevelSize = 15;
const float Globals::kBaseProb = 0.0001;
const float Globals::kBandGap = 1.0;
const unsigned short Globals::kHeapResults = 3;

KnowledgeManager& Globals::get_knowledge_manager()
{
    return *knowledge_manager_;
}

void Globals::set_knowledge_manager(std::shared_ptr<KnowledgeManager>& knowledge_manager)
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
