#include "MultiLevelOrganizer.h"
#include "utils/Utils.h"

MultiLevelOrganizer::MultiLevelOrganizer(const std::vector<unsigned short>& level_sizes,
                                         std::vector<std::unique_ptr<SymbolMapping>>&& level_mappings) :
    level_sizes_(level_sizes)
{
    // take control of all symbols
    for (std::unique_ptr<SymbolMapping>& level_mapping : level_mappings)
        level_mappings_.push_back(std::move(level_mapping));

    // organize symbols in HashTries
    for (const std::unique_ptr<SymbolMapping>& mapping : level_mappings_) {
        const std::set<Symbol>& level_symbols = mapping->GetAllSymbols();
        tries_.emplace_back(new HashTrie<std::string>());
        for (Symbol level_symbol : level_symbols)
            // from NGramHandler::GetAllSymbols()
            tries_.back()->Add(SymbolToVectorSymbol(level_symbol, ' '));
    }
}
