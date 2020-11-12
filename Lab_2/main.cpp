#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
//#include <future>
//#include <condition_variable>
#include <vector>
#include <array>
#include <chrono>
#include <ctime>
#include <string>


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

void CheckArr(std::vector<int> &arr, int numThreads, unsigned int workTime, std::string solutionName)
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
    std::cout << "Work time: " << workTime << std::endl;

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

    std::cout << "What do you want?\n" << "1. Without sleep\n" << "2. With sleep\n";
    std::cin >> choice;

}



int main()
{
//    char *arr = InitializeMy();
    const int numTasks = 1024 * 1024;
    std::vector<int> const numThreads = {4, 8, 16, 32};

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
            std::cout << "Task 2:\n"
                      << "1. Dynamic-sized queue\n"
                      << "2. Mutex fixed size queue\n"
                      << "3. Atomic fixed size queue\n"
                      << "0. Exit\n";
            std::cin >> choice;

            switch (choice) {
                case 1: {

                    break;
                }
                case 2: {

                    break;
                }
                default:
                    break;
            }
        }

    } while (choice != 0);

    return 0;
}
