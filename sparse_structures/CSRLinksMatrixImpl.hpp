#ifndef CSRLINKSMATRIXIMPL_H
#define CSRLINKSMATRIXIMPL_H

#include "Utils.h"
#include "CSRLinksMatrix.hpp"

template <typename T>
CSRLinksMatrix<T>::CSRLinksMatrix(size_t, num_rows, num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename T>
CSRLinksMatrix<T>::CSRLinksMatrix(IKnowledgeLinks<T> &base) : num_rows_(base.get_num_rows()), num_cols_(base.get_num_cols())
{
    a_.resize(base.GetNnz());
    ja_.resize(base.GetNnz());

    int index_a = 0;
    // data begins at 0 in A
    ia_.push_back(index_a);
    int row = 0;

    for (const std::pair<std::pair<size_t, size_t>, T>& element : base.GetNzElements()) {
        // advance in lines until the row of element
        while (row != element.first) {
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

#endif // CSRLINKSMATRIXIMPL_H
