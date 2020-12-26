//
// Created by Boom on 13.11.2020.
//

#include "MutexFixedSizeQueue.h"

#include <iostream>

MutexFixedSizeQueue::MutexFixedSizeQueue(int size, int num, ConsumerEndImplementation consumerEndImpl) : _size(size)
{
    _consumerEndImpl = consumerEndImpl;
    _num = num;
}

// Записывает элемент в очередь.
// Если очередь фиксированного размера и заполнена,
// поток повисает внутри функции пока не освободится место
void MutexFixedSizeQueue::push(uint8_t val) {
    std::unique_lock<std::mutex> ul(_lock);

    _full_cond.wait(ul, [this]() { return _queue.size() < _size; }); // if _queue is full, thread should wait

    _queue.push(val);

//    ul.unlock();
    _empty_cond.notify_one();
//    ul.lock();
}

// Если очередь пуста, ждем 1 мс записи в очередь.
// Если очередь не пуста, помещает значение головы в val,
// удаляет голову и возвращает true.
// Если очередь по прежнему пуста, возвращаем false
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
