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

#ifndef IKNOWLEDGELINKS_H
#define IKNOWLEDGELINKS_H

#include <string>
#include <set>
#include <memory>

#include "IExcitationVector.hpp"

template <typename T>
class IKnowledgeLinks
{
public:
    virtual void SetElement(const unsigned long r, const unsigned long c, const T& value) = 0;
    virtual void SetElementQuick(const unsigned long r, unsigned long c, const T& value) = 0;

    virtual T GetElement(const unsigned long r, const unsigned long c) const = 0;
    virtual T GetElementQuick(const unsigned long r, const unsigned long c) const = 0;

    virtual unsigned long get_num_rows() const = 0;
    virtual unsigned long get_num_cols() const = 0;

    virtual unsigned long GetNnz() const = 0;

    void CheckBounds(const unsigned long r, const unsigned long c) const
    {
        if (r >= get_num_rows() || c >= get_num_cols())
            throw std::out_of_range("2D Out of Range");
    }

    virtual std::unique_ptr<IExcitationVector<T>> Multiply(const IExcitationVector<T>& vec) const = 0;
    virtual std::set<std::pair<std::pair<unsigned long, unsigned long>, T>> GetNzElements() const = 0;

    std::string ToString() const
    {
        const unsigned long num_rows = get_num_rows();
        const unsigned long num_cols = get_num_cols();

        std::string str = std::string("LinksMatrix [num_lines=") + std::to_string(num_rows) + ", num_cols=" + std::to_string(num_cols) + "]\n";
        for (unsigned long r = 0; r < num_rows; ++r) {
            for (unsigned long c = 0; c < num_cols; ++c)
                str += std::to_string(GetElement(r, c)) + " ";

            str += "\n";
        }
        return str;
    }

};

#endif // IKNOWLEDGELINKS_H
