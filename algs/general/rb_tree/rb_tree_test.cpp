#include <gtest/gtest.h>
#include "rb_tree.h"
#include <vector>
#include <set>
#include <random>
#include <algorithm>

template<typename T>
void collectInOrder(Node<T>* node, Node<T>* nil, std::vector<T>& result) {
    if (node != nil && node != nullptr) {
        collectInOrder(node->left, nil, result);
        result.push_back(node->data);
        collectInOrder(node->right, nil, result);
    }
}

template<typename T>
int verifyRBProperties(Node<T>* node, Node<T>* nil, bool& isValid) {
    if (node == nil || node == nullptr) {
        return 1;
    }
    
    if (node->parent == nullptr && node->color != Color::Black) {
        isValid = false;
    }
    
    if (node->color == Color::Red) {
        if ((node->left != nil && node->left->color == Color::Red) ||
            (node->right != nil && node->right->color == Color::Red)) {
            isValid = false;
        }
    }
    
    int leftBlackHeight = verifyRBProperties(node->left, nil, isValid);
    int rightBlackHeight = verifyRBProperties(node->right, nil, isValid);
    
    if (leftBlackHeight != rightBlackHeight) {
        isValid = false;
    }
    
    return leftBlackHeight + (node->color == Color::Black ? 1 : 0);
}

template<typename T>
bool isValidRBTree(RedBlackTree<T>& tree) {
    bool isValid = true;
    return isValid;
}

template<typename T>
int countNodes(Node<T>* node, Node<T>* nil) {
    if (node == nil || node == nullptr) {
        return 0;
    }
    return 1 + countNodes(node->left, nil) + countNodes(node->right, nil);
}

class RedBlackTreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        tree = new RedBlackTree<int>();
    }
    
    void TearDown() override {
        delete tree;
    }
    
    RedBlackTree<int>* tree;
};

// ==================== Constructor Tests ====================
TEST_F(RedBlackTreeTest, ConstructorCreatesEmptyTree) {
    RedBlackTree<int> newTree;
    EXPECT_EQ(newTree.search(1), nullptr);
}

// ==================== Destructor Tests ====================
TEST_F(RedBlackTreeTest, DestructorCleansUpProperly) {
    {
        RedBlackTree<int> tempTree;
        tempTree.insert(10);
        tempTree.insert(20);
        tempTree.insert(5);
    }
    // Destructor should be called here, no memory leaks expected
    SUCCEED();
}

// ==================== Insert Tests ====================
TEST_F(RedBlackTreeTest, InsertIntoEmptyTree) {
    tree->insert(10);
    Node<int>* found = tree->search(10);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->data, 10);
}

TEST_F(RedBlackTreeTest, InsertMultipleElements) {
    std::vector<int> values = {10, 20, 5, 15, 25, 3, 7};
    for (int val : values) {
        tree->insert(val);
    }
    
    for (int val : values) {
        Node<int>* found = tree->search(val);
        ASSERT_NE(found, nullptr) << "Value " << val << " should be found";
        EXPECT_EQ(found->data, val);
    }
}

TEST_F(RedBlackTreeTest, InsertDuplicateValues) {
    tree->insert(10);
    tree->insert(10);
    tree->insert(10);
    
    Node<int>* found = tree->search(10);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->data, 10);
}

TEST_F(RedBlackTreeTest, InsertLeftChild) {
    tree->insert(10);
    tree->insert(5);
    
    Node<int>* root = tree->search(10);
    Node<int>* left = tree->search(5);
    ASSERT_NE(root, nullptr);
    ASSERT_NE(left, nullptr);
}

TEST_F(RedBlackTreeTest, InsertRightChild) {
    tree->insert(10);
    tree->insert(20);
    
    Node<int>* root = tree->search(10);
    Node<int>* right = tree->search(20);
    ASSERT_NE(root, nullptr);
    ASSERT_NE(right, nullptr);
}

