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

#include "TwoLevelMultiConfabulation.h"
#include "Globals.h"
#include "Dbg.h"
#include <chrono>

TwoLevelMultiConfabulation::TwoLevelMultiConfabulation(size_t num_word_modules,
													   const std::string &symbol_file,
													   const std::string &master_file,
													   uint8_t min_single_occurrences,
													   uint8_t min_multi_occurrences)
{
	num_word_modules_ = num_word_modules;
	num_phrase_modules_ = num_word_modules_;
	num_modules_ = num_word_modules_ + num_phrase_modules_;

	// knowledge base specification
	std::vector<std::vector<bool>> kb_specs;
	kb_specs.resize(num_modules_);
	for (size_t i = 0; i < num_modules_; ++i) {
		kb_specs[i].resize(num_modules_);
	}

	// word-to-future-word knowledge bases (affects initialization)
	for (size_t i = 0; i < num_word_modules; ++i) {
		for (size_t j = i + 1; j < num_word_modules && j < i + 1 + Globals::kReferenceFrameSize; ++j) {
			kb_specs[i][j] = true;
		}
	}

	// word-to-past-word knowledge bases (affects swirl)
	for (size_t i = 1; i < num_word_modules; ++i) {
		for (int j = i - 1; j >= 0 && j >= ConvertToSigned(i) - ConvertToSigned(Globals::kMaxMultiWordSize); --j) {
			kb_specs[i][j] = true;
		}
	}

	// word-to-future-phrase knowledge bases (affects initialization)
	for (size_t i = 0; i < num_word_modules; ++i) {
		for (size_t j = num_word_modules + i + 1; j < 2 * num_word_modules && j < num_word_modules + i + 1 + Globals::kReferenceFrameSize; ++j) {
			kb_specs[i][j] = true;
		}
	}

	// word-to-past-phrase knowledge bases (affects swirl)
	for (size_t i = 0; i < num_word_modules; ++i) {
		for (size_t j = num_word_modules + i; j >= num_word_modules && j >= num_word_modules + i + 1 - Globals::kMaxMultiWordSize; --j) {
			kb_specs[i][j] = true;
		}
	}

	// phrase-to-phrase knowledge bases (affects initialization)
	for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i) {
		for (size_t j = i + 2; j < 2 * num_word_modules && j < i + 1 + Globals::kReferenceFrameSize; ++j) {
			kb_specs[i][j] = true;
		}
	}

	// phrase-to-word knowledge bases (affects swirl)
	for (size_t i = num_word_modules; i < 2 * num_word_modules; ++i) {
		kb_specs[i][i - num_word_modules] = true;
	}

	std::vector<uint8_t> level_sizes;
	level_sizes.push_back(num_word_modules);
	level_sizes.push_back(num_word_modules);

	Initialize(kb_specs, level_sizes, symbol_file, master_file, min_single_occurrences, min_multi_occurrences);
}

int8_t TwoLevelMultiConfabulation::AutoIndexToComplete(const std::vector<std::string> &symbols)
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

bool TwoLevelMultiConfabulation::CheckIndex(const std::vector<std::string> &symbols, int8_t index_to_complete)
{
	if (index_to_complete < 0) {
		std::cout << "Index to complete is negative" << "\n" << std::flush;
		return false;
	}

	if (index_to_complete >= num_modules_) {
		std::cout << "Support for completion only for indices in [0, " << (num_modules_ - 1) << "\n" << std::flush;
		return false;
	}

	if (((size_t)index_to_complete < symbols.size()) && (!symbols[index_to_complete].empty())) {
		std::cout << "There is already symbol \"" << symbols[index_to_complete] << "\") at index " << index_to_complete << "\n" << std::flush;
		return false;
	}

	return true;
}

bool TwoLevelMultiConfabulation::CheckArguments(const std::vector<std::string> &symbols, int8_t index_to_complete)
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

		if (CheckIndex(symbols, index_to_complete)) {
			return CheckVocabulary(symbols);
		} else {
			return false;
		}
	}

	// autodetect mode
	int8_t index = AutoIndexToComplete(symbols);

	if (CheckIndex(symbols, index)) {
		return CheckVocabulary(symbols);
	} else {
		return false;
	}
}

int8_t TwoLevelMultiConfabulation::GetStartPosition(const std::vector<std::string> &symbols, int8_t index_to_complete)
{
	int8_t index = std::min((int8_t)ConvertToSigned(symbols.size()), (int8_t)index_to_complete);
	int8_t max_nonempty_pos = index - FindNumberOfEmptyStringsBeforeIndex(symbols, index);
	if (start_position_ >= 0) {
	   return std::min(start_position_, max_nonempty_pos);
	} else {
	   return max_nonempty_pos;
	}
}

