#pragma once

#include <iterator>
#include <initializer_list>
#include <algorithm>
#include <cassert>

template <typename Type>
class SingleLinkedList {
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


		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return !(*this == rhs);
		}

		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return !(*this == rhs);
		}

		BasicIterator& operator++() noexcept {
			assert(node_ != nullptr);
			node_ = node_->next_node;
			return *this;
		}

		BasicIterator operator++(int) noexcept {
			BasicIterator copy_node_(*this);
			++(*this);
			return copy_node_;
		}

		[[nodiscard]] reference operator*() const noexcept {
			assert(node_ != nullptr);
			return node_->value;
		}

		[[nodiscard]] pointer operator->() const noexcept {
			assert(node_ != nullptr);
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
			}
		}
		delete prev_node;
		size_ = 0;
	}

	void swap(SingleLinkedList& other) noexcept {
		if (this != &other) {
			std::swap(other.head_.next_node, head_.next_node);
			std::swap(size_, other.size_);
		}
	}

	void PopFront() noexcept {
		assert(!IsEmpty());
		Node* first_node = head_.next_node;
		head_.next_node = first_node->next_node;
		delete first_node;
		--size_;
	}

	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		if ((size_ == 0) && (pos != this->before_begin())) {
			assert(IsEmpty());
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

	[[nodiscard]] Iterator before_begin() noexcept {
		return  Iterator(&head_);
	}

	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		ConstIterator result;
		result.node_ = const_cast<Node*>(&head_);
		return result;
	}

	[[nodiscard]] ConstIterator before_begin() const noexcept {
		return cbefore_begin();
	}

	[[nodiscard]] Iterator begin() noexcept {
		return Iterator(head_.next_node);
	}

	[[nodiscard]] Iterator end() noexcept {
		return { Iterator(nullptr) };
	}

	[[nodiscard]] ConstIterator begin() const noexcept {
		return cbegin();
	}

	[[nodiscard]] ConstIterator end() const noexcept {
		return cend();
	}

	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return { ConstIterator(head_.next_node) };
	}

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
	return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (rhs < lhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs < rhs);
}