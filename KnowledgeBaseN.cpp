#include "KnowledgeBaseN.h"

KnowledgeBaseN::KnowledgeBaseN(const std::string& id, std::unique_ptr<SymbolMapping>& src_map, std::unique_ptr<SymbolMapping>& targ_map) :
    id_(id),
    src_map_(std::move(src_map)),
    targ_map_(std::move(targ_map))
{}

void KnowledgeBaseN::Add(const std::string& src_symbol, const std::string& targ_symbol)
{
    size_t row = src_map_->IndexOf(src_symbol);
    size_t col = targ_map_->IndexOf(targ_symbol);

    Add(row, col);
}

void KnowledgeBaseN::Add(size_t src_symbol, size_t targ_symbol)
{
    kbase_.reset(nullptr); // discard out of date version
    target_symbol_count_[targ_symbol]++;
    size_t previous_count = cooccurrence_counts_->GetElement(src_symbol, targ_symbol);
    cooccurrence_counts_->SetElement(src_symbol, targ_symbol, previous_count + 1);
}

void KnowledgeBaseN::ComputeLinkStrengths()
{
     std::unique_ptr<DOKLinksMatrix<float>> link_strengths(
                 new DOKLinksMatrix<float>(cooccurrence_counts_->get_num_rows(), cooccurrence_counts_->get_num_cols()));
//    for (Entry<Pair<Integer, Integer>, Integer> e : cooccurrence_counts
//            .nz_elements()) {

//        Pair<Integer, Integer> coord = e.getKey();
//        Integer l = coord.first;
//        Integer c = coord.second;

//        link_strengths.set(l, c, link_strength(e.getValue()
//                / (float) row_count[l]));
//    }

//    // store them in an more static but much more efficient format
//    kbase_.reset(new CSRLinksMatrix<float>(link_strengths));

}
