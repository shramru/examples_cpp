//
// Created by vladislav on 13.06.16.
//

#ifndef PROJECT_THREADPOOL_HPP
#define PROJECT_THREADPOOL_HPP

#include <map>
#include <queue>
#include <future>
#include <algorithm>

class ThreadPool {
    std::map<std::thread::id, std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
    size_t initSize, maxSize;
    std::atomic<size_t> busy;

    void add_worker(size_t count = 1);

public:
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args);
    size_t size() const;

    ThreadPool(size_t threadsMin, size_t threadsMax);
    ~ThreadPool();
};

#include "../src/threadpool.tcc"
#endif //PROJECT_THREADPOOL_HPP
