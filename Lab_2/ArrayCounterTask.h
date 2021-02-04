#ifndef LAB_2_ARRAYCOUNTERTASK_H
#define LAB_2_ARRAYCOUNTERTASK_H


#include <vector>
#include <thread>
#include <mutex>


enum class CounterType { mutex, atomic };


class ArrayCounterTask {
private:
    const int _numTasks;
    const std::vector<int> _numThreads;
    std::mutex _lock;

    template<class T>
    void ThreadWork(CounterType workType, std::vector<int> &arr, T &counter, bool sleep);
    static bool Verification(std::vector<int> &arr, int &errorsNum, std::vector<int> &errInd);
    static void PrintResults(bool isCorrect, int numThreads, long long workTime, bool sleep,
                             const std::string& solutionName, std::vector<int> &arr, int errorsNum,
                             std::vector<int> &errInd);
public:
    ArrayCounterTask(int numTasks, std::vector<int>  numThreads);

    void Execute(CounterType workType, bool sleep);
};


#endif //LAB_2_ARRAYCOUNTERTASK_H
