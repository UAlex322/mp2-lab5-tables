#include "../include/polinomial.h"
#include <string>
using namespace std;

// приведение подобных слагаемых в многочлене с упорядоченными слагаемыми
void Polinomial::reduction(List<Monomial> &list) {
	auto prev_it = list.before_begin(), curr_it = list.begin(), next_it = (curr_it == list.end()) ? list.end() : std::next(list.begin());

	while (curr_it != list.end()) {
		if (next_it != list.end() && (*curr_it).power == (*next_it).power) {
			(*curr_it).value += (*next_it).value;
			++next_it;
			list.erase_after(curr_it);
		}
		else if (abs((*curr_it).value) < std::numeric_limits<double>::epsilon()) {
			if (next_it == list.end()) {
				list.erase_after(prev_it);
				break;
			}
			else {
				++curr_it, ++next_it;
				list.erase_after(prev_it);
			}
		}
		else if (next_it != list.end())
			++prev_it, ++curr_it, ++next_it;
		else ++curr_it; // сработает в последней итерации цикла, если текущий моном ненулевой
	}
}

// парсинг строки с многочленом
void Polinomial::parse(const std::string &s) {
	enum states {Number, Variable, Power, Sum, Mult}; // состояния: число, переменная, знак степени, знак суммы/разности, знак умножения
	states last_state = Sum;

	size_t first_digit_pos = -1; // позиция начала текущего считываемого числа
	double sign_value = 1.0; // множитель, обозначающий знак перед мономом
	bool was_dot = false; // была ли считана точка текущего числа

	char curr_variable = ' '; // текущая переменная, к которой будет относиться степень
	double curr_coefficient; // коэффициент перед текущим мономом
	int curr_powers[3]{}; // степени текущего монома

	size_t i = 0;
	if (s[i] == '-') {
		sign_value = -1.0;
		++i;
	}
	else if (s[i] == '+') {
		sign_value = 1.0;
		++i;
	}

	for ( ; i < s.size(); ++i) {
		switch (last_state) {
			case Sum:
				if ('0' <= s[i] && s[i] <= '9') {
					first_digit_pos = i;
					last_state = Number;
				}

				else if (s[i] == 'x' || s[i] == 'y' || s[i] == 'z') {
					curr_coefficient = sign_value;
					curr_variable = s[i];
					last_state = Variable;
				}

				//else if (s[i] == '-')

				else if (s[i] != ' ')
					throw string("error: polinomial is incorrect");

				break;
			case Number:
				// встретился разделитель
				if (s[i] == ' ' || s[i] == '\t' || s[i] == '*' || s[i] == '+' || s[i] == '-') {
					if (first_digit_pos != -1) {
						curr_coefficient = sign_value * std::stod(s.substr(first_digit_pos, i-first_digit_pos));
						first_digit_pos = -1;
						was_dot = false;
					}

					if (s[i] == '*')
						last_state = Mult;

					if (s[i] == '+' || s[i] == '-') {
						list.emplace_front(curr_coefficient, 0);
						sign_value = (s[i] == '+') ? 1.0 : -1.0;
						last_state = Sum;
					}
				}

				else if (s[i] == '.' && !was_dot)
					was_dot = true;

				else if (s[i] == 'x' || s[i] == 'y' || s[i] == 'z') {
					if (first_digit_pos != -1) {
						curr_coefficient = sign_value * std::stod(s.substr(first_digit_pos, i-first_digit_pos));
						first_digit_pos = -1;
						was_dot = false;
					}

					curr_variable = s[i];
					last_state = Variable;
				}

				else if (s[i] < '0' || s[i] > '9')
					throw string("error: polinomial is incorrect");

				break;
			case Variable:
				if (s[i] == 'x' || s[i] == 'y' || s[i] == 'z') {
					if (curr_variable != ' ')
						++curr_powers[curr_variable-'x'];
					curr_variable = s[i];
				}

				else if (s[i] == '*') {
					if (curr_variable != ' ')
						++curr_powers[curr_variable-'x'];
					curr_variable = ' ';
					last_state = Mult;
				}

				else if (s[i] == '^')
					last_state = Power;

				else if (s[i] == '+' || s[i] == '-') {
					if (curr_variable != ' ') {
						++curr_powers[curr_variable-'x'];
					}
					if (curr_powers[0] < 10 && curr_powers[1] < 10 && curr_powers[2] < 10) {
						list.emplace_front(curr_coefficient, 100*curr_powers[0] + 10*curr_powers[1] + curr_powers[2]);
						curr_powers[0] = curr_powers[1] = curr_powers[2] = 0;
					}

					sign_value = (s[i] == '+') ? 1.0 : -1.0;
					last_state = Sum;
				}

				else if (s[i] != ' ')
					throw string("error: polinomial is incorrect");

				break;
			case Power:
				if ('0' <= s[i] && s[i] <= '9') {
					curr_powers[curr_variable-'x'] += s[i]-'0';
					curr_variable = ' ';
					last_state = Variable;
				}

				else if (s[i] != ' ')
					throw string("error: polinomial is incorrect");

				break;
			case Mult:
				if (s[i] == 'x' || s[i] == 'y' || s[i] == 'z') {
					curr_variable = s[i];
					last_state = Variable;
				}

				else if (s[i] != ' ')
					throw string("error: polinomial is incorrect");
		}
	}

	if (last_state == Number) {
		curr_coefficient = sign_value * std::stod(s.substr(first_digit_pos, s.size()-first_digit_pos));
		list.emplace_front(curr_coefficient, 0);
	}

	if (last_state == Variable) {
		if (curr_variable != ' ')
			++curr_powers[s.back()-'x'];

		if (curr_powers[0] < 10 && curr_powers[1] < 10 && curr_powers[2] < 10) {
			list.emplace_front(curr_coefficient, 100*curr_powers[0] + 10*curr_powers[1] + curr_powers[2]);
			curr_powers[0] = curr_powers[1] = curr_powers[2] = 0;
		}
		else
			throw string("error: polinomial is incorrect");
	}
}

