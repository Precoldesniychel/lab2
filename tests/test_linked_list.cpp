#include <gtest/gtest.h>
#include "../include/Base/LinkedList.h"

TEST(LinkedListTest, ConstructorsAndBasicOps) {
    LinkedList<int> list1;
    EXPECT_EQ(list1.GetLength(), 0);

    int raw_arr[] = {10, 20, 30};
    LinkedList<int> list2(raw_arr, 3);
    EXPECT_EQ(list2.GetLength(), 3);
    EXPECT_EQ(list2.GetFirst(), 10);
    EXPECT_EQ(list2.GetLast(), 30);
    EXPECT_EQ(list2.Get(1), 20);

    LinkedList<int> list3(list2);
    EXPECT_EQ(list3.GetLength(), 3);
    EXPECT_EQ(list3.Get(1), 20);
}

TEST(LinkedListTest, ModificationOps) {
    LinkedList<int> list;

    list.Append(20);
    list.Prepend(10);
    list.Append(30);

    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.Get(0), 10);
    EXPECT_EQ(list.Get(2), 30);

    list.InsertAt(15, 1);
    EXPECT_EQ(list.Get(1), 15);
    EXPECT_EQ(list.Get(2), 20);

    list.InsertAt(5, 0);
    list.InsertAt(35, list.GetLength());
    EXPECT_EQ(list.GetFirst(), 5);
    EXPECT_EQ(list.GetLast(), 35);
}

TEST(LinkedListTest, GetSubList) {
    int raw_arr[] = {1, 2, 3, 4, 5};
    LinkedList<int> list(raw_arr, 5);

    LinkedList<int>* sub = list.GetSubList(1, 3);

    EXPECT_EQ(sub->GetLength(), 3);
    EXPECT_EQ(sub->Get(0), 2);
    EXPECT_EQ(sub->Get(2), 4);
    delete sub;
}

TEST(LinkedListTest, Concat) {
    int arr1[] = {1, 2};
    int arr2[] = {3, 4};
    LinkedList<int> list1(arr1, 2);
    LinkedList<int> list2(arr2, 2);

    LinkedList<int>* res = list1.Concat(list2);

    EXPECT_EQ(res->GetLength(), 4);
    EXPECT_EQ(res->Get(0), 1);
    EXPECT_EQ(res->Get(3), 4);

    delete res;
}

TEST(LinkedListTest, Exceptions) {
    LinkedList<int> empty_list;

    EXPECT_THROW(empty_list.GetFirst(), std::out_of_range);
    EXPECT_THROW(empty_list.GetLast(), std::out_of_range);
    EXPECT_THROW(empty_list.Get(0), std::out_of_range);
    int raw[] = {1, 2};
    LinkedList<int> list(raw, 2);

    EXPECT_THROW(list.Get(-1), std::out_of_range);
    EXPECT_THROW(list.Get(2), std::out_of_range);
    EXPECT_THROW(list.InsertAt(100, 5), std::out_of_range);

    EXPECT_THROW(list.GetSubList(1, 0), std::out_of_range);

    try {
        list.Get(10);
    } catch (const std::out_of_range& e) {
        EXPECT_STREQ(e.what(), "IndexOutOfRange");
    }
}