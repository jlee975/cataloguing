#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

#include "fileutil/memorymappedfile.h"

#include "collection.h"

namespace marc
{
/** @brief The Database class
 *
 * @todo Separate index file that maps to one or more data files. So we don't have to walk the whole data set.
 * @todo Compressed fields
 * @todo Don't store string length for short strings.
 * @todo Store fields in sorted order
 */
class Database
{
public:
    explicit Database(std::string);
    Database(const Database&) = delete;
    Database(Database&&) = default;
    Database& operator=(const Database&) = delete;
    Database& operator=(Database&&) = default;
    void insert(Collection);
    std::size_t size(std::size_t) const;
    std::string label(std::size_t, std::size_t) const;
    Record get_record(std::size_t, std::size_t) const;
    void reload();
    void load(std::string);
private:
    static void save(const std::string&, const std::vector< Collection >&);
    std::string load_leader(std::size_t) const;
    Record load_record(std::size_t) const;

    std::string path;

    /// The database file
    MemoryMappedFile file;

    /// Offsets of records in the database, grouped by collection
    std::vector< std::vector< std::size_t > > colls;
};
}

#endif // DATABASE_H
