#include "memorymappedfile.h"

#include <stdexcept>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

MemoryMappedFile::MemoryMappedFile() : mapping(nullptr)
{
}

MemoryMappedFile::MemoryMappedFile(const std::string& path_)
    : size_(0), mapping(nullptr)
{
    const int fd_ = ::open(path_.c_str(), O_RDONLY);

    if (fd_ != -1)
    {
        struct stat buf;
        if (::fstat(fd_, &buf) == 0 && S_ISREG(buf.st_mode))
        {
            if (void* p = ::mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd_, 0))
            {
                mapping = p;
                size_ = buf.st_size;
            }
        }
        ::close(fd_);
    }
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile && o) : size_(o.size_), mapping(o.mapping)
{
    o.size_ = 0;
    o.mapping = nullptr;
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (mapping)
        ::munmap(mapping, size_);
}

MemoryMappedFile & MemoryMappedFile::operator=(MemoryMappedFile && o)
{
    MemoryMappedFile t(std::move(o));
    swap(t);
    return *this;
}

void MemoryMappedFile::swap(MemoryMappedFile & o)
{
    std::swap(size_, o.size_);
    std::swap(mapping, o.mapping);
}

char MemoryMappedFile::operator[](std::size_t i) const
{
    return static_cast< char* >(mapping)[i];
}

const char *MemoryMappedFile::data() const
{
    return static_cast< char* >(mapping);
}

void MemoryMappedFile::open(const std::string & path_)
{
    MemoryMappedFile t(path_);
    swap(t);
}

void MemoryMappedFile::close()
{
    MemoryMappedFile t;
    swap(t);
}

std::size_t MemoryMappedFile::size() const
{
    return size_;
}
