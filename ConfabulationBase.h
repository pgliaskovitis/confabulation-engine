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

#include <vector>
#include <memory>
#include "text_processing/TextReader.h"
#include "text_processing/NGramHandler.h"
#include "MultiLevelOrganizer.h"
#include "Module.hpp"
#include "KnowledgeBase.hpp"

enum class ModuleType {
	word_t,
	phrase_t
};

enum class KnowledgeBaseType {
	word_to_word_t,
	phrase_to_phrase_t,
	word_to_phrase_t,
	phrase_to_word_t
};

class ConfabulationBase
{
public:
	ConfabulationBase();
	ConfabulationBase(const ConfabulationBase& rhs) = delete;
	ConfabulationBase& operator=(const ConfabulationBase& rhs) = delete;
	ConfabulationBase(ConfabulationBase&& rhs) = delete;
	ConfabulationBase&& operator=(ConfabulationBase&& rhs) = delete;

	~ConfabulationBase();

	ModuleType GetModuleType(size_t module_index);
	KnowledgeBaseType GetKnowledgeBaseType(size_t source_module_index, size_t target_module_index);

	void Initialize(const std::vector<std::vector<bool>>& kb_specs,
					const std::vector<uint8_t> level_specs,
					const std::string& symbol_file,
					const std::string& master_file,
					uint8_t min_single_occurrences,
					uint8_t min_multi_occurrences);
	void Build();
	void Learn(size_t num_word_modules);
	void Clean();
	MultiLevelOrganizer* GetOrganizer() { return organizer_.get(); }

	virtual std::vector<std::string> Confabulation(const std::vector<std::string>& symbols, int8_t index_to_complete, bool expectation) = 0;
	virtual void Activate(const std::vector<std::string>& symbols) = 0;

	template <typename TRow, typename TCol>
	void TransferExcitation(Module<TCol>* source_module, KnowledgeBase<TRow, TCol>* kb, Module<TRow>* target_module);

	template <typename TRow>
	void TransferAllExcitations(int8_t target_index, Module<TRow>* target_module);

	Module<uint16_t>* GetWordModule(size_t index) { return word_modules_[index].get(); }
	Module<uint32_t>* GetPhraseModule(size_t index) { return phrase_modules_[index].get(); }
	KnowledgeBase<uint16_t, uint16_t>* GetWordToWordKnowledgeBase(size_t source, size_t target)
	{
		return word_to_word_knowledge_bases_[source][target].get();
	}
	KnowledgeBase<uint32_t, uint32_t>* GetPhraseToPhraseKnowledgeBase(size_t source, size_t target)
	{
		return phrase_to_phrase_knowledge_bases_[source][target].get();
	}
	KnowledgeBase<uint32_t, uint16_t>* GetWordToPhraseKnowledgeBase(size_t source, size_t target)
	{
		return word_to_phrase_knowledge_bases_[source][target].get();
	}
	KnowledgeBase<uint16_t, uint32_t>* GetPhraseToWordKnowledgeBase(size_t source, size_t target)
	{
		return phrase_to_word_knowledge_bases_[source][target].get();
	}

protected:
	uint8_t num_modules_;
	uint8_t num_word_modules_;
	uint8_t num_phrase_modules_;
	uint8_t min_single_occurrences_;
	uint8_t min_multi_occurrences_;
	int8_t start_position_;
	std::string symbol_file_;
	std::string master_file_;

	std::unique_ptr<MultiLevelOrganizer> organizer_;
	std::vector<std::vector<bool>> kb_specs_;
	std::vector<uint8_t> level_specs_;

	std::vector<std::unique_ptr<Module<uint16_t>>> word_modules_;
	std::vector<std::unique_ptr<Module<uint32_t>>> phrase_modules_;

	std::vector<std::vector<std::unique_ptr<KnowledgeBase<uint16_t, uint16_t>>>> word_to_word_knowledge_bases_;
	std::vector<std::vector<std::unique_ptr<KnowledgeBase<uint32_t, uint32_t>>>> phrase_to_phrase_knowledge_bases_;
	std::vector<std::vector<std::unique_ptr<KnowledgeBase<uint32_t, uint16_t>>>> word_to_phrase_knowledge_bases_;
	std::vector<std::vector<std::unique_ptr<KnowledgeBase<uint16_t, uint32_t>>>> phrase_to_word_knowledge_bases_;

	bool CheckVocabulary(const std::vector<std::string>& symbols);
	std::vector<std::unique_ptr<SymbolMapping>> ProduceSymbolMappings(const std::string &symbol_file, const std::string &master_file);

	// compute index to complete with confabulation
	virtual int8_t AutoIndexToComplete(const std::vector<std::string>& symbols) = 0;

	// check if provided index is valid in the current setup
	virtual bool CheckIndex(const std::vector<std::string>& symbols, int8_t index_to_complete) = 0;

	// check arguments against current setup
	virtual bool CheckArguments(const std::vector<std::string>& symbols, int8_t index_to_complete) = 0;

	// index to complete taking into account non-activated modules
	virtual int8_t GetStartPosition(const std::vector<std::string> &symbols, int8_t index_to_complete);
};
