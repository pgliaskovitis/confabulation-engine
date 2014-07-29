#include "Module.h"
#include "Globals.h"
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

void Module::ActivateWord(const Symbol &word, unsigned int K)
{
    normalized_excitations_.reset(nullptr);

    size_t index = symbol_mapping_.IndexOf(word);
    // If module is frozen, only activate word when contained in the active symbols
    if (IsFrozen()) {
        if (frozen_indexes_->find(index) != frozen_indexes_->end()) {
            excitations_->SetElement(index, K * Globals::kBandGap);
            kb_inputs_->SetElement(index, K);
            return;
        }
    } else {
        excitations_->SetElement(index, K * Globals::kBandGap);
        kb_inputs_->SetElement(index, K);
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
