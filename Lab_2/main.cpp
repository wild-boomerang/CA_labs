#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>
#include <string>

#include "DynamicQueue.h"
#include "MutexFixedSizeQueue.h"


void CheckArr(std::vector<int> &arr, int numThreads, long long workTime, const std::string& solutionName,
              bool sleep)
{
    int errorsQuantity = 0;
    std::vector<int> errInd;

    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] != 1) {
            errorsQuantity++;
            errInd.push_back(i);
        }
    }

    std::cout << solutionName + ((sleep) ? " with sleep" : " without sleep") << ":\n";
    std::cout << "Threads quantity = " << numThreads << ":\n";
    std::cout << "Work time: " << workTime << " ms\n";

    if (errorsQuantity == 0) {
        std::cout << "Everything is clear\n";
    } else {
        std::cout << "Founded errors!!!\n" << errorsQuantity << " errors at indices..." << std::endl;
        for (int i : errInd) {
            std::cout << "arr[" << i << "] = " << arr[i] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}

void ThreadWork(std::vector<int> &arr, int &counter, std::mutex &lock, bool sleep) {
    int index, size = arr.size();
    while (true) {
        lock.lock();
        index = counter++;
        lock.unlock();

        if (index >= size)
            break;

        arr[index]++;

        if (sleep) { std::this_thread::sleep_for(std::chrono::nanoseconds(10)); }
    }
}

void MutexSolution(const int numTasks, const std::vector<int> &numThreads, bool sleep) {
    for (auto numThread : numThreads) {
        std::vector<int> arr(numTasks, 0);
        std::vector<std::thread> threads;
        int counter = 0;
        std::mutex lock;

        auto start = std::chrono::high_resolution_clock::now();

        threads.reserve(numThread);
        for (int j = 0; j < numThread; j++) {
            threads.emplace_back(ThreadWork, std::ref(arr), std::ref(counter), std::ref(lock), sleep);
        }

        for (auto & thread : threads) {
            thread.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        CheckArr(arr, numThread, elapsed_seconds, "Mutex solution", sleep);
    }
}

void ThreadWorkAtomic(std::vector<int> &arr, std::atomic<int> &counter, bool sleep) {
    int index, size = arr.size();
    while (true) {
        index = counter++;

        if (index >= size)
            break;

        arr[index]++;

        if (sleep) { std::this_thread::sleep_for(std::chrono::nanoseconds(10)); }
    }
}

void AtomicSolution(const int numTasks, const std::vector<int> &numThreads, bool sleep)
{
    for (auto numThread : numThreads) {
        std::vector<int> arr(numTasks, 0);
        std::vector<std::thread> threads;
        std::atomic<int> counter(0);

        auto start = std::chrono::high_resolution_clock::now();

        threads.reserve(numThread);
        for (int j = 0; j < numThread; j++) {
            threads.emplace_back(ThreadWorkAtomic, std::ref(arr), std::ref(counter), sleep);
        }

        for (auto & thread : threads) {
            thread.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        CheckArr(arr, numThread, elapsed_seconds, "Atomic solution", sleep);
    }
}

void ExecuteTask_1(bool sleep)
{
    const int numTasks = 1024 * 1024;
    const std::vector<int> numThreads = {4, 8, 16, 32};

    MutexSolution(numTasks, numThreads, sleep);
    AtomicSolution(numTasks, numThreads, sleep);
}

void CheckTask2(std::vector<int> &counters, int producerNum, int consumerNum, int taskNum,
                long long duration, const std::string& about, int queueSize=0)
{
    int consumerAnswer = 0, producerAnswer = producerNum * taskNum;
    for (auto counter : counters) {
        consumerAnswer += counter;
    }

    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << std::ctime(&time)
              << about + ":\n"
              << ((queueSize) ? "Queue size = " + std::to_string(queueSize) + "\n" : "")
              << "ProducerNum = " << producerNum << ", " << "ConsumerNum = " << consumerNum << "\n"
              << "Duration: " << (double)duration / 1000 << " s\n";

    if (consumerAnswer == producerAnswer) {
        std::cout << "Everything is clear!\n\n";
    } else {
        std::cout << "Error!\n"
                  << consumerAnswer << "\n"
                  << producerAnswer << "\n\n";
    }
}


void ProducerWork(queue *queue, const int taskNum) {
    for (int i = 0; i < taskNum; i++) {
        queue->push(1);
    }
}

void ConsumerWork(queue *queue, int &counter) {
    uint8_t val;
    while (!queue->isDone()) {
        if (queue->pop(val)) { counter += val; }
    }
}

void Task2(const std::vector<int> &producerNum, const std::vector<int> &consumerNum, const int taskNum,
           std::vector<int> queueSize=std::vector<int>(), bool atomicQueue=false)
{
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);

    std::cout << "Started: " << std::ctime(&start_time) << "\n";

    if (queueSize.empty()) { queueSize.push_back(0); } // it means that it is a dynamic-sized queue
    for (auto curQueueSize : queueSize) {
        for (auto prNum : producerNum) {
            for (auto conNum : consumerNum) {
                queue *queue;
                std::string about;

                if (queueSize.front() == 0) {
                    queue = new DynamicQueue(taskNum * prNum);
                    about = "Dynamic-sized queue";
                } else {
                    if (!atomicQueue) {
                        queue = new MutexFixedSizeQueue(curQueueSize, taskNum * prNum);
                        about = "Mutex fixed size queue";
                    } else {
//                        queue = new AtomicFixedSizeQueue(curQueueSize);
                        about = "Atomic fixed size queue";
                        return;
                    }
                }

                std::vector<std::thread> producers;
                std::vector<std::thread> consumers;
                std::vector<int> counters(conNum, 0);

                auto cur_start = std::chrono::high_resolution_clock::now();

                producers.reserve(prNum);
                for (int i = 0; i < prNum; i++) {
                    producers.emplace_back(ProducerWork, queue, taskNum);
                }

                consumers.reserve(conNum);
                for (int i = 0; i < conNum; i++) {
                    consumers.emplace_back(ConsumerWork, queue, std::ref(counters[i]));
                }

                for (auto &p : producers) {
                    p.join();
                }

                for (auto &c : consumers) {
                    c.join();
                }

                auto cur_end = std::chrono::high_resolution_clock::now();
                auto workTime = std::chrono::duration_cast<std::chrono::milliseconds>(cur_end - cur_start).count();

                CheckTask2(counters, prNum, conNum, taskNum, workTime, about, curQueueSize);

                delete queue;
            }
        }
    }

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "Finished: " << std::ctime(&end_time)
              << "Elapsed time: " << elapsed_seconds.count() << " s = " << elapsed_seconds.count() / 60 << " min\n\n";
}

enum class Queue { dynamic, fixedMutex, fixedAtomic };

void ExecuteTask_2(Queue queueCase) {
    const std::vector<int> producerNum {1, 2, 4};
    const std::vector<int> consumerNum {1, 2, 4};
    const int taskNum = 4 * 1024 * 1024;
    std::vector<int> queueSize {1, 4, 16};

    switch (queueCase) {
        case Queue::dynamic:
            Task2(producerNum, consumerNum, taskNum);
            break;
        case Queue::fixedMutex:
            Task2(producerNum, consumerNum, taskNum, queueSize, false);
            break;
        case Queue::fixedAtomic:
            Task2(producerNum, consumerNum, taskNum, queueSize, true);
            break;
    }
}

void Menu() {
    int choice;

    do {
        std::cout << "Number of physical threads: " << std::thread::hardware_concurrency() << std::endl;
        std::cout << "What do you want?\n"
                  << "1. Task 1\n"
                  << "2. Task 2\n"
                  << "3. All tasks consistently\n"
                  << "0. Exit\n";
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "Task 1:\n"
                      << "1. Without sleep\n"
                      << "2. With sleep\n"
                      << "0. Exit\n";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    ExecuteTask_1(false);
                    break;
                }
                case 2: {
                    ExecuteTask_1(true);
                    break;
                }
                default:
                    break;
            }
        } else if (choice == 2) {
            choice = 0;
            std::cout << "Task 2:\n"
                      << "1. Dynamic-sized queue\n"
                      << "2. Mutex fixed size queue\n"
                      << "3. Atomic fixed size queue\n"
                      << "0. Exit\n";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    ExecuteTask_2(Queue::dynamic);
                    break;
                }
                case 2: {
                    ExecuteTask_2(Queue::fixedMutex);
                    break;
                }
                case 3: {
//                    ExecuteTask_2(Queue::fixedAtomic);
                    break;
                }
                default:
                    break;
            }
        } else if (choice == 3) {
            ExecuteTask_1(false);
            ExecuteTask_1(true);
            ExecuteTask_2(Queue::dynamic);
            ExecuteTask_2(Queue::fixedMutex);
//            ExecuteTask_2(Queue::fixedAtomic);
        }
    } while (choice != 0);
}

int main() {
    Menu();

    return 0;
}
