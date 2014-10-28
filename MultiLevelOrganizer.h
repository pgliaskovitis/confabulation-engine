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

#ifndef MULTILEVELORGANIZER_H
#define MULTILEVELORGANIZER_H

#include <vector>
#include <memory>
#include "SymbolMapping.h"
#include "utils/HashTrie.hpp"

#ifndef DEBUG_1_H_
//#define DEBUG_1_H_
#endif

class MultiLevelOrganizer
{
public:
    MultiLevelOrganizer(const std::vector<unsigned short>& level_sizes_, std::vector<std::unique_ptr<SymbolMapping>>&& level_mappings);
    MultiLevelOrganizer(const MultiLevelOrganizer& rhs) = delete;
    MultiLevelOrganizer& operator=(const MultiLevelOrganizer& rhs) = delete;
    MultiLevelOrganizer(MultiLevelOrganizer&& rhs) = delete;
    MultiLevelOrganizer&& operator=(MultiLevelOrganizer&& rhs) = delete;

    std::vector<std::vector<std::string>> Organize(const std::vector<std::string>& symbols, bool &match_found);

    const std::unique_ptr<SymbolMapping>& get_mappings_for_level(unsigned short level) { return level_mappings_.at(level); }

private:
    std::vector<unsigned short> level_sizes_;
    std::vector<std::unique_ptr<SymbolMapping>> level_mappings_;
    std::vector<std::unique_ptr<HashTrie<std::string>>> tries_;
};

#endif // MULTILEVELORGANIZER_H
