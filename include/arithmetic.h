// объ¤вление функций и классов дл¤ вычислени¤ арифметических выражений
#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <stack>
#include <iostream>
#include "polinomial.h"
using namespace std;

template <class Table>
class Expression {
	// Приоритет операций (левоассоциативные имеют отрицательное значение, правоассоциативные - положительное)
	map<string, char> prior{{"+", -2}, {"-", -2}, {"*", -1}, {"-u", 1}};


	// Типы токенов: 1 - число, 2 - операция, 3 - открывающая скобка, 4 - закрывающая скобка
	enum token_type {NUM = 1, OP, OPEN_BR, CLOSE_BR};

	struct Token {
		token_type type;		// имеет одно из определённых выше значений
		Polinomial pol;	// значение многочлена, если токен хранит многочлен
		string name;	// символ операции, если хранится операция, или название переменной, если хранится число

		explicit Token(token_type type, const string& name = "", const Polinomial &pol = Polinomial()): type(type), pol(pol), name(name) {}

		bool operator==(const Token &token) {
			return (type == token.type && name == token.name);
		}
		bool operator!=(const Token &token) {
			return !(*this == token);
		}
	};

	vector<Token> parse(const string &expr);	  // выражение -> токены
	string check(Table &names);					  // проверка выражения на корректность
	void make_postfix_notation();				  // инфиксная нотация -> постфиксная запись

	// DATA
	vector<Token> _tokens;							// набор лексем в постфиксной записи
	map<string, vector<int>> variable_positions;	// пары из названий переменных и индексов лексем в постфиксной записи, в которых они находятся

public:

	Expression(const string &expr, Table &names);
	Polinomial compute(Table values);	// вычисление выражения в постфиксной записи
	bool has_no_variables();			// проверка на наличие переменных в выражении
};

template <typename Table>
Expression<Table>::Expression(const string &expr, Table &names) {
	_tokens = parse(expr);

	string verdict = check(names);
	if (_tokens.empty() || verdict != "") {
		//verdict = "Error: expression is incorrect!\n" + verdict;
		if (_tokens.empty())
			verdict += "Error: empty expression\n";

		throw verdict;
	}

	make_postfix_notation();
}

template <typename Table>
vector<typename Expression<Table>::Token> Expression<Table>::parse(const string &expr) {
	vector<Token> tokens;
	size_t first_digit_pos = -1;	// позиция первой цифры секущего считываемого числа; -1, если в данный момент не считывается число
	size_t first_letter_pos = -1;	// позиция первой буквы текущей считываемой переменной; -1, если в данный момент не считывается переменная
	bool was_dot = false;			// была ли считана точка действительного числа

	for (size_t i = 0; i < expr.size(); ++i) {
		if (expr[i] == ' ' && first_digit_pos == -1 && first_letter_pos == -1) continue;

		else if ('a' <= expr[i] && expr[i] <= 'z' || 'A' <= expr[i] && expr[i] <= 'Z') {
			if (first_letter_pos == -1)
				first_letter_pos = i;
		}

		else if ('0' <= expr[i] && expr[i] <= '9') {
			if (first_digit_pos == -1)
				first_digit_pos = i;
		}

		else if (first_digit_pos != -1 && expr[i] == '.' && !was_dot)
			was_dot = true;

		else {
			was_dot = false;

			if (first_digit_pos != -1) {
				tokens.push_back(Token(NUM, "", Polinomial(expr.substr(first_digit_pos, i-first_digit_pos))));
				first_digit_pos = -1;
			}

			if (first_letter_pos != -1) {
				tokens.push_back(Token(NUM, expr.substr(first_letter_pos, i-first_letter_pos)));
				first_letter_pos = -1;
			}

			if (expr[i] == '(')
				tokens.push_back(Token(OPEN_BR));

			else if (expr[i] == ')')
				tokens.push_back(Token(CLOSE_BR));

			else if (expr[i] == '-' && (tokens.empty() || tokens.back().type == OPEN_BR || tokens.back().type == OP))
				tokens.push_back(Token(OP, "-u"));

			else if (expr[i] != ' ')
				tokens.push_back(Token(OP, string(1, expr[i])));
		}
	}

	if (first_digit_pos != -1) {
		tokens.push_back(Token(NUM, "", Polinomial(expr.substr(first_digit_pos, expr.size()-first_digit_pos))));
	}

	if (first_letter_pos != -1) {
		tokens.push_back(Token(NUM, expr.substr(first_letter_pos, expr.size()-first_letter_pos)));
	}

	return tokens;
}