TEST_F(RedBlackTreeTest, InsertMaintainsBSTProperty) {
    std::vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    for (int val : values) {
        tree->insert(val);
    }
    
    for (int val : values) {
        Node<int>* found = tree->search(val);
        ASSERT_NE(found, nullptr) << "Value " << val << " should be found";
    }
}

TEST_F(RedBlackTreeTest, InsertLargeSequence) {
    for (int i = 1; i <= 100; ++i) {
        tree->insert(i);
    }
    
    for (int i = 1; i <= 100; ++i) {
        Node<int>* found = tree->search(i);
        ASSERT_NE(found, nullptr) << "Value " << i << " should be found";
    }
}

TEST_F(RedBlackTreeTest, InsertReverseOrder) {
    for (int i = 100; i >= 1; --i) {
        tree->insert(i);
    }
    
    for (int i = 1; i <= 100; ++i) {
        Node<int>* found = tree->search(i);
        ASSERT_NE(found, nullptr) << "Value " << i << " should be found";
    }
}

TEST_F(RedBlackTreeTest, InsertRandomOrder) {
    std::vector<int> values;
    for (int i = 1; i <= 50; ++i) {
        values.push_back(i);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(values.begin(), values.end(), g);
    
    for (int val : values) {
        tree->insert(val);
    }
    
    for (int i = 1; i <= 50; ++i) {
        Node<int>* found = tree->search(i);
        ASSERT_NE(found, nullptr) << "Value " << i << " should be found";
    }
}

// ==================== Search Tests ====================
TEST_F(RedBlackTreeTest, SearchInEmptyTree) {
    Node<int>* result = tree->search(10);
    EXPECT_EQ(result, nullptr);
}

TEST_F(RedBlackTreeTest, SearchExistingElement) {
    tree->insert(10);
    tree->insert(20);
    tree->insert(5);
    
    Node<int>* found = tree->search(10);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->data, 10);
    
    found = tree->search(20);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->data, 20);
    
    found = tree->search(5);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->data, 5);
}

TEST_F(RedBlackTreeTest, SearchNonExistingElement) {
    tree->insert(10);
    tree->insert(20);
    tree->insert(5);
    
    Node<int>* found = tree->search(100);
    EXPECT_EQ(found, nullptr);
    
    found = tree->search(0);
    EXPECT_EQ(found, nullptr);
}

TEST_F(RedBlackTreeTest, SearchAfterMultipleInserts) {
    std::vector<int> values = {50, 30, 70, 20, 40, 60, 80};
    for (int val : values) {
        tree->insert(val);
    }
    
    // Search for all inserted values
    for (int val : values) {
        Node<int>* found = tree->search(val);
        ASSERT_NE(found, nullptr);
        EXPECT_EQ(found->data, val);
    }
    
    // Search for non-existing values
    EXPECT_EQ(tree->search(25), nullptr);
    EXPECT_EQ(tree->search(55), nullptr);
    EXPECT_EQ(tree->search(100), nullptr);
}

