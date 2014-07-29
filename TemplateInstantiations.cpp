#include "TemplateInstantiations.h"
#include "sparse_structures/DOKExcitationVector.hpp"
#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"

template class DOKExcitationVector<float>;
template class DOKExcitationVector<size_t>;
template class DOKExcitationVector<unsigned int>;

template class DOKLinksMatrix<float>;
template class DOKLinksMatrix<size_t>;

template class CSRLinksMatrix<float>;
