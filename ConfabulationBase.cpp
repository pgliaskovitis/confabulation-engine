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

#include <algorithm>
#include <deque>
#include <Globals.h>
#include "ConfabulationBase.h"
#include "utils/Utils.h"
#include "Dbg.h"

ConfabulationBase::ConfabulationBase() : start_position_(-1)
{}

ConfabulationBase::~ConfabulationBase()
{
	word_modules_.clear();
	phrase_modules_.clear();
	word_to_word_knowledge_bases_.clear();
	phrase_to_phrase_knowledge_bases_.clear();
	word_to_phrase_knowledge_bases_.clear();
	phrase_to_word_knowledge_bases_.clear();
}

ModuleType ConfabulationBase::GetModuleType(size_t module_index)
{
	if (module_index < num_word_modules_) {
		return ModuleType::word_t;
	}

	if (module_index >= num_word_modules_) {
		return ModuleType::phrase_t;
	}

	throw std::logic_error("Invalid module type");
}

KnowledgeBaseType ConfabulationBase::GetKnowledgeBaseType(size_t source_module_index, size_t target_module_index)
{
	if (source_module_index < num_word_modules_ && target_module_index < num_word_modules_) {
		return KnowledgeBaseType::word_to_word_t;
	}

	if (source_module_index >= num_word_modules_ && target_module_index >= num_word_modules_) {
		return KnowledgeBaseType::phrase_to_phrase_t;
	}

	if (source_module_index < num_word_modules_ && target_module_index >= num_word_modules_) {
		return KnowledgeBaseType::word_to_phrase_t;
	}

	if (source_module_index >= num_word_modules_ && target_module_index < num_word_modules_) {
		return KnowledgeBaseType::phrase_to_word_t;
	}

	throw std::logic_error("Invalid knowledge base type");
}

void ConfabulationBase::Initialize(const std::vector<std::vector<bool>>& kb_specs,
								   const std::vector<uint8_t> level_specs,
								   const std::string &symbol_file,
								   const std::string &master_file,
								   uint8_t min_single_occurrences,
								   uint8_t min_multi_occurrences)
{
	if (2 * num_word_modules_ != kb_specs.size()) {
		std::cout << "Initialize called with knowledge base specs size = "
			<< kb_specs.size() << " and word modules size = " << num_word_modules_ << "\n" << std::flush;
		throw std::logic_error("Initialize called with wrong knowledge base specs");
	}
	kb_specs_ = kb_specs;
	level_specs_ = level_specs;
	symbol_file_ = symbol_file;
	master_file_ = master_file;
	min_single_occurrences_ = min_single_occurrences;
	min_multi_occurrences_ = min_multi_occurrences;
	Build();
	Learn(level_specs_[0]);
}

