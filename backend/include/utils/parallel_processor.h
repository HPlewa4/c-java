#ifndef PARALLEL_PROCESSOR_H
#define PARALLEL_PROCESSOR_H

#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include <atomic>
#include <iostream>

namespace ParallelProcessing {
    class ThreadPool {
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queue_mutex;
        std::condition_variable condition;
        std::atomic<bool> stop;
        size_t num_threads;

    public:
        explicit ThreadPool(size_t threads = std::thread::hardware_concurrency())
            : stop(false), num_threads(threads) {

            std::cout << "Creating thread pool with " << threads << " threads" << std::endl;

            for (size_t i = 0; i < threads; ++i) {
                workers.emplace_back([this, i] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock, [this] {
                                return this->stop.load() || !this->tasks.empty();
                            });

                            if (this->stop.load() && this->tasks.empty()) {
                                return;
                            }

                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }
                        task();
                    }
                });
            }
        }

        ~ThreadPool() {
            stop.store(true);
            condition.notify_all();

            for (std::thread& worker : workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }
        }

        template<typename F, typename... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
            using return_type = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<return_type> result = task->get_future();
            {
                std::unique_lock<std::mutex> lock(queue_mutex);

                if (stop.load()) {
                    throw std::runtime_error("Cannot enqueue on stopped ThreadPool");
                }

                tasks.emplace([task]() { (*task)(); });
            }
            condition.notify_one();
            return result;
        }

        size_t size() const { return num_threads; }
    };

    template <typename InputType, typename OutputType>
    class ParallelBatchProcessor {
    private:
        ThreadPool pool;
        size_t batch_size;

    public:
        explicit ParallelBatchProcessor(size_t num_threads = std::thread::hardware_concurrency(),
                                       size_t batch = 8)
            : pool(num_threads), batch_size(batch) {}

        std::vector<OutputType> process_batch(
            const std::vector<InputType>& inputs,
            std::function<OutputType(const InputType&)> process_func) {

            std::vector<std::future<OutputType>> futures;
            futures.reserve(inputs.size());

            for (const auto& input : inputs) {
                futures.push_back(pool.enqueue(process_func, std::cref(input)));
            }

            std::vector<OutputType> results;
            results.reserve(inputs.size());

            for (auto& future : futures) {
                results.push_back(future.get());
            }

            return results;
        }

        std::vector<OutputType> process_indexed(
            const std::vector<InputType>& inputs,
            std::function<OutputType(const InputType&, size_t)> process_func) {

            std::vector<std::future<OutputType>> futures;
            futures.reserve(inputs.size());

            for (size_t i = 0; i < inputs.size(); ++i) {
                futures.push_back(pool.enqueue(
                    [&inputs, i, &process_func]() {
                        return process_func(inputs[i], i);
                    }
                ));
            }

            std::vector<OutputType> results;
            results.reserve(inputs.size());

            for (auto& future : futures) {
                results.push_back(future.get());
            }

            return results;
        }

        template <typename MapFunc>
        auto parallel_map(const std::vector<InputType>& inputs, MapFunc func)
            -> std::vector<decltype(func(inputs[0]))> {

            using result_type = decltype(func(inputs[0]));
            std::vector<std::future<result_type>> futures;
            futures.reserve(inputs.size());

            for (const auto& input : inputs) {
                futures.push_back(pool.enqueue(func, std::cref(input)));
            }

            std::vector<result_type> results;
            results.reserve(inputs.size());

            for (auto& future : futures) {
                results.push_back(future.get());
            }

            return results;
        }
    };

    inline void parallel_for(size_t start, size_t end,
                            std::function<void(size_t)> func,
                            size_t num_threads = std::thread::hardware_concurrency()) {

        if (end <= start) return;

        size_t range = end - start;
        size_t chunk_size = (range + num_threads - 1) / num_threads;

        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        for (size_t t = 0; t < num_threads; ++t) {
            size_t chunk_start = start + t * chunk_size;
            size_t chunk_end = std::min(chunk_start + chunk_size, end);

            if (chunk_start >= end) break;

            threads.emplace_back([chunk_start, chunk_end, &func]() {
                for (size_t i = chunk_start; i < chunk_end; ++i) {
                    func(i);
                }
            });
        }

        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    template <typename T>
    T parallel_reduce(const std::vector<T>& data,
                     T init,
                     std::function<T(T, T)> reduce_func,
                     size_t num_threads = std::thread::hardware_concurrency()) {

        if (data.empty()) return init;
        if (data.size() == 1) return reduce_func(init, data[0]);

        size_t chunk_size = (data.size() + num_threads - 1) / num_threads;
        std::vector<std::thread> threads;
        std::vector<T> partial_results(num_threads, init);

        for (size_t t = 0; t < num_threads; ++t) {
            size_t start = t * chunk_size;
            size_t end = std::min(start + chunk_size, data.size());

            if (start >= data.size()) break;

            threads.emplace_back([&data, start, end, &partial_results, t, init, &reduce_func]() {
                T local_result = init;
                for (size_t i = start; i < end; ++i) {
                    local_result = reduce_func(local_result, data[i]);
                }
                partial_results[t] = local_result;
            });
        }

        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        T final_result = init;
        for (const auto& partial : partial_results) {
            final_result = reduce_func(final_result, partial);
        }

        return final_result;
    }
}

#endif