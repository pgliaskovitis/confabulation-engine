confabulation-engine
====================

Implementation of language cognition and generation based on Robert Hecht Nielsen's Confabulation Theory.

"One of the most exasperating things about confabulation theory is that it seems impossible that just forming links between co-occurring symbols and then using these links to approximately maximize cogency could ever yield anything resembling human cognition. The theory appears to be nothing but a giant mountain of wishful thinking! That such a simple construction can do all of cognition is indeed astounding. Yet that is precisely the claim of confabulation theory."

"The nature of cognition is very different from that of computing. So much depends upon designing clever architectures of modules and knowledge bases and upon using clever, highly threaded, but very simple thought processes to control these architectures."

                                  Confabulation Theory - The Mechanism of Thought, --Robert Hecht Nielsen

Introduction
------------

Confabulation theory is a model of how cognition (understanding) could work in the brains of humans and animals (crows for example). It is essentially a _**massively parallelizable graph-based model**_ for cognition.

Imagine a graph node that can take on any one of multiple values/features. These values/features can be anything, e.g., strings, real vectors etc. Graph nodes are *modules*. Node values are *symbols*.

- A *confabulation architecture* is a directed graph of multi-valued nodes.

- Nodes in the graph are connected to each other with directed edges. Edges are *knowledge links*. An edge passes energy (excitation) from a source node to a target node, multiplied by its weight. However, the weight must encompass all pairs of possible values (symbols) of the source and target nodes, i.e., the edge weight is a matrix (or tensor) itself.

- The weight of the knowledge link represents the likelihood of the _target_ symbol occurring, given that the _source_ symbol has already occurred. Hence it is the value of a _**likelihood function**_ regarding the target symbol as the _**argument of the function**_.

- *Confabulation* is contraction of the values professed by a node. Partial contraction (partial confabulation) can happen by silencing those symbols that are excited below a threshold. Extreme contraction (elementary confabulation) keeps only the module symbol with the maximum excitation and silences all others. Confabulation of a node is the basic means of performing _**maximum likelihood estimation locally**_ on the graph.

- A *thought process* is a highly concurrent program passing around excitations in the graph and contracting the nodes (confabulating), so that, in the end, every node in the architecture professes a single value, i.e., a single symbol. The thought process is essentially thus a parallelizable algorithm to perform _**maximum likelihood estimation globally**_ on the graph.

- The symbols and knowledge links can be learned _**by any appropriate workable means**_, i.e., _**any mathematical model that learns features**_ (e.g., standard deep neural nets).

- Thought processes are themselves confabulation architectures. The confabulation _swirl_ is such a thought process.

A frequently overlooked implicit assumption of the above is that the modules must have many, thousands or tens of thousands of, highly specialized symbols so that only a small percentage of them is ever excited in any given module. Consequently the edges (knowledge links) and resulting maximum likelihood transfers are also highly specialized. Otherwise all module symbols would always be excited and huge graphs would be required to incorporate long term dependencies and context meaningfully. For example, a confabulation language model with modules based on characters (very few symbols per module) would probably not work easily.

The principles of the theory are very general and can be applied to all aspects of human sensory perception. To test the point in hand the theory is used as a cognitive and generative model for text data. The goal is, given a sequence of starting words, with or without an abstract theme known, for the system to "understand" these words and continue the sentence on its own. This is where the fun starts! Confabulation systems, depending on what you provide as reading material can produce any nymber of interesting results.

Relation to other models
------------------------

- The primary difference compared to vanilla language models is that confabulation uses the likelihood function and not the conditional probability function. For example:

  Common trigram language model: Select w_3 that maximizes P(W_3 | W_1 = w_1, W_2 = w_2)

  Confabulation language model: Select w_3 that maximizes P(W_1 = w_1, W_2 = w_2 | W_3)

  These are starkly different optimization objectives, i.e., in confabulation each successive symbol is considered as a
  _parameter_ in the model for the already observed symbols. Afterwards it becomes itself one of the observed symbols. The
  likelihood is essentially used to estimate parameters of the model which in this case are also data values.

- At the neuronal level, confabulation theory allows for modules to be neural attractor networks having the symbols as stable states and for knowledge links to also be neural networks. However, until the simulation of massive separate neural sub-networks is possible in hardware, handling things at the neuronal level to get results initially proves unwieldy.

