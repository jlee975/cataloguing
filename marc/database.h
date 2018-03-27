#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <memory>

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
    void import_xml(const std::string&);
    std::size_t size(std::size_t) const;
    std::string label(std::size_t, std::size_t) const;
    Record get_record(std::size_t, std::size_t) const;
private:
    std::vector< Collection > collections;
};
}

#endif // DATABASE_H
