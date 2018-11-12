/*
 * Copyright 2018 Periklis G. Liaskovitis
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

#include <iostream>

#include "klib/khash.h"
#include "utils/Utils.h"
#include "IExcitationVector.hpp"

KHASH_INIT(vector_float, uint16_t, float, 0, kh_int_hash_func, kh_int_hash_equal)

template <typename T>
class KHashExcitationVector : public IExcitationVector<T>
{
public:
	KHashExcitationVector(const uint16_t num_rows);
	KHashExcitationVector(const IExcitationVector<T>& base);

	KHashExcitationVector(const KHashExcitationVector& rhs) = delete;
	KHashExcitationVector& operator=(const KHashExcitationVector& rhs) = delete;
	KHashExcitationVector(KHashExcitationVector&& rhs) = delete;
	KHashExcitationVector&& operator=(KHashExcitationVector&& rhs) = delete;

	~KHashExcitationVector();

	virtual void SetElement(const uint16_t r, const T& value);
	virtual void SetElementQuick(const uint16_t r, const T& value);

	virtual T GetElement(const uint16_t r) const;
	virtual T GetElementQuick(const uint16_t r) const;

	virtual uint16_t get_num_rows() const { return num_rows_; }

	virtual uint16_t GetNnz() const { return map_->size; }

	virtual void Add(const IExcitationVector<T>& other);
	virtual void Normalize();
	virtual void Whiten();
	virtual std::set<std::pair<uint16_t, T>> GetNzElements() const;

private:
	const uint16_t num_rows_;
	khash_t(vector_float) *map_ = kh_init(vector_float);
};

template <typename T>
KHashExcitationVector<T>::KHashExcitationVector(const uint16_t num_rows) : num_rows_(num_rows)
{
	if (!std::is_same<T, float>::value) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}
}

template <typename T>
KHashExcitationVector<T>::KHashExcitationVector(const IExcitationVector<T> &base) : num_rows_(base.get_num_rows())
{
	int absent = 0;
	khint_t k = 0;

	if (!std::is_same<T, float>::value) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}

	for (const std::pair<uint16_t, T>& element : base.GetNzElements()) {
		k = kh_put(vector_float, map_, element.first, &absent);
		kh_value(map_, k) = element.second;
	}
}

template <typename T>
KHashExcitationVector<T>::~KHashExcitationVector()
{
	kh_destroy(vector_float, map_);
}

template <typename T>
void KHashExcitationVector<T>::SetElement(const uint16_t r, const T& value)
{
	int absent = 0;
	khint_t k = 0;

	if (!std::is_same<T, float>::value) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}

	IExcitationVector<T>::CheckBounds(r);
	if (IsNearlyEqual(value, 0.0)) {
		k = kh_get(vector_float, map_, r);
		absent = (k == kh_end(map_));
		if (!absent) {
			kh_del(vector_float, map_, k);
		}
	} else {
		SetElementQuick(r, value);
	}
}

template <typename T>
void KHashExcitationVector<T>::SetElementQuick(const uint16_t r, const T &value)
{
	int absent = 0;
	khint_t k = 0;

	k = kh_put(vector_float, map_, r, &absent);
	if (!absent) {
		kh_del(vector_float, map_, k);
	}

	kh_value(map_, k) = value;
}

template <typename T>
T KHashExcitationVector<T>::GetElement(const uint16_t r) const
{
	if (!std::is_same<T, float>::value) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}

	IExcitationVector<T>::CheckBounds(r);
	return GetElementQuick(r);
}

template <typename T>
T KHashExcitationVector<T>::GetElementQuick(const uint16_t r) const
{
	T result;
	int present = 0;
	khint_t k = 0;

	k = kh_get(vector_float, map_, r);
	present = (k == kh_end(map_));

	if (present) {
		result = kh_value(map_, k);
	} else {
		result = 0.0f;
	}

	return result;
}

template <typename T>
void KHashExcitationVector<T>::Add(const IExcitationVector<T>& other)
{
	if (!std::is_same<T, float>::value) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}

	for (const std::pair<uint16_t, T>& element : other.GetNzElements()) {
		SetElement(element.first, PositiveClip(element.second + GetElement(element.first)));
	}
}

template <typename T>
void KHashExcitationVector<T>::Normalize()
{
	double sum = 0.0;
	khint_t k = 0;

	if (!std::is_same<T, float>::value) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}

	for (k = kh_begin(map_); k != kh_end(map_); ++k) {
		if (kh_exist(map_, k)) {
			sum += kh_value(map_, k);
		}
	}

	if (sum > 0.0) {
		for (k = kh_begin(map_); k != kh_end(map_); ++k) {
			if (kh_exist(map_, k)) {
				kh_value(map_, k) /= sum;
			}
		}
	}
}

template <typename T>
void KHashExcitationVector<T>::Whiten()
{
	double sum = 0.0;
	double squared_sum = 0.0;
	double mean = 0.0;
	double variance = 0.0;
	double std = 0.0;
	khint_t k = 0;

	if (!std::is_same<T, float>::value) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}

	for (k = kh_begin(map_); k != kh_end(map_); ++k) {
		if (kh_exist(map_, k)) {
			sum += kh_value(map_, k);
			squared_sum += kh_value(map_, k) * kh_value(map_, k);
		}
	}

	mean = sum / map_->size;
	variance = squared_sum / map_->size - mean * mean;
	std = sqrt(variance) + 1e-6;

	if (sum > 0.0) {
		for (k = kh_begin(map_); k != kh_end(map_); ++k) {
			if (kh_exist(map_, k)) {
				kh_value(map_, k) = (kh_value(map_, k) - mean) / std;
			}
		}
	}
}

template <typename T>
std::set<std::pair<uint16_t, T>> KHashExcitationVector<T>::GetNzElements() const
{
	typename std::set<std::pair<uint16_t, T>> result;
	khint_t k = 0;

	if (!std::is_same<T, float>::value) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}

	for (k = kh_begin(map_); k != kh_end(map_); ++k) {
		if (kh_exist(map_, k)) {
			result.insert(std::make_pair(kh_key(map_, k), kh_value(map_, k)));
		}
	}

	return result;
}
