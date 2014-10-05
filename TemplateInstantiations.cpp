#include "TemplateInstantiations.h"
#include "sparse_structures/DOKExcitationVector.hpp"
#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"
#include "utils/HashTrie.hpp"

template class DOKExcitationVector<float>;
template class DOKExcitationVector<size_t>;
template class DOKExcitationVector<int>;

template class DOKLinksMatrix<float>;
template class DOKLinksMatrix<size_t>;

template class CSRLinksMatrix<float>;

template class HashTrie<std::string>;
