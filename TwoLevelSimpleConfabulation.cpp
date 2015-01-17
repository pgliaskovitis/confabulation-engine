#include "TwoLevelSimpleConfabulation.h"

TwoLevelSimpleConfabulation::TwoLevelSimpleConfabulation(size_t num_word_modules, const std::string &symbol_file, const std::string &master_file)
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

    // word-to-phrase knowledge bases
    for (size_t i = 0; i < num_word_modules; ++i)
        for (size_t j = num_word_modules + i; j >= num_word_modules; --j)
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
