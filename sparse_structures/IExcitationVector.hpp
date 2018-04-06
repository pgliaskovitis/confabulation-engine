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

#ifndef IEXCITATIONVECTOR_H
#define IEXCITATIONVECTOR_H

#include <stdexcept>
#include <string>
#include <set>

template <typename T>
class IExcitationVector
{
public:
	virtual ~IExcitationVector() {}

	virtual void SetElement(const uint16_t r, const T& value) = 0;
	virtual void SetElementQuick(const uint16_t r, const T& value) = 0;

	virtual T GetElement(const uint16_t r) const = 0;
	virtual T GetElementQuick(const uint16_t r) const = 0;

	virtual uint16_t get_num_rows() const = 0;

	virtual uint16_t GetNnz() const = 0;

	void CheckBounds(const uint16_t r) const
	{
		if (r >= get_num_rows()) {
			throw std::out_of_range(std::string("1D Out of Range, for row ") +
									std::to_string(r));
		}
	}

	virtual void Add(const IExcitationVector& other) = 0;

	virtual void Normalize() = 0;

	virtual void Whiten() = 0;

	virtual std::set<std::pair<uint16_t, T>> GetNzElements() const = 0;

	std::string ToString() const
	{
		const uint16_t num_rows = get_num_rows();

		std::string str = std::string("ExcitationVector [num_lines=") + std::to_string(num_rows) + "]\n";
		for (uint16_t r = 0; r < num_rows; ++r) {
			str += std::to_string(GetElement(r)) + " ";
			str += "\n";
		}
		return str;
	}
};

#endif // IEXCITATIONVECTORINTERFACE_H
