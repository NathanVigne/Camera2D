#include "memorymanager.h"

MemoryManager::MemoryManager(std::mutex *mutex)
{
    mutex_ = mutex;
}

MemoryManager::~MemoryManager()
{
    switch (type_) {
    case U8:
        delete static_cast<unsigned char *>(buffer0);
        delete static_cast<unsigned char *>(buffer1);
        break;
    case U16:
        delete static_cast<unsigned short *>(buffer0);
        delete static_cast<unsigned short *>(buffer1);
        break;
    case BUFF_END:
        break;
    }
}

void MemoryManager::allocateMem(int width, int height, BUFFTYPE type)
{
    type_ = type;
    switch (type_) {
    case U8:
        buffer0 = new unsigned char[width * height];
        buffer1 = new unsigned char[width * height];

        break;
    case U16:
        buffer0 = new unsigned short[width * height];
        buffer1 = new unsigned short[width * height];
        break;
    case BUFF_END:
        std::clog << "Memory manager :: Buff type not supported" << std::endl;
        break;
    }

    current_ = buffer0;
    next_ = buffer1;
}

/*!
 * \brief MemoryManager::swap
 * 
 * Swap pointer to the buffer
 * 
 */
void MemoryManager::swap()
{
    //std::scoped_lock locker{*mutex_};
    void *temp;
    temp = current_;
    current_ = next_;
    next_ = temp;
}

void MemoryManager::setMutex(std::mutex *newMutex)
{
    mutex_ = newMutex;
}

BUFFTYPE MemoryManager::type() const
{
    return type_;
}

void *MemoryManager::save() const
{
    return current_;
}

void *MemoryManager::display() const
{
    return next_;
}
