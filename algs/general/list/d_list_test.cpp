#include <gtest/gtest.h>
#include "d_list.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

class DoublyLinkedListTest : public ::testing::Test {
protected:
    void SetUp() override {
        list = new DoublyLinkedList<int>();
    }
    
    void TearDown() override {
        delete list;
    }
    
    DoublyLinkedList<int>* list;
};

// ==================== Constructor Tests ====================
TEST_F(DoublyLinkedListTest, DefaultConstructorCreatesEmptyList) {
    DoublyLinkedList<int> newList;
    EXPECT_TRUE(newList.empty());
    EXPECT_EQ(newList.size(), 0);
}

TEST_F(DoublyLinkedListTest, DefaultConstructorInitializesCorrectly) {
    DoublyLinkedList<int> newList;
    EXPECT_EQ(newList.begin(), newList.end());
}

// ==================== Destructor Tests ====================
TEST_F(DoublyLinkedListTest, DestructorCleansUpProperly) {
    {
        DoublyLinkedList<int> tempList;
        tempList.push_back(1);
        tempList.push_back(2);
        tempList.push_back(3);
    }
    SUCCEED();
}

TEST_F(DoublyLinkedListTest, DestructorHandlesEmptyList) {
    {
        DoublyLinkedList<int> tempList;
    }
    SUCCEED();
}

// ==================== Copy Constructor Tests ====================
TEST_F(DoublyLinkedListTest, CopyConstructorEmptyList) {
    DoublyLinkedList<int> original;
    DoublyLinkedList<int> copy(original);
    
    EXPECT_TRUE(copy.empty());
    EXPECT_EQ(copy.size(), 0);
    EXPECT_TRUE(original.empty());
}

TEST_F(DoublyLinkedListTest, CopyConstructorSingleElement) {
    DoublyLinkedList<int> original;
    original.push_back(42);
    
    DoublyLinkedList<int> copy(original);
    
    EXPECT_EQ(copy.size(), 1);
    EXPECT_EQ(copy.front(), 42);
    EXPECT_EQ(copy.back(), 42);
    EXPECT_EQ(original.size(), 1);
}

TEST_F(DoublyLinkedListTest, CopyConstructorMultipleElements) {
    DoublyLinkedList<int> original;
    original.push_back(1);
    original.push_back(2);
    original.push_back(3);
    
    DoublyLinkedList<int> copy(original);
    
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy.front(), 1);
    EXPECT_EQ(copy.back(), 3);
    
    std::vector<int> originalValues, copyValues;
    for (auto it = original.begin(); it != original.end(); ++it) {
        originalValues.push_back(*it);
    }
    for (auto it = copy.begin(); it != copy.end(); ++it) {
        copyValues.push_back(*it);
    }
    
    EXPECT_EQ(originalValues, copyValues);
    EXPECT_EQ(original.size(), 3);
}

TEST_F(DoublyLinkedListTest, CopyConstructorIndependence) {
    DoublyLinkedList<int> original;
    original.push_back(1);
    original.push_back(2);
    
    DoublyLinkedList<int> copy(original);
    copy.push_back(3);
    
    EXPECT_EQ(original.size(), 2);
    EXPECT_EQ(copy.size(), 3);
}

// ==================== Assignment Operator Tests ====================
TEST_F(DoublyLinkedListTest, AssignmentOperatorEmptyToEmpty) {
    DoublyLinkedList<int> list1;
    DoublyLinkedList<int> list2;
    
    list2 = list1;
    
    EXPECT_TRUE(list2.empty());
    EXPECT_EQ(list2.size(), 0);
}

TEST_F(DoublyLinkedListTest, AssignmentOperatorEmptyToNonEmpty) {
    DoublyLinkedList<int> list1;
    DoublyLinkedList<int> list2;
    list2.push_back(1);
    list2.push_back(2);
    
    list2 = list1;
    
    EXPECT_TRUE(list2.empty());
    EXPECT_EQ(list2.size(), 0);
}

