/*
 * KnowledgeManager.cpp
 *
 *  Created on: Jun 1, 2013
 *      Author: Periklis G. Liaskovitis
 */

#include "SymbolAttribute.h"
#include "KnowledgeBase.h"
#include "KnowledgeManager.h"
#include "Globals.h"
#include "SentenceTokenizer.h"

KnowledgeManager::KnowledgeManager(Globals& globals_manager) : globals_manager_(globals_manager),
                                                               all_symbols_(new std::map<Symbol, std::shared_ptr<Symbol>>)
{}

void KnowledgeManager::Init()
{

    try {
        // Simple confabulation architecture with a horizontal layer n identical modules
        all_knowledge_bases_.resize(Globals::kLevelSize - 1);

        for (unsigned short i = 0; i < Globals::kLevelSize - 1; i++) {

            all_knowledge_bases_[i].resize(Globals::kLevelSize - (i + 1));

            for (unsigned short j = i + 1; j <= Globals::kLevelSize - 1; j++) {
				//define the modules
                std::pair<unsigned short, unsigned short> current_symbol_id(i, j);

				//initialize the knowledge bases
                std::unique_ptr<KnowledgeBase> current_knowledge_base(new KnowledgeBase(current_symbol_id, globals_manager_));
#ifdef DEBUG_1_H_
				std::cout << "Initializing knowledge base (" << i << ", " << j << ") at index (" << i << ", " << j - (i + 1) << ")\n";
#endif
				//currentKnowledgeBase->init();
                all_knowledge_bases_[i][j - (i + 1)] = std::move(current_knowledge_base);
			}
		}
    } catch (int e) {
		std::cerr << "Exception while initializing confabulation architecture";
	}

}

void KnowledgeManager::DeInit()
{
}

std::shared_ptr<Symbol> KnowledgeManager::AddSymbol(const Symbol& symbol)
{
    if (all_symbols_->find(symbol) == all_symbols_->end()) {
		//Add new symbol to symbol list
        std::shared_ptr<Symbol> new_symbol_ptr(new Symbol(symbol));
        all_symbols_->insert(std::pair<Symbol, std::shared_ptr<Symbol>>(*new_symbol_ptr, new_symbol_ptr));
        return new_symbol_ptr;
	}

	return GetSymbolPtr(symbol);

}

std::shared_ptr<Symbol> KnowledgeManager::GetSymbolPtr(const Symbol& symbol) const
{
    std::map<Symbol, std::shared_ptr<Symbol>>::iterator it = all_symbols_->find(symbol);
    if (it != all_symbols_->end())
		return it->second;

    return nullptr;
}

