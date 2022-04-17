#include "../gtest/gtest.h"
#include "../include/UnorderedTable.h"
#include "../include/OrderedTable.h"
#include "../include/HashTable.h"
#include <random>
#include <iostream>
#include <string>
using namespace std;

mt19937 gen(random_device{}());

//UNORDERED_TABLE


// Макросы ругаются на несколько параметров в шаблонах классов!
using UnorderedTableIntInt = UnorderedTable<int,int>;

TEST(UnorderedTable, can_create_empty_table) {
	ASSERT_NO_THROW(UnorderedTableIntInt t);
}

TEST(UnorderedTable, initializer_list_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		initializer_list<pair<int, int>> init{{4,gen()},{3,gen()},{5,gen()},{2,gen()},{1,gen()}};

		ASSERT_NO_THROW(
			UnorderedTableIntInt table(init);
			for (auto entry: table) {
				for (auto pair: init) {
					if (entry.first == pair.first) {
						EXPECT_EQ(entry.second, pair.second);
						break;
					}
				}
			}
		);
	}
}

TEST(UnorderedTable, range_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int,int>> vec(500);
		for (size_t i = 0; i < 500; ++i)
			vec[i] = {i,gen()};
		shuffle(vec.begin(), vec.end(), gen);
		
		ASSERT_NO_THROW(
			UnorderedTableIntInt table(vec.begin(), vec.end());
			for (auto entry: table) {
				for (auto pair: vec) {
					if (entry.first == pair.first) {
						EXPECT_EQ(entry.second, pair.second);
						break;
					}
				}
			}
		);
	}
}

TEST(UnorderedTable, find_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		UnorderedTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) {
				auto iter = table.find(elem.first);
				EXPECT_NE(iter, table.end());
				EXPECT_EQ(iter->second, elem.second);
			}
		);
		for (size_t i = data.size(); i < data.size()*2; ++i)
			ASSERT_NO_THROW(EXPECT_EQ(table.find(i), table.end()));
	}
}

TEST(UnorderedTable, insert_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		UnorderedTableIntInt table;
		for (size_t i = 0; i < data.size(); ++i) {
			table.insert(data[i]);
			auto iter = table.find(data[i].first);
			EXPECT_NE(iter, table.end());
			EXPECT_EQ(iter->second, data[i].second);
		}

		for (auto elem: data)
			// если ключ занят, выбрасывается исключение
			ASSERT_ANY_THROW(table.insert(elem));
	}
}

TEST(UnorderedTable, erase_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		UnorderedTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) {
				auto iter = table.find(elem.first);
				EXPECT_NE(iter, table.end());
				EXPECT_EQ(iter->second, elem.second);

				table.erase(elem.first);
				EXPECT_EQ(table.find(elem.first), table.end());
			}
		);

		for (auto elem: data)
			ASSERT_ANY_THROW(table.erase(elem.first));
	}
}

TEST(UnorderedTable, square_brackets_operator_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		UnorderedTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) 
				EXPECT_EQ(table[elem.first], elem.second);
		);

		for (size_t i = data.size(); i < 2*data.size(); ++i) {
			ASSERT_NO_THROW(EXPECT_EQ(table[i], int()));
			EXPECT_EQ(table.size(), i+1);
		}
	}
}

//ORDERED_TABLE

using OrderedTableIntInt = OrderedTable<int,int>;

TEST(OrderedTable, can_create_empty_table) {
	ASSERT_NO_THROW(OrderedTableIntInt t);
}

TEST(OrderedTable, initializer_list_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		initializer_list<pair<int, int>> init{{4,gen()},{3,gen()},{5,gen()},{2,gen()},{1,gen()}};

		ASSERT_NO_THROW(
			OrderedTableIntInt table(init);
		for (auto entry: table) {
			for (auto pair: init) {
				if (entry.first == pair.first) {
					EXPECT_EQ(entry.second, pair.second);
					break;
				}
			}
		}
		);
	}
}

TEST(OrderedTable, range_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int,int>> vec(500);
		for (size_t i = 0; i < 500; ++i)
			vec[i] = {i,gen()};
		shuffle(vec.begin(), vec.end(), gen);

		ASSERT_NO_THROW(
			OrderedTableIntInt table(vec.begin(), vec.end());
		for (auto entry: table) {
			for (auto pair: vec) {
				if (entry.first == pair.first) {
					EXPECT_EQ(entry.second, pair.second);
					break;
				}
			}
		}
		);
	}
}