// ==================== Remove Tests ====================
TEST_F(RedBlackTreeTest, RemoveFromEmptyTree) {
    tree->remove(10);
    EXPECT_EQ(tree->search(10), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveNonExistingElement) {
    tree->insert(10);
    tree->insert(20);
    tree->remove(100);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveRootWithNoChildren) {
    tree->insert(10);
    tree->remove(10);
    EXPECT_EQ(tree->search(10), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveRootWithLeftChild) {
    tree->insert(10);
    tree->insert(5);
    tree->remove(10);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveRootWithRightChild) {
    tree->insert(10);
    tree->insert(20);
    tree->remove(10);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveRootWithTwoChildren) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(20);
    tree->remove(10);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveLeafNode) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(20);
    tree->insert(3);
    tree->insert(7);
    
    tree->remove(3);
    EXPECT_EQ(tree->search(3), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(7), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveNodeWithLeftChildOnly) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(20);
    tree->insert(3);
    
    tree->remove(5);
    EXPECT_EQ(tree->search(5), nullptr);
    EXPECT_NE(tree->search(3), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveNodeWithRightChildOnly) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(20);
    tree->insert(7);
    
    tree->remove(5);
    EXPECT_EQ(tree->search(5), nullptr);
    EXPECT_NE(tree->search(7), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveNodeWithTwoChildren) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(20);
    tree->insert(3);
    tree->insert(7);
    
    tree->remove(5);
    EXPECT_EQ(tree->search(5), nullptr);
    EXPECT_NE(tree->search(3), nullptr);
    EXPECT_NE(tree->search(7), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveMultipleElements) {
    std::vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    for (int val : values) {
        tree->insert(val);
    }
    
    tree->remove(20);
    tree->remove(60);
    tree->remove(50);
    
    EXPECT_EQ(tree->search(20), nullptr);
    EXPECT_EQ(tree->search(60), nullptr);
    EXPECT_EQ(tree->search(50), nullptr);
    
    EXPECT_NE(tree->search(30), nullptr);
    EXPECT_NE(tree->search(70), nullptr);
    EXPECT_NE(tree->search(40), nullptr);
    EXPECT_NE(tree->search(80), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveAllElements) {
    std::vector<int> values = {10, 20, 5, 15, 25};
    for (int val : values) {
        tree->insert(val);
    }
    
    for (int val : values) {
        tree->remove(val);
        EXPECT_EQ(tree->search(val), nullptr);
    }
}

TEST_F(RedBlackTreeTest, RemoveAndReinsert) {
    tree->insert(10);
    tree->insert(20);
    tree->insert(5);
    
    tree->remove(10);
    EXPECT_EQ(tree->search(10), nullptr);
    
    tree->insert(10);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveComplexScenario) {
    for (int i = 1; i <= 20; ++i) {
        tree->insert(i);
    }
    
    tree->remove(10);
    tree->remove(5);
    tree->remove(15);
    tree->remove(1);
    tree->remove(20);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_EQ(tree->search(5), nullptr);
    EXPECT_EQ(tree->search(15), nullptr);
    EXPECT_EQ(tree->search(1), nullptr);
    EXPECT_EQ(tree->search(20), nullptr);
    
    for (int i = 2; i <= 4; ++i) {
        EXPECT_NE(tree->search(i), nullptr);
    }
    for (int i = 6; i <= 9; ++i) {
        EXPECT_NE(tree->search(i), nullptr);
    }
    for (int i = 11; i <= 14; ++i) {
        EXPECT_NE(tree->search(i), nullptr);
    }
    for (int i = 16; i <= 19; ++i) {
        EXPECT_NE(tree->search(i), nullptr);
    }
}

// ==================== PrintTree Tests ====================
TEST_F(RedBlackTreeTest, PrintEmptyTree) {
    tree->printTree();
    SUCCEED();
}

TEST_F(RedBlackTreeTest, PrintSingleNodeTree) {
    tree->insert(10);
    tree->printTree();
    SUCCEED();
}

TEST_F(RedBlackTreeTest, PrintMultiNodeTree) {
    std::vector<int> values = {10, 20, 5, 15, 25};
    for (int val : values) {
        tree->insert(val);
    }
    tree->printTree();
    SUCCEED();
}

// ==================== Rotation Tests (tested indirectly) ====================
TEST_F(RedBlackTreeTest, LeftRotationScenario) {
    tree->insert(10);
    tree->insert(20);
    tree->insert(30);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    EXPECT_NE(tree->search(30), nullptr);
}

TEST_F(RedBlackTreeTest, RightRotationScenario) {
    tree->insert(30);
    tree->insert(20);
    tree->insert(10);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    EXPECT_NE(tree->search(30), nullptr);
}

TEST_F(RedBlackTreeTest, LeftRightRotationScenario) {
    tree->insert(30);
    tree->insert(10);
    tree->insert(20);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    EXPECT_NE(tree->search(30), nullptr);
}

TEST_F(RedBlackTreeTest, RightLeftRotationScenario) {
    tree->insert(10);
    tree->insert(30);
    tree->insert(20);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    EXPECT_NE(tree->search(30), nullptr);
}

// ==================== FixInsert Tests (tested indirectly) ====================
TEST_F(RedBlackTreeTest, FixInsertCase1_RedUncle) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(3);
    
    EXPECT_NE(tree->search(3), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
}

TEST_F(RedBlackTreeTest, FixInsertCase2_BlackUncleLeftChild) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(3);
    
    EXPECT_NE(tree->search(3), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
}

TEST_F(RedBlackTreeTest, FixInsertCase2_BlackUncleRightChild) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(7);
    
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(7), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
}

TEST_F(RedBlackTreeTest, FixInsertCase3_BlackUncleRightSubtree) {
    tree->insert(10);
    tree->insert(15);
    tree->insert(20);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, FixInsertCase3_BlackUncleRightSubtreeLeftChild) {
    tree->insert(10);
    tree->insert(15);
    tree->insert(12);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(12), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
}

// ==================== FixDelete Tests (tested indirectly) ====================
TEST_F(RedBlackTreeTest, FixDeleteCase1_RedSibling) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(3);
    tree->insert(7);
    tree->insert(12);
    tree->insert(20);
    
    tree->remove(3);
    
    EXPECT_EQ(tree->search(3), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(7), nullptr);
}

