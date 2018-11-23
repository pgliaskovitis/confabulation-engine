/*
 * Copyright 2014 Periklis G. Liaskovitis
 *
 * This file is part of confab-engine.
 *
 * confab-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * confab-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with confab-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TemplateInstantiations.h"
#include "sparse_structures/DOKExcitationVector.hpp"
#include "sparse_structures/KHashExcitationVector.hpp"
#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/SparseHashLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"
#include "utils/HashTrieNode.hpp"
#include "utils/HashTrie.hpp"

// vector classes
template class DOKExcitationVector<float>;
template class DOKExcitationVector<uint16_t>;
template class KHashExcitationVector<float>;
template class KHashExcitationVector<uint16_t>;

// matrix classes
template class DOKLinksMatrix<uint32_t>;
template class SparseHashLinksMatrix<uint32_t>;
template class CSRLinksMatrix<float>;

// NLP classes
template class HashTrieNode<std::string>;
template class HashTrie<std::string>;