TEST_F(DoublyLinkedListTest, AssignmentOperatorNonEmptyToEmpty) {
    DoublyLinkedList<int> list1;
    list1.push_back(1);
    list1.push_back(2);
    DoublyLinkedList<int> list2;
    
    list2 = list1;
    
    EXPECT_EQ(list2.size(), 2);
    EXPECT_EQ(list2.front(), 1);
    EXPECT_EQ(list2.back(), 2);
}

TEST_F(DoublyLinkedListTest, AssignmentOperatorNonEmptyToNonEmpty) {
    DoublyLinkedList<int> list1;
    list1.push_back(1);
    list1.push_back(2);
    DoublyLinkedList<int> list2;
    list2.push_back(10);
    list2.push_back(20);
    list2.push_back(30);
    
    list2 = list1;
    
    EXPECT_EQ(list2.size(), 2);
    EXPECT_EQ(list2.front(), 1);
    EXPECT_EQ(list2.back(), 2);
}

TEST_F(DoublyLinkedListTest, AssignmentOperatorSelfAssignment) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    
    *list = *list;
    
    EXPECT_EQ(list->size(), 3);
    EXPECT_EQ(list->front(), 1);
    EXPECT_EQ(list->back(), 3);
}

TEST_F(DoublyLinkedListTest, AssignmentOperatorIndependence) {
    DoublyLinkedList<int> list1;
    list1.push_back(1);
    list1.push_back(2);
    DoublyLinkedList<int> list2;
    list2.push_back(10);
    
    list2 = list1;
    list2.push_back(3);
    
    EXPECT_EQ(list1.size(), 2);
    EXPECT_EQ(list2.size(), 3);
}

// ==================== Push Front Tests ====================
TEST_F(DoublyLinkedListTest, PushFrontToEmptyList) {
    list->push_front(10);
    
    EXPECT_FALSE(list->empty());
    EXPECT_EQ(list->size(), 1);
    EXPECT_EQ(list->front(), 10);
    EXPECT_EQ(list->back(), 10);
}

TEST_F(DoublyLinkedListTest, PushFrontMultipleElements) {
    list->push_front(3);
    list->push_front(2);
    list->push_front(1);
    
    EXPECT_EQ(list->size(), 3);
    EXPECT_EQ(list->front(), 1);
    EXPECT_EQ(list->back(), 3);
}

TEST_F(DoublyLinkedListTest, PushFrontMaintainsOrder) {
    for (int i = 5; i >= 1; --i) {
        list->push_front(i);
    }
    
    EXPECT_EQ(list->size(), 5);
    int expected = 1;
    for (auto it = list->begin(); it != list->end(); ++it) {
        EXPECT_EQ(*it, expected++);
    }
}

// ==================== Push Back Tests ====================
TEST_F(DoublyLinkedListTest, PushBackToEmptyList) {
    list->push_back(10);
    
    EXPECT_FALSE(list->empty());
    EXPECT_EQ(list->size(), 1);
    EXPECT_EQ(list->front(), 10);
    EXPECT_EQ(list->back(), 10);
}

TEST_F(DoublyLinkedListTest, PushBackMultipleElements) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    
    EXPECT_EQ(list->size(), 3);
    EXPECT_EQ(list->front(), 1);
    EXPECT_EQ(list->back(), 3);
}

TEST_F(DoublyLinkedListTest, PushBackMaintainsOrder) {
    for (int i = 1; i <= 5; ++i) {
        list->push_back(i);
    }
    
    EXPECT_EQ(list->size(), 5);
    int expected = 1;
    for (auto it = list->begin(); it != list->end(); ++it) {
        EXPECT_EQ(*it, expected++);
    }
}

TEST_F(DoublyLinkedListTest, PushBackAfterPushFront) {
    list->push_front(1);
    list->push_back(2);
    list->push_back(3);
    
    EXPECT_EQ(list->size(), 3);
    EXPECT_EQ(list->front(), 1);
    EXPECT_EQ(list->back(), 3);
}

// ==================== Insert After Tests ====================
TEST_F(DoublyLinkedListTest, InsertAfterAtEnd) {
    list->push_back(1);
    list->push_back(2);
    
    auto it = list->end();
    auto result = list->insert_after(it, 3);
    
    EXPECT_EQ(list->size(), 3);
    EXPECT_EQ(list->back(), 3);
    EXPECT_EQ(*result, 3);
}

