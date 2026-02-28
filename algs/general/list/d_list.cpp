#ifndef D_LIST_CPP
#define D_LIST_CPP

#include "d_list.h"
#include <stdexcept>

template<typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr), list_size(0) {}

template<typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    clear();
}

template<typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList& other) 
    : head(nullptr), tail(nullptr), list_size(0) {
    Node<T>* current = other.head;
    while (current != nullptr) {
        push_back(current->data);
        current = current->next;
    }
}

template<typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList& other) {
    if (this != &other) {
        clear();
        Node<T>* current = other.head;
        while (current != nullptr) {
            push_back(current->data);
            current = current->next;
        }
    }
    return *this;
}

template<typename T>
void DoublyLinkedList<T>::unlink_node(Node<T>* node) {
    if (node == nullptr) {
        return;
    }

    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        // node is head
        head = node->next;
    }

    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        // node is tail
        tail = node->prev;
    }

    delete node;
    list_size--;
}

template<typename T>
void DoublyLinkedList<T>::link_node_after(Node<T>* new_node, Node<T>* pos) {
    if (new_node == nullptr) {
        return;
    }

    if (pos == nullptr) {
        // Insert at the beginning (before head)
        if (head == nullptr) {
            head = new_node;
            tail = new_node;
            new_node->prev = nullptr;
            new_node->next = nullptr;
        } else {
            new_node->next = head;
            new_node->prev = nullptr;
            head->prev = new_node;
            head = new_node;
        }
    } else {
        // Insert after pos
        new_node->next = pos->next;
        new_node->prev = pos;
        
        if (pos->next != nullptr) {
            pos->next->prev = new_node;
        } else {
            // pos is tail
            tail = new_node;
        }
        
        pos->next = new_node;
    }
    
    list_size++;
}

template<typename T>
void DoublyLinkedList<T>::push_front(const T& value) {
    Node<T>* new_node = new Node<T>(value);
    link_node_after(new_node, nullptr);
}

template<typename T>
void DoublyLinkedList<T>::push_back(const T& value) {
    Node<T>* new_node = new Node<T>(value);
    
    if (tail == nullptr) {
        // Empty list
        head = new_node;
        tail = new_node;
        new_node->prev = nullptr;
        new_node->next = nullptr;
        list_size++;
    } else {
        link_node_after(new_node, tail);
    }
}

template<typename T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::insert_after(Iterator pos, const T& value) {
    if (pos.current == nullptr) {
        // Insert at the end
        push_back(value);
        return Iterator(tail);
    }

    Node<T>* new_node = new Node<T>(value);
    link_node_after(new_node, pos.current);
    return Iterator(new_node);
}

template<typename T>
void DoublyLinkedList<T>::erase(Iterator pos) {
    if (pos.current == nullptr) {
        return;
    }
    unlink_node(pos.current);
}

template<typename T>
T& DoublyLinkedList<T>::front() {
    if (head == nullptr) {
        throw std::runtime_error("List is empty");
    }
    return head->data;
}

template<typename T>
const T& DoublyLinkedList<T>::front() const {
    if (head == nullptr) {
        throw std::runtime_error("List is empty");
    }
    return head->data;
}

template<typename T>
T& DoublyLinkedList<T>::back() {
    if (tail == nullptr) {
        throw std::runtime_error("List is empty");
    }
    return tail->data;
}

template<typename T>
const T& DoublyLinkedList<T>::back() const {
    if (tail == nullptr) {
        throw std::runtime_error("List is empty");
    }
    return tail->data;
}

template<typename T>
bool DoublyLinkedList<T>::empty() const {
    return list_size == 0;
}

template<typename T>
size_t DoublyLinkedList<T>::size() const {
    return list_size;
}

template<typename T>
void DoublyLinkedList<T>::clear() {
    Node<T>* current = head;
    while (current != nullptr) {
        Node<T>* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
    list_size = 0;
}

template<typename T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::begin() {
    return Iterator(head);
}

template<typename T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::end() {
    return Iterator(nullptr);
}

#endif // D_LIST_CPP
