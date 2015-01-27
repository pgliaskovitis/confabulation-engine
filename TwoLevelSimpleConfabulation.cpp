#include "TwoLevelSimpleConfabulation.h"
#include "Globals.h"
#include "Dbg.h"

TwoLevelSimpleConfabulation::TwoLevelSimpleConfabulation(size_t num_word_modules, const std::string &symbol_file, const std::string &master_file)
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

    Initialize(kb_specs, level_sizes, symbol_file, master_file);
}

//// first attempt at two-level simple confabulation
//std::vector<std::string> TwoLevelSimpleConfabulation::Confabulation(const std::vector<std::string> &symbols, int index_to_complete, bool expectation)
//{
//    std::vector<std::string> result;
//    if (!CheckArguments(symbols, index_to_complete)) {
//        std::cout << "Input sentence does not satisfy conditions for confabulation with this architecture";
//        return result;
//    }

//    int index;
//    if (index_to_complete < 0) {
//        index = AutoIndexToComplete(symbols);
//    } else {
//        index = index_to_complete;
//    }

//    int initial_index_to_complete = index;
//    //log_info("Initial index to complete is %d ", initial_index_to_complete);
//    std::vector<std::string> temp_input(symbols.begin(), symbols.end());

//    for (size_t i = 0; i < Globals::kMaxMultiWordSize; ++i) {

//        int actual_K = ActualK(temp_input, index);
//        const std::unique_ptr<Module>& target_module = modules_[index];
//        target_module->ExcitationsToZero();

//        // activate known symbols from input
//        Activate(temp_input);

//        // find expectation on all phrase modules before word module at initial_index_to_complete
//        for (size_t m = 0; m < initial_index_to_complete; ++m)
//            for (size_t n = num_word_modules_ + m; n >= num_word_modules_; --n) {
//                //log_info("Tranferring excitation from %u to %u", m, n);
//                TransferExcitation(modules_[m],
//                                   knowledge_bases_[m][n],
//                                   modules_[n]);
//        }

//        for (size_t m = num_word_modules_; m < num_word_modules_ + initial_index_to_complete; ++m) {
//            //log_info("Partially confabulating module %u", m);
//            if (num_word_modules_ + initial_index_to_complete - m < Globals::kMaxMultiWordSize)
//                modules_[m]->PartialConfabulation(num_word_modules_ + initial_index_to_complete - m, false);
//            else
//                modules_[m]->PartialConfabulation(Globals::kMaxMultiWordSize, false);
//        }

//        // cleanup of phrase module directly above last input word,
//        // because it would be excited twice by the next operation
//        modules_[num_word_modules_ + initial_index_to_complete - 1]->Reset();

//        // find expectation on phrase modules above latest output word module and up to max-multi-word-size modules before that
//        for (size_t j = 0; j <= i; ++j)
//            for (int k = ConvertToSigned(j); k >= 0; --k) {
//                //log_info("Tranferring excitation from %u to %u", initial_index_to_complete + j - 1, num_word_modules_ + initial_index_to_complete + j - k - 1);
//                if (num_word_modules_ + initial_index_to_complete + j - 1 < num_modules_)
//                    TransferExcitation(modules_[initial_index_to_complete + j - 1],
//                                       knowledge_bases_[initial_index_to_complete + j - 1][num_word_modules_ + initial_index_to_complete + j - k - 1],
//                                       modules_[num_word_modules_ + initial_index_to_complete + j - k - 1]);
//            }

//        for (size_t j = 0; j <= i; ++j) {
//            //log_info("Partially confabulating module %u", initial_index_to_complete + j - 1);
//            if (num_word_modules_ + initial_index_to_complete + j - 1 < num_modules_)
//                modules_[num_word_modules_ + initial_index_to_complete + j - 1]->PartialConfabulation(ConvertToSigned(i - j + 1), false);
//        }

//        // find expectation on unknown word module
//        TransferAllExcitations(index, target_module);

//        if (expectation) {
//            result = target_module->PartialConfabulation(actual_K, false);
//        } else {
//            std::string next_word = target_module->ElementaryConfabulation(actual_K);
//            result.push_back(next_word);
//            temp_input.push_back(next_word);
//        }

//        ++index;

//        Clean();
//    }

//    return result;
//}

//// second attempt at two-level simple confabulation
//std::vector<std::string> TwoLevelSimpleConfabulation::Confabulation(const std::vector<std::string> &symbols, int index_to_complete, bool expectation)
//{
//    std::vector<std::string> result;
//    if (!CheckArguments(symbols, index_to_complete)) {
//        std::cout << "Input sentence does not satisfy conditions for confabulation with this architecture";
//        return result;
//    }

//    int index;
//    if (index_to_complete < 0) {
//        index = AutoIndexToComplete(symbols);
//    } else {
//        index = index_to_complete;
//    }

//    //log_info("Initial index to complete is %d ", index);

//    int actual_K = ActualK(symbols, index);
//    const std::unique_ptr<Module>& target_module = modules_[num_word_modules_ + index];
//    target_module->ExcitationsToZero();

//    // activate known symbols from input
//    Activate(symbols);

//    // find expectation on all phrase modules before word module at index
//    for (size_t m = 0; m < index; ++m)
//        for (size_t n = num_word_modules_ + m; n >= num_word_modules_; --n)
//            TransferExcitation(modules_[m],
//                               knowledge_bases_[m][n],
//                               modules_[n]);

//    for (size_t m = num_word_modules_; m < num_word_modules_ + index; ++m)
//        if (num_word_modules_ + index - m < Globals::kMaxMultiWordSize)
//            modules_[m]->PartialConfabulation(num_word_modules_ + index - m, false);
//        else
//            modules_[m]->PartialConfabulation(Globals::kMaxMultiWordSize, false);

//    // find expectation on first unfilled phrase module

//    // I) from all previous phrase modules
//    for (size_t m = num_word_modules_; m < num_word_modules_ + index; ++m)
//        TransferExcitation(modules_[m],
//                           knowledge_bases_[m][num_word_modules_ + index],
//                           modules_[num_word_modules_ + index]);

//    // II) from all previous word modules
//    for (size_t m = 0; m < index; ++m)
//        TransferExcitation(modules_[m],
//                           knowledge_bases_[m][num_word_modules_ + index],
//                           modules_[num_word_modules_ + index]);

//    if (expectation) {
//        result = target_module->PartialConfabulation(actual_K, false);
//    } else {
//        std::string next_phrase = target_module->ElementaryConfabulation(actual_K);
//        result.push_back(next_phrase);
//    }

//    ++index;

//    Clean();

//    return result;
//}

// third attempt at two-level simple confabulation
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

        // find expectation on all phrase modules before word module at index
        for (size_t m = num_word_modules_; m < num_word_modules_ + index; ++m)
            TransferAllExcitations(m, modules_[m]);

        for (size_t m = num_word_modules_; m < num_word_modules_ + index; ++m)
            modules_[m]->PartialConfabulation(num_word_modules_ + index - m, false);

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
