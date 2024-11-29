//#pragma once
//#include "IMemoryAllocator.h"
//#include <vector>
//#include <unordered_map>
//#include <memory>
//#include <string>
//
//class PagingAllocator : public IMemoryAllocator {
//public:
//    PagingAllocator(size_t maxMemorySize, size_t memPerFrame);
//
//    void* allocate(std::shared_ptr<Process> process) override;
//    void deallocate(std::shared_ptr<Process> process) override;
//    std::string visualizeMemory() override;
//
//private:
//    size_t maxMemorySize;
//    size_t numFrames;
//    std::unordered_map<size_t, size_t> frameMap; // frameIndex -> processId
//    std::vector<size_t> freeFrameList;
//
//    size_t allocateFrames(size_t numFrames, size_t processId);
//    void deallocateFrames(size_t numFrames, size_t frameIndex);
//};
//#pragma once
