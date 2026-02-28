#ifndef RB_TREE_CPP
#define RB_TREE_CPP

#include "rb_tree.h"
#include <iostream>
#include <string>
#include <algorithm>

template<typename T>
RedBlackTree<T>::RedBlackTree() {
    nil = new Node<T>(T());
    nil->color = Color::Black;
    nil->left = nil;
    nil->right = nil;
    nil->parent = nullptr;
    root = nil;
}

template<typename T>
RedBlackTree<T>::~RedBlackTree() {
    deleteTree(root);
    delete nil;
}

template<typename T>
void RedBlackTree<T>::deleteTree(Node<T>* node) {
    if (node != nil) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

template<typename T>
void RedBlackTree<T>::rotateLeft(Node<T>*& node) {
    Node<T>* rightChild = node->right;
    node->right = rightChild->left;
    
    if (rightChild->left != nil) {
        rightChild->left->parent = node;
    }
    
    rightChild->parent = node->parent;
    
    if (node->parent == nullptr) {
        root = rightChild;
    } else if (node == node->parent->left) {
        node->parent->left = rightChild;
    } else {
        node->parent->right = rightChild;
    }
    
    rightChild->left = node;
    node->parent = rightChild;
}

template<typename T>
void RedBlackTree<T>::rotateRight(Node<T>*& node) {
    Node<T>* leftChild = node->left;
    node->left = leftChild->right;
    
    if (leftChild->right != nil) {
        leftChild->right->parent = node;
    }
    
    leftChild->parent = node->parent;
    
    if (node->parent == nullptr) {
        root = leftChild;
    } else if (node == node->parent->right) {
        node->parent->right = leftChild;
    } else {
        node->parent->left = leftChild;
    }
    
    leftChild->right = node;
    node->parent = leftChild;
}

template<typename T>
void RedBlackTree<T>::fixInsert(Node<T>*& node) {
    Node<T>* uncle;
    
    while (node->parent != nullptr && node->parent->color == Color::Red) {
        if (node->parent == node->parent->parent->right) {
            uncle = node->parent->parent->left;
            
            if (uncle != nil && uncle->color == Color::Red) {
                uncle->color = Color::Black;
                node->parent->color = Color::Black;
                node->parent->parent->color = Color::Red;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rotateRight(node);
                }
                node->parent->color = Color::Black;
                node->parent->parent->color = Color::Red;
                rotateLeft(node->parent->parent);
            }
        } else {
            uncle = node->parent->parent->right;
            
            if (uncle != nil && uncle->color == Color::Red) {
                uncle->color = Color::Black;
                node->parent->color = Color::Black;
                node->parent->parent->color = Color::Red;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rotateLeft(node);
                }
                node->parent->color = Color::Black;
                node->parent->parent->color = Color::Red;
                rotateRight(node->parent->parent);
            }
        }
        
        if (node == root) {
            break;
        }
    }
    
    root->color = Color::Black;
}

template<typename T>
void RedBlackTree<T>::insert(T key) {
    Node<T>* newNode = new Node<T>(key);
    newNode->left = nil;
    newNode->right = nil;
    
    Node<T>* parent = nullptr;
    Node<T>* current = root;
    
    while (current != nil) {
        parent = current;
        if (newNode->data < current->data) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    newNode->parent = parent;
    
    if (parent == nullptr) {
        root = newNode;
    } else if (newNode->data < parent->data) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }
    
    fixInsert(newNode);
}

template<typename T>
Node<T>* RedBlackTree<T>::minValueNode(Node<T>*& node) {
    Node<T>* current = node;
    while (current->left != nil) {
        current = current->left;
    }
    return current;
}

