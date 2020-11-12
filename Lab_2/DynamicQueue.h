//
// Created by Boom on 12.11.2020.
//

#ifndef LAB_2_DYNAMICQUEUE_H
#define LAB_2_DYNAMICQUEUE_H

#include <queue>
#include <mutex>
#include "queue.h"

class DynamicQueue : queue
{
private:
    std::queue<uint8_t> _queue;
    std::mutex _lcck;
public:
    void push(uint8_t val) override;
    bool pop(uint8_t& val) override;
};


#endif //LAB_2_DYNAMICQUEUE_H