void ConfabulationBase::Build()
{
	log_info("Starting Build phase for confabulation");

	organizer_.reset(new MultiLevelOrganizer(level_specs_, ProduceSymbolMappings(symbol_file_, master_file_)));

	// create the modules
	for (size_t i = 0; i < num_modules_; ++i) {
		if (i < num_word_modules_) {
			const SymbolMapping* symbols_at_level = organizer_->GetMappingsForLevel(0);
			word_modules_.emplace_back(new Module<uint16_t>(*symbols_at_level, i));
		} else {
			word_modules_.emplace_back(nullptr);
		}
	}

	for (size_t i = 0; i < num_modules_; ++i) {
		if (i >= num_word_modules_) {
			const SymbolMapping* symbols_at_level = organizer_->GetMappingsForLevel(1);
			phrase_modules_.emplace_back(new Module<uint32_t>(*symbols_at_level, i));
		} else {
			phrase_modules_.emplace_back(nullptr);
		}
	}

	// create the knowledge bases according to specifications matrix
	word_to_word_knowledge_bases_.resize(num_modules_);
	for (size_t i = 0; i < num_modules_; ++i) {
		word_to_word_knowledge_bases_[i].resize(num_modules_);
		for (size_t j = 0; j < num_modules_; ++j) {
			if (kb_specs_[i][j] &&
				GetKnowledgeBaseType(i, j) == KnowledgeBaseType::word_to_word_t) {
				std::string id(std::to_string(i));
				id += "-";
				id += std::to_string(j);
				assert(i < num_word_modules_);
				assert(j < num_word_modules_);
				word_to_word_knowledge_bases_[i][j].reset(new KnowledgeBase<uint16_t, uint16_t>(id,
															   word_modules_[i]->GetSymbolMapping(),
															   word_modules_[j]->GetSymbolMapping()));
			} else
				word_to_word_knowledge_bases_[i][j].reset(nullptr);
		}
	}

	phrase_to_phrase_knowledge_bases_.resize(num_modules_);
	for (size_t i = 0; i < num_modules_; ++i) {
		phrase_to_phrase_knowledge_bases_[i].resize(num_modules_);
		for (size_t j = 0; j < num_modules_; ++j) {
			if (kb_specs_[i][j] &&
				GetKnowledgeBaseType(i, j) == KnowledgeBaseType::phrase_to_phrase_t) {
				std::string id(std::to_string(i));
				id += "-";
				id += std::to_string(j);
				assert(i >= num_word_modules_);
				assert(j >= num_word_modules_);
				phrase_to_phrase_knowledge_bases_[i][j].reset(new KnowledgeBase<uint32_t, uint32_t>(id,
															   phrase_modules_[i]->GetSymbolMapping(),
															   phrase_modules_[j]->GetSymbolMapping()));
			} else
				phrase_to_phrase_knowledge_bases_[i][j].reset(nullptr);
		}
	}

	word_to_phrase_knowledge_bases_.resize(num_modules_);
	for (size_t i = 0; i < num_modules_; ++i) {
		word_to_phrase_knowledge_bases_[i].resize(num_modules_);
		for (size_t j = 0; j < num_modules_; ++j) {
			if (kb_specs_[i][j] &&
				GetKnowledgeBaseType(i, j) == KnowledgeBaseType::word_to_phrase_t) {
				std::string id(std::to_string(i));
				id += "-";
				id += std::to_string(j);
				assert(j >= num_word_modules_);
				word_to_phrase_knowledge_bases_[i][j].reset(new KnowledgeBase<uint32_t, uint16_t>(id,
															   word_modules_[i]->GetSymbolMapping(),
															   phrase_modules_[j]->GetSymbolMapping()));
			} else
				word_to_phrase_knowledge_bases_[i][j].reset(nullptr);
		}
	}

	phrase_to_word_knowledge_bases_.resize(num_modules_);
	for (size_t i = 0; i < num_modules_; ++i) {
		phrase_to_word_knowledge_bases_[i].resize(num_modules_);
		for (size_t j = 0; j < num_modules_; ++j) {
			if (kb_specs_[i][j] &&
				GetKnowledgeBaseType(i, j) == KnowledgeBaseType::phrase_to_word_t) {
				std::string id(std::to_string(i));
				id += "-";
				id += std::to_string(j);
				assert(i >= num_word_modules_);
				phrase_to_word_knowledge_bases_[i][j].reset(new KnowledgeBase<uint16_t, uint32_t>(id,
															   phrase_modules_[i]->GetSymbolMapping(),
															   word_modules_[j]->GetSymbolMapping()));
			} else
				phrase_to_word_knowledge_bases_[i][j].reset(nullptr);
		}
	}

	log_info("Finished Build phase for confabulation");
}

