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

#include "KnowledgeBase.h"
#include "Globals.h"

KnowledgeBase::KnowledgeBase(const std::string& id, const SymbolMapping& src_map, const SymbolMapping& targ_map) :
	id_(id),
	src_map_(src_map),
	targ_map_(targ_map),
	cooccurrence_counts_(new SparseHashLinksMatrix<uint16_t, uint16_t, uint32_t>(targ_map.Size(), src_map.Size())),
	target_symbol_sums_(targ_map.Size())
{}

void KnowledgeBase::Add(const std::string& src_symbol, const std::string& targ_symbol)
{
	try {
		uint16_t row = targ_map_.IndexOf(targ_symbol);
		uint16_t col = src_map_.IndexOf(src_symbol);

		Add(row, col);
	} catch (std::out_of_range&) {
	}
}

void KnowledgeBase::Add(uint16_t targ_index, uint16_t src_index)
{
	target_symbol_sums_[targ_index]++;
	uint32_t previous_count = cooccurrence_counts_->GetElement(targ_index, src_index);
	cooccurrence_counts_->SetElement(targ_index, src_index, previous_count + 1);
}

void KnowledgeBase::ComputeLinkStrengths()
{
	std::unique_ptr<IKnowledgeLinks<uint16_t, uint16_t, float>> link_strengths(new SparseHashLinksMatrix<uint16_t, uint16_t, float>(cooccurrence_counts_->GetNumRows(), cooccurrence_counts_->GetNumCols()));

	for (const std::pair<std::pair<uint16_t, uint16_t>, float>& e: cooccurrence_counts_->GetNzElements()) {
		uint16_t row = e.first.first;
		uint16_t col = e.first.second;
		link_strengths->SetElement(row, col, ComputeLinkStrength(float(e.second) / (float) target_symbol_sums_[row]));
	}

	kbase_.reset(new CSRLinksMatrix<uint16_t, uint16_t, float>(*link_strengths));
}

std::unique_ptr<IExcitationVector<uint16_t, float>> KnowledgeBase::Transmit(const IExcitationVector<uint16_t, float> &normalized_excitations) const
{
	if (normalized_excitations.GetNumRows() != src_map_.Size()) {
		throw std::out_of_range("Input excitations should match the size of the input wordsmapping");
	}

	return kbase_->Multiply(normalized_excitations);
}

float KnowledgeBase::ComputeLinkStrength(double antecedent_support_probability)
{
	if (antecedent_support_probability > Globals::kBaseProb) {
		return static_cast<float>(Globals::kSynapseAmplifier *
								  (log2(antecedent_support_probability) - Globals::kBaseLogProb) +
								  Globals::kBandGap);
	} else if (antecedent_support_probability > 0.0) {
		return static_cast<float>(Globals::kBandGap);
	}

	return 0.0;
}
