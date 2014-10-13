/*
 * Copyright 2014 Periklis Liaskovitis
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

#ifndef SYMBOLATTRIBUTE_H_
#define SYMBOLATTRIBUTE_H_

#include <cstring>
#include <string>
#include <memory>
#include <map>

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

class SymbolAttribute
{
public:

	SymbolAttribute(float initial_source_excitation,
					float initial_target_excitation,
					std::unique_ptr<Synapse> initial_knowledge_link);

    float get_source_excitation();
    float get_target_excitation();

    void set_source_excitation(float source_excitation);
    void set_target_excitation(float target_excitation);

    Synapse& get_knowledge_link();

private:
	//total excitation of this symbol in the source module of the knowledge base
    float source_excitation_;

	//total excitation of this symbol in the target module of the knowledge base
    float target_excitation_;

    std::unique_ptr<Synapse> knowledge_link_;

	//operators
	SymbolAttribute(const SymbolAttribute& src);
	SymbolAttribute& operator=(const SymbolAttribute& rhs);
};

#endif /* SYMBOLATTRIBUTE_H_ */
