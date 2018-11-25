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

#include <unordered_map>
#include "DOKExcitationVector.hpp"
#include "IKnowledgeLinks.hpp"

template <typename TRow, typename TCol, typename T>
class DOKLinksMatrix : public IKnowledgeLinks<TRow, TCol, T>
{
public:
	DOKLinksMatrix(TRow num_rows, TCol num_cols);
	DOKLinksMatrix(IKnowledgeLinks<TRow, TCol, T>& base);

	DOKLinksMatrix(const DOKLinksMatrix& rhs) = delete;
	DOKLinksMatrix& operator=(const DOKLinksMatrix& rhs) = delete;
	DOKLinksMatrix(DOKLinksMatrix&& rhs) = delete;
	DOKLinksMatrix&& operator=(DOKLinksMatrix&& rhs) = delete;

	~DOKLinksMatrix();

	virtual void SetElement(TRow r, TCol c, const T& value);
	virtual void SetElementQuick(TRow r, TCol c, const T& value);

	virtual T GetElement(TRow r, TCol c) const;
	virtual T GetElementQuick(TRow r, TCol c) const;

	virtual TRow get_num_rows() const { return num_rows_; }
	virtual TCol get_num_cols() const { return num_cols_; }

	virtual uint32_t GetNnz() const { return map_.size(); }

	virtual std::unique_ptr<IExcitationVector<TRow, T>> Multiply(const IExcitationVector<TCol, T>& vec) const;
	virtual std::set<std::pair<std::pair<TRow, TCol>, T> > GetNzElements() const;

private:
	const TRow num_rows_;
	const TCol num_cols_;

	std::unordered_map<std::pair<TRow, TCol>, T, PairHash, PairEquals> map_;
};

template <typename TRow, typename TCol, typename T>
DOKLinksMatrix<TRow, TCol, T>::DOKLinksMatrix(TRow num_rows, TCol num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename TRow, typename TCol, typename T>
DOKLinksMatrix<TRow, TCol, T>::DOKLinksMatrix(IKnowledgeLinks<TRow, TCol, T> &base) : num_rows_(base.get_num_rows()), num_cols_(base.get_num_cols())
{
	for (const std::pair<std::pair<TRow, TCol>, T>& element : base.GetNzElements()) {
		map_.insert(element);
	}
}

template <typename TRow, typename TCol, typename T>
DOKLinksMatrix<TRow, TCol, T>::~DOKLinksMatrix()
{
	map_.clear();
}

template <typename TRow, typename TCol, typename T>
T DOKLinksMatrix<TRow, TCol, T>::GetElement(TRow r, TCol c) const
{
	IKnowledgeLinks<TRow, TCol, T>::CheckBounds(r, c);
	return GetElementQuick(r, c);
}

template <typename TRow, typename TCol, typename T>
T DOKLinksMatrix<TRow, TCol, T>::GetElementQuick(TRow r, TCol c) const
{
	T result;

	try {
		result = map_.at(std::make_pair(r, c));
	} catch (const std::out_of_range& oor) {
		result = 0.0;
	}

	return result;
}

template <typename TRow, typename TCol, typename T>
void DOKLinksMatrix<TRow, TCol, T>::SetElement(TRow r, TCol c, const T &value)
{
	IKnowledgeLinks<TRow, TCol, T>::CheckBounds(r, c);
	if (IsNearlyEqual(value, 0.0)) {
		typename std::unordered_map<std::pair<TRow, TCol>, T, PairHash, PairEquals>::iterator it = map_.find(std::make_pair(r, c));
		if (it != map_.end()) {
			map_.erase(it);
		}
	} else {
		SetElementQuick(r, c, value);
	}
}

template <typename TRow, typename TCol, typename T>
void DOKLinksMatrix<TRow, TCol, T>::SetElementQuick(TRow r, TCol c, const T& value)
{
	map_[std::make_pair(r, c)] = value;
}

template <typename TRow, typename TCol, typename T>
std::unique_ptr<IExcitationVector<TRow, T>> DOKLinksMatrix<TRow, TCol, T>::Multiply(const IExcitationVector<TCol, T>& vec) const
{
	std::set<std::pair<TCol, T>> vec_elements = vec.GetNzElements();

	T link_strength = 0.0;
	std::unique_ptr<IExcitationVector<TRow, T>> result(new DOKExcitationVector<TRow, T>(num_rows_));

	for (const std::pair<TCol, T>& element : vec_elements) {
		uint32_t c = element.first;
		for (TRow r = 0; r < num_rows_; ++r) {
			link_strength = GetElementQuick(r, c);
			if (!IsNearlyEqual(link_strength, 0.0)) {
				result->SetElement(r, result->GetElementQuick(r) + link_strength * element.second);
			}
		}
	}

	return result;
}

template <typename TRow, typename TCol, typename T>
std::set<std::pair<std::pair<TRow, TCol>, T>> DOKLinksMatrix<TRow, TCol, T>::GetNzElements() const
{
	typename std::set<std::pair<std::pair<TRow, TCol>, T>> result;

	for (typename std::unordered_map<std::pair<TRow, TCol>, T, PairHash, PairEquals>::const_iterator it = map_.begin(); it != map_.end(); ++it) {
		result.insert(*it);
	}

	return result;
}
