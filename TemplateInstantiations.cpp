#include "TemplateInstantiations.h"
#include "sparse_structures/DOKExcitationVector.hpp"
#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"

template class DOKExcitationVector<float>;
template class DOKLinksMatrix<float>;
template class CSRLinksMatrix<float>;
