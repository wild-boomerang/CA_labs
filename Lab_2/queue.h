//
// Created by Boom on 11.11.2020.
//

#ifndef LAB_2_QUEUE_H
#define LAB_2_QUEUE_H


#include <cstdint>

class queue {
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
    virtual bool isDone() = 0;
    virtual ~queue();
};


#endif //LAB_2_QUEUE_H