template<typename T>
void RedBlackTree<T>::transplant(Node<T>*& u, Node<T>*& v) {
    if (u->parent == nullptr) {
        root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

template<typename T>
void RedBlackTree<T>::fixDelete(Node<T>*& node) {
    Node<T>* sibling;
    
    while (node != root && node->color == Color::Black) {
        if (node == node->parent->left) {
            sibling = node->parent->right;
            
            if (sibling->color == Color::Red) {
                sibling->color = Color::Black;
                node->parent->color = Color::Red;
                rotateLeft(node->parent);
                sibling = node->parent->right;
            }
            
            if (sibling->left->color == Color::Black && sibling->right->color == Color::Black) {
                sibling->color = Color::Red;
                node = node->parent;
            } else {
                if (sibling->right->color == Color::Black) {
                    sibling->left->color = Color::Black;
                    sibling->color = Color::Red;
                    rotateRight(sibling);
                    sibling = node->parent->right;
                }
                
                sibling->color = node->parent->color;
                node->parent->color = Color::Black;
                sibling->right->color = Color::Black;
                rotateLeft(node->parent);
                node = root;
            }
        } else {
            sibling = node->parent->left;
            
            if (sibling->color == Color::Red) {
                sibling->color = Color::Black;
                node->parent->color = Color::Red;
                rotateRight(node->parent);
                sibling = node->parent->left;
            }
            
            if (sibling->right->color == Color::Black && sibling->left->color == Color::Black) {
                sibling->color = Color::Red;
                node = node->parent;
            } else {
                if (sibling->left->color == Color::Black) {
                    sibling->right->color = Color::Black;
                    sibling->color = Color::Red;
                    rotateLeft(sibling);
                    sibling = node->parent->left;
                }
                
                sibling->color = node->parent->color;
                node->parent->color = Color::Black;
                sibling->left->color = Color::Black;
                rotateRight(node->parent);
                node = root;
            }
        }
    }
    
    node->color = Color::Black;
}

template<typename T>
void RedBlackTree<T>::remove(T key) {
    Node<T>* nodeToDelete = search(key);
    if (nodeToDelete == nullptr || nodeToDelete == nil) {
        return;
    }
    
    Node<T>* x;
    Node<T>* y = nodeToDelete;
    Color originalColor = y->color;
    
    if (nodeToDelete->left == nil) {
        x = nodeToDelete->right;
        transplant(nodeToDelete, nodeToDelete->right);
    } else if (nodeToDelete->right == nil) {
        x = nodeToDelete->left;
        transplant(nodeToDelete, nodeToDelete->left);
    } else {
        y = minValueNode(nodeToDelete->right);
        originalColor = y->color;
        x = y->right;
        
        if (y->parent == nodeToDelete) {
            x->parent = y;
        } else {
            transplant(y, y->right);
            y->right = nodeToDelete->right;
            y->right->parent = y;
        }
        
        transplant(nodeToDelete, y);
        y->left = nodeToDelete->left;
        y->left->parent = y;
        y->color = nodeToDelete->color;
    }
    
    delete nodeToDelete;
    
    if (originalColor == Color::Black) {
        fixDelete(x);
    }
}

template<typename T>
Node<T>* RedBlackTree<T>::search(T key) {
    Node<T>* current = root;
    
    while (current != nil && current->data != key) {
        if (key < current->data) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return (current != nil) ? current : nullptr;
}

template<typename T>
void RedBlackTree<T>::printTreeHelper(Node<T>* node, std::string indent, bool last) {
    if (node != nil) {
        std::cout << indent;
        if (last) {
            std::cout << "R----";
            indent += "     ";
        } else {
            std::cout << "L----";
            indent += "|    ";
        }
        
        std::string color = (node->color == Color::Red) ? "RED" : "BLACK";
        std::cout << node->data << "(" << color << ")" << std::endl;
        printTreeHelper(node->left, indent, false);
        printTreeHelper(node->right, indent, true);
    }
}

template<typename T>
void RedBlackTree<T>::printTree() {
    if (root != nil) {
        printTreeHelper(root, "", true);
    } else {
        std::cout << "Tree is empty" << std::endl;
    }
}

#endif // RB_TREE_CPP
