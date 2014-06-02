/*
 * KnowledgeManager.h
 *
 *  Created on: Jun 1, 2013
 *      Author: Periklis G. Liaskovitis
 */

#ifndef KNOWLEDGEMANAGER_H_
#define KNOWLEDGEMANAGER_H_

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <memory>
#include <limits>
#include <algorithm>
#include <math.h>

#include <vector>
#include <set>
#include <map>
#include <queue>

//forward declarations
class KnowledgeBase;
class Globals;

#ifndef DEBUG_1_H_
//#define DEBUG_1_H_
#endif

#ifndef CONFAB_H_
//#define CONFAB_H_
#endif

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

#ifndef POINTERKEYCOMP_H_
#define POINTERKEYCOMP_H_
struct Symbol_Cmp
{
   bool operator()(std::shared_ptr<Symbol> a, std::shared_ptr<Symbol> b)
   {
      return strcmp(a->c_str(), b->c_str()) < 0;
   }
};
#endif

#ifndef SYNAPSE_H_
#define SYNAPSE_H_
//a function of symbol co-occurences, with the attribute symbol at the receiving end (as target)
typedef std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp> Synapse;
#endif

#ifndef CLEAR_WEAK_LINKS_H_
//#define CLEAR_WEAK_LINKS_H_
#endif

struct Excit_Cmp
{
	bool operator()(std::pair<float, Symbol>& a, std::pair<float, Symbol>& b)
	{
		return (a.first < b.first);
	}
};

//a function of final symbol excitations
typedef std::map<std::shared_ptr<Symbol>, float, Symbol_Cmp> Excitation;

class KnowledgeManager
{
public:

    KnowledgeManager(Globals& globals_manager);

    void Init();

    void DeInit();

    void EnrichKnowledge(const std::vector<Symbol>& sentence);

    const std::vector<Symbol> GetLayeredConfabulatedSymbol(const std::vector<Symbol>& sentence) const;

    const std::vector<std::pair<float, Symbol>> GetConfabulatedSymbol(const std::vector<Symbol>& sentence) const;

    std::shared_ptr<Symbol> AddSymbol(const Symbol& symbol);

    std::shared_ptr<Symbol> GetSymbolPtr(const Symbol& symbol) const;

    void PersistRecallableKnowledge();

    void RecallPersistedKnowledge();

private:
    Globals& globals_manager_;

	//each element of this vector is an assortment of the symbols comprising a module,
	//with different module types at different levels of the confabulation architecture
    std::unique_ptr<std::map<Symbol, std::shared_ptr<Symbol>>> all_symbols_;

    //2-D structure of actual knowledge base matrices, indexed by the above IDs
    std::vector<std::vector<std::unique_ptr<KnowledgeBase>>> all_knowledge_bases_;
};

#endif /* KNOWLEDGEMANAGER_H_ */
