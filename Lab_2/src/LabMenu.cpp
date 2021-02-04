#include <iostream>
#include <thread>

#include "LabMenu.h"
#include "TaskExecutor.h"


void LabMenu::Run() {
    int choice;
    do {
        std::cout << "Number of physical threads: " << std::thread::hardware_concurrency() << std::endl;
        std::cout << "What do you want?\n"
                  << "1. Task 1\n"
                  << "2. Task 2\n"
                  << "3. All tasks consistently\n"
                  << "0. Exit\n";
        std::cin >> choice;

        switch (choice) {  // todo catching exceptions
            case 1:
                LabMenu::Task_1_Menu();
                break;
            case 2:
                LabMenu::Task_2_Menu();
                break;
            case 3:
                LabMenu::AllTasks();
                break;
            default:
                break;
        }
    } while (choice != 0);
}

void LabMenu::Task_1_Menu() {
    int choice;
    std::cout << "Task 1:\n"
              << "1. Without sleep\n"
              << "2. With sleep\n"
              << "0. Exit\n";
    std::cin >> choice;

    switch (choice) {
        case 1: {
            TaskExecutor::ExecuteTask_1(false);
            break;
        }
        case 2: {
            TaskExecutor::ExecuteTask_1(true);
            break;
        }
        default:
            break;
    }
}

void LabMenu::Task_2_Menu() {
    int choice;
    std::cout << "Task 2:\n"
              << "1. Dynamic-sized queue\n"
              << "2. Mutex fixed size queue\n"
              << "3. Atomic fixed size queue\n"
              << "0. Exit\n";
    std::cin >> choice;

    switch (choice) {
        case 1: {
            TaskExecutor::ExecuteTask_2(QueueType::dynamic);
            break;
        }
        case 2: {
            TaskExecutor::ExecuteTask_2(QueueType::fixedMutex);
            break;
        }
        case 3: {
//            TaskExecutor::ExecuteTask_2(QueueType::fixedAtomic);
            break;
        }
        default:
            break;
    }
}

void LabMenu::AllTasks() {
    TaskExecutor::ExecuteTask_1(false);
    TaskExecutor::ExecuteTask_1(true);
    TaskExecutor::ExecuteTask_2(QueueType::dynamic);
    TaskExecutor::ExecuteTask_2(QueueType::fixedMutex);
//    TaskExecutor::ExecuteTask_2(QueueType::fixedAtomic);
}
