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

#ifndef CONFABULATIONTEST_H_
#define CONFABULATIONTEST_H_

#include <memory>
#include <vector>

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

//solely for testing shared pointers
struct C
{
	int a;
	int b;
};

class Globals;

class ConfabulationTest
{
public:
    int TestTokenizeFixedString(const Symbol& input) const;
    int TestReadSymbolFile(const Symbol& input) const;
    int TestReadFixedFile(const Symbol& symbolfile, const Symbol& textfile) const;
    int TestReadMultipleFiles(const Symbol& symbolfile, const Symbol& masterfile) const;

    void TestDOKExcitationVector() const;
    void TestDOKLinksMatrix() const;
    void TestCSRLinksMatrix() const;

    void TestSymbolMapping() const;

    void TestNGrams(const Symbol& symbolfile, const Symbol& masterfile) const;

    void TestHashTrie(const Symbol& symbolfile, const Symbol& masterfile) const;

    void TestProduceKnowledgeLinkCombinations() const;

    void TestTokenizePersistedKnowledge() const;

    void TestSimpleConfabulation(const Symbol& symbolfile, const Symbol& masterfile, const std::vector<Symbol>& sentences) const;
    void TestConfabulationWithPersistedKnowledge(const Symbol& symbolfile, const Symbol& supplementfile, const std::vector<Symbol>& sentences) const;

private:
    void PerformConfabulation(Globals& globals, const std::vector<Symbol>& sentences) const;
};

#endif /* CONFABULATIONTEST_H_ */
