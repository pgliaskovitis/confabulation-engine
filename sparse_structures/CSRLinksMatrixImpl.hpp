#ifndef CSRLINKSMATRIXIMPL_H
#define CSRLINKSMATRIXIMPL_H

#include "Utils.h"
#include "CSRLinksMatrix.hpp"

template <typename T>
CSRLinksMatrix<T>::CSRLinksMatrix(size_t, num_rows, num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

#endif // CSRLINKSMATRIXIMPL_H
