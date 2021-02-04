#ifndef LAB_2_QUEUETASK_H
#define LAB_2_QUEUETASK_H


#include <vector>
#include <string>

#include "queue.h"


enum class QueueType { dynamic, fixedMutex, fixedAtomic };


class QueueTask {
private:
    const std::vector<int> _producerNum;
    const std::vector<int> _consumerNum;
    const int _taskNum;
    std::vector<int> _queueSize;
    ConsumerEndImplementation _consumerEndImpl;

    void RunWitGivenQueueSize(QueueType queueType, int queueSize=0);
    void ProducerWork(queue *queue) const;
    static void ConsumerWork(queue *queue, int &counter);
    bool Verification(std::vector<int> &counters, int producerNum, int &consumerAnswer, int &producerAnswer) const;
    static void PrintResults(bool isCorrect, int producerNum, int consumerNum, long long duration,
                             const std::string& about, int consumerAnswer, int producerAnswer, int queueSize);
public:
    QueueTask(std::vector<int> producerNum, std::vector<int> consumerNum, int taskNum,
              std::vector<int> queueSize, ConsumerEndImplementation consumerEndImpl);

    void Execute(QueueType queueType);
};


#endif //LAB_2_QUEUETASK_H
