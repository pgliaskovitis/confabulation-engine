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

#include "ConfabulationTest.h"
#include "SentenceTokenizer.h"
#include "TextReader.h"
#include "SymbolMapping.h"
#include "NGramHandler.h"
#include "SymbolAttribute.h"
#include "KnowledgeBase.h"
#include "KnowledgeManager.h"
#include "Globals.h"
#include "sparse_structures/DOKExcitationVector.hpp"
#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"
#include "utils/HashTrie.hpp"
#include "utils/Utils.h"

int ConfabulationTest::TestTokenizeFixedString(const Symbol& input) const
{
	SentenceTokenizer tok(input);

    while (tok.Tokenize(Globals::kSentenceDelimiters))
        std::cout << tok.Str() << std::endl;

	return 0;
}

int ConfabulationTest::TestReadSymbolFile(const Symbol& input) const
{
	Globals globals;
	TextReader reader(globals);

    reader.HandleSymbolFile(input);

	return 0;
}

int ConfabulationTest::TestReadFixedFile(const Symbol& symbolfile, const Symbol& textfile) const
{
	Globals globals;
	std::shared_ptr<TextReader> reader(new TextReader(globals));
	std::shared_ptr<KnowledgeManager> manager(new KnowledgeManager(globals));

	manager->Init();
    globals.set_knowledge_manager(manager);
    globals.set_text_reader(reader);
    reader->HandleSymbolFile(symbolfile);
    reader->HandleAllSentences(textfile);

	return 0;
}

int ConfabulationTest::TestReadMultipleFiles(const Symbol& symbolfile, const Symbol& masterfile) const
{
	Globals globals;
	std::shared_ptr<TextReader> reader(new TextReader(globals));
	std::shared_ptr<KnowledgeManager> manager(new KnowledgeManager(globals));

	manager->Init();
    globals.set_knowledge_manager(manager);
    globals.set_text_reader(reader);
    reader->HandleSymbolFile(symbolfile);
    reader->HandleAllTextFiles(masterfile);

    return 0;
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

     my_vec_ptr2->SetElement(1, 1.0);
     my_vec_ptr2->SetElement(2, 2.0);
     my_vec_ptr2->SetElement(9, 9.0);

     my_vec_ptr1->Add(*my_vec_ptr2);

     std::cout << "Vector after addition:" << std::endl << my_vec_ptr1->ToString();
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

    std::unique_ptr<IExcitationVector<float>> my_result_vec_ptr = std::move(my_matrix_ptr->Multiply(*my_vec_ptr));

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

    std::unique_ptr<IExcitationVector<float>> my_result_vec_ptr = std::move(my_csr_matrix_ptr->Multiply(*my_vec_ptr));

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
	Symbol knowledgeFragment = "New:::York{1765|||2308}---->Haven{7|||201}---->Year{1111|||6538}";
	SentenceTokenizer tok(knowledgeFragment);
    tok.KnowledgeTokenize(KnowledgeBase::kPersistenceDelimiters); //without const, it does not compile
}

void ConfabulationTest::TestNGrams(const Symbol& symbolfile, const Symbol& masterfile) const
{
    Globals globals;
    std::shared_ptr<TextReader> reader(new TextReader(globals));
    std::shared_ptr<NGramHandler> ngram_handler(new NGramHandler(3, globals));
    std::shared_ptr<KnowledgeManager> manager(new KnowledgeManager(globals));

    manager->Init();
    globals.set_knowledge_manager(manager);
    globals.set_text_reader(reader);
    globals.set_ngram_handler(ngram_handler);
    reader->HandleSymbolFile(symbolfile);

    reader->HandleAllTextFiles(masterfile);

    globals.get_ngram_handler().CleanupNGrams();
    std::cout << "Multi-word count after cleanup is: " << globals.get_ngram_handler().get_multi_word_count() << "\n";
}

