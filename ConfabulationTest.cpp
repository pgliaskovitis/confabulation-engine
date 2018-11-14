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

#include <algorithm>
#include <regex>
#include "Globals.h"
#include "ConfabulationTest.h"
#include "text_processing/SentenceTokenizer.h"
#include "SymbolMapping.h"
#include "text_processing/NGramHandler.h"
#include "text_processing/TextReader.h"
#include "sparse_structures/KHashExcitationVector.hpp"
#include "sparse_structures/DOKExcitationVector.hpp"
#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"
#include "utils/HashTrie.hpp"
#include "utils/Utils.h"
#include "ForwardConfabulation.h"
#include "TwoLevelMultiConfabulation.h"

void ConfabulationTest::TestTokenizeSentences(const std::string& input) const
{
	SentenceTokenizer tok(input);

	int i = 0;
	while (tok.Tokenize(Globals::kSentenceDelimiters)) {
		std::cout << tok.Str() << " " << i++ << std::endl;
	}
}

void ConfabulationTest::TestTokenizeWithinSentences(const std::string& input) const
{
	SentenceTokenizer tok(input);

	int i = 0;
	while (tok.Tokenize(Globals::kSentenceDelimiters)) {
		SentenceTokenizer inner_tok(tok.Str());
		while (inner_tok.Tokenize(Globals::kTokenDelimiters)) {
			if (!inner_tok.Delim().empty()) {
				std::regex comma_regex(",+"); // matches commas
				if(std::regex_match(inner_tok.Delim(), comma_regex)) {
					std::cout << inner_tok.Delim() << " " << i++ << std::endl;
				}
			}
			std::cout << inner_tok.Str() << " " << i++ << std::endl;
		}
	}
}

void ConfabulationTest::TestDOKExcitationVector() const
{
	std::unique_ptr<IExcitationVector<float>> my_vec_ptr1(new DOKExcitationVector<float>(10));

	my_vec_ptr1->SetElement(1, 1.0);
	my_vec_ptr1->SetElement(2, 2.0);
	my_vec_ptr1->SetElement(9, 9.0);

	try {
		my_vec_ptr1->SetElement(10, 10.0);
	} catch (const std::out_of_range& oor) {
	}

	std::cout << "Vector before addition:" << std::endl << my_vec_ptr1->ToString();

	std::unique_ptr<IExcitationVector<float>> my_vec_ptr2(new DOKExcitationVector<float>(10));

	my_vec_ptr2->SetElement(1, 1.5);
	my_vec_ptr2->SetElement(2, 2.5);
	my_vec_ptr2->SetElement(9, 9.5);

	std::cout << "Vector to add:" << std::endl << my_vec_ptr2->ToString();

	my_vec_ptr1->Add(*my_vec_ptr2);

	std::cout << "Vector after addition:" << std::endl << my_vec_ptr1->ToString();

	my_vec_ptr1->Normalize();

	std::cout << "Vector after normalization:" << std::endl << my_vec_ptr1->ToString();
}

void ConfabulationTest::TestKHashExcitationVector() const
{
	std::unique_ptr<IExcitationVector<float>> my_vec_ptr1(new KHashExcitationVector<float>(10));

	my_vec_ptr1->SetElement(1, 1.0);
	my_vec_ptr1->SetElement(2, 2.0);
	my_vec_ptr1->SetElement(9, 9.0);

	try {
		my_vec_ptr1->SetElement(10, 10.0);
	} catch (const std::out_of_range& oor) {
	}

	std::cout << "Vector before addition:" << std::endl << my_vec_ptr1->ToString();

	std::unique_ptr<IExcitationVector<float>> my_vec_ptr2(new KHashExcitationVector<float>(10));

	my_vec_ptr2->SetElement(1, 1.5);
	my_vec_ptr2->SetElement(2, 2.5);
	my_vec_ptr2->SetElement(9, 9.5);

	std::cout << "Vector to add:" << std::endl << my_vec_ptr2->ToString();

	my_vec_ptr1->Add(*my_vec_ptr2);

	std::cout << "Vector after addition:" << std::endl << my_vec_ptr1->ToString();

	my_vec_ptr1->Normalize();

	std::cout << "Vector after normalization:" << std::endl << my_vec_ptr1->ToString();
}

