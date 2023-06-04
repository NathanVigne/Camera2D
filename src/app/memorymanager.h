#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <iostream>
#include <mutex>

enum BUFFTYPE { U8, U16, BUFF_END };

class MemoryManager
{
public:
    MemoryManager(std::mutex *mutexSave, std::mutex *mutexDisplay);
    ~MemoryManager();

    void allocateMem(int width, int height, BUFFTYPE type);
    void swap();
    void *save() const;
    void *display() const;

    BUFFTYPE type() const;

    int getWidth() const;
    int getHeight() const;

private:
    std::mutex *Smutex_;
    std::mutex *Dmutex_;

    BUFFTYPE type_;
    int width_;
    int height_;

    // pointer to the current buffer
    void *current_;
    void *next_;

    // memory buffer to allocate
    void *buffer0;
    void *buffer1;
};

#endif // MEMORYMANAGER_H
