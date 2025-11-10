#include "neural_network.h"
#include "activation_function.h"
#include "sigmoid_activation.h"
#include "relu_activation.h"
#include "tanh_activation.h"
#include "data_buffer.h"
#include "matrix.h"
#include <iostream>
#include <cassert>
#include <cmath>

// Simple test framework
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) void name()
#define ASSERT_TRUE(condition) \
    if (condition) { \
        tests_passed++; \
        std::cout << "  ✓ PASS: " << #condition << std::endl; \
    } else { \
        tests_failed++; \
        std::cerr << "  ✗ FAIL: " << #condition << std::endl; \
    }

#define ASSERT_EQ(a, b) \
    if ((a) == (b)) { \
        tests_passed++; \
        std::cout << "  ✓ PASS: " << #a << " == " << #b << std::endl; \
    } else { \
        tests_failed++; \
        std::cerr << "  ✗ FAIL: " << #a << " != " << #b << " (" << (a) << " != " << (b) << ")" << std::endl; \
    }

#define ASSERT_NEAR(a, b, epsilon) \
    if (std::abs((a) - (b)) < (epsilon)) { \
        tests_passed++; \
        std::cout << "  ✓ PASS: " << #a << " ≈ " << #b << std::endl; \
    } else { \
        tests_failed++; \
        std::cerr << "  ✗ FAIL: " << #a << " !≈ " << #b << " (" << (a) << " vs " << (b) << ")" << std::endl; \
    }

#define RUN_TEST(test_func) \
    std::cout << "\nRunning " << #test_func << "..." << std::endl; \
    test_func();

// Test Neural Network Construction
TEST(test_neural_network_construction) {
    NeuralNetwork nn({2, 3, 1});
    ASSERT_TRUE(true);  // If we get here, construction succeeded
}

// Test Neural Network Forward Pass
TEST(test_neural_network_forward) {
    NeuralNetwork nn({2, 3, 1});
    std::vector<double> input = {0.5, 0.5};
    auto output = nn.forward(input);

    ASSERT_EQ(output.size(), 1);
    ASSERT_TRUE(output[0] >= 0.0 && output[0] <= 1.0);
}

// Test Neural Network Training
TEST(test_neural_network_training) {
    NeuralNetwork nn({2, 2, 1}, 0.1);
    std::vector<double> input = {0.0, 1.0};
    std::vector<double> target = {1.0};

    auto output_before = nn.forward(input);
    nn.train(input, target);
    auto output_after = nn.forward(input);

    // After training, output should change
    ASSERT_TRUE(output_before[0] != output_after[0]);
}

// Test Activation Functions - Sigmoid
TEST(test_sigmoid_activation) {
    SigmoidActivation sigmoid;

    double result = sigmoid.activate(0.0);
    ASSERT_NEAR(result, 0.5, 0.001);

    ASSERT_EQ(sigmoid.getType(), ActivationType::SIGMOID);
}

// Test Activation Functions - ReLU
TEST(test_relu_activation) {
    ReLUActivation relu;

    ASSERT_EQ(relu.activate(5.0), 5.0);
    ASSERT_EQ(relu.activate(-5.0), 0.0);
    ASSERT_EQ(relu.getType(), ActivationType::RELU);
}

// Test Activation Functions - Tanh
TEST(test_tanh_activation) {
    TanhActivation tanh_act;

    double result = tanh_act.activate(0.0);
    ASSERT_NEAR(result, 0.0, 0.001);

    ASSERT_EQ(tanh_act.getType(), ActivationType::TANH);
}

// Test Activation Factory - Polymorphism
TEST(test_activation_factory) {
    auto sigmoid = ActivationFactory::create(ActivationType::SIGMOID);
    auto relu = ActivationFactory::create(ActivationType::RELU);
    auto tanh = ActivationFactory::create(ActivationType::TANH);

    ASSERT_TRUE(sigmoid != nullptr);
    ASSERT_TRUE(relu != nullptr);
    ASSERT_TRUE(tanh != nullptr);

    ASSERT_EQ(sigmoid->getType(), ActivationType::SIGMOID);
    ASSERT_EQ(relu->getType(), ActivationType::RELU);
    ASSERT_EQ(tanh->getType(), ActivationType::TANH);
}

// Test DataBuffer Template Class
TEST(test_data_buffer) {
    DataBuffer<int> buffer(10);

    buffer.add(5);
    buffer.add(10);
    buffer.add(3);

    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.get(0), 5);
    ASSERT_EQ(buffer.get(1), 10);
    ASSERT_EQ(buffer.findMin(), 3);
    ASSERT_EQ(buffer.findMax(), 10);
}

