#pragma once

#include <thread>
#include "ForwardConfabulation.h"

class TwoLevelMultiConfabulation : public ForwardConfabulation
{
public:
	TwoLevelMultiConfabulation(size_t num_word_modules,
								const std::string &symbol_file,
								const std::string &master_file,
								uint8_t min_single_occurrences,
								uint8_t min_multi_occurrences);
	TwoLevelMultiConfabulation(const TwoLevelMultiConfabulation& rhs) = delete;
	TwoLevelMultiConfabulation& operator=(const TwoLevelMultiConfabulation& rhs) = delete;
	TwoLevelMultiConfabulation(TwoLevelMultiConfabulation&& rhs) = delete;
	TwoLevelMultiConfabulation&& operator=(TwoLevelMultiConfabulation&& rhs) = delete;

	virtual std::vector<std::string> Confabulation(const std::vector<std::string>& symbols, int8_t index_to_complete, bool expectation);

private:
	size_t num_word_modules_;
	std::vector<std::thread> threads_;

	std::vector<std::string> InitializationAtIndex(int index, int phrase_excit_level, int word_excit_level);

	std::vector<std::string> BasicSwirlAtIndex(int index);
	std::vector<std::string> BasicTransitionAtIndex(int index);
	std::vector<std::string> BasicTransitionOverMultipleIndices(int index, int span);

	std::vector<std::string> FullSwirlAtIndex(int index);
	std::vector<std::string> FullSwirlOverMultipleIndices(int index, int span);
	std::vector<std::string> RetroSwirlOverMultipleIndices(int index, int span);

	std::vector<std::string> FullTransitionAtIndex(int index);
	std::vector<std::string> FullRetroTransitionAtIndex(int index, int span);
};
