#include "min_heap.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <set>
#include <vector>

namespace {

std::vector<int> drain(MinHeap<int> &heap) {
    std::vector<int> out;
    while (!heap.empty()) {
        out.push_back(heap.pop());
    }
    return out;
}

} // namespace

TEST(MinHeap, PopsInAscendingOrder) {
    // The values from the heap test that used to be commented out in main.cc.
    std::vector<int> values{7, 9, 18, 2, 10, 5, 26, 14, 91, 87, 24, 31, 83};
    MinHeap<int> heap;
    for (int v : values) {
        heap.push(v);
    }
    std::sort(values.begin(), values.end());
    EXPECT_EQ(drain(heap), values);
}

// Regression test: the old sift_down always swapped with the smaller child,
// even when the parent was already smaller, and 456 of these 1000 random heaps
// came out in the wrong order.
TEST(MinHeap, RandomHeapsPopInOrder) {
    std::mt19937 rng{1};
    for (int trial = 0; trial < 1000; trial++) {
        MinHeap<int> heap;
        std::vector<int> values;
        int count = 1 + static_cast<int>(rng() % 40);
        for (int i = 0; i < count; i++) {
            int v = static_cast<int>(rng() % 100);
            values.push_back(v);
            heap.push(v);
        }
        std::sort(values.begin(), values.end());
        ASSERT_EQ(drain(heap), values) << "trial " << trial;
    }
}

TEST(MinHeap, InterleavedPushAndPopMatchMultiset) {
    std::mt19937 rng{2};
    MinHeap<int> heap;
    std::multiset<int> reference;
    for (int i = 0; i < 20000; i++) {
        if (reference.empty() || rng() % 3 != 0) {
            int v = static_cast<int>(rng() % 1000);
            heap.push(v);
            reference.insert(v);
        } else {
            ASSERT_EQ(heap.top(), *reference.begin());
            ASSERT_EQ(heap.pop(), *reference.begin());
            reference.erase(reference.begin());
        }
        ASSERT_EQ(heap.size(), reference.size());
    }
}

TEST(MinHeap, UsesCustomComparator) {
    MinHeap<int, std::greater<int>> heap;
    for (int v : {3, 1, 4, 1, 5, 9, 2, 6}) {
        heap.push(v);
    }
    EXPECT_EQ(heap.pop(), 9);
    EXPECT_EQ(heap.pop(), 6);
    EXPECT_EQ(heap.pop(), 5);
}

TEST(MinHeap, EmptyHeapThrows) {
    MinHeap<int> heap;
    EXPECT_THROW(heap.top(), std::out_of_range);
    EXPECT_THROW(heap.pop(), std::out_of_range);
}