void ConfabulationBase::Learn(size_t num_word_modules)
{
	log_info("Starting Learn phase for confabulation");

	TextReader text_reader(symbol_file_, master_file_);
	text_reader.Initialize();

	bool finished_reading = false;
	bool large_sentence = false;
	do {
		std::vector<std::string> read_sentence = text_reader.GetNextSentenceTokens(finished_reading);

		if (read_sentence.size() < 2) {
			continue;
		}

		std::deque<std::string> read_sentence_buffer(read_sentence.begin(), read_sentence.end());

		// std::cout << "\nRead sentence of size " << read_sentence.size() << " : " << VectorSymbolToSymbol(read_sentence, ' ') << "\n" << std::flush;

		do  {
			// in the case where the read sentence is larger than the number of word modules of the architecture,
			// we must use a sliding window approach to learn as much as possible from this large sentence
			size_t remaining_sentence_size = read_sentence_buffer.size();
			size_t current_sentence_size = 0u;
			if (num_word_modules < remaining_sentence_size) {
				large_sentence = true;
				current_sentence_size = num_word_modules;
			} else {
				current_sentence_size = remaining_sentence_size;
			}

			std::vector<std::string> sentence(read_sentence_buffer.begin(), read_sentence_buffer.begin() + current_sentence_size);

			if (large_sentence) {
				// in case of large sentences, a window step is chosen such that the next sentence
				// overlaps by kMaxMultiWordSize words with the current one
				read_sentence_buffer.erase(read_sentence_buffer.begin(), read_sentence_buffer.begin() + num_word_modules - Globals::kMaxMultiWordSize);
			} else {
				// in case of small sentences, we do not need the buffer any more
				read_sentence_buffer.clear();
			}

			// make sure that sentence does not wholly consist of empty strings

			if (!(FindFirstIndexNotOfSymbol(sentence, "") < 0)) {
				const std::vector<std::vector<std::string>>& activated_modules = organizer_->Organize(sentence);

				// convolve 2D phrases-words over the whole architecture so as to make the sentence position-neutral
				const std::vector<std::vector<std::string>>& module_combinations = ProduceKnowledgeLinkCombinations(activated_modules, num_modules_);

				// wire up the knowledge links
				for (const std::vector<std::string>& module_combination : module_combinations) {

					// std::cout << "Finding module activations for combination: " << VectorSymbolToSymbol(module_combination, '#') << "\n" << std::flush;

					// activation of all modules in a 1-D array
					for (size_t src = 0; src < num_modules_; ++src) {
						for (size_t targ = 0; targ < num_modules_; ++targ) {
							if (!module_combination[src].empty() && !module_combination[targ].empty()) {
								switch(GetKnowledgeBaseType(src, targ)) {
								case KnowledgeBaseType::word_to_word_t:
									if (word_to_word_knowledge_bases_[src][targ] != nullptr) {
										word_to_word_knowledge_bases_[src][targ]->Add(module_combination[src], module_combination[targ]);
									}
									break;
								case KnowledgeBaseType::phrase_to_phrase_t:
									if (phrase_to_phrase_knowledge_bases_[src][targ] != nullptr) {
										phrase_to_phrase_knowledge_bases_[src][targ]->Add(module_combination[src], module_combination[targ]);
									}
									break;
								case KnowledgeBaseType::word_to_phrase_t:
									if (word_to_phrase_knowledge_bases_[src][targ] != nullptr) {
										word_to_phrase_knowledge_bases_[src][targ]->Add(module_combination[src], module_combination[targ]);
									}
									break;
								case KnowledgeBaseType::phrase_to_word_t:
									if (phrase_to_word_knowledge_bases_[src][targ] != nullptr) {
										phrase_to_word_knowledge_bases_[src][targ]->Add(module_combination[src], module_combination[targ]);
									}
									break;
								}
							}
						}
					}
				}
			}
			large_sentence = false;
		} while (read_sentence_buffer.size() >= 2);
	} while (!finished_reading);

	// compute knowledge link strengths
	for (const std::vector<std::unique_ptr<KnowledgeBase<uint16_t, uint16_t>>>& kb_row : word_to_word_knowledge_bases_) {
		for (const std::unique_ptr<KnowledgeBase<uint16_t, uint16_t>>& kb : kb_row) {
			if (kb != nullptr) {
				kb->ComputeLinkStrengths();
				kb->ResetCooccurrenceCounts();
				kb->ResetTargetSymbolSums();
				log_info("Finished computing knowledge link strengths for knowledge base %s", kb->GetId().c_str());
			}
		}
	}

	for (const std::vector<std::unique_ptr<KnowledgeBase<uint32_t, uint32_t>>>& kb_row : phrase_to_phrase_knowledge_bases_) {
		for (const std::unique_ptr<KnowledgeBase<uint32_t, uint32_t>>& kb : kb_row) {
			if (kb != nullptr) {
				kb->ComputeLinkStrengths();
				kb->ResetCooccurrenceCounts();
				kb->ResetTargetSymbolSums();
				log_info("Finished computing knowledge link strengths for knowledge base %s", kb->GetId().c_str());
			}
		}
	}

	for (const std::vector<std::unique_ptr<KnowledgeBase<uint32_t, uint16_t>>>& kb_row : word_to_phrase_knowledge_bases_) {
		for (const std::unique_ptr<KnowledgeBase<uint32_t, uint16_t>>& kb : kb_row) {
			if (kb != nullptr) {
				kb->ComputeLinkStrengths();
				kb->ResetCooccurrenceCounts();
				kb->ResetTargetSymbolSums();
				log_info("Finished computing knowledge link strengths for knowledge base %s", kb->GetId().c_str());
			}
		}
	}

	for (const std::vector<std::unique_ptr<KnowledgeBase<uint16_t, uint32_t>>>& kb_row : phrase_to_word_knowledge_bases_) {
		for (const std::unique_ptr<KnowledgeBase<uint16_t, uint32_t>>& kb : kb_row) {
			if (kb != nullptr) {
				kb->ComputeLinkStrengths();
				kb->ResetCooccurrenceCounts();
				kb->ResetTargetSymbolSums();
				log_info("Finished computing knowledge link strengths for knowledge base %s", kb->GetId().c_str());
			}
		}
	}

	log_info("Finished Learn phase for confabulation");
}

