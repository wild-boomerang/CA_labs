#include "queue.h"


queue::~queue() = default;

bool queue::isConsumersEnd() {
    std::lock_guard<std::mutex> lk(_lock);
    switch (_consumerEndImpl) {
        case ConsumerEndImplementation::elementsNum:
            return _num <= 0;
        case ConsumerEndImplementation::producersNum:
            return _num <= 0 && _queue.empty();
    }
}

void queue::producerDone() {
    std::lock_guard<std::mutex> lk(_lock);
    _num--;
}
