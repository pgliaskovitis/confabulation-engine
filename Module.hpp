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
#include <vector>
#include <set>
#include <mutex>
#include "sparse_structures/KHashExcitationVector.hpp"
#include "utils/Utils.h"
#include "SymbolMapping.h"
#include "Globals.h"

template <typename TRow>
class Module
{
public:
	Module(const SymbolMapping& symbol_mapping, size_t id);
	Module(const Module& rhs) = delete;
	Module& operator=(const Module& rhs) = delete;
	Module(Module&& rhs) = delete;
	Module&& operator=(Module&& rhs) = delete;

	void ActivateSymbol(const std::string& word, int8_t band_add_level);
	void AddExcitationVector(const IExcitationVector<TRow, float>& input);
	void ExcitationsToZero();
	void TighteningLevelToZero();
	void Lock();
	void UnLock();

	IExcitationVector<TRow, float>* GetExcitations() const { return excitations_.get(); }
	std::unique_ptr<IExcitationVector<TRow, float>> GetNormalizedExcitations() const;
	std::unique_ptr<IExcitationVector<TRow, float>> GetWhitenedExcitations() const;
	std::vector<std::string> GetExpectation() const;

	std::string ElementaryConfabulation(float *max_excitation);
	std::string ElementaryConfabulation(int8_t band_cut_level, float *max_excitation);

	std::vector<std::string> PartialConfabulation(int8_t band_cut_level);
	std::vector<std::string> TighteningPartialConfabulation(int8_t band_tighten_level);

	size_t GetId() const { return id_; }
	size_t GetCurrentTighteningLevel() const { return current_tightening_level_ ; }
	const SymbolMapping& GetSymbolMapping() const { return symbol_mapping_; }

private:
	const SymbolMapping& symbol_mapping_;
	size_t id_;
	std::mutex mutex_;
	std::unique_ptr<IExcitationVector<TRow, float>> excitations_;
	std::unique_ptr<IExcitationVector<TRow, uint16_t>> kb_inputs_;

	std::unique_ptr<std::pair<TRow, float>> MaxExcitation(const std::set<std::pair<TRow, float> > &nz_excitations);
	std::set<std::pair<TRow, float>> ExcitationsAbove(int8_t K, const std::set<std::pair<TRow, float>>& nz_excitations);

	size_t current_tightening_level_;
};

template <typename TRow>
Module<TRow>::Module(const SymbolMapping &symbol_mapping, size_t id) :
	symbol_mapping_(symbol_mapping),
	id_(id),
	excitations_(new KHashExcitationVector<TRow, float>(symbol_mapping.Size())),
	kb_inputs_(new KHashExcitationVector<TRow, uint16_t>(symbol_mapping.Size())),
	current_tightening_level_(0)
{
	ExcitationsToZero();
}

template <typename TRow>
void Module<TRow>::ActivateSymbol(const std::string &word, int8_t band_add_level)
{
	try {
		TRow index = symbol_mapping_.IndexOf(word);
		excitations_->SetElement(index, band_add_level * Globals::kBandGap);
		kb_inputs_->SetElement(index, band_add_level);
	} catch (std::out_of_range&) {
		std::cout << "Word \"" << word << "\" could not be found in symbol mapping of module " << GetId() << " during activation" << "\n" << std::flush;
	}
}

template <typename TRow>
void Module<TRow>::AddExcitationVector(const IExcitationVector<TRow, float> &input)
{
	excitations_->Add(input);
	for (const std::pair<TRow, float>& e : input.GetNzElements()) {
		TRow i = e.first;
		kb_inputs_->SetElement(i, kb_inputs_->GetElement(i) + 1);
	}
}

template <typename TRow>
std::unique_ptr<IExcitationVector<TRow, float>> Module<TRow>::GetNormalizedExcitations() const
{
	std::unique_ptr<IExcitationVector<TRow, float>> normalized_excitations;
	normalized_excitations.reset(new KHashExcitationVector<TRow, float>(symbol_mapping_.Size()));

	for (const std::pair<TRow, float>& e : excitations_->GetNzElements()) {
		normalized_excitations->SetElement(e.first, e.second);
	}

	normalized_excitations->Normalize();

	return normalized_excitations;
}

template <typename TRow>
std::unique_ptr<IExcitationVector<TRow, float>> Module<TRow>::GetWhitenedExcitations() const
{
	std::unique_ptr<IExcitationVector<TRow, float>> whitened_excitations;
	whitened_excitations.reset(new KHashExcitationVector<TRow, float>(symbol_mapping_.Size()));

	for (const std::pair<TRow, float>& e : excitations_->GetNzElements()) {
		whitened_excitations->SetElement(e.first, e.second);
	}

	whitened_excitations->Whiten();

	return whitened_excitations;
}

template <typename TRow>
std::vector<std::string> Module<TRow>::GetExpectation() const
{
	std::vector<std::string> result(excitations_->GetNnz());

	TRow res_index = 0;
	for (const std::pair<TRow, float>& e : excitations_->GetNzElements()) {
		TRow index = e.first;
		result[res_index] = symbol_mapping_.GetSymbol(index);
		++res_index;
	}

	return result;
}

