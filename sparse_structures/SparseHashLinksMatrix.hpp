#ifndef SPARSEHASHLINKSMATRIX_HPP
#define SPARSEHASHLINKSMATRIX_HPP

#include <google/sparse_hash_map>
#include "IKnowledgeLinks.hpp"

template <typename T>
class SparseHashLinksMatrix : public IKnowledgeLinks<T>
{
public:
	SparseHashLinksMatrix(uint16_t num_rows, uint16_t num_cols);
	SparseHashLinksMatrix(IKnowledgeLinks<T>& base);

	SparseHashLinksMatrix(const SparseHashLinksMatrix& rhs) = delete;
	SparseHashLinksMatrix& operator=(const SparseHashLinksMatrix& rhs) = delete;
	SparseHashLinksMatrix(SparseHashLinksMatrix&& rhs) = delete;
	SparseHashLinksMatrix&& operator=(SparseHashLinksMatrix&& rhs) = delete;

	~SparseHashLinksMatrix();

	virtual void SetElement(uint16_t r, uint16_t c, const T& value);
	virtual void SetElementQuick(uint16_t r, uint16_t c, const T& value);

	virtual T GetElement(uint16_t r, uint16_t c) const;
	virtual T GetElementQuick(uint16_t r, uint16_t c) const;

	virtual uint16_t get_num_rows() const { return num_rows_; }
	virtual uint16_t get_num_cols() const { return num_cols_; }

	virtual uint32_t GetNnz() const { return map_.size(); }

	virtual std::unique_ptr<IExcitationVector<T>> Multiply(const IExcitationVector<T>& vec) const;
	virtual std::set<std::pair<std::pair<uint16_t, uint16_t>, T> > GetNzElements() const;

private:
	const uint16_t num_rows_;
	const uint16_t num_cols_;

	google::sparse_hash_map<std::pair<uint16_t, uint16_t>, T, PairHash, PairEquals> map_;
};

template <typename T>
SparseHashLinksMatrix<T>::SparseHashLinksMatrix(uint16_t num_rows, uint16_t num_cols) : num_rows_(num_rows), num_cols_(num_cols)
{}

template <typename T>
SparseHashLinksMatrix<T>::SparseHashLinksMatrix(IKnowledgeLinks<T> &base) : num_rows_(base.get_num_rows()), num_cols_(base.get_num_cols())
{
	for (const std::pair<std::pair<uint16_t, uint16_t>, T>& element : base.GetNzElements()) {
		map_.insert(element);
	}
}

template <typename T>
SparseHashLinksMatrix<T>::~SparseHashLinksMatrix()
{
	map_.clear();
}

template <typename T>
T SparseHashLinksMatrix<T>::GetElement(uint16_t r, uint16_t c) const
{
	IKnowledgeLinks<T>::CheckBounds(r, c);
	return GetElementQuick(r, c);
}

template <typename T>
T SparseHashLinksMatrix<T>::GetElementQuick(uint16_t r, uint16_t c) const
{
	T result;

	typename google::sparse_hash_map<std::pair<uint16_t, uint16_t>, T, PairHash, PairEquals>::const_iterator it = map_.find(std::make_pair(r, c));
	if (it != map_.end()) {
		result = it->second;
	} else {
		result = 0.0;
	}

	return result;
}

template <typename T>
void SparseHashLinksMatrix<T>::SetElement(uint16_t r, uint16_t c, const T &value)
{
	IKnowledgeLinks<T>::CheckBounds(r, c);
	if (IsNearlyEqual(value, 0.0)) {
		typename google::sparse_hash_map<std::pair<uint16_t, uint16_t>, T, PairHash, PairEquals>::iterator it = map_.find(std::make_pair(r, c));
		if (it != map_.end()) {
			map_.erase(it);
		}
	} else {
		SetElementQuick(r, c, value);
	}
}

template <typename T>
void SparseHashLinksMatrix<T>::SetElementQuick(uint16_t r, uint16_t c, const T& value)
{
	map_[std::make_pair(r, c)] = value;
}

template <typename T>
std::unique_ptr<IExcitationVector<T>> SparseHashLinksMatrix<T>::Multiply(const IExcitationVector<T>& vec) const
{
	std::set<std::pair<uint16_t, T>> vec_elements = vec.GetNzElements();

	T link_strength = 0.0;
	std::unique_ptr<IExcitationVector<T>> result(new DOKExcitationVector<T>(num_rows_));

	for (const std::pair<uint32_t, T>& element : vec_elements) {
		uint32_t c = element.first;
		for (uint16_t r = 0; r < num_rows_; ++r) {
			link_strength = GetElementQuick(r, c);
			if (!IsNearlyEqual(link_strength, 0.0)) {
				result->SetElement(r, result->GetElementQuick(r) + link_strength * element.second);
			}
		}
	}

	return result;
}

template <typename T>
std::set<std::pair<std::pair<uint16_t, uint16_t>, T>> SparseHashLinksMatrix<T>::GetNzElements() const
{
	typename std::set<std::pair<std::pair<uint16_t, uint16_t>, T>> result;

	for (typename google::sparse_hash_map<std::pair<uint16_t, uint16_t>, T, PairHash, PairEquals>::const_iterator it = map_.begin(); it != map_.end(); ++it) {
		result.insert(*it);
	}

	return result;
}

#endif // SPARSEHASHLINKSMATRIX_HPP
