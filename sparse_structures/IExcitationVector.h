#ifndef IEXCITATIONVECTOR_H
#define IEXCITATIONVECTOR_H

#include <string>
#include <set>

template <typename T>
class IExcitationVector
{
public:
    virtual void SetElement(size_t r, T& value) = 0;
    virtual void SetElementQuick(size_t r, T& value) = 0;

    virtual const T& GetElement(size_t r) const = 0;
    virtual const T& GetElementQuick(size_t r) const = 0;

    virtual size_t get_num_rows() = 0;
    virtual size_t getnnz() = 0;

    bool CheckBounds(size_t r)
    {
        if (r < 0 || r >= get_num_rows())
            return false;

        return true;
    }

    void add(IExcitationVector& other);

    virtual std::set<std::pair<size_t, T>> GetNzElements() = 0;

    std::string ToString()
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
