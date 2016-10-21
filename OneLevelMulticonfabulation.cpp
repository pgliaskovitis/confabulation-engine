#include "OneLevelMultiConfabulation.h"
#include "Globals.h"
#include "Dbg.h"

OneLevelMultiConfabulation::OneLevelMultiConfabulation(size_t num_word_modules,
                                                       const std::string &symbol_file,
                                                       const std::string &master_file,
                                                       uint8_t min_single_occurrences,
                                                       uint8_t min_multi_occurrences)
    : num_word_modules_(num_word_modules)
{
    num_modules_ = num_word_modules;

    // knowledge base specification
    std::vector<std::vector<bool>> kb_specs;
    kb_specs.resize(num_modules_);
    for (size_t i = 0; i < num_modules_; ++i) {
        kb_specs[i].resize(num_modules_);
    }

    // word-to-future-word knowledge bases (reference frame length ahead)
    for (size_t i = 0; i < num_word_modules; ++i) {
        for (size_t j = i + 1; j < num_word_modules && j < i + 1 + Globals::kReferenceFrameSize; ++j) {
            kb_specs[i][j] = true;
        }
    }

    // word-to-past-word knowledge bases (max phrase length ago)
    for (size_t i = 1; i < num_word_modules; ++i) {
        for (int j = i - 1; j >= 0 && j >= ConvertToSigned(i) - ConvertToSigned(Globals::kMaxMultiWordSize); --j) {
            kb_specs[i][j] = true;
        }
    }

    std::vector<uint8_t> level_sizes;
    level_sizes.push_back(num_word_modules);

    Initialize(kb_specs, level_sizes, symbol_file, master_file, min_single_occurrences, min_multi_occurrences);
}

std::vector<std::string> OneLevelMultiConfabulation::Confabulation(const std::vector<std::string> &symbols, int8_t index_to_complete, bool expectation)
{
    std::vector<std::string> result;

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

    for (; index < num_word_modules_;) {
        int8_t actual_K = ActualK(temp_input, index);
        modules_[index]->ExcitationsToZero();

        // activate known symbols from input
        Activate(temp_input);

        // find initial expectation on word module at index
        TransferAllExcitations(index, modules_[index]);
        modules_[index]->AdditivePartialConfabulation(Globals::kMaxMultiWordSize);

        if (index + 1 < num_word_modules_) {
            FullSwirlAtIndex(index);

            if (index + 2 < num_word_modules_) {
                FullSwirlAtIndex(index + 1);
                FullSwirlOverMultipleIndices(index, 2);

                if (index + 3 < num_word_modules_) {
                    FullSwirlAtIndex(index + 2);
                    FullSwirlOverMultipleIndices(index, 3);
                }
            }
        }

        // one final excitation boost
        if (index + 3 < num_word_modules_) {
            TransferExcitation(modules_[index + 3],
                               knowledge_bases_[index + 3][index],
                               modules_[index]);
        }

        if (index + 2 < num_word_modules_) {
            TransferExcitation(modules_[index + 2],
                               knowledge_bases_[index + 2][index],
                               modules_[index]);
        }

        if (index + 1 < num_word_modules_) {
            TransferExcitation(modules_[index + 1],
                               knowledge_bases_[index + 1][index],
                               modules_[index]);
        }

        if (!expectation) {
            float word_excitation;
            std::string next_word = modules_[index]->ElementaryConfabulation(actual_K, &word_excitation);

            result.push_back("{");
            result.push_back(next_word);
            temp_input.push_back(next_word);
            ++index;
            result.push_back("}");
        }

        Clean();
    }

    return result;
}

// tighten expectation on target word modules once
std::vector<std::string> OneLevelMultiConfabulation::BasicSwirlAtIndex(int index)
{
    std::vector<std::string> result;

    TransferExcitation(modules_[index],
                       knowledge_bases_[index][index + 1],
                       modules_[index + 1]);
    modules_[index + 1]->AdditivePartialConfabulation(1);

    TransferExcitation(modules_[index + 1],
                       knowledge_bases_[index + 1][index],
                       modules_[index]);
    result = modules_[index]->AdditivePartialConfabulation(1);

    return result;
}

// tighten expectation on target word modules continuously
std::vector<std::string> OneLevelMultiConfabulation::FullSwirlAtIndex(int index)
{
    std::vector<std::string> result;
    size_t current_result_size = std::numeric_limits<size_t>::max();
    size_t previous_result_size = 0;

    do {
        previous_result_size = current_result_size;
        result = BasicSwirlAtIndex(index);
        current_result_size = result.size();
    } while (current_result_size < previous_result_size);

    return result;
}

// tighten expectation from index + span towards index
std::vector<std::string> OneLevelMultiConfabulation::FullSwirlOverMultipleIndices(int index, int span)
{
    std::vector<std::string> result = modules_[index]->AdditivePartialConfabulation(0);
    size_t current_result_size = result.size();
    size_t previous_result_size = 0;

    do {
        previous_result_size = current_result_size;

        TransferExcitation(modules_[index + span],
                           knowledge_bases_[index + span][index],
                           modules_[index]);
        result = modules_[index]->AdditivePartialConfabulation(1);
        current_result_size = result.size();

        if (current_result_size == previous_result_size) {
            break;
        }

        BasicSwirlOverMultipleIndices(index, span);
    } while (current_result_size < previous_result_size);

    return result;
}

std::vector<std::string> OneLevelMultiConfabulation::BasicSwirlOverMultipleIndices(int index, int span)
{
    std::vector<std::string> result;

    for (int cursor = 0; cursor < span; ++cursor) {
        if (cursor == 0) {
            result = BasicSwirlAtIndex(index + cursor);
        } else {
            BasicSwirlAtIndex(index + cursor);
        }
    }

    return result;
}
