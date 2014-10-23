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

#include "ConfabulationBase.h"

ConfabulationBase::ConfabulationBase(const std::vector<std::vector<bool>>& kb_specs, std::vector<unsigned short> level_specs) :
    kb_specs_(kb_specs),
    level_specs_(level_specs)
{}

void ConfabulationBase::Initialize(const std::string &symbol_file, const std::string &master_file)
{
    organizer_.reset(new MultiLevelOrganizer(level_specs_, ProduceSymbolMappings(symbol_file, master_file)));

    // create the modules
    unsigned short num_mods = kb_specs_.size();
    modules_.resize(num_mods);
    for (size_t i = 0; i < num_mods; ++i) {
        const std::unique_ptr<SymbolMapping>& symbols_at_level = organizer_->get_mappings_for_level(i);
        modules_.emplace_back(new Module(*symbols_at_level));
    }

    // create the knowledge bases according to specifications matrix
    knowledge_bases_.resize(num_mods);
    for (size_t i = 0; i < num_mods; ++i) {
        knowledge_bases_[i].resize(num_mods);
        for (size_t j = 0; j < num_mods; ++j) {
            if (kb_specs_[i][j]) {
                std::string id(std::to_string(i));
                id += "-";
                id += std::to_string(j);
                knowledge_bases_[i][j].reset(new KnowledgeBase(id, modules_[i]->get_symbol_mapping(), modules_[j]->get_symbol_mapping()));
            }
        }
    }
}

std::vector<std::unique_ptr<SymbolMapping>> ConfabulationBase::ProduceSymbolMappings(const std::string &symbol_file, const std::string &master_file)
{
    std::vector<std::unique_ptr<SymbolMapping>> result;

    result.resize(2); //currently, only single word symbols at position [0] and multi-word symbols at position [1]

    TextReader text_reader(symbol_file, master_file);
    text_reader.Initialize();

    NGramHandler ngram_handler(3);

    std::vector<std::string> sentence;
    bool finished_reading = false;
    do {
        sentence = text_reader.GetNextSentenceTokens(finished_reading);
        ngram_handler.ExtractAndStoreNGrams(sentence);
    } while (!finished_reading);

    result.push_back(ngram_handler.GetSingleWordSymbols());
    result.push_back(ngram_handler.GetAllSymbols());

    return result; //at this point, NGramHandler is destroyed so we save its internal memory
}