void ConfabulationTest::TestDOKLinksMatrix() const
{
	std::unique_ptr<IKnowledgeLinks<float>> my_matrix_ptr(new DOKLinksMatrix<float>(3, 4));

	my_matrix_ptr->SetElement(0, 1, 1.0);
	my_matrix_ptr->SetElement(1, 3, 1.0);
	my_matrix_ptr->SetElement(2, 2, 1.0);

	std::cout << "Matrix before multiplication:" << std::endl << my_matrix_ptr->ToString();

	std::unique_ptr<IExcitationVector<float>> my_vec_ptr(new DOKExcitationVector<float>(4));

	my_vec_ptr->SetElement(0, 1.0);
	my_vec_ptr->SetElement(1, 2.0);
	my_vec_ptr->SetElement(3, 4.0);

	std::cout << "Vector before multiplication:" << std::endl << my_vec_ptr->ToString();

	std::unique_ptr<IExcitationVector<float>> my_result_vec_ptr = my_matrix_ptr->Multiply(*my_vec_ptr);

	std::cout << "Vector after multiplication:" << std::endl << my_result_vec_ptr->ToString();
}

void ConfabulationTest::TestCSRLinksMatrix() const
{
	std::unique_ptr<IKnowledgeLinks<float>> my_dok_matrix_ptr(new DOKLinksMatrix<float>(3, 4));

	my_dok_matrix_ptr->SetElement(0, 1 , 1.0);
	my_dok_matrix_ptr->SetElement(1, 3, 1.0);
	my_dok_matrix_ptr->SetElement(2, 2, 1.0);

	std::unique_ptr<IKnowledgeLinks<float>> my_csr_matrix_ptr(new CSRLinksMatrix<float>(*my_dok_matrix_ptr));

	std::cout << "Matrix before multiplication:" << std::endl << my_csr_matrix_ptr->ToString();

	std::unique_ptr<IExcitationVector<float>> my_vec_ptr(new DOKExcitationVector<float>(4));

	my_vec_ptr->SetElement(0, 1.0);
	my_vec_ptr->SetElement(1, 2.0);
	my_vec_ptr->SetElement(3, 4.0);

	std::cout << "Vector before multiplication:" << std::endl << my_vec_ptr->ToString();

	std::unique_ptr<IExcitationVector<float>> my_result_vec_ptr = my_csr_matrix_ptr->Multiply(*my_vec_ptr);

	std::cout << "Vector after multiplication:" << std::endl << my_result_vec_ptr->ToString();
}

void ConfabulationTest::TestSymbolMapping() const
{
	std::unique_ptr<SymbolMapping> my_symbol_mapping_ptr(new SymbolMapping());

	my_symbol_mapping_ptr->AddSymbol("perilous");
	my_symbol_mapping_ptr->AddSymbol("cellar");
	my_symbol_mapping_ptr->AddSymbol("door");
	my_symbol_mapping_ptr->AddSymbol("to");
	my_symbol_mapping_ptr->AddSymbol("the");
	my_symbol_mapping_ptr->AddSymbol("wonder");
	my_symbol_mapping_ptr->AddSymbol("Darko");

	std::cout << "SymbolMapping after addition:" << std::endl << my_symbol_mapping_ptr->ToString();
}

void ConfabulationTest::TestTokenizePersistedKnowledge() const
{
	std::string knowledgeFragment = "New:::York{1765|||2308}---->Haven{7|||201}---->Year{1111|||6538}";
	SentenceTokenizer tok(knowledgeFragment);
	tok.KnowledgeTokenize(Globals::kPersistenceDelimiters);
}

void ConfabulationTest::TestProduceNGrams(const std::string& symbolfile, const std::string& masterfile) const
{
	TextReader reader(symbolfile, masterfile);
	reader.Initialize();

	NGramHandler ngram_handler(3, 1, 3);

	std::vector<std::string> sentence;
	bool finished_reading = false;
	do {
		sentence = reader.GetNextSentenceTokens(finished_reading);
		ngram_handler.ExtractAndStoreNGrams(sentence);
	} while (!finished_reading);

	ngram_handler.CleanupNGrams();
	std::cout << "Multi-word count after cleanup is: " << ngram_handler.get_multi_word_count() << "\n";
}

