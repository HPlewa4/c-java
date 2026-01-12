#include "utils/test_framework.h"
#include "model/activation/activation_function.h"
#include "model/activation/relu_activation.h"
#include "model/activation/sigmoid_activation.h"
#include "model/activation/tanh_activation.h"
#include <iostream>
#include <cmath>

void test_relu_activation() {
    TestFramework::TestSuite suite("ReLU Activation Function");

    auto relu = ActivationFactory::create(ActivationType::RELU);

    suite.assert_near(5.0, relu->activate(5.0), 1e-9, "ReLU positive input");
    suite.assert_near(100.0, relu->activate(100.0), 1e-9, "ReLU large positive input");

    suite.assert_near(0.0, relu->activate(-5.0), 1e-9, "ReLU negative input");
    suite.assert_near(0.0, relu->activate(-100.0), 1e-9, "ReLU large negative input");

    suite.assert_near(0.0, relu->activate(0.0), 1e-9, "ReLU zero input");

    suite.assert_near(1.0, relu->derivative(5.0), 1e-9, "ReLU derivative positive");
    suite.assert_near(0.0, relu->derivative(-5.0), 1e-9, "ReLU derivative negative");
    suite.assert_near(0.0, relu->derivative(0.0), 1e-9, "ReLU derivative zero");

    suite.assert_true(relu->getType() == ActivationType::RELU, "ReLU type check");

    suite.print_summary();
}

void test_sigmoid_activation() {
    TestFramework::TestSuite suite("Sigmoid Activation Function");

    auto sigmoid = ActivationFactory::create(ActivationType::SIGMOID);

    double result = sigmoid->activate(0.0);
    suite.assert_near(0.5, result, 1e-9, "Sigmoid at zero");

    result = sigmoid->activate(10.0);
    suite.assert_true(result > 0.99, "Sigmoid large positive approaches 1");

    result = sigmoid->activate(-10.0);
    suite.assert_true(result < 0.01, "Sigmoid large negative approaches 0");

    double x = sigmoid->activate(0.0);
    double deriv = sigmoid->derivative(x);
    suite.assert_near(0.25, deriv, 1e-9, "Sigmoid derivative at 0.5");

    suite.assert_true(sigmoid->getType() == ActivationType::SIGMOID, "Sigmoid type check");

    suite.print_summary();
}

void test_tanh_activation() {
    TestFramework::TestSuite suite("Tanh Activation Function");

    auto tanh_fn = ActivationFactory::create(ActivationType::TANH);

    double result = tanh_fn->activate(0.0);
    suite.assert_near(0.0, result, 1e-9, "Tanh at zero");

    result = tanh_fn->activate(10.0);
    suite.assert_true(result > 0.99, "Tanh large positive approaches 1");

    result = tanh_fn->activate(-10.0);
    suite.assert_true(result < -0.99, "Tanh large negative approaches -1");

    double x = tanh_fn->activate(0.0);
    double deriv = tanh_fn->derivative(x);
    suite.assert_near(1.0, deriv, 1e-9, "Tanh derivative at 0");

    suite.assert_true(tanh_fn->getType() == ActivationType::TANH, "Tanh type check");

    suite.print_summary();
}

void test_activation_polymorphism() {
    TestFramework::TestSuite suite("Activation Polymorphism");

    auto relu = ActivationFactory::create(ActivationType::RELU);
    auto sigmoid = ActivationFactory::create(ActivationType::SIGMOID);
    auto tanh_fn = ActivationFactory::create(ActivationType::TANH);

    double input = 0.5;
    double relu_out = relu->activate(input);
    double sigmoid_out = sigmoid->activate(input);
    double tanh_out = tanh_fn->activate(input);

    suite.assert_near(0.5, relu_out, 1e-9, "Polymorphic ReLU call");
    suite.assert_true(sigmoid_out > 0.6 && sigmoid_out < 0.7, "Polymorphic Sigmoid call");
    suite.assert_true(tanh_out > 0.45 && tanh_out < 0.47, "Polymorphic Tanh call");

    suite.assert_true(relu_out != sigmoid_out, "Different activation functions produce different results (1)");
    suite.assert_true(sigmoid_out != tanh_out, "Different activation functions produce different results (2)");

    suite.print_summary();
}

void test_activation_edge_cases() {
    TestFramework::TestSuite suite("Activation Edge Cases");

    auto relu = ActivationFactory::create(ActivationType::RELU);
    auto sigmoid = ActivationFactory::create(ActivationType::SIGMOID);

    double large_pos = 1000.0;
    double large_neg = -1000.0;

    suite.assert_near(large_pos, relu->activate(large_pos), 1e-9, "ReLU very large positive");
    suite.assert_near(0.0, relu->activate(large_neg), 1e-9, "ReLU very large negative");

    double sig_large = sigmoid->activate(large_pos);
    suite.assert_true(sig_large >= 0.0 && sig_large <= 1.0, "Sigmoid doesn't overflow on large positive");

    sig_large = sigmoid->activate(large_neg);
    suite.assert_true(sig_large >= 0.0 && sig_large <= 1.0, "Sigmoid doesn't overflow on large negative");

    suite.print_summary();
}

void test_activation_chain() {
    TestFramework::TestSuite suite("Activation Chain Rule");

    auto relu = ActivationFactory::create(ActivationType::RELU);

    double input = 3.5;
    double activated = relu->activate(input);
    double grad = relu->derivative(activated);

    suite.assert_near(3.5, activated, 1e-9, "Forward pass");
    suite.assert_near(1.0, grad, 1e-9, "Backward pass gradient");

    input = -2.0;
    activated = relu->activate(input);
    grad = relu->derivative(activated);

    suite.assert_near(0.0, activated, 1e-9, "Forward pass negative");
    suite.assert_near(0.0, grad, 1e-9, "Backward pass gradient negative");

    suite.print_summary();
}

int main() {
    std::cout << "================================" << std::endl;
    std::cout << "  Activation Unit Tests" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

    test_relu_activation();
    test_sigmoid_activation();
    test_tanh_activation();
    test_activation_polymorphism();
    test_activation_edge_cases();
    test_activation_chain();

    std::cout << "================================" << std::endl;
    std::cout << "  All Activation Tests Complete!" << std::endl;
    std::cout << "================================" << std::endl;

    return 0;
}