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

#include "Dbg.h"
#include "klib/khash.h"
#include "utils/Utils.h"
#include "IExcitationVector.hpp"

KHASH_INIT(vector_u16_float, uint16_t, float, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(vector_u16_u16, uint16_t, uint16_t, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(vector_u32_float, uint32_t, float, 1, kh_int_hash_func, kh_int_hash_equal)
KHASH_INIT(vector_u32_u16, uint32_t, uint16_t, 1, kh_int_hash_func, kh_int_hash_equal)

enum class VectorType {
	vector_u16_float_t,
	vector_u16_u16_t,
	vector_u32_float_t,
	vector_u32_u16_t
};

template <typename TRow, typename T>
class KHashExcitationVector : public IExcitationVector<TRow, T>
{
public:
	KHashExcitationVector(const TRow num_rows);
	KHashExcitationVector(const IExcitationVector<TRow, T>& base);

	KHashExcitationVector(const KHashExcitationVector& rhs) = delete;
	KHashExcitationVector& operator=(const KHashExcitationVector& rhs) = delete;
	KHashExcitationVector(KHashExcitationVector&& rhs) = delete;
	KHashExcitationVector&& operator=(KHashExcitationVector&& rhs) = delete;

	~KHashExcitationVector();

	virtual void SetElement(const TRow r, const T& value);
	virtual void SetElementQuick(const TRow r, const T& value);

	virtual T GetElement(const TRow r) const;
	virtual T GetElementQuick(const TRow r) const;

	virtual TRow GetNumRows() const { return num_rows_; }

	virtual TRow GetNnz() const;

	virtual void Add(const IExcitationVector<TRow, T>& other);
	virtual void Normalize();
	virtual void Whiten();
	virtual std::set<std::pair<TRow, T>> GetNzElements() const;

	static VectorType GetType()
	{
		if (std::is_same<TRow, uint16_t>::value && std::is_same<T, float>::value) {
			return VectorType::vector_u16_float_t;
		} else if (std::is_same<TRow, uint16_t>::value && std::is_same<T, uint16_t>::value) {
			return VectorType::vector_u16_u16_t;
		} else if (std::is_same<TRow, uint32_t>::value && std::is_same<T, float>::value) {
			return VectorType::vector_u32_float_t;
		} else if (std::is_same<TRow, uint32_t>::value && std::is_same<T, uint16_t>::value) {
			return VectorType::vector_u32_u16_t;
		}
	}

private:

	const TRow num_rows_;
	khash_t(vector_u16_float) *map_u16_float_;
	khash_t(vector_u16_u16) *map_u16_u16_;
	khash_t(vector_u32_float) *map_u32_float_;
	khash_t(vector_u32_u16) *map_u32_u16_;
};

template <typename TRow, typename T>
KHashExcitationVector<TRow, T>::KHashExcitationVector(const TRow num_rows) : num_rows_(num_rows)
{
	VectorType vector_type = GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		map_u16_float_ = kh_init(vector_u16_float);
		break;
	case VectorType::vector_u16_u16_t:
		map_u16_u16_ = kh_init(vector_u16_u16);
		break;
	case VectorType::vector_u32_float_t:
		map_u32_float_ = kh_init(vector_u32_float);
		break;
	case VectorType::vector_u32_u16_t:
		map_u32_u16_ = kh_init(vector_u32_u16);
		break;
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}
}

template <typename TRow, typename T>
KHashExcitationVector<TRow, T>::KHashExcitationVector(const IExcitationVector<TRow, T> &base) : num_rows_(base.GetNumRows())
{
	int absent = 0;
	khint_t k = 0;

	VectorType vector_type = GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		map_u16_float_ = kh_init(vector_u16_float);
		for (const std::pair<TRow, T>& element : base.GetNzElements()) {
			k = kh_put(vector_u16_float, map_u16_float_, element.first, &absent);
			kh_value(map_u16_float_, k) = element.second;
		}
		break;
	case VectorType::vector_u16_u16_t:
		map_u16_u16_ = kh_init(vector_u16_u16);
		for (const std::pair<TRow, T>& element : base.GetNzElements()) {
			k = kh_put(vector_u16_u16, map_u16_u16_, element.first, &absent);
			kh_value(map_u16_u16_, k) = element.second;
		}
		break;
	case VectorType::vector_u32_float_t:
		map_u32_float_ = kh_init(vector_u32_float);
		for (const std::pair<TRow, T>& element : base.GetNzElements()) {
			k = kh_put(vector_u32_float, map_u32_float_, element.first, &absent);
			kh_value(map_u32_float_, k) = element.second;
		}
		break;
	case VectorType::vector_u32_u16_t:
		map_u32_u16_ = kh_init(vector_u32_u16);
		for (const std::pair<TRow, T>& element : base.GetNzElements()) {
			k = kh_put(vector_u32_u16, map_u32_u16_, element.first, &absent);
			kh_value(map_u32_u16_, k) = element.second;
		}
		break;
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}
}

