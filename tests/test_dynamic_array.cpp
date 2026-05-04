#include <gtest/gtest.h>
#include "../include/Base/DynamicArray.h"

TEST(DynamicArrayTest, Constructors) {
    DynamicArray<int> arr1(5);
    EXPECT_EQ(arr1.GetSize(), 5);

    int raw_arr[] = {1, 2, 3, 4, 5};
    DynamicArray<int> arr2(raw_arr, 5);
    EXPECT_EQ(arr2.GetSize(), 5);
    EXPECT_EQ(arr2.Get(0), 1);
    EXPECT_EQ(arr2.Get(4), 5);

    DynamicArray<int> arr3(arr2);
    EXPECT_EQ(arr3.GetSize(), 5);
    EXPECT_EQ(arr3.Get(2), 3);
}

TEST(DynamicArrayTest, SetAndGet) {
    DynamicArray<int> arr(3);
    arr.Set(0, 10);
    arr.Set(1, 20);
    arr.Set(2, 30);

    EXPECT_EQ(arr.Get(0), 10);
    EXPECT_EQ(arr.Get(1), 20);
    EXPECT_EQ(arr.Get(2), 30);
}

TEST(DynamicArrayTest, Resize) {
    int raw_arr[] = {1, 2, 3};
    DynamicArray<int> arr(raw_arr, 3);

    arr.Resize(5);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);

    arr.Resize(2);
    EXPECT_EQ(arr.GetSize(), 2);
    EXPECT_EQ(arr.Get(1), 2);
}

TEST(DynamicArrayTest, Exceptions) {
    DynamicArray<int> arr(5);

    EXPECT_THROW(arr.Get(-1), std::out_of_range);

    EXPECT_THROW(arr.Get(5), std::out_of_range);

    EXPECT_THROW(arr.Set(10, 100), std::out_of_range);

    try {
        arr.Get(10);
    } catch (const std::out_of_range& e) {
        EXPECT_STREQ(e.what(), "IndexOutOfRange");
    }
}

TEST(DynamicArrayTest, ZeroSize) {
    DynamicArray<int> arr(0);
    EXPECT_EQ(arr.GetSize(), 0);

    EXPECT_THROW(arr.Get(0), std::out_of_range);

    arr.Resize(1);
    EXPECT_EQ(arr.GetSize(), 1);
}