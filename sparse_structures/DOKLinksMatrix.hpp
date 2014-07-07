#ifndef DOKLINKSMATRIX_H
#define DOKLINKSMATRIX_H

#include <map>
#include "DOKExcitationVector.hpp"
#include "IKnowledgeLinks.hpp"

template <typename T>
class DOKLinksMatrix : public IKnowledgeLinks<T>
{
public:
    DOKLinksMatrix(size_t num_rows, size_t num_cols);
    DOKLinksMatrix(IKnowledgeLinks<T>& base);

    DOKLinksMatrix(const DOKLinksMatrix& rhs) = delete;
    DOKLinksMatrix& operator=(const DOKLinksMatrix& rhs) = delete;
    DOKLinksMatrix(DOKLinksMatrix&& rhs) = delete;
    DOKLinksMatrix&& operator=(DOKLinksMatrix&& rhs) = delete;

    virtual void SetElement(size_t r, size_t c, T& value);
    virtual void SetElementQuick(size_t r, size_t c, T& value);

    virtual const T& GetElement(size_t r, size_t c) const;
    virtual const T& GetElementQuick(size_t r, size_t c) const;

    virtual size_t get_num_rows() { return num_rows_; }
    virtual size_t get_num_cols() { return num_cols_; }

    virtual size_t GetNnz() { return map_.size(); }

    virtual std::unique_ptr<IExcitationVector<T>> multiply(const IExcitationVector<T>& vec) const;
    virtual std::set<std::pair<std::pair<size_t, size_t>, T>> GetNzElements() const;

private:
    const size_t num_rows_;
    const size_t num_cols_;

    std::map<std::pair<size_t, size_t>, T> map_;
};

template <typename T>
DOKLinksMatrix<T>::DOKLinksMatrix(size_t num_rows, size_t num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename T>
DOKLinksMatrix<T>::DOKLinksMatrix(IKnowledgeLinks<T> &base) : num_rows_(base.get_num_rows()), num_cols_(base.get_num_cols())
{
    for (const std::pair<std::pair<size_t, size_t>, T>& element : base.GetNzElements())
        map_.insert(element);
}

template <typename T>
const T& DOKLinksMatrix<T>::GetElement(size_t r, size_t c) const
{
    IKnowledgeLinks<T>::CheckBounds(r, c);
    return GetElementQuick(r, c);
}

template <typename T>
const T& DOKLinksMatrix<T>::GetElementQuick(size_t r, size_t c) const
{
    T result;

    try {
        result = map_.at(std::make_pair(r, c));
    } catch (const std::out_of_range& oor) {
        result = 0;
    }

    return result;
}

template <typename T>
void DOKLinksMatrix<T>::SetElement(size_t r, size_t c, T& value)
{
    IKnowledgeLinks<T>::CheckBounds(r, c);
    if (IsNearlyEqual(value, 0)) {
        typename std::map<std::pair<size_t, size_t>, T>::iterator it = map_.find(std::make_pair(r, c));
        if (it != map_.end())
            map_.erase(it);
    } else
        SetElementQuick(r, c, value);
}

template <typename T>
void DOKLinksMatrix<T>::SetElementQuick(size_t r, size_t c, T& value)
{
    map_[std::make_pair(r, c)] = value;
}

template <typename T>
std::unique_ptr<IExcitationVector<T>> DOKLinksMatrix<T>::multiply(const IExcitationVector<T>& vec) const
{
    std::set<std::pair<size_t, T>> vec_elements = vec.GetNzElements();

    T link_strength = 0;
    std::unique_ptr<IExcitationVector<T>> result(new DOKExcitationVector<T>(num_rows_));

    for (const std::pair<size_t, T>& element : vec_elements) {
        size_t c = element.first;
        for (size_t r = 0; r < num_rows_; ++r) {
            link_strength = GetElementQuick(r, c);
            if (!IsNearlyEqual(link_strength, 0)) {
                result->SetElement(r, result->GetElementQuick(r) + link_strength * element.second);
            }
        }
    }

    return result;
}

template <typename T>
std::set<std::pair<std::pair<size_t, size_t>, T>> DOKLinksMatrix<T>::GetNzElements() const
{
    typename std::set<std::pair<std::pair<size_t, size_t>, T>> result;

    for (typename std::map<std::pair<size_t, size_t>, T>::const_iterator it = map_.begin(); it != map_.end(); ++it)
        result.insert(*it);

    return result;
}

#endif // DOKLINKSMATRIX_H
