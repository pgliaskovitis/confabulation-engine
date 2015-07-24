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
    normalized_excitations_(nullptr),
    kb_inputs_(new DOKExcitationVector<uint32_t>(symbol_mapping.Size()))
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
    normalized_excitations_.reset(nullptr);
    excitations_.reset(new DOKExcitationVector<float>(symbol_mapping_.Size()));
    kb_inputs_.reset(new DOKExcitationVector<uint32_t>(symbol_mapping_.Size()));
}

void Module::ActivateSymbol(const std::string &word, int32_t K)
{
    normalized_excitations_.reset(nullptr);

    if (K < 0)
        throw std::logic_error("ActivateSymbol called with negative K");

    try {
        uint32_t index = symbol_mapping_.IndexOf(word);
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

void Module::AddExcitationToIndex(uint32_t index, float value)
{
    normalized_excitations_.reset(nullptr);

    if (IsFrozen()) {
        // If module is frozen, only activate index if contained in the active indices
        if (frozen_indexes_->find(index) != frozen_indexes_->end()) {
            float new_val = excitations_->GetElement(index) + value;
            excitations_->SetElement(index, PositiveClip(new_val)); //value could be negative
            uint32_t num_inputs = static_cast<uint32_t>(new_val / Globals::kBandGap);
            kb_inputs_->SetElement(index, num_inputs + kb_inputs_->GetElement(index));
        }
    } else {
        float new_val = excitations_->GetElement(index) + value;
        excitations_->SetElement(index, PositiveClip(new_val)); //value could be negative
        uint32_t num_inputs = static_cast<uint32_t>(new_val / Globals::kBandGap);
        kb_inputs_->SetElement(index, num_inputs + kb_inputs_->GetElement(index));
    }
}

void Module::AddExcitationToAllSymbols(int32_t K)
{
    normalized_excitations_.reset(nullptr);

    if (IsFrozen()) {
        // If module is frozen, only further activate already active symbols
        for (uint32_t i : *frozen_indexes_) {
            float val = PositiveClip(excitations_->GetElement(i) + K * Globals::kBandGap);
            excitations_->SetElement(i, val);
            uint32_t num_inputs = kb_inputs_->GetElement(i) + K;
            kb_inputs_->SetElement(i, num_inputs);
        }
    } else {
        for (const std::pair<uint32_t, float>& e : excitations_->GetNzElements()) {
            uint32_t i = e.first;
            float val = PositiveClip(e.second + K * Globals::kBandGap);
            excitations_->SetElement(i, val);
            uint32_t num_inputs = kb_inputs_->GetElement(i) + K;
            kb_inputs_->SetElement(i, num_inputs);
        }
    }
}

void Module::AddExcitationVector(const IExcitationVector<float> &input)
{
    normalized_excitations_.reset(nullptr);

    if (IsFrozen()) {
        // If module is frozen, only further activate already active symbols
        for (uint32_t i : *frozen_indexes_) {
            float new_val = excitations_->GetElement(i) + input.GetElement(i);
            excitations_->SetElement(i,  PositiveClip(new_val));
            kb_inputs_->SetElement(i, kb_inputs_->GetElement(i) + 1);
        }
    } else {
        excitations_->Add(input);
        for (const std::pair<uint32_t, float>& e : input.GetNzElements()) {
            uint32_t i = e.first;
            kb_inputs_->SetElement(i, kb_inputs_->GetElement(i) + 1);
        }
    }
}

void Module::Freeze()
{
    frozen_indexes_.reset(new std::set<uint32_t>());
    for (const std::pair<uint32_t, float>& e : excitations_->GetNzElements())
        frozen_indexes_->insert(e.first);
}

void Module::Unfreeze()
{
    frozen_indexes_.reset(nullptr);
}

const std::unique_ptr<IExcitationVector<float> > &Module::GetNormalizedExcitations()
{
    if (normalized_excitations_ != nullptr) {
        return normalized_excitations_;
    }

    normalized_excitations_.reset(new DOKExcitationVector<float>(symbol_mapping_.Size()));

    double sum = 0.0;
    for (const std::pair<uint32_t, float>& e : excitations_->GetNzElements())
        sum += e.second;

    for (const std::pair<uint32_t, float>& e : excitations_->GetNzElements())
        normalized_excitations_->SetElement(e.first, (float) (e.second / sum));

    return normalized_excitations_;
}

std::vector<std::string> Module::GetExpectation()
{
    std::vector<std::string> result(excitations_->GetNnz());

    uint32_t res_index = 0;
    for (const std::pair<uint32_t, float>& e : excitations_->GetNzElements()) {
        uint32_t index = e.first;
        result[res_index] = symbol_mapping_.GetSymbol(index);
        ++res_index;
    }

    return result;
}

std::string Module::ElementaryConfabulation(float *max_excitation)
{
    return ElementaryConfabulation(1, max_excitation);
}

std::string Module::ElementaryConfabulation(int32_t K, float *max_excitation)
{
    normalized_excitations_.reset(nullptr);

    // compute K if negative
    K = ActualK(K);

    const std::set<std::pair<uint32_t, float>>& nz_excit = excitations_->GetNzElements();
    //std::cout << "Initially excited " << nz_excit.size() << " symbols" << " \n" << std::flush;

    uint32_t max_index = 0;
    uint32_t n_inputs_max = 0;

    // try with all possible K, starting from the maximum one, until a solution is found
    std::unique_ptr<std::pair<uint32_t, float>> max_excit;
    do {
        const std::set<std::pair<uint32_t, float>>& min_K_excit = ExcitationsAbove(K, nz_excit);
        //std::cout << "Reduced to " << min_K_excit.size() << " symbols for K=" << K << " \n" << std::flush;
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

std::vector<std::string> Module::PartialConfabulation(int32_t K, bool multiconf)
{
    normalized_excitations_.reset(nullptr);

    std::vector<std::string> result;

    std::unique_ptr<std::vector<std::pair<uint32_t, float>>> expectations;

    if (!multiconf) {
        K = ActualK(K);
        const std::set<std::pair<uint32_t, float>>& nz_excit = excitations_->GetNzElements();
        const std::set<std::pair<uint32_t, float>>& min_K_excit = ExcitationsAbove(K, nz_excit);
        expectations.reset(new std::vector<std::pair<uint32_t, float>>(min_K_excit.begin(), min_K_excit.end()));
    } else {
        // we are in the multiconfabulation case, so,
        // if the max excitation level is less than B, we keep everything, otherwise
        // we keep all the values such that value >= (max - BandGap), i.e.,
        // all the symbols that have at least as many incoming knowledge links as the
        // maximally excited one

        const std::set<std::pair<uint32_t, float>>& nz_excit = excitations_->GetNzElements();
        std::unique_ptr<std::pair<uint32_t, float>> max_excit = MaxExcitation(nz_excit);

        if (max_excit == nullptr) {
            Freeze();
            return result;
        }

        float threshold = std::max(max_excit->second - Globals::kBandGap, 0.0f);
        expectations.reset(new std::vector<std::pair<uint32_t, float>>());
        for (const std::pair<uint32_t, float>& e : nz_excit)
            if (e.second > threshold)
                expectations->push_back(e);
    }

    // saving needed info from intermediate state
    DOKExcitationVector<uint32_t> kb_inputs_temp(*kb_inputs_);

    // cleanup of intermediate state
    Reset();

    result.resize(expectations->size());
    uint32_t res_index = 0;

    // activate only symbols with excitations above threshold
    for (const std::pair<uint32_t, float>& e : *expectations) {
        uint32_t index = e.first;
        excitations_->SetElement(index, e.second);
        kb_inputs_->SetElement(index, kb_inputs_temp.GetElement(index));
        result[res_index] = symbol_mapping_.GetSymbol(index);
        ++res_index;
    }

    Freeze();

    return result;
}

std::unique_ptr<std::pair<uint32_t, float> > Module::MaxExcitation(const std::set<std::pair<uint32_t, float> > &nz_excitations)
{
    std::unique_ptr<std::pair<uint32_t, float>> result(nullptr);
    float max_value = 0;

    for (const std::pair<uint32_t, float>& e : nz_excitations) {
        if (result == nullptr) {
            result.reset(new std::pair<uint32_t, float>(e.first, e.second));
        } else if (e.second > max_value) {
            result->first = e.first;
            result->second = e.second;
            max_value = e.second;
        }
    }

    return result;
}

std::set<std::pair<uint32_t, float> > Module::ExcitationsAbove(int8_t K, const std::set<std::pair<uint32_t, float> > &nz_excitations)
{
    std::set<std::pair<uint32_t, float>> result;

    for (const std::pair<uint32_t, float>& e : nz_excitations) {
        if (kb_inputs_->GetElement(e.first) >= K) {
            result.insert(e);
        }
    }

    return result;
}

int32_t Module::ActualK(int32_t K)
{
    if (K >= 0)
        return K;

    return MaxK() + K + 1;
}

int32_t Module::MaxK()
{
    int32_t result = 0;

    for (const std::pair<uint32_t, uint32_t>& e : kb_inputs_->GetNzElements()) {
        int32_t val = ConvertToSigned(e.second);
        if (val > result) {
            result = val;
        }
    }

    return result;
}
