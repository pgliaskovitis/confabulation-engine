#include "TwoLevelSimpleConfabulation.h"
#include "Globals.h"
#include "Dbg.h"

TwoLevelSimpleConfabulation::TwoLevelSimpleConfabulation(size_t num_word_modules,
                                                         const std::string &symbol_file,
                                                         const std::string &master_file,
                                                         unsigned short min_single_occurrences,
                                                         unsigned short min_multi_occurrences)
    : num_word_modules_(num_word_modules)
{
    num_modules_ = 2 * num_word_modules;

    // knowledge base specification
    std::vector<std::vector<bool>> kb_specs;
    kb_specs.resize(num_modules_);
    for (size_t i = 0; i < num_modules_; ++i)
        kb_specs[i].resize(num_modules_);

    // word-to-word knowledge bases
    for (size_t i = 0; i < num_word_modules; ++i)
        for (size_t j = i + 1; j < num_word_modules; ++j)
            kb_specs[i][j] = true;

    // word-to-past-phrase knowledge bases
    for (size_t i = 0; i < num_word_modules; ++i)
        for (size_t j = num_word_modules + i; j >= num_word_modules; --j)
            kb_specs[i][j] = true;

    // word-to-future-phrase knowledge bases
    for (size_t i = 0; i < num_word_modules; ++i)
        for (size_t j = num_word_modules + i + 1; j < num_modules_; ++j)
            kb_specs[i][j] = true;

    // phrase-to-word knowledge bases
    for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i)
        for (size_t j = i - num_word_modules; j < num_word_modules; ++j)
            kb_specs[i][j] = true;

    // phrase-to-phrase knowledge bases
    for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i)
        for (size_t j = i + 1; j < 2 * num_word_modules; ++j)
            kb_specs[i][j] = true;

    std::vector<unsigned short> level_sizes;
    level_sizes.push_back(num_word_modules);
    level_sizes.push_back(num_word_modules);

    Initialize(kb_specs, level_sizes, symbol_file, master_file, min_single_occurrences, min_multi_occurrences);
}

std::vector<std::string> TwoLevelSimpleConfabulation::Confabulation(const std::vector<std::string> &symbols, int index_to_complete, bool expectation)
{
    std::vector<std::string> result;
    if (!CheckArguments(symbols, index_to_complete)) {
        std::cout << "Input sentence does not satisfy conditions for confabulation with this architecture";
        return result;
    }

    int index;
    if (index_to_complete < 0) {
        index = AutoIndexToComplete(symbols);
    } else {
        index = index_to_complete;
    }

    std::vector<std::string> temp_input(symbols.begin(), symbols.end());

    for (size_t i = 0; i < Globals::kMaxMultiWordSize; ++i) {

        int actual_K = ActualK(temp_input, index);
        const std::unique_ptr<Module>& target_module = modules_[index];
        target_module->ExcitationsToZero();

        // activate known symbols from input
        Activate(temp_input);

        // ATTEMPT I
        // // find expectation on all phrase modules before word module at index
        //  for (size_t m = num_word_modules_; m < num_word_modules_ + index; ++m) {
        //    TransferAllExcitations(m, modules_[m]);
        //
        //for (size_t m = num_word_modules_; m < num_word_modules_ + index; ++m)
        //    modules_[m]->PartialConfabulation(num_word_modules_ + index - m, false);

        // ATTEMPT II (more in accordance with multi-confabulation)
        // find expectation on phrase module above word module at index
        for (size_t n = 0; n < index; ++n)
            TransferExcitation(modules_[n], knowledge_bases_[n][num_word_modules_ + index], modules_[num_word_modules_ + index]);

        modules_[num_word_modules_ + index]->PartialConfabulation(1, false);

        // find expectation on unknown word module
        TransferAllExcitations(index, target_module);

        if (expectation) {
            result = target_module->PartialConfabulation(actual_K, false);
        } else {
            std::string next_word = target_module->ElementaryConfabulation(actual_K);
            result.push_back(next_word);
            temp_input.push_back(next_word);
        }

        ++index;

        Clean();
    }

    return result;
}
