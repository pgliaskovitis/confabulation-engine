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

#include "Module.h"
#include "Globals.h"
#include "utils/Utils.h"
#include <sparse_structures/KHashExcitationVector.hpp>

Module::Module(const SymbolMapping &symbol_mapping, size_t id) :
	symbol_mapping_(symbol_mapping),
	id_(id),
	excitations_(new KHashExcitationVector<uint16_t, float>(symbol_mapping.Size())),
	kb_inputs_(new KHashExcitationVector<uint16_t, uint16_t>(symbol_mapping.Size())),
	current_tightening_level_(0)
{
	ExcitationsToZero();
}

void Module::ActivateSymbol(const std::string &word, int8_t band_add_level)
{
	try {
		uint16_t index = symbol_mapping_.IndexOf(word);
		excitations_->SetElement(index, band_add_level * Globals::kBandGap);
		kb_inputs_->SetElement(index, band_add_level);
	} catch (std::out_of_range&) {
		std::cout << "Word \"" << word << "\" could not be found in symbol mapping of module during activation" << "\n" << std::flush;
	}
}

void Module::AddExcitationVector(const IExcitationVector<uint16_t, float> &input)
{
	excitations_->Add(input);
	for (const std::pair<uint16_t, float>& e : input.GetNzElements()) {
		uint16_t i = e.first;
		kb_inputs_->SetElement(i, kb_inputs_->GetElement(i) + 1);
	}
}

std::unique_ptr<IExcitationVector<uint16_t, float>> Module::GetNormalizedExcitations() const
{
	std::unique_ptr<IExcitationVector<uint16_t, float>> normalized_excitations;
	normalized_excitations.reset(new KHashExcitationVector<uint16_t, float>(symbol_mapping_.Size()));

	for (const std::pair<uint16_t, float>& e : excitations_->GetNzElements()) {
		normalized_excitations->SetElement(e.first, e.second);
	}

	normalized_excitations->Normalize();

	return normalized_excitations;
}

std::unique_ptr<IExcitationVector<uint16_t, float>> Module::GetWhitenedExcitations() const
{
	std::unique_ptr<IExcitationVector<uint16_t, float>> whitened_excitations;
	whitened_excitations.reset(new KHashExcitationVector<uint16_t, float>(symbol_mapping_.Size()));

	for (const std::pair<uint16_t, float>& e : excitations_->GetNzElements()) {
		whitened_excitations->SetElement(e.first, e.second);
	}

	whitened_excitations->Whiten();

	return whitened_excitations;
}

std::vector<std::string> Module::GetExpectation() const
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
	std::unique_lock<std::mutex> module_lock(mutex_);
	return ElementaryConfabulation(1, max_excitation);
}

std::string Module::ElementaryConfabulation(int8_t band_cut_level, float *max_excitation)
{
	const std::set<std::pair<uint16_t, float>>& nz_excit = excitations_->GetNzElements();

	uint16_t max_index = 0;
	uint16_t n_inputs_max = 0;

	// try with all possible K, starting from the maximum one, until a solution is found
	std::unique_ptr<std::pair<uint16_t, float>> max_excit;
	do {
		const std::set<std::pair<uint16_t, float>>& min_K_excit = ExcitationsAbove(band_cut_level, nz_excit);
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

std::vector<std::string> Module::PartialConfabulation(int8_t band_cut_level)
{
	std::vector<std::string> result;
	std::unique_ptr<std::vector<std::pair<uint16_t, float>>> expectations;

	if (band_cut_level < 0) {
		std::cout << "PartialConfabulation called with band_cut_level = " << band_cut_level << "\n" << std::flush;
		throw std::logic_error("PartialConfabulation called with negative band_cut_level");
	}

	const std::set<std::pair<uint16_t, float>>& nz_excit = excitations_->GetNzElements();
	const std::set<std::pair<uint16_t, float>>& band_cut_excit = ExcitationsAbove(band_cut_level, nz_excit);
	expectations.reset(new std::vector<std::pair<uint16_t, float>>(band_cut_excit.begin(), band_cut_excit.end()));

	// saving needed info from intermediate state
	KHashExcitationVector<uint16_t, uint16_t> kb_inputs_temp(*kb_inputs_);

	// cleanup intermediate state
	ExcitationsToZero();

	result.resize(expectations->size());

	// activate only symbols with excitations above threshold
	uint16_t res_index = 0;
	for (const std::pair<uint16_t, float>& e : *expectations) {
		uint16_t index = e.first;
		excitations_->SetElement(index, e.second);
		kb_inputs_->SetElement(index, kb_inputs_temp.GetElement(index));
		result[res_index] = symbol_mapping_.GetSymbol(index);
		++res_index;
	}

	return result;
}

std::vector<std::string> Module::TighteningPartialConfabulation(int8_t band_tighten_level)
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

void Module::ExcitationsToZero()
{
	excitations_.reset(new KHashExcitationVector<uint16_t, float>(symbol_mapping_.Size()));
	kb_inputs_.reset(new KHashExcitationVector<uint16_t, uint16_t>(symbol_mapping_.Size()));
}

void Module::TighteningLevelToZero()
{
	current_tightening_level_ = 0;
}

void Module::Lock()
{
	mutex_.lock();
}

void Module::UnLock()
{
	mutex_.unlock();
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
