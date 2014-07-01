#ifndef DOKEXCITATIONVECTOR_H
#define DOKEXCITATIONVECTOR_H

#include <unordered_set>
#include <unordered_map>

#include "IExcitationVector.hpp"

template <typename T>
class DOKExcitationVector : public IExcitationVector<T>
{
public:
    DOKExcitationVector(const size_t num_rows);
    DOKExcitationVector(const IExcitationVector<T>& base);

    DOKExcitationVector(const DOKExcitationVector& rhs) = delete;
    DOKExcitationVector& operator=(const DOKExcitationVector& rhs) = delete;
    DOKExcitationVector(DOKExcitationVector&& rhs) = delete;
    DOKExcitationVector&& operator=(DOKExcitationVector&& rhs) = delete;

    virtual void SetElement(const size_t r, const T& value);
    virtual void SetElementQuick(const size_t r, const T& value);

    virtual const T& GetElement(const size_t r) const;
    virtual const T& GetElementQuick(const size_t r) const;

    virtual size_t get_num_rows() { return num_rows_; }

    virtual size_t GetNnz() { return map_.size(); }

    virtual void Add(const IExcitationVector<T>& other);
    virtual std::set<std::pair<size_t, T>> GetNzElements() const;

private:
    const size_t num_rows_;

    std::unordered_map<size_t, T> map_;
};

#endif // DOKEXCITATIONVECTOR_H