TEST_F(DoublyLinkedListTest, InsertAfterAtBeginning) {
    list->push_back(1);
    list->push_back(3);
    
    auto it = list->begin();
    auto result = list->insert_after(it, 2);
    
    EXPECT_EQ(list->size(), 3);
    EXPECT_EQ(*result, 2);
    
    std::vector<int> values;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        values.push_back(*iter);
    }
    EXPECT_EQ(values, std::vector<int>({1, 2, 3}));
}

TEST_F(DoublyLinkedListTest, InsertAfterInMiddle) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(4);
    
    auto it = list->begin();
    ++it;
    auto result = list->insert_after(it, 3);
    
    EXPECT_EQ(list->size(), 4);
    EXPECT_EQ(*result, 3);
    
    std::vector<int> values;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        values.push_back(*iter);
    }
    EXPECT_EQ(values, std::vector<int>({1, 2, 3, 4}));
}

TEST_F(DoublyLinkedListTest, InsertAfterAtTail) {
    list->push_back(1);
    list->push_back(2);
    
    auto it = list->begin();
    ++it;
    auto result = list->insert_after(it, 3);
    
    EXPECT_EQ(list->size(), 3);
    EXPECT_EQ(list->back(), 3);
    EXPECT_EQ(*result, 3);
}

TEST_F(DoublyLinkedListTest, InsertAfterEmptyList) {
    auto it = list->end();
    auto result = list->insert_after(it, 1);
    
    EXPECT_EQ(list->size(), 1);
    EXPECT_EQ(list->front(), 1);
    EXPECT_EQ(*result, 1);
}

TEST_F(DoublyLinkedListTest, InsertAfterMultipleTimes) {
    list->push_back(1);
    list->push_back(5);
    
    auto it = list->begin();
    list->insert_after(it, 2);
    ++it;
    list->insert_after(it, 3);
    ++it;
    list->insert_after(it, 4);
    
    EXPECT_EQ(list->size(), 5);
    std::vector<int> values;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        values.push_back(*iter);
    }
    EXPECT_EQ(values, std::vector<int>({1, 2, 3, 4, 5}));
}

// ==================== Erase Tests ====================
TEST_F(DoublyLinkedListTest, EraseFromEmptyList) {
    auto it = list->end();
    list->erase(it);
    
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(list->size(), 0);
}

TEST_F(DoublyLinkedListTest, EraseSingleElement) {
    list->push_back(42);
    
    auto it = list->begin();
    list->erase(it);
    
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(list->size(), 0);
}

TEST_F(DoublyLinkedListTest, EraseFirstElement) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    
    auto it = list->begin();
    list->erase(it);
    
    EXPECT_EQ(list->size(), 2);
    EXPECT_EQ(list->front(), 2);
    EXPECT_EQ(list->back(), 3);
}

TEST_F(DoublyLinkedListTest, EraseLastElement) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    
    auto it = list->begin();
    ++it;
    ++it;
    list->erase(it);
    
    EXPECT_EQ(list->size(), 2);
    EXPECT_EQ(list->front(), 1);
    EXPECT_EQ(list->back(), 2);
}

TEST_F(DoublyLinkedListTest, EraseMiddleElement) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    
    auto it = list->begin();
    ++it;
    list->erase(it);
    
    EXPECT_EQ(list->size(), 2);
    EXPECT_EQ(list->front(), 1);
    EXPECT_EQ(list->back(), 3);
}

TEST_F(DoublyLinkedListTest, EraseAllElements) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    
    while (!list->empty()) {
        list->erase(list->begin());
    }
    
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(list->size(), 0);
}

TEST_F(DoublyLinkedListTest, EraseMultipleElements) {
    for (int i = 1; i <= 5; ++i) {
        list->push_back(i);
    }

    auto it = list->begin();
    ++it;
    list->erase(it);
    
    it = list->begin();
    ++it;
    ++it;
    list->erase(it);
    
    EXPECT_EQ(list->size(), 3);
    std::vector<int> values;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        values.push_back(*iter);
    }
    EXPECT_EQ(values, std::vector<int>({1, 3, 5}));
}

