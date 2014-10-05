#ifndef HASHTRIE_H
#define HASHTRIE_H

#include <vector>
#include "HashTrieNode.hpp"

template <typename T>
class HashTrie {
public:
    HashTrie() : root_("dummy", false), size_(0) {}

    HashTrie(const std::vector<std::vector<T>> sequences) : root_("dummy", false), size_(0)
    {
        AddAll(sequences);
    }

    void Add(const std::vector<T>& sequence);
    void AddAll(const std::vector<std::vector<T>>& sequences);

    std::vector<T> FindLongest(const std::vector<T>& sequence);

    size_t Size() { return size_; }

private:
    HashTrieNode<T> root_;
    size_t size_;

};

template <typename T>
void HashTrie<T>::Add(const std::vector<T> &sequence) {

    if (sequence.empty()) {
        return;
    }

    HashTrieNode<T>* current_node_ptr = &root_;
    for (const T& e : sequence) {
        HashTrieNode<T>& current_node = current_node_ptr->Put(e, false);
        current_node_ptr = &current_node;
    }

    if (!current_node_ptr->is_leaf()) {
        current_node_ptr->set_leaf(true);
        ++size_;
    }
}

template <typename T>
void HashTrie<T>::AddAll(const std::vector<std::vector<T>>& sequences) {

    if (sequences.empty()) {
        return;
    }

    for (const std::vector<T>& e : sequences) {
        Add(e);
    }
}

template <typename T>
std::vector<T> HashTrie<T>::FindLongest(const std::vector<T>& sequence) {

    std::vector<T> result;
    std::vector<T> unconfirmed;

    HashTrieNode<T>* current_node_ptr = &root_;
    for (const T& e : sequence) {

        try {
            HashTrieNode<T>& child = current_node_ptr->Get(e);

            unconfirmed.push_back(e);
            if (child.is_leaf()) {

                for (T& u_e : unconfirmed)
                    result.push_back(std::move(u_e));

                unconfirmed.clear();
            }

            current_node_ptr = &child;
        } catch (std::out_of_range) {
            return result;
        }
    }

    return result;
}

#endif // HASHTRIE_H


