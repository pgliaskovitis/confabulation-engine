#include <list>
#include <algorithm>
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
        tries_.emplace_back(new HashTrie<std::string>());
        const std::set<Symbol>& level_symbols = mapping->GetAllSymbols();
        for (Symbol level_symbol : level_symbols)
            tries_.back()->Add(SymbolToVectorSymbol(level_symbol, ' '));
    }
}

std::vector<std::vector<std::string>> MultiLevelOrganizer::Organize(const std::vector<std::string>& symbols, bool& match_found)
{
    size_t n_levels = level_sizes_.size();
    std::vector<std::vector<std::string>> levels;
    levels.resize(n_levels);

    for (size_t i = 0; i < n_levels; ++i) {

        std::vector<std::string>& level = levels[i];
        level.resize(level_sizes_[i]);
        HashTrie<std::string>& trie = *(tries_.at(i));
        std::list<std::string> temp_symbols_list(symbols.begin(), symbols.end());

        // find longest match and remove matched symbols
        size_t j = 0;
        while (j < level.size() && !temp_symbols_list.empty()) {

            std::list<std::string> match = trie.FindLongest(temp_symbols_list);

            if (match.empty()) {
                match_found = false;
                return levels;
            }

            // store found multisymbol
            level[j] = ListSymbolToSymbol(temp_symbols_list, ' ');

            size_t end = std::min(level.size(), j + match.size());
            while (j < end) {
                temp_symbols_list.pop_front();
                ++j;
            }
        }
    }

    // this will return a 2D vector with activated symbols as follows:

    // level 2: multisymbol_21  ""          ""          ""              ""
    // level 1: multisymbol_11  ""          ""          multisymbol_12  ""
    // level 0: symbol_1        symbol_2    symbol_3    symbol_4        symbol_5

    return levels;
}
