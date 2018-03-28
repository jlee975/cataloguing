#include "memorymappedfile.h"

#include <stdexcept>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

MemoryMappedFile::MemoryMappedFile() : fd(-1), mapping(nullptr)
{
}

MemoryMappedFile::MemoryMappedFile(const std::string & path_)
    : fd(-1), size_(0), mapping(nullptr)
{
    const int fd_ = ::open(path_.c_str(), O_RDONLY);

    if (fd_ != -1)
    {
        struct stat buf;
        if (::fstat(fd_, &buf) == 0 && S_ISREG(buf.st_mode))
        {
            if (void* p = ::mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd_, 0))
            {
                path = path_;
                mapping = p;
                size_ = buf.st_size;
                fd = fd_;
                return;
            }
        }
        ::close(fd_);
    }
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile && o) : path(std::move(o.path)), fd(o.fd), size_(o.size_), mapping(o.mapping)
{
    o.fd = -1;
    o.size_ = 0;
    o.mapping = nullptr;
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (mapping)
        ::munmap(mapping, size_);
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
    path.swap(o.path);
    std::swap(fd, o.fd);
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

const std::string &MemoryMappedFile::get_path() const
{
    return path;
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