void ConfabulationTest::TestHashTrie(const std::string& symbolfile, const std::string& masterfile) const
{
	TextReader reader(symbolfile, masterfile);
	reader.Initialize();

	NGramHandler ngram_handler(3, 1, 3);

	std::vector<std::string> sentence;
	bool finished_reading = false;
	do {
		sentence = reader.GetNextSentenceTokens(finished_reading);
		ngram_handler.ExtractAndStoreNGrams(sentence);
	} while (!finished_reading);

	ngram_handler.CleanupNGrams();
	std::cout << "Multi-word count after cleanup is: " << ngram_handler.get_multi_word_count() << "\n";

	std::unique_ptr<SymbolMapping> all_symbols_mapping = ngram_handler.GetAllSymbols();

	const std::set<std::string>& all_symbols = all_symbols_mapping->GetAllSymbols();

	HashTrie<std::string> hash_trie;
	for (const std::string& e : all_symbols) {
		std::cout << e << "\n";
		const std::vector<std::string>& symbol_words = SymbolToVectorSymbol(e, ' ');
		hash_trie.Add(symbol_words);
	}

	std::list<std::string> test_list1;
	std::vector<std::string> test_vec1 = {"as", "for", "his", "beloved", "fiancee"};
	std::copy(test_vec1.begin(), test_vec1.end(), std::back_inserter(test_list1));

	std::list<std::string> test_list2;
	std::vector<std::string> test_vec2 = {"one", "would", "have", "thought", "that", "the", "boy", "was"};
	std::copy(test_vec2.begin(), test_vec2.end(), std::back_inserter(test_list2));

	std::list<std::string> test_list3;
	std::vector<std::string> test_vec3 = {"the", "success", "of", "his", "father's", "business"};
	std::copy(test_vec3.begin(), test_vec3.end(), std::back_inserter(test_list3));

	std::cout << "Test1: " << ListSymbolToSymbol(hash_trie.FindLongest(test_list1), ' ') << "\n";
	std::cout << "Test2: " << ListSymbolToSymbol(hash_trie.FindLongest(test_list2), ' ') << "\n";
	std::cout << "Test3: " << ListSymbolToSymbol(hash_trie.FindLongest(test_list3), ' ') << "\n";
}

void ConfabulationTest::TestProduceKnowledgeLinkCombinations() const
{
	std::vector<std::vector<std::string>> input_1 = {
		{"as", "for", "the", "evil", "witch", "she"},
		{"as for the", "", "", "evil witch", ""},
		{"fairy tale", ""}
	};

	const std::vector<std::vector<std::string>> result_1 = ProduceKnowledgeLinkCombinations(input_1, 13);

	for (size_t i = 0; i < result_1.size(); ++i) {
		std::cout << "Combination " << i << ":" << VectorSymbolToSymbol(result_1[i], ',') << "\n";
	}

	std::cout << "\n" << std::flush;

	std::vector<std::vector<std::string>> input_2 = {
		{"as", "for", "the", "evil", "witch", "she"},
		{"as for", "", "", "evil witch", ""},
		{"fairy tale", ""}
	};

	const std::vector<std::vector<std::string>> result_2 = ProduceKnowledgeLinkCombinations(input_2, 13);

	for (size_t i = 0; i < result_2.size(); ++i) {
		std::cout << "Combination " << i << ":" << VectorSymbolToSymbol(result_2[i], ',') << "\n";
	}
}

