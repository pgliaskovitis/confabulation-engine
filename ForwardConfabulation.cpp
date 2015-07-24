#include "ForwardConfabulation.h"

ForwardConfabulation::ForwardConfabulation(size_t num_modules,
                                           const std::string &symbol_file,
                                           const std::string &master_file,
                                           uint8_t min_single_occurrences,
                                           uint8_t min_multi_occurrences)
{
    num_modules_ = num_modules;

    // knowledge base specification
    std::vector<std::vector<bool>> kb_specs;

    // triangle matrix, so that any module before is linked to any module after
    kb_specs.resize(num_modules_);
    for (size_t i = 0; i < num_modules_; ++i) {
        kb_specs[i].resize(num_modules_);
        for (size_t j = i + 1; j < num_modules_; ++j) {
            kb_specs[i][j] = true;
        }
    }

    std::vector<uint8_t> level_sizes;
    level_sizes.push_back(num_modules_);

    Initialize(kb_specs, level_sizes, symbol_file, master_file, min_single_occurrences, min_multi_occurrences);
}

std::vector<std::string> ForwardConfabulation::Confabulation(const std::vector<std::string> &symbols, int8_t index_to_complete, bool expectation)
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

    int32_t actual_K = ActualK(symbols, index);
    const std::unique_ptr<Module>& target_module = modules_[index];
    target_module->ExcitationsToZero();

    // core algorithm
    Activate(symbols);
    TransferAllExcitations(index, target_module);

    if (expectation) {
        result = target_module->PartialConfabulation(actual_K, false);
    } else {
        float dummy;
        result.push_back(target_module->ElementaryConfabulation(actual_K, &dummy));
    }

    Clean();

    return result;
}

int8_t ForwardConfabulation::AutoIndexToComplete(const std::vector<std::string> &symbols)
{
    if (symbols.empty() || symbols[0].empty()) {
        return -1;
    }

    int8_t index = FindFirstIndexOfSymbol(symbols, "");

    if (index >= 0) {
        return index;
    } else {
        // array full of symbols
        return ConvertToSigned(symbols.size());
    }
}

bool ForwardConfabulation::CheckIndex(const std::vector<std::string> &symbols, int8_t index_to_complete)
{
    if (index_to_complete < 0) {
        std::cout << "Index to complete is negative" << "\n" << std::flush;
        return false;
    }

    if (index_to_complete >= num_modules_) {
        std::cout << "Support for completion only for indices in [0, " << (num_modules_ - 1) << "\n" << std::flush;
        return false;
    }

    if ((index_to_complete < symbols.size()) && (!symbols[index_to_complete].empty())) {
        std::cout << "There is already symbol \"" << symbols[index_to_complete] << "\") at index " << index_to_complete << "\n" << std::flush;
        return false;
    }

    return true;
}

bool ForwardConfabulation::CheckArguments(const std::vector<std::string> &symbols, int8_t index_to_complete)
{
    if (symbols.empty()) {
        std::cout << "Symbols vector is empty" << "\n" << std::flush;
        return false;
    }

    if (index_to_complete >= 0) {
        // check there is at least one symbol before index_to_complete
        if (FindFirstIndexNotOfSymbol(symbols, "") >= index_to_complete) {
            std::cout << "Symbols vector does not contain any symbol before index_to_complete" << "\n" << std::flush;
            return false;
        }

        if (CheckIndex(symbols, index_to_complete))
            return CheckVocabulary(symbols);
        else return false;
    }

    // autodetect mode
    int8_t index = AutoIndexToComplete(symbols);

    if (CheckIndex(symbols, index))
        return CheckVocabulary(symbols);
    else return false;
}
