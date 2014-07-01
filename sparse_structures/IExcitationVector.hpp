#ifndef IEXCITATIONVECTOR_H
#define IEXCITATIONVECTOR_H

#include <stdexcept>
#include <string>
#include <set>

template <typename T>
class IExcitationVector
{
public:
    virtual void SetElement(const size_t r, const T& value) = 0;
    virtual void SetElementQuick(const size_t r, const T& value) = 0;

    virtual const T& GetElement(const size_t r) const = 0;
    virtual const T& GetElementQuick(const size_t r) const = 0;

    virtual size_t get_num_rows() const = 0;

    virtual size_t GetNnz() const = 0;

    void CheckBounds(const size_t r) const
    {
        if (r >= get_num_rows())
            throw std::out_of_range("1D Out of Range");
    }

    virtual void Add(const IExcitationVector& other) = 0;

    virtual std::set<std::pair<size_t, T>> GetNzElements() const = 0;

    std::string ToString() const
    {
        const size_t num_rows = get_num_rows();

        std::string str = std::string("ExcitationVector [num_lines=") + std::to_string(num_rows) + "]\n";
        for (size_t r = 0; r < num_rows; ++r) {
            str += GetElement(r) + " ";
            str += "\n";
        }
        return str;
    }
};

#endif // IEXCITATIONVECTORINTERFACE_H
