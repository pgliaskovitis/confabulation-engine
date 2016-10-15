#ifndef TWOLEVELMULTICONFABULATION_H
#define TWOLEVELMULTICONFABULATION_H

#include "ForwardConfabulation.h"

class TwoLevelMultiConfabulation : public ForwardConfabulation
{
public:
    TwoLevelMultiConfabulation(size_t num_word_modules,
                                const std::string &symbol_file,
                                const std::string &master_file,
                                uint8_t min_single_occurrences,
                                uint8_t min_multi_occurrences);
    TwoLevelMultiConfabulation(const TwoLevelMultiConfabulation& rhs) = delete;
    TwoLevelMultiConfabulation& operator=(const TwoLevelMultiConfabulation& rhs) = delete;
    TwoLevelMultiConfabulation(TwoLevelMultiConfabulation&& rhs) = delete;
    TwoLevelMultiConfabulation&& operator=(TwoLevelMultiConfabulation&& rhs) = delete;

    virtual std::vector<std::string> Confabulation(const std::vector<std::string>& symbols, int8_t index_to_complete, bool expectation);

private:
    size_t num_word_modules_;

    size_t BasicSwirlAtIndex(int index, size_t initial_word_progression, size_t initial_phrase_progression);

    size_t BasicTransitionAtIndex(int index, size_t initial_word_progression, size_t initial_phrase_progression);
};

#endif // TWOLEVELMULTICONFABULATION_H