void KnowledgeManager::EnrichKnowledge(const std::vector<Symbol>& sentence)
{

    for (size_t n1 = 0; n1 < sentence.size(); n1++) {
		//STAGE 1:
		//Check if all symbols actually exist in the module and add them, with the necessary initializations if they don't

        std::shared_ptr<Symbol> next_symbol_ptr = AddSymbol(sentence[n1]);
        for (unsigned short i = 0; i < Globals::kLevelSize - 1; i++) {
            for (unsigned short j = i + 1; j <= Globals::kLevelSize - 1; j++) {
				//Initialize all knowledge bases with the new symbol as potential source symbol
				std::unique_ptr<Synapse> initialSynapse(new Synapse);
				float initialSourceExcitation = 0.0;
				float initialTargetExcitation = 0.0;
				std::unique_ptr<SymbolAttribute> initialSymbolAttribute(new SymbolAttribute(initialSourceExcitation, initialTargetExcitation, std::move(initialSynapse)));
                all_knowledge_bases_[i][j - (i + 1)]->get_knowledge_links().insert(std::pair<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>>(next_symbol_ptr, std::move(initialSymbolAttribute)));
#ifdef DEBUG_1_H_
                std::cout << "Knowledge Base (" << i << ", " << j << ")" << " now has " << all_knowledge_bases_[i][j - (i + 1)]->get_knowledge_links().size() << " source symbols (" << *nextSymbolPtr << ")\n";
#endif
                all_knowledge_bases_[i][j - (i + 1)]->get_target_symbol_occurences().insert(std::pair<std::shared_ptr<Symbol>, size_t>(next_symbol_ptr, 0));
			}
		}
#ifdef DEBUG_1_H_
		std::cout << "\n";
#endif

		//STAGE 2:
		//Use a sliding window and add or update appropriate knowledge links to the latest read target symbol
		//n1 is head of window
		//n2 is tail of window
		size_t n2;
        n1 < Globals::kLevelSize ? n2 = 0 : n2 = n1 - Globals::kLevelSize + 1; //(n1 - n2 = Globals::levelSize - 1)

		//std::cout << "Cursor's at " << n2 << "->" << n1 << "\n";
        for (unsigned short k = 0; k < n1 - n2; k++) {

			//std::shared_ptr<Symbol> previousSymbolPtr(new Symbol(sentence[k + n2]));
            std::shared_ptr<Symbol> previous_symbol_ptr = GetSymbolPtr(sentence[k + n2]);
#ifdef DEBUG_1_H_
            std::cout << "About to create knowledge link ----" << *previous_symbol_ptr << "=>" << *next_symbol_ptr << "----"
					  << " in knowledge base (" << k << ", " << n1 - n2 << ")";
#endif
			//STAGE 2.1: find the link set emanating from the source symbol, at k + n2
            std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>, Symbol_Cmp>::iterator attribute_it = all_knowledge_bases_[k][n1 - n2 - (k + 1)]->get_knowledge_links().find(previous_symbol_ptr);
            if (attribute_it != all_knowledge_bases_[k][n1 - n2 - (k + 1)]->get_knowledge_links().end()) {
                Synapse& current_synapse = attribute_it->second->get_knowledge_link();
#ifdef DEBUG_1_H_
                std::cout << "\nCurrent synapse has " << current_synapse.size() << " targets";
#endif

				//STAGE 2.2: within the link set, update or introduce the pair corresponding to the target symbol, at n1
                Synapse::iterator links_it = current_synapse.find(next_symbol_ptr);
                if (links_it != current_synapse.end()) {
                    links_it->second = links_it->second + 1;
#ifdef DEBUG_1_H_
                    std::cout << ", link strength for target \"" << *next_symbol_ptr << "\" becomes " << linksIt->second << "\n";
#endif
                } else {
                    current_synapse.insert(std::pair<std::shared_ptr<Symbol>, size_t>(next_symbol_ptr, 1));
#ifdef DEBUG_1_H_
                    std::cout << ", link strength for target \"" << *next_symbol_ptr << "\" initializes at 1\n";
#endif
				}
            } else {
                std::cout << "\nSynapse not found for ----->" << *next_symbol_ptr << "<----- in knowledge base (" << k << ", " << n1 - n2 << ")\n";
			}

			//STAGE 2.3: within the target symbol occurrence set, update or introduce the count corresponding to the target symbol, at n1
            std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp>::iterator target_symbols_it = all_knowledge_bases_[k][n1 - n2 - (k + 1)]->get_target_symbol_occurences().find(next_symbol_ptr);
            if (target_symbols_it != all_knowledge_bases_[k][n1 - n2 - (k + 1)]->get_target_symbol_occurences().end()) {
				// the occurrence counts should have been initialized to 0
                target_symbols_it->second = target_symbols_it->second + 1;
#ifdef DEBUG_1_H_
                std::cout << "Total count for symbol \"" << *next_symbol_ptr << "\" as target in knowledge base (" << k << ", " << n1 - n2 << ")"
                          << " becomes " << target_symbols_it->second << "\n\n";
#endif
            } else {
                std::cout << "Target symbol count not found for ----->" << *next_symbol_ptr << "<----- in knowledge base (" << k << ", " << n1 - n2 << ")\n\n";
			}
		}
	}

}

const std::vector<Symbol> KnowledgeManager::GetLayeredConfabulatedSymbol(const std::vector<Symbol>& sentence) const
{
	std::priority_queue<std::pair<float, Symbol>,
						std::vector<std::pair<float, Symbol>>,
                        Excit_Cmp> max_heap;

    for (unsigned short i = 0; i < Globals::kLevelSize - 1; i++) {
		std::vector<Symbol> buffer;
		int k = sentence.size() - i - 1;
        if (k >= 0) {
			for (unsigned short j = sentence.size() - i - 1; j <= sentence.size() - 1; j++)
                buffer.push_back(sentence[j]);

            std::vector<std::pair<float, Symbol>> partial_output = GetConfabulatedSymbol(buffer);
            for (std::vector<std::pair<float, Symbol>>::iterator it = partial_output.begin(); it != partial_output.end(); ++it) {
                max_heap.push(*it);
				std::cout << "Candidate: (" << it->second << ", " << it->first << "), ";
			}
			std::cout << std::endl;
        } else {
            for (unsigned short j = 0; j <= sentence.size() - 1 - k; j++) {
				if (j < -k)
                    buffer.push_back("DamnedIfIKnow");
                else
					buffer.push_back(sentence[j + k]);
			}
            std::vector<std::pair<float, Symbol>> partial_output = GetConfabulatedSymbol(buffer);
            for (std::vector<std::pair<float, Symbol>>::iterator it = partial_output.begin(); it != partial_output.end(); ++it) {
                max_heap.push(*it);
				std::cout << "Candidate: (" << it->second << ", " << it->first << "), ";
			}
			std::cout << std::endl;

		}
	}

	std::vector<Symbol> output;
    unsigned short k = 0;
    while (!max_heap.empty() && (k < Globals::kHeapResults))	{
        if (max_heap.top().second != "") {
            output.push_back(max_heap.top().second);
            k++;
        }
        max_heap.pop();
	}

	return output;
}

