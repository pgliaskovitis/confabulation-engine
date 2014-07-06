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
    virtual void SetElement(const size_t r, const size_t c, const T& value) = 0;
    virtual void SetElementQuick(const size_t r, size_t c, const T& value) = 0;

    virtual const T& GetElement(const size_t r, const size_t c) const = 0;
    virtual const T& GetElementQuick(const size_t r, const size_t c) const = 0;

    virtual size_t get_num_rows() const = 0;
    virtual size_t get_num_cols() const = 0;

    virtual size_t GetNnz() const = 0;

    void CheckBounds(const size_t r, const size_t c) const
    {
        if (r >= get_num_rows() || c >= get_num_cols())
            throw std::out_of_range("2D Out of Range");
    }

    virtual std::unique_ptr<IExcitationVector<T>> multiply(const IExcitationVector<T>& vec) const = 0;
    virtual std::set<std::pair<std::pair<size_t, size_t>, T>> GetNzElements() const = 0;

    std::string ToString() const
    {
        const size_t num_rows = get_num_rows();
        const size_t num_cols = get_num_cols();

        std::string str = std::string("LinksMatrix [num_lines=") + std::to_string(num_rows) + ", num_cols=" + std::string(num_cols) + "]\n";
        for (size_t r = 0; r < num_rows; ++r) {
            for (size_t c = 0; c < num_cols; ++c)
                str += GetElement(r, c) + " ";

            str += "\n";
        }
        return str;
    }

};

#endif // IKNOWLEDGELINKS_H
