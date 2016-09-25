#include "TwoLevelMultiConfabulation.h"
#include "Globals.h"
#include "Dbg.h"

const size_t TwoLevelMultiConfabulation::reference_frame_length = 5;

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
            kb_specs[i][i - num_word_modules + 1] = true;
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
    size_t swirl_progression_0 = 0;
    size_t swirl_progression_1 = 0;
    size_t swirl_progression_2 = 0;

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
        int8_t actual_K = std::min<int8_t>(ActualK(temp_input, index), 4);
        modules_[index]->ExcitationsToZero();

        // activate known symbols from input
        Activate(temp_input);

        // find initial expectation on phrase module above word module at index
        TransferAllExcitations(num_word_modules_ + index, modules_[num_word_modules_ + index]);
        modules_[num_word_modules_ + index]->PartialConfabulation(1);

        // find initial expectation on word module at index
        TransferAllExcitations(index, modules_[index]);
        modules_[index]->PartialConfabulation(1);

        if (index + 1 < num_word_modules_) {
            // find expectation on word module at index + 1
            TransferExcitation(modules_[index],
                               knowledge_bases_[index][index + 1],
                               modules_[index + 1]);
            modules_[index + 1]->PartialConfabulation(1);

            swirl_progression_0 = BasicSwirlAtIndex(index);

            // find expectation on phrase module above word module at index + 1
            TransferExcitation(modules_[index],
                               knowledge_bases_[index][num_word_modules_ + index + 1],
                               modules_[num_word_modules_ + index + 1]);
            modules_[num_word_modules_ + index + 1]->PartialConfabulation(1);

            // find additional expectation on word module at index + 1
            TransferExcitation(modules_[num_word_modules_ + index + 1],
                               knowledge_bases_[num_word_modules_ + index + 1][index + 1],
                               modules_[index + 1]);
            modules_[index + 1]->PartialConfabulation(1);

            if (index + 2 < num_word_modules_) {
                // find expectation on word module at index + 2
                TransferExcitation(modules_[index + 1],
                                   knowledge_bases_[index + 1][index + 2],
                                   modules_[index + 2]);
                modules_[index + 2]->PartialConfabulation(1);

                swirl_progression_1 = BasicSwirlAtIndex(index + 1);

                // constraint satisfaction from index + 2 towards index
                TransferExcitation(modules_[index + 2],
                                   knowledge_bases_[index + 2][num_word_modules_ + index],
                                   modules_[num_word_modules_ + index]);
                modules_[num_word_modules_ + index]->PartialConfabulation(swirl_progression_0);
                TransferExcitation(modules_[index + 2],
                                   knowledge_bases_[index + 2][index],
                                   modules_[index]);
                modules_[index]->PartialConfabulation(swirl_progression_0);
                ++swirl_progression_0;

                // find expectation on phrase module above word module at index + 2
                TransferExcitation(modules_[index],
                                   knowledge_bases_[index][num_word_modules_ + index + 2],
                                   modules_[num_word_modules_ + index + 2]);
                TransferExcitation(modules_[index + 1],
                                   knowledge_bases_[index + 1][num_word_modules_ + index + 2],
                                   modules_[num_word_modules_ + index + 2]);
                modules_[num_word_modules_ + index + 2]->PartialConfabulation(1);

                // find additional expectation on word module at index + 2
                TransferExcitation(modules_[num_word_modules_ + index + 2],
                                   knowledge_bases_[num_word_modules_ + index + 2][index + 2],
                                   modules_[index + 2]);
                modules_[index + 2]->PartialConfabulation(1);

                if (index + 3 < num_word_modules_) {
                    // find expectation on word module at index + 3
                    TransferExcitation(modules_[index + 2],
                                       knowledge_bases_[index + 2][index + 3],
                                       modules_[index + 3]);
                    modules_[index + 3]->PartialConfabulation(1);

                    swirl_progression_2 = BasicSwirlAtIndex(index + 2);

                    // constraint satisfaction from index + 3 towards index
                    TransferExcitation(modules_[index + 3],
                                       knowledge_bases_[index + 3][num_word_modules_ + index],
                                       modules_[num_word_modules_ + index]);
                    modules_[num_word_modules_ + index]->PartialConfabulation(swirl_progression_0);
                    TransferExcitation(modules_[index + 3],
                                       knowledge_bases_[index + 3][index],
                                       modules_[index]);
                    modules_[index]->PartialConfabulation(swirl_progression_0);
                    ++swirl_progression_0;

                    // constraint satisfaction from index + 3 towards index + 1
                    TransferExcitation(modules_[index + 3],
                                       knowledge_bases_[index + 3][num_word_modules_ + index + 1],
                                       modules_[num_word_modules_ + index + 1]);
                    modules_[num_word_modules_ + index]->PartialConfabulation(swirl_progression_1);
                    TransferExcitation(modules_[index + 3],
                                       knowledge_bases_[index + 3][index + 1],
                                       modules_[index + 1]);
                    modules_[index]->PartialConfabulation(swirl_progression_1);
                    ++swirl_progression_1;
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

size_t TwoLevelMultiConfabulation::BasicSwirlAtIndex(int index)
{
    std::vector<std::string> result_backward_word;
    std::vector<std::string> result_backward_phrase;
    size_t current_result_size = std::numeric_limits<size_t>::max();
    size_t previous_result_size = 0;

    // tighten expectation on target phrase and word modules
    size_t swirl_progression = 1;

    do {
        previous_result_size = current_result_size;

        TransferExcitation(modules_[index + 1], knowledge_bases_[index + 1][index], modules_[index]);
        result_backward_word = modules_[index]->PartialConfabulation(swirl_progression + 1);

        TransferExcitation(modules_[index + 1], knowledge_bases_[index + 1][num_word_modules_ + index], modules_[num_word_modules_ + index]);
        result_backward_phrase = modules_[num_word_modules_ + index]->PartialConfabulation(swirl_progression + 1);

        current_result_size = result_backward_word.size() + result_backward_phrase.size();

        TransferExcitation(modules_[num_word_modules_ + index], knowledge_bases_[num_word_modules_ + index][index], modules_[index]);
        modules_[index]->PartialConfabulation(swirl_progression + 2);

        TransferExcitation(modules_[index],  knowledge_bases_[index][index + 1], modules_[index + 1]);
        modules_[index + 1]->PartialConfabulation(swirl_progression + 1);

        ++swirl_progression;
    } while (current_result_size < previous_result_size);

    return swirl_progression;
}

