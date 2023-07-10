#pragma once
#include <iostream>

#include <iterator>
#include <initializer_list>
#include <algorithm>

template <typename Type>
class SingleLinkedList {
	// Узел списка
	struct Node {
		Node() = default;
		Node(const Type& val, Node* next)
			: value(val)
			, next_node(next) {
		}
		Type value;
		Node* next_node = nullptr;
	};

	template <typename ValueType>
	class BasicIterator {
	public:

		friend class SingleLinkedList;

		explicit BasicIterator(Node* node) : node_(node)
		{
		}

		BasicIterator() = default;

		// Конвертирующий конструктор/конструктор копирования
		BasicIterator(const BasicIterator<Type>& other) noexcept {
			node_ = other.node_;
		}

		BasicIterator& operator=(const BasicIterator& rhs) = default;

		// Категория итератора — forward iterator (итератор, который поддерживает операции инкремента и многократное разыменование)
		using iterator_category = std::forward_iterator_tag;
		// Тип элементов, по которым перемещается итератор
		using value_type = Type;
		// Тип, используемый для хранения смещения между итераторами
		using difference_type = std::ptrdiff_t;
		// Тип указателя на итерируемое значение
		using pointer = ValueType*;
		// Тип ссылки на итерируемое значение
		using reference = ValueType&;


		// Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		// Оператор проверки итераторов на неравенство
		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return !(node_ == rhs.node_);
		}

		// Оператор сравнения итераторов (в роли второго аргумента итератор)
		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		// Оператор проверки итераторов на неравенство
		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return !(node_ == rhs.node_);
		}

		// Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
		BasicIterator& operator++() noexcept {
			node_ = node_->next_node;
			return *this;
		}

		// Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
		BasicIterator operator++(int) noexcept {
			BasicIterator copy_node_(*this);
			++(*this);
			return copy_node_;
		}

		// Операция разыменования. Возвращает ссылку на текущий элемент
		[[nodiscard]] reference operator*() const noexcept {
			return node_->value;
		}

		// Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
		[[nodiscard]] pointer operator->() const noexcept {
			return &node_->value;
		}

	private:
		Node* node_ = nullptr;
	};

