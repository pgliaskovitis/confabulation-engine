confabulation-engine
====================

Implementation of language cognition and generation based on Robert Hecht Nielsen's Confabulation Theory.

The temporal feedback mechanism of Recurrent Neural Networks does bear a resemblance to the 'swirl' operation of multiconfabulation. Multiconfabulation can effectively be thought of as a "Recurrent Maximum Likelihood" system. Of course, in confabulation there is no concept of optimizing an objective function.

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
