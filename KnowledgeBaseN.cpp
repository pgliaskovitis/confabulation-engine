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

#include "KnowledgeBaseN.h"
#include "Globals.h"

KnowledgeBaseN::KnowledgeBaseN(const std::string& id, std::unique_ptr<SymbolMapping>& src_map, std::unique_ptr<SymbolMapping>& targ_map) :
    id_(id),
    src_map_(std::move(src_map)),
    targ_map_(std::move(targ_map)),
    cooccurrence_counts_ (new DOKLinksMatrix<size_t>(targ_map->Size(), src_map->Size())),
    kbase_(new CSRLinksMatrix<float>(targ_map->Size(), src_map->Size())),
    target_symbol_sums_(targ_map->Size())
{}

void KnowledgeBaseN::Add(const std::string& src_symbol, const std::string& targ_symbol)
{
    size_t row = targ_map_->IndexOf(targ_symbol);
    size_t col = src_map_->IndexOf(src_symbol);

    Add(row, col);
}

void KnowledgeBaseN::Add(size_t targ_index, size_t src_index)
{
    kbase_.reset(nullptr);
    target_symbol_sums_[targ_index]++;
    size_t previous_count = cooccurrence_counts_->GetElement(targ_index, src_index);
    cooccurrence_counts_->SetElement(targ_index, src_index, previous_count + 1);
}

void KnowledgeBaseN::ComputeLinkStrengths()
{
    std::unique_ptr<DOKLinksMatrix<float>> link_strengths(
                new DOKLinksMatrix<float>(cooccurrence_counts_->get_num_rows(), cooccurrence_counts_->get_num_cols()));

    for (const std::pair<std::pair<size_t, size_t>, float>& e: cooccurrence_counts_->GetNzElements()) {
        size_t row = e.first.first;
        size_t col = e.first.second;
        link_strengths->SetElement(row, col, ComputeLinkStrength(e.second / (float) target_symbol_sums_[row]));
    }

    kbase_.reset(new CSRLinksMatrix<float>(*link_strengths));
}

float KnowledgeBaseN::GetPercentOfElementsLessThanThreshold(size_t threshold)
{
    int count = 0;
    for (const std::pair<std::pair<size_t, size_t>, float>& e: cooccurrence_counts_->GetNzElements())
        if (e.second < threshold)
            ++count;

    return count / cooccurrence_counts_->GetNnz();

}

std::unique_ptr<IExcitationVector<float> > KnowledgeBaseN::Transmit(const IExcitationVector<float> &normalized_excitations) const
{
    if (normalized_excitations.get_num_rows() != src_map_->Size())
        throw std::out_of_range("Input excitations should match the size of the input wordsmapping");

    return kbase_->Multiply(normalized_excitations);
}

float KnowledgeBaseN::ComputeLinkStrength(double antecedent_support_probability)
{
    if (antecedent_support_probability > Globals::kBaseProb)
        return static_cast<float>(log(antecedent_support_probability / (double) Globals::kBaseProb)) + Globals::kBandGap;

    return 0.0;
}
