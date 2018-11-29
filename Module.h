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
#include <sparse_structures/IExcitationVector.hpp>
#include <SymbolMapping.h>

class Module
{
public:
	Module(const SymbolMapping& symbol_mapping, size_t id);
	Module(const Module& rhs) = delete;
	Module& operator=(const Module& rhs) = delete;
	Module(Module&& rhs) = delete;
	Module&& operator=(Module&& rhs) = delete;

	void ActivateSymbol(const std::string& word, int8_t band_add_level);
	void AddExcitationVector(const IExcitationVector<uint16_t, float>& input);
	void ExcitationsToZero();
	void TighteningLevelToZero();
	void Lock();
	void UnLock();

	IExcitationVector<uint16_t, float>* GetExcitations() const { return excitations_.get(); }
	std::unique_ptr<IExcitationVector<uint16_t, float>> GetNormalizedExcitations() const;
	std::unique_ptr<IExcitationVector<uint16_t, float>> GetWhitenedExcitations() const;
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
	std::unique_ptr<IExcitationVector<uint16_t, float>> excitations_;
	std::unique_ptr<IExcitationVector<uint16_t, uint16_t>> kb_inputs_;

	std::unique_ptr<std::pair<uint16_t, float>> MaxExcitation(const std::set<std::pair<uint16_t, float> > &nz_excitations);
	std::set<std::pair<uint16_t, float>> ExcitationsAbove(int8_t K, const std::set<std::pair<uint16_t, float>>& nz_excitations);

	size_t current_tightening_level_;
};
