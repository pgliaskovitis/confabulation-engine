/*
 * Copyright 2014 Periklis G. Liaskovitis
 *
 * This file is part of confab-engine.
 *
 * confab-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * confab-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with confab-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

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

	virtual void Activate(const std::vector<std::string>& symbols);
	virtual std::vector<std::string> Confabulation(const std::vector<std::string>& symbols, int8_t index_to_complete, bool expectation);

private:
	uint8_t num_word_modules_;
	std::vector<std::thread> threads_;

	std::vector<std::string> InitializationAtIndex(int index);
	std::vector<std::string> ExcitedSymbolsAtIndex(int index);
	std::vector<std::string> TransferAndTightenAtIndex(int source_index, int target_index);

	std::vector<std::string> BasicSwirlAtIndex(int index);
	std::vector<std::string> BasicTransitionAtIndex(int index);
	std::vector<std::string> BasicTransitionOverMultipleIndices(int index, int span);

	std::vector<std::string> FullSwirlAtIndex(int index);
	std::vector<std::string> FullSwirlOverMultipleIndices(int index, int span);
};
