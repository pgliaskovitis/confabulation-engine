#ifndef MULTILEVELORGANIZER_H
#define MULTILEVELORGANIZER_H

#include <vector>
#include <memory>
#include "SymbolMapping.h"
#include <utils/HashTrie.hpp>

class MultiLevelOrganizer
{
public:
    MultiLevelOrganizer(const std::vector<unsigned short>& level_sizes_,
                        std::vector<std::unique_ptr<SymbolMapping>>&& level_mappings);

    MultiLevelOrganizer(const MultiLevelOrganizer& rhs) = delete;
    MultiLevelOrganizer& operator=(const MultiLevelOrganizer& rhs) = delete;
    MultiLevelOrganizer(MultiLevelOrganizer&& rhs) = delete;
    MultiLevelOrganizer&& operator=(MultiLevelOrganizer&& rhs) = delete;

private:
    std::vector<unsigned short> level_sizes_;
    std::vector<std::unique_ptr<SymbolMapping>> level_mappings_;
    std::vector<std::unique_ptr<HashTrie<std::string>>> tries_;
};

#endif // MULTILEVELORGANIZER_H
