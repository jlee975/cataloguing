#ifndef MEMORYMAPPEDFILE_H
#define MEMORYMAPPEDFILE_H

#include <string>

class MemoryMappedFile
{
public:
    MemoryMappedFile();
    explicit MemoryMappedFile(const std::string&);
    MemoryMappedFile(const MemoryMappedFile&) = delete;
    MemoryMappedFile(MemoryMappedFile&&);
    ~MemoryMappedFile();
    MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
    MemoryMappedFile& operator=(MemoryMappedFile&&);
    void swap(MemoryMappedFile&);
    char operator[](std::size_t) const;
    const char* data() const;
    void open(const std::string&);
    void close();
    std::size_t size() const;
private:
    std::size_t size_;
    void* mapping;
};

#endif // MEMORYMAPPEDFILE_H
