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

#ifndef CONFABULATIONBASE_H
#define CONFABULATIONBASE_H

#include <vector>
#include <memory>
#include "text_processing/TextReader.h"
#include "text_processing/NGramHandler.h"
#include "MultiLevelOrganizer.h"
#include "Module.h"
#include "KnowledgeBase.h"

#ifndef DEBUG_1_H_
//#define DEBUG_1_H_
#endif

class ConfabulationBase
{
public:
    ConfabulationBase();
    ConfabulationBase(const ConfabulationBase& rhs) = delete;
    ConfabulationBase& operator=(const ConfabulationBase& rhs) = delete;
    ConfabulationBase(ConfabulationBase&& rhs) = delete;
    ConfabulationBase&& operator=(ConfabulationBase&& rhs) = delete;

    int ActualK(const std::vector<std::string> &symbols, int index_to_complete);
    int get_K() const { return K_; }
    void set_K(int K) { K_ = K; }

    void Initialize(const std::vector<std::vector<bool>>& kb_specs,
                    const std::vector<unsigned short> level_specs,
                    const std::string& symbol_file,
                    const std::string& master_file);
    void Build();
    void Learn();
    std::vector<std::string> Confabulation(const std::vector<std::string>& symbols, int index_to_complete, bool expectation);
    void Clean();

protected:
    unsigned short num_modules_;
    int K_;
    std::string symbol_file_;
    std::string master_file_;

    std::unique_ptr<MultiLevelOrganizer> organizer_;
    std::vector<std::vector<bool>> kb_specs_;
    std::vector<unsigned short> level_specs_;

    std::vector<std::unique_ptr<Module>> modules_;
    std::vector<std::vector<std::unique_ptr<KnowledgeBase>>> knowledge_bases_;

    bool CheckVocabulary(const std::vector<std::string>& symbols);
    void Activate(const std::vector<std::string>& symbols);
    void TransferExcitation(const std::unique_ptr<Module>& source_module, const std::unique_ptr<KnowledgeBase>& kb, const std::unique_ptr<Module>& target_module);
    void TransferAllExcitations(int target_index, const std::unique_ptr<Module> &target_module);
    std::vector<std::unique_ptr<SymbolMapping>> ProduceSymbolMappings(const std::string &symbol_file, const std::string &master_file);

    // compute index to complete with confabulation
    virtual int AutoIndexToComplete(const std::vector<std::string>& symbols) = 0;

    // check if provided index is valid in the current setup
    virtual bool CheckIndex(const std::vector<std::string>& symbols, int index_to_complete) = 0;

    // check arguments against current setup
    virtual bool CheckArguments(const std::vector<std::string>& symbols, int index_to_complete) = 0;
};

#endif // CONFABULATIONBASE_H