// ==================== Front Tests ====================
TEST_F(DoublyLinkedListTest, FrontOnEmptyListThrows) {
    EXPECT_THROW(list->front(), std::runtime_error);
}

TEST_F(DoublyLinkedListTest, FrontReturnsFirstElement) {
    list->push_back(10);
    list->push_back(20);
    
    EXPECT_EQ(list->front(), 10);
}

TEST_F(DoublyLinkedListTest, FrontIsMutable) {
    list->push_back(10);
    list->front() = 100;
    
    EXPECT_EQ(list->front(), 100);
}

TEST_F(DoublyLinkedListTest, FrontConstVersion) {
    list->push_back(10);
    list->push_back(20);
    
    const DoublyLinkedList<int>* constList = list;
    EXPECT_EQ(constList->front(), 10);
}

TEST_F(DoublyLinkedListTest, FrontConstVersionThrowsOnEmpty) {
    const DoublyLinkedList<int> constList;
    EXPECT_THROW(constList.front(), std::runtime_error);
}

// ==================== Back Tests ====================
TEST_F(DoublyLinkedListTest, BackOnEmptyListThrows) {
    EXPECT_THROW(list->back(), std::runtime_error);
}

TEST_F(DoublyLinkedListTest, BackReturnsLastElement) {
    list->push_back(10);
    list->push_back(20);
    
    EXPECT_EQ(list->back(), 20);
}

TEST_F(DoublyLinkedListTest, BackIsMutable) {
    list->push_back(10);
    list->back() = 100;
    
    EXPECT_EQ(list->back(), 100);
}

TEST_F(DoublyLinkedListTest, BackConstVersion) {
    list->push_back(10);
    list->push_back(20);
    
    const DoublyLinkedList<int>* constList = list;
    EXPECT_EQ(constList->back(), 20);
}

TEST_F(DoublyLinkedListTest, BackConstVersionThrowsOnEmpty) {
    const DoublyLinkedList<int> constList;
    EXPECT_THROW(constList.back(), std::runtime_error);
}

// ==================== Empty Tests ====================
TEST_F(DoublyLinkedListTest, EmptyOnNewList) {
    EXPECT_TRUE(list->empty());
}

TEST_F(DoublyLinkedListTest, EmptyAfterPush) {
    list->push_back(1);
    EXPECT_FALSE(list->empty());
}

TEST_F(DoublyLinkedListTest, EmptyAfterClear) {
    list->push_back(1);
    list->push_back(2);
    list->clear();
    EXPECT_TRUE(list->empty());
}

TEST_F(DoublyLinkedListTest, EmptyAfterEraseAll) {
    list->push_back(1);
    list->erase(list->begin());
    EXPECT_TRUE(list->empty());
}

// ==================== Size Tests ====================
TEST_F(DoublyLinkedListTest, SizeOnEmptyList) {
    EXPECT_EQ(list->size(), 0);
}

TEST_F(DoublyLinkedListTest, SizeAfterPushFront) {
    list->push_front(1);
    EXPECT_EQ(list->size(), 1);
    list->push_front(2);
    EXPECT_EQ(list->size(), 2);
}

TEST_F(DoublyLinkedListTest, SizeAfterPushBack) {
    list->push_back(1);
    EXPECT_EQ(list->size(), 1);
    list->push_back(2);
    EXPECT_EQ(list->size(), 2);
}

TEST_F(DoublyLinkedListTest, SizeAfterInsertAfter) {
    list->push_back(1);
    EXPECT_EQ(list->size(), 1);
    list->insert_after(list->begin(), 2);
    EXPECT_EQ(list->size(), 2);
}

TEST_F(DoublyLinkedListTest, SizeAfterErase) {
    list->push_back(1);
    list->push_back(2);
    EXPECT_EQ(list->size(), 2);
    list->erase(list->begin());
    EXPECT_EQ(list->size(), 1);
}

