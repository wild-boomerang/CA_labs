//
// Created by Boom on 13.11.2020.
//

#ifndef LAB_2_MUTEXFIXEDSIZEQUEUE_H
#define LAB_2_MUTEXFIXEDSIZEQUEUE_H


#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
//#include <shared_mutex>

#include "queue.h"


class MutexFixedSizeQueue : public queue
{
private:
    std::queue<uint8_t> _queue;
    std::mutex _lock;
    const int _size;

    std::condition_variable _full_cond;
    std::condition_variable _empty_cond;

    int _elementsNum;

public:
    explicit MutexFixedSizeQueue(int size, int elementsNum);

    void push(uint8_t val) override;
    bool pop(uint8_t& val) override;
    bool isDone() override;
    bool isProducersDone() override;
    void producerDone() override;
};


#endif //LAB_2_MUTEXFIXEDSIZEQUEUE_H
