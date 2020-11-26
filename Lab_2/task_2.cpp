//
// Created by Boom on 11.11.2020.
//

#include <iostream>
#include <thread>
#include <vector>

void ThreadFunc(int one, int two)
{

}

int start()
{
    std::vector<std::thread> producers;
    int size = 4;

    producers.reserve(size);
    for (int i = 0; i < size; i++) {
        producers.emplace_back(ThreadFunc, size, size);
    }

    return 0;
}

