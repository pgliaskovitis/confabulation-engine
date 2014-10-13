/*
 * Copyright 2014 Periklis Liaskovitis
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

#ifndef SYMBOLMAPPING_H
#define SYMBOLMAPPING_H

#include <string>
#include <set>
#include <unordered_map>
#include <memory>

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

class SymbolMapping
{
private:
    std::unique_ptr<std::unordered_map<Symbol, std::unique_ptr<Symbol>>> all_symbols_;
    std::unique_ptr<std::unordered_map<Symbol*, size_t>> symbol_to_index_;
    std::unique_ptr<std::unordered_map<size_t, Symbol*>> index_to_symbol_;

public:
    SymbolMapping();
    SymbolMapping(const SymbolMapping& rhs) = delete;
    SymbolMapping& operator=(const SymbolMapping& rhs) = delete;
    SymbolMapping(SymbolMapping&& rhs) = delete;
    SymbolMapping&& operator=(SymbolMapping&& rhs) = delete;

    void AddSymbol(const Symbol& symbol);
    bool Contains(const Symbol& symbol) const;

    size_t IndexOf(const Symbol& symbol) const;
    Symbol GetSymbol(size_t index) const;

    size_t Size() const { return symbol_to_index_->size(); }

    std::set<Symbol> GetAllSymbols();

    std::string ToString();
};

#endif // SYMBOLMAPPING_H
