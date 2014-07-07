#ifndef CSRLINKSMATRIX_H
#define CSRLINKSMATRIX_H

#include <vector>

#include "DOKExcitationVector.hpp"
#include "IKnowledgeLinks.hpp"

template <typename T>
class CSRLinksMatrix : public IKnowledgeLinks<T>
{
public:
    CSRLinksMatrix(size_t num_rows, size_t num_cols);
    CSRLinksMatrix(IKnowledgeLinks<T>& base);

    CSRLinksMatrix(const CSRLinksMatrix& rhs) = delete;
    CSRLinksMatrix& operator=(const CSRLinksMatrix& rhs) = delete;
    CSRLinksMatrix(CSRLinksMatrix&& rhs) = delete;
    CSRLinksMatrix&& operator=(CSRLinksMatrix&& rhs) = delete;

    virtual void SetElement(size_t r, size_t c, T& value) { (void)r; (void)c; (void)value; /*not supported*/ }
    virtual void SetElementQuick(size_t r, size_t c, T& value) { (void)r; (void)c; (void)value; /*not supported*/ }

    virtual const T& GetElement(size_t r, size_t c) const;
    virtual const T& GetElementQuick(size_t r, size_t c) const;

    virtual size_t get_num_rows() { return num_rows_; }
    virtual size_t get_num_cols() { return num_cols_; }

    virtual size_t GetNnz() { return a_.size(); }

    virtual std::unique_ptr<IExcitationVector<T>> multiply(const IExcitationVector<T>& vec) const;
    virtual std::set<std::pair<std::pair<size_t, size_t>, T>> GetNzElements() const;

private:
    const size_t num_rows_;
    const size_t num_cols_;

    std::vector<T> a_;
    std::vector<size_t> ia_;
    std::vector<size_t> ja_;
};

template <typename T>
CSRLinksMatrix<T>::CSRLinksMatrix(size_t num_rows, size_t num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename T>
CSRLinksMatrix<T>::CSRLinksMatrix(IKnowledgeLinks<T> &base) : num_rows_(base.get_num_rows()), num_cols_(base.get_num_cols())
{
    a_.resize(base.GetNnz());
    ja_.resize(base.GetNnz());

    int index_a = 0;
    // data begins at 0 in A
    ia_.push_back(index_a);
    size_t row = 0;

    for (const std::pair<std::pair<size_t, size_t>, T>& element : base.GetNzElements()) {
        // advance in lines until the row of element
        while (row != element.first.first) {
            ia_.push_back(index_a);
            ++row;
        }

        // insert element's value and column in A and JA
        a_[index_a] = element.second;
        ja_[index_a] = element.first.second;
        ++index_a;
    }

    // end of non-zero elements: fill remaining IA indexes
    for (; row < num_rows_; ++row) {
        ia_.push_back(index_a);
    }
}

template <typename T>
const T& CSRLinksMatrix<T>::GetElement(size_t r, size_t c) const
{
    IKnowledgeLinks<T>::CheckBounds(r, c);
    return GetElementQuick(r, c);
}

template <typename T>
const T& CSRLinksMatrix<T>::GetElementQuick(size_t r, size_t c) const
{
    std::vector<size_t>::const_iterator begin_it = ja_.begin() + ia_[r];
    std::vector<size_t>::const_iterator end_it = ja_.begin() + ia_[r + 1];

    const size_t index = BinarySearch(begin_it, end_it, c);

    if (IsNearlyEqual(a_[index], c))
        return a_[index];
    else
        return 0;
}

template <typename T>
std::unique_ptr<IExcitationVector<T>> CSRLinksMatrix<T>::multiply(const IExcitationVector<T>& vec) const
{
    std::unique_ptr<IExcitationVector<T>> result(new DOKExcitationVector<T>(num_rows_));
    T row_sum;

    for (size_t r = 0; r < num_rows_; ++r) {
        row_sum = 0;
        for (size_t i = ia_[r]; i < ia_[r + 1]; ++i) {
            row_sum += a_[i] * vec.GetElement(ja_[i]);
        }
        result->SetElement(r, row_sum);
    }

    return result;
}

template <typename T>
std::set<std::pair<std::pair<size_t, size_t>, T>> CSRLinksMatrix<T>::GetNzElements() const
{
    typename std::set<std::pair<std::pair<size_t, size_t>, T>> result;

    for (size_t r = 0; r < num_rows_; ++r)
        for (size_t i = ia_[r]; i < ia_[r + 1]; ++i)
            result.insert(std::make_pair(std::make_pair(r, ja_[i]), a_[i]));

    return result;
}

#endif // CSRLINKSMATRIX_H
