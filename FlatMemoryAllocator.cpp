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
        if (!allocationMap[i] && canAllocateAt(i, size)) {
            allocateAt(i, size);
            return &memory[i];
        }
    }
    return nullptr;
}

void FlatMemoryAllocator::deallocate(void* ptr) {
    size_t index = static_cast<char*>(ptr) - &memory[0];
    if (allocationMap[index]) {
        deallocateAt(index, 1); // Assuming deallocation of 1 block for simplicity
    }
}

std::string FlatMemoryAllocator::visualizeMemory() {
    return std::string(memory.begin(), memory.end());
}

void FlatMemoryAllocator::initializeMemory() {
    for (size_t i = 0; i < maximumSize; ++i) {
        allocationMap[i] = false;
    }
}

bool FlatMemoryAllocator::canAllocateAt(size_t index, size_t size) const {
    if (index + size > maximumSize) return false;
    for (size_t i = index; i < index + size; ++i) {
        if (allocationMap.at(i)) return false;
    }
    return true;
}

void FlatMemoryAllocator::allocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = true;
        memory[i] = 'X'; // Mark allocated memory
    }
    allocatedSize += size;
}

void FlatMemoryAllocator::deallocateAt(size_t index, size_t size) {
    for (size_t i = index; i < index + size; ++i) {
        allocationMap[i] = false;
        memory[i] = '.'; // Mark deallocated memory
    }
    allocatedSize -= size;
}
