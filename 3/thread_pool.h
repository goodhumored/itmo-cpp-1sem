#pragma once

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <vector>

class SpinLock {
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            while (flag_.test(std::memory_order_relaxed)) {
                std::this_thread::yield();
            }
        }
    }

    void unlock() {
        flag_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template<typename Fn>
    auto submit(Fn&& f) -> std::future<decltype(f())> {
        using ReturnType = decltype(f());
        
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::forward<Fn>(f));
        std::future<ReturnType> future = task->get_future();
        
        auto wrapper = [task]() { (*task)(); };
        
        push_task(std::move(wrapper));
        
        return future;
    }

private:
    struct Task {
        std::function<void()> func;
        Task* next = nullptr;
    };

    void worker_thread();
    void push_task(std::function<void()> func);
    Task* pop_task();

    std::vector<std::thread> workers_;
    
    std::atomic<Task*> head_{nullptr};
    std::atomic<Task*> tail_{nullptr};
    SpinLock queue_lock_;
    
    std::atomic<bool> stop_{false};
    std::atomic<size_t> pending_tasks_{0};
};
