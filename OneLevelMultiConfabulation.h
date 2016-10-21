#ifndef ONELEVELMULTICONFABULATION_H
#define ONELEVELMULTICONFABULATION_H

#include "ForwardConfabulation.h"

class OneLevelMultiConfabulation : public ForwardConfabulation
{
public:
    OneLevelMultiConfabulation(size_t num_word_modules,
                                const std::string &symbol_file,
                                const std::string &master_file,
                                uint8_t min_single_occurrences,
                                uint8_t min_multi_occurrences);
    OneLevelMultiConfabulation(const OneLevelMultiConfabulation& rhs) = delete;
    OneLevelMultiConfabulation& operator=(const OneLevelMultiConfabulation& rhs) = delete;
    OneLevelMultiConfabulation(OneLevelMultiConfabulation&& rhs) = delete;
    OneLevelMultiConfabulation&& operator=(OneLevelMultiConfabulation&& rhs) = delete;

    virtual std::vector<std::string> Confabulation(const std::vector<std::string>& symbols, int8_t index_to_complete, bool expectation);

private:
    size_t num_word_modules_;

    std::vector<std::string> BasicSwirlAtIndex(int index);
    std::vector<std::string> FullSwirlAtIndex(int index);
    std::vector<std::string> FullSwirlOverMultipleIndices(int index, int span);

    std::vector<std::string> BasicSwirlOverMultipleIndices(int index, int span);
};

#endif // ONELEVELMULTICONFABULATION_H
