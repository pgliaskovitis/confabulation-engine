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

#ifndef CSRLINKSMATRIX_H
#define CSRLINKSMATRIX_H

#include <vector>

#include "DOKExcitationVector.hpp"
#include "IKnowledgeLinks.hpp"

template <typename T>
class CSRLinksMatrix : public IKnowledgeLinks<T>
{
public:
    CSRLinksMatrix(uint32_t num_rows, uint32_t num_cols);
    CSRLinksMatrix(IKnowledgeLinks<T>& base);

    CSRLinksMatrix(const CSRLinksMatrix& rhs) = delete;
    CSRLinksMatrix& operator=(const CSRLinksMatrix& rhs) = delete;
    CSRLinksMatrix(CSRLinksMatrix&& rhs) = delete;
    CSRLinksMatrix&& operator=(CSRLinksMatrix&& rhs) = delete;

    ~CSRLinksMatrix();

    virtual void SetElement(uint32_t r, uint32_t c, const T& value) { (void)r; (void)c; (void)value; /*not supported*/ }
    virtual void SetElementQuick(uint32_t r, uint32_t c, const T& value) { (void)r; (void)c; (void)value; /*not supported*/ }

    virtual T GetElement(uint32_t r, uint32_t c) const;
    virtual T GetElementQuick(uint32_t r, uint32_t c) const;

    virtual uint32_t get_num_rows() const { return num_rows_; }
    virtual uint32_t get_num_cols() const { return num_cols_; }

    virtual uint32_t GetNnz() const { return a_.size(); }

    virtual std::unique_ptr<IExcitationVector<T>> Multiply(const IExcitationVector<T>& vec) const;
    virtual std::set<std::pair<std::pair<uint16_t, uint16_t>, T>> GetNzElements() const;

private:
    const uint16_t num_rows_;
    const uint16_t num_cols_;

    std::vector<T> a_;
    std::vector<uint32_t> ia_;
    std::vector<uint32_t> ja_;
};

template <typename T>
CSRLinksMatrix<T>::CSRLinksMatrix(uint32_t num_rows, uint32_t num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename T>
CSRLinksMatrix<T>::~CSRLinksMatrix()
{
    a_.clear();
    ia_.clear();
    ja_.clear();
}

template <typename T>
CSRLinksMatrix<T>::CSRLinksMatrix(IKnowledgeLinks<T> &base) : num_rows_(base.get_num_rows()), num_cols_(base.get_num_cols())
{
    a_.resize(base.GetNnz());
    ja_.resize(base.GetNnz());

    uint32_t index_a = 0;

    // data begins at 0 in A
    ia_.push_back(index_a);
    uint32_t row = 0;

    for (const std::pair<std::pair<uint32_t, uint32_t>, T>& element : base.GetNzElements()) {
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

template <typename T>
T CSRLinksMatrix<T>::GetElement(uint32_t r, uint32_t c) const
{
    IKnowledgeLinks<T>::CheckBounds(r, c);
    return GetElementQuick(r, c);
}

template <typename T>
T CSRLinksMatrix<T>::GetElementQuick(uint32_t r, uint32_t c) const
{
    std::vector<uint32_t>::const_iterator begin_it = ja_.begin() + ia_[r];
    std::vector<uint32_t>::const_iterator end_it = ja_.begin() + ia_[r + 1];

    bool found_element = false;
    const uint32_t index = BinarySearch(begin_it, end_it, c, found_element);

    if (found_element) {
        return a_[index];
    } else {
        return 0;
    }
}

template <typename T>
std::unique_ptr<IExcitationVector<T>> CSRLinksMatrix<T>::Multiply(const IExcitationVector<T>& vec) const
{
    std::unique_ptr<IExcitationVector<T>> result(new DOKExcitationVector<T>(num_rows_));
    T row_sum;

    for (uint32_t r = 0; r < num_rows_; ++r) {
        row_sum = 0;
        for (uint32_t i = ia_[r]; i < ia_[r + 1]; ++i) {
            row_sum += a_[i] * vec.GetElement(ja_[i]);
        }
        result->SetElement(r, row_sum);
    }

    return result;
}

template <typename T>
std::set<std::pair<std::pair<uint16_t, uint16_t>, T>> CSRLinksMatrix<T>::GetNzElements() const
{
    typename std::set<std::pair<std::pair<uint16_t, uint16_t>, T>> result;

    for (uint32_t r = 0; r < num_rows_; ++r) {
        for (uint32_t i = ia_[r]; i < ia_[r + 1]; ++i) {
            result.insert(std::make_pair(std::make_pair(r, ja_[i]), a_[i]));
        }
    }

    return result;
}

#endif // CSRLINKSMATRIX_H
