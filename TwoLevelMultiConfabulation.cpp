#include "TwoLevelMultiConfabulation.h"
#include "Globals.h"
#include "Dbg.h"

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

    // word-to-future-phrase knowledge bases (max phrase length ahead)
    for (size_t i = 0; i < num_word_modules; ++i) {
        for (size_t j = num_word_modules + i + 1; j < 2 * num_word_modules && j < num_word_modules + i + 1 + Globals::kMaxMultiWordSize; ++j) {
            kb_specs[i][j] = true;
        }
    }

    // word-to-past-phrase knowledge bases (max phrase length ago)
    for (size_t i = 0; i < num_word_modules; ++i) {
        for (size_t j = num_word_modules + i; j >= num_word_modules && j >= num_word_modules + (i - Globals::kMaxMultiWordSize); --j) {
            kb_specs[i][j] = true;
        }
    }

    // phrase-to-phrase knowledge bases (reference frame length ahead - no single word phrases here)
    for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i) {
        for (size_t j = i + 2; j < 2 * num_word_modules && j < i + 1 + Globals::kReferenceFrameSize; ++j) {
            kb_specs[i][j] = true;
        }
    }

    // phrase-to-word knowledge bases (only directly below and directly next to below)
    for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i) {
        kb_specs[i][i - num_word_modules] = true;
    }

    std::vector<uint8_t> level_sizes;
    level_sizes.push_back(num_word_modules);
    level_sizes.push_back(num_word_modules);

    Initialize(kb_specs, level_sizes, symbol_file, master_file, min_single_occurrences, min_multi_occurrences);
}

std::vector<std::string> TwoLevelMultiConfabulation::Confabulation(const std::vector<std::string> &symbols, int8_t index_to_complete, bool expectation)
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
    int8_t end_completion = index + Globals::kMaxMultiWordSize;

    std::vector<std::string> temp_input(symbols.begin(), symbols.end());

    for (; index < end_completion;) {
        int8_t actual_K = ActualK(temp_input, index);
        int8_t initial_excitation_level = std::min<int8_t>(Globals::kMaxMultiWordSize, actual_K);

        std::vector<std::string> initial_result;
        do {
            modules_[index]->ExcitationsToZero();

            // activate known symbols from input
            Activate(temp_input);

            // find initial expectation on phrase module above word module at index
            TransferAllExcitations(num_word_modules_ + index, modules_[num_word_modules_ + index]);
            modules_[num_word_modules_ + index]->AdditivePartialConfabulation(1);

            // find initial expectation on word module at index (including phrase module above)
            TransferAllExcitations(index, modules_[index]);
            initial_result = modules_[index]->AdditivePartialConfabulation(initial_excitation_level);
            initial_excitation_level--;
        } while (initial_result.size() == 0 && initial_excitation_level > 0);

        if (initial_result.size() == 0) {
            return result;
        }

        if (index + 1 < num_word_modules_) {
            FullTransitionAtIndex(index);

            if (index + 2 < num_word_modules_) {
                FullTransitionAtIndex(index + 1);
                FullSwirlOverMultipleIndices(index, 2);

                if (index + 3 < num_word_modules_) {
                    FullTransitionAtIndex(index + 2);
                    FullSwirlOverMultipleIndices(index, 3);

                    if (index + 4 < num_word_modules_) {
                        FullSwirlAtIndex(index + 3);
                        FullSwirlOverMultipleIndices(index, 4);
                    }
                }
            }
        }

        // one final excitation boost
        if (index + 3 < num_word_modules_) {
            TransferExcitation(modules_[index + 3],
                               knowledge_bases_[index + 3][num_word_modules_ + index],
                               modules_[num_word_modules_ + index]);
            TransferExcitation(modules_[index + 3],
                               knowledge_bases_[index + 3][index],
                               modules_[index]);
        }

        if (index + 2 < num_word_modules_) {
            TransferExcitation(modules_[index + 2],
                               knowledge_bases_[index + 2][num_word_modules_ + index],
                               modules_[num_word_modules_ + index]);
            TransferExcitation(modules_[index + 2],
                               knowledge_bases_[index + 2][index],
                               modules_[index]);
        }

        if (index + 1 < num_word_modules_) {
            TransferExcitation(modules_[index + 1],
                               knowledge_bases_[index + 1][num_word_modules_ + index],
                               modules_[num_word_modules_ + index]);
            TransferExcitation(modules_[index + 1],
                               knowledge_bases_[index + 1][index],
                               modules_[index]);
        }

        TransferExcitation(modules_[index],
                           knowledge_bases_[index][num_word_modules_ + index],
                           modules_[num_word_modules_ + index]);

        if (!expectation) {
            float word_excitation;
            float phrase_excitation;
            std::string next_word = modules_[index]->ElementaryConfabulation(actual_K, &word_excitation);
            std::string next_phrase =  modules_[num_word_modules_ + index]->ElementaryConfabulation(actual_K, &phrase_excitation);

            result.push_back("{");
            if (word_excitation > phrase_excitation) {
                result.push_back(next_word);
                temp_input.push_back(next_word);
                ++index;
            } else {
                result.push_back(next_phrase);
                const std::vector<std::string>& result_tokens = SymbolToVectorSymbol(next_phrase, ' ');
                for (int m; m < result_tokens.size(); ++m) {
                    temp_input.push_back(result_tokens.at(m));
                }
                index += result_tokens.size();
            }
            result.push_back("}");
        }

        Clean();
    }

    return result;
}

