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
#include "Globals.h"
#include "ConfabulationTest.h"
#include "text_processing/SentenceTokenizer.h"
#include "SymbolMapping.h"
#include "text_processing/NGramHandler.h"
#include "text_processing/TextReader.h"
#include "sparse_structures/DOKExcitationVector.hpp"
#include "sparse_structures/DOKLinksMatrix.hpp"
#include "sparse_structures/CSRLinksMatrix.hpp"
#include "utils/HashTrie.hpp"
#include "utils/Utils.h"
#include "ForwardConfabulation.h"
#include "TwoLevelSimpleConfabulation.h"

int ConfabulationTest::TestTokenizeFixedString(const std::string& input) const
{
	SentenceTokenizer tok(input);

    while (tok.Tokenize(Globals::kSentenceDelimiters))
        std::cout << tok.Str() << std::endl;

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
    std::vector<std::vector<std::string>> input = {
        {"as", "for", "the", "evil", "witch", "she"},
        {"as for the", "", "", "evil witch", ""},
        {"fairy tale", ""}
    };

    const std::vector<std::vector<std::string>> result = ProduceKnowledgeLinkCombinations(input, 13);

    for (size_t i = 0; i < result.size(); ++i)
        std::cout << "Combination " << i << ":" << VectorSymbolToSymbol(result[i], ',') << "\n";
}

void ConfabulationTest::TestTransferExcitations(const std::string& symbolfile, const std::string& masterfile) const
{
    size_t num_word_modules = 6;
    TwoLevelSimpleConfabulation confab_engine(num_word_modules, symbolfile, masterfile, 1, 1);
    TextReader reader(symbolfile, masterfile);
    reader.Initialize();

    const std::unique_ptr<Module>& module_6 = confab_engine.get_module(num_word_modules);
    const std::unique_ptr<Module>& module_7 = confab_engine.get_module(num_word_modules + 1);

    const std::unique_ptr<Module>& module_0 = confab_engine.get_module(0);
    const std::unique_ptr<KnowledgeBase>& knowledge_base_0_6 = confab_engine.get_knowledge_base(0, num_word_modules);
    const std::unique_ptr<KnowledgeBase>& knowledge_base_0_7 = confab_engine.get_knowledge_base(0, num_word_modules + 1);

    const std::unique_ptr<Module>& module_1 = confab_engine.get_module(1);
    const std::unique_ptr<KnowledgeBase>& knowledge_base_1_6 = confab_engine.get_knowledge_base(1, num_word_modules);
    const std::unique_ptr<KnowledgeBase>& knowledge_base_1_7 = confab_engine.get_knowledge_base(1, num_word_modules + 1);

    confab_engine.Activate({"the", "world"});
    confab_engine.TransferExcitation(module_0, knowledge_base_0_6, module_6);
    confab_engine.TransferExcitation(module_1, knowledge_base_1_6, module_6);

    const std::vector<std::string>& expectation_at_target_a = module_6->GetExpectation();
    std::cout << "Excited symbols at module " << num_word_modules << " are: \n" << VectorSymbolToSymbol(expectation_at_target_a, '\n') << "\n" << std::flush;

    const std::vector<std::string>& excitations_C1F_at_target_a = module_6->PartialConfabulation(1, false);
    std::cout << "C1F confabulated symbols at module " << num_word_modules << " is: \n" << VectorSymbolToSymbol(excitations_C1F_at_target_a, '\n') << "\n" << std::flush;

    const std::vector<std::string>& excitations_C2F_at_target_a = module_6->PartialConfabulation(2, false);
    std::cout << "C2F confabulated symbols at module " << num_word_modules << " is: \n" << VectorSymbolToSymbol(excitations_C2F_at_target_a, '\n') << "\n" << std::flush;

    confab_engine.Clean();
    confab_engine.Activate({"the", "sense"});
    confab_engine.TransferExcitation(module_0, knowledge_base_0_7, module_7);
    confab_engine.TransferExcitation(module_1, knowledge_base_1_7, module_7);

    const std::vector<std::string>& expectation_at_target_b = module_7->GetExpectation();
    std::cout << "Excited symbols at module " << num_word_modules + 1 << " are: \n" << VectorSymbolToSymbol(expectation_at_target_b, '\n') << "\n" << std::flush;

    const std::vector<std::string> excitations_C1F_at_target_b = module_7->PartialConfabulation(1, false);
    std::cout << "C1F confabulated symbols at module " << num_word_modules + 1 << " is: \n" << VectorSymbolToSymbol(excitations_C1F_at_target_b, '\n') << "\n" << std::flush;

    const std::vector<std::string> excitations_C2F_at_target_b = module_7->PartialConfabulation(2, false);
    std::cout << "C2F confabulated symbols at module " << num_word_modules + 1 << " is: \n" << VectorSymbolToSymbol(excitations_C2F_at_target_b, '\n') << "\n" << std::flush;
}

