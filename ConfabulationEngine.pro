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
    ConfabulationTest.cpp \
    Globals.cpp \
    KnowledgeBase.cpp \
    KnowledgeManager.cpp \
    SentenceTokenizer.cpp \
    SymbolAttribute.cpp \
    TextReader.cpp \
    sparse_structures/CSRLinksMatrix.cpp \
    sparse_structures/DOKExcitationVector.cpp

HEADERS += \
    ConfabulationTest.h \
    Globals.h \
    KnowledgeBase.h \
    KnowledgeManager.h \
    SentenceTokenizer.h \
    SymbolAttribute.h \
    TextReader.h \
    sparse_structures/IExcitationVector.h \
    sparse_structures/IKnowledgeLinks.h \
    sparse_structures/CSRLinksMatrix.h \
    sparse_structures/DOKExcitationVector.h
