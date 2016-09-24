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

#ifndef DOKLINKSMATRIX_H
#define DOKLINKSMATRIX_H

#include <map>
#include "DOKExcitationVector.hpp"
#include "IKnowledgeLinks.hpp"

template <typename T>
class DOKLinksMatrix : public IKnowledgeLinks<T>
{
public:
    DOKLinksMatrix(uint16_t num_rows, uint16_t num_cols);
    DOKLinksMatrix(IKnowledgeLinks<T>& base);

    DOKLinksMatrix(const DOKLinksMatrix& rhs) = delete;
    DOKLinksMatrix& operator=(const DOKLinksMatrix& rhs) = delete;
    DOKLinksMatrix(DOKLinksMatrix&& rhs) = delete;
    DOKLinksMatrix&& operator=(DOKLinksMatrix&& rhs) = delete;

    ~DOKLinksMatrix();

    virtual void SetElement(uint16_t r, uint16_t c, const T& value);
    virtual void SetElementQuick(uint16_t r, uint16_t c, const T& value);

    virtual T GetElement(uint16_t r, uint16_t c) const;
    virtual T GetElementQuick(uint16_t r, uint16_t c) const;

    virtual uint32_t get_num_rows() const { return num_rows_; }
    virtual uint32_t get_num_cols() const { return num_cols_; }

    virtual uint32_t GetNnz() const { return map_.size(); }

    virtual std::unique_ptr<IExcitationVector<T>> Multiply(const IExcitationVector<T>& vec) const;
    virtual std::set<std::pair<std::pair<uint16_t, uint16_t>, T> > GetNzElements() const;

private:
    const uint16_t num_rows_;
    const uint16_t num_cols_;

    std::map<std::pair<uint16_t, uint16_t>, T> map_;
};

template <typename T>
DOKLinksMatrix<T>::DOKLinksMatrix(uint16_t num_rows, uint16_t num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename T>
DOKLinksMatrix<T>::DOKLinksMatrix(IKnowledgeLinks<T> &base) : num_rows_(base.get_num_rows()), num_cols_(base.get_num_cols())
{
    for (const std::pair<std::pair<uint16_t, uint16_t>, T>& element : base.GetNzElements()) {
        map_.insert(element);
    }
}

template <typename T>
DOKLinksMatrix<T>::~DOKLinksMatrix()
{
    map_.clear();
}

template <typename T>
T DOKLinksMatrix<T>::GetElement(uint16_t r, uint16_t c) const
{
    IKnowledgeLinks<T>::CheckBounds(r, c);
    return GetElementQuick(r, c);
}

template <typename T>
T DOKLinksMatrix<T>::GetElementQuick(uint16_t r, uint16_t c) const
{
    T result;

    try {
        result = map_.at(std::make_pair(r, c));
    } catch (const std::out_of_range& oor) {
        result = 0.0;
    }

    return result;
}

template <typename T>
void DOKLinksMatrix<T>::SetElement(uint16_t r, uint16_t c, const T &value)
{
    IKnowledgeLinks<T>::CheckBounds(r, c);
    if (IsNearlyEqual(value, 0.0)) {
        typename std::map<std::pair<uint16_t, uint16_t>, T>::iterator it = map_.find(std::make_pair(r, c));
        if (it != map_.end()) {
            map_.erase(it);
        }
    } else {
        SetElementQuick(r, c, value);
    }
}

template <typename T>
void DOKLinksMatrix<T>::SetElementQuick(uint16_t r, uint16_t c, const T& value)
{
    map_[std::make_pair(r, c)] = value;
}

template <typename T>
std::unique_ptr<IExcitationVector<T>> DOKLinksMatrix<T>::Multiply(const IExcitationVector<T>& vec) const
{
    std::set<std::pair<uint32_t, T>> vec_elements = vec.GetNzElements();

    T link_strength = 0.0;
    std::unique_ptr<IExcitationVector<T>> result(new DOKExcitationVector<T>(num_rows_));

    for (const std::pair<uint32_t, T>& element : vec_elements) {
        uint32_t c = element.first;
        for (uint32_t r = 0; r < num_rows_; ++r) {
            link_strength = GetElementQuick(r, c);
            if (!IsNearlyEqual(link_strength, 0.0)) {
                result->SetElement(r, result->GetElementQuick(r) + link_strength * element.second);
            }
        }
    }

    return result;
}

template <typename T>
std::set<std::pair<std::pair<uint16_t, uint16_t>, T>> DOKLinksMatrix<T>::GetNzElements() const
{
    typename std::set<std::pair<std::pair<uint16_t, uint16_t>, T>> result;

    for (typename std::map<std::pair<uint16_t, uint16_t>, T>::const_iterator it = map_.begin(); it != map_.end(); ++it) {
        result.insert(*it);
    }

    return result;
}

#endif // DOKLINKSMATRIX_H
