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

#include <string>
#include <set>
#include <memory>

#include "IExcitationVector.hpp"

struct PairHash {
public:
	template <typename TRow, typename TCol>
	std::size_t operator()(const std::pair<TRow, TCol> &x) const
	{
		return x.first * std::numeric_limits<TCol>::max() + x.second;
	}

	/*
	template <typename TRow, typename TCol>
	std::size_t operator()(const std::pair<TRow, TCol> &x) const
	{
		return std::hash<TRow>()(x.first) ^ std::hash<TCol>()(x.second);
	}
	*/
};

struct PairEquals {
public:
	template <typename T>
	bool operator()(const std::pair<T, T> &x, const std::pair<T, T> &y) const
	{
		return x == y;
	}
};

template <typename TRow, typename TCol, typename T>
class IKnowledgeLinks
{
public:
	virtual ~IKnowledgeLinks() {}

	virtual void SetElement(const TRow r, const TCol c, const T& value) = 0;
	virtual void SetElementQuick(const TRow r, TCol c, const T& value) = 0;

	virtual T GetElement(const TRow r, const TCol c) const = 0;
	virtual T GetElementQuick(const TRow r, const TCol c) const = 0;

	virtual TRow GetNumRows() const = 0;
	virtual TCol GetNumCols() const = 0;

	virtual uint32_t GetNnz() const = 0;

	void CheckBounds(const TRow r, const TCol c) const
	{
		if (r >= GetNumRows() || c >= GetNumCols()) {
			throw std::out_of_range(std::string("2D Out of Range, for row ") +
									std::to_string(r) +
									" and column " +
									std::to_string(c));
		}
	}

	virtual std::unique_ptr<IExcitationVector<TRow, T>> Multiply(const IExcitationVector<TCol, T>& vec) const = 0;
	virtual std::set<std::pair<std::pair<TRow, TCol>, T>> GetNzElements() const = 0;

	std::string ToString() const
	{
		const TRow num_rows = GetNumRows();
		const TCol num_cols = GetNumCols();

		std::string str = std::string("LinksMatrix [num_lines=") + std::to_string(num_rows) + ", num_cols=" + std::to_string(num_cols) + "]\n";
		for (TRow r = 0; r < num_rows; ++r) {
			for (TCol c = 0; c < num_cols; ++c) {
				str += std::to_string(GetElement(r, c)) + " ";
			}

			str += "\n";
		}
		return str;
	}
};
