#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <initializer_list>
using namespace std;

template <typename Key, typename Value>
class UnorderedTable {

	struct Entry {
		Key key;
		Value *value_ptr;
		
		Entry(const Key &key = Key(), Value *value_ptr = nullptr): key(key), value_ptr(value_ptr) {}
	};

	vector<Entry> table;
	using iterator = typename vector<Entry>::iterator;
#ifdef ENABLE_PROFILING
	size_t find_ops;
	size_t insert_ops;
	size_t erase_ops;
	size_t clear_ops;
	size_t brackets_ops;
#endif
	
	iterator common_find(const Key &key) {
	#ifdef ENABLE_PROFILING
		find_ops = 0;
	#endif
		for (iterator it = table.begin(); it != table.end(); ++it) {
		#ifdef ENABLE_PROFILING
			++find_ops;
		#endif
			if ((*it).key == key)
				return it;
		}

		return table.end();
	}


public:

	UnorderedTable() {}

	UnorderedTable(const UnorderedTable &copy): table(copy.table) {}
	UnorderedTable(UnorderedTable &&mov): table(move(mov.table)) {}

	template <typename Iterator>
	UnorderedTable(Iterator first, Iterator last) {
		size_t sz = distance(first, last);
		table.reserve(2*sz);

		for (auto it = first; it != last; ++it)
			table.emplace_back((*it).first, new Value((*it).second));
	}

	UnorderedTable(const initializer_list<pair<Key,Value>> &init) {
		table.reserve(2*init.size());

		for (auto it = init.begin(); it != init.end(); ++it)
			table.emplace_back((*it).first, new Value((*it).second));
	}

	~UnorderedTable() {
		clear();
	}

	iterator find(const Key &key) {
		return common_find(key);
	}

	void insert(const Key &key, const Value &value) {
	#ifdef ENABLE_PROFILING
		insert_ops = 0;
	#endif
		iterator it = find(key);
	#ifdef ENABLE_PROFILING
		insert_ops += find_ops;
	#endif
		if (it != end())
			throw string("Entry with this key already exists");

		table.emplace_back(key, new Value(value));
	#ifdef ENABLE_PROFILING
		++insert_ops;
	#endif
	}

	void insert(const pair<Key, Value> &pair) {
		insert(pair.first, pair.second);
	}

	void erase(const Key &key) {
	#ifdef ENABLE_PROFILING
		erase_ops = 0;
	#endif
		iterator it = find(key);
	#ifdef ENABLE_PROFILING
		erase_ops += find_ops;
	#endif
		if (it == end())
			throw string("Entry with this key doesn't exist");

		delete (*it).value_ptr;
		swap(*it, table.back());
		table.pop_back();

	#ifdef ENABLE_PROFILING
		erase_ops += 2;
	#endif
	}

	Value &operator[](const Key &key) {
	#ifdef ENABLE_PROFILING
		brackets_ops = 0;
	#endif
		auto it = common_find(key);
	#ifdef ENABLE_PROFILING
		brackets_ops += find_ops;
	#endif

		if (it != table.end())
			return *((*it).value_ptr);
		else {
			table.emplace_back(key, new Value());
		#ifdef ENABLE_PROFILING
			++brackets_ops;
		#endif
			return *(table.back().value_ptr);
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
			table.emplace_back(key, new Value());
		#ifdef ENABLE_PROFILING
			++brackets_ops;
		#endif
			return *(table.back().value_ptr);
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

	iterator begin() noexcept { return table.begin(); }
	iterator end() noexcept { return table.end(); }
	iterator rbegin() noexcept { return table.rbegin(); }
	iterator rend() noexcept { return table.rend(); }

	iterator cbegin() noexcept { return table.cbegin(); }
	iterator cend() noexcept { return table.cend(); }
	iterator crbegin() noexcept { return table.crbegin(); }
	iterator crend() noexcept { return table.crend(); }

#ifdef ENABLE_PROFILING
	size_t get_find_ops() const { return find_ops; }
	size_t get_insert_ops() const { return insert_ops; }
	size_t get_erase_ops() const { return erase_ops; }
	size_t get_brackets_ops() const { return brackets_ops; }
	size_t get_clear_ops() const { return clear_ops; }
#endif
};