void ConfabulationTest::TestTransferExcitations(const std::string& symbolfile, const std::string& masterfile) const
{
	size_t num_word_modules = 10;
	TwoLevelMultiConfabulation confab_engine(num_word_modules, symbolfile, masterfile, 1, 1);
	TextReader reader(symbolfile, masterfile);
	reader.Initialize();

	const std::unique_ptr<Module>& module_0 = confab_engine.get_module(0);
	const std::unique_ptr<Module>& module_1 = confab_engine.get_module(1);
	const std::unique_ptr<Module>& module_2 = confab_engine.get_module(2);
	const std::unique_ptr<Module>& module_3 = confab_engine.get_module(3);
	const std::unique_ptr<Module>& module_5 = confab_engine.get_module(5);
	const std::unique_ptr<Module>& module_4 = confab_engine.get_module(4);
	const std::unique_ptr<Module>& module_6 = confab_engine.get_module(num_word_modules);
	const std::unique_ptr<Module>& module_7 = confab_engine.get_module(num_word_modules + 1);

	const std::unique_ptr<KnowledgeBase>& knowledge_base_0_6 = confab_engine.get_knowledge_base(0, num_word_modules);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_1_6 = confab_engine.get_knowledge_base(1, num_word_modules);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_1_7 = confab_engine.get_knowledge_base(1, num_word_modules + 1);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_2_7 = confab_engine.get_knowledge_base(2, num_word_modules + 1);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_6_0 = confab_engine.get_knowledge_base(num_word_modules, 0);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_6_1 = confab_engine.get_knowledge_base(num_word_modules, 1);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_6_2 = confab_engine.get_knowledge_base(num_word_modules, 2);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_6_3 = confab_engine.get_knowledge_base(num_word_modules, 3);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_6_4 = confab_engine.get_knowledge_base(num_word_modules, 4);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_7_1 = confab_engine.get_knowledge_base(num_word_modules + 1, 1);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_7_2 = confab_engine.get_knowledge_base(num_word_modules + 1, 2);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_7_3 = confab_engine.get_knowledge_base(num_word_modules + 1, 3);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_7_4 = confab_engine.get_knowledge_base(num_word_modules + 1, 4);
	const std::unique_ptr<KnowledgeBase>& knowledge_base_7_5 = confab_engine.get_knowledge_base(num_word_modules + 1, 5);

	std::cout << "Knowledge base 0-6: " << knowledge_base_0_6->GetNumKnowledgeLinks() << " links" << std::endl;
	std::cout << "Knowledge base 1-6: " << knowledge_base_1_6->GetNumKnowledgeLinks() << " links" << std::endl;
	std::cout << "Knowledge base 2-7: " << knowledge_base_2_7->GetNumKnowledgeLinks() << " links" << std::endl;
	std::cout << "Knowledge base 1-7: " << knowledge_base_1_7->GetNumKnowledgeLinks() << " links" << std::endl;

	// preliminary checks on modules
	const std::set<std::string>& all_module_0_symbols = module_0->get_symbol_mapping().GetAllSymbols();
	const std::set<std::string>& all_module_1_symbols = module_1->get_symbol_mapping().GetAllSymbols();
	assert(all_module_0_symbols.size() == all_module_1_symbols.size());

	const std::set<std::string>& all_module_6_symbols = module_6->get_symbol_mapping().GetAllSymbols();
	const std::set<std::string>& all_module_7_symbols = module_7->get_symbol_mapping().GetAllSymbols();
	assert(all_module_6_symbols.size() == all_module_7_symbols.size());
	std::cout << "Symbols at module " << num_word_modules << " are: \n" << SetSymbolToSymbol(all_module_6_symbols, '\n') << "\n" << std::flush;

	// preliminary checks on tries
	const std::unique_ptr<HashTrie<std::string>>& level_1_trie = confab_engine.get_organizer()->get_trie_for_level(1);
	std::vector<std::list<std::string>> all_subsequences_1 = level_1_trie->FindAll({"the", "sense", "of", "duty"});
	std::cout << "Trie results I: " << "\n" << std::flush;
	for (const std::list<std::string>& e: all_subsequences_1) {
		std::cout << ListSymbolToSymbol(e, ' ') << "\n" << std::flush;
	}

	std::vector<std::list<std::string>> all_subsequences_2 = level_1_trie->FindAll({"the", "sense", "of", "humor"});
	std::cout << "Trie results II: " << "\n" << std::flush;
	for (const std::list<std::string>& e: all_subsequences_2) {
		std::cout << ListSymbolToSymbol(e, ' ') << "\n" << std::flush;
	}

	std::vector<std::list<std::string>> all_subsequences_3 = level_1_trie->FindAll({"the", "world", "has", "rubbed"});
	std::cout << "Trie results III: " << "\n" << std::flush;
	for (const std::list<std::string>& e: all_subsequences_3) {
		std::cout << ListSymbolToSymbol(e, ' ') << "\n" << std::flush;
	}

	confab_engine.Activate({"the", "sense"});
	confab_engine.TransferExcitation(module_0, knowledge_base_0_6, module_6);
	confab_engine.TransferExcitation(module_1, knowledge_base_1_6, module_6);

	const std::vector<std::string>& expectation_at_target_a = module_6->GetExpectation();
	std::cout << "Excited symbols at module " << num_word_modules << " are: \n" << VectorSymbolToSymbol(expectation_at_target_a, '\n') << "\n" << std::flush;

	const std::vector<std::string>& excitations_C1F_at_target_a = module_6->PartialConfabulation(1);
	std::cout << "C1F confabulated symbols at module " << num_word_modules << " are: \n" << VectorSymbolToSymbol(excitations_C1F_at_target_a, '\n') << "\n" << std::flush;

	const std::vector<std::string>& excitations_C2F_at_target_a = module_6->PartialConfabulation(2);
	std::cout << "C2F confabulated symbol at module " << num_word_modules << " is: \n" << VectorSymbolToSymbol(excitations_C2F_at_target_a, '\n') << "\n" << std::flush;

	confab_engine.Clean();
	confab_engine.Activate({"", "the", "sense"});
	confab_engine.TransferExcitation(module_1, knowledge_base_1_7, module_7);
	confab_engine.TransferExcitation(module_2, knowledge_base_2_7, module_7);

	const std::vector<std::string>& expectation_at_target_b = module_7->GetExpectation();
	std::cout << "Excited symbols at module " << num_word_modules + 1 << " are: \n" << VectorSymbolToSymbol(expectation_at_target_b, '\n') << "\n" << std::flush;

	const std::vector<std::string> excitations_C1F_at_target_b = module_7->PartialConfabulation(1);
	std::cout << "C1F confabulated symbols at module " << num_word_modules + 1 << " is: \n" << VectorSymbolToSymbol(excitations_C1F_at_target_b, '\n') << "\n" << std::flush;

	const std::vector<std::string> excitations_C2F_at_target_b = module_7->PartialConfabulation(2);
	std::cout << "C2F confabulated symbol at module " << num_word_modules + 1 << " is: \n" << VectorSymbolToSymbol(excitations_C2F_at_target_b, '\n') << "\n" << std::flush;

	confab_engine.Clean();
	confab_engine.Activate({"", "the", "world"});
	confab_engine.TransferExcitation(module_1, knowledge_base_1_7, module_7);
	confab_engine.TransferExcitation(module_2, knowledge_base_2_7, module_7);

	const std::vector<std::string>& expectation_at_target_c = module_7->GetExpectation();
	std::cout << "Excited symbols at module " << num_word_modules + 1 << " are: \n" << VectorSymbolToSymbol(expectation_at_target_c, '\n') << "\n" << std::flush;

	const std::vector<std::string> excitations_C1F_at_target_c = module_7->PartialConfabulation(1);
	std::cout << "C1F confabulated symbols at module " << num_word_modules + 1 << " is: \n" << VectorSymbolToSymbol(excitations_C1F_at_target_c, '\n') << "\n" << std::flush;

	const std::vector<std::string> excitations_C2F_at_target_c = module_7->PartialConfabulation(2);
	std::cout << "C2F confabulated symbol at module " << num_word_modules + 1 << " is: \n" << VectorSymbolToSymbol(excitations_C2F_at_target_c, '\n') << "\n" << std::flush;

	confab_engine.Clean();
	confab_engine.Activate({"", "", "", "", "", "", "", "", "", "", "the voice of honor clamors"});
	confab_engine.TransferExcitation(module_6, knowledge_base_6_0, module_0);
	confab_engine.TransferExcitation(module_6, knowledge_base_6_1, module_1);
	confab_engine.TransferExcitation(module_6, knowledge_base_6_2, module_2);
	confab_engine.TransferExcitation(module_6, knowledge_base_6_3, module_3);
	confab_engine.TransferExcitation(module_6, knowledge_base_6_4, module_4);

	const std::vector<std::string>& expectation_at_target_d = module_0->GetExpectation();
	std::cout << "C1F confabulated symbols at module 0 are: \n" << VectorSymbolToSymbol(expectation_at_target_d, '\n') << "\n" << std::flush;

	const std::vector<std::string>& expectation_at_target_e = module_1->GetExpectation();
	std::cout << "C1F confabulated symbols at module 1 are: \n" << VectorSymbolToSymbol(expectation_at_target_e, '\n') << "\n" << std::flush;

	const std::vector<std::string>& expectation_at_target_f = module_2->GetExpectation();
	std::cout << "C1F confabulated symbols at module 2 are: \n" << VectorSymbolToSymbol(expectation_at_target_f, '\n') << "\n" << std::flush;

	const std::vector<std::string>& expectation_at_target_g = module_3->GetExpectation();
	std::cout << "C1F confabulated symbols at module 3 are: \n" << VectorSymbolToSymbol(expectation_at_target_g, '\n') << "\n" << std::flush;

	const std::vector<std::string>& expectation_at_target_h = module_4->GetExpectation();
	std::cout << "C1F confabulated symbols at module 4 are: \n" << VectorSymbolToSymbol(expectation_at_target_h, '\n') << "\n" << std::flush;

	confab_engine.Clean();
	confab_engine.Activate({"", "", "", "", "", "", "", "", "", "", "", "the voice of honor clamors"});
	confab_engine.TransferExcitation(module_7, knowledge_base_7_1, module_1);
	confab_engine.TransferExcitation(module_7, knowledge_base_7_2, module_2);
	confab_engine.TransferExcitation(module_7, knowledge_base_7_3, module_3);
	confab_engine.TransferExcitation(module_7, knowledge_base_7_4, module_4);
	confab_engine.TransferExcitation(module_7, knowledge_base_7_5, module_5);

	const std::vector<std::string>& expectation_at_target_i = module_1->GetExpectation();
	std::cout << "C1F confabulated symbols at module 1 are: \n" << VectorSymbolToSymbol(expectation_at_target_i, '\n') << "\n" << std::flush;

	const std::vector<std::string>& expectation_at_target_j = module_2->GetExpectation();
	std::cout << "C1F confabulated symbols at module 2 are: \n" << VectorSymbolToSymbol(expectation_at_target_j, '\n') << "\n" << std::flush;

	const std::vector<std::string>& expectation_at_target_k = module_3->GetExpectation();
	std::cout << "C1F confabulated symbols at module 3 are: \n" << VectorSymbolToSymbol(expectation_at_target_k, '\n') << "\n" << std::flush;

	const std::vector<std::string>& expectation_at_target_l = module_4->GetExpectation();
	std::cout << "C1F confabulated symbols at module 4 are: \n" << VectorSymbolToSymbol(expectation_at_target_l, '\n') << "\n" << std::flush;

	const std::vector<std::string>& expectation_at_target_m = module_5->GetExpectation();
	std::cout << "C1F confabulated symbols at module 5 are: \n" << VectorSymbolToSymbol(expectation_at_target_m, '\n') << "\n" << std::flush;
}

