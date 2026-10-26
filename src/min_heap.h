#ifndef PATHFINDER_MIN_HEAP_H
#define PATHFINDER_MIN_HEAP_H

#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

// Binary min-heap stored in a vector. For the node at index i, its children
// are at 2i + 1 and 2i + 2 and its parent is at (i - 1) / 2.
template <typename T, typename Less = std::less<T>> class MinHeap {
    std::vector<T> items_;
    Less less_;

    void sift_up(size_t idx) {
        while (idx > 0) {
            size_t parent = (idx - 1) / 2;
            if (!less_(items_[idx], items_[parent])) {
                return;
            }
            std::swap(items_[idx], items_[parent]);
            idx = parent;
        }
    }

    void sift_down(size_t idx) {
        const size_t size = items_.size();
        while (true) {
            size_t smallest = idx;
            size_t left = 2 * idx + 1;
            size_t right = left + 1;
            if (left < size && less_(items_[left], items_[smallest])) {
                smallest = left;
            }
            if (right < size && less_(items_[right], items_[smallest])) {
                smallest = right;
            }
            // Stop as soon as the parent is no larger than both children.
            if (smallest == idx) {
                return;
            }
            std::swap(items_[idx], items_[smallest]);
            idx = smallest;
        }
    }

  public:
    bool empty() const { return items_.empty(); }
    size_t size() const { return items_.size(); }

    const T &top() const {
        if (items_.empty()) {
            throw std::out_of_range("top() on an empty heap");
        }
        return items_.front();
    }

    void push(T value) {
        items_.push_back(std::move(value));
        sift_up(items_.size() - 1);
    }

    T pop() {
        if (items_.empty()) {
            throw std::out_of_range("pop() on an empty heap");
        }
        T result = std::move(items_.front());
        items_.front() = std::move(items_.back());
        items_.pop_back();
        if (!items_.empty()) {
            sift_down(0);
        }
        return result;
    }
};

#endif
