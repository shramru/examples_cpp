//
// Created by vladislav on 13.06.16.
//

#ifndef PROJECT_THREADPOOL_HPP
#error Include threadpool.hpp instead of this
#endif

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) {
    using return_type = decltype(f(args...));

    auto task = std::make_shared<std::packaged_task<return_type ()>> (
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    auto res = std::shared_future<return_type>(task->get_future());

    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if(stop)
            throw std::runtime_error("Enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });

        if (size() < maxSize && size() - busy == 0)
            add_worker();
    }

    condition.notify_one();
    return res;
}