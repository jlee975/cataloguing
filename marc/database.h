#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

#include "collection.h"

namespace marc
{
class Database
{
public:
    Database() = default;
    Database(const Database&) = delete;
    Database(Database&&) = default;
    Database& operator=(const Database&) = delete;
    Database& operator=(Database&&) = default;
    void insert(Collection);
    std::size_t size(std::size_t) const;
    std::string label(std::size_t, std::size_t) const;
    Record get_record(std::size_t, std::size_t) const;
    void save(const std::string&) const;
    void load(const std::string&);
private:
    std::vector< Collection > collections;
};
}

#endif // DATABASE_H
