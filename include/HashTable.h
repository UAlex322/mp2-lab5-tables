#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <functional>
#include <initializer_list>
using namespace std;

template <typename Key, typename Value>
class HashTable {

	struct Entry {
		Key key;
		Value *value_ptr;

		Entry(const Key &key = Key(), Value *const value_ptr = nullptr): key(key), value_ptr(value_ptr) {}
	};

public:

	// NESTED CLASSES

	class iterator {
		friend class HashTable;

		using list_iterator = typename list<Entry>::iterator;
		using value_type = Entry;
		using pointer = Entry*;
		using reference = Entry&;
		using difference_type = ptrdiff_t;
		using iterator_category = std::bidirectional_iterator_tag;

		iterator(HashTable &ht, const list_iterator &it, int index): ht(ht), it(it), index(index) {}

	public:

		reference operator*() const { return *it; }
		pointer operator->() const {return &(*it); }

		iterator& operator++() { 
			++it;

			if (it == ht.bucket[index].end()) {
				++index;
				while (ht.bucket[index].empty() && index < ht.size_)
					++index;

				if (index == ht.size_) {
					it = ht.bucket.back().end();
					index = ht.num_of_buckets - 1;
				}
				else 
					it = ht.bucket[index].begin();
			}

			return *this;
		}
		iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

		iterator& operator--() {
			if (it == ht.bucket[index].begin()) {
				--index;
				while (ht.bucket[index].empty() && index >= 0)
					--index;

				if (index == -1)
					it = prev(ht.bucket[0].begin());
				else
					it = prev(ht.bucket[index].end());
			}
			else
				--it;

			return *this;
		}
		iterator operator--(int) {iterator tmp = *this; --(*this); return tmp; }

		bool operator==(const iterator &iter) const { return index == iter.index && this->it == iter.it; }
		bool operator!=(const iterator &iter) const {return index != iter.index || this->it != iter.it; }

	private:
		HashTable &ht; // исходная таблица нужна, чтобы обращаться к её полям
		list_iterator it;
		int index;
	};

	class const_iterator {
		friend class HashTable;

		using list_iterator = typename list<Entry>::const_iterator;
		using value_type = const Entry;
		using pointer = const Entry*;
		using reference = const Entry&;
		using difference_type = ptrdiff_t;
		using iterator_category = std::bidirectional_iterator_tag;

		const_iterator(const HashTable &ht, const list_iterator &it, int index): ht(ht), it(it), index(index) {}

	public:

		reference operator*() const { return *it; }
		pointer operator->() const {return &(*it); }

		const_iterator& operator++() { 
			++it;

			if (it == ht.bucket[index].cend()) {
				++index;
				while (ht.bucket[index].empty() && index < ht.size_)
					++index;

				// проверка, прошёл ли итератор через все элементы
				if (index == ht.size_) {
					it = ht.bucket.back().cend();
					index = ht.num_of_buckets - 1;
				}
				else
					it = ht.bucket[index].cbegin();
			}

			return *this;
		}
		const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }

		const_iterator& operator--() {
			if (it == ht.bucket[index].begin()) {
				--index;
				while (ht.bucket[index].empty() && index >= 0)
					--index;

				// проверка, прошёл ли итератор через все элементы
				if (index == -1)
					it = prev(ht.bucket[0].cbegin());
				else
					it = prev(ht.bucket[index].cend());
			}
			else
				--it;

			return *this;
		}
		const_iterator operator--(int) {const_iterator tmp = *this; --(*this); return tmp; }

		bool operator==(const iterator &iter) const { return index == it.index && this->it == iter.it; }
		bool operator!=(const iterator &iter) const { return index != it.index || this->it != iter.it; }

	private:
		const HashTable &ht; // исходная таблица нужна, чтобы обращаться к её полям
		list_iterator it;
		int index;
	};

	// METHODS

	HashTable(): num_of_buckets(8), size_(0), key_mask(num_of_buckets-1) {
		bucket.resize(num_of_buckets);
	}

	HashTable(const HashTable &copy): bucket(copy.bucket), num_of_buckets(copy.num_of_buckets), size_(copy.size_), key_mask(copy.key_mask) {}
	HashTable(HashTable &&mov): bucket(move(mov.bucket)), num_of_buckets(mov.num_of_buckets), size_(mov.size_), key_mask(mov.key_mask) {}

	~HashTable() {
		clear();
	}

	iterator find(const Key &key);
	void insert(const Key &key, const Value &value);
	void erase(const Key &key);
	Value& operator[](const Key &key);
	const Value& operator[](const Key &key) const;

	size_t size() {return size_;}
	void clear();
	bool empty() const noexcept {
		return size_ == 0;
	}

	iterator begin();
	iterator end();
	const_iterator cbegin();
	const_iterator cend();

#ifdef ENABLE_PROFILING
	size_t get_find_ops() const { return find_ops; }
	size_t get_insert_ops() const { return insert_ops; }
	size_t get_erase_ops() const { return erase_ops; }
	size_t get_brackets_ops() const { return brackets_ops; }
	size_t get_clear_ops() const { return clear_ops; }
#endif

private:
	vector<list<Entry>> bucket;
	std::hash<Key> hash;
	size_t num_of_buckets;
	size_t size_;
	size_t key_mask;
	double max_load_factor = 1.0;
#ifdef ENABLE_PROFILING
	size_t find_ops;
	size_t insert_ops;
	size_t erase_ops;
	size_t clear_ops;
	size_t brackets_ops;
