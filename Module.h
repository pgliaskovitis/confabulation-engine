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

#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include <set>
#include <sparse_structures/IExcitationVector.hpp>
#include <SymbolMapping.h>

class Module
{
public:
    Module(const SymbolMapping& symbol_mapping);
    Module(const Module& rhs) = delete;
    Module& operator=(const Module& rhs) = delete;
    Module(Module&& rhs) = delete;
    Module&& operator=(Module&& rhs) = delete;

    void Reset();
    void ExcitationsToZero();

    void ActivateSymbol(const std::string& word, int32_t K);

    void AddExcitationToIndex(uint32_t index, float value);
    void AddExcitationToAllSymbols(int32_t K);

    void AddExcitationVector(const IExcitationVector<float>& input);

    void Freeze();
    void Unfreeze();
    bool IsFrozen() const { return frozen_indexes_ != nullptr; }

    const std::unique_ptr<IExcitationVector<float>>& GetNormalizedExcitations();
    std::vector<std::string> GetExpectation();

    std::string ElementaryConfabulation(float *max_excitation);
    std::string ElementaryConfabulation(int32_t K, float *max_excitation);

    std::vector<std::string> PartialConfabulation(int32_t K);

    const SymbolMapping& get_symbol_mapping() { return symbol_mapping_; }

private:
    const SymbolMapping& symbol_mapping_;

    std::unique_ptr<IExcitationVector<float>> excitations_;
    std::unique_ptr<IExcitationVector<float>> normalized_excitations_;
    std::unique_ptr<IExcitationVector<uint32_t>> kb_inputs_;
    std::unique_ptr<std::set<uint32_t>> frozen_indexes_;

    std::unique_ptr<std::pair<uint32_t, float>> MaxExcitation(const std::set<std::pair<uint32_t, float>>& nz_excitations);
    std::set<std::pair<uint32_t, float>> ExcitationsAbove(int8_t K, const std::set<std::pair<uint32_t, float>>& nz_excitations);
    int32_t ActualK(int32_t K);
    int32_t MaxK();
};

#endif // MODULE_H
