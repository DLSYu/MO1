// FlatMemoryAllocator.h
#pragma once
#include <vector>
#include <unordered_map>
#include <string>

class FlatMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maximumSize);
    ~FlatMemoryAllocator();
    void* allocate(size_t size);
    void deallocate(void* ptr, size_t size);
    std::string visualizeMemory();

private:
    size_t maximumSize;
    size_t allocatedSize;
    std::vector<char> memory;
    std::unordered_map<size_t, size_t> allocationMap;

    void initializeMemory();
    bool canAllocateAt(size_t index, size_t size) const;
    void allocateAt(size_t index, size_t size);
    void deallocateAt(size_t index, size_t size);
};


