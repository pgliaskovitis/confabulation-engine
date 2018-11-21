/*
 * Copyright 2018 Periklis G. Liaskovitis
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

#include "TwoLevelMultiConfabulation.h"
#include "Globals.h"
#include "Dbg.h"
#include <chrono>

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

	// word-to-future-word knowledge bases (reference frame length ahead -- affects initialization)
	for (size_t i = 0; i < num_word_modules; ++i) {
		for (size_t j = i + 1; j < num_word_modules && j < i + 1 + Globals::kMaxMultiWordSize; ++j) {
			kb_specs[i][j] = true;
		}
	}

	// word-to-past-word knowledge bases (max phrase length ago -- affects swirl)
	for (size_t i = 1; i < num_word_modules; ++i) {
		for (int j = i - 1; j >= 0 && j >= ConvertToSigned(i) - ConvertToSigned(Globals::kMaxMultiWordSize); --j) {
			kb_specs[i][j] = true;
		}
	}

	// word-to-future-phrase knowledge bases (max phrase length ahead -- affects initialization)
	for (size_t i = 0; i < num_word_modules; ++i) {
		for (size_t j = num_word_modules + i + 1; j < 2 * num_word_modules && j < num_word_modules + i + 1 + Globals::kMaxMultiWordSize; ++j) {
			kb_specs[i][j] = true;
		}
	}

	// word-to-past-phrase knowledge bases (max phrase length ago -- affects swirl)
	for (size_t i = 0; i < num_word_modules; ++i) {
		for (size_t j = num_word_modules + i; j >= num_word_modules && j >= num_word_modules + i + 1 - Globals::kMaxMultiWordSize; --j) {
			kb_specs[i][j] = true;
		}
	}

	// phrase-to-phrase knowledge bases (max phrase length ahead -- no single word phrases here -- affects initialization)
	for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i) {
		for (size_t j = i + 2; j < 2 * num_word_modules && j < i + 1 + Globals::kMaxMultiWordSize; ++j) {
			kb_specs[i][j] = true;
		}
	}

	// phrase-to-word knowledge bases (only directly below -- affects swirl)
	for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i) {
		kb_specs[i][i - num_word_modules] = true;
	}

	std::vector<uint8_t> level_sizes;
	level_sizes.push_back(num_word_modules);
	level_sizes.push_back(num_word_modules);

	Initialize(kb_specs, level_sizes, symbol_file, master_file, min_single_occurrences, min_multi_occurrences);
}

void TwoLevelMultiConfabulation::Activate(const std::vector<std::string> &symbols)
{
	const std::vector<std::vector<std::vector<std::string>>>& activated_module_layouts = organizer_->Organize(symbols);

	const std::vector<std::vector<std::string>>& activated_multiwords = activated_module_layouts[0];

	for (size_t i = 0; i < activated_multiwords.size(); ++i) {
		std::cout << "Examining multiword symbol: " << VectorSymbolToSymbol(activated_multiwords[i], '#') << "\n" << std::flush;
		// if (!activated_multiwords[i].empty()) {
		// 	const std::string& multiword = VectorSymbolToSymbol(activated_multiwords[i], ' ');
		//	modules_[num_word_modules_ + i]->ActivateSymbol(multiword, 1);
		//	std::cout << "Activated multiword symbol: " << multiword << "\n" << std::flush;
		// }
	}

	for (size_t i = 0, j = 0; i < activated_multiwords.size(); ++i) {
		if (!activated_multiwords[i].empty()) {
			j += activated_multiwords[i].size();
			continue;
		}
		/*
		if (i == j) {
			const std::string& word = VectorSymbolToSymbol(activated_words[i], ' ');
			modules_[j]->ActivateSymbol(word, 1);
			std::cout << "Activated word symbol: " << word << "\n" << std::flush;
			j++;
		}
		*/
	}
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
	int8_t end_completion = index + Globals::kMaxCompletionSize;

	std::vector<std::string> temp_input(symbols.begin(), symbols.end());

	for (; index < end_completion && (uint8_t)index < num_word_modules_;) {
		int8_t start_pos = GetStartPosition(temp_input, index);

		Clean();
		Activate(temp_input);
		const std::vector<std::string>& initial_result = InitializationAtIndex(index, 0, 0);

		if (initial_result.size() == 0) {
			std::cout << "Failed to initialize symbols" << std::endl;
			return result;
		}

		int8_t max_span = std::min((int)Globals::kMaxMultiWordSize, (int)(num_word_modules_ - index));

		if (Globals::kUseMultiThreading) {
			threads_.clear();
			for (int8_t context_span = 0; context_span < max_span; ++context_span) {
				threads_.push_back(std::thread(&TwoLevelMultiConfabulation::FullSwirlOverMultipleIndices, this, index, max_span));
			}
			for (std::thread& th: threads_) {
				th.join();
			}
		} else {
			FullSwirlOverMultipleIndices(index, max_span);
		}

		if (!expectation) {
			float word_excitation;
			float phrase_excitation;
			std::string next_word = modules_[index]->ElementaryConfabulation(start_pos, &word_excitation);
			std::string next_phrase =  modules_[num_word_modules_ + index]->ElementaryConfabulation(start_pos, &phrase_excitation);

			result.push_back("{");
			if (word_excitation > phrase_excitation) {
				result.push_back(next_word);
				temp_input.push_back(next_word);
				++index;
			} else {
				result.push_back(next_phrase);
				const std::vector<std::string>& result_tokens = SymbolToVectorSymbol(next_phrase, ' ');
				for (size_t m = 0; m < result_tokens.size(); ++m) {
					temp_input.push_back(result_tokens.at(m));
				}
				index += result_tokens.size();
			}
			result.push_back("}");
		}
	}

	return result;
}

