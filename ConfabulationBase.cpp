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

ConfabulationBase::ConfabulationBase(const std::vector<std::vector<std::string>>& kb_specs) : kb_specs_(kb_specs)
{}

void ConfabulationBase::Initialize(const std::string &symbol_file, const std::string &master_file)
{
    TextReader text_reader(symbol_file, master_file);

//    std::vector<std::string> sentence;
//    do {
//        sentence = text_reader.GetNextSentenceTokens();
//        ngram_handler_->ExtractAndStoreNGrams();
//    } while (!sentence.empty());

//    unsigned short num_mods = kb_specs_.size();

//    // create the modules
//    modules_.resize(num_mods);
//    for (size_t i = 0; i < num_mods; ++i) {
//        modules_.push_back(new Module(mappings[i]));
//    }

//    // create the knowledge bases according to matrix
//    kbs = new KnowledgeBase[num_mods][num_mods];
//    for (int i = 0; i < num_mods; i++) {
//        for (int j = 0; j < num_mods; j++) {

//            if (kbs_spec[i][j]) {
//                kbs[i][j] = new KnowledgeBase(modules_[i].sm, modules_[j].sm);
//            }
//        }
//    }
}
