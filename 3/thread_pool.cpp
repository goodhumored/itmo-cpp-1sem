#include "thread_pool.h"

ThreadPool::ThreadPool(size_t num_threads) {
    workers_.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(&ThreadPool::worker_thread, this);
    }
}

ThreadPool::~ThreadPool() {
    while (pending_tasks_.load(std::memory_order_acquire) > 0) {
        std::this_thread::yield();
    }
    
    stop_.store(true, std::memory_order_release);
    
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    Task* task = head_.load(std::memory_order_relaxed);
    while (task) {
        Task* next = task->next;
        delete task;
        task = next;
    }
}

void ThreadPool::worker_thread() {
    while (true) {
        Task* task = pop_task();
        
        if (task) {
            task->func();
            delete task;
            pending_tasks_.fetch_sub(1, std::memory_order_release);
        } else if (stop_.load(std::memory_order_acquire)) {
            break;
        } else {
            std::this_thread::yield();
        }
    }
}

void ThreadPool::push_task(std::function<void()> func) {
    Task* new_task = new Task{std::move(func), nullptr};
    
    pending_tasks_.fetch_add(1, std::memory_order_release);
    
    queue_lock_.lock();
    
    Task* old_tail = tail_.load(std::memory_order_relaxed);
    if (old_tail) {
        old_tail->next = new_task;
    } else {
        head_.store(new_task, std::memory_order_relaxed);
    }
    tail_.store(new_task, std::memory_order_relaxed);
    
    queue_lock_.unlock();
}

ThreadPool::Task* ThreadPool::pop_task() {
    queue_lock_.lock();
    
    Task* task = head_.load(std::memory_order_relaxed);
    if (task) {
        head_.store(task->next, std::memory_order_relaxed);
        if (!task->next) {
            tail_.store(nullptr, std::memory_order_relaxed);
        }
    }
    
    queue_lock_.unlock();
    
    return task;
}
