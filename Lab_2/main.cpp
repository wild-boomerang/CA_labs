#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
//#include <future>
//#include <condition_variable>
#include <vector>
#include <chrono>
#include <ctime>
#include <string>

#include "DynamicQueue.h"
#include "MutexFixedSizeQueue.h"


char* InitializeMy()
{
    int numTasks = 1024 * 1024;
    char *arr = new char [numTasks];
    for (int i = 0; i < numTasks; i++)
    {
        arr[i] = 0;
    }

    return arr;
}

void CheckArr(std::vector<int> &arr, int numThreads, unsigned int workTime, const std::string& solutionName)
{
    int errorsQuantity = 0;
    std::vector<int> errInd;

    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] != 1)
        {
            errorsQuantity++;
            errInd.push_back(i);
        }
    }

    std::cout << solutionName << ":\n";
    std::cout << "Threads quantity = " << numThreads << ":\n";
    std::cout << "Work time: " << workTime << " ms\n";

    if (errorsQuantity == 0)
    {
        std::cout << "Everything is clear\n";
    } else
    {
        std::cout << "Founded errors!!!\n" << errorsQuantity << " errors at indices..." << std::endl;
        for (int i : errInd)
        {
            std::cout << "arr[" << i << "] = " << arr[i] << " ";
        }
        std::cout << std::endl;
//        std::cout << "Array size = " << arr.size() << "\nCounter = " << counter;
    }

    std::cout << std::endl;
}

void ThreadWork(std::vector<int> &arr, int &counter, std::mutex &lock, bool sleep)
{
    int index, size = arr.size();
    while (true)
    {
        lock.lock();
        index = counter++;
        lock.unlock();

        if (index >= size)
            break;

        arr[index]++;

        if (sleep) { std::this_thread::sleep_for(std::chrono::nanoseconds(10)); }
    }
}

void MutexSolution(const int numTasks, const std::vector<int> &numThreads, bool sleep)
{
//    for (int i = 0; i < 100; i++)
//    {
        for (auto numThread : numThreads)
        {
            std::vector<int> arr(numTasks, 0);
            std::vector<std::thread> threads;
            int counter = 0;
            std::mutex lock;

            unsigned int start = clock();
            auto start2 = std::chrono::high_resolution_clock::now();

            for (int j = 0; j < numThread; j++)
            {
                std::thread thread(ThreadWork, std::ref(arr), std::ref(counter), std::ref(lock), sleep);
                threads.push_back(std::move(thread));
            }

            for (auto & thread : threads)
            {
                thread.join();
            }

            unsigned int end = clock();
            auto end2 = std::chrono::high_resolution_clock::now();
            auto res = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

//            std::cout << "\nTIME: " << end - start << "\t" << res << std::endl;

            CheckArr(arr, numThread, res, "Mutex solution");
        }
//    }
}

void ThreadWorkAtomic(std::vector<int> &arr, std::atomic<int> &counter, bool sleep)
{
    int index, size = arr.size();
    while (true)
    {
        index = counter++;

        if (index >= size)
            break;

        arr[index]++;

        if (sleep) { std::this_thread::sleep_for(std::chrono::nanoseconds(10)); }
    }
}

void AtomicSolution(const int numTasks, const std::vector<int> &numThreads, bool sleep)
{
//    for (int i = 0; i < 100; i++)
//    {
        for (auto numThread : numThreads)
        {
            std::vector<int> arr(numTasks, 0);
            std::vector<std::thread> threads;
            std::atomic<int> counter(0);

            unsigned int start = clock();

            for (int j = 0; j < numThread; j++)
            {
                std::thread thread(ThreadWorkAtomic, std::ref(arr), std::ref(counter), sleep);
                threads.push_back(std::move(thread));
            }

            for (auto & thread : threads)
            {
                thread.join();
            }

            unsigned int end = clock();

            CheckArr(arr, numThread, (end - start), "Atomic solution");
        }
//    }
}

void Menu()
{
    int choice;

    std::cout << "What do you want?\n"
              << "1. Without sleep\n"
              << "2. With sleep\n";
    std::cin >> choice;

}

