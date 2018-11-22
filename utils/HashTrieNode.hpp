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

#pragma once

#include <stdexcept>
#include <memory>
#include <unordered_map>

template<typename T>
class HashTrieNode {
public:
	HashTrieNode(T value, bool final_node) :
		value_(value),
		leaf_node_(final_node) {}

	bool is_leaf() const {
		return leaf_node_;
	}

	void set_leaf(bool final_node) {
		leaf_node_ = final_node;
	}

	HashTrieNode& Get(const T& child) const {
		return *(map_.at(child));
	}

	HashTrieNode& Put(const T& child, bool final_node);

	const T& get_value() const { return value_; }

private:
	std::unordered_map<T, std::unique_ptr<HashTrieNode<T>>> map_;

	T value_;
	bool leaf_node_; // leaf node is a separate extra node
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

	std::unique_ptr<HashTrieNode<T>> new_child(new HashTrieNode<T>(child, final_node));
	map_.insert(std::make_pair(child, std::move(new_child)));

	return *(map_.at(child));
}
