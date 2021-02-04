#include "DynamicQueue.h"


DynamicQueue::DynamicQueue(int num, ConsumerEndImplementation consumerEndImpl)
{
    _consumerEndImpl = consumerEndImpl;
    _num = num;
}

void DynamicQueue::push(uint8_t val) {
    std::lock_guard<std::mutex> lk(_lock);
    _queue.push(val);
}

bool DynamicQueue::pop(uint8_t &val) {
    std::unique_lock<std::mutex> ul(_lock);
    if (_queue.empty())
    {
        ul.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ul.lock();
        if (_queue.empty()) { return false; }
    }

    val = _queue.front();
    _queue.pop();

    if (_consumerEndImpl == ConsumerEndImplementation::elementsNum) {
        _num--;
    }

    return true;
}
