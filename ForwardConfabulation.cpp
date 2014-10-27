#include "ForwardConfabulation.h"

ForwardConfabulation::ForwardConfabulation(size_t num_modules, const std::string &symbol_file, const std::string &master_file)
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

    std::vector<unsigned short> level_sizes;
    level_sizes.push_back(num_modules_);

    Initialize(kb_specs, level_sizes, symbol_file, master_file);
}

int ForwardConfabulation::AutoIndexToComplete(const std::vector<std::string> &symbols)
{
    if (symbols.empty() || symbols[0].empty()) {
        return -1;
    }

    int index = FindFirstIndexOfSymbol(symbols, "");

    if (index >= 0) {
        return index;
    } else {
        // array full of symbols
        return ConvertToSigned(symbols.size());
    }
}

bool ForwardConfabulation::CheckIndex(const std::vector<std::string> &symbols, int index_to_complete)
{
    if (index_to_complete < 0) {
        std::cout << "Index to complete is negative" << "\n" << std::flush;
        return false;
    }

    if (index_to_complete >= num_modules_) {
        std::cout << "Support for indices in [0, " << (num_modules_ - 1) << "\n" << std::flush;
        return false;
    }

    if ((index_to_complete < symbols.size()) && (!symbols[index_to_complete].empty())) {
        std::cout << "There is already symbol \"" << symbols[index_to_complete] << "\") at index " << index_to_complete << "\n" << std::flush;
        return false;
    }

    return true;
}

bool ForwardConfabulation::CheckArguments(const std::vector<std::string> &symbols, int index_to_complete)
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
    int index = AutoIndexToComplete(symbols);

    if (CheckIndex(symbols, index))
        return CheckVocabulary(symbols);
    else return false;
}
