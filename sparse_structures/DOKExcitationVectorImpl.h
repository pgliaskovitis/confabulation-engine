#ifndef DOKEXCITATIONVECTORIMPL_H
#define DOKEXCITATIONVECTORIMPL_H

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
    if (CheckBounds(r))
        SetElementQuick(r, value);
}

template <typename T>
void DOKExcitationVector<T>::SetElementQuick(size_t r, T& value)
{
    map_[r] = value;
}

template <typename T>
const T& DOKExcitationVector<T>::GetElement(size_t r) const
{
    if (CheckBounds(r))
        return GetElementQuick(r);
}

template <typename T>
const T& DOKExcitationVector<T>::GetElementQuick(size_t r) const
{
    return map_[r];
}

template <typename T>
std::set<std::pair<size_t, T>> DOKExcitationVector<T>::GetNzElements()
{
    std::set<std::pair<size_t, T>> result;
}

#endif // DOKEXCITATIONVECTORIMPL_H