TEST_F(DoublyLinkedListTest, SizeAfterClear) {
    for (int i = 0; i < 10; ++i) {
        list->push_back(i);
    }
    EXPECT_EQ(list->size(), 10);
    list->clear();
    EXPECT_EQ(list->size(), 0);
}

// ==================== Clear Tests ====================
TEST_F(DoublyLinkedListTest, ClearEmptyList) {
    list->clear();
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(list->size(), 0);
}

TEST_F(DoublyLinkedListTest, ClearSingleElement) {
    list->push_back(1);
    list->clear();
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(list->size(), 0);
}

TEST_F(DoublyLinkedListTest, ClearMultipleElements) {
    for (int i = 0; i < 10; ++i) {
        list->push_back(i);
    }
    list->clear();
    EXPECT_TRUE(list->empty());
    EXPECT_EQ(list->size(), 0);
}

TEST_F(DoublyLinkedListTest, ClearAndReuse) {
    list->push_back(1);
    list->push_back(2);
    list->clear();
    
    list->push_back(10);
    EXPECT_EQ(list->size(), 1);
    EXPECT_EQ(list->front(), 10);
}

// ==================== Begin/End Tests ====================
TEST_F(DoublyLinkedListTest, BeginEqualsEndOnEmptyList) {
    EXPECT_EQ(list->begin(), list->end());
}

TEST_F(DoublyLinkedListTest, BeginPointsToFirstElement) {
    list->push_back(10);
    EXPECT_EQ(*list->begin(), 10);
}

TEST_F(DoublyLinkedListTest, EndIsNullIterator) {
    list->push_back(10);
    auto it = list->end();
    EXPECT_EQ(it, DoublyLinkedList<int>::Iterator(nullptr));
}

TEST_F(DoublyLinkedListTest, BeginToEndIteration) {
    for (int i = 1; i <= 5; ++i) {
        list->push_back(i);
    }
    
    int count = 0;
    int sum = 0;
    for (auto it = list->begin(); it != list->end(); ++it) {
        count++;
        sum += *it;
    }
    
    EXPECT_EQ(count, 5);
    EXPECT_EQ(sum, 15);
}

// ==================== Iterator Tests ====================
TEST_F(DoublyLinkedListTest, IteratorDereference) {
    list->push_back(42);
    auto it = list->begin();
    EXPECT_EQ(*it, 42);
}

TEST_F(DoublyLinkedListTest, IteratorArrowOperator) {
    struct TestStruct {
        int value;
        TestStruct(int v) : value(v) {}
    };
    
    DoublyLinkedList<TestStruct> structList;
    structList.push_back(TestStruct(42));
    auto it = structList.begin();
    EXPECT_EQ(it->value, 42);
}

TEST_F(DoublyLinkedListTest, IteratorPrefixIncrement) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    
    auto it = list->begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
}

TEST_F(DoublyLinkedListTest, IteratorPostfixIncrement) {
    list->push_back(1);
    list->push_back(2);
    
    auto it = list->begin();
    auto oldIt = it++;
    EXPECT_EQ(*oldIt, 1);
    EXPECT_EQ(*it, 2);
}

TEST_F(DoublyLinkedListTest, IteratorPrefixDecrement) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    
    auto it = list->begin();
    ++it;
    ++it; // Points to 3
    EXPECT_EQ(*it, 3);
    --it;
    EXPECT_EQ(*it, 2);
    --it;
    EXPECT_EQ(*it, 1);
}

TEST_F(DoublyLinkedListTest, IteratorPostfixDecrement) {
    list->push_back(1);
    list->push_back(2);
    
    auto it = list->begin();
    ++it; // Points to 2
    auto oldIt = it--;
    EXPECT_EQ(*oldIt, 2);
    EXPECT_EQ(*it, 1);
}

TEST_F(DoublyLinkedListTest, IteratorEquality) {
    list->push_back(1);
    auto it1 = list->begin();
    auto it2 = list->begin();
    EXPECT_EQ(it1, it2);
}

