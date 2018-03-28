#include "memorymappedfile.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

MemoryMappedFile::MemoryMappedFile() : fd(-1), mapping(nullptr)
{
}

MemoryMappedFile::MemoryMappedFile(const std::string & path)
    : fd(-1), size(0), mapping(nullptr)
{
    const int fd_ = ::open(path.c_str(), O_RDONLY);

    if (fd_ != -1)
    {
        struct stat buf;
        if (::fstat(fd_, &buf) == 0 && S_ISREG(buf.st_mode))
        {
            if (void* p = ::mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd_, 0))
            {
                mapping = p;
                size = buf.st_size;
                fd = fd_;
                return;
            }
        }
        ::close(fd_);
    }
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile && o) : fd(o.fd), size(o.size), mapping(o.mapping)
{
    o.fd = -1;
    o.size = 0;
    o.mapping = nullptr;
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (mapping)
        ::munmap(mapping, size);
    ::close(fd);
}

MemoryMappedFile & MemoryMappedFile::operator=(MemoryMappedFile && o)
{
    MemoryMappedFile t(std::move(o));
    swap(t);
    return *this;
}

void MemoryMappedFile::swap(MemoryMappedFile & o)
{
    std::swap(fd, o.fd);
    std::swap(size, o.size);
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
