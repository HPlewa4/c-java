#include "utils/test_framework.h"
#include "optimizer/optimizer.h"
#include "optimizer/adam_optimizer.h"
#include <iostream>
#include <vector>
#include <cmath>

void test_sgd_optimizer_1d() {
    TestFramework::TestSuite suite("SGD Optimizer 1D Updates");

    SGDOptimizer sgd(0.1);

    std::vector<double> params = {1.0, 2.0, 3.0};
    std::vector<double> grads = {0.1, 0.2, 0.3};

    sgd.update_1d(params, grads);

    suite.assert_near(0.99, params[0], 1e-9, "SGD 1D update param[0]");
    suite.assert_near(1.98, params[1], 1e-9, "SGD 1D update param[1]");
    suite.assert_near(2.97, params[2], 1e-9, "SGD 1D update param[2]");

    suite.print_summary();
}

void test_sgd_optimizer_2d() {
    TestFramework::TestSuite suite("SGD Optimizer 2D Updates");

    SGDOptimizer sgd(0.05);

    std::vector<std::vector<double>> params = {
        {1.0, 2.0},
        {3.0, 4.0}
    };
    std::vector<std::vector<double>> grads = {
        {0.1, 0.2},
        {0.3, 0.4}
    };

    sgd.update_2d(params, grads);

    suite.assert_near(0.995, params[0][0], 1e-9, "SGD 2D update params[0][0]");
    suite.assert_near(1.99, params[0][1], 1e-9, "SGD 2D update params[0][1]");
    suite.assert_near(2.985, params[1][0], 1e-9, "SGD 2D update params[1][0]");
    suite.assert_near(3.98, params[1][1], 1e-9, "SGD 2D update params[1][1]");

    suite.print_summary();
}

void test_adam_optimizer() {
    TestFramework::TestSuite suite("Adam Optimizer");

    AdamOptimizer adam(0.001);

    std::vector<double> params = {1.0, 2.0};
    std::vector<double> grads = {0.5, 1.0};

    double initial_param0 = params[0];
    double initial_param1 = params[1];

    adam.update_1d(params, grads);

    suite.assert_true(params[0] != initial_param0, "Adam changes parameter[0]");
    suite.assert_true(params[1] != initial_param1, "Adam changes parameter[1]");

    suite.assert_true(params[0] < initial_param0, "Adam decreases parameter[0]");
    suite.assert_true(params[1] < initial_param1, "Adam decreases parameter[1]");

    double after_first_param0 = params[0];
    adam.update_1d(params, grads);

    suite.assert_true(params[0] < after_first_param0, "Adam continues to update");

    suite.print_summary();
}

void test_optimizer_factory() {
    TestFramework::TestSuite suite("Optimizer Factory");

    auto sgd = OptimizerFactory::create(OptimizerType::SGD, 0.01);
    suite.assert_true(sgd != nullptr, "Factory creates SGD optimizer");
    suite.assert_near(0.01, sgd->get_learning_rate(), 1e-9, "SGD has correct learning rate");

    auto adam = OptimizerFactory::create(OptimizerType::ADAM, 0.001);
    suite.assert_true(adam != nullptr, "Factory creates Adam optimizer");
    suite.assert_near(0.001, adam->get_learning_rate(), 1e-9, "Adam has correct learning rate");

    suite.print_summary();
}

void test_optimizer_learning_rate() {
    TestFramework::TestSuite suite("Optimizer Learning Rate");

    SGDOptimizer sgd(0.1);

    suite.assert_near(0.1, sgd.get_learning_rate(), 1e-9, "Initial learning rate");

    sgd.set_learning_rate(0.05);
    suite.assert_near(0.05, sgd.get_learning_rate(), 1e-9, "Updated learning rate");

    std::vector<double> params = {1.0};
    std::vector<double> grads = {1.0};

    sgd.update_1d(params, grads);

    suite.assert_near(0.95, params[0], 1e-9, "Update uses new learning rate");

    suite.print_summary();
}

void test_optimizer_polymorphism() {
    TestFramework::TestSuite suite("Optimizer Polymorphism");

    std::unique_ptr<Optimizer> opt1 = OptimizerFactory::create(OptimizerType::SGD, 0.1);
    std::unique_ptr<Optimizer> opt2 = OptimizerFactory::create(OptimizerType::ADAM, 0.001);

    std::vector<double> params1 = {1.0, 2.0};
    std::vector<double> params2 = {1.0, 2.0};
    std::vector<double> grads = {0.5, 0.5};

    opt1->update_1d(params1, grads);
    opt2->update_1d(params2, grads);

    suite.assert_true(params1[0] < 1.0, "Polymorphic SGD updates parameters");
    suite.assert_true(params2[0] < 1.0, "Polymorphic Adam updates parameters");

    suite.assert_true(std::abs(params1[0] - params2[0]) > 1e-6,
                     "Different optimizer types produce different updates");

    suite.print_summary();
}

void test_adam_momentum() {
    TestFramework::TestSuite suite("Adam Momentum Behavior");

    AdamOptimizer adam(0.01);

    std::vector<double> params = {0.0};
    std::vector<double> grads = {1.0};

    double prev_change = 0.0;
    for (int i = 0; i < 5; i++) {
        double prev_param = params[0];
        adam.update_1d(params, grads);
        double change = prev_param - params[0];
        prev_change = change;
    }

    suite.assert_true(params[0] < -0.01, "Adam accumulates momentum over multiple steps");

    suite.print_summary();
}

int main() {
    std::cout << "================================" << std::endl;
    std::cout << "  Optimizer Unit Tests" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

    test_sgd_optimizer_1d();
    test_sgd_optimizer_2d();
    test_adam_optimizer();
    test_optimizer_factory();
    test_optimizer_learning_rate();
    test_optimizer_polymorphism();
    test_adam_momentum();

    std::cout << "================================" << std::endl;
    std::cout << "  All Optimizer Tests Complete!" << std::endl;
    std::cout << "================================" << std::endl;

    return 0;
}