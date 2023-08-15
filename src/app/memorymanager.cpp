#include "memorymanager.h"
#include "QThread"

/*!
 * \brief MemoryManager::MemoryManager
 * \param std::mutx *mutexSave
 * \param std::mutex *mutexDisplay
 * 
 * memory mlanager constructor. Take two mutex one for the save buffer
 * and one for the display buffer !
 * 
 */
MemoryManager::MemoryManager(std::mutex *mutexSave, std::mutex *mutexDisplay)
{
    Smutex_ = mutexSave;
    Dmutex_ = mutexDisplay;
    std::clog << "MemoryManager :: Constructor. Thread : " << QThread::currentThreadId()
              << std::endl;
}

/*!
 * \brief MemoryManager::~MemoryManager
 * 
 * Destructor used to clean up the allocated memory
 */
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
    std::clog << "MemoryManager :: Destructor" << std::endl;
}

/*!
 * \brief MemoryManager::allocateMem
 * \param int width
 * \param int height
 * \param BUFFTYPE type
 * 
 * Fonction to allocate the buffer memory. for generic purpose
 * the buffer are defined as void* then the allocation is dependant 
 * on the BUFFTYPE
 */
void MemoryManager::allocateMem(int width, int height, BUFFTYPE type)
{
    type_ = type;
    width_ = width;
    height_ = height;
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
    std::scoped_lock lock(*Dmutex_);
    void *temp;
    temp = current_;
    current_ = next_;
    next_ = temp;
}

/*!
 * \brief MemoryManager::type
 * \return BUFFTYPE
 * 
 * Getter for the bufftype memory
 * 
 */
BUFFTYPE MemoryManager::type() const
{
    return type_;
}

/*!
 * \brief MemoryManager::getWidth
 * \return int
 * 
 * Getter for the buffer width
 * 
 */
int MemoryManager::getWidth() const
{
    return width_;
}

/*!
 * \brief MemoryManager::getHeight
 * \return int
 * 
 * Getter for the buffer heigth
 * 
 */
int MemoryManager::getHeight() const
{
    return height_;
}

/*!
 * \brief MemoryManager::Smutex
 * \return std::mutex
 * 
 * Getter for the save mutex
 * 
 */
std::mutex *MemoryManager::Smutex() const
{
    return Smutex_;
}

/*!
 * \brief MemoryManager::Dmutex
 * \return std::mutex
 * 
 * Getter for the display mutex
 * 
 */
std::mutex *MemoryManager::Dmutex() const
{
    return Dmutex_;
}

/*!
 * \brief MemoryManager::save
 * \return void*
 * 
 * return the void* pointer to the "save" buffer
 */
void *MemoryManager::save() const
{
    return current_;
}

/*!
 * \brief MemoryManager::display
 * \return void*
 * 
 * return the void* pointer to the "display" buffer
 */
void *MemoryManager::display() const
{
    return next_;
}
