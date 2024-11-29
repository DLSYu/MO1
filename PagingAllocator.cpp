//#include "PagingAllocator.h"
//#include <iostream>
//#include <algorithm>
//#include <Process.h>
//
//PagingAllocator::PagingAllocator(size_t maxMemorySize, size_t memPerFrame)
//    : maxMemorySize(maxMemorySize), numFrames(maxMemorySize / memPerFrame) {
//    for (size_t i = 0; i < numFrames; ++i) {
//        freeFrameList.push_back(i);
//    }
//}
//
//void* PagingAllocator::allocate(std::shared_ptr<Process> process) {
//    size_t processId = process->getPid();
//    size_t numFramesNeeded = process->getPagesRequired();
//    if (numFramesNeeded > freeFrameList.size()) {
//        std::cerr << "Memory allocation failed. Not enough free frames.\n";
//        return nullptr;
//    }
//
//    size_t frameIndex = allocateFrames(numFramesNeeded, processId);
//    return reinterpret_cast<void*>(frameIndex);
//}
//
//void* PagingAllocator::allocateBackingStore(std::shared_ptr<Process> process) {
//    size_t processId = process->getPid();
//    size_t numFramesNeeded = process->getPagesRequired();
//    if (numFramesNeeded > freeFrameList.size()) {
//        std::cerr << "Memory allocation failed. Not enough free frames.\n";
//        return nullptr;
//    }
//
//    size_t frameIndex = allocateFrames(numFramesNeeded, processId);
//    return reinterpret_cast<void*>(frameIndex);
//}
//
//void PagingAllocator::deallocate(std::shared_ptr<Process> process) {
//    size_t processId = process->getPid();
//
//    // Deallocate all frames allocated to this process
//    for (auto it = frameMap.begin(); it != frameMap.end();) {
//        if (it->second == processId) {
//            size_t frameIndex = it->first;
//            it = frameMap.erase(it);
//            freeFrameList.push_back(frameIndex);
//        }
//        else {
//            ++it;
//        }
//    }
//}
//
//std::string PagingAllocator::visualizeMemory() {
//    std::string result = "Memory Visualization:\n";
//    for (size_t frameIndex = 0; frameIndex < numFrames; ++frameIndex) {
//        auto it = frameMap.find(frameIndex);
//        if (it != frameMap.end()) {
//            result += "Frame " + std::to_string(frameIndex) + " -> Process " +
//                std::to_string(it->second) + "\n";
//        }
//        else {
//            result += "Frame " + std::to_string(frameIndex) + " -> Free\n";
//        }
//    }
//    result += "---------------------\n";
//    return result;
//}
//
//size_t PagingAllocator::allocateFrames(size_t numFrames, size_t processId) {
//    size_t startFrame = freeFrameList.back();
//    for (size_t i = 0; i < numFrames; ++i) {
//        size_t frameIndex = freeFrameList.back();
//        freeFrameList.pop_back();
//        frameMap[frameIndex] = processId;
//    }
//    return startFrame;
//}
//
//void PagingAllocator::deallocateFrames(size_t numFrames, size_t frameIndex) {
//    for (size_t i = 0; i < numFrames; ++i) {
//        frameMap.erase(frameIndex + i);
//        freeFrameList.push_back(frameIndex + i);
//    }
//}
