#include <iostream>
#include <chrono>
#include <mutex>
#include <atomic>

#include "ArrayCounterTask.h"


ArrayCounterTask::ArrayCounterTask(const int numTasks, std::vector<int> numThreads) : _numTasks(numTasks),
                                                                                      _numThreads(std::move(numThreads))
{

}

void ArrayCounterTask::Execute(CounterType workType, bool sleep) {
    std::string about;
    switch (workType) {
        case CounterType::mutex:
            about = "Mutex solution";
            break;
        case CounterType::atomic:
            about = "Atomic solution";
            break;
    }

    for (auto numThread : _numThreads) {
        std::vector<int> arr(_numTasks, 0);
        std::vector<std::thread> threads;
        int mutexCounter = 0;
        std::atomic<int> atomicCounter(0);

        auto start = std::chrono::high_resolution_clock::now();

        threads.reserve(numThread);
        for (int j = 0; j < numThread; j++) {
            switch (workType) {
                case CounterType::mutex:
                    threads.emplace_back(&ArrayCounterTask::ThreadWork<int>, this, workType, std::ref(arr),
                                         std::ref(mutexCounter), sleep);
                    break;
                case CounterType::atomic:
                    threads.emplace_back(&ArrayCounterTask::ThreadWork<std::atomic<int>>, this, workType,
                                         std::ref(arr), std::ref(atomicCounter), sleep);
                    break;
            }
        }

        for (auto & thread : threads) {
            thread.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        int errorsNum;
        std::vector<int> errInd;
        bool check = ArrayCounterTask::Verification(arr, errorsNum, errInd);
        PrintResults(check, numThread, elapsed_seconds, sleep, about, arr, errorsNum, errInd);
    }
}

template<class T>
void ArrayCounterTask::ThreadWork(CounterType workType, std::vector<int> &arr, T &counter, bool sleep) {
    int index, size = arr.size();
    while (true) {
        if (workType == CounterType::mutex) { _lock.lock(); }
        index = counter++;
        if (workType == CounterType::mutex) { _lock.unlock(); }

        if (index >= size)
            break;

        arr[index]++;

        if (sleep) { std::this_thread::sleep_for(std::chrono::nanoseconds(10)); }
    }
}

bool ArrayCounterTask::Verification(std::vector<int> &arr, int &errorsNum, std::vector<int> &errInd)
{
    errorsNum = 0;

    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] != 1) {
            errorsNum++;
            errInd.push_back(i);
        }
    }

    return errorsNum == 0;
}

void ArrayCounterTask::PrintResults(bool isCorrect, int numThreads, long long workTime, bool sleep,
                                    const std::string& solutionName, std::vector<int> &arr, int errorsNum,
                                    std::vector<int> &errInd) {
    std::cout << solutionName + ((sleep) ? " with sleep" : " without sleep") << ":\n";
    std::cout << "Threads quantity = " << numThreads << ":\n";
    std::cout << "Work time: " << workTime << " ms\n";

    if (isCorrect) {
        std::cout << "Everything is clear\n";
    } else {
        std::cerr << "Founded errors!!!\n" << errorsNum << " errors at indices..." << std::endl;
        for (int i : errInd) {
            std::cerr << "arr[" << i << "] = " << arr[i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
