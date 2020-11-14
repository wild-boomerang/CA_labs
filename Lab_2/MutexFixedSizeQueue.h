//
// Created by Boom on 13.11.2020.
//

#ifndef LAB_2_MUTEXFIXEDSIZEQUEUE_H
#define LAB_2_MUTEXFIXEDSIZEQUEUE_H

#include <queue>
#include <mutex>
#include <thread>

#include "queue.h"

class MutexFixedSizeQueue : public queue
{
private:
    std::queue<uint8_t> _queue;
    std::mutex _lock;
    unsigned int _size;
public:
    explicit MutexFixedSizeQueue(unsigned int size);

    void push(uint8_t val) override;
    bool pop(uint8_t& val) override;
};


#endif //LAB_2_MUTEXFIXEDSIZEQUEUE_H