- At the symbolic level, confabulation architectures are very related to Markov random fields, mean field theory and energy models in general. Likelihood as a product of antecedent symbol likelihoods is reminiscent of the mean field variational approximation. However, here there is no underlying joint probability distribution learning. And, unlike general MRFs, confabulation architectures are by definition massively parallelizable.

- The swirl thought process is reminiscent of the temporal feedback mechanism of Recurrent Neural Networks (RNNs). The swirl can effectively be thought of as "Recurrent Maximum Likelihood". Of course, in confabulation there is no concept of optimizing an objective function. Furthermore, RNNs encode the whole space of possible word contexts into a single real vector (or a set of real vectors), the RNN state. Yet, there is of now no interpretable correspondence of these vectors to symbols as there could presumably be in the human brain.

- The strength and intricacy of confabulation lies in that it can combine all modes of human sensory perception under a single unifying framework of abstract symbols. A word can be a "symbol", as can be a phoneme, a particular sound signal, the output of an edge detector, or the representations learned by the hidden layers of a deep convolutional neural network. The concept of "apple" can be simultaneously connected to a symbol for the image of an apple, a symbol for the smell of an apple and a symbol for the taste of an apple. A thought process for recognizing apples is also a symbol.

Possible Extensions
-------------------

In decreasing order of importance:

1. Actually make language generation run on multiple threads. Right now it is single-threaded with some infrastructure for multi-threading, but confabulation results are not being duplicated in the latter case.

2. Capability to persist knowledge bases in files, most probably in their dictionary form. Learning and language generation currently happen only in-memory.

3. Capability to perform incremental knowledge learning, i.e., add the knowledge from a single new source directly to an already persisted set of knowledge bases. This would require storage of both co-occurrence counts and total occurrences for each symbol (i.e., both numerator and denominator in conditional probability terms).

4. Capability to have soft representation of symbols vs the hard representation as it stands now. Symbols are currently represented as strings and matching of the symbols on input sentences must be exact. A soft representation would allow more than one related symbols from a given input to be excited proportionally to a distance metric. Possible soft representations could be the feature vectors learned from a Deep Neural Network (also known as word vectors).

5. Capability to perform matrix multiplications on the GPU, i.e., using CUDA or OPENCL. This will speed up language generation.

6. Capability to generate punctuation marks and especially fullstops. The goal is to move on to generation of multiple sentences.

7. Capability to incorporate excitations for partially known symbols, e.g., parts of words. This is also related to 4.

Installation
------------

You will need to have Google SparseHash installed at the root source tree, in a "google" directory. A suitable version has been included for convenience in later versions of this repo.

You will also need khash.h in a "klib" directory, obtainable from https://github.com/attractivechaos/klib (also check out the excellent blog). A suitable version has been included for convenience in later versions of this repo.

Finally you will need bytell_hash_map.hpp in a "flat_hash_map" directory obtainable from https://github.com/skarupke/flat_hash_map (also check out another escellent blog). A suitable version has been included for convenience in later versions of this repo.

The currently preferred way to build is through cmake:

- sudo apt install cmake

- mkdir build

- cd build

- cmake ..

- cmake --build . --config Release

Please ensure that the produced executable is run from the same directory that contains the text_data directory. The engine needs to read the texts contained there when it fires up (currently, literary books by authors of special appeal to me).

The filenames of the texts that are read can be found in the master file:

text_data/sample_master_reduced.txt

Adding more books/documents to the corpus can greatly increase the engine's capabilities. The text of a new document should be added under text_data/ and its filename appended to the above master file.

The main() function lies in ConfabulationTest.cpp. Test sentence beginnings can also be found and altered there.

A high level schematic of the particular confabulation architectures implemented in this project can be found in Confabulation_Rationale.pdf

Sample Results
--------------

These are cherry-picked after training with default settings and 300 MB of text data:

His quest was one of *{ the battles of the heart }*

He suddenly collapsed and started crying like a *{ child left alone }*

A dark winter was eating through the *{ plains of the north }*

Hosts of archers stood ready along the *{ bridge road }*

Enlightenment goes well with a *{ bucket of cold water }*