const std::vector<std::pair<float, Symbol>> KnowledgeManager::GetConfabulatedSymbol(const std::vector<Symbol>& sentence) const
{

	std::cout << "Confabulate: ";
	for (std::vector<Symbol>::const_iterator it = sentence.begin(); it != sentence.end(); ++it)	{
		std::cout << *it << "----->";
	}
	std::cout << "\n";

    bool large_sentence = false;
    unsigned short target_module_id;
    if (sentence.size() >= Globals::kLevelSize)	{
        large_sentence = true;
        target_module_id = Globals::kLevelSize - 1;
    } else {
        target_module_id = sentence.size();
	}

	//STAGE I: Annul excitation levels of all knowledge bases and of output map
    Excitation output_map;
    for (unsigned short i = 0; i < Globals::kLevelSize - 1; i++) {
        for (unsigned short j = i + 1; j <= Globals::kLevelSize - 1; j++) {
            const std::unique_ptr<KnowledgeBase>& current_knowledge_base = all_knowledge_bases_[i][j - (i + 1)];
            for(std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>>::iterator it = current_knowledge_base->get_knowledge_links().begin(); it != current_knowledge_base->get_knowledge_links().end(); ++it) {
                it->second->set_source_excitation(0.0);
                it->second->set_target_excitation(0.0);
			}
		}
	}

	//STAGE II: Find knowledgeBases that have the target module (the one that needs to be confabulated) as target
	//This should be static and known by the architecture itself

	/*
	 * I(t) = Sum_source_modules { Sum_source_symbols I(s) * [(ln(p( s | t) / p_0) + Band_gap) ] }
	 */

    for (unsigned short i = 0; i < Globals::kLevelSize - 1; i++) {
        for (unsigned short j = i + 1; j <= Globals::kLevelSize - 1; j++) {
			//find source symbol in source module
            Symbol current_source_symbol;
            if (large_sentence)
               current_source_symbol = sentence[sentence.size() - Globals::kLevelSize + 1 + i];
			else if (i < static_cast<unsigned short>(sentence.size()))
                current_source_symbol = sentence[i];
			else
				continue;

            std::shared_ptr<Symbol> current_source_ptr = GetSymbolPtr(current_source_symbol);
            if ((j == target_module_id) && (current_source_ptr != nullptr)) {
				//found KnowledgeBase that targets given module
#ifdef CONFAB_H_
                std::cout << "Found knowledge base (" << i << ", " << j << ") with source symbol " << *current_source_ptr << "\n";
#endif
                const std::unique_ptr<KnowledgeBase>& current_knowledge_base = all_knowledge_bases_[i][j - (i + 1)];

                std::map<std::shared_ptr<Symbol>, std::unique_ptr<SymbolAttribute>, Symbol_Cmp>::const_iterator itSource = current_knowledge_base->get_knowledge_links().find(current_source_ptr);
                if (itSource != current_knowledge_base->get_knowledge_links().end()) {
					//initialize excitation of source symbol
                    itSource->second->set_source_excitation(1.0);

					//compute link strength for all target symbols
                    Synapse::iterator itTarget = itSource->second->get_knowledge_link().begin();
                    while (itTarget != itSource->second->get_knowledge_link().end()) {
                        std::shared_ptr<Symbol> current_target_ptr = itTarget->first;
                        std::map<std::shared_ptr<Symbol>, size_t, Symbol_Cmp>::iterator current_target_symbol_occurences_it = current_knowledge_base->get_target_symbol_occurences().find(current_target_ptr);
                        size_t current_target_symbol_occurences;
                        if (current_target_symbol_occurences_it != current_knowledge_base->get_target_symbol_occurences().end())
                            current_target_symbol_occurences = current_target_symbol_occurences_it->second;
                        else {
                            current_target_symbol_occurences = std::numeric_limits<size_t>::max();
							std::cout << "Warning: Target symbol occurences for symbol " << itTarget->first << " were not found!" << "\n";
						}

						//STAGE III: For each active symbol in the source module of each of these knowledge bases calculate the additive term in the
						//confabulation sum
                        Synapse::iterator current = itTarget++;
                        float current_link_strength = ((float)(current->second)) / ((float)current_target_symbol_occurences * Globals::kBaseProb);
#ifdef CLEAR_WEAK_LINKS_H_
                        if (current_link_strength <= 1.0) {
                            itSource->second->get_knowledge_link()->erase(current);
							continue;
						}
#endif
                        float former_target_excitation = itSource->second->get_target_excitation();
                        float excitation_increment = (itSource->second->get_source_excitation()) * (log(current_link_strength) + Globals::kBandGap);
                        float new_target_excitation = former_target_excitation + excitation_increment;
                        itSource->second->set_target_excitation(new_target_excitation);

						//Excitation vector represents the cumulative excitation of target symbols in all the knowledge bases
						//that have the module (j == moduleId) as target module
                        Excitation::iterator exc_it = output_map.find(current_target_ptr);
                        if (exc_it != output_map.end()) {
                            exc_it->second = exc_it->second + excitation_increment;
#ifdef CONFAB_H_
                            std::cout << "Total excitation for symbol " << *current_target_ptr << " becomes " << exc_it->second << "\n";
#endif
                        } else {
                            output_map.insert(std::pair<std::shared_ptr<Symbol>, float>(current_target_ptr, excitation_increment));
#ifdef CONFAB_H_
                            std::cout << "Excitation for symbol " << *current_target_ptr << " initializes at " << excitation_increment << "\n";
#endif
						}

					}
				}
			}
		}
	}

	std::priority_queue<std::pair<float, Symbol>,
						std::vector<std::pair<float, Symbol>>,
                        Excit_Cmp> max_heap;

	//STAGE IV: Select the symbol of the target module with the highest sum
    for(Excitation::iterator it = output_map.begin(); it != output_map.end(); ++it) {
        max_heap.push(std::pair<float, Symbol>(it->second, *(it->first)));
#ifdef CONFAB_H_
		std::cout << "Just entered symbol \"" << *(it->first) << "\" with excitation " << it->second << std::endl;
#endif
	}

	std::vector<std::pair<float, Symbol>> output;
    unsigned short k = 0;
    while (!max_heap.empty() && (k < Globals::kHeapResults)) {
        if (max_heap.top().second != "") {
            output.push_back(max_heap.top());
            k++;
        }
        max_heap.pop();
	}

	return output;
}

