cmake_minimum_required(VERSION 3.16)
project(Lab_2)

set(CMAKE_CXX_STANDARD 14)

add_executable(Lab_2 main.cpp first_try.cpp backupMain.cpp queue.cpp queue.h DynamicQueue.cpp DynamicQueue.h MutexFixedSizeQueue.cpp MutexFixedSizeQueue.h ArrayCounterTask.cpp ArrayCounterTask.h QueueTask.cpp QueueTask.h TaskExecutor.cpp TaskExecutor.h LabMenu.cpp LabMenu.h)