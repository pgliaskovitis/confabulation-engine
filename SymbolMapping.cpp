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

#include "SymbolMapping.h"

SymbolMapping::SymbolMapping() :
    all_symbols_(new std::unordered_map<Symbol, std::unique_ptr<Symbol>>),
    symbol_to_index_(new std::unordered_map<Symbol*, size_t>),
    index_to_symbol_(new std::unordered_map<size_t, Symbol*>)
{}

void SymbolMapping::AddSymbol(const Symbol &symbol)
{
    std::unordered_map<Symbol, std::unique_ptr<Symbol>>::const_iterator existence_it = all_symbols_->find(symbol);
    if (existence_it == all_symbols_->end()) {
        size_t index = all_symbols_->size();
        std::unique_ptr<Symbol> symbol_ptr(new Symbol(symbol));
        all_symbols_->insert(std::make_pair(symbol, std::move(symbol_ptr)));
        std::unique_ptr<Symbol>& storage_ptr = all_symbols_->at(symbol);
        symbol_to_index_->insert(std::make_pair(storage_ptr.get(), index));
        index_to_symbol_->insert(std::make_pair(index, storage_ptr.get()));
    }
}

bool SymbolMapping::Contains(const Symbol &symbol) const
{
    std::unordered_map<Symbol, std::unique_ptr<Symbol>>::const_iterator it = all_symbols_->find(symbol);
    return (it != all_symbols_->end());
}

size_t SymbolMapping::IndexOf(const Symbol &symbol) const
{
    const std::unique_ptr<Symbol>& existence_ptr = all_symbols_->at(symbol);
    return symbol_to_index_->at(existence_ptr.get());
}

Symbol SymbolMapping::GetSymbol(size_t index) const
{
    const Symbol* existence_ptr = index_to_symbol_->at(index);
    return *existence_ptr;
}

std::set<Symbol> SymbolMapping::GetAllSymbols()
{
    std::set<Symbol> result;

    std::unordered_map<Symbol, std::unique_ptr<Symbol>>::const_iterator it = all_symbols_->begin();
    for(; it != all_symbols_->end(); ++it)
        result.insert(it->first);

    return result;
}

std::string SymbolMapping::ToString()
{
    std::unordered_map<Symbol*, size_t>::const_iterator it = symbol_to_index_->begin();
    std::string result;
    for(; it != symbol_to_index_->end(); ++it) {
        result += *(it->first) + " ";
        result += std::to_string(it->second) + "\n";
     }

    return result;
}
