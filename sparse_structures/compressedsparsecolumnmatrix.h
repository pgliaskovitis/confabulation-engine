#ifndef COMPRESSEDSPARSECOLUMNMATRIX_H
#define COMPRESSEDSPARSECOLUMNMATRIX_H

#include <vector>

#include "MatrixInterface.h"

template <typename T>
class CompressedSparseColumnMatrix: public MatrixInterface<T>
{
public:

private:
    size_t num_rows_;
    size_t num_cols_;

    std::vector<T> a_;
    std::vector<size_t> ia_;
    std::vector<size_t> ja_;
};

#endif // COMPRESSEDSPARSECOLUMNMATRIX_H