TEST_F(DoublyLinkedListTest, IteratorInequality) {
    list->push_back(1);
    list->push_back(2);
    auto it1 = list->begin();
    auto it2 = list->begin();
    ++it2;
    EXPECT_NE(it1, it2);
}

TEST_F(DoublyLinkedListTest, IteratorEndEquality) {
    list->push_back(1);
    auto it1 = list->end();
    auto it2 = list->end();
    EXPECT_EQ(it1, it2);
}

TEST_F(DoublyLinkedListTest, IteratorIncrementBeyondEnd) {
    list->push_back(1);
    auto it = list->begin();
    ++it;
    ++it;
    EXPECT_EQ(it, list->end());
}

TEST_F(DoublyLinkedListTest, IteratorDecrementFromBegin) {
    list->push_back(1);
    auto it = list->begin();
    --it;
    SUCCEED();
}

TEST_F(DoublyLinkedListTest, IteratorConstDereference) {
    list->push_back(42);
    const auto it = list->begin();
    EXPECT_EQ(*it, 42);
}

TEST_F(DoublyLinkedListTest, IteratorConstArrowOperator) {
    struct TestStruct {
        int value;
        TestStruct(int v) : value(v) {}
    };
    
    DoublyLinkedList<TestStruct> structList;
    structList.push_back(TestStruct(42));
    const auto it = structList.begin();
    EXPECT_EQ(it->value, 42);
}

// ==================== Unlink Node Tests (indirect) ====================
TEST_F(DoublyLinkedListTest, UnlinkNodeHead) {
    list->push_back(1);
    list->push_back(2);
    list->erase(list->begin());
    
    EXPECT_EQ(list->size(), 1);
    EXPECT_EQ(list->front(), 2);
}

TEST_F(DoublyLinkedListTest, UnlinkNodeTail) {
    list->push_back(1);
    list->push_back(2);
    auto it = list->begin();
    ++it;
    list->erase(it);
    
    EXPECT_EQ(list->size(), 1);
    EXPECT_EQ(list->front(), 1);
}

TEST_F(DoublyLinkedListTest, UnlinkNodeMiddle) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(3);
    auto it = list->begin();
    ++it;
    list->erase(it);
    
    EXPECT_EQ(list->size(), 2);
    EXPECT_EQ(list->front(), 1);
    EXPECT_EQ(list->back(), 3);
}

TEST_F(DoublyLinkedListTest, UnlinkNodeSingleElement) {
    list->push_back(1);
    list->erase(list->begin());
    
    EXPECT_TRUE(list->empty());
}

// ==================== Link Node After Tests (indirect) ====================
TEST_F(DoublyLinkedListTest, LinkNodeAfterEmptyList) {
    list->push_front(1);
    
    EXPECT_EQ(list->size(), 1);
    EXPECT_EQ(list->front(), 1);
}

TEST_F(DoublyLinkedListTest, LinkNodeAfterHead) {
    list->push_back(1);
    list->push_back(3);
    list->insert_after(list->begin(), 2);
    
    EXPECT_EQ(list->size(), 3);
    std::vector<int> values;
    for (auto it = list->begin(); it != list->end(); ++it) {
        values.push_back(*it);
    }
    EXPECT_EQ(values, std::vector<int>({1, 2, 3}));
}

TEST_F(DoublyLinkedListTest, LinkNodeAfterTail) {
    list->push_back(1);
    list->push_back(2);
    auto it = list->begin();
    ++it;
    list->insert_after(it, 3);
    
    EXPECT_EQ(list->size(), 3);
    EXPECT_EQ(list->back(), 3);
}

TEST_F(DoublyLinkedListTest, LinkNodeAfterMiddle) {
    list->push_back(1);
    list->push_back(2);
    list->push_back(4);
    auto it = list->begin();
    ++it;
    list->insert_after(it, 3);
    
    EXPECT_EQ(list->size(), 4);
    std::vector<int> values;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        values.push_back(*iter);
    }
    EXPECT_EQ(values, std::vector<int>({1, 2, 3, 4}));
}

