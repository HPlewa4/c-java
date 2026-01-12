#include "utils/test_framework.h"
#include "utils/parallel_processor.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <numeric>
#include <cmath>

using namespace ParallelProcessing;

void test_thread_pool_creation() {
    TestFramework::TestSuite suite("Thread Pool Creation");

    ThreadPool pool(4);
    suite.assert_equal(size_t(4), pool.size(), "Thread pool has correct size");

    ThreadPool pool2;
    size_t hw_threads = std::thread::hardware_concurrency();
    suite.assert_equal(hw_threads, pool2.size(), "Thread pool uses hardware concurrency");

    suite.print_summary();
}

void test_thread_pool_basic_tasks() {
    TestFramework::TestSuite suite("Thread Pool Basic Tasks");

    ThreadPool pool(4);

    auto future = pool.enqueue([]() { return 42; });
    int result = future.get();
    suite.assert_equal(42, result, "Thread pool executes simple task");

    auto future2 = pool.enqueue([](int a, int b) { return a + b; }, 10, 20);
    result = future2.get();
    suite.assert_equal(30, result, "Thread pool executes task with arguments");

    suite.print_summary();
}

void test_thread_pool_multiple_tasks() {
    TestFramework::TestSuite suite("Thread Pool Multiple Tasks");

    ThreadPool pool(4);
    std::vector<std::future<int>> futures;

    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.enqueue([](int x) { return x * x; }, i));
    }

    bool all_correct = true;
    for (int i = 0; i < 10; ++i) {
        int result = futures[i].get();
        if (result != i * i) {
            all_correct = false;
            break;
        }
    }

    suite.assert_true(all_correct, "Thread pool executes multiple tasks correctly");

    suite.print_summary();
}

void test_parallel_batch_processor() {
    TestFramework::TestSuite suite("Parallel Batch Processor");

    ParallelBatchProcessor<int, int> processor(4);

    std::vector<int> inputs = {1, 2, 3, 4, 5, 6, 7, 8};

    auto results = processor.process_batch(inputs, [](const int& x) {
        return x * x;
    });

    suite.assert_equal(size_t(8), results.size(), "Batch processor returns correct number of results");
    suite.assert_equal(1, results[0], "Result[0] = 1^2");
    suite.assert_equal(4, results[1], "Result[1] = 2^2");
    suite.assert_equal(64, results[7], "Result[7] = 8^2");

    suite.print_summary();
}

void test_parallel_batch_processor_with_index() {
    TestFramework::TestSuite suite("Parallel Batch Processor with Index");

    ParallelBatchProcessor<int, int> processor(4);

    std::vector<int> inputs = {10, 20, 30, 40};

    auto results = processor.process_indexed(inputs,
        [](const int& value, size_t index) {
            return value + static_cast<int>(index);
        });

    suite.assert_equal(10, results[0], "Result[0] = 10 + 0");
    suite.assert_equal(21, results[1], "Result[1] = 20 + 1");
    suite.assert_equal(32, results[2], "Result[2] = 30 + 2");
    suite.assert_equal(43, results[3], "Result[3] = 40 + 3");

    suite.print_summary();
}

void test_parallel_map() {
    TestFramework::TestSuite suite("Parallel Map Operation");

    ParallelBatchProcessor<double, double> processor(4);

    std::vector<double> inputs = {1.0, 2.0, 3.0, 4.0};

    auto results = processor.parallel_map(inputs, [](const double& x) {
        return std::sqrt(x);
    });

    suite.assert_near(1.0, results[0], 1e-9, "sqrt(1.0)");
    suite.assert_near(1.414213, results[1], 1e-5, "sqrt(2.0)");
    suite.assert_near(1.732050, results[2], 1e-5, "sqrt(3.0)");
    suite.assert_near(2.0, results[3], 1e-9, "sqrt(4.0)");

    suite.print_summary();
}