public:
	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;

	using Iterator = BasicIterator<Type>;
	using ConstIterator = BasicIterator<const Type>;

	SingleLinkedList() :head_() {
	}

	~SingleLinkedList() {
		Clear();
	}

	SingleLinkedList(std::initializer_list<Type> values) {
		Initializer(values);
	}

	SingleLinkedList(const SingleLinkedList& other) {
		Initializer(other);
	}

	SingleLinkedList& operator=(const SingleLinkedList& rhs) {
		if (this != &rhs) {
			auto copy_rhs(rhs);
			swap(copy_rhs);
			return *this;
		}
		return *this;
	}

	[[nodiscard]] size_t GetSize() const noexcept {
		return size_;
	}

	[[nodiscard]] size_t size() const noexcept {
		return GetSize();
	}

	[[nodiscard]] bool IsEmpty() const noexcept {
		return size_ == 0;
	}

	void PushFront(const Type& value) {
		head_.next_node = new Node(value, head_.next_node);
		++size_;
	}

	void Clear() noexcept {
		Node* prev_node = nullptr;
		for (; head_.next_node != nullptr; prev_node = head_.next_node, head_.next_node = prev_node->next_node) {
			if (prev_node != nullptr) {
				delete prev_node;
				--size_;
			}
		}
		delete prev_node;
		--size_;
	}

	void swap(SingleLinkedList& other) noexcept {
		if (this != &other) {
			Node* temp_head = other.head_.next_node;
			size_t temp_size = other.GetSize();

			other.head_.next_node = head_.next_node;
			other.size_ = size_;

			head_.next_node = temp_head;
			size_ = temp_size;
		}
	}

	void PopFront() noexcept {
		if (size_ == 0) {
			return;
		}
		Node* first_node = head_.next_node;
		head_.next_node = first_node->next_node;
		delete first_node;
		--size_;
	}

	//Вставляет элемент value после элемента, на который указывает pos.
	//Возвращает итератор на вставленный элемент
	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		if (size_ == 0) {
			Node* new_value = new Node(value, nullptr);
			head_.next_node = new_value;
			++size_;
			return { Iterator(new_value) };
		}
		
		Node* prev_node = &head_;
		for (auto itera = this->before_begin(); itera != this->end(); ++itera) {
			Node* curr_node = prev_node->next_node;

			if (itera == pos) {
				Node* new_value = new Node(value, curr_node);
				prev_node->next_node = new_value;
				new_value->next_node = curr_node;
				++size_;
				return { Iterator(new_value) };
			}
			prev_node = curr_node;
		}
		return { this->end() };
	}

	//Удаляет элемент, следующий за pos.
	//Возвращает итератор на элемент, следующий за удалённым
	Iterator EraseAfter(ConstIterator pos) noexcept {
		if (size_ == 0) {
			return Iterator(nullptr);
		}
		
		Node* prev_node = &head_;

		for (auto itera = this->before_begin(); itera != this->end(); ++itera) {
			Node* curr_node = prev_node->next_node;

			if (itera == pos) {
				prev_node->next_node = curr_node->next_node;
				delete curr_node;
				--size_;
				return { Iterator(prev_node->next_node) };
			}
			prev_node = curr_node;
		}
		return { this->end() };
	}

	// Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
	[[nodiscard]] Iterator before_begin() noexcept {
		return  Iterator(&head_);
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		ConstIterator result;
		result.node_ = const_cast<Node*>( & head_);
		return result;
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	[[nodiscard]] ConstIterator before_begin() const noexcept {
		return cbefore_begin();
	}

	// Возвращает итератор, ссылающийся на первый элемент
	[[nodiscard]] Iterator begin() noexcept {
		return Iterator(head_.next_node);
	}

	// Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	[[nodiscard]] Iterator end() noexcept {
		return { Iterator(nullptr) };
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	[[nodiscard]] ConstIterator begin() const noexcept {
		return cbegin();
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	[[nodiscard]] ConstIterator end() const noexcept {
		return cend();
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return { ConstIterator(head_.next_node) };
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	[[nodiscard]] ConstIterator cend() const noexcept {
		return { ConstIterator(nullptr) };
	}

private:
	Node head_;
	size_t size_ = 0;

	template<typename Container>
	void Initializer(const Container& container) {
		SingleLinkedList copy_list;
		copy_list.size_ = container.size();

		if (container.begin() != container.end()) {
			auto itera = container.begin();
			Node* head_temp = new Node(*itera, nullptr);
			copy_list.head_.next_node = head_temp;

			for (++itera; itera != container.end(); ++itera) {
				Node* next_node = new Node(*itera, nullptr);
				head_temp->next_node = next_node;
				head_temp = next_node;
			}
			swap(copy_list);
		}
	}
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	if (lhs.GetSize() != rhs.GetSize()) {
		return false;
	}

	auto r_itera = rhs.cbegin();
	auto l_itera = lhs.cbegin();
	auto re_itera = rhs.cend();
	auto le_itera = lhs.cend();

	for (; r_itera != re_itera && l_itera != le_itera; ++r_itera, ++l_itera) {
		if (*l_itera != *r_itera) {
			return false;
		}
	}
	return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	auto r_itera = rhs.cbegin();
	auto l_itera = lhs.cbegin();
	auto re_itera = rhs.cend();
	auto le_itera = lhs.cend();

	for (; r_itera != re_itera && l_itera != le_itera; ++r_itera, ++l_itera) {
		if (*l_itera > *r_itera) {
			return false;
		}
	}
	return true;
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	auto r_itera = rhs.cbegin();
	auto l_itera = lhs.cbegin();
	auto re_itera = rhs.cend();
	auto le_itera = lhs.cend();

	for (; r_itera != re_itera && l_itera != le_itera; ++r_itera, ++l_itera) {
		if (*l_itera < *r_itera) {
			return false;
		}
	}
	return true;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs < rhs) || (lhs == rhs);
}