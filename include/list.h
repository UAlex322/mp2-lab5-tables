#pragma once
#include <iterator>
#include <initializer_list>
#include <iostream>

// Односвязный список

template <typename Type>
class List {
private:
	// Узел списка
	struct Node {
		Node *next;
		Type data;

		Node(const Node &copy): data(copy.data) {}
		Node(const Type &value = Type()): data(value) {}
	};

	void merge_general(Node *&first, Node *second); // обобщённая процедура слияния (принимает узлы списков, результат слияния лежит по первому указателю)
	void mergesort(Node *&first); // внутренняя функция сортировки слиянием

	Node *head = new Node(); // голова списка
	size_t _size; // текущее количество узлов в списке без учёта головы

public:

	List(size_t _size = 0);
	List(const List &copy);
	List(List &&move) noexcept;
	List(const std::initializer_list<Type> &list);
	template <typename Iterator>
	List(Iterator first, Iterator second); // range-конструктор
	~List();
	List& operator=(const List &copy);
	List& operator=(List &&move) noexcept;

	size_t size() const;
	bool empty() const;
	void clear();

	void swap(List &other);
	void reverse();
	void merge(List other); // слияние списков
	void sort();

	bool operator==(const List &other) const;
	bool operator!=(const List &other) const;

	// Итератор списка
	class iterator {
	public:
		friend class List;

		using value_type = Type;
		using pointer = Type*;
		using reference = Type&;
		using difference_type = ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		iterator(Node *ptr): ptr(ptr) {}

		reference operator*() const { return (*ptr).data; }
		iterator operator++() { ptr = ptr->next; return *this; }
		iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
		bool operator==(const iterator &it) const { return this->ptr == it.ptr; }
		bool operator!=(const iterator &it) const { return this->ptr != it.ptr; }
	private:
		Node *ptr;
	};

	// Константный итератор списка
	class const_iterator {
	public:
		friend class List;

		using value_type = const Type;
		using pointer = const Type*;
		using reference = const Type&;
		using difference_type = ptrdiff_t;
		using iterator_category = std::forward_iterator_tag;

		const_iterator(Node *ptr): ptr(ptr) {}

		reference operator*() const { return (*ptr).data; }
		const_iterator operator++() { ptr = ptr->next; return *this; }
		const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
		bool operator==(const const_iterator &it) const { return this->ptr == it.ptr; }
		bool operator!=(const const_iterator &it) const { return this->ptr != it.ptr; }
	private:
		Node *ptr;
	};

	iterator before_begin() {return iterator(head);} // итератор на голову списка
	const_iterator before_begin() const {return const_iterator(head);} // константный итератор на голову списка
	iterator begin() {return iterator(head->next);} // итератор на первый элемент списка
	const_iterator begin() const {return const_iterator(head->next);} // константный итератор на первый элемент списка
	iterator end() {return iterator(nullptr);} // итератор на конец списка
	const_iterator end() const {return const_iterator(nullptr);} // итератор на конец списка
	const_iterator cbefore_begin() const {return const_iterator(head);}
	const_iterator cbegin() const {return const_iterator(head->next);}
	const_iterator cend() const {return const_iterator(nullptr);}

	iterator find_prev(const Type &value); // Поиск элемента и возврат итератора перед узлом с данным значением
	iterator insert_after(const iterator &pos, const Type &value); // вставка после элемента, на который указывает итератор
	template <typename... Args>
	iterator emplace_after(const iterator &pos, Args&&... args); // 
	iterator erase_after(const iterator &pos);
	void push_front(const Type &value);
	template <typename... Args>
	void emplace_front(Args&&... args);
	void pop_front();

};

template<typename Type>
List<Type>::List(size_t _size): _size(_size) {
	Node *curr_node = head;

	for (size_t i = 0; i < _size; ++i) {
		curr_node->next = new Node();
		curr_node = curr_node->next;
	}

	curr_node->next = nullptr;
}

