#ifndef SYMBOLMAPPING_H
#define SYMBOLMAPPING_H

#include <string>
#include <set>
#include <unordered_map>
#include <memory>

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

class SymbolMapping
{
private:
    std::unique_ptr<std::unordered_map<Symbol, std::unique_ptr<Symbol>>> all_symbols_;
    std::unique_ptr<std::unordered_map<Symbol*, size_t>> symbol_to_index_;
    std::unique_ptr<std::unordered_map<size_t, Symbol*>> index_to_symbol_;

public:
    SymbolMapping();
    SymbolMapping(const SymbolMapping& rhs) = delete;
    SymbolMapping& operator=(const SymbolMapping& rhs) = delete;
    SymbolMapping(SymbolMapping&& rhs) = delete;
    SymbolMapping&& operator=(SymbolMapping&& rhs) = delete;

    void AddSymbol(const Symbol& symbol);
    bool Contains(const Symbol& symbol);

    size_t IndexOf(const Symbol& symbol);
    Symbol GetSymbol(size_t index);

    size_t Size() { return symbol_to_index_->size(); }

    std::set<Symbol> GetAllSymbols();

    std::string ToString();
};

#endif // SYMBOLMAPPING_H
