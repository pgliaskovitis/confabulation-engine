#ifndef DOKEXCITATIONVECTOR_H
#define DOKEXCITATIONVECTOR_H

#include <unordered_map>

#include "IExcitationVector.h"

template <typename T>
class DOKExcitationVector : public IExcitationVector<T>
{
public:
    DOKExcitationVector(size_t n_rows);
    DOKExcitationVector(IExcitationVector<T>& base);

    DOKExcitationVector(const DOKExcitationVector& rhs) = delete;
    DOKExcitationVector& operator=(const DOKExcitationVector& rhs) = delete;
    DOKExcitationVector(DOKExcitationVector&& rhs) = delete;
    DOKExcitationVector&& operator=(DOKExcitationVector&& rhs) = delete;

    virtual void SetElement(size_t r, T& value) = 0;
    virtual void SetElementQuick(size_t r, size_t c, T& value) = 0;

    virtual const T& GetElement(size_t l, size_t c) const = 0;
    virtual const T& GetElementQuick(size_t l, size_t c) const = 0;

    virtual size_t get_num_rows() { return num_rows_; }
    virtual size_t getnnz() { return map_.size(); }

private:
    const size_t num_rows_;

    std::unordered_map<size_t, T> map_;
};

#endif // DOKEXCITATIONVECTOR_H
