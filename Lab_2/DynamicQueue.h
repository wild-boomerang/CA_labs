#ifndef LAB_2_DYNAMICQUEUE_H
#define LAB_2_DYNAMICQUEUE_H


#include <thread>

#include "queue.h"


class DynamicQueue : public queue
{
public:
    explicit DynamicQueue(int num, ConsumerEndImplementation consumerEndImpl);

    void push(uint8_t val) override;
    bool pop(uint8_t& val) override;
};


#endif //LAB_2_DYNAMICQUEUE_H
