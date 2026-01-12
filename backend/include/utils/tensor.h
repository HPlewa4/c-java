#ifndef TENSOR_H
#define TENSOR_H

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>

template <typename T>
class Tensor {
private:
    std::vector<T> data;
    std::vector<size_t> shape;
    size_t total_size;
    size_t calculate_size(const std::vector<size_t>& dims) const {
        if (dims.empty()) return 0;
        return std::accumulate(dims.begin(), dims.end(), size_t(1), std::multiplies<size_t>());
    }

public:
    Tensor() : total_size(0) {}

    explicit Tensor(const std::vector<size_t>& dims)
        : shape(dims), total_size(calculate_size(dims)) {
        data.resize(total_size, T(0));
    }

    Tensor(const std::vector<size_t>& dims, T initial_value)
        : shape(dims), total_size(calculate_size(dims)) {
        data.resize(total_size, initial_value);
    }
    Tensor(const std::vector<size_t>& dims, const std::vector<T>& values)
        : shape(dims), total_size(calculate_size(dims)) {
        if (values.size() != total_size) {
            throw std::invalid_argument("Data size doesn't match tensor shape");
        }
        data = values;
    }
    const std::vector<size_t>& get_shape() const { return shape; }

    size_t size() const { return total_size; }

    const std::vector<T>& get_data() const { return data; }
    std::vector<T>& get_data() { return data; }

    T& operator[](size_t idx) {
        if (idx >= total_size) {
            throw std::out_of_range("Tensor index out of bounds");
        }
        return data[idx];
    }

    const T& operator[](size_t idx) const {
        if (idx >= total_size) {
            throw std::out_of_range("Tensor index out of bounds");
        }
        return data[idx];
    }

    void fill(T value) {
        std::fill(data.begin(), data.end(), value);
    }

    Tensor<T> operator+(const Tensor<T>& other) const {
        if (shape != other.shape) {
            throw std::invalid_argument("Tensor shapes must match for addition");
        }
        Tensor<T> result(shape);
        for (size_t i = 0; i < total_size; i++) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    Tensor<T> operator-(const Tensor<T>& other) const {
        if (shape != other.shape) {
            throw std::invalid_argument("Tensor shapes must match for subtraction");
        }
        Tensor<T> result(shape);
        for (size_t i = 0; i < total_size; i++) {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }

    Tensor<T> operator*(T scalar) const {
        Tensor<T> result(shape);
        for (size_t i = 0; i < total_size; i++) {
            result.data[i] = data[i] * scalar;
        }
        return result;
    }

    Tensor<T> multiply(const Tensor<T>& other) const {
        if (shape != other.shape) {
            throw std::invalid_argument("Tensor shapes must match for multiplication");
        }
        Tensor<T> result(shape);
        for (size_t i = 0; i < total_size; i++) {
            result.data[i] = data[i] * other.data[i];
        }
        return result;
    }

    template <typename Func>
    Tensor<T> apply(Func func) const {
        Tensor<T> result(shape);
        for (size_t i = 0; i < total_size; i++) {
            result.data[i] = func(data[i]);
        }
        return result;
    }

    T sum() const {
        return std::accumulate(data.begin(), data.end(), T(0));
    }

    T mean() const {
        if (total_size == 0) return T(0);
        return sum() / static_cast<T>(total_size);
    }

    T max() const {
        if (data.empty()) throw std::runtime_error("Cannot find max of empty tensor");
        return *std::max_element(data.begin(), data.end());
    }

    T min() const {
        if (data.empty()) throw std::runtime_error("Cannot find min of empty tensor");
        return *std::min_element(data.begin(), data.end());
    }

    void reshape(const std::vector<size_t>& new_shape) {
        size_t new_size = calculate_size(new_shape);
        if (new_size != total_size) {
            throw std::invalid_argument("New shape must have same total size");
        }
        shape = new_shape;
    }

    Tensor<T> clone() const {
        return Tensor<T>(shape, data);
    }
};

namespace TensorUtils {
    template <typename T>
    T clamp(T value, T min_val, T max_val) {
        return std::max(min_val, std::min(value, max_val));
    }

    template <typename T>
    T lerp(T a, T b, T t) {
        return a + t * (b - a);
    }

    template <typename TOut, typename TIn>
    Tensor<TOut> convert(const Tensor<TIn>& input) {
        Tensor<TOut> output(input.get_shape());
        const auto& in_data = input.get_data();
        auto& out_data = output.get_data();

        for (size_t i = 0; i < input.size(); i++) {
            out_data[i] = static_cast<TOut>(in_data[i]);
        }
        return output;
    }
}

#endif