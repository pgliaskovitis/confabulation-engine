confabulation-engine
====================

Implementation of language cognition and generation based on Robert Hecht Nielsen's Confabulation Theory.

Introduction
------------

Confabulation theory is a model of how cognition (understanding) could work in the brains of humans and animals (crows for example). 

The theory proposes collections of neurons organized into "symbols" that are in turn organized into modules that are in turn organized into hierachical architectures. "Symbols" are learned throughout childhood and can be recalled at will, as well as logically produce other symbols (logic here is meant in a propositional Aristotelian calculus sense). The logical coherence among symbols is based on discrete maximum likelihood, with the added twist that the winner symbol "takes all", i.e., it is the only activated conclusion overshadowing all others in its corresponding module. This is effectively propositional logic by means of maximum likelihood. 

The principles of the theory are very general and can be applied to all aspects of human sensory perception. A word can be a "symbol", as can be a phoneme, a particular sound signal, the output of an edge detector, or the representations learned by the hidden layers of a deep convolutional neural network. 

The theory is described in detail in Robert Hecht Nielsen's 2006 book "Confabulation Theory: The Mechanism of Thought". To test the point in hand, in the book, the theory is used as a cognitive and generative model for text data. The goal is to be given a sequence of starting words, with or without an abstract theme known, and for the system to "understand" these words and continue the sentence on its own. 

This is where the fun starts! Confabulation systems, depending on what you provide as reading material can produce any nymber of interesting results. 

Relation to other models
------------------------

Confabulation architectures are a kind of analog constraint satisfaction mechanism. The confabulation 'swirl' is essentially a parallelizable constraint satisfaction machine. In that sense, they are very related to Markov Random Fields (MRFs -- or the more specific Restricted Boltzmann Machines RBMs), although here there is no underlying joint probability distribution learning, at least not explicitly. And, unlike general MRFs, confabulation architectures are by definition parallelizable.

The temporal feedback mechanism of Recurrent Neural Networks (RNNs) does bear a resemblance to the 'swirl' operation of multiconfabulation. Multiconfabulation can effectively be thought of as a "Recurrent Maximum Likelihood" system. Of course, in confabulation there is no concept of optimizing an objective function. Furthermore, RNNs encode the whole space of possible word contexts into a single real vector (or a set of real vectors), the RNN state. Yet, there is of now no interpretable correspondence of these vectors to symbols as there could presumably be in the human brain.

The strength and intricacy of confabulation lies in that it can combine all modes of human sensory perception under a single unifying framework of abstract symbols. The concept of "apple" can be simultaneously connected to a symbol for the image of an apple, a symbol for the smell of an apple and a symbol for the taste of an apple.

As I see it, a very interesting avenue of work would be to try and combine the distributed representations of neural nets, especially for visual stimuli, with a confabulation architecture.

Possible Extensions
-------------------

In decreasing order of importance:

I) Capability to persist knowledge bases in files, most probably in their dictionary form. Learning and language generation currently happen only in-memory.

II) Capability to perform incremental knowledge learning, i.e., add the knowledge from a single new source directly to an already persisted set of knowledge bases. This would require storage of both co-occurrence counts and total occurrences for each symbol (i.e., both numerator and denominator in conditional probability terms).

III) Capability to have soft representation of symbols vs the hard representation as it stands now. Symbols are currently represented as strings and matching of the symbols on input sentences must be exact. A soft representation would allow more than one related symbols from a given input to be excited proportionally to a distance metric. Possible soft representations could be the feature vectors learned from a Deep Neural Network (also known as word vectors). 

IV) Capability to perform matrix multiplications on the GPU, i.e., using CUDA. This will speed up language generation (but not knowledge learning).

V) Capability to generate punctuation marks and especially fullstops. The goal is to move on to generation of multiple sentences.

Installation
------------

You will need to have Google SparseHash installed.

The confabulation project has been developed primarily in QtCreator. Project files for QtCreator are available. In order 
to build the project independently of QtCreator you will need the qmake facility (e.g., you can install
it via sudo apt-get-install qt4-qmake). After installing qmake, do: 

qmake -r
make clean
make 

However the project is built, i.e., either within QtCreator or with regular make, please ensure that the produced 
executable is run from the same directory that contains the text_data directory. The engine needs to read 
the texts contained there when it fires up (currently, literary books by authors of special appeal to me).
The filenames of the texts that are read can be found in the master file:

text_data/sample_master_reduced.txt

Adding more books/documents to the corpus can greatly increase the engine's capabilities. The text of a new 
document should be added under text_data/ and its filename appended to the above master file.

The main() function lies in ConfabulationTest.cpp. Test sentence beginnings can also be found and altered there.

A high level schematic of the particular confabulation architectures implemented in this project can be found in Confabulation_Rationale.pdf
