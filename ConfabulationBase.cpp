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
#include "ConfabulationBase.h"
#include "utils/Utils.h"

ConfabulationBase::ConfabulationBase(const std::vector<std::vector<bool>>& kb_specs, std::vector<unsigned short> level_specs) :
    num_modules_(kb_specs.size()),
    K_(-1),
    kb_specs_(kb_specs),
    level_specs_(level_specs)
{}

void ConfabulationBase::Initialize(const std::string &symbol_file, const std::string &master_file)
{
    symbol_file_ = symbol_file;
    master_file_ = master_file;
    Build();
    Learn();
}

void ConfabulationBase::Build()
{
    organizer_.reset(new MultiLevelOrganizer(level_specs_, ProduceSymbolMappings(symbol_file_, master_file_)));

    // create the modules
    modules_.resize(num_modules_);
    for (size_t i = 0; i < num_modules_; ++i) {
        const std::unique_ptr<SymbolMapping>& symbols_at_level = organizer_->get_mappings_for_level(i);
        modules_.emplace_back(new Module(*symbols_at_level));
    }

    // create the knowledge bases according to specifications matrix
    knowledge_bases_.resize(num_modules_);
    for (size_t i = 0; i < num_modules_; ++i) {
        knowledge_bases_[i].resize(num_modules_);
        for (size_t j = 0; j < num_modules_; ++j) {
            if (kb_specs_[i][j]) {
                std::string id(std::to_string(i));
                id += "-";
                id += std::to_string(j);
                knowledge_bases_[i][j].reset(new KnowledgeBase(id,
                                                               modules_[i]->get_symbol_mapping(),
                                                               modules_[j]->get_symbol_mapping()));
            }
        }
    }
}

void ConfabulationBase::Learn()
{
    TextReader text_reader(symbol_file_, master_file_);
    text_reader.Initialize();

    std::vector<std::string> sentence;
    bool finished_reading = false;
    do {
        sentence = text_reader.GetNextSentenceTokens(finished_reading);

        bool match_found = false;
        const std::vector<std::vector<std::string>>& activated_modules = organizer_->Organize(sentence, match_found);
        const std::vector<std::vector<std::string>>& module_combinations = ProduceKnowledgeLinkCombinations(activated_modules, num_modules_);

        // wire the knowledge links
        for (const std::vector<std::string>& module_combination : module_combinations) {
            for (size_t src = 0; src < num_modules_; src++) {
                if (!module_combination[src].empty()) {
                    for (size_t targ = 0; targ < num_modules_; targ++) {
                        if ((knowledge_bases_[src][targ] != nullptr) && (!module_combination[targ].empty()))
                             knowledge_bases_[src][targ]->Add(module_combination[src], module_combination[targ]);
                    }
                }
            }
        }

    } while (!finished_reading);

    // compute knowledge link strengths
    for (const std::vector<std::unique_ptr<KnowledgeBase>>& kb_row : knowledge_bases_) {
        for (const std::unique_ptr<KnowledgeBase>& kb : kb_row) {
            if (kb != nullptr)
                kb->ComputeLinkStrengths();
        }
    }
}

int ConfabulationBase::ActualK(const std::vector<std::string> &symbols, int index_to_complete)
{
    int index = std::min(ConvertToSigned(symbols.size()), index_to_complete);

    // number of non-null symbols before index_to_complete
    int max_K = index; // - ArrayTools.number_equal(null, Arrays.copyOf(symbols, index));

    if (K_ >= 0) {
       return std::min(K_, max_K);
    } else {
       // autocompute: use max_K
       return max_K;
    }
}

void ConfabulationBase::Activate(const std::vector<std::string> &symbols)
{
    for (size_t i = 0; i < std::min(symbols.size(), modules_.size()); ++i) {
        if ((!symbols[i].empty()) && (modules_[i] != nullptr)) {
            modules_[i]->ActivateSymbol(symbols[i], 1);
            modules_[i]->Freeze();
        }
    }
}

void ConfabulationBase::TransferExcitation(const std::unique_ptr<Module> &source_module, const std::unique_ptr<KnowledgeBase> &kb, const std::unique_ptr<Module> &target_module)
{
    target_module->AddExcitationVector(*(kb->Transmit(*(source_module->GetNormalizedExcitations()))));
}

void ConfabulationBase::TransferAllExcitations(int target_index, const std::unique_ptr<Module>& target_module)
{
    for (const std::unique_ptr<Module>& source_module : modules_) {
        for (size_t i = 0; i < knowledge_bases_.size(); ++i) {
            if (knowledge_bases_[i][target_index] != nullptr) {
                TransferExcitation(source_module, knowledge_bases_[i][target_index], target_module);
            }
        }
    }
}

std::vector<std::string> ConfabulationBase::Confabulation(const std::vector<std::string> &symbols, int index_to_complete, bool expectation)
{
    int index;
    if (index_to_complete < 0) {
        index = AutoIndexToComplete(symbols);
    } else {
        index = index_to_complete;
    }

    int actual_K = ActualK(symbols, index);
    const std::unique_ptr<Module>& target_module = modules_[index];

    // core algorithm
    Activate(symbols);
    TransferAllExcitations(index, target_module);

    std::vector<std::string> result;

    if (expectation) {
        result = target_module->PartialConfabulation(actual_K, false);
    } else {
        result.push_back(target_module->ElementaryConfabulation(actual_K));
    }

    Clean();

    return result;
}

void ConfabulationBase::Clean()
{
    for (const std::unique_ptr<Module>& module : modules_) {
        if (module != nullptr) {
            module->Reset();
            module->Unfreeze();
        }
    }
}

std::vector<std::unique_ptr<SymbolMapping>> ConfabulationBase::ProduceSymbolMappings(const std::string &symbol_file, const std::string &master_file)
{
    std::vector<std::unique_ptr<SymbolMapping>> result;

    result.resize(2);

    TextReader text_reader(symbol_file, master_file);
    text_reader.Initialize();

    NGramHandler ngram_handler(3);

    std::vector<std::string> sentence;
    bool finished_reading = false;
    do {
        sentence = text_reader.GetNextSentenceTokens(finished_reading);
        ngram_handler.ExtractAndStoreNGrams(sentence);
    } while (!finished_reading);

    result.push_back(ngram_handler.GetSingleWordSymbols()); // single word symbols at position [0]
    result.push_back(ngram_handler.GetAllSymbols()); // multi-word symbols (including single word ones at position [1]

    return result; //at this point, NGramHandler is destroyed so we save its internal memory
}