void ConfabulationTest::TestSimpleConfabulation(const std::string& symbolfile, const std::string& masterfile, const std::vector<std::string>& sentences) const
{
	size_t num_word_modules = Globals::kReferenceFrameSize + 1;
	ForwardConfabulation confab_engine(num_word_modules, symbolfile, masterfile, 1, 2);
	TextReader reader(symbolfile, masterfile);
	reader.Initialize();

	for (const std::string& e : sentences) {
		const std::vector<std::string> current_feed_tokens(reader.ExtractTokens(e));
		FillWithEmptyStrings(current_feed_tokens, num_word_modules);
		const std::vector<std::string>& current_result_tokens = confab_engine.Confabulation(current_feed_tokens, -1, false);
		std::cout << e << current_result_tokens[0] << "\n" << std::flush;
	}
}

void ConfabulationTest::TestTwoLevelMultiConfabulation(const std::string& symbolfile, const std::string& masterfile, const std::vector<std::string>& sentences) const
{
	size_t num_word_modules = Globals::kReferenceFrameSize + 1;
	TwoLevelMultiConfabulation confab_engine(num_word_modules, symbolfile, masterfile, 1, 2);
	TextReader reader(symbolfile, masterfile);
	reader.Initialize();

	for (const std::string& e : sentences) {
		const std::vector<std::string> current_feed_tokens(reader.ExtractTokens(e));
		FillWithEmptyStrings(current_feed_tokens, num_word_modules);
		const std::vector<std::string>& current_result_tokens = confab_engine.Confabulation(current_feed_tokens, -1, false);
		std::cout << e << VectorSymbolToSymbol(current_result_tokens, ' ') << "\n" << std::flush;
	}
}