template<typename Type>
List<Type>::List(const List &copy): _size(copy._size) {
	Node *curr_ptr = head, *copy_curr_ptr = copy.head;

	while (copy_curr_ptr->next != nullptr) {
		curr_ptr->next = new Node(copy_curr_ptr->next->data);
		curr_ptr = curr_ptr->next;
		copy_curr_ptr = copy_curr_ptr->next;
	}

	curr_ptr->next = nullptr;
}

template<typename Type>
List<Type>::List(List &&move) noexcept: _size(move._size) {
	head->next = move.head->next;
	move.head->next = nullptr;
	move._size = 0;
}

template<typename Type>
List<Type>::List(const std::initializer_list<Type> &list): _size(list.size()) {
	Node *curr_ptr = head;

	for (auto &elem : list) {
		curr_ptr->next = new Node(elem);
		curr_ptr = curr_ptr->next;
	}

	curr_ptr->next = nullptr;
}

template <typename Type>
template <typename Iterator>
List<Type>::List(Iterator first, Iterator second) { // промежуток [first; last)
	Node *curr = head;
	
	while (first != second) {
		curr->next = new Node(*first);
		curr = curr->next;
		++first;
	}
	curr->next = nullptr;
}

template<typename Type>
List<Type>& List<Type>::operator=(const List &copy) {
	if (this == &copy) return *this;

	Node *curr_ptr = head, *copy_curr_ptr = copy.head;

	while (curr_ptr->next != nullptr && copy_curr_ptr->next != nullptr) {
		*(curr_ptr->next) = *(copy_curr_ptr->next);
		curr_ptr = curr_ptr->next;
		copy_curr_ptr = copy_curr_ptr->next;
	}

	// Если закончился исходный список, а у нового больший размер
	if (curr_ptr->next == nullptr) {
		while (copy_curr_ptr->next != nullptr) {
			curr_ptr->next = new Node(*(copy_curr_ptr->next));
			curr_ptr = curr_ptr->next;
			copy_curr_ptr = copy_curr_ptr->next;
		}
		curr_ptr->next = nullptr;
	}
	// Новый список кончился, а исходного больший размер
	else if (copy_curr_ptr->next == nullptr) {
		Node *end_ptr = curr_ptr, *next_ptr = curr_ptr;

		while (curr_ptr->next != nullptr) {
			next_ptr = curr_ptr->next;
			delete curr_ptr;
			curr_ptr = next_ptr;
		}

		end_ptr->next = nullptr;
	}

	_size = copy._size;

	return *this;
}

template<typename Type>
List<Type> &List<Type>::operator=(List &&move) noexcept {
	head->next = move.head->next;
	move.head->next = nullptr;
	_size = move._size;
	move._size = 0;

	return *this;
}

template<typename Type>
List<Type>::~List() {
	clear();
}

template<typename Type>
size_t List<Type>::size() const {
	return _size;
}

template<typename Type>
bool List<Type>::empty() const {
	return _size == 0;
}

template<typename Type>
void List<Type>::clear() {
	Node *next_ptr, *curr_ptr = head;

	while (curr_ptr->next != nullptr) {
		next_ptr = curr_ptr->next;
		delete curr_ptr;
		curr_ptr = next_ptr;
	}

	head->next = nullptr;
	_size = 0;
}

template<typename Type>
typename List<Type>::iterator List<Type>::find_prev(const Type &value) {
	Node *curr_ptr = head;

	while (curr_ptr->next != nullptr) {
		if (curr_ptr->next->data == value)
			return iterator(curr_ptr);
		curr_ptr = curr_ptr->next;
	}

	return iterator(curr_ptr); // Если нет такого элемента, возвращается итератор на последний элемент
}

template<typename Type>
typename List<Type>::iterator List<Type>::insert_after(const iterator &pos, const Type &value) {
	Node *new_node = new Node(value);

	new_node->next = pos.ptr->next;
	pos.ptr->next = new_node;
	++(_size);

	return new_node;
}

