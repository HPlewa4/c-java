#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <vector>
#include <stdexcept>
#include <algorithm>

// Generic template class for storing and managing data
template<typename T>
class DataBuffer {
private:
    std::vector<T> data;
    size_t max_size;

public:
    // Constructor
    DataBuffer(size_t max = 1000) : max_size(max) {
        data.reserve(max);
    }

    // Add item to buffer
    void add(const T& item) {
        if (data.size() >= max_size) {
            throw std::overflow_error("Buffer is full");
        }
        data.push_back(item);
    }

    // Get item at index
    T& get(size_t idx) {
        if (idx >= data.size()) {
            throw std::out_of_range("Index out of range");
        }
        return data[idx];
    }

    const T& get(size_t idx) const {
        if (idx >= data.size()) {
            throw std::out_of_range("Index out of range");
        }
        return data[idx];
    }

    // Size of buffer
    size_t size() const {
        return data.size();
    }

    // Clear buffer
    void clear() {
        data.clear();
    }

    // Check if empty
    bool empty() const {
        return data.empty();
    }

    // Get all data
    const std::vector<T>& getData() const {
        return data;
    }

    // STL algorithm usage - find minimum element
    T findMin() const {
        if (data.empty()) {
            throw std::runtime_error("Buffer is empty");
        }
        return *std::min_element(data.begin(), data.end());
    }

    // STL algorithm usage - find maximum element
    T findMax() const {
        if (data.empty()) {
            throw std::runtime_error("Buffer is empty");
        }
        return *std::max_element(data.begin(), data.end());
    }

    // STL algorithm usage - count elements matching predicate
    template<typename Predicate>
    size_t countIf(Predicate pred) const {
        return std::count_if(data.begin(), data.end(), pred);
    }
};

#endif