void ConfabulationTest::TestSimpleConfabulation(const std::string& symbolfile, const std::string& masterfile, const std::vector<std::string>& sentences) const
{
    size_t num_word_modules = 15;
    ForwardConfabulation confab_engine(num_word_modules, symbolfile, masterfile, 1, 5);
    TextReader reader(symbolfile, masterfile);
    reader.Initialize();

    for (const std::string& e : sentences) {
        const std::vector<std::string> current_feed_tokens(reader.ExtractTokens(e));
        FillWithEmptyStrings(current_feed_tokens, num_word_modules);
        const std::vector<std::string>& current_result_tokens = confab_engine.Confabulation(current_feed_tokens, -1, false);
        std::cout << e << current_result_tokens[0] << "\n" << std::flush;
    }
}

void ConfabulationTest::TestTwoLevelSimpleConfabulation(const std::string& symbolfile, const std::string& masterfile, const std::vector<std::string>& sentences) const
{
    size_t num_word_modules = 15;
    TwoLevelSimpleConfabulation confab_engine(num_word_modules, symbolfile, masterfile, 1, 5);
    TextReader reader(symbolfile, masterfile);
    reader.Initialize();

    for (const std::string& e : sentences) {
        const std::vector<std::string> current_feed_tokens(reader.ExtractTokens(e));
        FillWithEmptyStrings(current_feed_tokens, num_word_modules);
        const std::vector<std::string>& current_result_tokens = confab_engine.Confabulation(current_feed_tokens, -1, false);
        std::cout << e << VectorSymbolToSymbol(current_result_tokens, ' ') << "\n" << std::flush;
    }
}

/*
void ConfabulationTest::TestSimpleConfabulation(const std::string& symbolfile, const std::string& masterfile, const std::vector<std::string>& sentences) const
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

void ConfabulationTest::TestConfabulationWithPersistedKnowledge(const std::string& symbolfile, const std::string& supplementfile, const std::vector<std::string>& sentences) const
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

void ConfabulationTest::PerformConfabulation(Globals& globals, const std::vector<std::string>& sentences) const
{
    for (std::vector<std::string>::const_iterator it = sentences.begin(); it != sentences.end(); ++it) {
        std::vector<std::string> currentfeedTokens = globals.get_text_reader().ExtractSentenceTokens(*it);
        //std::pair<float, std::string> output = globals.getKnowledgeManager()->getConfabulatedSymbol(currentfeedTokens);
        std::vector<std::string> output = globals.get_knowledge_manager().GetLayeredConfabulatedSymbol(currentfeedTokens);
        std::cout << *it << "{";
        for (unsigned short k = 0; k < Globals::kHeapResults; k++)
            std::cout << output[k] << ", ";
        std::cout << "}" << std::endl << std::endl;
    }
}
*/