void ConfabulationTest::TestHashTrie(const Symbol& symbolfile, const Symbol& masterfile) const
{
    Globals globals;
    std::shared_ptr<TextReader> reader(new TextReader(globals));
    std::shared_ptr<NGramHandler> ngram_handler(new NGramHandler(3, globals));
    std::shared_ptr<KnowledgeManager> manager(new KnowledgeManager(globals));

    manager->Init();
    globals.set_knowledge_manager(manager);
    globals.set_text_reader(reader);
    globals.set_ngram_handler(ngram_handler);
    reader->HandleSymbolFile(symbolfile);

    reader->HandleAllTextFiles(masterfile);

    globals.get_ngram_handler().CleanupNGrams();
    std::cout << "Multi-word count after cleanup is: " << globals.get_ngram_handler().get_multi_word_count() << "\n";

    std::unique_ptr<SymbolMapping> all_symbols_mapping = globals.get_ngram_handler().GetAllSymbols();

    const std::set<Symbol>& all_symbols = all_symbols_mapping->GetAllSymbols();

    HashTrie<std::string> hash_trie;
    for (const Symbol& e : all_symbols) {
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
    std::vector<std::vector<std::string>> input = {
        {"as", "for", "the", "evil", "witch", "she"},
        {"as for the", "", "", "evil witch", ""},
        {"fairy tale", ""}
    };

    const std::vector<std::vector<std::string>> result = ProduceKnowledgeLinkCombinations(input, 13);

    for (size_t i = 0; i < result.size(); ++i)
        std::cout << "Combination " << i << ":" << VectorSymbolToSymbol(result[i], ',') << "\n";
}

void ConfabulationTest::TestSimpleConfabulation(const Symbol& symbolfile, const Symbol& masterfile, const std::vector<Symbol>& sentences) const
{
	Globals globals;
	std::shared_ptr<TextReader> reader(new TextReader(globals));
    std::shared_ptr<NGramHandler> ngram_handler(new NGramHandler(3, globals));
	std::shared_ptr<KnowledgeManager> manager(new KnowledgeManager(globals));

	manager->Init();
    globals.set_knowledge_manager(manager);
    globals.set_text_reader(reader);
    globals.set_ngram_handler(ngram_handler);
    reader->HandleSymbolFile(symbolfile);

    reader->HandleAllTextFiles(masterfile);

    manager->CleanUpWeakLinks();
    PerformConfabulation(globals, sentences);
    manager->PersistRecallableKnowledge();
}

void ConfabulationTest::TestConfabulationWithPersistedKnowledge(const Symbol& symbolfile, const Symbol& supplementfile, const std::vector<Symbol>& sentences) const
{
	Globals globals;
	std::shared_ptr<TextReader> reader(new TextReader(globals));
    std::shared_ptr<NGramHandler> ngram_handler(new NGramHandler(3, globals));
	std::shared_ptr<KnowledgeManager> manager(new KnowledgeManager(globals));

	manager->Init();
    globals.set_knowledge_manager(manager);
    globals.set_text_reader(reader);
    globals.set_ngram_handler(ngram_handler);
    reader->HandleSymbolFile(symbolfile);

	manager->RecallPersistedKnowledge();
    reader->HandleAllTextFiles(supplementfile);
    manager->CleanUpWeakLinks();
    PerformConfabulation(globals, sentences);
	manager->PersistRecallableKnowledge();
}

void ConfabulationTest::PerformConfabulation(Globals& globals, const std::vector<Symbol>& sentences) const
{
    for (std::vector<Symbol>::const_iterator it = sentences.begin(); it != sentences.end(); ++it) {
        std::vector<Symbol> currentfeedTokens = globals.get_text_reader().ExtractSentenceTokens(*it);
		//std::pair<float, Symbol> output = globals.getKnowledgeManager()->getConfabulatedSymbol(currentfeedTokens);
        std::vector<Symbol> output = globals.get_knowledge_manager().GetLayeredConfabulatedSymbol(currentfeedTokens);
		std::cout << *it << "{";
        for (unsigned short k = 0; k < Globals::kHeapResults; k++)
			std::cout << output[k] << ", ";
		std::cout << "}" << std::endl << std::endl;
	}
}

int main()
{
	std::shared_ptr<ConfabulationTest> test1(new ConfabulationTest());

    //test1->TestFixedString("This is, alas, the primal knowledge. \"My fumblings will be your quickening, minion.\"");

    //test1->TestSymbolFile("text_data/ascii_symbols.txt");

    //test1->TestFixedFile("text_data/ascii_symbols.txt", "text_data/Balzac_1.txt");

    //test1->TestMultipleFiles("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt");

    //test1->TestDOKExcitationVector();

    //test1->TestDOKLinksMatrix();

    //test1->TestCSRLinksMatrix();

    //test1->TestSymbolMapping();

    //test1->TestNGrams("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt");

    //test1->TestHashTrie("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt");

    test1->TestProduceKnowledgeLinkCombinations();

    //test1->TestTokenizePersistedKnowledge();

    Symbol copy_feed1 = "The umbrella was a black and prosaic "; //bundle
    Symbol copy_feed2 = "I will accept the post three times and refuse it "; //afterwards
    Symbol copy_feed3 = "He scuttled across the room rather like a "; //rabbit
    Symbol copy_feed4 = "In short, there is in life an element of elfin "; //coincidence
    Symbol copy_feed5 = "At night the wicket gate is replaced by a solid "; //door
    Symbol copy_feed6 = "In his own mind he had completely surrendered "; //himself
    Symbol copy_feed7 = "Every one is grave in public, and funny in "; //private
    Symbol copy_feed8 = "He had seen a great poet prostituting his muse to "; //journalism
    Symbol copy_feed9 = "It was clear that they were the accursed of all men, and they knew not "; //why
    Symbol copy_feed10 = "His long bare neck and sloping shoulders were the shape of a champagne "; //bottle

    std::shared_ptr<std::vector<Symbol>> allCopyFeeds(new std::vector<Symbol>());

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

	std::shared_ptr<std::vector<Symbol>> allOriginalFeeds(new std::vector<Symbol>());

    //test1->TestSimpleConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt", *allCopyFeeds);

	Symbol feed1 = "The hooded men were steadily chanting beneath the ";
	Symbol feed2 = "An army of little insects gathered on top of ";
	Symbol feed3 = "The prostitute smiled and slowly caressed his ";
	Symbol feed4 = "A dark winter was eating through the ";
	Symbol feed5 = "The eagle circled above the valley, eager for ";
	Symbol feed6 = "However, our initial impression of the situation was ";
	Symbol feed7 = "At the slightest hint of movement, he was ready to cut ";
	Symbol feed8 = "A warm and sweet scent was emanating from her ";
	Symbol feed9 = "She clumsily looked at her stained dress and ";
	Symbol feed10 = "She had not only her husband killed but also her ";

    Symbol feed11 = "The harsh realization that only writers could write well, threw him into a ";
    Symbol feed12 = "He looked at her, the love of his life, and whispered ";
    Symbol feed13 = "He collapsed impulsively, crying like a ";
    Symbol feed14 = "Alas, the bird's nest was too high up in the mountain for him to ";
    Symbol feed15 = "Suffocating in a sea of uninspired people, he started a ";
    Symbol feed16 = "She walked her wild paths without ";
    Symbol feed17 = "The clan and the khan were ";
    Symbol feed18 = "The beauty and the ";
    Symbol feed19 = "Yes, supreme effort had helped him get away from the ";
    Symbol feed20 = "Hosts of archers stood ready along the ";

    Symbol feed21 = "The joker laughed out loud and squeezed the ";
    Symbol feed22 = "She moaned with desire when he ";
    Symbol feed23 = "The thief stabbed Frederick viciously while he was ";
    Symbol feed24 = "The green hat swirled and landed on the ";
    Symbol feed25 = "Ominous waves surrounded us, just before we jumped ";
    Symbol feed26 = "He was mesmerized by her ";
    Symbol feed27 = "Unconventionally, the knight decided to kill the maiden and marry the ";
    Symbol feed28 = "The council of the void sent out its darkest agents to accomplish this ";
    Symbol feed29 = "Enlightenment, he said, goes hand in hand with a bowl of good ";
    Symbol feed30 = "The captain was listening indifferently for a while, but then he ";

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

    //test1->TestSimpleConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt", *allOriginalFeeds);

    //test1->TestConfabulationWithPersistedKnowledge("text_data/ascii_symbols.txt", "text_data/sample_master_supplement.txt", *allOriginalFeeds);
    //test1->TestConfabulationWithPersistedKnowledge("text_data/ascii_symbols.txt", "text_data/sample_master_empty.txt", *allOriginalFeeds);

	return 0;

}
