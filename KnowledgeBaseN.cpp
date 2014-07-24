#include "KnowledgeBaseN.h"

KnowledgeBaseN::KnowledgeBaseN(const std::string& id, std::unique_ptr<SymbolMapping>& src_map, std::unique_ptr<SymbolMapping>& targ_map) :
    id_(id),
    src_map_(std::move(src_map)),
    targ_map_(std::move(targ_map))
{}
