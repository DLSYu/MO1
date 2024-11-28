#include "FlatMemoryAllocator.h"

FlatMemoryAllocator::FlatMemoryAllocator(size_t maximumSize)
    : maximumSize(maximumSize), allocatedSize(0), memory(maximumSize, '.'), allocationMap() {
    initializeMemory();
}

FlatMemoryAllocator::~FlatMemoryAllocator() {
    memory.clear();
}

void* FlatMemoryAllocator::allocate(size_t size) {
    for (size_t i = 0; i < maximumSize - size + 1; ++i) {
        if (allocationMap.find(i) == allocationMap.end() && canAllocateAt(i, size)) {
            allocateAt(i, size);
            return &memory[i];
        }
    }
    return nullptr;
}

void FlatMemoryAllocator::deallocate(void* ptr, size_t size) {
    size_t index = static_cast<char*>(ptr) - &memory[0];
    if (allocationMap.find(index) != allocationMap.end()) {
        deallocateAt(index, size);
        allocationMap.erase(index);
    }
}

std::string FlatMemoryAllocator::visualizeMemory() {
    return std::string(memory.begin(), memory.end());
}

void FlatMemoryAllocator::initializeMemory() {
    // No need to initialize allocationMap as it is empty by default
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
    if (index + size > maximumSize) return false;
    for (size_t i = index; i < index + size; ++i) {
        if (allocationMap.find(i) != allocationMap.end()) return false;
    }
    return true;
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        memory[i] = 'X'; // Mark allocated memory
    }
    allocationMap[index] = size; // Store the size of the allocated block
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        memory[i] = '.'; // Mark deallocated memory
    }
    allocatedSize -= size;
}

size_t FlatMemoryAllocator::getAllocatedSize() const {
    return allocatedSize;
}