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

#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include <set>

#include <sparse_structures/IExcitationVector.hpp>
#include <SymbolMapping.h>

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

class Module
{
public:
    Module(const SymbolMapping& symbol_mapping);

    void Reset();
    void ExcitationsToZero();

    void ActivateSymbol(const Symbol& word, int K);

    void AddExcitationToIndex(size_t index, float value);
    void AddExcitationToAllSymbols(int K);

    void AddExcitationVector(const IExcitationVector<float>& input);

    void Freeze();
    void Unfreeze();
    bool IsFrozen() const { return frozen_indexes_ != nullptr; }

    std::unique_ptr<IExcitationVector<float>> GetNormalizedExcitations();
    std::vector<Symbol> GetExpectation();

    Symbol ElementaryConfabulation();
    Symbol ElementaryConfabulation(int K);

    std::vector<Symbol> PartialConfabulation(int K, bool multiconf);

private:
    const SymbolMapping& symbol_mapping_;

    std::unique_ptr<IExcitationVector<float>> excitations_;
    std::unique_ptr<IExcitationVector<float>> normalized_excitations_;
    std::unique_ptr<IExcitationVector<int>> kb_inputs_;
    std::unique_ptr<std::set<size_t>> frozen_indexes_;

    std::unique_ptr<std::pair<size_t, float>> MaxExcitation(const std::set<std::pair<size_t, float>>& nz_excitations);
    std::set<std::pair<size_t, float>> ExcitationsAbove(int K, const std::set<std::pair<size_t, float>>& nz_excitations);
    int ActualK(int K);
    int MaxK();
};

#endif // MODULE_H