// tighten expectation on target (phrase and) word modules once
std::vector<std::string> TwoLevelMultiConfabulation::BasicSwirlAtIndex(int index)
{
    std::vector<std::string> result;

    TransferExcitation(modules_[num_word_modules_ + index],
                       knowledge_bases_[num_word_modules_ + index][index],
                       modules_[index]);
    modules_[index]->AdditivePartialConfabulation(0);

    TransferExcitation(modules_[index],
                       knowledge_bases_[index][index + 1],
                       modules_[index + 1]);
    modules_[index + 1]->AdditivePartialConfabulation(1);

    TransferExcitation(modules_[index + 1],
                       knowledge_bases_[index + 1][num_word_modules_ + index],
                       modules_[num_word_modules_ + index]);
    modules_[num_word_modules_ + index]->AdditivePartialConfabulation(1);

    TransferExcitation(modules_[index + 1],
                       knowledge_bases_[index + 1][index],
                       modules_[index]);
    result = modules_[index]->AdditivePartialConfabulation(1);

    return result;
}

// tighten expectation on target (phrase and) word modules continuously
std::vector<std::string> TwoLevelMultiConfabulation::FullSwirlAtIndex(int index)
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
std::vector<std::string> TwoLevelMultiConfabulation::FullSwirlOverMultipleIndices(int index, int span)
{
    std::vector<std::string> result = modules_[index]->AdditivePartialConfabulation(0);
    size_t current_result_size = result.size();
    size_t previous_result_size = 0;

    do {
        previous_result_size = current_result_size;

        TransferExcitation(modules_[index + span],
                           knowledge_bases_[index + span][num_word_modules_ + index],
                           modules_[num_word_modules_ + index]);
        modules_[num_word_modules_ + index]->AdditivePartialConfabulation(1);
        TransferExcitation(modules_[index + span],
                           knowledge_bases_[index + span][index],
                           modules_[index]);
        result = modules_[index]->AdditivePartialConfabulation(1);
        current_result_size = result.size();

        if (current_result_size == previous_result_size) {
            break;
        }

        BasicTransitionOverMultipleIndices(index, span);
    } while (current_result_size < previous_result_size);

    return result;
}

std::vector<std::string> TwoLevelMultiConfabulation::BasicTransitionAtIndex(int index)
{
    const std::vector<std::string>& result = BasicSwirlAtIndex(index);

    TransferExcitation(modules_[index],
                       knowledge_bases_[index][num_word_modules_ + index + 1],
                       modules_[num_word_modules_ + index + 1]);
    modules_[num_word_modules_ + index + 1]->AdditivePartialConfabulation(1);

    return result;
}

std::vector<std::string> TwoLevelMultiConfabulation::BasicTransitionOverMultipleIndices(int index, int span)
{
    std::vector<std::string> result;

    for (int cursor = 0; cursor < span; ++cursor) {
        if (cursor == 0) {
            result = BasicTransitionAtIndex(index + cursor);
        } else if (cursor == span - 1) {
            BasicSwirlAtIndex(index + cursor);
        } else {
            BasicTransitionAtIndex(index + cursor);
        }
    }

    return result;
}

std::vector<std::string> TwoLevelMultiConfabulation::FullTransitionAtIndex(int index)
{
    const std::vector<std::string>& result = FullSwirlAtIndex(index);

    TransferExcitation(modules_[index],
                       knowledge_bases_[index][num_word_modules_ + index + 1],
                       modules_[num_word_modules_ + index + 1]);
    modules_[num_word_modules_ + index + 1]->AdditivePartialConfabulation(1);

    return result;
}
