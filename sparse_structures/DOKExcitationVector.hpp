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
#include <iostream>

#include "utils/Utils.h"
#include "IExcitationVector.hpp"

template <typename TRow, typename T>
class DOKExcitationVector : public IExcitationVector<TRow, T>
{
public:
	DOKExcitationVector(const TRow num_rows);
	DOKExcitationVector(const IExcitationVector<TRow, T>& base);

	DOKExcitationVector(const DOKExcitationVector& rhs) = delete;
	DOKExcitationVector& operator=(const DOKExcitationVector& rhs) = delete;
	DOKExcitationVector(DOKExcitationVector&& rhs) = delete;
	DOKExcitationVector&& operator=(DOKExcitationVector&& rhs) = delete;

	~DOKExcitationVector();

	virtual void SetElement(const TRow r, const T& value);
	virtual void SetElementQuick(const TRow r, const T& value);

	virtual T GetElement(const TRow r) const;
	virtual T GetElementQuick(const TRow r) const;

	virtual TRow get_num_rows() const { return num_rows_; }

	virtual TRow GetNnz() const { return map_.size(); }

	virtual void Add(const IExcitationVector<TRow, T>& other);
	virtual void Normalize();
	virtual void Whiten();
	virtual std::set<std::pair<TRow, T>> GetNzElements() const;

private:
	const TRow num_rows_;
	std::unordered_map<TRow, T> map_;
};

template <typename TRow, typename T>
DOKExcitationVector<TRow, T>::DOKExcitationVector(const TRow num_rows) : num_rows_(num_rows)
{}

template <typename TRow, typename T>
DOKExcitationVector<TRow, T>::DOKExcitationVector(const IExcitationVector<TRow, T> &base) : num_rows_(base.get_num_rows())
{
	for (const std::pair<TRow, T>& element : base.GetNzElements()) {
		map_[element.first] = element.second;
	}
}

template <typename TRow, typename T>
DOKExcitationVector<TRow, T>::~DOKExcitationVector()
{
	map_.clear();
}

template <typename TRow, typename T>
void DOKExcitationVector<TRow, T>::SetElement(const TRow r, const T& value)
{
	IExcitationVector<TRow, T>::CheckBounds(r);
	if (IsNearlyEqual(value, 0.0)) {
		typename std::unordered_map<TRow, T>::iterator it = map_.find(r);
		if (it != map_.end()) {
			map_.erase(it);
		}
	} else {
		SetElementQuick(r, value);
	}
}

template <typename TRow, typename T>
void DOKExcitationVector<TRow, T>::SetElementQuick(const TRow r, const T &value)
{
	map_[r] = value;
}

template <typename TRow, typename T>
T DOKExcitationVector<TRow, T>::GetElement(const TRow r) const
{
	IExcitationVector<TRow, T>::CheckBounds(r);
	return GetElementQuick(r);
}

template <typename TRow, typename T>
T DOKExcitationVector<TRow, T>::GetElementQuick(const TRow r) const
{
	T result;

	try {
		result = map_.at(r);
	} catch (const std::out_of_range& oor) {
		result = 0.0f;
	}

	return result;
}

template <typename TRow, typename T>
void DOKExcitationVector<TRow, T>::Add(const IExcitationVector<TRow, T>& other)
{
	for (const std::pair<TRow, T>& element : other.GetNzElements()) {
		SetElement(element.first, PositiveClip(element.second + GetElement(element.first)));
	}
}

template <typename TRow, typename T>
void DOKExcitationVector<TRow, T>::Normalize()
{
	double sum = 0.0;

	for (typename std::unordered_map<TRow, T>::const_iterator it = map_.begin(); it != map_.end(); ++it) {
		sum += it->second;
	}

	if (sum > 0.0) {
		for (typename std::unordered_map<TRow, T>::iterator it = map_.begin(); it != map_.end(); ++it) {
			it->second /= sum;
		}
	}
}

template <typename TRow, typename T>
void DOKExcitationVector<TRow, T>::Whiten()
{
	double sum = 0.0;
	double squared_sum = 0.0;
	double mean = 0.0;
	double variance = 0.0;
	double std = 0.0;

	for (typename std::unordered_map<TRow, T>::const_iterator it = map_.begin(); it != map_.end(); ++it) {
		sum += it->second;
		squared_sum += it->second * it->second;
	}

	mean = sum / map_.size();
	variance = squared_sum / map_.size() - mean * mean;
	std = sqrt(variance) + 0.000001;

	for (typename std::unordered_map<TRow, T>::iterator it = map_.begin(); it != map_.end(); ++it) {
		it->second = (it->second - mean) / std;
	}
}

template <typename TRow, typename T>
std::set<std::pair<TRow, T>> DOKExcitationVector<TRow, T>::GetNzElements() const
{
	typename std::set<std::pair<TRow, T>> result;

	for (typename std::unordered_map<TRow, T>::const_iterator it = map_.begin(); it != map_.end(); ++it) {
		result.insert(*it);
	}

	return result;
}
