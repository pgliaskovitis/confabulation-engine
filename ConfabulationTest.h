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

#ifndef CONFABULATIONTEST_H_
#define CONFABULATIONTEST_H_

#include <memory>
#include <vector>

class ConfabulationTest
{
public:
    int8_t TestTokenizeFixedString(const std::string& input) const;

    void TestDOKExcitationVector() const;
    void TestDOKLinksMatrix() const;
    void TestCSRLinksMatrix() const;

    void TestSymbolMapping() const;

    void TestTokenizePersistedKnowledge() const;

    void TestProduceNGrams(const std::string& symbolfile, const std::string& masterfile) const;

    void TestHashTrie(const std::string& symbolfile, const std::string& masterfile) const;

    void TestProduceKnowledgeLinkCombinations() const;

    void TestTransferExcitations(const std::string& symbolfile, const std::string& masterfile) const;

    void TestSimpleConfabulation(const std::string& symbolfile, const std::string& masterfile, const std::vector<std::string>& sentences) const;

    void TestTwoLevelSimpleConfabulation(const std::string& symbolfile, const std::string& masterfile, const std::vector<std::string>& sentences) const;

private:
    void PerformConfabulation(Globals& globals, const std::vector<std::string>& sentences) const;
};

#endif /* CONFABULATIONTEST_H_ */
