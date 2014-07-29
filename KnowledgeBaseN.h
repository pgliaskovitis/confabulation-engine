#ifndef KNOWLEDGEBASEN_H
#define KNOWLEDGEBASEN_H

#include <string>
#include <memory>

#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"
#include "SymbolMapping.h"

class KnowledgeBaseN
{
public:
    KnowledgeBaseN(const std::string& id, std::unique_ptr<SymbolMapping>& src_map, std::unique_ptr<SymbolMapping>& targ_map);

    KnowledgeBaseN(const KnowledgeBaseN& rhs) = delete;
    KnowledgeBaseN& operator=(const KnowledgeBaseN& rhs) = delete;
    KnowledgeBaseN(KnowledgeBaseN&& rhs) = delete;
    KnowledgeBaseN&& operator=(KnowledgeBaseN&& rhs) = delete;

    void Add(const std::string& src_symbol, const std::string& targ_symbol);
    void Add(size_t targ_index, size_t src_index);
    void ComputeLinkStrengths();
    float GetPercentOfElementsLessThanThreshold(size_t threshold);
    std::unique_ptr<IExcitationVector<float>> Transmit(const IExcitationVector<float>& normalized_excitations) const;

    std::string get_id() const { return id_; }

    std::string GetStats() { return std::string("number of knowledge links: ") + std::to_string(GetNumKnowledgeLinks()); }
    size_t GetSizeSrc() { return cooccurrence_counts_->get_num_rows(); }
    size_t GetSizeTarg() { return cooccurrence_counts_->get_num_cols(); }
    size_t GetNumKnowledgeLinks() { return kbase_->GetNnz(); }

private:
    const std::string id_;

    static float ComputeLinkStrength(double antecedent_support_probability);

    // The knowledge base is essentially a matrix of conditional probabilities P(s | t)
    // Symbols s are providing the input excitations that pass through the knowledge base
    // In order to be able to transmit excitations with regular matrix multiplication
    // the source symbols must correspond to the COLUMNS of the matrix

    std::unique_ptr<SymbolMapping> src_map_;
    std::unique_ptr<SymbolMapping> targ_map_;
    std::unique_ptr<IKnowledgeLinks<size_t>> cooccurrence_counts_;
    std::unique_ptr<IKnowledgeLinks<float>> kbase_;
    std::vector<size_t> target_symbol_sums_;
};

#endif // KNOWLEDGEBASEN_H
