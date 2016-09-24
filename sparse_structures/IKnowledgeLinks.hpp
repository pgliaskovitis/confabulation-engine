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
    virtual ~IKnowledgeLinks() {}

    virtual void SetElement(const uint16_t r, const uint16_t c, const T& value) = 0;
    virtual void SetElementQuick(const uint16_t r, uint16_t c, const T& value) = 0;

    virtual T GetElement(const uint16_t r, const uint16_t c) const = 0;
    virtual T GetElementQuick(const uint16_t r, const uint16_t c) const = 0;

    virtual uint16_t get_num_rows() const = 0;
    virtual uint16_t get_num_cols() const = 0;

    virtual uint32_t GetNnz() const = 0;

    void CheckBounds(const uint16_t r, const uint16_t c) const
    {
        if (r >= get_num_rows() || c >= get_num_cols()) {
            throw std::out_of_range(std::string("2D Out of Range, for row ") +
                                    std::to_string(r) +
                                    " and column " +
                                    std::to_string(c));
        }
    }

    virtual std::unique_ptr<IExcitationVector<T>> Multiply(const IExcitationVector<T>& vec) const = 0;
    virtual std::set<std::pair<std::pair<uint16_t, uint16_t>, T>> GetNzElements() const = 0;

    std::string ToString() const
    {
        const uint32_t num_rows = get_num_rows();
        const uint32_t num_cols = get_num_cols();

        std::string str = std::string("LinksMatrix [num_lines=") + std::to_string(num_rows) + ", num_cols=" + std::to_string(num_cols) + "]\n";
        for (uint32_t r = 0; r < num_rows; ++r) {
            for (uint32_t c = 0; c < num_cols; ++c) {
                str += std::to_string(GetElement(r, c)) + " ";
            }

            str += "\n";
        }
        return str;
    }

};

#endif // IKNOWLEDGELINKS_H