int main()
{
	std::shared_ptr<ConfabulationTest> test1(new ConfabulationTest());

	// test1->TestTokenizeSentences("This is, alas, the primal knowledge. The magic exists, but is not for everyone. \"My fumblings will be your quickening, minion.\"");

	// test1->TestTokenizeWithinSentences("This is,  alas , the primal knowledge. The magic exists  ,but is not for everyone. \"My fumblings will be your quickening  ,   minion.\"");

	// test1->TestDOKExcitationVector();

	// test1->TestKHashExcitationVector();

	// test1->TestDOKLinksMatrix();

	// test1->TestCSRLinksMatrix();

	// test1->TestSymbolMapping();

	// test1->TestProduceNGrams("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt");

	// test1->TestHashTrie("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt");

	// test1->TestProduceKnowledgeLinkCombinations();

	// test1->TestTransferExcitations("text_data/ascii_symbols.txt", "text_data/sample_master_debug.txt");

	// test1->TestTokenizePersistedKnowledge();

	std::string copy_feed1 = "Tormented by reflection, or for whom strife has become a "; // plaything
	std::string copy_feed2 = "I will accept the post three times and refuse it "; // afterwards
	std::string copy_feed3 = "Then he turned his own rather fatuous face to the "; // company
	std::string copy_feed4 = "Only a man who knows nothing of motors talks of motoring without "; // petrol
	std::string copy_feed5 = "At night the wicket gate is replaced by a solid "; // door
	std::string copy_feed6 = "He knew that he was doing wrong, and did it "; // deliberately
	std::string copy_feed7 = "A second moan brought her to the landing near his "; // room
	std::string copy_feed8 = "He noticed a group of busts mounted on wooden pedestals, painted to resemble "; // marble
	std::string copy_feed9 = "Across these was still moving the square black mob like one monstrous "; // beetle
	std::string copy_feed10 = "He was very pale, and he wore blue "; // spectacles

	std::string copy_feed11 = "And this dainty change of toilette "; // had been made for me
	std::string copy_feed12 = "She laid hands on the pears counted out "; // by her father, and piled them in a pyramid
	std::string copy_feed13 = "Yet, for all his good impulses, he "; // could not bring himself to make profession of the
	std::string copy_feed14 = "Only, if you want to avoid storms "; // at home for the future, do not leave me for three weeks
	std::string copy_feed15 = "To do really good work, my boy, means that "; // that you will draw out the energy
	std::string copy_feed16 = "Even the man's start and suspicious stare "; // as the priest went by were consonant enough
	std::string copy_feed17 = "Turnbull was of that more masculine type in which "; // a sense of responsibility increases the appetite
	std::string copy_feed18 = "The wheel grew larger and larger "; // till it filled heaven like the wheel of stars
	std::string copy_feed19 = "While the author and the victims alike treated "; // the whole matter as a silly public charade
	std::string copy_feed20 = "The difficulty is therefore that the actual raising of "; // the standard of athletics has probably been

	std::shared_ptr<std::vector<std::string>> allCopyFeeds(new std::vector<std::string>());

	allCopyFeeds->push_back(copy_feed1);
	allCopyFeeds->push_back(copy_feed2);
	allCopyFeeds->push_back(copy_feed3);
	allCopyFeeds->push_back(copy_feed4);
	allCopyFeeds->push_back(copy_feed5);
	allCopyFeeds->push_back(copy_feed6);
	allCopyFeeds->push_back(copy_feed7);
	allCopyFeeds->push_back(copy_feed8);
	allCopyFeeds->push_back(copy_feed9);
	allCopyFeeds->push_back(copy_feed10);
	allCopyFeeds->push_back(copy_feed11);
	allCopyFeeds->push_back(copy_feed12);
	allCopyFeeds->push_back(copy_feed13);
	allCopyFeeds->push_back(copy_feed14);
	allCopyFeeds->push_back(copy_feed15);
	allCopyFeeds->push_back(copy_feed16);
	allCopyFeeds->push_back(copy_feed17);
	allCopyFeeds->push_back(copy_feed18);
	allCopyFeeds->push_back(copy_feed19);
	allCopyFeeds->push_back(copy_feed20);

	test1->TestTwoLevelMultiConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt", *allCopyFeeds);

	std::shared_ptr<std::vector<std::string>> allOriginalFeeds(new std::vector<std::string>());

	std::string feed1 = "The hooded men were chanting beneath the ";
	std::string feed2 = "An army of little insects had gathered on top of ";
	std::string feed3 = "The girl smiled and caressed ";
	std::string feed4 = "A dark winter was eating through the ";
	std::string feed5 = "The dragon circled above the valley, hungry for ";
	std::string feed6 = "Our initial assessment of the situation had been ";
	std::string feed7 = "At the slightest hint of movement, he was ready to ";
	std::string feed8 = "A strong scent of cinnamon was coming from her ";
	std::string feed9 = "She stumbled over her dress and ";
	std::string feed10 = "She had her husband poisoned and her lover ";

	std::string feed11 = "In the end, only dancing could ";
	std::string feed12 = "He leaned into her, the love of his life, and ";
	std::string feed13 = "He suddenly collapsed, crying like a ";
	std::string feed14 = "The cave was too high up for him to ";
	std::string feed15 = "In that party of dull people, he started ";
	std::string feed16 = "She led her wild life without ";
	std::string feed17 = "The chief and the whole clan were ";
	std::string feed18 = "All that beauty had suddenly turned into ";
	std::string feed19 = "With one last desperate effort he managed to ";
	std::string feed20 = "Hosts of archers stood ready along the ";

	std::string feed21 = "The joker laughed and continued ";
	std::string feed22 = "She moaned softly when he ";
	std::string feed23 = "The thief stabbed him while he was ";
	std::string feed24 = "The green leaf swayed and landed on ";
	std::string feed25 = "Ominous waves surrounded us, just before we ";
	std::string feed26 = "He was hopelessly enchanted by ";
	std::string feed27 = "The knight decided to kill the maiden and ";
	std::string feed28 = "The council sent out its most dedicated agents to ";
	std::string feed29 = "Enlightenment goes well with a ";
	std::string feed30 = "The captain was silent for a while, but then ";

	std::string feed31 = "Ultimately, his quest was one of ";
	std::string feed32 = "Fortunately, there will always exist ";
	std::string feed33 = "As it turns out, luck favors the ";

	allOriginalFeeds->push_back(feed31);
	allOriginalFeeds->push_back(feed32);
	allOriginalFeeds->push_back(feed33);

	allOriginalFeeds->push_back(feed1);
	allOriginalFeeds->push_back(feed2);
	allOriginalFeeds->push_back(feed3);
	allOriginalFeeds->push_back(feed4);
	allOriginalFeeds->push_back(feed5);
	allOriginalFeeds->push_back(feed6);
	allOriginalFeeds->push_back(feed7);
	allOriginalFeeds->push_back(feed8);
	allOriginalFeeds->push_back(feed9);
	allOriginalFeeds->push_back(feed10);
	allOriginalFeeds->push_back(feed11);
	allOriginalFeeds->push_back(feed12);
	allOriginalFeeds->push_back(feed13);
	allOriginalFeeds->push_back(feed14);
	allOriginalFeeds->push_back(feed15);
	allOriginalFeeds->push_back(feed16);
	allOriginalFeeds->push_back(feed17);
	allOriginalFeeds->push_back(feed18);
	allOriginalFeeds->push_back(feed19);
	allOriginalFeeds->push_back(feed20);
	allOriginalFeeds->push_back(feed21);
	allOriginalFeeds->push_back(feed22);
	allOriginalFeeds->push_back(feed23);
	allOriginalFeeds->push_back(feed24);
	allOriginalFeeds->push_back(feed25);
	allOriginalFeeds->push_back(feed26);
	allOriginalFeeds->push_back(feed27);
	allOriginalFeeds->push_back(feed28);
	allOriginalFeeds->push_back(feed29);
	allOriginalFeeds->push_back(feed30);

	// test1->TestConfabulationWithPersistedKnowledge("text_data/ascii_symbols.txt", "text_data/sample_master_supplement.txt", *allOriginalFeeds);
	// test1->TestConfabulationWithPersistedKnowledge("text_data/ascii_symbols.txt", "text_data/sample_master_empty.txt", *allOriginalFeeds);

	// test1->TestTwoLevelMultiConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt", *allOriginalFeeds);
	// test1->TestTwoLevelMultiConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_selection.txt", *allOriginalFeeds);
	// test1->TestTwoLevelMultiConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master.txt", *allOriginalFeeds);

	return 0;
}
