/*
 * Copyright 2014 Periklis Liaskovitis
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

#ifndef HASHTRIENODE_H
#define HASHTRIENODE_H

#include <stdexcept>
#include <memory>
#include <unordered_map>

template<typename T>
class HashTrieNode {
public:
    HashTrieNode(T value, bool final_node) :
        value_(value),
        leaf_node_(final_node) {}

    bool is_leaf() {
        return leaf_node_;
    }

    void set_leaf(bool final_node) {
        leaf_node_ = final_node;
    }

    HashTrieNode& Get(const T& child) {
        return *(map_.at(child));
    }

    HashTrieNode& Put(const T& child, bool final_node);

private:
    std::unordered_map<T, std::shared_ptr<HashTrieNode<T>>> map_;

    T value_;
    bool leaf_node_;
};

template <typename T>
HashTrieNode<T>& HashTrieNode<T>::Put(const T& child, bool final_node) {

    try {
        HashTrieNode<T>& child_node = *(map_.at(child));
        if (final_node) {
            child_node.set_leaf(final_node);
        }
        return child_node;
    } catch (std::out_of_range& oor) {
    }

    std::shared_ptr<HashTrieNode<T>> new_child(new HashTrieNode<T>(child, final_node));
    map_.insert(std::make_pair(child, std::move(new_child)));

    return *(map_.at(child));
}

#endif // HASHTRIENODE_H
