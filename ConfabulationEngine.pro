#-------------------------------------------------
#
# Project created by QtCreator 2014-05-24T11:56:06
#
#-------------------------------------------------

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O0
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

QMAKE_CXXFLAGS_DEBUG -= -O
QMAKE_CXXFLAGS_DEBUG *= -O0
QMAKE_CXXFLAGS_DEBUG -= -O1
QMAKE_CXXFLAGS_DEBUG -= -O2
QMAKE_CXXFLAGS_DEBUG -= -O3

QMAKE_CXXFLAGS_APP -= -fPIE

QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    text_processing\TextReader.cpp \
    text_processing\SentenceTokenizer.cpp \
    text_processing\NGramHandler.cpp \
    TemplateInstantiations.cpp \
    ConfabulationTest.cpp \
    Globals.cpp \
    SymbolMapping.cpp \
    KnowledgeBase.cpp \
    Module.cpp \
    MultiLevelOrganizer.cpp \
    ConfabulationBase.cpp \
    ForwardConfabulation.cpp

HEADERS += \
    sparse_structures/CSRLinksMatrix.hpp \
    sparse_structures/DOKExcitationVector.hpp \
    sparse_structures/IExcitationVector.hpp \
    sparse_structures/IKnowledgeLinks.hpp \
    sparse_structures/DOKLinksMatrix.hpp \
    text_processing\TextReader.h \
    text_processing\SentenceTokenizer.h \
    text_processing\NGramHandler.h \
    utils/HashTrie.hpp \
    utils/Utils.h \
    utils/HashTrieNode.hpp \
    TemplateInstantiations.h \
    ConfabulationTest.h \
    Globals.h \
    SymbolMapping.h \
    KnowledgeBase.h \
    Module.h \
    MultiLevelOrganizer.h \
    ConfabulationBase.h \
    ForwardConfabulation.h


