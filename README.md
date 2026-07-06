# Lock-Free Linked List

## Overview

This repository contains two related projects around lock-free linked lists:

1. **LockFreeLinkedList** — the main implementation based on Harris’ Ordered List-Based Set and Michael’s Hazard Pointer technique.
2. **LockFreeLinkedList with Checkpoint Approach** — an enhancement built on top of the main project that uses checkpointing to improve traversal performance in certain cases.

---

## 1. LockFreeLinkedList

### Overview

The main project implements a thread-safe and lock-free ordered linked list using Harris’ non-blocking linked list algorithm. It also uses Michael’s Hazard Pointers for safe memory reclamation, making the implementation ABA-safe.

### Features

* Thread-safe and lock-free implementation
* ABA-safe memory handling
* Ordered singly linked list based set
* Hazard pointer based memory management
* Supports multi-producer and multi-consumer operations
* Concurrent insert, delete, and find operations

### API

```cpp
bool Insert(const T& data);
bool Insert(T&& data);
bool Emplace(Args&&... args);
bool Delete(const T& data);
bool Find(const T& data);
size_t size() const;
```

### Benchmark

| Magnitude | Insert    | Delete | Insert & Delete |
| --------- | --------- | ------ | --------------- |
| 1K        | 1.2ms     | 0ms    | 3.6ms           |
| 10K       | 147.1ms   | 18.9ms | 293.5ms         |
| 100K      | 15064.4ms | 1647ms | 27176ms         |

The benchmark was tested on a 2013 MacBook Pro with an Intel Core i7, 4 cores, 2.3 GHz.

For the first and second columns, 8 threads were used to insert and delete concurrently. For the third column, 4 threads were used for insertion and 4 threads were used for deletion. Each test was repeated 10 times and the average time was recorded.

### Build and Run

```bash
make
./test
```

---

## 2. LockFreeLinkedList with Checkpoint Approach

### Overview

This project is an enhancement over the main lock-free linked list implementation. It introduces a checkpoint approach where pointer positions are stored during traversal. In many cases, storing the current pointer can reduce repeated traversal work and improve performance.

> Note: This is not the main project. It is an experimental enhancement built on top of the main project. The main project is available in `MultiCore.zip`.

### Features

* Concurrent insertions
* Concurrent deletions
* Checkpoint-based traversal optimization
* Supports multiple threads
* Built using POSIX threads

### Dependencies

* Makefile
* POSIX Threads
* C++ compiler

### Build

Navigate to the checkpoint project directory:

```bash
cd LockfreelinkedlistWithCheckPoint
```

Build the executable:

```bash
make
```

### Run Tests

```bash
./lockFreeLinkedListWithCheckPoint 100 1000 10000 4
```

This runs the test with `100`, `1000`, and `10000` elements using `4` threads.

You can also use:

```cpp
std::thread::hardware_concurrency()
```

to automatically use the number of logical cores available on the machine.

---

## Proof of Build

Proof of build on `crunch5` is included as:

```text
img.png
```

---

## References

1. Timothy L. Harris, *A Pragmatic Implementation of Non-Blocking Linked Lists*
2. Maged M. Michael, *Hazard Pointers: Safe Memory Reclamation for Lock-Free Objects*

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
