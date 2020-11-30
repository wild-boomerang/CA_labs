//
// Created by Boom on 13.11.2020.
//

#include "MutexFixedSizeQueue.h"

#include <iostream>

MutexFixedSizeQueue::MutexFixedSizeQueue(int size, int elementsNum) : _size(size), _elementsNum(elementsNum)
{

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
    _elementsNum--;

//    ul.unlock();
    _full_cond.notify_one();
//    ul.lock();

    return true;
}

// Возвращаем true, если потребителям слудет закончить свою работу,
// иначе - false
bool MutexFixedSizeQueue::isDone() {
    std::lock_guard<std::mutex> lk(_lock);
    return _elementsNum <= 0;
}

// Производитель по окончании своей работы должен вызвать этот метод
void MutexFixedSizeQueue::producerDone() {
    std::lock_guard<std::mutex> lk(_lock);
    _elementsNum--;
}

// Возвращаем true, если потребителям слудет закончить свою работу,
// иначе - false
bool MutexFixedSizeQueue::isProducersDone() {
    std::lock_guard<std::mutex> lk(_lock);
    return _elementsNum <= 0 && _queue.empty();
}
