#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <initializer_list>
using namespace std;

template <typename Key, typename Value>
class OrderedTable {

	struct Entry {
		Key key;
		Value *value_ptr;

		Entry(const Key &key = Key(), Value *value_ptr = nullptr): key(key), value_ptr(value_ptr) {}
	};

	vector<Entry> table;
#ifdef ENABLE_PROFILING
	size_t find_ops;
	size_t insert_ops;
	size_t erase_ops;
	size_t clear_ops;
	size_t brackets_ops;
#endif

	using iterator = typename vector<Entry>::iterator;

	size_t common_find(const Key &key) {
	#ifdef ENABLE_PROFILING
		find_ops = 0;
	#endif

		size_t left = 0, right = table.size(), middle;

		while (left+1 < right) {
			middle = left + (right-left)/2;
			if (table[middle].key <= key) left = middle;
			else right = middle;
		#ifdef ENABLE_PROFILING
			++find_ops;
		#endif
		}

		return left;
	}

public:

	OrderedTable() {}

	OrderedTable(const OrderedTable &copy): table(copy.table) {}
	OrderedTable(OrderedTable &&mov): table(move(mov.table)) {}

	template <typename Iterator>
	OrderedTable(Iterator first, Iterator last) {
		size_t sz = distance(first, last);
		table.reserve(2*sz);

		for (auto it = first; it != last; ++it)
			table.emplace_back((*it).first, new Value((*it).second));

		sort(table.begin(), table.end(), [](const Entry &e1, const Entry &e2) {
			return e1.key < e2.key;
			 });
	}

	OrderedTable(const initializer_list<pair<Key, Value>> &init) {
		table.reserve(2*init.size());

		for (auto it = init.begin(); it != init.end(); ++it)
			table.emplace_back((*it).first, new Value((*it).second));

		sort(table.begin(), table.end(), [](const Entry &e1, const Entry &e2) {
			return e1.key < e2.key;
			 });
	}

	~OrderedTable() {
		clear();
	}

	iterator find(const Key &key) {
		size_t pos = common_find(key);
	
		if (table.size() && table[pos].key == key) return table.begin() + pos;
		return table.end();
	}

	void insert(const Key &key, const Value &value) {
	#ifdef ENABLE_PROFILING
		insert_ops = 0;
	#endif

		size_t pos = common_find(key);
	#ifdef ENABLE_PROFILING
		insert_ops += find_ops;
	#endif
		if (table.size() && table[pos].key == key)
			throw string("Entry with this key already exists");

		// Вставка в конец и перестановка на нужное место (все бОльшие сдвигаются вправо) 
		table.emplace_back(key, new Value(value));
	#ifdef ENABLE_PROFILING
		insert_ops += 2;
	#endif
		for (size_t i = table.size()-1; i > 0 && table[i].key < table[i-1].key; --i) {
			swap(table[i], table[i-1]);
		#ifdef ENABLE_PROFILING
			++insert_ops;
		#endif
		}
	}

	void erase(const Key &key) {
	#ifdef ENABLE_PROFILING
		erase_ops = 0;
	#endif

		size_t pos = common_find(key);
	#ifdef ENABLE_PROFILING
		erase_ops += find_ops;
	#endif
		if (!table.size() || table[pos].key != key)
			throw string("No entry with this key exists");
	#ifdef ENABLE_PROFILING
		++erase_ops;
	#endif

		// "Проталкиваем" удаляемый элемент в конец и удаляем из вектора
		for (size_t i = pos+1; i != table.size(); ++i) {
			swap(table[i], table[i-1]);
		#ifdef ENABLE_PROFILING
			++erase_ops;
		#endif
		}
		delete table.back().value_ptr;
		table.pop_back();

	#ifdef ENABLE_PROFILING
		++erase_ops;
	#endif
	}

	Value &operator[](const Key &key) {
	#ifdef ENABLE_PROFILING
		brackets_ops = 0;
	#endif

		auto it = find(key);
	#ifdef ENABLE_PROFILING
		brackets_ops += find_ops;
	#endif

		if (it != table.end())
			return *((*it).value_ptr);
		else {
			insert(key, Value());
		#ifdef ENABLE_PROFILING
			brackets_ops += insert_ops;
		#endif
			auto it = find(key);
		#ifdef ENABLE_PROFILING
			brackets_ops += find_ops;
		#endif
			return *((*it).value_ptr);
		}
	}

	const Value &operator[](const Key &key) const {
	#ifdef ENABLE_PROFILING
		brackets_ops = 0;
	#endif

		auto it = find(key);
	#ifdef ENABLE_PROFILING
		brackets_ops += find_ops;
	#endif

		if (it != table.end())
			return *((*it).value_ptr);
		else {
			insert(key, Value());
		#ifdef ENABLE_PROFILING
			brackets_ops += insert_ops;
		#endif
			auto it = find(key);
		#ifdef ENABLE_PROFILING
			brackets_ops += find_ops;
		#endif
			return *((*it).value_ptr);
		}
	}

	void size() const noexcept {
		return table.size();
	}

	bool empty() const noexcept {
		return table.size() == 0;
	}

	void clear() {
	#ifdef ENABLE_PROFILING
		clear_ops = 0;
	#endif

		for (auto it: table)
			delete it.value_ptr;
	#ifdef ENABLE_PROFILING
		clear_ops += table.size();
	#endif

		table.clear();
	}

	iterator begin() { return table.begin(); }
	iterator end() { return table.end(); }
	iterator rbegin() { return table.rbegin(); }
	iterator rend() { return table.rend(); }

	iterator cbegin() { return table.cbegin(); }
	iterator cend() { return table.cend(); }
	iterator crbegin() { return table.crbegin(); }
	iterator crend() { return table.crend(); }

#ifdef ENABLE_PROFILING
	size_t get_find_ops() const { return find_ops; }
	size_t get_insert_ops() const { return insert_ops; }
	size_t get_erase_ops() const { return erase_ops; }
	size_t get_brackets_ops() const { return brackets_ops; }
	size_t get_clear_ops() const { return clear_ops; }
#endif
};