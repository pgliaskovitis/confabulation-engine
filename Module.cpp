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

#include "Module.h"
#include "Globals.h"
#include "utils/Utils.h"
#include <sparse_structures/DOKExcitationVector.hpp>

Module::Module(const SymbolMapping &symbol_mapping) :
    symbol_mapping_(symbol_mapping),
    excitations_(new DOKExcitationVector<float>(symbol_mapping.Size())),
    kb_inputs_(new DOKExcitationVector<uint8_t>(symbol_mapping.Size())),
    current_excitation_level_(0)
{
    Reset();
}

void Module::Reset()
{
    ExcitationsToZero();
    Unfreeze();
}

void Module::ExcitationsToZero()
{
    excitations_.reset(new DOKExcitationVector<float>(symbol_mapping_.Size()));
    kb_inputs_.reset(new DOKExcitationVector<uint8_t>(symbol_mapping_.Size()));
    current_excitation_level_ = 0;
}

void Module::ActivateSymbol(const std::string &word, int8_t K)
{
    if (K < 0) {
        throw std::logic_error("ActivateSymbol called with negative K");
    }

    try {
        uint16_t index = symbol_mapping_.IndexOf(word);
        if (IsFrozen()) {
            // If module is frozen, only activate symbol if contained in the active symbols
            if (frozen_indexes_->find(index) != frozen_indexes_->end()) {
                excitations_->SetElement(index, K * Globals::kBandGap);
                kb_inputs_->SetElement(index, K);
            }
        } else {
            excitations_->SetElement(index, K * Globals::kBandGap);
            kb_inputs_->SetElement(index, K);
        }
    } catch (std::out_of_range&) {
        std::cout << "Word \"" << word << "\" could not be found in symbol mapping of module during activation" << "\n" << std::flush;
    }
}

void Module::AddExcitationToIndex(uint8_t index, float value)
{
    if (IsFrozen()) {
        // If module is frozen, only activate index if contained in the active indices
        if (frozen_indexes_->find(index) != frozen_indexes_->end()) {
            float new_val = excitations_->GetElement(index) + value;
            excitations_->SetElement(index, PositiveClip(new_val)); // value could be negative
            uint16_t num_inputs = static_cast<uint16_t>(new_val / Globals::kBandGap);
            kb_inputs_->SetElement(index, num_inputs + kb_inputs_->GetElement(index));
        }
    } else {
        float new_val = excitations_->GetElement(index) + value;
        excitations_->SetElement(index, PositiveClip(new_val)); // value could be negative
        uint16_t num_inputs = static_cast<uint16_t>(new_val / Globals::kBandGap);
        kb_inputs_->SetElement(index, num_inputs + kb_inputs_->GetElement(index));
    }
}

void Module::AddExcitationToAllSymbols(int8_t K)
{
    if (IsFrozen()) {
        // If module is frozen, only further activate already active symbols
        for (uint16_t i : *frozen_indexes_) {
            float val = PositiveClip(excitations_->GetElement(i) + K * Globals::kBandGap);
            excitations_->SetElement(i, val);
            uint16_t num_inputs = kb_inputs_->GetElement(i) + K;
            kb_inputs_->SetElement(i, num_inputs);
        }
    } else {
        for (const std::pair<uint16_t, float>& e : excitations_->GetNzElements()) {
            uint16_t i = e.first;
            float val = PositiveClip(e.second + K * Globals::kBandGap);
            excitations_->SetElement(i, val);
            uint16_t num_inputs = kb_inputs_->GetElement(i) + K;
            kb_inputs_->SetElement(i, num_inputs);
        }
    }
}

void Module::AddExcitationVector(const IExcitationVector<float> &input)
{
    if (IsFrozen()) {
        // If module is frozen, only further activate already active symbols
        for (uint16_t i : *frozen_indexes_) {
            float new_val = excitations_->GetElement(i) + input.GetElement(i);
            excitations_->SetElement(i,  PositiveClip(new_val));
            kb_inputs_->SetElement(i, kb_inputs_->GetElement(i) + 1);
        }
    } else {
        excitations_->Add(input);
        for (const std::pair<uint16_t, float>& e : input.GetNzElements()) {
            uint16_t i = e.first;
            kb_inputs_->SetElement(i, kb_inputs_->GetElement(i) + 1);
        }
    }
}

void Module::NormalizeExcitations()
{
    excitations_->Normalize();
}

void Module::Freeze()
{
    frozen_indexes_.reset(new std::set<uint16_t>());
    for (const std::pair<uint16_t, float>& e : excitations_->GetNzElements()) {
        frozen_indexes_->insert(e.first);
    }
}

void Module::Unfreeze()
{
    frozen_indexes_.reset(nullptr);
}

const std::unique_ptr<IExcitationVector<float> > &Module::GetExcitations()
{
    return excitations_;
}

std::unique_ptr<IExcitationVector<float>> Module::GetNormalizedExcitations()
{
    std::unique_ptr<IExcitationVector<float>> normalized_excitations;

    normalized_excitations.reset(new DOKExcitationVector<float>(symbol_mapping_.Size()));

    for (const std::pair<uint16_t, float>& e : excitations_->GetNzElements()) {
        normalized_excitations->SetElement(e.first, e.second);
    }

    normalized_excitations->Normalize();

    return normalized_excitations;
}

