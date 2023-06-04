#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <iostream>
#include <mutex>

enum BUFFTYPE { U8, U16, BUFF_END };

class MemoryManager
{
public:
    MemoryManager(std::mutex *mutex);
    ~MemoryManager();

    void allocateMem(int width, int height, BUFFTYPE type);
    void swap();
    void *save() const;
    void *display() const;
    void setMutex(std::mutex *newMutex);

    BUFFTYPE type() const;

private:
    std::mutex *mutex_;
    BUFFTYPE type_;

    // pointer to the current buffer
    void *current_;
    void *next_;

    // memory buffer to allocate
    void *buffer0;
    void *buffer1;
};

#endif // MEMORYMANAGER_H
