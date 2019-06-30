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

#include <google/sparse_hash_map>
#include "IKnowledgeLinks.hpp"

template <typename TRow, typename TCol, typename T>
class SparseHashLinksMatrix : public IKnowledgeLinks<TRow, TCol, T>
{
public:
	SparseHashLinksMatrix(TRow num_rows, TCol num_cols);
	SparseHashLinksMatrix(IKnowledgeLinks<TRow, TCol, T>& base);

	SparseHashLinksMatrix(const SparseHashLinksMatrix& rhs) = delete;
	SparseHashLinksMatrix& operator=(const SparseHashLinksMatrix& rhs) = delete;
	SparseHashLinksMatrix(SparseHashLinksMatrix&& rhs) = delete;
	SparseHashLinksMatrix&& operator=(SparseHashLinksMatrix&& rhs) = delete;

	~SparseHashLinksMatrix() override final;

	virtual void SetElement(TRow r, TCol c, const T& value) override final;
	virtual void SetElementQuick(TRow r, TCol c, const T& value) override final;

	virtual T GetElement(TRow r, TCol c) const override final;
	virtual T GetElementQuick(TRow r, TCol c) const override final;

	virtual TRow GetNumRows() const override final { return num_rows_; }
	virtual TCol GetNumCols() const override final { return num_cols_; }

	virtual uint32_t GetNnz() const override final { return map_.size(); }

	virtual std::unique_ptr<IExcitationVector<TRow, T>> Multiply(const IExcitationVector<TCol, T>& vec) const override final;
	virtual std::set<std::pair<std::pair<TRow, TCol>, T> > GetNzElements() const override final;

private:
	const TRow num_rows_;
	const TCol num_cols_;

	google::sparse_hash_map<std::pair<TRow, TCol>, T, PairHash, PairEquals> map_;
};

template <typename TRow, typename TCol, typename T>
SparseHashLinksMatrix<TRow, TCol, T>::SparseHashLinksMatrix(TRow num_rows, TCol num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename TRow, typename TCol, typename T>
SparseHashLinksMatrix<TRow, TCol, T>::SparseHashLinksMatrix(IKnowledgeLinks<TRow, TCol, T> &base) : num_rows_(base.GetNumRows()), num_cols_(base.GetNumCols())
{
	for (const std::pair<std::pair<TRow, TCol>, T>& element : base.GetNzElements()) {
		map_.insert(element);
	}
}

template <typename TRow, typename TCol, typename T>
SparseHashLinksMatrix<TRow, TCol, T>::~SparseHashLinksMatrix()
{
	map_.clear();
}

template <typename TRow, typename TCol, typename T>
T SparseHashLinksMatrix<TRow, TCol, T>::GetElement(TRow r, TCol c) const
{
	IKnowledgeLinks<TRow, TCol, T>::CheckBounds(r, c);
	return GetElementQuick(r, c);
}

template <typename TRow, typename TCol, typename T>
T SparseHashLinksMatrix<TRow, TCol, T>::GetElementQuick(TRow r, TCol c) const
{
	T result;

	typename google::sparse_hash_map<std::pair<TRow, TCol>, T, PairHash, PairEquals>::const_iterator it = map_.find(std::make_pair(r, c));
	if (it != map_.end()) {
		result = it->second;
	} else {
		result = 0.0;
	}

	return result;
}

template <typename TRow, typename TCol, typename T>
void SparseHashLinksMatrix<TRow, TCol, T>::SetElement(TRow r, TCol c, const T &value)
{
	IKnowledgeLinks<TRow, TCol, T>::CheckBounds(r, c);
	if (IsNearlyEqual(value, 0.0)) {
		typename google::sparse_hash_map<std::pair<TRow, TCol>, T, PairHash, PairEquals>::iterator it = map_.find(std::make_pair(r, c));
		if (it != map_.end()) {
			map_.erase(it);
		}
	} else {
		SetElementQuick(r, c, value);
	}
}

template <typename TRow, typename TCol, typename T>
void SparseHashLinksMatrix<TRow, TCol, T>::SetElementQuick(TRow r, TCol c, const T& value)
{
	map_[std::make_pair(r, c)] = value;
}

template <typename TRow, typename TCol, typename T>
std::unique_ptr<IExcitationVector<TRow, T>> SparseHashLinksMatrix<TRow, TCol, T>::Multiply(const IExcitationVector<TCol, T>& vec) const
{
	std::set<std::pair<TCol, T>> vec_elements = vec.GetNzElements();

	T link_strength = 0.0;
	std::unique_ptr<IExcitationVector<TRow, T>> result = std::make_unique<DOKExcitationVector<TRow, T>>(num_rows_);

	for (const std::pair<TCol, T>& element : vec_elements) {
		uint32_t c = element.first;
		for (uint16_t r = 0; r < num_rows_; ++r) {
			link_strength = GetElementQuick(r, c);
			if (!IsNearlyEqual(link_strength, 0.0)) {
				result->SetElement(r, result->GetElementQuick(r) + link_strength * element.second);
			}
		}
	}

	return result;
}

template <typename TRow, typename TCol, typename T>
std::set<std::pair<std::pair<TRow, TCol>, T>> SparseHashLinksMatrix<TRow, TCol, T>::GetNzElements() const
{
	typename std::set<std::pair<std::pair<TRow, TCol>, T>> result;

	for (typename google::sparse_hash_map<std::pair<TRow, TCol>, T, PairHash, PairEquals>::const_iterator it = map_.begin(); it != map_.end(); ++it) {
		result.insert(*it);
	}

	return result;
}
