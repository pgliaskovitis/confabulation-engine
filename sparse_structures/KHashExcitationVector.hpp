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
KHASH_INIT(vector_uint16, uint16_t, uint16_t, 0, kh_int_hash_func, kh_int_hash_equal)

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

	virtual uint16_t GetNnz() const;

	virtual void Add(const IExcitationVector<T>& other);
	virtual void Normalize();
	virtual void Whiten();
	virtual std::set<std::pair<uint16_t, T>> GetNzElements() const;

private:
	const uint16_t num_rows_;
	khash_t(vector_float) *map_float_ = kh_init(vector_float);
	khash_t(vector_uint16) *map_uint16_ = kh_init(vector_uint16);
};

template <typename T>
KHashExcitationVector<T>::KHashExcitationVector(const uint16_t num_rows) : num_rows_(num_rows)
{
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;

	if (!is_uint16 && !is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float and uint16_t" );
	}
}

template <typename T>
KHashExcitationVector<T>::KHashExcitationVector(const IExcitationVector<T> &base) : num_rows_(base.get_num_rows())
{
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;
	int absent = 0;
	khint_t k = 0;

	if (!is_uint16 && !is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float and uint16_t" );
	}

	if (is_float) {
		for (const std::pair<uint16_t, T>& element : base.GetNzElements()) {
			k = kh_put(vector_float, map_float_, element.first, &absent);
			kh_value(map_float_, k) = element.second;
		}
	} else if (is_uint16) {
		for (const std::pair<uint16_t, T>& element : base.GetNzElements()) {
			k = kh_put(vector_uint16, map_uint16_, element.first, &absent);
			kh_value(map_uint16_, k) = element.second;
		}
	}
}

template <typename T>
KHashExcitationVector<T>::~KHashExcitationVector()
{
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;

	if (!is_uint16 && !is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float and uint16_t" );
	}

	if (is_float) {
		kh_destroy(vector_float, map_float_);
	} else if (is_uint16) {
		kh_destroy(vector_uint16, map_uint16_);
	}
}

template <typename T>
void KHashExcitationVector<T>::SetElement(const uint16_t r, const T& value)
{
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;
	int absent = 0;
	khint_t k = 0;

	if (!is_uint16 && !is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float and uint16_t" );
	}

	IExcitationVector<T>::CheckBounds(r);

	if (IsNearlyEqual(value, 0.0)) {
		if (is_float) {
			k = kh_get(vector_float, map_float_, r);
			absent = (k == kh_end(map_float_));
			if (!absent) {
				kh_del(vector_float, map_float_, k);
			}
		} else if (is_uint16) {
			k = kh_get(vector_uint16, map_uint16_, r);
			absent = (k == kh_end(map_uint16_));
			if (!absent) {
				kh_del(vector_uint16, map_uint16_, k);
			}
		}
	} else {
		SetElementQuick(r, value);
	}
}

template <typename T>
void KHashExcitationVector<T>::SetElementQuick(const uint16_t r, const T &value)
{
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;
	int absent = 0;
	khint_t k = 0;

	if (is_float) {
		k = kh_put(vector_float, map_float_, r, &absent);
		if (!absent) {
			kh_del(vector_float, map_float_, k);
		}
		kh_value(map_float_, k) = value;
	} else if (is_uint16) {
		k = kh_put(vector_uint16, map_uint16_, r, &absent);
		if (!absent) {
			kh_del(vector_uint16, map_uint16_, k);
		}
		kh_value(map_uint16_, k) = value;
	}
}

template <typename T>
T KHashExcitationVector<T>::GetElement(const uint16_t r) const
{
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;

	if (!is_uint16 && !is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float and uint16_t" );
	}

	IExcitationVector<T>::CheckBounds(r);
	return GetElementQuick(r);
}

