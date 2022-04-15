// Включение "режима профилировки - при выполнении операций с таблицей
// в консоль выводится количество операций, выполненных каждым видом таблиц
#define ENABLE_PROFILING

#include "../include/arithmetic.h"
#include "../include/polinomial.h"
#include "../include/UnorderedTable.h"
#include "../include/OrderedTable.h"
#include "../include/HashTable.h"
using namespace std;

// Макрос для вывода количества операций
#define PRINT_OPERATIONS(OPERATION) \
cout << "Number of operations:" << endl; \
cout << "Unordered table: " << unord_table.get_##OPERATION##_ops() << endl; \
cout << "Ordered table: " << ord_table.get_##OPERATION##_ops() << endl; \
cout << "Hash table: " << hash_table.get_##OPERATION##_ops() << endl \


// Проверка на пробелы в строке
bool is_empty_string(string str) {
	str.erase(remove_if(str.begin(), str.end(), [](char ch){return (ch == ' ' || ch == '\t');}), str.end());
	return str == "";
}

// Проверка корректности названия - оно должно содержать только буквы и символ '_' (возвращает True, если имя подходит)
bool check_name(const string &name) {
	return find_if(name.begin(), name.end(), [](char ch){return (ch < 'a' || ch > 'z') && (ch > 'Z' || ch < 'A') && (ch != '_');}) == name.end();
}

void start_message() {
	cout << "TABLE TEST PROGRAM\n";
	cout << "This is a program for working on polinomials.\n";
	cout << "You can create polinomials, perform addition, subtraction andmultiplication, find polinomials, print anddelete them.\n\n";
	cout << "Available commands:\n\n";
	cout << "1) create 'name' = 'value' - creates a polinomial with name 'name' and value 'value'. You can use only created polinomial in operations.\n";
	cout <<	"   'name' may have only english letters and symbol '_'. Example: create f = 5*x\n";
	cout << "   OR create 'name' - creates a polinomial with name 'name' and default value 0. Example: create f\n\n";
	cout << "2) set 'name' = 'value' - assigns polinomial 'value' to existing polinomial 'name'. If polinomial 'name' doesn't exist, the error message will be displayed.\n";
	cout << "   Rules of 'value' are featured in POLINOMIAL CALCULATOR's description." << endl;
	cout << "	Example: set f = 1 - x + x^2 - x^3 + x^4\n\n";
	cout << "3) calc 'poli_name' = 'expression' - calculates 'expression' and assigns the result to existing polinomial 'poli_name'.\n";
	cout << "   'expression' contains numbers, names of polinomials and operations with them ('+', '-', '*'). 'expression' must be a correct arithmetic expression.\n";
	cout << "   Example: calc f = 3*g + h*u*w + 8 + 5.0*21.2\n\n";
	cout << "4) create_and_calc 'name' = 'expression' - creates polinomial 'name' and assigns it value of calculated polinomial 'expression'." << endl;
	cout << "	Rules for 'expression' are the same as for previous command." << endl;
	cout << "	Example: create_and_calc f = 3*g + h*u*w + 8 + 5.0*21.2\n\n";
	cout << "5) find 'poli_name' - finds polinomial 'poli_name' and prints its value on the screen. Example: find plnml\n\n";
	cout << "6) print_all - prints all existing polinomials and their values.\n\n";
	cout << "7) delete 'poli_name' - deletes a polinomial 'poli_name'. This means you can't do operations with the polinomial with the very name until you create it again.\n";
	cout << "   Example: delete f\n\n";
	cout << "8) delete_all - deletes all existing polinomials.\n\n";
	cout << "9) exit - exits the application.\n\n";

	cout << "Enjoy!\n\n";
}

