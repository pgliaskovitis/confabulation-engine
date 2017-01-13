confabulation-engine
====================

Implementation of language cognition and generation based on Robert Hecht Nielsen's Confabulation Theory.

"One of the most exasperating things about confabulation theory is that it seems inpossible that just forming links between co-occurring symbols and then using these links to approximately maximize cogency could ever yield anything resembling human cognition. The theory appears to be nothing but a giant mountain of wishful thinking! That such a simple construction can do all of cognition is indeed astounding."

"The nature of cognition is very different from that of computing. So much depends upon designing clever architectures of modules and knowledge bases and upon using clever, highly threaded, but very simple thought processes to control these architectures."

                                  Confabulation Theory - The Mechanism of Thought, --Robert Hecht Nielsen

Introduction
------------

Confabulation theory is a model of how cognition (understanding) could work in the brains of humans and animals (crows for example). It is essentially a _**massively parallelizable graph-based model**_ for cognition.

Imagine a graph node that can take on any one of multiple values/features. These values/features can be anything, e.g., strings, real vectors etc. Graph nodes are *modules*. Node values are *symbols*. 

A *confabulation architecture* is a directed graph of multi-valued nodes. 

Nodes in the graph are connected to each other with directed edges. Edges are *knowledge links*. An edge passes energy (excitation) from a source node to a target node, multiplied by its weight. However, the weight must encompass all pairs of possible values (symbols) of the source and target nodes, i.e., the edge weight is a matrix (or tensor) itself. The weight represents the conditional probability of the source symbol occurring given the target symbol has occurred, i.e., it is a likelihood value.

*Confabulation* is contraction of the values professed by a node. Partial contraction (partial confabulation) can happen by silencing those symbols that are excited below a threshold. Extreme contraction (elementary confabulation) keeps only the module symbol with the maximum excitation and silences all others.

A *thought process* is a highly concurrent program passing around energy in the graph and contracting the nodes (confabulating), so that, in the end, every node in the architecture professes a single value, i.e., a single symbol.

The symbols and knowledge links can be learned _**by any appropriate workable means**_, i.e., _**any mathematical model that learns features or likelihoods**_ (e.g., standard neural nets).

Thought processes are themselves confabulation architectures. The confabulation _swirl_ is such a thought process.

The interesting problem is how to learn new and effective thought processes. Embodied intelligence may be a way to do this.

A frequently overlooked implicit assumption of the theory is that the modules must have many, thousands or tens of thousands of, highly specialized symbols so that only a small percentage of them is ever excited in any given module. Consequently the edges (knowledge links) and resulting maximum likelihood transfers are also highly specialized. Otherwise all module symbols would always be excited and huge graphs would be required to incorporate long term dependencies and context meaningfully. For example, a confabulation language model with modules based on characters (very few symbols per module) would probably not work easily.

The principles of the theory are very general and can be applied to all aspects of human sensory perception. To test the point in hand the theory is used as a cognitive and generative model for text data. The goal is, given a sequence of starting words, with or without an abstract theme known, for the system to "understand" these words and continue the sentence on its own. This is where the fun starts! Confabulation systems, depending on what you provide as reading material can produce any nymber of interesting results. 

Relation to other models
------------------------

At the neuronal level, confabulation theory allows for modules to be neural attractor networks having the symbols as stable states and for knowledge links to also be neural networks. However, until the simulation of massive separate neural sub-networks is possible in hardware, handling things at the neuronal level to get results initially proves unwieldy. 

At the symbolic level, confabulation architectures are very related to Markov Random Fields (MRFs -- or the more specific Restricted Boltzmann Machines RBMs), Attractor Neural Networks and Energy Models in general. However, here there is no underlying joint probability distribution learning and no energy minimization objectives, at least not explicitly. And, unlike general MRFs, confabulation architectures are by definition massively parallelizable.

The temporal feedback mechanism of Recurrent Neural Networks (RNNs) does bear a resemblance to the swirl thought process. The latter can effectively be thought of as "Recurrent Maximum Likelihood". Of course, in confabulation there is no concept of optimizing an objective function. Furthermore, RNNs encode the whole space of possible word contexts into a single real vector (or a set of real vectors), the RNN state. Yet, there is of now no interpretable correspondence of these vectors to symbols as there could presumably be in the human brain. 

The strength and intricacy of confabulation lies in that it can combine all modes of human sensory perception under a single unifying framework of abstract symbols. The concept of "apple" can be simultaneously connected to a symbol for the image of an apple, a symbol for the smell of an apple and a symbol for the taste of an apple. A word can be a "symbol", as can be a phoneme, a particular sound signal, the output of an edge detector, or the representations learned by the hidden layers of a deep convolutional neural network. 

As I see it, a very interesting avenue of work would be to try and combine the distributed representations of neural nets, especially for visual stimuli, with a confabulation architecture. 

Possible Extensions
-------------------

In decreasing order of importance:

I) Capability to persist knowledge bases in files, most probably in their dictionary form. Learning and language generation currently happen only in-memory.

II) Capability to perform incremental knowledge learning, i.e., add the knowledge from a single new source directly to an already persisted set of knowledge bases. This would require storage of both co-occurrence counts and total occurrences for each symbol (i.e., both numerator and denominator in conditional probability terms).

III) Capability to have soft representation of symbols vs the hard representation as it stands now. Symbols are currently represented as strings and matching of the symbols on input sentences must be exact. A soft representation would allow more than one related symbols from a given input to be excited proportionally to a distance metric. Possible soft representations could be the feature vectors learned from a Deep Neural Network (also known as word vectors). 

IV) Capability to perform matrix multiplications on the GPU, i.e., using CUDA. This will speed up language generation (but not knowledge learning).

V) Capability to generate punctuation marks and especially fullstops. The goal is to move on to generation of multiple sentences.

VI) Capability to incorporate excitations for partially known symbols, e.g., parts of words.

Installation
------------

You will need to have Google SparseHash installed.

The confabulation project has been developed primarily in QtCreator. Project files for QtCreator are available. In order 
to build the project independently of QtCreator you will need the qmake facility:

sudo apt-get install qt5-default

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
