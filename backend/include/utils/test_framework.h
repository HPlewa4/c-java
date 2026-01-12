#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>

namespace TestFramework {
    struct TestResult {
        std::string test_name;
        bool passed;
        std::string message;
    };

    class TestSuite {
    private:
        std::string suite_name;
        std::vector<TestResult> results;
        int total_tests = 0;
        int passed_tests = 0;

    public:
        explicit TestSuite(const std::string& name) : suite_name(name) {}

        void assert_true(bool condition, const std::string& test_name, const std::string& message = "") {
            total_tests++;
            TestResult result;
            result.test_name = test_name;
            result.passed = condition;
            result.message = message;

            if (condition) {
                passed_tests++;
                std::cout << "  ✓ " << test_name << std::endl;
            } else {
                std::cout << "  ✗ " << test_name << " - " << message << std::endl;
            }

            results.push_back(result);
        }

        template <typename T>
        void assert_equal(const T& expected, const T& actual, const std::string& test_name) {
            bool passed = (expected == actual);
            std::string message = passed ? "" : "Expected: " + std::to_string(expected) + ", Got: " + std::to_string(actual);
            assert_true(passed, test_name, message);
        }

        void assert_near(double expected, double actual, double tolerance, const std::string& test_name) {
            bool passed = std::abs(expected - actual) < tolerance;
            std::string message = passed ? "" : "Expected: " + std::to_string(expected) +
                                                ", Got: " + std::to_string(actual) +
                                                ", Tolerance: " + std::to_string(tolerance);
            assert_true(passed, test_name, message);
        }

        template <typename ExceptionType>
        void assert_throws(std::function<void()> func, const std::string& test_name) {
            bool threw_correct_exception = false;
            try {
                func();
            } catch (const ExceptionType& e) {
                threw_correct_exception = true;
            }
            assert_true(threw_correct_exception, test_name,
                       threw_correct_exception ? "" : "Did not throw expected exception");
        }

        void print_summary() {
            std::cout << "\n" << suite_name << " Summary:" << std::endl;
            std::cout << "  Total tests: " << total_tests << std::endl;
            std::cout << "  Passed: " << passed_tests << std::endl;
            std::cout << "  Failed: " << (total_tests - passed_tests) << std::endl;

            if (passed_tests == total_tests) {
                std::cout << "  ✓ All tests passed!" << std::endl;
            } else {
                std::cout << "  ✗ Some tests failed" << std::endl;
            }
            std::cout << std::endl;
        }

        bool all_passed() const {
            return passed_tests == total_tests;
        }

        int get_passed_count() const { return passed_tests; }

        int get_total_count() const { return total_tests; }
    };
}

#endif