void KnowledgeManager::PersistRecallableKnowledge()
{
	//Must iterate through all knowledge bases and transform maps of knowledge links into vectors of outgoing knowledge links
    for (unsigned short i = 0; i < Globals::kLevelSize - 1; i++) {
        for (unsigned short j = i + 1; j <= Globals::kLevelSize - 1; j++) {
			std::ofstream current_output_file;
			std::stringstream temp_output_filename;
			temp_output_filename << "knowledge_data/KnowledgeBase_" << i << "-" << j;
			//temp_output_filename << "knowledge_data/KnowledgeBase_" << i << "-" << j << "_n";
			const std::string current_output_filename = temp_output_filename.str();
			current_output_file.open(current_output_filename.c_str());
            all_knowledge_bases_[i][j - (i + 1)]->PersistRecallableKnowledge(current_output_file);
			current_output_file.close();
		}
	}
}

void KnowledgeManager::RecallPersistedKnowledge()
{
	//Must first create all knowledge bases
	Init();

	//Must iterate through all knowledge base persisted files
	//and transform text lines into knowledge bases
    for (unsigned short i = 0; i < Globals::kLevelSize - 1; i++) {
        for (unsigned short j = i + 1; j <= Globals::kLevelSize - 1; j++) {
			std::ifstream current_input_file;
			std::stringstream temp_input_filename;
			temp_input_filename << "knowledge_data/KnowledgeBase_" << i << "-" << j;
			const std::string current_input_filename = temp_input_filename.str();
			current_input_file.open(current_input_filename.c_str());
            all_knowledge_bases_[i][j - (i + 1)]->RecallPersistedKnowledge(current_input_file);
			current_input_file.close();
			std::cout << "Finished recalling from file " << current_input_filename << std::endl;
		}
	}

}
