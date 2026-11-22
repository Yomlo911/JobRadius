#pragma once

#include <vector>
#include <stdexcept>
#include "utils.h"

// First in, first out.
template <typename T>
class Queue {
private:
    vector<T> items;
    size_t head;

public:
    Queue() : head(0) {}

    void enqueue(const T& value) { items.push_back(value); }

    T dequeue() {
        if (isEmpty()) throw runtime_error("dequeue on empty queue");
        T value = items[head++];
        if (head == items.size()) { items.clear(); head = 0; }
        return value;
    }

    const T& front() const {
        if (isEmpty()) throw runtime_error("front on empty queue");
        return items[head];
    }

    bool isEmpty() const { return head == items.size(); }
    size_t size() const  { return items.size() - head; }
};
