#ifndef LOCKFREE_LINKEDLIST_H
#define LOCKFREE_LINKEDLIST_H

#include <atomic>
#include <cstdio>
#include <iostream>

// Partial traversal
template <typename T>
class LockFreeLinkedListWithCheckPoint {
public:
    struct Node;

    // Constructor to initialize the list with a dummy head node
    LockFreeLinkedListWithCheckPoint() : head_(new Node()), size_(0), checkpoint_(head_) {}

    // Destructor to clean up all nodes
    ~LockFreeLinkedListWithCheckPoint() {
        Node* currentNode = head_;
        while (currentNode != nullptr) {
            Node* nextNode = currentNode->next.load(std::memory_order_acquire);
            delete currentNode;
            currentNode = nextNode;
        }
    }

    // Insert method to add elements to the list
    bool Insert(const T& elem) {
        Node* newNode = new Node(elem);
        Node* prev = nullptr;
        Node* cur = nullptr;

        while (true) {
            if (Search(elem, &prev, &cur)) {
                newNode->next.store(cur, std::memory_order_relaxed);
                if (prev->next.compare_exchange_strong(cur, newNode, std::memory_order_release, std::memory_order_relaxed)) {
                    size_.fetch_add(1, std::memory_order_relaxed);
                    return true;
                }
            } else {
                delete newNode;
                return false;
            }
        }
    }

    // Delete method to remove elements from the list
    bool Delete(const T& elem) {
        Node* prev = nullptr;
        Node* cur = nullptr;
        Node* next = nullptr;

        while (true) {
            if (!Search(elem, &prev, &cur)) {
                return false;
            }
            next = cur->next.load(std::memory_order_acquire);
            if (cur->next.compare_exchange_strong(next, nullptr, std::memory_order_release, std::memory_order_relaxed)) {
                if (prev->next.compare_exchange_strong(cur, next, std::memory_order_release, std::memory_order_relaxed)) {
                    delete cur;
                    size_.fetch_sub(1, std::memory_order_relaxed);
                    return true;
                } else {
                    cur->next.store(next, std::memory_order_release);
                }
            }
        }
    }

    // Search method enhanced with checkpoint logic
    bool Search(const T& data, Node** prev_ptr, Node** cur_ptr) {
        Node* prev = checkpoint_.load(std::memory_order_acquire);
        Node* cur = prev->next.load(std::memory_order_acquire);

        while (cur != nullptr) {
            if (cur->data >= data) {
                *prev_ptr = prev;
                *cur_ptr = cur;
                if (cur->data == data) {
                    checkpoint_.store(cur, std::memory_order_release);  // Update checkpoint
                    return true;
                }
                return false;
            }
            prev = cur;
            cur = cur->next.load(std::memory_order_acquire);
        }

        *prev_ptr = prev;
        *cur_ptr = cur;
        return false;
    }

    // Getter for list size
    size_t size() const {
        return size_.load(std::memory_order_relaxed);
    }

public:
    struct Node {
        T data;
        std::atomic<Node*> next;

        Node() : next(nullptr) {}
        explicit Node(const T& data) : data(data), next(nullptr) {}
    };

    Node* head_;
    std::atomic<size_t> size_;
    std::atomic<Node*> checkpoint_;  // Checkpoint for partial re-traversal
};

#endif // LOCKFREE_LINKEDLIST_H