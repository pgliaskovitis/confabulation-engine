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

#include <vector>

#include "DOKExcitationVector.hpp"
#include "IKnowledgeLinks.hpp"

template <typename TRow, typename TCol, typename T>
class CSRLinksMatrix : public IKnowledgeLinks<TRow, TCol, T>
{
public:
	CSRLinksMatrix(TRow num_rows, TCol num_cols);
	CSRLinksMatrix(IKnowledgeLinks<TRow, TCol, T>& base);

	CSRLinksMatrix(const CSRLinksMatrix& rhs) = delete;
	CSRLinksMatrix& operator=(const CSRLinksMatrix& rhs) = delete;
	CSRLinksMatrix(CSRLinksMatrix&& rhs) = delete;
	CSRLinksMatrix&& operator=(CSRLinksMatrix&& rhs) = delete;

	~CSRLinksMatrix();

	virtual void SetElement(TRow r, TCol c, const T& value) override final { (void)r; (void)c; (void)value; /*not supported*/ }
	virtual void SetElementQuick(TRow r, TCol c, const T& value) override final { (void)r; (void)c; (void)value; /*not supported*/ }

	virtual T GetElement(TRow r, TCol c) const override final;
	virtual T GetElementQuick(TRow r, TCol c) const override final;

	virtual TRow GetNumRows() const override final { return num_rows_; }
	virtual TCol GetNumCols() const override final { return num_cols_; }

	virtual uint32_t GetNnz() const override final { return a_.size(); }

	virtual std::unique_ptr<IExcitationVector<TRow, T>> Multiply(const IExcitationVector<TCol, T>& vec) const override final;
	virtual std::set<std::pair<std::pair<TRow, TCol>, T>> GetNzElements() const override final;

private:
	const TRow num_rows_;
	const TCol num_cols_;

	std::vector<T> a_;
	std::vector<uint32_t> ia_;
	std::vector<uint32_t> ja_;
};

template <typename TRow, typename TCol, typename T>
CSRLinksMatrix<TRow, TCol, T>::CSRLinksMatrix(TRow num_rows, TCol num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename TRow, typename TCol, typename T>
CSRLinksMatrix<TRow, TCol, T>::~CSRLinksMatrix()
{
	a_.clear();
	ia_.clear();
	ja_.clear();
}

template <typename TRow, typename TCol, typename T>
CSRLinksMatrix<TRow, TCol, T>::CSRLinksMatrix(IKnowledgeLinks<TRow, TCol, T> &base) : num_rows_(base.GetNumRows()), num_cols_(base.GetNumCols())
{
	a_.resize(base.GetNnz());
	ja_.resize(base.GetNnz());

	uint32_t index_a = 0;

	// data begins at 0 in A
	ia_.push_back(index_a);
	uint32_t row = 0;

	for (const std::pair<std::pair<TRow, TCol>, T>& element : base.GetNzElements()) {
		// advance in lines until the row of element
		while (row != element.first.first) {
			ia_.push_back(index_a);
			++row;
		}

		// insert element's value and column in A and JA
		a_[index_a] = element.second;
		ja_[index_a] = element.first.second;
		++index_a;
	}

	// end of non-zero elements: fill remaining IA indexes
	for (; row < num_rows_; ++row) {
		ia_.push_back(index_a);
	}
}

template <typename TRow, typename TCol, typename T>
T CSRLinksMatrix<TRow, TCol, T>::GetElement(TRow r, TCol c) const
{
	IKnowledgeLinks<TRow, TCol, T>::CheckBounds(r, c);
	return GetElementQuick(r, c);
}

template <typename TRow, typename TCol, typename T>
T CSRLinksMatrix<TRow, TCol, T>::GetElementQuick(TRow r, TCol c) const
{
	auto begin_it = ja_.begin() + ia_[r];
	auto end_it = ja_.begin() + ia_[r + 1];

	bool found_element = false;
	const uint32_t index = BinarySearch(begin_it, end_it, c, found_element);

	if (found_element) {
		return a_[index];
	} else {
		return 0;
	}
}

template <typename TRow, typename TCol, typename T>
std::unique_ptr<IExcitationVector<TRow, T>> CSRLinksMatrix<TRow, TCol, T>::Multiply(const IExcitationVector<TCol, T>& vec) const
{
	std::unique_ptr<IExcitationVector<TRow, T>> result = std::make_unique<DOKExcitationVector<TRow, T>>(num_rows_);
	T row_sum;

	for (TRow r = 0; r < num_rows_; ++r) {
		row_sum = 0;
		for (uint32_t i = ia_[r]; i < ia_[r + 1]; ++i) {
			row_sum += a_[i] * vec.GetElement(ja_[i]);
		}
		result->SetElement(r, row_sum);
	}

	return result;
}

template <typename TRow, typename TCol, typename T>
std::set<std::pair<std::pair<TRow, TCol>, T>> CSRLinksMatrix<TRow, TCol, T>::GetNzElements() const
{
	typename std::set<std::pair<std::pair<TRow, TCol>, T>> result;

	for (TRow r = 0; r < num_rows_; ++r) {
		for (uint32_t i = ia_[r]; i < ia_[r + 1]; ++i) {
			result.insert(std::make_pair(std::make_pair(r, ja_[i]), a_[i]));
		}
	}

	return result;
}