template <typename TRow, typename T>
KHashExcitationVector<TRow, T>::~KHashExcitationVector()
{
	VectorType vector_type = KHashExcitationVector::GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		kh_destroy(vector_u16_float, map_u16_float_);
		break;
	case VectorType::vector_u16_u16_t:
		kh_destroy(vector_u16_u16, map_u16_u16_);
		break;
	case VectorType::vector_u32_float_t:
		kh_destroy(vector_u32_float, map_u32_float_);
		break;
	case VectorType::vector_u32_u16_t:
		kh_destroy(vector_u32_u16, map_u32_u16_);
		break;
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}
}

template <typename TRow, typename T>
void KHashExcitationVector<TRow, T>::SetElement(const TRow r, const T& value)
{
	khint_t k = 0;

	IExcitationVector<TRow, T>::CheckBounds(r);

	VectorType vector_type = KHashExcitationVector::GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		if (IsNearlyEqual(value, 0.0f)) {
			k = kh_get(vector_u16_float, map_u16_float_, r);
			kh_value(map_u16_float_, k) = 0.0f;
		} else {
			SetElementQuick(r, value);
		}
		break;
	case VectorType::vector_u16_u16_t:
		SetElementQuick(r, value);
		break;
	case VectorType::vector_u32_float_t:
		if (IsNearlyEqual(value, 0.0f)) {
			k = kh_get(vector_u32_float, map_u32_float_, r);
			kh_value(map_u32_float_, k) = 0.0f;
		} else {
			SetElementQuick(r, value);
		}
		break;
	case VectorType::vector_u32_u16_t:
		SetElementQuick(r, value);
		break;
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}
}

template <typename TRow, typename T>
void KHashExcitationVector<TRow, T>::SetElementQuick(const TRow r, const T &value)
{
	int absent = 0;
	khint_t k = 0;

	VectorType vector_type = KHashExcitationVector::GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		k = kh_put(vector_u16_float, map_u16_float_, r, &absent);
		kh_value(map_u16_float_, k) = value;
		break;
	case VectorType::vector_u16_u16_t:
		k = kh_put(vector_u16_u16, map_u16_u16_, r, &absent);
		kh_value(map_u16_u16_, k) = value;
		break;
	case VectorType::vector_u32_float_t:
		k = kh_put(vector_u32_float, map_u32_float_, r, &absent);
		kh_value(map_u32_float_, k) = value;
		break;
	case VectorType::vector_u32_u16_t:
		k = kh_put(vector_u32_u16, map_u32_u16_, r, &absent);
		kh_value(map_u32_u16_, k) = value;
		break;
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}
}

template <typename TRow, typename T>
T KHashExcitationVector<TRow, T>::GetElement(const TRow r) const
{
	IExcitationVector<TRow, T>::CheckBounds(r);
	return GetElementQuick(r);
}

template <typename TRow, typename T>
T KHashExcitationVector<TRow, T>::GetElementQuick(const TRow r) const
{
	T result;
	int present = 0;
	khint_t k = 0;

	VectorType vector_type = GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		k = kh_get(vector_u16_float, map_u16_float_, r);
		present = !(k == kh_end(map_u16_float_));
		if (present) {
			result = kh_value(map_u16_float_, k);
		} else {
			result = 0.0f;
		}
		break;
	case VectorType::vector_u16_u16_t:
		k = kh_get(vector_u16_u16, map_u16_u16_, r);
		present = !(k == kh_end(map_u16_u16_));
		if (present) {
			result = kh_value(map_u16_u16_, k);
		} else {
			result = 0;
		}
		break;
	case VectorType::vector_u32_float_t:
		k = kh_get(vector_u32_float, map_u32_float_, r);
		present = !(k == kh_end(map_u32_float_));
		if (present) {
			result = kh_value(map_u32_float_, k);
		} else {
			result = 0;
		}		break;
	case VectorType::vector_u32_u16_t:
		k = kh_get(vector_u32_u16, map_u32_u16_, r);
		present = !(k == kh_end(map_u32_u16_));
		if (present) {
			result = kh_value(map_u32_u16_, k);
		} else {
			result = 0;
		}
		break;
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}

	return result;
}

template <typename TRow, typename T>
TRow KHashExcitationVector<TRow, T>::GetNnz() const
{
	TRow result = 0;

	VectorType vector_type = GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		return kh_size(map_u16_float_);
	case VectorType::vector_u16_u16_t:
		return kh_size(map_u16_u16_);
	case VectorType::vector_u32_float_t:
		return kh_size(map_u32_float_);
	case VectorType::vector_u32_u16_t:
		return kh_size(map_u32_u16_);
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}

	return result;
}

template <typename TRow, typename T>
void KHashExcitationVector<TRow, T>::Add(const IExcitationVector<TRow, T>& other)
{
	for (const std::pair<TRow, T>& element : other.GetNzElements()) {
		SetElement(element.first, PositiveClip(element.second + GetElement(element.first)));
	}
}

