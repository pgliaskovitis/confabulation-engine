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
#include "TextReader.h"
#include "NGramHandler.h"
#include "MultiLevelOrganizer.h"
#include "Module.h"
#include "KnowledgeBase.h"

class ConfabulationBase
{
public:
    ConfabulationBase(const std::vector<std::vector<bool>>& kb_specs, std::vector<unsigned short> level_specs);

    ConfabulationBase(const ConfabulationBase& rhs) = delete;
    ConfabulationBase& operator=(const ConfabulationBase& rhs) = delete;
    ConfabulationBase(ConfabulationBase&& rhs) = delete;
    ConfabulationBase&& operator=(ConfabulationBase&& rhs) = delete;

    void Initialize(const std::string& symbol_file, const std::string& master_file);
    void Build();
    void Learn();

protected:
    std::unique_ptr<MultiLevelOrganizer> organizer_;

    std::vector<std::vector<bool>> kb_specs_;
    std::vector<unsigned short> level_specs_;
    std::vector<std::unique_ptr<Module>> modules_;
    std::vector<std::vector<std::unique_ptr<KnowledgeBase>>> knowledge_bases_;

    std::vector<std::unique_ptr<SymbolMapping>> ProduceSymbolMappings(const std::string &symbol_file, const std::string &master_file);
};

#endif // CONFABULATIONBASE_H
