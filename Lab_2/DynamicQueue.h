//
// Created by Boom on 12.11.2020.
//

#ifndef LAB_2_DYNAMICQUEUE_H
#define LAB_2_DYNAMICQUEUE_H


#include <queue>
#include <mutex>
#include <thread>

#include "queue.h"


class DynamicQueue : public queue
{
private:
    std::queue<uint8_t> _queue;
    std::mutex _lock;

    int _elementsNum;
public:
    explicit DynamicQueue(int elementsNum);

    void push(uint8_t val) override;
    bool pop(uint8_t& val) override;
    bool isDone() override;
    bool isProducersDone() override;
    void producerDone() override;
};


#endif //LAB_2_DYNAMICQUEUE_H