void CheckTask2(std::vector<int> &counters, int producerNum, int consumerNum, int taskNum, queue *queue,
        unsigned int duration, const std::string& about, int queueSize=0)
{
    int consumerAnswer = 0, producerAnswer = producerNum * taskNum;
    for (auto counter : counters)
    {
        consumerAnswer += counter;
    }

    std::cout << about + ":\n"
              << ((queueSize) ? "Queue size = " + std::to_string(queueSize) + "\n" : "")
              << "ProducerNum = " << producerNum << ", " << "ConsumerNum = " << consumerNum << "\n"
              << "Duration: " << duration << " ms\n";

    uint8_t val;
    int quantity = 0;
    while (queue->pop(val))
    {
        quantity += val;
    }

    if (consumerAnswer == producerAnswer)
    {
        std::cout << "Everything is clear!\n\n";
    } else
    {
        std::cout << "Error!\n"
                  << (((quantity + consumerAnswer) == producerAnswer) ? "true\n" : "false\n")
                  << consumerAnswer << "\n"
                  << producerAnswer << "\n\n";
    }
}


void ProducerWork(queue *queue, const int taskNum)
{
    for (int i = 0; i < taskNum; i++)
    {
        queue->push(1);
    }
}

void ConsumerWork(queue *queue, int &counter)
{
    uint8_t val;
    while (queue->pop(val))
    {
        counter += val;
    }
}

void Task2(const std::vector<int> &producerNum, const std::vector<int> &consumerNum, const int taskNum,
           std::vector<int> queueSize=std::vector<int>(), bool atomicQueue=false)
{
    if (queueSize.empty()) { queueSize.push_back(0); }
    for (auto curQueueSize : queueSize)
    {
        for (auto prNum : producerNum)
        {
            for (auto conNum : consumerNum)
            {
                queue *queue;
                std::string about;

                if (queueSize.front() == 0)
                {
                    queue = new DynamicQueue();
                    about = "Dynamic-sized queue";
                } else
                {
                    if (!atomicQueue)
                    {
                        queue = new MutexFixedSizeQueue(curQueueSize);
                        about = "Mutex fixed size queue";
                    } else
                    {
//                        queue = new AtomicFixedSizeQueue(curQueueSize);
                        about = "Atomic fixed size queue";
                    }
                }

//                DynamicQueue queue;

                std::vector<std::thread> producers;
                std::vector<std::thread> consumers;
                std::vector<int> counters(conNum, 0);

                unsigned int start = clock();
                for (int i = 0; i < prNum; i++)
                {
//                    std::thread prod(ProducerWork, std::ref(queue), taskNum);
                    std::thread prod(ProducerWork, queue, taskNum);
                    producers.push_back(std::move(prod));
                }

                for (int i = 0; i < conNum; i++)
                {
//                    std::thread consumer(ConsumerWork, std::ref(queue), std::ref(counters[i]));
                    std::thread consumer(ConsumerWork, queue, std::ref(counters[i]));
                    consumers.push_back(std::move(consumer));
                }

                for (auto &p : producers)
                {
                    p.join();
                }

                for (auto &c : consumers)
                {
                    c.join();
                }

                unsigned int end = clock();

                CheckTask2(counters, prNum, conNum, taskNum, queue, end - start, about, curQueueSize);

                delete queue;
            }
        }
    }
}

int main()
{
    int choice;

    do
    {
        std::cout << "Number of physical threads: " << std::thread::hardware_concurrency() << std::endl;
        std::cout << "What do you want?\n"
                  << "1. Task 1\n"
                  << "2. Task 2\n"
                  << "0. Exit\n";
        std::cin >> choice;

        if (choice == 1)
        {
//            char *arr = InitializeMy();
            const int numTasks = 1024 * 1024;
            const std::vector<int> numThreads = {4, 8, 16, 32};

            std::cout << "Task 1:\n"
                      << "1. Without sleep\n"
                      << "2. With sleep\n"
                      << "0. Exit\n";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    MutexSolution(numTasks, numThreads, false);
                    AtomicSolution(numTasks, numThreads, false);
                    break;
                }
                case 2: {
                    MutexSolution(numTasks, numThreads, true);
                    AtomicSolution(numTasks, numThreads, true);
                    break;
                }
                default:
                    break;
            }
        } else if (choice == 2)
        {
            choice = 0;
            const std::vector<int> producerNum {1, 2, 4};
            const std::vector<int> consumerNum {1, 2, 4};
            const int taskNum = 4 * 1024 * 1024;
            std::vector<int> queueSize {1, 4, 16};

            std::cout << "Task 2:\n"
                      << "1. Dynamic-sized queue\n"
                      << "2. Mutex fixed size queue\n"
                      << "3. Atomic fixed size queue\n"
                      << "0. Exit\n";
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    Task2(producerNum, consumerNum, taskNum);
                    break;
                }
                case 2: {
                    Task2(producerNum, consumerNum, taskNum, queueSize);
                    break;
                }
                default:
                    break;
            }
        }

    } while (choice != 0);

    return 0;
}