// Test DataBuffer with doubles
TEST(test_data_buffer_double) {
    DataBuffer<double> buffer(100);

    buffer.add(1.5);
    buffer.add(2.5);
    buffer.add(0.5);

    ASSERT_EQ(buffer.size(), 3);
    ASSERT_NEAR(buffer.findMin(), 0.5, 0.001);
    ASSERT_NEAR(buffer.findMax(), 2.5, 0.001);
}

// Test Matrix Template Class
TEST(test_matrix_creation) {
    Matrix<int> mat(3, 3, 0);

    ASSERT_EQ(mat.getRows(), 3);
    ASSERT_EQ(mat.getCols(), 3);
}

// Test Matrix Addition (Operator Overloading)
TEST(test_matrix_addition) {
    Matrix<int> mat1(2, 2, 1);
    Matrix<int> mat2(2, 2, 2);

    Matrix<int> result = mat1 + mat2;

    ASSERT_EQ(result[0][0], 3);
    ASSERT_EQ(result[1][1], 3);
}

// Test Matrix Subtraction (Operator Overloading)
TEST(test_matrix_subtraction) {
    Matrix<int> mat1(2, 2, 5);
    Matrix<int> mat2(2, 2, 2);

    Matrix<int> result = mat1 - mat2;

    ASSERT_EQ(result[0][0], 3);
    ASSERT_EQ(result[1][1], 3);
}

// Test Matrix Scalar Multiplication (Operator Overloading)
TEST(test_matrix_scalar_multiplication) {
    Matrix<int> mat(2, 2, 3);

    Matrix<int> result = mat * 2;

    ASSERT_EQ(result[0][0], 6);
    ASSERT_EQ(result[1][1], 6);
}

// Test Matrix Transpose
TEST(test_matrix_transpose) {
    Matrix<int> mat(2, 3, 0);
    mat[0][0] = 1; mat[0][1] = 2; mat[0][2] = 3;
    mat[1][0] = 4; mat[1][1] = 5; mat[1][2] = 6;

    Matrix<int> transposed = mat.transpose();

    ASSERT_EQ(transposed.getRows(), 3);
    ASSERT_EQ(transposed.getCols(), 2);
    ASSERT_EQ(transposed[0][0], 1);
    ASSERT_EQ(transposed[1][0], 2);
    ASSERT_EQ(transposed[2][1], 6);
}

// Test Neural Network with Different Activation Types
TEST(test_neural_network_with_different_activations) {
    NeuralNetwork nn_sigmoid({2, 3, 1}, 0.01, ActivationType::SIGMOID);
    NeuralNetwork nn_relu({2, 3, 1}, 0.01, ActivationType::RELU);
    NeuralNetwork nn_tanh({2, 3, 1}, 0.01, ActivationType::TANH);

    ASSERT_TRUE(nn_sigmoid.getActivationType() == ActivationType::SIGMOID);
    ASSERT_TRUE(nn_relu.getActivationType() == ActivationType::RELU);
    ASSERT_TRUE(nn_tanh.getActivationType() == ActivationType::TANH);
}

// Test Enum Class
TEST(test_enum_class) {
    ActivationType type1 = ActivationType::SIGMOID;
    ActivationType type2 = ActivationType::RELU;

    ASSERT_TRUE(type1 != type2);
    ASSERT_TRUE(type1 == ActivationType::SIGMOID);
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "  Neural Network Unit Tests" << std::endl;
    std::cout << "==================================" << std::endl;

    RUN_TEST(test_neural_network_construction);
    RUN_TEST(test_neural_network_forward);
    RUN_TEST(test_neural_network_training);
    RUN_TEST(test_sigmoid_activation);
    RUN_TEST(test_relu_activation);
    RUN_TEST(test_tanh_activation);
    RUN_TEST(test_activation_factory);
    RUN_TEST(test_data_buffer);
    RUN_TEST(test_data_buffer_double);
    RUN_TEST(test_matrix_creation);
    RUN_TEST(test_matrix_addition);
    RUN_TEST(test_matrix_subtraction);
    RUN_TEST(test_matrix_scalar_multiplication);
    RUN_TEST(test_matrix_transpose);
    RUN_TEST(test_neural_network_with_different_activations);
    RUN_TEST(test_enum_class);

    std::cout << "\n==================================" << std::endl;
    std::cout << "  Test Summary" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "Tests Passed: " << tests_passed << std::endl;
    std::cout << "Tests Failed: " << tests_failed << std::endl;
    std::cout << "Total Tests:  " << (tests_passed + tests_failed) << std::endl;

    if (tests_failed == 0) {
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cerr << "\n✗ Some tests failed!" << std::endl;
        return 1;
    }
}
