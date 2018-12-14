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

#pragma once

#include <string>
#include <memory>

#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/SparseHashLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"
#include "SymbolMapping.h"
#include "Globals.h"

template <typename TRow, typename TCol>
class KnowledgeBase
{
public:
	KnowledgeBase(const std::string& id, const SymbolMapping& src_map, const SymbolMapping& targ_map);
	KnowledgeBase(const KnowledgeBase& rhs) = delete;
	KnowledgeBase& operator=(const KnowledgeBase& rhs) = delete;
	KnowledgeBase(KnowledgeBase&& rhs) = delete;
	KnowledgeBase&& operator=(KnowledgeBase&& rhs) = delete;

	void Add(const std::string& src_symbol, const std::string& targ_symbol);
	void Add(TRow targ_index, TCol src_index);
	void ComputeLinkStrengths();

	std::unique_ptr<IExcitationVector<TRow, float>> Transmit(const IExcitationVector<TCol, float>& normalized_excitations) const;

	void ResetCooccurrenceCounts() { cooccurrence_counts_.reset(); }
	void ResetTargetSymbolSums() { target_symbol_sums_.clear(); }
	std::string GetId() const { return id_; }
	uint32_t GetNumKnowledgeLinks() const { return kbase_->GetNnz(); }

private:
	// The knowledge base is essentially a sparse matrix of likelihoods P(s | t)
	// Symbols s are the likelihood values that pass through the knowledge base
	// Symbols t are the likelihood parameters that are computed by the knowledge base
	// In order to be able to transmit excitations with left-hand side matrix multiplication
	// the source symbols must correspond to the COLUMNS of the matrix
	// and the target symbols to the ROWS of the matrix
	// (hence target_symbol_sums_ are essentially the row sums)

	const std::string id_;
	const SymbolMapping& src_map_;
	const SymbolMapping& targ_map_;
	std::unique_ptr<IKnowledgeLinks<TRow, TCol, uint32_t>> cooccurrence_counts_;
	std::unique_ptr<IKnowledgeLinks<TRow, TCol, float>> kbase_;
	std::vector<uint32_t> target_symbol_sums_;

	static float ComputeLinkStrength(double antecedent_support_probability);
};

template <typename TRow, typename TCol>
KnowledgeBase<TRow, TCol>::KnowledgeBase(const std::string& id, const SymbolMapping& src_map, const SymbolMapping& targ_map) :
	id_(id),
	src_map_(src_map),
	targ_map_(targ_map),
	cooccurrence_counts_(new SparseHashLinksMatrix<TRow, TCol, uint32_t>(targ_map.Size(), src_map.Size())),
	target_symbol_sums_(targ_map.Size())
{}

template <typename TRow, typename TCol>
void KnowledgeBase<TRow, TCol>::Add(const std::string& src_symbol, const std::string& targ_symbol)
{
	TCol col;
	TRow row;

	try {
		col = src_map_.IndexOf(src_symbol);
	} catch (std::out_of_range&) {
		std::cout << "Could not find symbol " << src_symbol << " in source module for base " << GetId() << std::endl;
		return;
	}

	try {
		row = targ_map_.IndexOf(targ_symbol);
	} catch (std::out_of_range&) {
		std::cout << "Could not find symbol " << targ_symbol << " in target module for base " << GetId() << std::endl;
		return;
	}

	Add(row, col);
}

template <typename TRow, typename TCol>
void KnowledgeBase<TRow, TCol>::Add(TRow targ_index, TCol src_index)
{
	target_symbol_sums_[targ_index]++;
	uint32_t previous_count = cooccurrence_counts_->GetElement(targ_index, src_index);
	cooccurrence_counts_->SetElement(targ_index, src_index, previous_count + 1);
}

template <typename TRow, typename TCol>
void KnowledgeBase<TRow, TCol>::ComputeLinkStrengths()
{
	std::unique_ptr<IKnowledgeLinks<TRow, TCol, float>> link_strengths(new SparseHashLinksMatrix<TRow, TCol, float>(cooccurrence_counts_->GetNumRows(), cooccurrence_counts_->GetNumCols()));

	for (const std::pair<std::pair<TRow, TCol>, uint32_t>& e: cooccurrence_counts_->GetNzElements()) {
		TRow row = e.first.first;
		TCol col = e.first.second;
		link_strengths->SetElement(row, col,
								   ComputeLinkStrength(
								   static_cast<float>(e.second) /
								   static_cast<float>(target_symbol_sums_[row])));
	}

	kbase_.reset(new CSRLinksMatrix<TRow, TCol, float>(*link_strengths));
}

template <typename TRow, typename TCol>
std::unique_ptr<IExcitationVector<TRow, float>> KnowledgeBase<TRow, TCol>::Transmit(const IExcitationVector<TCol, float>& normalized_excitations) const
{
	if (normalized_excitations.GetNumRows() != src_map_.Size()) {
		throw std::out_of_range("Input excitations should match the size of the input wordsmapping");
	}

	return kbase_->Multiply(normalized_excitations);
}

template <typename TRow, typename TCol>
float KnowledgeBase<TRow, TCol>::ComputeLinkStrength(double antecedent_support_probability)
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
