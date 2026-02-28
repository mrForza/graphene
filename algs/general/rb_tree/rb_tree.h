#ifndef RB_TREE_H
#define RB_TREE_H

#include <iostream>

enum class Color {
    Red,
    Black
};

template<typename T>
struct Node {
    T data;
    Color color;
    Node* parent;
    Node* left;
    Node* right;

    Node(T data) : data(data), color(Color::Red), parent(nullptr), left(nullptr), right(nullptr) {}
};

template<typename T>
class RedBlackTree {
private:
    Node<T>* root;
    Node<T>* nil; // Sentinel node

    void rotateLeft(Node<T>*& node);
    void rotateRight(Node<T>*& node);
    void fixInsert(Node<T>*& node);
    void fixDelete(Node<T>*& node);
    Node<T>* minValueNode(Node<T>*& node);
    void transplant(Node<T>*& u, Node<T>*& v);
    void deleteTree(Node<T>* node);
    void printTreeHelper(Node<T>* node, std::string indent, bool last);

public:
    RedBlackTree();
    ~RedBlackTree();
    void insert(T key);
    void remove(T key);
    void printTree();
    Node<T>* search(T key);
};

#include "rb_tree.cpp"

#endif // RB_TREE_H
