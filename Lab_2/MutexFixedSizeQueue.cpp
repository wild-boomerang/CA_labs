//
// Created by Boom on 13.11.2020.
//

#include "MutexFixedSizeQueue.h"

MutexFixedSizeQueue::MutexFixedSizeQueue(unsigned int size) : _size(size)
{

}

// Записывает элемент в очередь.
// Если очередь фиксированного размера и заполнена,
// поток повисает внутри функции пока не освободится место
void MutexFixedSizeQueue::push(uint8_t val) {
    std::lock_guard<std::mutex> lockGuard(_lock);
    if (_queue.size() < _size)
    {
        _queue.push(val);
    } else
    {

    }
}

// Если очередь пуста, ждем 1 мс записи в очередь.
// Если очередь не пуста, помещает значение головы в val,
// удаляет голову и возвращает true.
// Если очередь по прежнему пуста, возвращаем false
bool MutexFixedSizeQueue::pop(uint8_t &val) {
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
    return true;
}