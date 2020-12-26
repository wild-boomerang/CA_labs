//
// Created by Boom on 11.11.2020.
//

#ifndef LAB_2_QUEUE_H
#define LAB_2_QUEUE_H


#include <cstdint>
#include <queue>
#include <mutex>


enum class ConsumerEndImplementation { elementsNum, producersNum };


class queue {
protected:
    std::queue<uint8_t> _queue;
    std::mutex _lock;
    // depending on value of ConsumerEndImplementation _num represents either num of elements or num of producers
    int _num;
    ConsumerEndImplementation _consumerEndImpl;
public:
    // Записывает элемент в очередь.
    // Если очередь фиксированного размер и заполнена,
    // поток повисает внутри функции пока не освободится место
    virtual void push(uint8_t val) = 0;

    // Если очередь пуста, ждем 1 мс записи в очередь.
    // Если очередь не пуста, помещает значение головы в val,
    // удаляет голову и возвращает true.
    // Если очередь по прежнему пуста, возвращаем false
    virtual bool pop(uint8_t& val) = 0;

    // Возвращаем true, если потребителям слудет закончить свою работу,
    // иначе - false
    bool isConsumersEnd();

    // Производитель по окончании своей работы должен вызвать этот метод
    void producerDone();

    virtual ~queue();
};


#endif //LAB_2_QUEUE_H
