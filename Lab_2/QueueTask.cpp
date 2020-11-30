//
// Created by Boom on 27.11.2020.
//

#include "QueueTask.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "DynamicQueue.h"
#include "MutexFixedSizeQueue.h"


QueueTask::QueueTask(std::vector<int> producerNum, std::vector<int> consumerNum, const int taskNum,
                     std::vector<int> queueSize) : _producerNum(std::move(producerNum)),
                                                   _consumerNum(std::move(consumerNum)),
                                                   _taskNum(taskNum),
                                                   _queueSize(std::move(queueSize))
{

}

void QueueTask::Execute(QueueType queueType)
{
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);

    std::cout << "Started: " << std::ctime(&start_time) << "\n";

    if (queueType == QueueType::dynamic) {
        RunWitGivenQueueSize(queueType);
    } else {
        for (auto curQueueSize : _queueSize) {
            RunWitGivenQueueSize(queueType, curQueueSize);
        }
    }

    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "Finished: " << std::ctime(&end_time)
              << "Elapsed time: " << elapsed_seconds.count() << " s = " << elapsed_seconds.count() / 60 << " min\n\n";
}

void QueueTask::RunWitGivenQueueSize(QueueType queueType, int queueSize)
{
    for (auto prNum : _producerNum) {
        for (auto conNum : _consumerNum) {
            queue *queue;
            std::string about;

            switch (queueType) {
                case QueueType::dynamic:
                    queue = new DynamicQueue(_taskNum * prNum);
//                    queue = new DynamicQueue(prNum);
                    about = "Dynamic-sized queue";
                    break;
                case QueueType::fixedMutex:
                    queue = new MutexFixedSizeQueue(queueSize, _taskNum * prNum);
//                    queue = new MutexFixedSizeQueue(queueSize, prNum);
                    about = "Mutex fixed size queue";
                    break;
                case QueueType::fixedAtomic:
//                    queue = new AtomicFixedSizeQueue(curQueueSize);
                    about = "Atomic fixed size queue";
                    break;
            }

            std::vector<std::thread> producers;
            std::vector<std::thread> consumers;
            std::vector<int> counters(conNum, 0);

            auto cur_start = std::chrono::high_resolution_clock::now();

            producers.reserve(prNum);
            for (int i = 0; i < prNum; i++) {
                producers.emplace_back(&QueueTask::ProducerWork, this, queue);
            }

            consumers.reserve(conNum);
            for (int i = 0; i < conNum; i++) {
                consumers.emplace_back(&QueueTask::ConsumerWork, queue, std::ref(counters[i]));
            }

            for (auto &p : producers) {
                p.join();
            }

            for (auto &c : consumers) {
                c.join();
            }

            auto cur_end = std::chrono::high_resolution_clock::now();
            auto workTime = std::chrono::duration_cast<std::chrono::milliseconds>(cur_end - cur_start).count();

            int consumerAnswer, producerAnswer;
            bool check = Verification(counters, prNum, consumerAnswer, producerAnswer);
            PrintResults(check, prNum, conNum, workTime, about, consumerAnswer, producerAnswer, queueSize);

            delete queue;
        }
    }
}

void QueueTask::ProducerWork(queue *queue) const {
    for (int i = 0; i < _taskNum; i++) {
        queue->push(1);
    }
//    queue->producerDone();
}

void QueueTask::ConsumerWork(queue *queue, int &counter) {
    uint8_t val;
    while (!queue->isDone()) {
//    while (!queue->isProducersDone()) {
        if (queue->pop(val)) { counter += val; }
    }
}

bool QueueTask::Verification(std::vector<int> &counters, int producerNum, int &consumerAnswer, int &producerAnswer) const
{
    consumerAnswer = 0;
    producerAnswer = producerNum * _taskNum;
    for (auto counter : counters) {
        consumerAnswer += counter;
    }

    return consumerAnswer == producerAnswer;
}

void QueueTask::PrintResults(bool isCorrect, int producerNum, int consumerNum, long long duration,
                             const std::string& about, int consumerAnswer, int producerAnswer, int queueSize=0) {
    std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << std::ctime(&time)
              << about + ":\n"
              << ((queueSize) ? "Queue size = " + std::to_string(queueSize) + "\n" : "")
              << "ProducerNum = " << producerNum << ", " << "ConsumerNum = " << consumerNum << "\n"
              << "Duration: " << (double)duration / 1000 << " s\n";

    if (isCorrect) {
        std::cout << "Everything is clear!\n\n";
    } else {
        std::cout << "Error!\n"
                  << consumerAnswer << "\n"
                  << producerAnswer << "\n\n";
    }
}