// initialize expectation on target (phrase and) word modules once
std::vector<std::string> TwoLevelMultiConfabulation::InitializationAtIndex(int index,
																		   int phrase_excit_level,
																		   int word_excit_level)
{
	// find initial expectation on phrase module above word module at index
	TransferAllExcitations(num_word_modules_ + index, modules_[num_word_modules_ + index]);
	modules_[num_word_modules_ + index]->AdditivePartialConfabulation(phrase_excit_level);

	// find initial expectation on word module at index (including phrase module above)
	TransferAllExcitations(index, modules_[index]);
	return modules_[index]->AdditivePartialConfabulation(word_excit_level);
}

// get excited symbols on target (phrase and) word modules
std::vector<std::string> TwoLevelMultiConfabulation::ExcitedSymbolsAtIndex(int index)
{
	std::vector<std::string> result;
	const std::vector<std::string>& result_word = modules_[index]->AdditivePartialConfabulation(0);
	const std::vector<std::string>& result_phrase = modules_[num_word_modules_ + index]->AdditivePartialConfabulation(0);
	result.insert(result.end(), result_word.begin(), result_word.end());
	result.insert(result.end(), result_phrase.begin(), result_phrase.end());
	return result;
}

// tighten expectation on target (phrase and) word modules only if source module actually has excited symbols
std::vector<std::string> TwoLevelMultiConfabulation::TransferAndTightenAtIndex(int source_index,
																			   int target_index)
{
	const std::vector<std::string>& result_source = modules_[source_index]->AdditivePartialConfabulation(0);

	int8_t tighten = result_source.size() > 0 ? 1 : 0;
	if (tighten) {
		TransferExcitation(modules_[source_index],
						   knowledge_bases_[source_index][target_index],
						   modules_[target_index]);
	}

	return modules_[target_index]->AdditivePartialConfabulation(tighten);
}

// tighten expectation on target (phrase and) word modules once
std::vector<std::string> TwoLevelMultiConfabulation::BasicSwirlAtIndex(int index)
{
	TransferAndTightenAtIndex(num_word_modules_ + index, index);

	if (index + 1 < num_word_modules_) {
		TransferAndTightenAtIndex(index, index + 1);
		TransferAndTightenAtIndex(index + 1, num_word_modules_ + index);
		TransferAndTightenAtIndex(index + 1, index);
	}

	TransferAndTightenAtIndex(index, num_word_modules_ + index);

	return ExcitedSymbolsAtIndex(index);
}

// tighten expectation on target (phrase and) word modules once and move on to the next index phrase
std::vector<std::string> TwoLevelMultiConfabulation::BasicTransitionAtIndex(int index)
{
	const std::vector<std::string>& result = BasicSwirlAtIndex(index);
	TransferAndTightenAtIndex(index, num_word_modules_ + index + 1);
	return result;
}

// tighten expectation on target (phrase and) word modules once and all modules up to index + span
std::vector<std::string> TwoLevelMultiConfabulation::BasicTransitionOverMultipleIndices(int index, int span)
{
	std::vector<std::string> result;

	for (int cursor = 0; cursor < span; ++cursor) {
		if (cursor == span - 1) {
			BasicSwirlAtIndex(index + cursor);
		} else {
			BasicTransitionAtIndex(index + cursor);
		}
	}

	return ExcitedSymbolsAtIndex(index);
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

// tighten expectation on target (phrase and) word modules and all modules up to index + span continuously
std::vector<std::string> TwoLevelMultiConfabulation::FullSwirlOverMultipleIndices(int index, int span)
{
	std::vector<std::string> result = ExcitedSymbolsAtIndex(index);
	size_t current_result_size = result.size();
	size_t previous_result_size = 0;

	do {
		previous_result_size = current_result_size;
		BasicTransitionOverMultipleIndices(index, span);
		for (size_t cursor = 2; cursor < (size_t)span; cursor++) {
			TransferAndTightenAtIndex(index + cursor, num_word_modules_ + index);
			TransferAndTightenAtIndex(index + cursor, index);
		}
		result = ExcitedSymbolsAtIndex(index);
		current_result_size = result.size();
	} while (current_result_size < previous_result_size);

	return result;
}
