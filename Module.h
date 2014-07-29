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
    Module(const SymbolMapping& symbol_mapping);

    void Reset();
    void ExcitationsToZero();

    void ActivateWord(const Symbol& word, unsigned int K);
    void AddToAllExcited(unsigned int K);
    void AddToIndex(size_t index, float value);

    void Freeze();
    void Unfreeze();
    bool IsFrozen() const { return frozen_indexes_ != nullptr; }

    IExcitationVector<float>& GetNormalizedExcitations();
    void AddExcitations(const IExcitationVector<float>& input);
    std::vector<Symbol> GetExpectation();

    Symbol ElementaryConfabulation();
    Symbol ElementaryConfabulation(unsigned int K);
    std::vector<Symbol> PartialConfabulation(unsigned int K, bool multiconf);

private:
    const SymbolMapping& symbol_mapping_;

    std::unique_ptr<IExcitationVector<float>> excitations_;
    std::unique_ptr<IExcitationVector<float>> normalized_excitations_;
    std::unique_ptr<IExcitationVector<size_t>> kb_inputs_;
    std::unique_ptr<std::set<size_t>> frozen_indexes_;
};

#endif // MODULE_H
