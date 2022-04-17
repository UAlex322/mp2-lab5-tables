#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <initializer_list>
using namespace std;

template <typename Key, typename Value>
class OrderedTable {

	using Entry = pair<Key,Value>;
	using iterator = typename vector<Entry>::iterator;

	vector<Entry> table;
#ifdef ENABLE_PROFILING
	size_t find_ops = 0;
	size_t insert_ops = 0;
	size_t erase_ops = 0;
	size_t clear_ops = 0;
	size_t brackets_ops = 0;
#endif


	size_t common_find(const Key &key) {
	#ifdef ENABLE_PROFILING
		find_ops = 0;
	#endif

		size_t left = 0, right = table.size(), middle;

		while (left+1 < right) {
			middle = left + (right-left)/2;
			if (table[middle].first <= key) left = middle;
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

		for (auto it = first; it != last; ++it) {
			if (find(it->first) == end())
				table.emplace_back(*it);
		}

		sort(table.begin(), table.end(), [](const Entry &e1, const Entry &e2) {
			return e1.first < e2.first;
		});
	}

	OrderedTable(const initializer_list<pair<Key, Value>> &init_list) {
		table.reserve(2*init_list.size());

		for (auto entry: init_list)
			table.emplace_back(entry);

		sort(table.begin(), table.end(), [](const Entry &e1, const Entry &e2) {
			return e1.first < e2.first;
		});
	}

	~OrderedTable() {
		clear();
	}

	iterator find(const Key &key) {
		size_t pos = common_find(key);

	#ifdef ENABLE_PROFILING
		++find_ops;
	#endif
		if (table.size() && table[pos].first == key) return table.begin() + pos;
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
		if (table.size() && table[pos].first == key)
			throw string("Entry with this key already exists");

		// Вставка в конец и перестановка на нужное место (все бОльшие сдвигаются вправо) 
		table.emplace_back(key, value);
	#ifdef ENABLE_PROFILING
		++insert_ops;
	#endif
		for (size_t i = table.size()-1; i > 0 && table[i].first < table[i-1].first; --i) {
			swap(table[i], table[i-1]);
		#ifdef ENABLE_PROFILING
			++insert_ops;
		#endif
		}
	}

	void insert(const pair<Key, Value> &pair) {
		insert(pair.first, pair.second);
	}

	void erase(const Key &key) {
	#ifdef ENABLE_PROFILING
		erase_ops = 0;
	#endif

		size_t pos = common_find(key);
	#ifdef ENABLE_PROFILING
		erase_ops += find_ops;
	#endif
		if (!table.size() || table[pos].first != key)
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
			return it->second;
		else {
			insert(key, Value());
		#ifdef ENABLE_PROFILING
			brackets_ops += insert_ops;
		#endif
			auto it = find(key);
		#ifdef ENABLE_PROFILING
			brackets_ops += find_ops;
		#endif
			return it->second;
		}
	}

	inline size_t size() const noexcept {
		return table.size();
	}

	inline bool empty() const noexcept {
		return table.size() == 0;
	}

	inline void clear() {
	#ifdef ENABLE_PROFILING
		clear_ops = table.size();
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