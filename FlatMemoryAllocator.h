// FlatMemoryAllocator.h
#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>
#include <iostream>

class FlatMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maximumSize);
    ~FlatMemoryAllocator();
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
    std::string visualizeMemory();
    bool canAllocateAt(size_t index, size_t size) const;
    size_t getAllocatedSize() const;

private:
    size_t maximumSize;
    size_t allocatedSize;
    std::vector<char> memory;
    std::unordered_map<size_t, size_t> allocationMap;
    std::mutex mtx;

    void initializeMemory();
    void allocateAt(size_t index, size_t size);
    void deallocateAt(size_t index, size_t size);
    
};


