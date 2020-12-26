//
// Created by Boom on 27.11.2020.
//

#include "TaskExecutor.h"
#include "ArrayCounterTask.h"

void TaskExecutor::ExecuteTask_1(bool sleep) {
    const int numTasks = 1024 * 1024;
    const std::vector<int> numThreads = {4, 8, 16, 32};

    ArrayCounterTask arrayCounterTask(numTasks, numThreads);
    arrayCounterTask.Execute(CounterType::mutex, sleep);
    arrayCounterTask.Execute(CounterType::atomic, sleep);
}

void TaskExecutor::ExecuteTask_2(QueueType queueCase) {
    const std::vector<int> producerNum {1, 2, 4};
    const std::vector<int> consumerNum {1, 2, 4};
    const int taskNum = 4 * 1024 * 1024;
    std::vector<int> queueSize {1, 4, 16};
    ConsumerEndImplementation consumerEndImpl = ConsumerEndImplementation::elementsNum;

    QueueTask queueTask(producerNum, consumerNum, taskNum, queueSize, consumerEndImpl);

    switch (queueCase) {
        case QueueType::dynamic:
            queueTask.Execute(QueueType::dynamic);
            break;
        case QueueType::fixedMutex:
            queueTask.Execute(QueueType::fixedMutex);
            break;
        case QueueType::fixedAtomic:
            queueTask.Execute(QueueType::fixedAtomic);
            break;
    }
}