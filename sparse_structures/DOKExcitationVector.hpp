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

#ifndef DOKEXCITATIONVECTOR_H
#define DOKEXCITATIONVECTOR_H

#include <unordered_map>
#include <iostream>

#include "utils/Utils.h"
#include "IExcitationVector.hpp"

template <typename T>
class DOKExcitationVector : public IExcitationVector<T>
{
public:
    DOKExcitationVector(const uint32_t num_rows);
    DOKExcitationVector(const IExcitationVector<T>& base);

    DOKExcitationVector(const DOKExcitationVector& rhs) = delete;
    DOKExcitationVector& operator=(const DOKExcitationVector& rhs) = delete;
    DOKExcitationVector(DOKExcitationVector&& rhs) = delete;
    DOKExcitationVector&& operator=(DOKExcitationVector&& rhs) = delete;

    virtual void SetElement(const uint32_t r, const T& value);
    virtual void SetElementQuick(const uint32_t r, const T& value);

    virtual T GetElement(const uint32_t r) const;
    virtual T GetElementQuick(const uint32_t r) const;

    virtual uint32_t get_num_rows() const { return num_rows_; }

    virtual uint32_t GetNnz() const { return map_.size(); }

    virtual void Add(const IExcitationVector<T>& other);
    virtual std::set<std::pair<uint32_t, T>> GetNzElements() const;

private:
    const uint32_t num_rows_;

    std::unordered_map<uint32_t, T> map_;
};

template <typename T>
DOKExcitationVector<T>::DOKExcitationVector(const uint32_t num_rows) : num_rows_(num_rows)
{}

template <typename T>
DOKExcitationVector<T>::DOKExcitationVector(const IExcitationVector<T> &base) : num_rows_(base.get_num_rows())
{
    for (const std::pair<uint32_t, T>& element : base.GetNzElements()) {
        map_[element.first] = element.second;
    }
}

template <typename T>
void DOKExcitationVector<T>::SetElement(const uint32_t r, const T& value)
{
    IExcitationVector<T>::CheckBounds(r);
    if (IsNearlyEqual(value, 0.0)) {
        typename std::unordered_map<uint32_t, T>::iterator it = map_.find(r);
        if (it != map_.end()) {
            map_.erase(it);
        }
    } else {
        SetElementQuick(r, value);
    }
}

template <typename T>
void DOKExcitationVector<T>::SetElementQuick(const uint32_t r, const T &value)
{
    map_[r] = value;
}

template <typename T>
T DOKExcitationVector<T>::GetElement(const uint32_t r) const
{
    IExcitationVector<T>::CheckBounds(r);
    return GetElementQuick(r);
}

template <typename T>
T DOKExcitationVector<T>::GetElementQuick(const uint32_t r) const
{
    T result;

    try {
        result = map_.at(r);
    } catch (const std::out_of_range& oor) {
        result = 0.0;
    }

    return result;
}

template <typename T>
void DOKExcitationVector<T>::Add(const IExcitationVector<T>& other) {
    for (const std::pair<uint32_t, T>& element : other.GetNzElements()) {
        SetElement(element.first, PositiveClip(element.second + GetElement(element.first)));
    }
}

template <typename T>
std::set<std::pair<uint32_t, T>> DOKExcitationVector<T>::GetNzElements() const
{
    typename std::set<std::pair<uint32_t, T>> result;
    for (typename std::unordered_map<uint32_t, T>::const_iterator it = map_.begin(); it != map_.end(); ++it) {
        result.insert(*it);
    }

    return result;
}

#endif // DOKEXCITATIONVECTOR_H
