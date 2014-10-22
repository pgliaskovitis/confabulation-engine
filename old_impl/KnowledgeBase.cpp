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

#include "SymbolAttribute.h"
#include "KnowledgeBase.h"
#include "KnowledgeManager.h"
#include "Globals.h"
#include "SentenceTokenizer.h"

#include <algorithm>

const std::vector<Symbol> KnowledgeBase::kPersistenceDelimiters = {":::", "{--->", "|||", "}--->"};

KnowledgeBase::KnowledgeBase(const std::pair<unsigned short, unsigned short>& linked_modules, Globals& globals_manager) :
                                   base_id_(linked_modules),
                                   globals_manager_(globals_manager),
                                   knowledge_links_(new std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>, Symbol_Cmp>),
                                   target_symbol_occurrences_(new std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp>)
{}

void KnowledgeBase::Init()
{}

void KnowledgeBase::DeInit()
{}

std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>, Symbol_Cmp>& KnowledgeBase::get_knowledge_links()
{
    return *knowledge_links_;
}

std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp>& KnowledgeBase::get_target_symbol_occurences()
{
    return *target_symbol_occurrences_;
}

void KnowledgeBase::PersistRecallableKnowledge(std::ofstream& file)
{

    for (std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>, Symbol_Cmp>::iterator it1 = knowledge_links_->begin(); it1 != knowledge_links_->end(); ++it1) {
		//source symbol is kept fixed
        std::shared_ptr<Symbol> current_source_symbol = it1->first;

#ifndef PERSIST_NONALPHA_H_
        if (!SentenceTokenizer::IsSymbolAlphanumeric(*current_source_symbol))
			//std::cout << "Source symbol " << *currentSourceSymbol << " is not alphanumeric" << std::endl;
			continue;
#endif

        file << (*current_source_symbol) << KnowledgeBase::kPersistenceDelimiters[0];

        Synapse& current_synapse = (it1->second)->get_knowledge_link();

        for (Synapse::iterator it2 = current_synapse.begin(); it2 != current_synapse.end(); ++it2) {
			//target symbol is changing
            std::shared_ptr<Symbol> current_target_symbol = it2->first;

			//link strength is changing
            std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp>::iterator currentTargetSymbolOccurencesIt = target_symbol_occurrences_->find(it2->first);
            size_t current_target_symbol_occurences = 0;
            if (currentTargetSymbolOccurencesIt != target_symbol_occurrences_->end())
                current_target_symbol_occurences = currentTargetSymbolOccurencesIt->second;
            else {
                current_target_symbol_occurences = 1;
				std::cout << "Warning: Target symbol occurrences for symbol " << it2->first << " were not found!" << "\n";
			}

            file << (*current_target_symbol) << KnowledgeBase::kPersistenceDelimiters[1]
                 << it2->second << KnowledgeBase::kPersistenceDelimiters[2]
                 << current_target_symbol_occurences << KnowledgeBase::kPersistenceDelimiters[3];
		}

		file << std::endl;
	}

}

void KnowledgeBase::RecallPersistedKnowledge(std::ifstream& file)
{
	file.clear(file.goodbit);
	Symbol knowledgeFragment;

	file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
    try {
        if (file.is_open()) {
            while (file.good()) {
				std::getline(file, knowledgeFragment);
				std::replace(knowledgeFragment.begin(), knowledgeFragment.end(), '\n', ' ');
                ExtractKnowledgeTokens(knowledgeFragment);
			}
		}
    } catch (std::ifstream::failure& e) {
		if (!file.eofbit)
            std::cerr << "Exception opening/reading/closing file\n";
	}
}

