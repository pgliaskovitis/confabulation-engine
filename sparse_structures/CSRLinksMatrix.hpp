#ifndef CSRLINKSMATRIX_H
#define CSRLINKSMATRIX_H

#include <vector>

#include "IKnowledgeLinks.hpp"

template <typename T>
class CSRLinksMatrix: public IKnowledgeLinks<T>
{
public:
    CSRLinksMatrix(size_t, num_rows, num_cols);
    CSRLinksMatrix(IKnowledgeLinks<T>& base);

    CSRLinksMatrix(const CSRLinksMatrix& rhs) = delete;
    CSRLinksMatrix& operator=(const CSRLinksMatrix& rhs) = delete;
    CSRLinksMatrix(CSRLinksMatrix&& rhs) = delete;
    CSRLinksMatrix&& operator=(CSRLinksMatrix&& rhs) = delete;

    virtual void SetElement(size_t r, size_t c, T& value) { /*not supported*/ }
    virtual void SetElementQuick(size_t r, size_t c, T& value) { /*not supported*/ }

    virtual const T& GetElement(size_t r, size_t c) const;
    virtual const T& GetElementQuick(size_t r, size_t c) const;

    virtual size_t get_num_rows() { return num_rows_; }
    virtual size_t get_num_cols() { return num_cols_; }

    virtual size_t GetNnz() { return a_.size(); }

    virtual IExcitationVector<T> multiply(IExcitationVector<T>& vec);
    virtual std::set<std::pair<std::pair<size_t, size_t>, T>> GetNzElements() const;

private:
    const size_t num_rows_;
    const size_t num_cols_;

    std::vector<T> a_;
    std::vector<size_t> ia_;
    std::vector<size_t> ja_;
};

#endif // CSRLINKSMATRIX_H
