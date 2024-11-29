#ifndef IMEMORYALLOCATOR_H
#define IMEMORYALLOCATOR_H

#include <cstddef>
#include <string>
#include <memory>
#include "Process.h" // Forward declaration

class IMemoryAllocator {
public:
    virtual ~IMemoryAllocator() = default;

    virtual void* allocate(std::shared_ptr<Process> process) = 0;
    virtual void deallocate(void* ptr, std::shared_ptr<Process> process) = 0;
    virtual std::string visualizeMemory() = 0;
    virtual size_t getAllocatedSize() const = 0;
};

#endif // IMEMORYALLOCATOR_H