void menu() {
	start_message();

	UnorderedTable<string, Polinomial> unord_table;
	OrderedTable<string, Polinomial> ord_table;
	HashTable<string, Polinomial> hash_table;
	string command, name, value, dummy, expression;

	while (true) {
		cout << ">>> ";
		cin >> command;

		try {

			if (command == "create") {
				cin >> name;
				// Проверка на корректность имени
				if (check_name(name) == false) 
					cout << "'" << name << "': invalid name" << endl << endl;
				// Проверка на существование многочлена с таким же именем
				else if (ord_table.find(name) != ord_table.end()) {
					getline(cin, dummy);
					cout << "'" << name << "': polinomial already exists" << endl << endl;
				}

				else {
					getline(cin, value);
					// Если нет знака и символов кроме пробелов, создаётся пустой многочлен
					if (value.find_first_of('=') == string::npos && is_empty_string(value)) {
						unord_table.insert(name, Polinomial());
						ord_table.insert(name, Polinomial());
						hash_table.insert(name, Polinomial());

						cout << "'" << name << "': polinomial has been created successfully" << endl;
						PRINT_OPERATIONS(insert);
					}
					// Если есть знак '=', создаём многочлен из строки, идущей за ним
					else if (value.find_first_of('=') != string::npos) {
						value.erase(find(value.begin(), value.end(), '='));
						value = value.substr(value.find_first_not_of(' '));
						try {
							Polinomial p(value);

							unord_table.insert(name, p);
							ord_table.insert(name, p);
							hash_table.insert(name, p);

							cout << "'" << name << "': polinomial has been created successfully" << endl;
							PRINT_OPERATIONS(insert);
						}
						catch (const string &exc) {
							throw string("'" + name + "': " + exc);
						}
					}

					else
						throw string("Incorrect command - missing '=' with non-empty value of polinomial");

					cout << endl;
				}
			}

			else if (command == "set") {
				cin >> name >> dummy; // лишнее считывание - для знака '='
				getline(cin, value);

				if (ord_table.find(name) == ord_table.end())
					cout << "'" << name << "': polinomial doesn't exist" << endl << endl;
				// Если отсутствует знак '=', команда неверная
				else if (dummy != "=") {
					cout << "Invalid command" << endl << endl;
				}

				else {
					value = value.substr(value.find_first_not_of(' '));
					// Многочлен при работе конструктора не знает своего имени, поэтому в исключении для удобства его нужно добавить
					try {
						Polinomial poli(value);

						unord_table[name] = poli;
						ord_table[name] = poli;
						hash_table[name] = poli;

						cout << "'" + name + "': value has been set successfully" << endl;
						PRINT_OPERATIONS(brackets);
					}
					catch (const string &exc) {
						throw string("'" + name + "': " + exc);
					}

					cout << endl;
				}
			}

			else if (command == "calc") {
				cin >> name >> dummy;
				getline(cin, expression);

				if (ord_table.find(name) == ord_table.end())
					cout << "'" << name << "': polinomial doesn't exist" << endl << endl;

				else if (dummy != "=") {
					cout << "Invalid command" << endl << endl;
				}

				else {
					try {
						Polinomial poli = Expression<OrderedTable<string,Polinomial>>(expression, ord_table).compute(ord_table);

						unord_table[name] = poli;
						ord_table[name] = poli;
						hash_table[name] = poli;

						cout << name << " = " << ord_table[name] << endl;
						PRINT_OPERATIONS(brackets);
					}
					catch (const string &exc) {
						throw exc;
					}

					cout << endl;
				}
			}

			else if (command == "create_and_calc") {
				cin >> name >> dummy;
				getline(cin, expression);

				if (check_name(name) == false) 
					cout << "'" << name << "': invalid name" << endl << endl;
				else if (dummy != "=") {
					cout << "Invalid command" << endl << endl;
				}
				else if (ord_table.find(name) != ord_table.end()) {
					getline(cin, dummy);
					cout << "'" << name << "': polinomial already exists" << endl << endl;
				}

				else {
					try {
						Polinomial poli = Expression<OrderedTable<string,Polinomial>>(expression, ord_table).compute(ord_table);
						unord_table.insert(name, poli);
						ord_table.insert(name, poli);
						hash_table.insert(name, poli);

						cout << "'" << name << "': polinomial has been created and calculated successfully" << endl;
						cout << name << " = " << ord_table[name] << endl;
						PRINT_OPERATIONS(insert);
					}
					catch (const string &exc) {
						throw exc;
					}

					cout << endl;
				}
			}

			else if (command == "find") {
				cin >> name;
				getline(cin, dummy);

				if (ord_table.find(name) == ord_table.end())
					cout << "'" << name << "': polinomial doesn't exist" << endl << endl;

				else if (!is_empty_string(dummy))
					cout << "Invalid command" << endl << endl;
				else {
					cout << "'" << name << "': polinomial has been found successfully" << endl;
					cout << name << " = " << ord_table[name] << endl;

					// "Холостые" запуски - для вывода информации о поиске
					unord_table.find(name);
					ord_table.find(name);
					hash_table.find(name);

					PRINT_OPERATIONS(find);
					cout << endl;
				}
			}

			else if (command == "print_all") {
				if (ord_table.empty())
					cout << "No polinomial exists" << endl << endl;
				else {
					cout << "All polinomials:" << endl;

					for (auto &p : ord_table)
						cout << "" << p.key << " = " << *(p.value_ptr) << endl;
					cout << endl;
				}
			}

			else if (command == "delete") {
				cin >> name;
				getline(cin, dummy);

				if (!is_empty_string(dummy))
					cout << "Invalid command" << endl << endl;

				else if (ord_table.find(name) == ord_table.end())
					cout << "'" << name << "': polinomial doesn't exist" << endl << endl;

				else {
					unord_table.erase(name);
					ord_table.erase(name);
					hash_table.erase(name);

					cout << "'" << name << "': polinomial has been deleted successfully" << endl;
					PRINT_OPERATIONS(erase);
					cout << endl;
				}
			}

			else if (command == "delete_all") {
				getline(cin, dummy);

				if (!is_empty_string(dummy))
					cout << "Invalid command" << endl << endl;

				else {
					unord_table.clear();
					ord_table.clear();
					hash_table.clear();

					cout << "'" << name << "': all polinomials have been deleted successfully" << endl;
					PRINT_OPERATIONS(clear);
					cout << endl;
				}
			}

			else if (command == "exit") {
				cout << "Exiting..." << endl << endl;
				exit(0);
			}

			else {
				// Дочитываем строку до конца, чтобы на одну команду не было несколько сообщений
				getline(cin, dummy);

				throw string("Invalid command");
			}
		}
		catch (const string &exc) {
			cout << exc << endl << endl;
		}
	}
}

int main() {
	menu();
}