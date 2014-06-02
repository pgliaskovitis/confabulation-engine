/*
 * Globals.h
 *
 *  Created on: Jun 1, 2013
 *      Author: Periklis G. Liaskovitis
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <string>
#include <memory>
#include <vector>

//forward declarations
class KnowledgeManager;
class TextReader;

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

    static const unsigned short kLevelSize;
    static const float kBaseProb;
    static const float kBandGap;
    static const unsigned short kHeapResults;

    TextReader& get_text_reader();
    KnowledgeManager& get_knowledge_manager();

    void set_text_reader(std::shared_ptr<TextReader> text_reader); //this will internally take ownership of the pointer
    void set_knowledge_manager(std::shared_ptr<KnowledgeManager>& knowledge_manager); //this will internally take ownership of the pointer

private:
    std::shared_ptr<KnowledgeManager> knowledge_manager_;

    std::shared_ptr<TextReader> text_reader_;
};

#endif /* GLOBALS_H_ */
