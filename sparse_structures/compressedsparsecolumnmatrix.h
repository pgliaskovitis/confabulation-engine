#ifndef COMPRESSEDSPARSECOLUMNMATRIX_H
#define COMPRESSEDSPARSECOLUMNMATRIX_H

#include <vector>

#include "IKnowledgeLinks.h"

template <typename T>
class CompressedSparseColumnMatrix: public IKnowledgeLinks<T>
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
