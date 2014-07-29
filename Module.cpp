#include "Module.h"
#include "Globals.h"
#include "Utils.h"
#include <sparse_structures/DOKExcitationVector.hpp>

Module::Module(const SymbolMapping &symbol_mapping) :
    symbol_mapping_(symbol_mapping),
    excitations_(new DOKExcitationVector<float>(symbol_mapping.Size())),
    normalized_excitations_(nullptr),
    kb_inputs_(new DOKExcitationVector<unsigned int>(symbol_mapping.Size()))
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
    kb_inputs_.reset(new DOKExcitationVector<unsigned int>(symbol_mapping_.Size()));
}

void Module::ActivateSymbol(const Symbol &word, unsigned int K)
{
    normalized_excitations_.reset(nullptr);

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

void Module::ActivateAllSymbols(unsigned int K)
{
    normalized_excitations_.reset(nullptr);
    if (IsFrozen()) {
        // If module is frozen, only further activate already active symbols
        for (size_t i : *frozen_indexes_) {
            float val = excitations_->GetElement(i) + K * Globals::kBandGap;
            excitations_->SetElement(i, val);
            unsigned int num_inputs = kb_inputs_->GetElement(i) + K;
            kb_inputs_->SetElement(i, num_inputs);
        }
    } else {
        for (const std::pair<size_t, float>& e : excitations_->GetNzElements()) {
            size_t i = e.first;
            float val = e.second + K * Globals::kBandGap;
            excitations_->SetElement(i, val);
            unsigned int num_inputs = kb_inputs_->GetElement(i) + K;
            kb_inputs_->SetElement(i, num_inputs);
        }
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

void Module::AddExcitationToAllSymbols(const IExcitationVector<float> &input)
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

Symbol Module::ElementaryConfabulation()
{
    return ElementaryConfabulation(1);
}

Symbol Module::ElementaryConfabulation(unsigned int K)
{
    return "";
}
