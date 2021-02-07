# CA_labs
Laboratory works on Computer architecture, the main purpose of which is to study multitasking, parallelism.

### Lab 1 - TECHNOLOGY MMX, SIMD

##### Task:
- Process arrays of 8 elements by the following expression:  
**F[i] = (A[i] + B[i]) * (C[i] + D[i])**, **i** = 1...8;  
**A**, **B**, and **C** are arrays of 8-bit signed integers (`_int8`);  
**D** is an array of 16-bit signed integers (`_int16`).

##### Solution:
- The laboratory work was done in two ways: through **assembler inserts** and through **Intel intrinsic instructions** 
(C style functions that provide access to MMX without the need to write assembly code).
- A debug mode has also been added to the implementation, allowing you to check each internal operation when solving a 
task.

### Lab 2 - Multi-threaded code development techniques

##### Purpose 
To study the methods of organizing the work of multithreaded programs.

##### General instructions. 
In work, you need to use the C++ language, the C++ 11 standard and above.
The modules of the standard library must be used: thread, mutex, atomic, future, condition_variable. 
It is best to implement each task and each item of each task as a separate function or module of one program.  

#### Part 1. Parallel traversal of an array of elements.
Write a function that instantiates an array of `NumTasks` bytes, each of which contains 0. Start `NumThreads` 
threads, each of which reads a thread-safe index, increments it by one and adds one to the array element at this index. 
Count the running time of all threads. After finishing the work of all threads, it is necessary to check the correctness
of filling the array and display the operating time.  
1. Implement a thread-safe counter in two ways: blocking with `std::mutex` and non-blocking with `std::atomic`
1. Measure the running time of a function using different counter implementations with `NumTasks = 1024 * 1024` and 
`NumThreads = {4, 8, 16, 32}`.
1. Add a stream lull **10ns** after each increment of an array element. Repeat measurements from point 2.

#### Part 2. Producer-consumer.
Write the code instantiating `ConsumerNum` of consumers and `ProducerNum` of producers. Bind them with a thread-safe 
queue that stores single-byte items. Consumers instantiate a local counter and then start reading the queue and adding 
the subtracted values ​​to the local counter. Each producer writes `TaskNum` units to the queue and exits. When all the 
producers have written their messages and the consumers have emptied the queue, the consumers finish their work and 
return the resulting amounts. It is necessary to track the working time of producers and consumers and check that the 
total of the results of all consumers is equal to `ProducerNum / TaskNum`. Test for `ProducerNum = {1, 2, 4}`, 
`ConsumerNum = {1, 2, 4}`, `TaskNum = 4 * 1024 * 1024`.  

Queue interface:  
```c++
class queue
{ 
public:
    // Writes an item to the queue.
    // If the queue is fixed size and full,
    // the thread hangs inside the function until space is freed
    void push(uint8_t val);
    // If the queue is empty, wait 1 ms for writing to the queue.
    // If the queue is not empty, puts the head value in val,
    // removes the head and returns true.
    // If the queue is still empty, return false
    bool pop(uint8_t& val);
};
```

The problem must be solved for the following options for implementing the queue:
1. Dynamic queue using std containers and `std::mutex`.
1. Fixed size queue QueueSize using `std::mutex` and `std::condition_variable` without busy wait. 
Test for `QueueSize = {1, 4, 16}`.
1. Fixed size queue using `std::atomic`. Use `std::condition_variable` and `std::mutex` only to notify threads 
about free cells or new tasks Test for `QueueSize = {1, 4, 16}`.

##### Solution:
- The design of this lab has been divided into modules.
- The program starts from the menu, which presents the `LabMenu` class with the following static methods:
  - `Task_1_Menu()`, which starts the first part of the lab;
  - `Task_2_Menu()`, which starts the second part of the lab;
  - `AllTasks()`, which runs all tasks in sequence.
- These methods transfer control to the `TaskExecutor` class, which runs the corresponding parts of the lab. 
In their static methods `ExecuteTask_1()` and `ExecuteTask_2()`, the necessary variables are initialized and the 
  classes that represent the parts of the lab — `ArrayCounterTask` (part one) and `QueueTask` (part two) are launched.
- These classes (`ArrayCounterTask` and `QueueTask`) directly fulfill the conditions of the laboratory work and display 
the results of their work in the console in a detailed and convenient form.
- Since the second part of the lab requires the creation of queues of different types, to fulfill this requirement, the 
  corresponding classes were created to represent queues of different types - `DynamicQueue` and `MutexFixedSizeQueue` - 
  which inherit from the `queue` class specified in the lab condition.  
- Due to the fact that consumers need to somehow communicate that producers have finished their work, the `queue` 
  interface has been expanded. Namely, the `isConsumersEnd()` and `producerDone()` methods were added. These methods made 
  it possible to provide functionality for timely notification of consumers about the end of the work of producers.  
- This functionality was developed in two ways, which are described by the `ConsumerEndImplementation` enumeration.   
  - In short, the main idea of ​​the first notification method is to notify consumers about the end of the producers' 
  work when all the elements provided for by the producer-consumer task are finished. The number of these elements 
  is passed to the constructor of the `queue` class. 
  - The idea of ​​the second method is that the number of producers is passed to the constructor of the `queue` class. 
  After the end of the work of each producer, consumers are notified about this. 
  - Thus, the relationship between the producer and the consumer is regulated.
- The program results are presented in the _**results/results.txt**_.

### Lab 3 - Programming multi-core architectures

##### Objective
Using the **OpenMP** interface for programming simple multithreaded applications.  
##### Task
Implement a matrix-vector multiplication algorithm with using the **OpenMP** interface.  
##### Solution:
- Laboratory work performed in accordance with the requirements.
- The results are displayed on the console in a convenient and detailed form (presented in the _**results/results.txt**_).
- The results of multiplying a matrix by a vector in one thread and several threads are checked for compliance 
with each other.