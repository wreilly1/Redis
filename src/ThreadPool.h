#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stdexcept>

// A simple thread pool implementation.
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Enqueue a new task into the thread pool.
    // This template function is defined inline here.
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        // Create a packaged task that wraps the function and its arguments.
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            // Add the task to the queue.
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

private:
    // Worker threads.
    std::vector<std::thread> workers;
    // Task queue.
    std::queue<std::function<void()>> tasks;

    // Synchronization.
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};
