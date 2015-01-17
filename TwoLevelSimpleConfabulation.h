#ifndef TWOLEVELSIMPLECONFABULATION_H
#define TWOLEVELSIMPLECONFABULATION_H

#include "ForwardConfabulation.h"

class TwoLevelSimpleConfabulation : public ForwardConfabulation
{
public:
    TwoLevelSimpleConfabulation(size_t num_word_modules, const std::string &symbol_file, const std::string &master_file);
    TwoLevelSimpleConfabulation(const TwoLevelSimpleConfabulation& rhs) = delete;
    TwoLevelSimpleConfabulation& operator=(const TwoLevelSimpleConfabulation& rhs) = delete;
    TwoLevelSimpleConfabulation(TwoLevelSimpleConfabulation&& rhs) = delete;
    TwoLevelSimpleConfabulation&& operator=(TwoLevelSimpleConfabulation&& rhs) = delete;

protected:
    std::vector<std::unique_ptr<Module>> words_;
    std::vector<std::unique_ptr<Module>> phrases_;

    std::vector<std::vector<std::unique_ptr<KnowledgeBase>>> words_to_words_;
    std::vector<std::vector<std::unique_ptr<KnowledgeBase>>> phrases_to_phrases_;
    std::vector<std::vector<std::unique_ptr<KnowledgeBase>>> words_to_phrases_;
    std::vector<std::vector<std::unique_ptr<KnowledgeBase>>> phrases_to_words_;
};

#endif // TWOLEVELSIMPLECONFABULATION_H