void KnowledgeBase::ExtractKnowledgeTokens(const Symbol& input)
{
	SentenceTokenizer tok(input);
    const std::vector<Symbol>& knowledge_tokens = tok.KnowledgeTokenize(KnowledgeBase::kPersistenceDelimiters);

    if (knowledge_tokens.size() == 0)
        return;

#ifdef DEBUG_2_H_
	std::cout << "Read knowledge fragment: ";

    for (std::vector<Symbol>::const_iterator it = knowledge_tokens.begin(); it != knowledge_tokens.end(); ++it)
		std::cout << *it << "<-----";

	std::cout << std::endl;
#endif

	unsigned int i = 0;
    std::shared_ptr<Symbol> source_symbol_ptr;
    std::shared_ptr<Symbol> current_target_symbol_ptr;
    for (std::vector<Symbol>::const_iterator it = knowledge_tokens.begin(); it != knowledge_tokens.end(); ++it) {
        if (it == knowledge_tokens.begin()) {
			//perform initializations for the source symbol
            source_symbol_ptr = InitializeForNewSymbol(it);
#ifdef DEBUG_2_H_
            std::cout << "Found source symbol " << *source_symbol_ptr << std::endl;
#endif
        } else {
			unsigned int j = i % 3;
			switch(j) {
                case 0: {
					//allocate heap memory for the current target symbol
                    current_target_symbol_ptr = InitializeForNewSymbol(it);
#ifdef DEBUG_2_H_
            std::cout << "Found target symbol " << *current_target_symbol_ptr << std::endl;
#endif
					i++;
					break;
				}
                case 1: {
					//store partial target symbol occurrences
                    std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>, Symbol_Cmp>::iterator attribute_it = knowledge_links_->find(source_symbol_ptr);
                    if (attribute_it != knowledge_links_->end()) {
                        Synapse& current_synapse = attribute_it->second->get_knowledge_link();
						//STAGE 2.2: within the link set, introduce the pair corresponding to the target symbol, at n1
                        current_synapse.insert(std::pair<std::shared_ptr<Symbol>, size_t>(current_target_symbol_ptr, atoi((*it).c_str())));
#ifdef DEBUG_2_H_
                        std::cout << "Current synapse has " << current_synapse.size() << " targets, after addition of " << *current_target_symbol_ptr
								  << " with " << atoi((*it).c_str()) << " occurrences";
#endif
                    } else
                        std::cout << "\nSynapse not found for ----->" << *current_target_symbol_ptr;
                    i++;
					break;
				}
                case 2: {
					//store total target symbol occurrences
                    std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp>::iterator target_symbols_it = target_symbol_occurrences_->find(current_target_symbol_ptr);
                    if (target_symbols_it != target_symbol_occurrences_->end()) {
						// the occurrence counts should have been initialized to 0
                        target_symbols_it->second = atoi((*it).c_str());
#ifdef DEBUG_2_H_
                        std::cout << ", current target has " << target_symbols_it->second << " total occurrences" << std::endl;
#endif
                    } else
                        std::cout << "\nTarget symbol count not found for ----->" << *current_target_symbol_ptr;
					i = 0;
					break;
				}
			}
		}
	}
}

const std::shared_ptr<Symbol> KnowledgeBase::InitializeForNewSymbol(const std::vector<Symbol>::const_iterator& new_symbol)
{
    std::shared_ptr<Symbol> new_symbol_ptr = globals_manager_.get_knowledge_manager().AddSymbol(*new_symbol);

	//Initialize knowledge base with the new symbol as potential source symbol
	std::unique_ptr<Synapse> initialSynapse(new Synapse);
	float initialSourceExcitation = 0.0;
	float initialTargetExcitation = 0.0;
	std::unique_ptr<SymbolAttribute> initialSymbolAttribute(new SymbolAttribute(initialSourceExcitation, initialTargetExcitation, std::move(initialSynapse)));
    knowledge_links_->insert(std::pair<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>>(new_symbol_ptr, std::move(initialSymbolAttribute)));
    target_symbol_occurrences_->insert(std::pair<std::shared_ptr<Symbol>, size_t>(new_symbol_ptr, 0));

    return new_symbol_ptr;
}
