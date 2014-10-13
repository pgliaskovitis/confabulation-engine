/*
 * Copyright 2014 Periklis G. Liaskovitis
 *
 * This file is part of confab-engine.
 *
 * confab-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * confab-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with confab-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KNOWLEDGEBASE_H_
#define KNOWLEDGEBASE_H_

#include <iostream>
#include <fstream>
#include <string>

#include <memory>

#include <vector>
#include <map>

//forward declarations
class SymbolAttribute;
class RecallableSymbolAttribute;
class Globals;

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
      return std::strcmp(a->c_str(), b->c_str()) < 0;
   }
};
#endif

#ifndef DEBUG_2_H_
//#define DEBUG_2_H_
#endif

#ifndef PERSIST_NONALPHA_H_
//#define PERSIST_NONALPHA_H_
#endif

//definition of a knowledge base head node, which differs from mere synapses
class KnowledgeBase
{
public:

    static const std::vector<Symbol> kPersistenceDelimiters;

    KnowledgeBase(const std::pair<unsigned short, unsigned short>& linked_modules, Globals& globals_manager);

    void Init();

    void DeInit();

    std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>, Symbol_Cmp>& get_knowledge_links();

    std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp>& get_target_symbol_occurences();

    void PersistRecallableKnowledge(std::ofstream& l_file);

    void RecallPersistedKnowledge(std::ifstream& l_file);

private:
	//<sourceModuleID, targetModuleID>
    const std::pair<unsigned short, unsigned short> base_id_;

    Globals& globals_manager_;

	//tree structure holding counts for symbol co-occurence counts
	//where the first symbol is in the source module
	//and the second symbol is in the target module
    std::unique_ptr<std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>, Symbol_Cmp>> knowledge_links_;

    //vector holding cumulative counts for symbols of the target module in this knowledge base
    //if the knowledge base is thought of as a sparse matrix
    //this vector holds the column sums
    std::unique_ptr<std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp>> target_symbol_occurrences_;

    void ExtractKnowledgeTokens(const Symbol& input);

    const std::shared_ptr<Symbol> InitializeForNewSymbol(const std::vector<Symbol>::const_iterator& new_symbol);

};

#endif /* KNOWLEDGEBASE_H_ */
