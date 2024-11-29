#include "PagingAllocator.h"
#include "Process.h"

#include <iostream>

PagingAllocator::PagingAllocator(size_t maxMemorySize, size_t memPerFrame)
    : maxMemorySize(maxMemorySize), numFrames(maxMemorySize / memPerFrame), memPerFrame(memPerFrame), allocatedSize(0) {
    // Initialize free frame list
    for (size_t i = 0; i < numFrames; ++i) {
        freeFrameList.push_back(i);
    }
}

void* PagingAllocator::allocate(std::shared_ptr<Process> process) {
    // Allocation logic
    size_t numFramesNeeded = process->getPagesRequired();
    size_t frameIndex = allocateFrames(numFramesNeeded, process->getPID());
    if (frameIndex == -1) {
        return nullptr; // Allocation failed
    }

    return process->getMemoryPointer();
}

void PagingAllocator::deallocate(void* ptr, std::shared_ptr<Process> process) {
    // Deallocation logic
    size_t frameIndex = reinterpret_cast<size_t>(ptr) / memPerFrame;
    size_t numFramesToDeallocate = process->getPagesRequired();
    deallocateFrames(numFramesToDeallocate, frameIndex);
}

void* PagingAllocator::allocateBackingStore(std::shared_ptr<Process> process) {
    size_t processId = process->getPID();
    size_t numFramesNeeded = process->getPagesRequired();
    if (numFramesNeeded > freeFrameList.size()) {
        std::cerr << "Memory allocation failed. Not enough free frames.\n";
        return nullptr;
    }

    size_t frameIndex = allocateFrames(numFramesNeeded, processId);
    return reinterpret_cast<void*>(frameIndex * memPerFrame);
}

std::string PagingAllocator::visualizeMemory() {
    // Visualization logic
	return freeFrameList.size() == numFrames ? std::string(numFrames, '.') : std::string(numFrames, 'X');
}

size_t PagingAllocator::getAllocatedSize() const {
    return allocatedSize;
}

size_t PagingAllocator::allocateFrames(size_t numFrames, size_t processId) {
    if (freeFrameList.size() < numFrames) {
        return -1; // Not enough free frames
    }
    size_t frameIndex = freeFrameList.back();
    freeFrameList.pop_back();
    frameMap[frameIndex] = processId;
    allocatedSize += numFrames * memPerFrame;
    return frameIndex;
}

void PagingAllocator::deallocateFrames(size_t numFrames, size_t frameIndex) {
    for (size_t i = 0; i < numFrames; ++i) {
        frameMap.erase(frameIndex + i);
        freeFrameList.push_back(frameIndex + i);
    }
    allocatedSize -= numFrames * memPerFrame;
}
