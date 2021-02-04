#ifndef LAB_2_MUTEXFIXEDSIZEQUEUE_H
#define LAB_2_MUTEXFIXEDSIZEQUEUE_H


#include <thread>
#include <condition_variable>
//#include <shared_mutex>

#include "queue.h"


class MutexFixedSizeQueue : public queue
{
private:
    const int _size;

    std::condition_variable _full_cond;
    std::condition_variable _empty_cond;

public:
    explicit MutexFixedSizeQueue(int size, int num, ConsumerEndImplementation consumerEndImpl);

    void push(uint8_t val) override;
    bool pop(uint8_t& val) override;
};


#endif //LAB_2_MUTEXFIXEDSIZEQUEUE_H