TEST(OrderedTable, find_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		OrderedTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) {
				auto iter = table.find(elem.first);
				EXPECT_NE(iter, table.end());
				EXPECT_EQ(iter->second, elem.second);
			}
		);
		for (size_t i = data.size(); i < data.size()*2; ++i)
			ASSERT_NO_THROW(EXPECT_EQ(table.find(i), table.end()));
	}
}

TEST(OrderedTable, insert_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		OrderedTableIntInt table;
		for (size_t i = 0; i < data.size(); ++i) {
			table.insert(data[i]);
			auto iter = table.find(data[i].first);
			EXPECT_NE(iter, table.end());
			EXPECT_EQ(iter->second, data[i].second);
		}

		for (auto elem: data)
			// если ключ занят, выбрасывается исключение
			ASSERT_ANY_THROW(table.insert(elem));
	}
}

TEST(OrderedTable, erase_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		OrderedTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) {
				auto iter = table.find(elem.first);
				EXPECT_NE(iter, table.end());
				EXPECT_EQ(iter->second, elem.second);

				table.erase(elem.first);
				EXPECT_EQ(table.find(elem.first), table.end());
			}
		);

		for (auto elem: data)
			ASSERT_ANY_THROW(table.erase(elem.first));
	}
}

TEST(OrderedTable, square_brackets_operator_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		OrderedTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) 
				EXPECT_EQ(table[elem.first], elem.second);
		);

		for (size_t i = data.size(); i < 2*data.size(); ++i) {
			ASSERT_NO_THROW(EXPECT_EQ(table[i], int()));
			EXPECT_EQ(table.size(), i+1);
		}
	}
}

//HASH_TABLE

using HashTableIntInt = HashTable<int,int>;

TEST(HashTable, can_create_empty_table) {
	ASSERT_NO_THROW(HashTableIntInt t);
}

TEST(HashTable, initializer_list_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		initializer_list<pair<int, int>> init{{4,gen()},{3,gen()},{5,gen()},{2,gen()},{1,gen()}};

		ASSERT_NO_THROW(
			HashTableIntInt table(init);
			for (auto entry: table) {
				for (auto pair: init) {
					if (entry.first == pair.first) {
						EXPECT_EQ(entry.second, pair.second);
						break;
					}
				}
			}
		);
	}
}

TEST(HashTable, range_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int,int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i,gen()};
		shuffle(data.begin(), data.end(), gen);

		ASSERT_NO_THROW(
			HashTableIntInt table(data.begin(), data.end());
			for (auto entry: table)
				EXPECT_NE(find(data.begin(),data.end(),entry), data.end());
		);
	}
}

int done = 0;
int it;
size_t iteration = 0;

TEST(HashTable, find_is_correct) {
	for (size_t &i = iteration; i < 10; ++i) {
		done = 0, it = 0;
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		HashTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) {		
				auto iter = table.find(elem.first);
				EXPECT_NE(iter, table.end());
				EXPECT_EQ(iter->second, elem.second);
				++it;
			}
		);
		done = 1;
		for (size_t i = data.size(); i < data.size()*2; ++i)
			ASSERT_NO_THROW(EXPECT_EQ(table.find(i), table.end()));
	}
}

TEST(HashTable, insert_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		HashTableIntInt table;
		for (size_t i = 0; i < data.size(); ++i) {
			table.insert(data[i]);
			auto iter = table.find(data[i].first);
			EXPECT_NE(iter, table.end());
			EXPECT_EQ(iter->second, data[i].second);
		}

		for (auto elem: data)
			// если ключ занят, выбрасывается исключение
			ASSERT_ANY_THROW(table.insert(elem));
	}
}

TEST(HashTable, erase_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		HashTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) {
				auto iter = table.find(elem.first);
				EXPECT_NE(iter, table.end());
				EXPECT_EQ(iter->second, elem.second);

				table.erase(elem.first);
				EXPECT_EQ(table.find(elem.first), table.end());
			}
		);

		for (auto elem: data)
			ASSERT_ANY_THROW(table.erase(elem.first));
	}
}

TEST(HashTable, square_brackets_operator_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int, int>> data(gen() % 100);
		for (size_t i = 0; i < data.size(); ++i)
			data[i] = {i, gen()};
		shuffle(data.begin(), data.end(), gen);

		HashTableIntInt table(data.begin(), data.end());
		ASSERT_NO_THROW(
			for (auto elem: data) 
				EXPECT_EQ(table[elem.first], elem.second);
		);

		for (size_t i = data.size(); i < 2*data.size(); ++i) {
			ASSERT_NO_THROW(EXPECT_EQ(table[i], int()));
			EXPECT_EQ(table.size(), i+1);
		}
	}
}