#ifndef D_LIST_H
#define D_LIST_H

#include <cstddef>

template<typename T>
struct Node {
    T data;
    Node* prev;
    Node* next;

    Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
};

template<typename T>
class DoublyLinkedList {
public:
    // Iterator class
    class Iterator {
    public:
        Iterator(Node<T>* node) : current(node) {}

        T& operator*() {
            return current->data;
        }

        const T& operator*() const {
            return current->data;
        }

        T* operator->() {
            return &(current->data);
        }

        const T* operator->() const {
            return &(current->data);
        }

        Iterator& operator++() {
            if (current != nullptr) {
                current = current->next;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        Iterator& operator--() {
            if (current != nullptr) {
                current = current->prev;
            }
            return *this;
        }

        Iterator operator--(int) {
            Iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        friend class DoublyLinkedList<T>;

    private:
        Node<T>* current;
    };

private:
    Node<T>* head;
    Node<T>* tail;
    size_t list_size;

    void unlink_node(Node<T>* node);
    void link_node_after(Node<T>* new_node, Node<T>* pos);

public:
    DoublyLinkedList();
    ~DoublyLinkedList();
    DoublyLinkedList(const DoublyLinkedList& other);
    DoublyLinkedList& operator=(const DoublyLinkedList& other);

    void push_front(const T& value);
    void push_back(const T& value);
    Iterator insert_after(Iterator pos, const T& value);
    void erase(Iterator pos);
    T& front();
    T& back();
    const T& front() const;
    const T& back() const;
    bool empty() const;
    size_t size() const;
    void clear();

    Iterator begin();
    Iterator end();
};

#include "d_list.cpp"

#endif // D_LIST_H
