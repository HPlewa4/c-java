#include "utils/test_framework.h"
#include "utils/tensor.h"
#include <iostream>

void test_tensor_basic() {
    TestFramework::TestSuite suite("Tensor Basic Operations");

    Tensor<double> t1({2, 3});
    suite.assert_equal(size_t(6), t1.size(), "Tensor size calculation");

    t1.fill(5.0);
    suite.assert_near(5.0, t1[0], 1e-9, "Tensor fill operation");
    suite.assert_near(5.0, t1[5], 1e-9, "Tensor fill all elements");

    Tensor<int> t2({3, 3}, 7);
    suite.assert_equal(7, t2[0], "Tensor construction with initial value");
    suite.assert_equal(9, static_cast<int>(t2.size()), "Tensor size 3x3");

    suite.assert_equal(63, t2.sum(), "Tensor sum operation");

    suite.assert_equal(7, t2.mean(), "Tensor mean operation");

    suite.print_summary();
}

void test_tensor_operations() {
    TestFramework::TestSuite suite("Tensor Mathematical Operations");

    Tensor<double> t1({2, 2}, 3.0);
    Tensor<double> t2({2, 2}, 2.0);
    Tensor<double> result = t1 + t2;
    suite.assert_near(5.0, result[0], 1e-9, "Tensor addition");
    suite.assert_near(5.0, result[3], 1e-9, "Tensor addition all elements");

    result = t1 - t2;
    suite.assert_near(1.0, result[0], 1e-9, "Tensor subtraction");

    result = t1 * 2.0;
    suite.assert_near(6.0, result[0], 1e-9, "Tensor scalar multiplication");

    result = t1.multiply(t2);
    suite.assert_near(6.0, result[0], 1e-9, "Tensor element-wise multiplication");

    suite.print_summary();
}

void test_tensor_functions() {
    TestFramework::TestSuite suite("Tensor Advanced Functions");

    Tensor<double> t1({2, 2}, 4.0);
    Tensor<double> result = t1.apply([](double x) { return x * x; });
    suite.assert_near(16.0, result[0], 1e-9, "Tensor apply function (square)");

    std::vector<double> data = {1.0, 5.0, 3.0, 9.0, 2.0, 7.0};
    Tensor<double> t2({2, 3}, data);
    suite.assert_near(9.0, t2.max(), 1e-9, "Tensor max element");
    suite.assert_near(1.0, t2.min(), 1e-9, "Tensor min element");

    t2.reshape({3, 2});
    auto shape = t2.get_shape();
    suite.assert_equal(size_t(3), shape[0], "Tensor reshape dimension 0");
    suite.assert_equal(size_t(2), shape[1], "Tensor reshape dimension 1");

    suite.print_summary();
}

void test_tensor_types() {
    TestFramework::TestSuite suite("Tensor Generic Types");

    Tensor<float> tf({2, 2}, 3.14f);
    suite.assert_near(3.14, static_cast<double>(tf[0]), 0.01, "Tensor with float type");

    Tensor<int> ti({3, 3}, 42);
    suite.assert_equal(42, ti[0], "Tensor with int type");
    suite.assert_equal(378, ti.sum(), "Tensor int sum (42 * 9)");

    Tensor<double> td = TensorUtils::convert<double, int>(ti);
    suite.assert_near(42.0, td[0], 1e-9, "Tensor type conversion int to double");

    suite.print_summary();
}

void test_tensor_utils() {
    TestFramework::TestSuite suite("Tensor Utility Functions");

    suite.assert_equal(5, TensorUtils::clamp(10, 0, 5), "Clamp max boundary");
    suite.assert_equal(0, TensorUtils::clamp(-5, 0, 5), "Clamp min boundary");
    suite.assert_equal(3, TensorUtils::clamp(3, 0, 5), "Clamp within range");

    suite.assert_near(5.0, TensorUtils::lerp(0.0, 10.0, 0.5), 1e-9, "Linear interpolation at 0.5");
    suite.assert_near(0.0, TensorUtils::lerp(0.0, 10.0, 0.0), 1e-9, "Linear interpolation at 0.0");
    suite.assert_near(10.0, TensorUtils::lerp(0.0, 10.0, 1.0), 1e-9, "Linear interpolation at 1.0");

    suite.print_summary();
}

void test_tensor_error_handling() {
    TestFramework::TestSuite suite("Tensor Error Handling");

    Tensor<double> t1({2, 2});
    Tensor<double> t2({3, 3});

    suite.assert_throws<std::invalid_argument>([&]() {
        auto result = t1 + t2;
    }, "Addition with mismatched shapes throws exception");

    suite.assert_throws<std::out_of_range>([&]() {
        double val = t1[100];
    }, "Out of bounds access throws exception");

    suite.assert_throws<std::invalid_argument>([&]() {
        t1.reshape({5, 5});
    }, "Invalid reshape throws exception");

    suite.print_summary();
}

int main() {
    std::cout << "================================" << std::endl;
    std::cout << "  Tensor Unit Tests" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

    test_tensor_basic();
    test_tensor_operations();
    test_tensor_functions();
    test_tensor_types();
    test_tensor_utils();
    test_tensor_error_handling();

    std::cout << "================================" << std::endl;
    std::cout << "  All Tensor Tests Complete!" << std::endl;
    std::cout << "================================" << std::endl;

    return 0;
}