// c * P(x,y,z)
Polinomial operator*(const double value, const Polinomial &pol) {
	Polinomial result = pol;
	result *= value;
	return result;
}

Polinomial::Polinomial() {
	list = List<Monomial>();
}

Polinomial::Polinomial(const Polinomial &copy) {
	list = copy.list;
}

Polinomial::Polinomial(Polinomial &&move) noexcept {
	list = std::move(move.list);
}

Polinomial::Polinomial(const std::string &str) {
	parse(str);
	list.sort();
	reduction(list);
}

Polinomial::Polinomial(const std::initializer_list<Mono> &init) {
	for (auto &elem : init)
		list.push_front(Monomial(elem.value, 100*elem.pow1 + 10*elem.pow2 + elem.pow3));
	list.sort();
	reduction(list);
}


Polinomial& Polinomial::operator=(const Polinomial &copy) {
	if (this == &copy) return *this;
	list = copy.list;
	return *this;
}

Polinomial& Polinomial::operator=(Polinomial &&move) noexcept {
	if (this == &move) return *this;
	list = std::move(move.list);
	return *this;
}


Polinomial& Polinomial::operator*=(const double value) {
	for (auto &mon : list)
		mon.value *= value;

	return *this;
}

Polinomial Polinomial::operator*(const double value) {
	Polinomial result = *this;
	result *= value;
	return result;
}

Polinomial& Polinomial::operator+=(const Polinomial &pol) {
	list.merge(pol.list);
	reduction(list);
	return *this;
}

Polinomial Polinomial::operator+(const Polinomial &pol) const {
	Polinomial result = *this;
	result += pol;
	return result;
}

Polinomial& Polinomial::operator-=(const Polinomial &pol) {
	*this += (-1.0)*pol;
	return *this;
}

Polinomial Polinomial::operator-(const Polinomial &pol) const {
	Polinomial result = *this;
	result -= pol;
	return result;
}

Polinomial Polinomial::operator-() const {
	return (-1.0)*(*this);
}

Polinomial& Polinomial::operator*=(const Polinomial &pol) {
	List<Monomial> result_list;

	for (const auto &mon1 : list) {
		for (const auto &mon2 : pol.list) {
			result_list.push_front(mon1*mon2);
		}
	}
	result_list.sort();
	reduction(result_list);
	list = std::move(result_list);

	return *this;
}

Polinomial Polinomial::operator*(const Polinomial &pol) const{
	Polinomial result = *this;
	result *= pol;
	return result;
}

bool Polinomial::operator==(const Polinomial &pol) const {
	return list == pol.list;
}

bool Polinomial::operator!=(const Polinomial &pol) const {
	return list != pol.list;
}

std::istream& operator>>(std::istream &ist, Polinomial &pol) {
	std::string str;

	getline(ist,str);
	pol = std::move(Polinomial(str));

	return ist;
}


std::ostream& operator<<(std::ostream &ost, const Polinomial &pol) {
	size_t count = 0;
	const double epsilon = std::numeric_limits<double>::epsilon();

	if (pol.list.empty()) ost << 0;
	else {
		for (auto &mon : pol.list) {
			// далее в некоторых местах делаются проверки, что коэффициент одночлена - 1 или -1; если это так, число не выводится
			if (count == 0) {
				if (mon.power == 0 || mon.power != 0 && std::abs(abs(mon.value)-1.0)>epsilon)
					ost << mon.value;
				else
					ost << ((mon.value >= 0.0) ? "" : "-");
			}
			else {
				if (mon.power != 0 && std::abs(abs(mon.value)-1.0)>epsilon)
					ost << ((mon.value >= 0.0) ? " + " : " - ") << abs(mon.value);
				else
					ost << ((mon.value >= 0.0) ? " + " : " - ");
			}

			if (mon.power > 0) {
				if (std::abs(abs(mon.value)-1.0)>epsilon)
					ost << '*';
				if (mon.power/100 > 1)
					ost << "x^" << mon.power/100;
				else if (mon.power/100 == 1)
					ost << "x";
				if (mon.power/10%10 > 1)
					ost << "y^" << mon.power/10%10;
				else if (mon.power/10%10 == 1)
					ost << "y";
				if (mon.power%10 > 1)
					ost << "z^" << mon.power%10;
				else if (mon.power%10 == 1)
					ost << "z";
			}

			++count;
		}
	}

	return ost;
}