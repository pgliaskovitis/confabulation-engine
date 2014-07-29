#include "Module.h"
#include "Globals.h"
#include "Utils.h"
#include <sparse_structures/DOKExcitationVector.hpp>

Module::Module(const SymbolMapping &symbol_mapping) :
    symbol_mapping_(symbol_mapping),
    excitations_(new DOKExcitationVector<float>(symbol_mapping.Size())),
    normalized_excitations_(nullptr),
    kb_inputs_(new DOKExcitationVector<size_t>(symbol_mapping.Size()))
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
    kb_inputs_.reset(new DOKExcitationVector<size_t>(symbol_mapping_.Size()));
}

void Module::ActivateSymbol(const Symbol &word, unsigned int K)
{
    normalized_excitations_.reset(nullptr);

    size_t index = symbol_mapping_.IndexOf(word);
    if (IsFrozen()) {
        // If module is frozen, only activate word when contained in the active symbols
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
        // If module is frozen, only activate word when contained in the active symbols
        if (frozen_indexes_->find(index) != frozen_indexes_->end()) {
            float previous_val = excitations_->GetElement(index);
            excitations_->SetElement(index, positive_clip(previous_val + value));
            unsigned int num_inputs = static_cast<unsigned int>(value / Globals::kBandGap) * Globals::kBandGap;
            kb_inputs_->SetElement(index, num_inputs + kb_inputs_->GetElement(index));
        }
    } else {
        float previous_val = excitations_->GetElement(index);
        excitations_->SetElement(index, positive_clip(previous_val + value));
        unsigned int num_inputs = static_cast<unsigned int>(value / Globals::kBandGap) * Globals::kBandGap;
        kb_inputs_->SetElement(index, num_inputs + kb_inputs_->GetElement(index));
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
