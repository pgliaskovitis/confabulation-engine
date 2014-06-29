#ifndef DOKEXCITATIONVECTORIMPL_H
#define DOKEXCITATIONVECTORIMPL_H

#include "Utils.h"
#include "DOKExcitationVector.hpp"

template <typename T>
DOKExcitationVector<T>::DOKExcitationVector(size_t n_rows) : num_rows_(num_rows)
{}

template <typename T>
DOKExcitationVector<T>::DOKExcitationVector(IExcitationVector<T>& base) : num_rows_(base.get_num_rows())
{
    for (std::pair<size_t, T> element: base.GetNzElements())
        map_[element.first] = element.second;
}

template <typename T>
void DOKExcitationVector<T>::SetElement(size_t r, T& value)
{
    CheckBounds(r);
    if (IsNearlyEqual(value, 0)) {
        std::unordered_map<size_t, T>::iterator it = map_.find(value);
        if (it != map_.end())
            map_.erase(it);
    } else
        SetElementQuick(r, value);
}

template <typename T>
void DOKExcitationVector<T>::SetElementQuick(size_t r, T& value)
{
    // no bounds checking
    // no zero value checking
    map_[r] = value;
}

template <typename T>
const T& DOKExcitationVector<T>::GetElement(size_t r) const
{
    CheckBounds(r);
    return GetElementQuick(r);
}

template <typename T>
const T& DOKExcitationVector<T>::GetElementQuick(size_t r) const
{
    // no bounds checking
    std::unordered_map<size_t, T>::iterator = map_.find(value);
    if (it != map_.end())
        return map_[r];
    else
        return 0;
}

template <typename T>
std::set<std::pair<size_t, T>> DOKExcitationVector<T>::GetNzElements()
{
    std::unordered_set<std::pair<size_t, T>> result;
    for (std::unordered_map<size_t, T>::iterator it = map_.begin(); it != map_.end(); ++it)
        result.insert(*it);

    return result;
}

#endif // DOKEXCITATIONVECTORIMPL_H
