#include "TwoLevelMultiConfabulation.h"
#include "Globals.h"
#include "Dbg.h"

const size_t TwoLevelMultiConfabulation::reference_frame_length = 4;

TwoLevelMultiConfabulation::TwoLevelMultiConfabulation(size_t num_word_modules,
                                                       const std::string &symbol_file,
                                                       const std::string &master_file,
                                                       uint8_t min_single_occurrences,
                                                       uint8_t min_multi_occurrences)
    : num_word_modules_(num_word_modules)
{
    num_modules_ = 2 * num_word_modules;

    // knowledge base specification
    std::vector<std::vector<bool>> kb_specs;
    kb_specs.resize(num_modules_);
    for (size_t i = 0; i < num_modules_; ++i) {
        kb_specs[i].resize(num_modules_);
    }

    // word-to-future-word knowledge bases (reference frame length ahead)
    for (size_t i = 0; i < num_word_modules; ++i) {
        for (size_t j = i + 1; j < num_word_modules && j < i + 1 + reference_frame_length; ++j) {
            kb_specs[i][j] = true;
        }
    }

    // word-to-past-word knowledge bases (reference frame length ago)
    for (size_t i = 1; i < num_word_modules; ++i) {
        for (int j = i - 1; j >= 0 && j >= ConvertToSigned(i) - ConvertToSigned(reference_frame_length); --j) {
            kb_specs[i][j] = true;
        }
    }

    // word-to-future-phrase knowledge bases (reference frame length ahead)
    for (size_t i = 0; i < num_word_modules; ++i) {
        for (size_t j = num_word_modules + i + 1; j < 2 * num_word_modules && j < num_word_modules + i + 1 + reference_frame_length; ++j) {
            kb_specs[i][j] = true;
        }
    }

    // word-to-past-phrase knowledge bases (reference frame length ago)
    for (size_t i = 0; i < num_word_modules; ++i) {
        for (size_t j = num_word_modules + i; j >= num_word_modules && j >= num_word_modules + (i - reference_frame_length); --j) {
            kb_specs[i][j] = true;
        }
    }

    // phrase-to-phrase knowledge bases (reference frame length ahead)
    for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i) {
        for (size_t j = i + 1; j < 2 * num_word_modules && j < i + 1 + reference_frame_length; ++j) {
            kb_specs[i][j] = true;
        }
    }

    // phrase-to-word knowledge bases (only directly below and directly next to below)
    for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i) {
        kb_specs[i][i - num_word_modules] = true;
        if (i - num_word_modules < num_word_modules - 1) {
            kb_specs[i][i - num_word_modules + 1];
        }
    }

    std::vector<uint8_t> level_sizes;
    level_sizes.push_back(num_word_modules);
    level_sizes.push_back(num_word_modules);

    Initialize(kb_specs, level_sizes, symbol_file, master_file, min_single_occurrences, min_multi_occurrences);
}

std::vector<std::string> TwoLevelMultiConfabulation::Confabulation(const std::vector<std::string> &symbols, int8_t index_to_complete, bool expectation)
{
    std::vector<std::string> result;
    std::vector<std::string> result_backward_word;
    std::vector<std::string> result_backward_phrase;

    size_t current_result_size = std::numeric_limits<size_t>::max();
    size_t previous_result_size = 0;

    if (!CheckArguments(symbols, index_to_complete)) {
        std::cout << "Input sentence does not satisfy conditions for confabulation with this architecture";
        return result;
    }

    int8_t index;
    if (index_to_complete < 0) {
        index = AutoIndexToComplete(symbols);
    } else {
        index = index_to_complete;
    }

    std::vector<std::string> temp_input(symbols.begin(), symbols.end());

    int32_t actual_K = std::min<int32_t>(ActualK(temp_input, index), ConvertToSigned(reference_frame_length));
    modules_[index]->ExcitationsToZero();

    // activate known symbols from input
    Activate(temp_input);

    // find expectation on phrase module above word module at index
    TransferAllExcitations(num_word_modules_ + index, modules_[num_word_modules_ + index]);
    modules_[num_word_modules_ + index]->PartialConfabulation(1, false);

    // find expectation on unknown word module
    TransferAllExcitations(index, modules_[index]);
    modules_[index]->PartialConfabulation(1, false);

    // find expectation on next to unknown word module
    TransferExcitation(modules_[index], knowledge_bases_[index][index + 1], modules_[index + 1]);
    modules_[index + 1]->PartialConfabulation(1, false);

    // tighten expectation on target phrase and word modules
    do {
        previous_result_size = current_result_size;

        TransferExcitation(modules_[index + 1], knowledge_bases_[index + 1][index], modules_[index]);
        result_backward_word = modules_[index]->PartialConfabulation(1, true);

        TransferExcitation(modules_[index + 1], knowledge_bases_[index + 1][num_word_modules_ + index], modules_[num_word_modules_ + index]);
        result_backward_phrase = modules_[num_word_modules_ + index]->PartialConfabulation(1, true);

        current_result_size = result_backward_word.size() + result_backward_phrase.size();

        TransferExcitation(modules_[num_word_modules_ + index], knowledge_bases_[num_word_modules_ + index][index], modules_[index]);
        modules_[index]->PartialConfabulation(1, true);

        TransferExcitation(modules_[index],  knowledge_bases_[index][index + 1], modules_[index + 1]);
        modules_[index + 1]->PartialConfabulation(1, true);

    } while (current_result_size < previous_result_size);

    if (expectation) {
        result.insert(result.begin(), result_backward_word.begin(), result_backward_word.end());
        result.insert(result.end(), result_backward_phrase.begin(),  result_backward_phrase.end());
    } else {
        result.push_back("{");
        std::string next_word = modules_[index]->ElementaryConfabulation(actual_K);
        result.push_back(next_word);
        result.push_back("}");

        result.push_back("{");
        std::string next_phrase =  modules_[num_word_modules_ + index]->ElementaryConfabulation(actual_K);
        result.push_back(next_phrase);
        result.push_back("}");
    }

    Clean();

    return result;
}