void ConfabulationBase::Clean()
{
	for (const std::unique_ptr<Module<uint16_t>>& module : word_modules_) {
		if (module != nullptr) {
			module->ExcitationsToZero();
			module->TighteningLevelToZero();
		}
	}

	for (const std::unique_ptr<Module<uint32_t>>& module : phrase_modules_) {
		if (module != nullptr) {
			module->ExcitationsToZero();
			module->TighteningLevelToZero();
		}
	}
}

bool ConfabulationBase::CheckVocabulary(const std::vector<std::string> &symbols)
{
	for (size_t i = 0; i < std::min(symbols.size(), word_modules_.size()); ++i) {
		if ((!symbols[i].empty()) && (word_modules_[i] == nullptr)) {
			std::cout << "Input has activated symbol \"" << symbols[i] << "\" at position " << i
					  << " and corresponding module is null" << "\n" << std::flush;
			return false;
		} else if ((!symbols[i].empty()) && (!word_modules_[i]->GetSymbolMapping().Contains(symbols[i]))) {
			std::cout << "Input has activated symbol \"" << symbols[i] << "\" at position " << i
					  << " not contained in corresponding module" << "\n" << std::flush;
		}
	}

	return true;
}

std::vector<std::unique_ptr<SymbolMapping>> ConfabulationBase::ProduceSymbolMappings(const std::string &symbol_file, const std::string &master_file)
{
	std::vector<std::unique_ptr<SymbolMapping>> result;

	TextReader text_reader(symbol_file, master_file);
	text_reader.Initialize();

	NGramHandler ngram_handler(Globals::kMaxMultiWordSize, min_single_occurrences_, min_multi_occurrences_);

	std::vector<std::string> sentence;
	bool finished_reading = false;
	do {
		sentence = text_reader.GetNextSentenceTokens(finished_reading);
		ngram_handler.ExtractAndStoreNGrams(sentence);
	} while (!finished_reading);

	ngram_handler.CleanupNGrams();

	log_info("NGramHandler has found %lu words", ngram_handler.GetSingleWordCount());
	log_info("NGramHandler has found %lu multi-words", ngram_handler.GetMultiWordCount());

	result.push_back(ngram_handler.GetSingleWordSymbols()); // single-word symbols at position [0]
	result.push_back(ngram_handler.GetMultiWordSymbols()); // multi-word symbols (excluding single-word ones) at position [1]
	// result.push_back(ngram_handler.GetAllSymbols()); // multi-word symbols (including single-word ones) at position [1]

	return result; //at this point, NGramHandler is destroyed, so we save its internal memory
}

template <>
void ConfabulationBase::TransferAllExcitations(int8_t target_index, Module<uint16_t>* target_module)
{
	// use only modules that can contribute to the given index as possible source modules
	assert(GetModuleType(target_index) == ModuleType::word_t);
	for (size_t i = 0; i < num_word_modules_; ++i) {
		if (word_to_word_knowledge_bases_[i][target_index] != nullptr) {
			TransferExcitation(word_modules_[i].get(), word_to_word_knowledge_bases_[i][target_index].get(), target_module);
		}
	}

	for (size_t i = num_word_modules_; i < phrase_to_word_knowledge_bases_.size(); ++i) {
		if (phrase_to_word_knowledge_bases_[i][target_index] != nullptr) {
			TransferExcitation(phrase_modules_[i].get(), phrase_to_word_knowledge_bases_[i][target_index].get(), target_module);
		}
	}
}

template <>
void ConfabulationBase::TransferAllExcitations(int8_t target_index, Module<uint32_t>* target_module)
{
	// use only modules that can contribute to the given index as possible source modules
	assert(GetModuleType(target_index) == ModuleType::phrase_t);
	for (size_t i = num_word_modules_; i < phrase_to_phrase_knowledge_bases_.size(); ++i) {
		if (phrase_to_phrase_knowledge_bases_[i][target_index] != nullptr) {
			TransferExcitation(phrase_modules_[i].get(), phrase_to_phrase_knowledge_bases_[i][target_index].get(), target_module);
		}
	}

	for (size_t i = 0; i < num_word_modules_; ++i) {
		if (word_to_phrase_knowledge_bases_[i][target_index] != nullptr) {
			TransferExcitation(word_modules_[i].get(), word_to_phrase_knowledge_bases_[i][target_index].get(), target_module);
		}
	}
}