template<typename Type>
template<typename... Args>
typename List<Type>::iterator List<Type>::emplace_after(const iterator &pos, Args&&... args) {
	Node *new_node = new Node(Type(args...));

	new_node->next = pos.ptr->next;
	pos.ptr->next = new_node;
	++(_size);

	return new_node;
}

template<typename Type>
typename List<Type>::iterator List<Type>::erase_after(const iterator &pos) {

	Node *erase_node = pos.ptr->next;
	pos.ptr->next = erase_node->next;
	delete erase_node;
	--(_size);

	return iterator(pos.ptr->next);
}

template<typename Type>
void List<Type>::push_front(const Type &value) {
	Node *new_node = new Node(value);

	new_node->next = head->next;
	head->next = new_node;
	++(_size);
}

template<typename Type>
template<typename... Args>
void List<Type>::emplace_front(Args&&... args) {
	Node *new_node = new Node(Type(args...));

	new_node->next = head->next;
	head->next = new_node;
	++(_size);
}

template<typename Type>
void List<Type>::pop_front() {
	Node *erase_node = head->next;

	head->next = erase_node->next;
	delete erase_node;
	--(_size);
}

template<typename Type>
void List<Type>::swap(List &other) {
	swap(head,other.head);
	swap(_size,other._size);
}

template<typename Type>
void List<Type>::reverse() {
	Node *new_head = new Node(), *curr = head->next, *copy_curr;
	new_head->next = nullptr;

	while (curr != nullptr) {
		copy_curr = curr;
		curr = curr->next;
		copy_curr->next = new_head->next;
		new_head->next = copy_curr;
	}

	head = new_head;
}


// Обобщённая процедура слияния (принимает узлы списков, результат слияния лежит по первому указателю)
template<typename Type>
void List<Type>::merge_general(Node *&first, Node *second) {
	Node *merged_begin = new Node(), *curr_merged = merged_begin, *curr1 = first, *curr2 = second;

	while (curr1 != nullptr && curr2 != nullptr) {
		if (curr1->data <= curr2->data) {
			curr_merged->next = curr1;
			curr1 = curr1->next;
		}
		else {
			curr_merged->next = curr2;
			curr2 = curr2->next;
		}
		curr_merged = curr_merged->next;
	}

	if (curr1 == nullptr) {
		while (curr2 != nullptr) {
			curr_merged->next = curr2;
			curr2 = curr2->next;
			curr_merged = curr_merged->next;
		}
	}
	else if (curr2 == nullptr) {
		while (curr1 != nullptr) {
			curr_merged->next = curr1;
			curr1 = curr1->next;
			curr_merged = curr_merged->next;
		}
	}

	curr_merged->next = nullptr;
	first = merged_begin->next;
}

// Слияние двух отсортированных списков
template<typename Type>
void List<Type>::merge(List other) {
	_size += other._size;

	merge_general(head->next, other.head->next);
	other.head->next = nullptr;
}

template<typename Type>
void List<Type>::mergesort(Node *&first) {
	if (first == nullptr || first->next == nullptr) return;

	Node *mid = first, *fast = first->next, *second;
	// Поиск середины
	while (fast->next != nullptr) {
		mid = mid->next;
		fast = fast->next;
		if (fast->next == nullptr) break;
		fast = fast->next;
	}

	second = mid->next;
	mid->next = nullptr;
	mergesort(first);
	mergesort(second);
	merge_general(first,second);
}

template<typename Type>
void List<Type>::sort() {
	mergesort(head->next);
}

template<typename Type>
bool List<Type>::operator==(const List &other) const {
	if (this == &other) return true;
	if (_size != other._size) return false;

	for (auto it1 = cbegin(), it2 = other.cbegin(); it1 != cend(); ++it1, ++it2)
		if (*it1 != *it2)
			return false;

	return true;
}

template<typename Type>
bool List<Type>::operator!=(const List &other) const {
	return !(*this == other);
}