template <typename T>
T KHashExcitationVector<T>::GetElementQuick(const uint16_t r) const
{
	T result;
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;
	int present = 0;
	khint_t k = 0;

	if (is_float) {
		k = kh_get(vector_float, map_float_, r);
		present = (k == kh_end(map_float_));
		if (present) {
			result = kh_value(map_float_, k);
		} else {
			result = 0.0f;
		}
	} else if (is_uint16) {
		k = kh_get(vector_uint16, map_uint16_, r);
		present = (k == kh_end(map_uint16_));
		if (present) {
			result = kh_value(map_uint16_, k);
		} else {
			result = 0;
		}
	}

	return result;
}

template <typename T>
uint16_t KHashExcitationVector<T>::GetNnz() const
{
	uint16_t result = 0;
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;

	if (!is_uint16 && !is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float and uint16_t" );
	}

	if (is_float) {
		result = map_float_->size;
	} else if (is_uint16) {
		result = map_uint16_->size;
	}

	return result;
}

template <typename T>
void KHashExcitationVector<T>::Add(const IExcitationVector<T>& other)
{
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;

	if (!is_uint16 && !is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float and uint16_t" );
	}

	for (const std::pair<uint16_t, T>& element : other.GetNzElements()) {
		SetElement(element.first, PositiveClip(element.second + GetElement(element.first)));
	}
}

template <typename T>
void KHashExcitationVector<T>::Normalize()
{
	bool is_float = std::is_same<T, float>::value;
	double sum = 0.0;
	khint_t k = 0;

	if (!is_float) {
		throw std::runtime_error( "KHashExcitationVector normalize is not supported for value types other than float" );
	}

	for (k = kh_begin(map_float_); k != kh_end(map_float_); ++k) {
		if (kh_exist(map_float_, k)) {
			sum += kh_value(map_float_, k);
		}
	}

	if (sum > 0.0) {
		for (k = kh_begin(map_float_); k != kh_end(map_float_); ++k) {
			if (kh_exist(map_float_, k)) {
				kh_value(map_float_, k) /= sum;
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
	bool is_float = std::is_same<T, float>::value;
	khint_t k = 0;

	if (!is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float" );
	}

	for (k = kh_begin(map_float_); k != kh_end(map_float_); ++k) {
		if (kh_exist(map_float_, k)) {
			sum += kh_value(map_float_, k);
			squared_sum += kh_value(map_float_, k) * kh_value(map_float_, k);
		}
	}

	mean = sum / map_float_->size;
	variance = squared_sum / map_float_->size - mean * mean;
	std = sqrt(variance) + 1e-6;

	if (sum > 0.0) {
		for (k = kh_begin(map_float_); k != kh_end(map_float_); ++k) {
			if (kh_exist(map_float_, k)) {
				kh_value(map_float_, k) = (kh_value(map_float_, k) - mean) / std;
			}
		}
	}
}

template <typename T>
std::set<std::pair<uint16_t, T>> KHashExcitationVector<T>::GetNzElements() const
{
	typename std::set<std::pair<uint16_t, T>> result;
	bool is_uint16 = std::is_same<T, uint16_t>::value;
	bool is_float = std::is_same<T, float>::value;
	khint_t k = 0;

	if (!is_uint16 && !is_float) {
		throw std::runtime_error( "KHashExcitationVector is not supported for value types other than float and uint16_t" );
	}

	if (is_float) {
		for (k = kh_begin(map_float_); k != kh_end(map_float_); ++k) {
			if (kh_exist(map_float_, k)) {
				result.insert(std::make_pair(kh_key(map_float_, k), kh_value(map_float_, k)));
			}
		}
	} else if (is_uint16) {
		for (k = kh_begin(map_uint16_); k != kh_end(map_uint16_); ++k) {
			if (kh_exist(map_uint16_, k)) {
				result.insert(std::make_pair(kh_key(map_uint16_, k), kh_value(map_uint16_, k)));
			}
		}
	}

	return result;
}