template <typename TRow>
std::string Module<TRow>::ElementaryConfabulation(float *max_excitation)
{
	std::unique_lock<std::mutex> module_lock(mutex_);
	return ElementaryConfabulation(1, max_excitation);
}

template <typename TRow>
std::string Module<TRow>::ElementaryConfabulation(int8_t band_cut_level, float *max_excitation)
{
	const std::set<std::pair<TRow, float>>& nz_excit = excitations_->GetNzElements();

	TRow max_index = 0;
	TRow n_inputs_max = 0;

	// try with all possible K, starting from the maximum one, until a solution is found
	std::unique_ptr<std::pair<TRow, float>> max_excit;
	do {
		const std::set<std::pair<TRow, float>>& min_K_excit = ExcitationsAbove(band_cut_level, nz_excit);
		max_excit = MaxExcitation(min_K_excit);

		if (max_excit != nullptr) {
			max_index = max_excit->first;
			n_inputs_max = kb_inputs_->GetElement(max_index);
		} else {
			--band_cut_level;
			continue;
		}
	} while ((max_excit == nullptr) && (band_cut_level > 0));

	ExcitationsToZero();

	if (max_excit == nullptr) {
		return Globals::kDummy;
	}

	// activate only the best word found
	excitations_->SetElement(max_index, max_excit->second);
	kb_inputs_->SetElement(max_index, n_inputs_max);
	*max_excitation = max_excit->second;

	return symbol_mapping_.GetSymbol(max_index);
}

template <typename TRow>
std::vector<std::string> Module<TRow>::PartialConfabulation(int8_t band_cut_level)
{
	std::vector<std::string> result;
	std::unique_ptr<std::vector<std::pair<TRow, float>>> expectations;

	if (band_cut_level < 0) {
		std::cout << "PartialConfabulation called with band_cut_level = " << band_cut_level << "\n" << std::flush;
		throw std::logic_error("PartialConfabulation called with negative band_cut_level");
	}

	const std::set<std::pair<TRow, float>>& nz_excit = excitations_->GetNzElements();
	const std::set<std::pair<TRow, float>>& band_cut_excit = ExcitationsAbove(band_cut_level, nz_excit);
	expectations.reset(new std::vector<std::pair<TRow, float>>(band_cut_excit.begin(), band_cut_excit.end()));

	// saving needed info from intermediate state
	KHashExcitationVector<TRow, uint16_t> kb_inputs_temp(*kb_inputs_);

	// cleanup intermediate state
	ExcitationsToZero();

	result.resize(expectations->size());

	// activate only symbols with excitations above threshold
	TRow res_index = 0;
	for (const std::pair<TRow, float>& e : *expectations) {
		TRow index = e.first;
		excitations_->SetElement(index, e.second);
		kb_inputs_->SetElement(index, kb_inputs_temp.GetElement(index));
		result[res_index] = symbol_mapping_.GetSymbol(index);
		++res_index;
	}

	return result;
}

template <typename TRow>
std::vector<std::string> Module<TRow>::TighteningPartialConfabulation(int8_t band_tighten_level)
{
	std::unique_lock<std::mutex> module_lock(mutex_);
	if (band_tighten_level >= 0) {
		current_tightening_level_ += band_tighten_level;
	} else {
		std::cout << "AdditivePartialConfabulation called with band_tighten_level = " << band_tighten_level << "\n" << std::flush;
		throw std::logic_error("AdditivePartialConfabulation called with negative band_tighten_level");
	}
	return PartialConfabulation(current_tightening_level_);
}

template <typename TRow>
void Module<TRow>::ExcitationsToZero()
{
	excitations_.reset(new KHashExcitationVector<TRow, float>(symbol_mapping_.Size()));
	kb_inputs_.reset(new KHashExcitationVector<TRow, uint16_t>(symbol_mapping_.Size()));
}

template <typename TRow>
void Module<TRow>::TighteningLevelToZero()
{
	current_tightening_level_ = 0;
}

template <typename TRow>
void Module<TRow>::Lock()
{
	mutex_.lock();
}

template <typename TRow>
void Module<TRow>::UnLock()
{
	mutex_.unlock();
}

template <typename TRow>
std::unique_ptr<std::pair<TRow, float>> Module<TRow>::MaxExcitation(const std::set<std::pair<TRow, float> > &nz_excitations)
{
	std::unique_ptr<std::pair<TRow, float>> result(nullptr);
	float max_value = 0;

	for (const std::pair<TRow, float>& e : nz_excitations) {
		if (result == nullptr) {
			result.reset(new std::pair<TRow, float>(e.first, e.second));
		} else if (e.second > max_value) {
			result->first = e.first;
			result->second = e.second;
			max_value = e.second;
		}
	}

	return result;
}

template <typename TRow>
std::set<std::pair<TRow, float>> Module<TRow>::ExcitationsAbove(int8_t K, const std::set<std::pair<TRow, float> > &nz_excitations)
{
	std::set<std::pair<TRow, float>> result;

	for (const std::pair<TRow, float>& e : nz_excitations) {
		if (kb_inputs_->GetElement(e.first) >= K) {
			result.insert(e);
		}
	}

	return result;
}
