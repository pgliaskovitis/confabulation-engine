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

	// phrase-to-word knowledge bases (only directly below)
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
	int8_t end_completion = index + Globals::kMaxCompletionSize;

	std::vector<std::string> temp_input(symbols.begin(), symbols.end());

	for (; index < end_completion && (uint8_t)index < num_word_modules_;) {
		int8_t start_pos = GetStartPosition(temp_input, index);
		// int8_t initial_excitation_level = std::min<int8_t>(Globals::kMaxMultiWordSize, start_pos);
		int8_t initial_excitation_level = start_pos;
		// int8_t initial_excitation_level = 1;
		std::vector<std::string> initial_result;

		do {
			Clean();

			// activate known symbols from input
			Activate(temp_input);

			initial_result = InitializationAtIndex(index, 1, 1);
			initial_excitation_level--;
		} while (initial_result.size() == 0 && initial_excitation_level > 0);

		if (initial_result.size() == 0) {
			std::cout << "Failed to initialize at initial excitation level: " << (int)initial_excitation_level + 1 << std::endl;
			return result;
		}

		if(Globals::kUseMultiThreading) {
			int max_span = std::min((int)Globals::kMaxMultiWordSize, (int)(num_word_modules_ - index));
			threads_.clear();
			for (int8_t context_span = 0; context_span < max_span; ++context_span) {
				threads_.push_back(std::thread(&TwoLevelMultiConfabulation::FullSwirlOverMultipleIndices, this, index, max_span));
			}
			for (std::thread& th: threads_) {
				th.join();
			}
		} else {
			if (Globals::kSingleIndexFullSwirl) {
				for (int8_t context_span = 1; context_span < Globals::kMaxMultiWordSize; ++context_span) {
					if (context_span == 1) {
						if ((uint8_t)(index + context_span) < num_word_modules_) {
							FullTransitionAtIndex(index);
						}
					} else {
						if ((uint8_t)(index + context_span) < num_word_modules_) {
							FullRetroTransitionAtIndex(index, context_span);
						}
					}
				}
			} else {
				int max_span = std::min((int)Globals::kMaxMultiWordSize, (int)(num_word_modules_ - index));
				FullSwirlOverMultipleIndices(index, max_span);
			}
		}

		// one final excitation boost
		for (int8_t context_span = 1; context_span < Globals::kMaxMultiWordSize; ++context_span) {
			if ((uint8_t)(index + context_span) < num_word_modules_) {
				TransferExcitation(modules_[index + context_span],
								   knowledge_bases_[index + context_span][num_word_modules_ + index],
								   modules_[num_word_modules_ + index]);
				TransferExcitation(modules_[index + context_span],
								   knowledge_bases_[index + context_span][index],
								   modules_[index]);
			}
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

// Initialize expectation on target (phrase and) word modules once
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

// tighten expectation on target (phrase and) word modules once
std::vector<std::string> TwoLevelMultiConfabulation::BasicSwirlAtIndex(int index)
{
	std::vector<std::string> result_word;
	std::vector<std::string> result_phrase;

	const std::vector<std::string>& initial_result_phrase = modules_[num_word_modules_ + index]->AdditivePartialConfabulation(0);
	int8_t phrase_word_transition = initial_result_phrase.size() ? 1 : 0;
	TransferExcitation(modules_[num_word_modules_ + index],
					   knowledge_bases_[num_word_modules_ + index][index],
					   modules_[index]);
	modules_[index]->AdditivePartialConfabulation(phrase_word_transition);

	TransferExcitation(modules_[index],
					   knowledge_bases_[index][index + 1],
					   modules_[index + 1]);
	modules_[index + 1]->AdditivePartialConfabulation(1);

	TransferExcitation(modules_[index + 1],
					   knowledge_bases_[index + 1][num_word_modules_ + index],
					   modules_[num_word_modules_ + index]);
	result_phrase = modules_[num_word_modules_ + index]->AdditivePartialConfabulation(1);

	TransferExcitation(modules_[index + 1],
					   knowledge_bases_[index + 1][index],
					   modules_[index]);
	result_word = modules_[index]->AdditivePartialConfabulation(1);

	result_word.insert(result_word.end(), result_phrase.begin(), result_phrase.end());

	return result_word;
}

// tighten expectation on target (phrase and) word modules once and move on to the next index phrase
std::vector<std::string> TwoLevelMultiConfabulation::BasicTransitionAtIndex(int index)
{
	const std::vector<std::string>& result = BasicSwirlAtIndex(index);

	TransferExcitation(modules_[index],
					   knowledge_bases_[index][num_word_modules_ + index + 1],
					   modules_[num_word_modules_ + index + 1]);
	modules_[num_word_modules_ + index + 1]->AdditivePartialConfabulation(1);

	return result;
}

// tighten expectation on target (phrase and) word modules once and all modules up to index + span
std::vector<std::string> TwoLevelMultiConfabulation::BasicTransitionOverMultipleIndices(int index, int span)
{
	std::vector<std::string> result;

	for (int cursor = 0; cursor < span; ++cursor) {
		if (cursor == span - 1) {
			result = BasicSwirlAtIndex(index + cursor);
		} else {
			result = BasicTransitionAtIndex(index + cursor);
		}
	}

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

// tighten expectation on target (phrase and) word modules and all modules up to index + span continuously
std::vector<std::string> TwoLevelMultiConfabulation::FullSwirlOverMultipleIndices(int index, int span)
{
	std::vector<std::string> result;
	const std::vector<std::string>& result_word = modules_[index]->AdditivePartialConfabulation(0);
	const std::vector<std::string>& result_phrase = modules_[num_word_modules_ + index]->AdditivePartialConfabulation(0);
	size_t current_result_size = result_word.size() + result_phrase.size();
	size_t previous_result_size = 0;

	do {
		previous_result_size = current_result_size;
		result = BasicTransitionOverMultipleIndices(index, span - 1);
		for (size_t cursor = 2; cursor < (size_t)span; cursor++) {
			if (index + cursor < num_word_modules_) {
				TransferExcitation(modules_[index + cursor],
								   knowledge_bases_[index + cursor][num_word_modules_ + index],
								   modules_[num_word_modules_ + index]);
				modules_[num_word_modules_ + index]->AdditivePartialConfabulation(1);
				TransferExcitation(modules_[index + cursor],
								   knowledge_bases_[index + cursor][index],
								   modules_[index]);
				modules_[index]->AdditivePartialConfabulation(1);
			}
		}
		current_result_size = result.size();
	} while (current_result_size < previous_result_size);

	return result;
}

// tighten expectation from index + span towards index
std::vector<std::string> TwoLevelMultiConfabulation::RetroSwirlOverMultipleIndices(int index, int span)
{
	std::vector<std::string> result;
	const std::vector<std::string>& result_word = modules_[index]->AdditivePartialConfabulation(0);
	const std::vector<std::string>& result_phrase = modules_[num_word_modules_ + index]->AdditivePartialConfabulation(0);
	size_t current_result_size = result_word.size() + result_phrase.size();
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
		modules_[index]->AdditivePartialConfabulation(1);
		result = BasicTransitionOverMultipleIndices(index, span);
		current_result_size = result.size();
	} while (current_result_size < previous_result_size);

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

std::vector<std::string> TwoLevelMultiConfabulation::FullRetroTransitionAtIndex(int index, int span)
{
	FullTransitionAtIndex(index + span - 1);
	return RetroSwirlOverMultipleIndices(index, span);
}