template <typename TRow, typename T>
void KHashExcitationVector<TRow, T>::Normalize()
{
	double sum = 0.0;
	khint_t k = 0;

	VectorType vector_type = GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		for (k = kh_begin(map_u16_float_); k != kh_end(map_u16_float_); ++k) {
			if (kh_exist(map_u16_float_, k)) {
				sum += kh_value(map_u16_float_, k);
			}
		}
		if (sum > 0.0) {
			for (k = kh_begin(map_u16_float_); k != kh_end(map_u16_float_); ++k) {
				if (kh_exist(map_u16_float_, k)) {
					kh_value(map_u16_float_, k) = kh_value(map_u16_float_, k) / sum;
				}
			}
		}
		break;
	case VectorType::vector_u32_float_t:
		for (k = kh_begin(map_u32_float_); k != kh_end(map_u32_float_); ++k) {
			if (kh_exist(map_u32_float_, k)) {
				sum += kh_value(map_u32_float_, k);
			}
		}
		if (sum > 0.0) {
			for (k = kh_begin(map_u32_float_); k != kh_end(map_u32_float_); ++k) {
				if (kh_exist(map_u32_float_, k)) {
					kh_value(map_u32_float_, k) = kh_value(map_u32_float_, k) / sum;
				}
			}
		}
		break;
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}
}

template <typename TRow, typename T>
void KHashExcitationVector<TRow, T>::Whiten()
{
	double sum = 0.0;
	double squared_sum = 0.0;
	double mean = 0.0;
	double variance = 0.0;
	double std = 0.0;
	khint_t k = 0;

	VectorType vector_type = GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t: {
		for (k = kh_begin(map_u16_float_); k != kh_end(map_u16_float_); ++k) {
			if (kh_exist(map_u16_float_, k)) {
				sum += kh_value(map_u16_float_, k);
				squared_sum += kh_value(map_u16_float_, k) * kh_value(map_u16_float_, k);
			}
		}
		mean = sum / kh_size(map_u16_float_);
		variance = squared_sum / kh_size(map_u16_float_) - mean * mean;
		std = sqrt(variance) + 1e-6;
		if (sum > 0.0) {
			for (k = kh_begin(map_u16_float_); k != kh_end(map_u16_float_); ++k) {
				if (kh_exist(map_u16_float_, k)) {
					kh_value(map_u16_float_, k) = (kh_value(map_u16_float_, k) - mean) / std;
				}
			}
		}
		break;
	}
	case VectorType::vector_u32_float_t: {
		for (k = kh_begin(map_u32_float_); k != kh_end(map_u32_float_); ++k) {
			if (kh_exist(map_u32_float_, k)) {
				sum += kh_value(map_u32_float_, k);
				squared_sum += kh_value(map_u32_float_, k) * kh_value(map_u32_float_, k);
			}
		}
		mean = sum / kh_size(map_u32_float_);
		variance = squared_sum / kh_size(map_u32_float_) - mean * mean;
		std = sqrt(variance) + 1e-6;
		if (sum > 0.0) {
			for (k = kh_begin(map_u32_float_); k != kh_end(map_u32_float_); ++k) {
				if (kh_exist(map_u32_float_, k)) {
					kh_value(map_u32_float_, k) = (kh_value(map_u32_float_, k) - mean) / std;
				}
			}
		}
		break;
	}
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}
}

template <typename TRow, typename T>
std::set<std::pair<TRow, T>> KHashExcitationVector<TRow, T>::GetNzElements() const
{
	typename std::set<std::pair<TRow, T>> result;
	khint_t k = 0;

	VectorType vector_type = GetType();
	switch(vector_type) {
	case VectorType::vector_u16_float_t:
		for (k = kh_begin(map_u16_float_); k != kh_end(map_u16_float_); ++k) {
			if (kh_exist(map_u16_float_, k)) {
				result.insert(std::make_pair(kh_key(map_u16_float_, k), kh_value(map_u16_float_, k)));
			}
		}
		break;
	case VectorType::vector_u16_u16_t:
		for (k = kh_begin(map_u16_u16_); k != kh_end(map_u16_u16_); ++k) {
			if (kh_exist(map_u16_u16_, k)) {
				result.insert(std::make_pair(kh_key(map_u16_u16_, k), kh_value(map_u16_u16_, k)));
			}
		}
		break;
	case VectorType::vector_u32_float_t:
		for (k = kh_begin(map_u32_float_); k != kh_end(map_u32_float_); ++k) {
			if (kh_exist(map_u32_float_, k)) {
				result.insert(std::make_pair(kh_key(map_u32_float_, k), kh_value(map_u32_float_, k)));
			}
		}
		break;
	case VectorType::vector_u32_u16_t:
		for (k = kh_begin(map_u32_u16_); k != kh_end(map_u32_u16_); ++k) {
			if (kh_exist(map_u32_u16_, k)) {
				result.insert(std::make_pair(kh_key(map_u32_u16_, k), kh_value(map_u32_u16_, k)));
			}
		}
		break;
	default:
		log_debug("KHashExcitationVector is not supported for the given types");
	}

	return result;
}
