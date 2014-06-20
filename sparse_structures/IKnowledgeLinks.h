#ifndef IKNOWLEDGELINKS_H
#define IKNOWLEDGELINKS_H

#include <string>
#include <set>
#include <tuple>

template <typename T>
class IKnowledgeLinks
{
public:
    virtual void SetElement(size_t l, size_t c, T& value) = 0;
    virtual void SetElementQuick(size_t l, size_t c, T& value) = 0;

    virtual const T& GetElement(size_t l, size_t c) const = 0;
    virtual const T& GetElementQuick(size_t l, size_t c) const = 0;

    virtual size_t get_num_lines() = 0;
    virtual size_t get_num_cols() = 0;
    virtual size_t getnnz() = 0;

    bool CheckBounds(size_t l, size_t c)
    {
        if (l < 0 || c < 0 || l >= get_num_lines() || c >= get_num_cols())
            return false;

        return true;
    }

    //public virtual DOK1Dfloat multiply(DOK1Dfloat vec) = 0;

    virtual std::set<std::tuple<size_t, size_t, T>> GetNzElements() = 0;

    std::string ToString() {
        const size_t num_lines = get_num_lines();
        const size_t num_cols = get_num_cols();

        std::string str = std::string("Matrix [num_lines=") + std::to_string(num_lines) + ", num_cols=" + std::string(num_cols) + "]\n";
        for (size_t l = 0; l < num_lines; ++l) {
            for (size_t c = 0; c < num_cols; ++c)
                str += GetElement(l, c) + " ";

            str += "\n";
        }
        return str;
    }

};

#endif // IKNOWLEDGELINKS_H
