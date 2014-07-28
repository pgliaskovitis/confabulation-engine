#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>
#include <set>

#include <sparse_structures/IExcitationVector.hpp>
#include <SymbolMapping.h>

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

class Module
{
public:
    Module(SymbolMapping symbol_mapping);

    void reset();
    void excitations_to_zero();
    void activate_word(Symbol word, int K);
    void add_to_all_excited(int K);
    void add_to_index(int index, float value);
    Symbol elementary_confabulation();
    Symbol elementary_confabulation(int K);
    std::vector<Symbol> partial_confabulation(int K, bool multiconf);
    void freeze();
    void unfreeze();
    bool is_frozen();
    std::unique_ptr<IExcitationVector<float>> getNormalised_excitations();
    void add_excitations(const IExcitationVector<float>& input);
    std::vector<Symbol> get_expectation();

private:
    std::unique_ptr<IExcitationVector<float>> excitations_;
    std::unique_ptr<IExcitationVector<float>> normalised_excitations_;
    std::unique_ptr<IExcitationVector<size_t>> kb_inputs_;
    std::vector<size_t> frozen_indexes;
    std::unique_ptr<SymbolMapping> symbol_mapping_;
};

#endif // MODULE_H
