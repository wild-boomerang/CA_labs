#include "MutexFixedSizeQueue.h"

#include <iostream>

MutexFixedSizeQueue::MutexFixedSizeQueue(int size, int num, ConsumerEndImplementation consumerEndImpl) : _size(size)
{
    _consumerEndImpl = consumerEndImpl;
    _num = num;
}

void MutexFixedSizeQueue::push(uint8_t val) {
    std::unique_lock<std::mutex> ul(_lock);

    _full_cond.wait(ul, [this]() { return _queue.size() < _size; }); // if _queue is full, thread should wait

    _queue.push(val);

//    ul.unlock();
    _empty_cond.notify_one();
//    ul.lock();
}

bool MutexFixedSizeQueue::pop(uint8_t &val) {
    std::unique_lock<std::mutex> ul(_lock);

    if (!_empty_cond.wait_for(ul, std::chrono::milliseconds(1),[this]() { return !_queue.empty(); })) {
        return false;
    }

    val = _queue.front();
    _queue.pop();

    if (_consumerEndImpl == ConsumerEndImplementation::elementsNum) {
        _num--;
    }

//    ul.unlock();
    _full_cond.notify_one();
//    ul.lock();

    return true;
}
