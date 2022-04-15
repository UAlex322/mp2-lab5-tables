#include "../gtest/gtest.h"
#include "../include/UnorderedTable.h"
#include "../include/OrderedTable.h"
#include "../include/HashTable.h"
#include <random>
#include <string>
using namespace std;

mt19937 gen(random_device{}());

//UNORDERED_TABLE

// ћакросы руг¤ютс¤ на несколько параметров в шаблонах классов!
using UnorderedTableIntInt = UnorderedTable<int,int>;

TEST(UnorderedTable, can_create_empty_table) {
	ASSERT_NO_THROW(UnorderedTableIntInt t);
}

TEST(UnorderedTable, initializer_list_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		initializer_list<pair<int, int>> init{{gen(),gen()},{gen(),gen()},{gen(),gen()},{gen(),gen()},{gen(),gen()}};

		ASSERT_NO_THROW(
			UnorderedTableIntInt table(init);
			for (auto entry: table) {
				for (auto pair: init) {
					if (entry.key == pair.first) {
						EXPECT_EQ(*entry.value_ptr, pair.second);
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
		for (auto pair: vec)
			pair = {gen(), gen()};
		
		ASSERT_NO_THROW(
			UnorderedTableIntInt table(vec.begin(), vec.end());
			for (auto entry: table) {
				for (auto pair: vec) {
					if (entry.key == pair.first) {
						EXPECT_EQ(*entry.value_ptr, pair.second);
						break;
					}
				}
			}
		);
	}
}

TEST(UnorderedTable, find_is_correct) {
	vector<pair<int,int>> vec(200);
	for (size_t i = 0; i < 200; ++i)
		vec[i] = {i, 2*i};

	UnorderedTableIntInt table(vec.rbegin(), vec.rend());
	for (int i = 0; i < 200; ++i) {
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		// проверка, что запись с таким ключом есть
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (int i = 200; i < 500; ++i)
		// проверка, что записей с таким ключом нет
		EXPECT_EQ(table.find(i), table.end()); 
}

TEST(UnorderedTable, insert_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i,2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i)
		// если ключ зан¤т, выбрасываетс¤ исключение
		ASSERT_ANY_THROW(table.insert(i,2*i)); 
}

TEST(UnorderedTable, erase_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i,2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i) {
		ASSERT_NO_THROW(table.erase(i));
		EXPECT_EQ(table.find(i), table.end());
	}

	for (size_t i = 0; i < 200; ++i)
		// повторное удаление записи выбрасывает исключение
		ASSERT_ANY_THROW(table.erase(i)); 
}

TEST(UnorderedTable, square_brackets_operator_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i, 2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i)
		ASSERT_NO_THROW(EXPECT_EQ(table[i], 2*i));
	for (size_t i = 200; i < 500; ++i)
		// при обращении к несуществующей записи создаЄтс¤ нова¤ со значением по умолчанию
		ASSERT_NO_THROW(EXPECT_EQ(table[i], int()));
}

//ORDERED_TABLE

using OrderedTableIntInt = OrderedTable<int,int>;

TEST(OrderedTable, can_create_empty_table) {
	ASSERT_NO_THROW(UnorderedTableIntInt t);
}

TEST(OrderedTable, initializer_list_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		initializer_list<pair<int, int>> init{{gen(),gen()},{gen(),gen()},{gen(),gen()},{gen(),gen()},{gen(),gen()}};

		ASSERT_NO_THROW(
			UnorderedTableIntInt table(init);
		for (auto entry: table) {
			for (auto pair: init) {
				if (entry.key == pair.first) {
					EXPECT_EQ(*entry.value_ptr, pair.second);
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
		for (auto pair: vec)
			pair = {gen(), gen()};

		ASSERT_NO_THROW(
			UnorderedTableIntInt table(vec.begin(), vec.end());
		for (auto entry: table) {
			for (auto pair: vec) {
				if (entry.key == pair.first) {
					EXPECT_EQ(*entry.value_ptr, pair.second);
					break;
				}
			}
		}
		);
	}
}

TEST(OrderedTable, find_is_correct) {
	vector<pair<int,int>> vec(200);
	for (size_t i = 0; i < 200; ++i)
		vec[i] = {i, 2*i};

	UnorderedTableIntInt table(vec.rbegin(), vec.rend());
	for (int i = 0; i < 200; ++i) {
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		// проверка, что запись с таким ключом есть
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (int i = 200; i < 500; ++i)
		// проверка, что записей с таким ключом нет
		EXPECT_EQ(table.find(i), table.end()); 
}

TEST(OrderedTable, insert_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i,2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i)
		// если ключ зан¤т, выбрасываетс¤ исключение
		ASSERT_ANY_THROW(table.insert(i,2*i)); 
}

TEST(OrderedTable, erase_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i,2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i) {
		ASSERT_NO_THROW(table.erase(i));
		EXPECT_EQ(table.find(i), table.end());
	}

	for (size_t i = 0; i < 200; ++i)
		// повторное удаление записи выбрасывает исключение
		ASSERT_ANY_THROW(table.erase(i)); 
}

TEST(OrderedTable, square_brackets_operator_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i, 2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i)
		ASSERT_NO_THROW(EXPECT_EQ(table[i], 2*i));
	for (size_t i = 200; i < 500; ++i)
		// при обращении к несуществующей записи создаЄтс¤ нова¤ со значением по умолчанию
		ASSERT_NO_THROW(EXPECT_EQ(table[i], int()));
}

//HASH_TABLE

using HashTableIntInt = HashTable<int,int>;

TEST(HashTable, can_create_empty_table) {
	ASSERT_NO_THROW(UnorderedTableIntInt t);
}

TEST(HashTable, initializer_list_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		initializer_list<pair<int, int>> init{{gen(),gen()},{gen(),gen()},{gen(),gen()},{gen(),gen()},{gen(),gen()}};

		ASSERT_NO_THROW(
			UnorderedTableIntInt table(init);
		for (auto entry: table) {
			for (auto pair: init) {
				if (entry.key == pair.first) {
					EXPECT_EQ(*entry.value_ptr, pair.second);
					break;
				}
			}
		}
		);
	}
}

TEST(HashTable, range_constructor_is_correct) {
	for (size_t i = 0; i < 10; ++i) {
		vector<pair<int,int>> vec(500);
		for (auto pair: vec)
			pair = {gen(), gen()};

		ASSERT_NO_THROW(
			UnorderedTableIntInt table(vec.begin(), vec.end());
		for (auto entry: table) {
			for (auto pair: vec) {
				if (entry.key == pair.first) {
					EXPECT_EQ(*entry.value_ptr, pair.second);
					break;
				}
			}
		}
		);
	}
}

TEST(HashTable, find_is_correct) {
	vector<pair<int,int>> vec(200);
	for (size_t i = 0; i < 200; ++i)
		vec[i] = {i, 2*i};

	UnorderedTableIntInt table(vec.rbegin(), vec.rend());
	for (int i = 0; i < 200; ++i) {
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		// проверка, что запись с таким ключом есть
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (int i = 200; i < 500; ++i)
		// проверка, что записей с таким ключом нет
		EXPECT_EQ(table.find(i), table.end()); 
}

TEST(HashTable, insert_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i,2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i)
		// если ключ зан¤т, выбрасываетс¤ исключение
		ASSERT_ANY_THROW(table.insert(i,2*i)); 
}

TEST(HashTable, erase_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i,2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i) {
		ASSERT_NO_THROW(table.erase(i));
		EXPECT_EQ(table.find(i), table.end());
	}

	for (size_t i = 0; i < 200; ++i)
		// повторное удаление записи выбрасывает исключение
		ASSERT_ANY_THROW(table.erase(i)); 
}

TEST(HashTable, square_brackets_operator_is_correct) {
	UnorderedTableIntInt table;

	for (size_t i = 0; i < 200; ++i) {
		table.insert(i, 2*i);
		auto iter = table.find(i);
		EXPECT_NE(iter, table.end());
		EXPECT_EQ(*((*iter).value_ptr), 2*i);
	}

	for (size_t i = 0; i < 200; ++i)
		ASSERT_NO_THROW(EXPECT_EQ(table[i], 2*i));
	for (size_t i = 200; i < 500; ++i)
		// при обращении к несуществующей записи создаЄтс¤ нова¤ со значением по умолчанию
		ASSERT_NO_THROW(EXPECT_EQ(table[i], int()));
}