std::vector<std::string> Module::GetExpectation()
{
    std::vector<std::string> result(excitations_->GetNnz());

    uint16_t res_index = 0;
    for (const std::pair<uint16_t, float>& e : excitations_->GetNzElements()) {
        uint16_t index = e.first;
        result[res_index] = symbol_mapping_.GetSymbol(index);
        ++res_index;
    }

    return result;
}

std::string Module::ElementaryConfabulation(float *max_excitation)
{
    return ElementaryConfabulation(1, max_excitation);
}

std::string Module::ElementaryConfabulation(int8_t K, float *max_excitation)
{
    // compute K if negative
    K = ActualK(K);

    const std::set<std::pair<uint16_t, float>>& nz_excit = excitations_->GetNzElements();
    // std::cout << "Initially excited " << nz_excit.size() << " symbols" << " \n" << std::flush;

    uint16_t max_index = 0;
    uint16_t n_inputs_max = 0;

    // try with all possible K, starting from the maximum one, until a solution is found
    std::unique_ptr<std::pair<uint16_t, float>> max_excit;
    do {
        const std::set<std::pair<uint16_t, float>>& min_K_excit = ExcitationsAbove(K, nz_excit);
        // std::cout << "Reduced to " << min_K_excit.size() << " symbols for K=" << K << " \n" << std::flush;
        max_excit = MaxExcitation(min_K_excit);

        if (max_excit != nullptr) {
            max_index = max_excit->first;
            n_inputs_max = kb_inputs_->GetElement(max_index);
        } else {
            --K;
            continue;
        }
    } while ((max_excit == nullptr) && (K > 0));

    ExcitationsToZero();

    if (max_excit == nullptr) {
        Freeze();
        return Globals::kDummy;
    }

    // activate only the best word found
    excitations_->SetElement(max_index, max_excit->second);
    kb_inputs_->SetElement(max_index, n_inputs_max);
    *max_excitation = max_excit->second;

    Freeze();

    return symbol_mapping_.GetSymbol(max_index);
}

std::vector<std::string> Module::PartialConfabulation(int8_t K)
{
    std::vector<std::string> result;

    std::unique_ptr<std::vector<std::pair<uint16_t, float>>> expectations;

    K = ActualK(K);
    const std::set<std::pair<uint16_t, float>>& nz_excit = excitations_->GetNzElements();
    const std::set<std::pair<uint16_t, float>>& min_K_excit = ExcitationsAbove(K, nz_excit);
    expectations.reset(new std::vector<std::pair<uint16_t, float>>(min_K_excit.begin(), min_K_excit.end()));

    // saving needed info from intermediate state
    DOKExcitationVector<uint8_t> kb_inputs_temp(*kb_inputs_);

    // cleanup of intermediate state
    Reset();

    result.resize(expectations->size());
    uint16_t res_index = 0;

    // activate only symbols with excitations above threshold
    for (const std::pair<uint16_t, float>& e : *expectations) {
        uint16_t index = e.first;
        excitations_->SetElement(index, e.second);
        kb_inputs_->SetElement(index, kb_inputs_temp.GetElement(index));
        result[res_index] = symbol_mapping_.GetSymbol(index);
        ++res_index;
    }

    Freeze();

    return result;
}

std::vector<std::string> Module::AdditivePartialConfabulation(int8_t K)
{
    current_excitation_level_ += K;
    return PartialConfabulation(current_excitation_level_);
}

std::unique_ptr<std::pair<uint16_t, float> > Module::MaxExcitation(const std::set<std::pair<uint16_t, float> > &nz_excitations)
{
    std::unique_ptr<std::pair<uint16_t, float>> result(nullptr);
    float max_value = 0;

    for (const std::pair<uint16_t, float>& e : nz_excitations) {
        if (result == nullptr) {
            result.reset(new std::pair<uint16_t, float>(e.first, e.second));
        } else if (e.second > max_value) {
            result->first = e.first;
            result->second = e.second;
            max_value = e.second;
        }
    }

    return result;
}

std::set<std::pair<uint16_t, float> > Module::ExcitationsAbove(int8_t K, const std::set<std::pair<uint16_t, float> > &nz_excitations)
{
    std::set<std::pair<uint16_t, float>> result;

    for (const std::pair<uint16_t, float>& e : nz_excitations) {
        if (kb_inputs_->GetElement(e.first) >= K) {
            result.insert(e);
        }
    }

    return result;
}

int8_t Module::ActualK(int8_t K)
{
    if (K >= 0) {
        return K;
    }

    return MaxK() + K + 1;
}

int8_t Module::MaxK()
{
    int8_t result = 0;

    for (const std::pair<uint16_t, uint8_t>& e : kb_inputs_->GetNzElements()) {
        int8_t val = ConvertToSigned(e.second);
        if (val > result) {
            result = val;
        }
    }

    return result;
}
