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

#ifndef HASHTRIE_H
#define HASHTRIE_H

#include <vector>
#include <list>
#include "HashTrieNode.hpp"
#include "Utils.h"

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

    std::list<T> FindLongest(const std::list<T>& sequence) const;

    std::vector<std::list<T>> FindAll(const std::list<T>& sequence) const;

    size_t Size() const { return size_; }

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
std::list<T> HashTrie<T>::FindLongest(const std::list<T>& sequence) const {

    std::list<T> result;
    std::list<T> unconfirmed;

    const HashTrieNode<T>* current_node_ptr = &root_;
    for (const T& e : sequence) {

        try {
            const HashTrieNode<T>& child = current_node_ptr->Get(e);

            unconfirmed.push_back(e);
            if (child.is_leaf()) {
                for (T& u_e : unconfirmed) {
                    result.push_back(std::move(u_e));
                }
                unconfirmed.clear();
            }

            current_node_ptr = &child;
        } catch (std::out_of_range) {
            return result;
        }
    }

    return result;
}

template <typename T>
std::vector<std::list<T>> HashTrie<T>::FindAll(const std::list<T>& sequence) const {

    std::vector<std::list<T>> result;
    std::list<T> partial;
    bool partial_is_symbol;

    const HashTrieNode<T>* current_node_ptr = &root_;

    for (const T& e : sequence) {
        try {
            const HashTrieNode<T>& child = current_node_ptr->Get(e);
            partial.push_back(e);
            if (child.is_leaf()) {
                partial_is_symbol = true;
            }
            current_node_ptr = &child;
        } catch (std::out_of_range) {
        }

        if (partial.size() > 1 && partial_is_symbol) {
            std::list<T> partial_copy(partial.begin(), partial.end());
            result.push_back(std::move(partial_copy));
            partial_is_symbol = false;
        }
    }

    return result;
}

#endif // HASHTRIE_H


