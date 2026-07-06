#include <iostream>
#include <chrono>
#include <memory>
#include <thread>
#include <random>
#include <cassert>
#include <unordered_map>
#include <vector>

#include "lockFreeLinkedListWithCheckPoint.h"
using namespace std;

// Here we will mention how many threads we want to devote to linkedList implementation
static int noOfThreads;
// = std::thread::hardware_concurrency();

int maxElements;
LockFreeLinkedListWithCheckPoint<int> list;
LockFreeLinkedListWithCheckPoint<int> newList;

// Insert successfully then ++totalCount, delete successfully then --totalCount.
std::atomic<int> totalCount = 0;

// doProceed is a boolean which tells thread to do work only when this boolean is set true. To get the exact time span
std::atomic<bool> doProceed = false;

//std::unordered_map<int, int*> timeDuration;
std::unordered_map<int, std::vector<int>> timeDuration;

void onInsert(int divide) {

    // wait till we get green signal
    while (!doProceed) {
        std::this_thread::yield();
    }

    int n = maxElements / divide;
    for (int i = 0; i < n; i++) {
        if (list.Insert(rand() % n)) {
            ++totalCount;
        }
    }
}

void TestConcurrentInsert() {
    int old_size = list.size();
    std::vector<std::thread> threads;
    for (int i = 0; i < noOfThreads; ++i) {
        threads.push_back(std::thread(onInsert, noOfThreads));
    }

    doProceed = true;

    auto t1_ = std::chrono::steady_clock::now();
    for (int i = 0; i < noOfThreads; ++i) {
        threads[i].join();
    }
    auto t2_ = std::chrono::steady_clock::now();

    assert(totalCount + old_size == static_cast<int>(list.size()));
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2_ - t1_).count();

    timeDuration[maxElements][0] += ms;
    std::cout << maxElements << " elements insert concurrently, timespan=" << ms << "ms" << "\n";
    doProceed = false;
}

void onDelete(int divide) {
    while (!doProceed) {
        std::this_thread::yield();
    }

    int n = maxElements / divide;
    for (int i = 0; i < n; i++) {
        if (list.Delete(rand() % n)) {
            --totalCount;
        }
    }
}

void TestConcurrentDelete() {
    int old_size = list.size();
    std::vector<std::thread> threads;
    for (int i = 0; i < noOfThreads; ++i) {
        threads.push_back(std::thread(onDelete, noOfThreads));
    }

    totalCount = 0;
    doProceed = true;
    auto t1_ = std::chrono::steady_clock::now();
    for (int i = 0; i < noOfThreads; ++i) {
        threads[i].join();
    }
    auto t2_ = std::chrono::steady_clock::now();

    assert(totalCount + old_size == static_cast<int>(list.size()));
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2_ - t1_).count();
    timeDuration[maxElements][1] += ms;
    std::cout << maxElements << " elements delete concurrently, timespan=" << ms << "ms" << "\n";

    totalCount = 0;
    doProceed = false;
}

void TestConcurrentInsertAndDequeue() {
    int old_size = list.size();

    int divide = noOfThreads / 2;
    std::vector<std::thread> insert_threads;
    for (int i = 0; i < divide; ++i) {
        insert_threads.push_back(std::thread(onInsert, divide));
    }

    std::vector<std::thread> delete_threads;
    for (int i = 0; i < divide; ++i) {
        delete_threads.push_back(std::thread(onDelete, divide));
    }

    totalCount = 0;
    doProceed = true;
    auto t1_ = std::chrono::steady_clock::now();
    for (int i = 0; i < divide; ++i) {
        insert_threads[i].join();
    }

    for (int i = 0; i < divide; ++i) {
        delete_threads[i].join();
    }
    auto t2_ = std::chrono::steady_clock::now();

    assert(totalCount + old_size == static_cast<int>(list.size()));
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2_ - t1_).count();

    timeDuration[maxElements][2] += ms;
    std::cout << maxElements << " elements insert and delete concurrently, timespan=" << ms << "ms" << "\n";

    totalCount = 0;
    doProceed = false;
}

int main(int argc, char const* argv[]) {
    srand(std::time(0));

    noOfThreads = atoi(argv[argc - 1]);

    std::cout << "Benchmark with " << noOfThreads << " threads:"
              << "\n";

    vector<int> elements;
    for (int i = 1; i < argc - 1; i++) {
        elements.push_back(atoi(argv[i]));
    }

    for (int elem : elements) {
        timeDuration[elem] = std::vector<int>(4, 0);
    }

    for (int i = 0; i < 10; ++i) {
        for (std::size_t j = 0; j < elements.size(); j++) {
            maxElements = elements[j];
            TestConcurrentInsert();
            TestConcurrentDelete();
            TestConcurrentInsertAndDequeue();
            cout<<"\n";
        }
    }

    cout<<"\n";

    std::cout<<"\n"<<"*********  Average Time Calculation  ***********"<<"\n";

    for (std::size_t i = 0; i < elements.size(); ++i) {
        maxElements = elements[i];
        std::cout << maxElements << " elements insert concurrently, average time =" << (float) timeDuration[maxElements][0] / 10.0f << "ms" << "\n";
        std::cout << maxElements << " elements delete concurrently, average time =" << (float) timeDuration[maxElements][1] / 10.0f << "ms" << "\n";
        std::cout << maxElements << " elements insert and delete concurrently, average time =" << (float) timeDuration[maxElements][2] / 10.0f << "ms" << "\n";
        cout<<"\n";
    }

    return 0;
}