template <typename Table>
string Expression<Table>::check(Table &names) {
	token_type last_state = OPEN_BR; // тип последнего считанного токена
	int correct_brackets = 0;
	string errors;

	for (const Token &token : _tokens) {
		switch (token.type) {
			case NUM:
				if (last_state == NUM || last_state == CLOSE_BR)
					return "Error: expression is incorrect";
				if (token.name != "" && names.find(token.name) == names.end())
					errors += ("'" + token.name + "' : polinomial doesn't exist");
				break;

			case OP:
				if (prior.find(token.name) == prior.end() || last_state == OP && prior[token.name] < 0 || last_state == OPEN_BR && prior[token.name] < 0)
					return "Error: expression is incorrect";
				break;

			case OPEN_BR:
				if (last_state == NUM || last_state == CLOSE_BR)
					return "Error: expression is incorrect";

				++correct_brackets;

				break;

			case CLOSE_BR:
				if (last_state == OP || last_state == OPEN_BR || correct_brackets < 1)
					return "Error: expression is incorrect";

				--correct_brackets;

				break;
		}

		last_state = token.type;
	}

	if (!_tokens.empty() && _tokens.back().type == OP && prior.find(_tokens.back().name) != prior.end() && prior[_tokens.back().name] > 0)
		return "Error: expression is incorrect";

	return (correct_brackets == 0) ? errors: "Error: expression is incorrect";

}

template <typename Table>
void Expression<Table>::make_postfix_notation() {
	vector<Token> postfix;
	stack<Token> ops;

	for (const Token &token: _tokens) {
		string name = token.name;
		switch (token.type) {
			case NUM:
				if (token.name != "")
					variable_positions[name].push_back(postfix.size());
				postfix.push_back(token);

				break;

			case OP:
				if (!ops.empty() && (prior[name] < 0 && prior[ops.top().name] >= prior[name] || prior[name] > 0 && prior[ops.top().name] > prior[name]))
					while (!ops.empty() && ops.top().type != OPEN_BR && (prior[name] < 0 && prior[ops.top().name] >= prior[name] || prior[name] > 0 && prior[ops.top().name] > prior[name])) {
						postfix.push_back(ops.top());
						ops.pop();
					}
				ops.push(token);

				break;

			case OPEN_BR:
				ops.push(token);

				break;

			case CLOSE_BR:
				while (!ops.empty() && ops.top().type != OPEN_BR) {
					postfix.push_back(ops.top());
					ops.pop();
				}
				ops.pop();

				break;
		}
	}

	while (!ops.empty()) {
		postfix.push_back(ops.top());
		ops.pop();
	}

	_tokens = postfix;
}

template <typename Table>
Polinomial Expression<Table>::compute(Table values) {
	vector<Token> copy_tokens = _tokens; // копия набора лексем для вычисления, чтобы не портить исходные данные
	stack<Polinomial> operands;
	Polinomial result, op1, op2;

	if (!variable_positions.empty()) {
		for (auto &var : variable_positions) {

			for (auto &pos : variable_positions[var.first])
				copy_tokens[pos].pol = values[var.first];
		}
	}

	for (Token &token: copy_tokens) {
		switch (token.type) {
			case NUM:
				operands.push(token.pol);
				break;

			case OP:
				string name = token.name;

				op1 = operands.top();
				operands.pop();

				if (name == "-u")
					result = -op1;

				else {
					op2 = operands.top();
					operands.pop();

					if (name == "+")
						result = op2 + op1;
					else if (name == "-")
						result = op2 - op1;
					else if (name == "*")
						result = op1 * op2;
				}

				operands.push(result);
				break;
		}
	}

	return operands.top();
}

template <typename Table>
bool Expression<Table>::has_no_variables() {
	return variable_positions.empty();
}