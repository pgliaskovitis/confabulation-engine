confab-engine
=============

Implementation of language cognition and generation based on Robert Hecht Nielsen's Confabulation Theory.

The project is being developed primarily in QtCreator. Project files for QtCreator are available. In order 
to build the project independently of QtCreator you will need the qmake facility (e.g., you can install
it via sudo apt-get-install qt4-qmake). After installing qmake, do: 

qmake -r
make clean
make 

However project is built, i.e., either within QtCreator or with regular make, please ensure that the produced 
executable is run from the same directory that contains the text_data directory. The engine needs to read 
the texts contained there when it fires up (currently, literary books by authors of special appeal to me).
The filenames of the texts that are read can be found in the master file:

text_data/sample_master_reduced.txt

Adding more books/documents to the corpus can greatly increase the engine's capabilities. The text of a new 
document should be added under text_data/ and its filename appended to the above master file.

The main() function lies in ConfabulationTest.cpp. Test sentence beginnings can also be found and altered there.

A high level schematic of the particular confabulation architectures implemented in this project can be found in Confabulation_Rationale.pdf