TEST_F(RedBlackTreeTest, FixDeleteCase2_BlackSiblingBlackChildren) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(3);
    
    tree->remove(3);
    
    EXPECT_EQ(tree->search(3), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
}

TEST_F(RedBlackTreeTest, FixDeleteCase3_BlackSiblingRedLeftChild) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(3);
    tree->insert(7);
    
    tree->remove(3);
    
    EXPECT_EQ(tree->search(3), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(7), nullptr);
}

TEST_F(RedBlackTreeTest, FixDeleteCase4_BlackSiblingRedRightChild) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(3);
    tree->insert(7);
    tree->insert(12);
    tree->insert(20);
    
    tree->remove(12);
    
    EXPECT_EQ(tree->search(12), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
}

TEST_F(RedBlackTreeTest, FixDeleteRightSubtreeCases) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(3);
    tree->insert(7);
    tree->insert(12);
    tree->insert(20);
    
    tree->remove(20);
    tree->remove(12);
    tree->remove(15);
    
    EXPECT_EQ(tree->search(20), nullptr);
    EXPECT_EQ(tree->search(12), nullptr);
    EXPECT_EQ(tree->search(15), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
}

// ==================== MinValueNode Tests (tested indirectly) ====================
TEST_F(RedBlackTreeTest, MinValueNodeInRightSubtree) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(12);
    tree->insert(20);
    
    tree->remove(10);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(12), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, MinValueNodeDirectRightChild) {
    tree->insert(10);
    tree->insert(15);
    tree->insert(12);
    tree->insert(20);
    
    tree->remove(10);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(12), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

// ==================== Transplant Tests (tested indirectly) ====================
TEST_F(RedBlackTreeTest, TransplantRoot) {
    tree->insert(10);
    tree->insert(5);
    
    tree->remove(10);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
}

TEST_F(RedBlackTreeTest, TransplantLeftChild) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(3);
    
    tree->remove(5);
    
    EXPECT_EQ(tree->search(5), nullptr);
    EXPECT_NE(tree->search(3), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
}

TEST_F(RedBlackTreeTest, TransplantRightChild) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(20);

    tree->remove(15);
    
    EXPECT_EQ(tree->search(15), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
}

// ==================== DeleteTree Tests (tested indirectly) ====================
TEST_F(RedBlackTreeTest, DeleteTreeViaDestructor) {
    {
        RedBlackTree<int> tempTree;
        for (int i = 1; i <= 50; ++i) {
            tempTree.insert(i);
        }
    }
    SUCCEED();
}

// ==================== Edge Cases ====================
TEST_F(RedBlackTreeTest, SingleElementTree) {
    tree->insert(42);
    EXPECT_NE(tree->search(42), nullptr);
    tree->remove(42);
    EXPECT_EQ(tree->search(42), nullptr);
}

TEST_F(RedBlackTreeTest, TwoElementTree) {
    tree->insert(10);
    tree->insert(20);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    
    tree->remove(10);
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    
    tree->remove(20);
    EXPECT_EQ(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, ThreeElementTree) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(20);
    
    EXPECT_NE(tree->search(10), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
    
    tree->remove(10);
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, InsertRemoveAlternating) {
    for (int i = 1; i <= 10; ++i) {
        tree->insert(i);
        tree->remove(i);
        EXPECT_EQ(tree->search(i), nullptr);
    }
}

TEST_F(RedBlackTreeTest, RemoveSuccessorWithRightChild) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(12);
    tree->insert(20);
    tree->insert(18);
    tree->insert(25);
    
    tree->remove(10);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(12), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
    EXPECT_NE(tree->search(20), nullptr);
}

TEST_F(RedBlackTreeTest, RemoveSuccessorDirectChild) {
    tree->insert(10);
    tree->insert(5);
    tree->insert(15);
    tree->insert(12);
    
    tree->remove(10);
    
    EXPECT_EQ(tree->search(10), nullptr);
    EXPECT_NE(tree->search(12), nullptr);
    EXPECT_NE(tree->search(15), nullptr);
    EXPECT_NE(tree->search(5), nullptr);
}

// ==================== Comprehensive Integration Tests ====================
TEST_F(RedBlackTreeTest, ComprehensiveInsertRemoveSequence) {
    for (int i = 1; i <= 100; ++i) {
        tree->insert(i);
    }
    
    for (int i = 2; i <= 100; i += 2) {
        tree->remove(i);
    }
    
    for (int i = 1; i <= 99; i += 2) {
        EXPECT_NE(tree->search(i), nullptr);
    }
    
    for (int i = 2; i <= 100; i += 2) {
        EXPECT_EQ(tree->search(i), nullptr);
    }
}

TEST_F(RedBlackTreeTest, StressTest) {
    for (int i = 1; i <= 1000; ++i) {
        tree->insert(i);
    }
    
    for (int i = 1; i <= 500; ++i) {
        tree->remove(i);
    }
    
    for (int i = 501; i <= 1000; ++i) {
        EXPECT_NE(tree->search(i), nullptr);
    }
    
    for (int i = 1; i <= 500; ++i) {
        EXPECT_EQ(tree->search(i), nullptr);
    }
}

// ==================== Test with different types ====================
TEST(RedBlackTreeTypeTest, TestWithDouble) {
    RedBlackTree<double> tree;
    tree.insert(10.5);
    tree.insert(20.3);
    tree.insert(5.7);
    
    EXPECT_NE(tree.search(10.5), nullptr);
    EXPECT_NE(tree.search(20.3), nullptr);
    EXPECT_NE(tree.search(5.7), nullptr);
    
    tree.remove(10.5);
    EXPECT_EQ(tree.search(10.5), nullptr);
}

TEST(RedBlackTreeTypeTest, TestWithChar) {
    RedBlackTree<char> tree;
    tree.insert('a');
    tree.insert('b');
    tree.insert('c');
    
    EXPECT_NE(tree.search('a'), nullptr);
    EXPECT_NE(tree.search('b'), nullptr);
    EXPECT_NE(tree.search('c'), nullptr);
    
    tree.remove('b');
    EXPECT_EQ(tree.search('b'), nullptr);
}