void TwoLevelMultiConfabulation::Activate(const std::vector<std::string> &symbols)
{
	const std::vector<std::vector<std::string>>& primary_layout = organizer_->Organize(symbols);
	const std::vector<std::string>& activated_words = primary_layout[0];
	const std::vector<std::string>& activated_multiwords = primary_layout[1];

	if (activated_words.size() != activated_multiwords.size()) {
		std::cout << "Organizer did not produce layouts with correct sizes for activation, aborting" << "\n" << std::flush;
		return;
	}

	/*
	std::cout << "Finding phrase activations for: " << VectorSymbolToSymbol(activated_multiwords, '#') << "\n" << std::flush;
	std::cout << "Finding word activations for: " << VectorSymbolToSymbol(activated_words, '#') << "\n" << std::flush;
	*/

	// activate phrases
	for (size_t i = 0; i < activated_multiwords.size(); ++i) {
		if (!activated_multiwords[i].empty()) {
			phrase_modules_[i]->ActivateSymbol(activated_multiwords[i], 1);
		}
	}

	// activate words
	for (size_t i = 0; i < std::min(symbols.size(), word_modules_.size()); ++i) {
		if ((!symbols[i].empty()) && (word_modules_[i] != nullptr)) {
			word_modules_[i]->ActivateSymbol(symbols[i], 1);
		}
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
		const std::vector<std::string>& initial_result = InitializationAtIndex(index);

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
			std::string next_word = word_modules_[index]->ElementaryConfabulation(start_pos, &word_excitation);
			std::string next_phrase =  phrase_modules_[index]->ElementaryConfabulation(start_pos, &phrase_excitation);

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
std::vector<std::string> TwoLevelMultiConfabulation::InitializationAtIndex(int index)
{
	TransferAllExcitations(index, word_modules_[index].get());
	TransferAllExcitations(index, phrase_modules_[index].get());
	return ExcitedSymbolsAtIndex(index);
}

// get excited symbols on target (phrase and) word modules
std::vector<std::string> TwoLevelMultiConfabulation::ExcitedSymbolsAtIndex(int index)
{
	std::vector<std::string> result;
	const std::vector<std::string>& result_word = word_modules_[index]->TighteningPartialConfabulation(0);
	const std::vector<std::string>& result_phrase = phrase_modules_[index]->TighteningPartialConfabulation(0);
	result.insert(result.end(), result_word.begin(), result_word.end());
	result.insert(result.end(), result_phrase.begin(), result_phrase.end());
	return result;
}

// tighten expectation on target (phrase and) word modules only if source module actually has excited symbols
std::vector<std::string> TwoLevelMultiConfabulation::TransferAndTightenAtIndex(int source_index,
																			   int target_index)
{
	std::vector<std::string> result_source;

	switch(GetModuleType(source_index)) {
	case ModuleType::word_t:
		result_source = word_modules_[source_index]->TighteningPartialConfabulation(0);
	case ModuleType::phrase_t:
		result_source = phrase_modules_[source_index]->TighteningPartialConfabulation(0);
	}

	int8_t tighten = result_source.size() > 0 ? 1 : 0;
	if (tighten) {
		switch(GetKnowledgeBaseType(source_index, target_index)) {
		case KnowledgeBaseType::word_to_word_t:
			TransferExcitation(word_modules_[source_index].get(),
							   word_to_word_knowledge_bases_[source_index][target_index].get(),
							   word_modules_[target_index].get());
		case KnowledgeBaseType::phrase_to_phrase_t:
			TransferExcitation(phrase_modules_[source_index].get(),
							   phrase_to_phrase_knowledge_bases_[source_index][target_index].get(),
							   phrase_modules_[target_index].get());
		case KnowledgeBaseType::word_to_phrase_t:
			TransferExcitation(word_modules_[source_index].get(),
						   word_to_phrase_knowledge_bases_[source_index][target_index].get(),
						   phrase_modules_[target_index].get());
		case KnowledgeBaseType::phrase_to_word_t:
			TransferExcitation(phrase_modules_[source_index].get(),
						   phrase_to_word_knowledge_bases_[source_index][target_index].get(),
						   word_modules_[target_index].get());
		}
	}

	switch(GetModuleType(target_index)) {
	case ModuleType::word_t:
		return word_modules_[target_index]->TighteningPartialConfabulation(tighten);
	case ModuleType::phrase_t:
		return phrase_modules_[target_index]->TighteningPartialConfabulation(tighten);
	}
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