int main()
{
	std::shared_ptr<ConfabulationTest> test1(new ConfabulationTest());

    //test1->TestFixedString("This is, alas, the primal knowledge. \"My fumblings will be your quickening, minion.\"");

    //test1->TestDOKExcitationVector();

    //test1->TestDOKLinksMatrix();

    //test1->TestCSRLinksMatrix();

    //test1->TestSymbolMapping();

    //test1->TestProduceNGrams("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt");

    //test1->TestHashTrie("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt");

    //test1->TestProduceKnowledgeLinkCombinations();

    //test1->TestTransferExcitations("text_data/ascii_symbols.txt", "text_data/sample_master_debug.txt");

    //test1->TestTokenizePersistedKnowledge();

    std::string copy_feed1 = "The umbrella was a black and prosaic "; //bundle
    std::string copy_feed2 = "I will accept the post three times and refuse it "; //afterwards
    std::string copy_feed3 = "He scuttled across the room rather like a "; //rabbit
    std::string copy_feed4 = "In short, there is in life an element of elfin "; //coincidence
    std::string copy_feed5 = "At night the wicket gate is replaced by a solid "; //door
    std::string copy_feed6 = "In his own mind he had completely surrendered "; //himself
    std::string copy_feed7 = "Every one is grave in public, and funny in "; //private
    std::string copy_feed8 = "He had seen a great poet prostituting his muse to "; //journalism
    std::string copy_feed9 = "It was clear that they were the accursed of all men, and they knew not "; //why
    std::string copy_feed10 = "His long bare neck and sloping shoulders were the shape of a champagne "; //bottle

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

    //test1->TestSimpleConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt", *allCopyFeeds);

    std::shared_ptr<std::vector<std::string>> allOriginalFeeds(new std::vector<std::string>());

    std::string feed1 = "The hooded men were chanting beneath the ";
    std::string feed2 = "An army of little insects gathered on top of ";
    std::string feed3 = "The prostitute smiled and slowly caressed his ";
    std::string feed4 = "A dark winter was eating through the ";
    std::string feed5 = "The eagle circled above the valley, eager for ";
    std::string feed6 = "Our initial impression of the situation was ";
    std::string feed7 = "At the slightest hint of movement, he was ready to cut ";
    std::string feed8 = "A warm and sweet scent was emanating from her ";
    std::string feed9 = "She stumbled over her dress and fell ";
    std::string feed10 = "She had not only her husband killed, but also her ";

    std::string feed11 = "He realized that only the dance could liberate ";
    std::string feed12 = "He leaned into her, the love of his life, and whispered ";
    std::string feed13 = "He suddenly collapsed, crying like a ";
    std::string feed14 = "The cave was too high up in the mountain for him to ";
    std::string feed15 = "Suffocating in a party of dull people, he started ";
    std::string feed16 = "She wandered in her wild paths without ";
    std::string feed17 = "The khan and the whole clan were ";
    std::string feed18 = "He contemplated upon the beauty and the ";
    std::string feed19 = "With desperate effort he managed to get away from ";
    std::string feed20 = "Hosts of archers stood ready along the ";

    std::string feed21 = "The joker laughed sardonically and squeezed her ";
    std::string feed22 = "She moaned with pleasure when he ";
    std::string feed23 = "The thief viciously stabbed John while he was chopping ";
    std::string feed24 = "The green hat swirled and landed on the ";
    std::string feed25 = "Ominous waves surrounded us, just before we jumped ";
    std::string feed26 = "He was hopelessly enchanted by her ";
    std::string feed27 = "The knight decided to kill the maiden and marry the ";
    std::string feed28 = "The council sent out its most fearsome agents to ";
    std::string feed29 = "Enlightenment goes well with a cup of ";
    std::string feed30 = "The captain was listening silently for a while, but then ";

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

    //test1->TestConfabulationWithPersistedKnowledge("text_data/ascii_symbols.txt", "text_data/sample_master_supplement.txt", *allOriginalFeeds);
    //test1->TestConfabulationWithPersistedKnowledge("text_data/ascii_symbols.txt", "text_data/sample_master_empty.txt", *allOriginalFeeds);

    //test1->TestSimpleConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt", *allOriginalFeeds);
    //test1->TestSimpleConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_selection.txt", *allOriginalFeeds);
    //test1->TestTwoLevelSimpleConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_reduced.txt", *allOriginalFeeds);
    test1->TestTwoLevelSimpleConfabulation("text_data/ascii_symbols.txt", "text_data/sample_master_selection.txt", *allOriginalFeeds);

	return 0;

}
