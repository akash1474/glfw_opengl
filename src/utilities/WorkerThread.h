#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <stdexcept>
#include <tuple> // Required for std::apply
#include <type_traits> // Required for std::invoke_result_t and std::is_void_v

class WorkerThread
{
public:
    /**
     * @brief Get the singleton instance of the WorkerThread.
     */
    static WorkerThread& GetInstance()
    {
        // Use a sensible default, but not necessarily all cores.
        // For I/O bound tasks, more can be better. For CPU bound, hardware_concurrency is good.
        static WorkerThread instance(std::max(1u, std::thread::hardware_concurrency()));
        return instance;
    }

    /**
     * @brief Enqueues a task and returns a future to get the result.
     */
    template <class F, class... Args>
    static auto Enqueue(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        return GetInstance().enqueueImpl(
            std::forward<F>(f),
            std::forward<Args>(args)...
        );
    }

    /**
     * @brief Enqueues a task with a callback to be executed upon completion.
     * The callback receives the result of the task as its argument.
     */
    template <class F, class Cb, class... Args>
    static void EnqueueWithCallback(F&& taskFunc, Cb&& callbackFunc, Args&&... args)
    {
        GetInstance().enqueueWithCallbackImpl(
            std::forward<F>(taskFunc),
            std::forward<Cb>(callbackFunc),
            std::forward<Args>(args)...
        );
    }

    /**
     * @brief Destructor that stops all threads and joins them.
     */
    ~WorkerThread()
    {
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mStop = true;
        }
        mCondition.notify_all();
        for (std::thread& worker : mWorkers)
        {
            if (worker.joinable())
                worker.join();
        }
    }

private:
    WorkerThread(size_t threads) : mStop(false)
    {
        if (threads == 0)
            throw std::invalid_argument("Thread pool size must be greater than 0.");

        for (size_t i = 0; i < threads; ++i)
        {
            mWorkers.emplace_back([this] {
                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mQueueMutex);
                        mCondition.wait(lock, [this] {
                            return mStop || !mTasks.empty();
                        });
                        if (mStop && mTasks.empty())
                            return;
                        task = std::move(mTasks.front());
                        mTasks.pop();
                    }
                    task();
                }
            });
        }
    }

    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator=(const WorkerThread&) = delete;

    // Implementation for future-based tasks
    template <class F, class... Args>
    auto enqueueImpl(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>
    {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            if (mStop)
                throw std::runtime_error("enqueue on stopped WorkerThread");
            mTasks.emplace([task]() { (*task)(); });
        }
        mCondition.notify_one();
        return res;
    }

    // NEW: Implementation for callback-based tasks
    template <class F, class Cb, class... Args>
    void enqueueWithCallbackImpl(F&& taskFunc, Cb&& callbackFunc, Args&&... args)
    {
        // Capture arguments in a tuple to pass to std::apply
        auto argsTuple = std::make_tuple(std::forward<Args>(args)...);
        
        // Create a wrapper function that executes the task and then the callback
        auto workAndCallback = [
                task = std::forward<F>(taskFunc),
                callback = std::forward<Cb>(callbackFunc),
                argsTuple
            ]() mutable {
            
            // Use if constexpr to handle tasks with a void return type differently
            using return_type = decltype(std::apply(task, argsTuple));

            if constexpr (std::is_void_v<return_type>)
            {
                // Task returns void, so callback takes no arguments
                std::apply(task, argsTuple);
                callback();
            }
            else
            {
                // Task returns a value, so pass it to the callback
                auto result = std::apply(task, argsTuple);
                callback(std::move(result));
            }
        };
        
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            if (mStop)
                throw std::runtime_error("enqueue on stopped WorkerThread");
                
            // Add the combined task to the queue
            mTasks.emplace(std::move(workAndCallback));
        }
        mCondition.notify_one();
    }

    std::vector<std::thread> mWorkers;
    std::queue<std::function<void()>> mTasks;

    std::mutex mQueueMutex;
    std::condition_variable mCondition;
    bool mStop;
};