#include "Module.h"
#include "Globals.h"
#include "Utils.h"
#include <sparse_structures/DOKExcitationVector.hpp>

Module::Module(const SymbolMapping &symbol_mapping) :
    symbol_mapping_(symbol_mapping),
    excitations_(new DOKExcitationVector<float>(symbol_mapping.Size())),
    normalized_excitations_(nullptr),
    kb_inputs_(new DOKExcitationVector<int>(symbol_mapping.Size()))
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
    kb_inputs_.reset(new DOKExcitationVector<int>(symbol_mapping_.Size()));
}

void Module::ActivateSymbol(const Symbol &word, int K)
{
    normalized_excitations_.reset(nullptr);

    if (K < 0)
        throw std::logic_error("ActivateSymbol with negative K");

    size_t index = symbol_mapping_.IndexOf(word);
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
}

void Module::AddExcitationToIndex(size_t index, float value)
{
    normalized_excitations_.reset(nullptr);

    if (IsFrozen()) {
        // If module is frozen, only activate index if contained in the active indices
        if (frozen_indexes_->find(index) != frozen_indexes_->end()) {
            float new_val = excitations_->GetElement(index) + value;
            excitations_->SetElement(index, PositiveClip(new_val)); //value could be negative
            unsigned int num_inputs = static_cast<unsigned int>(new_val / Globals::kBandGap);
            kb_inputs_->SetElement(index, num_inputs + kb_inputs_->GetElement(index));
        }
    } else {
        float new_val = excitations_->GetElement(index) + value;
        excitations_->SetElement(index, PositiveClip(new_val)); //value could be negative
        unsigned int num_inputs = static_cast<unsigned int>(new_val / Globals::kBandGap);
        kb_inputs_->SetElement(index, num_inputs + kb_inputs_->GetElement(index));
    }
}

void Module::AddExcitationToAllSymbols(int K)
{
    normalized_excitations_.reset(nullptr);
    if (IsFrozen()) {
        // If module is frozen, only further activate already active symbols
        for (size_t i : *frozen_indexes_) {
            float val = PositiveClip(excitations_->GetElement(i) + K * Globals::kBandGap);
            excitations_->SetElement(i, val);
            unsigned int num_inputs = kb_inputs_->GetElement(i) + K;
            kb_inputs_->SetElement(i, num_inputs);
        }
    } else {
        for (const std::pair<size_t, float>& e : excitations_->GetNzElements()) {
            size_t i = e.first;
            float val = PositiveClip(e.second + K * Globals::kBandGap);
            excitations_->SetElement(i, val);
            unsigned int num_inputs = kb_inputs_->GetElement(i) + K;
            kb_inputs_->SetElement(i, num_inputs);
        }
    }
}

void Module::AddExcitationVector(const IExcitationVector<float> &input)
{
    normalized_excitations_.reset(nullptr);

    if (IsFrozen()) {
        // If module is frozen, only further activate already active symbols
        for (size_t i : *frozen_indexes_) {
            float new_val = excitations_->GetElement(i) + input.GetElement(i);
            excitations_->SetElement(i,  PositiveClip(new_val));
            kb_inputs_->SetElement(i, kb_inputs_->GetElement(i) + 1);
        }
    } else {
        excitations_->Add(input);
        for (const std::pair<size_t, float>& e : input.GetNzElements()) {
            size_t i = e.first;
            kb_inputs_->SetElement(i, kb_inputs_->GetElement(i) + 1);
        }
    }
}

void Module::Freeze()
{
    frozen_indexes_.reset(new std::set<size_t>());
    for (const std::pair<size_t, float>& e : excitations_->GetNzElements())
        frozen_indexes_->insert(e.first);
}

void Module::Unfreeze()
{
    frozen_indexes_.reset(nullptr);
}

std::vector<Symbol> Module::GetExpectation()
{
    std::vector<Symbol> result(excitations_->GetNnz());

    size_t nz_index = 0;
    for (const std::pair<size_t, float>& e : excitations_->GetNzElements()) {
        size_t index = e.first;
        result[nz_index] = symbol_mapping_.GetSymbol(index);
        ++nz_index;
    }

    return result;
}

Symbol Module::ElementaryConfabulation()
{
    return ElementaryConfabulation(1);
}

Symbol Module::ElementaryConfabulation(int K)
{
    normalized_excitations_.reset(nullptr);

    // compute K if negative
    K = ActualK(K);

    const std::set<std::pair<size_t, float>>& nz_excit = excitations_->GetNzElements();
    const std::set<std::pair<size_t, float>>& min_K_input = ExcitationsAbove(K, nz_excit);
    std::unique_ptr<std::pair<size_t, float>> max_excit = MaxExcitation(min_K_input);

    int max_index = -1;
    int n_inputs_max = -1;
    if (max_excit != nullptr) {
        max_index = max_excit->first;
        n_inputs_max = kb_inputs_->GetElement(max_index);
    }

    ExcitationsToZero();

    if (max_excit == nullptr) {
        Freeze();
        return Globals::kDummy;
    }

    // activate only the best word found
    excitations_->SetElement(max_index, max_excit->second);
    kb_inputs_->SetElement(max_index, n_inputs_max);

    Freeze();

    return symbol_mapping_.GetSymbol(max_index);
}

std::unique_ptr<std::pair<size_t, float> > Module::MaxExcitation(const std::set<std::pair<size_t, float> > &nz_excitations)
{
    std::unique_ptr<std::pair<size_t, float>> result(nullptr);
    float max_value = 0;

    for (const std::pair<size_t, float>& e : nz_excitations) {
        if (result == nullptr) {
            result.reset(new std::pair<size_t, float>(e.first, e.second));
        } else if (e.second > max_value) {
            result->first = e.first;
            result->second = e.second;
            max_value = e.second;
        }
    }

    return result;
}

std::set<std::pair<size_t, float> > Module::ExcitationsAbove(int K, const std::set<std::pair<size_t, float> > &nz_excitations)
{
    std::set<std::pair<size_t, float>> result;

    for (const std::pair<size_t, float>& e : nz_excitations) {
        if (kb_inputs_->GetElement(e.first) >= K) {
            result.insert(e);
        }
    }

    return result;
}

int Module::ActualK(int K)
{
    if (K >= 0)
        return K;

    return MaxK() + 1 + K;
}

int Module::MaxK()
{
    int result = 0;

    for (const std::pair<size_t, float>& e : kb_inputs_->GetNzElements()) {
        int val = e.second;
        if (val > result) {
            result = val;
        }
    }

    return result;
}
