#ifndef FLATMEMORYALLOCATOR_H
#define FLATMEMORYALLOCATOR_H

#include "IMemoryAllocator.h"
#include <vector>
#include <unordered_map>
#include <mutex>

class FlatMemoryAllocator : public IMemoryAllocator {
public:
    FlatMemoryAllocator(size_t maximumSize);
    ~FlatMemoryAllocator();

    void* allocate(std::shared_ptr<Process> process) override;
    void deallocate(void* ptr, std::shared_ptr<Process> process) override;
    std::string visualizeMemory() override;
    size_t getAllocatedSize() const override;

private:
    void initializeMemory();
    bool canAllocateAt(size_t index, size_t size) const;
    void allocateAt(size_t index, size_t size);
    void deallocateAt(size_t index, size_t size);

    size_t maximumSize;
    size_t allocatedSize;
    std::vector<char> memory;
    std::unordered_map<size_t, size_t> allocationMap;
    std::mutex mtx;
};

#endif // FLATMEMORYALLOCATOR_H