void test_parallel_for() {
    TestFramework::TestSuite suite("Parallel For Loop");

    std::vector<int> data(100, 0);
    std::mutex mutex;

    parallel_for(0, data.size(), [&data, &mutex](size_t i) {
        std::lock_guard<std::mutex> lock(mutex);
        data[i] = i * 2;
    }, 4);

    bool all_correct = true;
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] != static_cast<int>(i * 2)) {
            all_correct = false;
            break;
        }
    }

    suite.assert_true(all_correct, "Parallel for loop processes all elements correctly");

    suite.print_summary();
}

void test_parallel_reduce() {
    TestFramework::TestSuite suite("Parallel Reduce Operation");

    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    int sum = parallel_reduce<int>(data, 0, [](int a, int b) {
        return a + b;
    }, 4);

    suite.assert_equal(55, sum, "Parallel reduce computes sum correctly");

    int product = parallel_reduce<int>(
        std::vector<int>{1, 2, 3, 4, 5}, 1, [](int a, int b) {
            return a * b;
        }, 2);

    suite.assert_equal(120, product, "Parallel reduce computes product correctly");

    suite.print_summary();
}

void test_parallel_performance() {
    TestFramework::TestSuite suite("Parallel Processing Performance");

    const size_t data_size = 1000;
    std::vector<double> data(data_size);

    for (size_t i = 0; i < data_size; ++i) {
        data[i] = static_cast<double>(i);
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<double> seq_results;
    for (const auto& val : data) {
        double result = 0;
        for (int j = 0; j < 1000; ++j) {
            result += std::sin(val) * std::cos(val);
        }
        seq_results.push_back(result);
    }
    auto seq_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start).count();

    ParallelBatchProcessor<double, double> processor(4);
    start = std::chrono::high_resolution_clock::now();
    auto par_results = processor.parallel_map(data, [](const double& val) {
        double result = 0;
        for (int j = 0; j < 1000; ++j) {
            result += std::sin(val) * std::cos(val);
        }
        return result;
    });
    auto par_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "  Sequential time: " << seq_time << "ms" << std::endl;
    std::cout << "  Parallel time: " << par_time << "ms" << std::endl;

    if (par_time > 0) {
        double speedup = static_cast<double>(seq_time) / par_time;
        std::cout << "  Speedup: " << speedup << "x" << std::endl;
        suite.assert_true(speedup > 1.0, "Parallel processing is faster than sequential");
    } else {
        suite.assert_true(true, "Parallel processing completed");
    }

    suite.assert_equal(seq_results.size(), par_results.size(),
                      "Parallel and sequential produce same number of results");

    suite.print_summary();
}

void test_thread_safety() {
    TestFramework::TestSuite suite("Thread Safety");

    ThreadPool pool(8);
    std::mutex mutex;
    int counter = 0;

    std::vector<std::future<void>> futures;

    for (int i = 0; i < 100; ++i) {
        futures.push_back(pool.enqueue([&counter, &mutex]() {
            std::lock_guard<std::mutex> lock(mutex);
            counter++;
        }));
    }

    for (auto& future : futures) {
        future.get();
    }

    suite.assert_equal(100, counter, "Thread-safe counter increment");

    suite.print_summary();
}

int main() {
    std::cout << "================================" << std::endl;
    std::cout << "  Parallel Processing Tests" << std::endl;
    std::cout << "  Using std::thread" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

    test_thread_pool_creation();
    test_thread_pool_basic_tasks();
    test_thread_pool_multiple_tasks();
    test_parallel_batch_processor();
    test_parallel_batch_processor_with_index();
    test_parallel_map();
    test_parallel_for();
    test_parallel_reduce();
    test_parallel_performance();
    test_thread_safety();

    std::cout << "================================" << std::endl;
    std::cout << "  All Parallel Tests Complete!" << std::endl;
    std::cout << "================================" << std::endl;

    return 0;
}