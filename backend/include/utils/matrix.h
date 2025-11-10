#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <stdexcept>
#include <iostream>

// Generic template class for Matrix operations
template<typename T>
class Matrix {
private:
    std::vector<std::vector<T>> data;
    size_t rows;
    size_t cols;

public:
    // Constructor
    Matrix(size_t r, size_t c, T init_val = T()) : rows(r), cols(c) {
        data.resize(rows, std::vector<T>(cols, init_val));
    }

    // Get dimensions
    size_t getRows() const { return rows; }
    size_t getCols() const { return cols; }

    // Access operators
    std::vector<T>& operator[](size_t row) {
        if (row >= rows) {
            throw std::out_of_range("Row index out of range");
        }
        return data[row];
    }

    const std::vector<T>& operator[](size_t row) const {
        if (row >= rows) {
            throw std::out_of_range("Row index out of range");
        }
        return data[row];
    }

    // Operator overloading: Matrix addition
    Matrix<T> operator+(const Matrix<T>& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions must match for addition");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result[i][j] = data[i][j] + other[i][j];
            }
        }
        return result;
    }

    // Operator overloading: Matrix subtraction
    Matrix<T> operator-(const Matrix<T>& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrix dimensions must match for subtraction");
        }

        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result[i][j] = data[i][j] - other[i][j];
            }
        }
        return result;
    }

    // Operator overloading: Scalar multiplication
    Matrix<T> operator*(T scalar) const {
        Matrix<T> result(rows, cols);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result[i][j] = data[i][j] * scalar;
            }
        }
        return result;
    }

    // Operator overloading: Output stream
    friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& mat) {
        for (size_t i = 0; i < mat.rows; i++) {
            for (size_t j = 0; j < mat.cols; j++) {
                os << mat.data[i][j] << " ";
            }
            os << "\n";
        }
        return os;
    }

    // Transpose
    Matrix<T> transpose() const {
        Matrix<T> result(cols, rows);
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                result[j][i] = data[i][j];
            }
        }
        return result;
    }
};

#endif
