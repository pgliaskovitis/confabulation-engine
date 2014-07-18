/*
 * ConfabulationTest.h
 *
 *  Created on: Jun 1, 2013
 *      Author: Periklis G. Liaskovitis
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
    int TestSharedPointer() const;

    int TestTokenizeFixedString(const Symbol& input) const;
    int TestReadSymbolFile(const Symbol& input) const;
    int TestReadFixedFile(const Symbol& symbolfile, const Symbol& textfile) const;
    int TestReadMultipleFiles(const Symbol& symbolfile, const Symbol& masterfile) const;

    void TestDOKExcitationVector() const;
    void TestDOKLinksMatrix() const;
    void TestCSRLinksMatrix() const;

    void TestNGrams() const;

    void TestTokenizePersistedKnowledge() const;

    void TestSimpleConfabulation(const Symbol& symbolfile, const Symbol& masterfile, const std::vector<Symbol>& sentences) const;
    void TestConfabulationWithPersistedKnowledge(const Symbol& symbolfile, const Symbol& supplementfile, const std::vector<Symbol>& sentences) const;

private:
    void PerformConfabulation(Globals& globals, const std::vector<Symbol>& sentences) const;
};

#endif /* CONFABULATIONTEST_H_ */
