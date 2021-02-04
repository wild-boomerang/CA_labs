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
    // Writes an item to the queue.
    // If the queue is fixed size and full,
    // the thread hangs inside the function until space is freed
    virtual void push(uint8_t val) = 0;

    // If the queue is empty, wait 1 ms for writing to the queue.
    // If the queue is not empty, puts the head value in val,
    // removes the head and returns true.
    // If the queue is still empty, return false
    virtual bool pop(uint8_t& val) = 0;

    // Return true if consumers should finish their work,
    // otherwise - false
    bool isConsumersEnd();

    // Producer, at the end of his work, must call this method.
    void producerDone();

    virtual ~queue();
};


#endif //LAB_2_QUEUE_H