#endif

	// служебный конструктор с возможностью задания размера
	HashTable(size_t num_of_buckets): num_of_buckets(num_of_buckets), size_(0), key_mask(num_of_buckets-1) {
		bucket.resize(num_of_buckets);
	}

	iterator common_find(const Key &key);

	// вставка ключа и указателя на данные
	void insert_fast(const Key &key, Value *const &value_ptr) {
		bucket[hash(key) & key_mask].emplace_front(key, value_ptr);
	}

	// рехеш
	void rehash();
};

template <typename Key, typename Value>
typename HashTable<Key,Value>::iterator HashTable<Key,Value>::begin() {
	int index = 0;

	while (index < num_of_buckets && bucket[index].empty())
		++index;

	if (index == num_of_buckets)
		return iterator(*this, bucket.back().end(), num_of_buckets-1);
	return iterator(*this, bucket[index].begin(), index);
}

template <typename Key, typename Value>
typename HashTable<Key,Value>::iterator HashTable<Key,Value>::end() {
	return iterator(*this, bucket.back().end(), num_of_buckets-1);
}

template <typename Key, typename Value>
typename HashTable<Key,Value>::const_iterator HashTable<Key,Value>::cbegin() {
	int index = 0;

	while (bucket[index].empty() && index < num_of_buckets)
		++index;

	if (index == num_of_buckets)
		return const_iterator(*this, bucket.back().cend(), num_of_buckets-1);
	return const_iterator(*this, bucket[index].cbegin(), index);
}

template <typename Key, typename Value>
typename HashTable<Key,Value>::const_iterator HashTable<Key,Value>::cend() {
	return const_iterator(*this, bucket.back().cend(), num_of_buckets-1);
}

template <typename Key, typename Value>
typename HashTable<Key,Value>::iterator HashTable<Key,Value>::common_find(const Key &key) {
#ifdef ENABLE_PROFILING
	find_ops = 1;
#endif

	size_t hash_val = hash(key);
	// Итераторы списка - текущий и конечный
	auto it = bucket[hash_val & key_mask].begin(),
		end = bucket[hash_val & key_mask].end();

	while (it != end) {
	#ifdef ENABLE_PROFILING
		++find_ops;
	#endif
		if ((*it).key == key)
			return iterator(*this, it, hash_val);
		++it;
	}

	return HashTable::end();
}

template <typename Key, typename Value>
typename HashTable<Key,Value>::iterator HashTable<Key,Value>::find(const Key &key) {
	auto it = common_find(key);

	return it;
}

template <typename Key, typename Value>
void HashTable<Key,Value>::insert(const Key &key, const Value &value) {
#ifdef ENABLE_PROFILING
	insert_ops = 0;
#endif
	if (common_find(key) != end())
		throw string("Entry with this key already exists");

#ifdef ENABLE_PROFILING
	insert_ops += find_ops;
#endif
	bucket[hash(key) & key_mask].emplace_front(key, new Value(value));
	++size_;
#ifdef ENABLE_PROFILING
	insert_ops += 2;
#endif

	if (size_ > num_of_buckets * max_load_factor) rehash();
}

template <typename Key, typename Value>
void HashTable<Key,Value>::erase(const Key &key) {
#ifdef ENABLE_PROFILING
	erase_ops = 0;
#endif
	auto iter = common_find(key);
	if (iter == end())
		throw string("No entry with this key exists");

#ifdef ENABLE_PROFILING
	erase_ops += find_ops;
#endif
	delete (*iter).value_ptr;
	bucket[hash(key) & key_mask].erase(iter.it);
	--size_;

#ifdef ENABLE_PROFILING
	erase_ops += 3;
#endif
}

template <typename Key, typename Value>
void HashTable<Key, Value>::rehash() {
	HashTable new_table(num_of_buckets*2);

	for (auto it: *this) {
		// Быстрая вставка без лишнего выделения памяти под данные (вставляются только ключ и указатель)
		new_table.insert_fast(it.key, it.value_ptr);
		// "Затирание" указателя, чтобы при удалении старой таблицы не удалялись данные по указателям
		it.value_ptr = nullptr;
	}
	new_table.size_ = size_;

	bucket = move(new_table.bucket);
}

template <typename Key, typename Value>
Value& HashTable<Key, Value>::operator[](const Key &key) {
#ifdef ENABLE_PROFILING
	brackets_ops = 0;
#endif
	auto it = find(key);
#ifdef ENABLE_PROFILING
	brackets_ops += find_ops;
#endif

	if (it != end()) {
		return *(it->value_ptr);
	}
	else {
		insert(key, Value());
	#ifdef ENABLE_PROFILING
		brackets_ops += insert_ops;
	#endif
		auto another_it = find(key);
	#ifdef ENABLE_PROFILING
		brackets_ops += find_ops;
	#endif
		return *(another_it->value_ptr);
	}
}

template <typename Key, typename Value>
const Value& HashTable<Key, Value>::operator[](const Key &key) const {
#ifdef ENABLE_PROFILING
	brackets_ops = 0;
#endif
	auto it = find(key);
#ifdef ENABLE_PROFILING
	brackets_ops += find_ops;
#endif

	if (it != end()) {
		return *(it->value_ptr);
	}
	else {
		insert(key, Value());
	#ifdef ENABLE_PROFILING
		brackets_ops += insert_ops;
	#endif
		auto another_it = find(key);
	#ifdef ENABLE_PROFILING
		brackets_ops += find_ops;
	#endif
		return *(another_it->value_ptr);
	}
}

template <typename Key, typename Value>
void HashTable<Key, Value>::clear() {
#ifdef ENABLE_PROFILING
	clear_ops = 0;
#endif
	for (auto it: *this)
		delete it.value_ptr;
#ifdef ENABLE_PROFILING
	brackets_ops += size_ + 2*num_of_buckets;
#endif

	size_ = 0;
	bucket.clear();
}