// ==================== Complex Scenarios ====================
TEST_F(DoublyLinkedListTest, PushFrontPushBackMix) {
    list->push_front(2);
    list->push_back(3);
    list->push_front(1);
    list->push_back(4);
    
    EXPECT_EQ(list->size(), 4);
    std::vector<int> values;
    for (auto it = list->begin(); it != list->end(); ++it) {
        values.push_back(*it);
    }
    EXPECT_EQ(values, std::vector<int>({1, 2, 3, 4}));
}

TEST_F(DoublyLinkedListTest, InsertEraseSequence) {
    list->push_back(1);
    list->push_back(4);
    
    auto it = list->begin();
    list->insert_after(it, 2);
    ++it;
    list->insert_after(it, 3);
    
    it = list->begin();
    ++it;
    list->erase(it);
    
    EXPECT_EQ(list->size(), 3);
    std::vector<int> values;
    for (auto iter = list->begin(); iter != list->end(); ++iter) {
        values.push_back(*iter);
    }
    EXPECT_EQ(values, std::vector<int>({1, 3, 4}));
}

TEST_F(DoublyLinkedListTest, ReverseIteration) {
    for (int i = 1; i <= 5; ++i) {
        list->push_back(i);
    }
    
    std::vector<int> reverseValues;
    auto it = list->begin();
    for (int i = 0; i < 4; ++i) {
        ++it;
    }
    for (int i = 0; i < 5; ++i) {
        reverseValues.push_back(*it);
        --it;
    }
    
    std::reverse(reverseValues.begin(), reverseValues.end());
    EXPECT_EQ(reverseValues, std::vector<int>({1, 2, 3, 4, 5}));
}

TEST_F(DoublyLinkedListTest, LargeSequence) {
    for (int i = 0; i < 1000; ++i) {
        list->push_back(i);
    }
    
    EXPECT_EQ(list->size(), 1000);
    EXPECT_EQ(list->front(), 0);
    EXPECT_EQ(list->back(), 999);
}

TEST_F(DoublyLinkedListTest, AlternatingOperations) {
    for (int i = 0; i < 10; ++i) {
        if (i % 2 == 0) {
            list->push_back(i);
        } else {
            list->push_front(i);
        }
    }
    
    EXPECT_EQ(list->size(), 10);
}

// ==================== Type Tests ====================
TEST(DoublyLinkedListTypeTest, TestWithDouble) {
    DoublyLinkedList<double> list;
    list.push_back(3.14);
    list.push_back(2.71);
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_DOUBLE_EQ(list.front(), 3.14);
    EXPECT_DOUBLE_EQ(list.back(), 2.71);
}

TEST(DoublyLinkedListTypeTest, TestWithString) {
    DoublyLinkedList<std::string> list;
    list.push_back("hello");
    list.push_back("world");
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), "hello");
    EXPECT_EQ(list.back(), "world");
}

TEST(DoublyLinkedListTypeTest, TestWithChar) {
    DoublyLinkedList<char> list;
    list.push_back('a');
    list.push_back('b');
    list.push_back('c');
    
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(list.front(), 'a');
    EXPECT_EQ(list.back(), 'c');
}

// ==================== Edge Cases ====================
TEST_F(DoublyLinkedListTest, EraseEndIterator) {
    list->push_back(1);
    list->erase(list->end());
    
    EXPECT_EQ(list->size(), 1);
}

TEST_F(DoublyLinkedListTest, InsertAfterEndOnEmptyList) {
    auto it = list->end();
    list->insert_after(it, 1);
    
    EXPECT_EQ(list->size(), 1);
    EXPECT_EQ(list->front(), 1);
}

TEST_F(DoublyLinkedListTest, MultipleClearOperations) {
    list->push_back(1);
    list->clear();
    list->clear();
    
    EXPECT_TRUE(list->empty());
}

TEST_F(DoublyLinkedListTest, CopyAfterModifications) {
    DoublyLinkedList<int> original;
    original.push_back(1);
    original.push_back(2);
    
    DoublyLinkedList<int> copy(original);
    original.push_back(3);
    copy.push_back(4);
    
    EXPECT_EQ(original.size(), 3);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(original.back(), 3);
    EXPECT_EQ(copy.back(), 4);
}
