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

#include <string>
#include <set>
#include <memory>
#include "flat_hash_map/bytell_hash_map.hpp"

class SymbolMapping
{
public:
	SymbolMapping();
	SymbolMapping(const SymbolMapping& rhs) = delete;
	SymbolMapping& operator=(const SymbolMapping& rhs) = delete;
	SymbolMapping(SymbolMapping&& rhs) = delete;
	SymbolMapping&& operator=(SymbolMapping&& rhs) = delete;

	void AddSymbol(const std::string& symbol);
	bool Contains(const std::string& symbol) const;

	uint32_t IndexOf(const std::string& symbol) const;
	std::string GetSymbol(uint32_t index) const;

	uint32_t Size() const { return symbol_to_index_->size(); }

	std::set<std::string> GetAllSymbols() const;

	std::string ToString() const;

private:
	std::unique_ptr<ska::bytell_hash_map<std::string, std::unique_ptr<std::string>>> all_symbols_;
	std::unique_ptr<ska::bytell_hash_map<std::string*, uint32_t>> symbol_to_index_;
	std::unique_ptr<ska::bytell_hash_map<uint32_t, std::string*>> index_to_symbol_;
};
