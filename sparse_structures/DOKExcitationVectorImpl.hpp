#ifndef DOKEXCITATIONVECTORIMPL_H
#define DOKEXCITATIONVECTORIMPL_H

#include "Utils.h"
#include "DOKExcitationVector.hpp"

template <typename T>
DOKExcitationVector<T>::DOKExcitationVector(const size_t num_rows) : num_rows_(num_rows)
{}

template <typename T>
DOKExcitationVector<T>::DOKExcitationVector(const IExcitationVector<T> &base) : num_rows_(base.get_num_rows())
{
    for (const std::pair<size_t, T>& element: base.GetNzElements())
        map_[element.first] = element.second;
}

template <typename T>
void DOKExcitationVector<T>::SetElement(const size_t r, const T& value)
{
    IExcitationVector<T>::CheckBounds(r);
    if (IsNearlyEqual(value, 0)) {
        typename std::unordered_map<size_t, T>::iterator it = map_.find(value);
        if (it != map_.end())
            map_.erase(it);
    } else
        SetElementQuick(r, value);
}

template <typename T>
void DOKExcitationVector<T>::SetElementQuick(const size_t r, const T &value)
{
    map_[r] = value;
}

template <typename T>
const T& DOKExcitationVector<T>::GetElement(const size_t r) const
{
    return map_.at(r);
}

template <typename T>
const T& DOKExcitationVector<T>::GetElementQuick(const size_t r) const
{
    return map_.at(r);
}

template <typename T>
void DOKExcitationVector<T>::Add(const IExcitationVector<T>& other) {
    for (const std::pair<size_t, T>& element : other.GetNzElements()) {
        SetElement(element.first, element.second + GetElement(element.first));
    }
}

template <typename T>
std::set<std::pair<size_t, T>> DOKExcitationVector<T>::GetNzElements() const
{
    typename std::set<std::pair<size_t, T>> result;
    for (typename std::unordered_map<size_t, T>::const_iterator it = map_.begin(); it != map_.end(); ++it)
        result.insert(*it);

    return result;
}

#endif // DOKEXCITATIONVECTORIMPL_H
