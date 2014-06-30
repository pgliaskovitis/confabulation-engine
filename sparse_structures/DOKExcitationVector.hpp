#ifndef DOKEXCITATIONVECTOR_H
#define DOKEXCITATIONVECTOR_H

#include <unordered_set>
#include <unordered_map>

#include "IExcitationVector.hpp"

template <typename T>
class DOKExcitationVector : public IExcitationVector<T>
{
public:
    DOKExcitationVector(size_t num_rows);
    DOKExcitationVector(IExcitationVector<T>& base);

    DOKExcitationVector(const DOKExcitationVector& rhs) = delete;
    DOKExcitationVector& operator=(const DOKExcitationVector& rhs) = delete;
    DOKExcitationVector(DOKExcitationVector&& rhs) = delete;
    DOKExcitationVector&& operator=(DOKExcitationVector&& rhs) = delete;

    virtual void SetElement(size_t r, T& value);
    virtual void SetElementQuick(size_t r, T& value);

    virtual const T& GetElement(size_t r) const;
    virtual const T& GetElementQuick(size_t r) const;

    virtual size_t get_num_rows() { return num_rows_; }
    virtual size_t getnnz() { return map_.size(); }

    virtual std::set<std::pair<size_t, T>> GetNzElements();

private:
    const size_t num_rows_;

    std::unordered_map<size_t, T> map_;
};

#endif // DOKEXCITATIONVECTOR_H
