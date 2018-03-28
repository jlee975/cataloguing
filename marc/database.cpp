#include "database.h"

#include <iostream>
#include <fstream>

namespace
{
void le64(char* buf, std::uint64_t x)
{
    for (std::size_t i = 0; i < 8; ++i)
        buf[i] = static_cast< unsigned char >((x >> (8 * i)) & 0xff);
}

std::uint64_t le64(const char* buf)
{
    std::uint64_t x = 0;

    for (std::size_t i = 0; i < 8; ++i)
        x += static_cast< std::uint64_t >(static_cast< unsigned char >(buf[i])) << (8 * i);
    return x;
}

std::string read(const MemoryMappedFile& map, std::size_t& off)
{
    const char* q = map.data();
    const std::size_t x = le64(q + off);
    off += 8;
    std::string t(q + off, x);
    off += x;
    return t;
}

}

namespace marc
{
class DatabaseFileWriter
{
public:
    explicit DatabaseFileWriter(const std::string& path) : out(path)
    {
        out.seekp(0, std::ios_base::end);
    }

    void write(const std::vector< Collection >& collections)
    {
        for (const auto& c : collections)
        {
            write(c);
        }
    }

    void write(const Collection& c)
    {
        char buf[8];

        out.write("c", 1);
        const auto startc = out.tellp();
        out.write(buf, 8); // reserve 8 bytes for the size of the collection

        for (std::size_t ir = 0, nr = c.size(); ir < nr; ++ir)
        {
            write(c.record(ir));
        }

        const auto endc = out.tellp();
        le64(buf, endc - startc);
        out.seekp(startc);
        out.write(buf, 8);
        out.seekp(endc);
    }

    void write(const Record& r)
    {
        char buf[8];

        out.write("r", 1);
        const auto startr = out.tellp();
        out.write(buf, 8);


        // Write Leaders
        for (std::size_t il = 0, nl = r.num_leaders(); il < nl; ++il)
        {
            write(r.get_leader(il));
        }

        for (std::size_t ic = 0, nc = r.num_controlfields(); ic < nc; ++ic)
        {
            write(r.get_controlfield(ic));
        }

        for (std::size_t id = 0, nd = r.num_datafields(); id < nd; ++id)
        {
            write(r.get_datafield(id));
        }

        const auto endr = out.tellp();
        le64(buf, endr - startr);
        out.seekp(startr);
        out.write(buf, 8);
        out.seekp(endr);
    }

    void write(const Leader& l)
    {
        out.write("l",1);
        write(l.get_content());
    }

    void write(const ControlField& cf)
    {
        out.write("f", 1);

        write(cf.get_tag().to_string());
        write(cf.get_content());
    }

    void write(const DataField& df)
    {
        out.write("d", 1);
        write(df.get_tag().to_string());
        write(to_string(df.get_indicator1()));
        write(to_string(df.get_indicator2()));

        for (std::size_t is = 0, ns = df.num_subfields(); is < ns; ++is)
        {
            write(df.get_subfield(is));
        }
    }

    void write(const SubField& sf)
    {
        out.write("s", 1);
        write(to_string(sf.get_code()));
        write(sf.get_content());
    }

    void write(const std::string_view& s)
    {
        char buf[8];
        const std::size_t n = s.length();
        le64(buf, n);
        out.write(buf, 8);
        out.write(s.data(), n);
    }

private:
    std::ofstream out;
};

void Database::insert(Collection c)
{
    const std::string s = file.get_path();

    {
        DatabaseFileWriter w(s);

        // unmap
        file.close();

        w.write(c);
    }

    // remap
    load(s);
}

std::size_t Database::size(std::size_t i) const
{
    return i < colls.size() ? colls[i].size() : 0;
}

std::string Database::label(std::size_t c, std::size_t r) const
{
    if (c < colls.size() && r < colls[c].size())
        return load_leader(colls[c][r]);

    return {};
}

Record Database::get_record(std::size_t c, std::size_t r) const
{
    if (c < colls.size() && r < colls[c].size())
        return load_record(colls[c][r]);

    return {};
}

std::string Database::load_leader(std::size_t off) const
{
    if (file[off + 9] == 'l')
    {
        off += 10;
        return read(file, off);
    }
    return {};
}

Record Database::load_record(std::size_t off) const
{
    Record rec;

    ++off;
    off += 8;
    while (file[off] == 'l')
    {
        ++off;
        Leader l;
        l.set_content(read(file, off));
        rec.append(std::move(l));
    }

    while (file[off] == 'f')
    {
        ++off;
        ControlField f;
        f.set_tag(Tag(read(file, off)));
        f.set_content(read(file, off));
        rec.append(std::move(f));
    }

    while (file[off] == 'd')
    {
        ++off;
        DataField d;

        d.set_tag(Tag(read(file, off)));
        d.set_indicator1(read(file, off));
        d.set_indicator2(read(file, off));

        while (file[off] == 's')
        {
            ++off;
            SubField s;
            s.set_code(read(file, off));
            s.set_content(read(file, off));
            d.append(std::move(s));
        }

        rec.append(std::move(d));
    }

    return rec;
}

void Database::save(const std::string & path, const std::vector< Collection >& collections)
{
    DatabaseFileWriter w(path);
    w.write(collections);
}

void Database::load(const std::string & path)
{
    MemoryMappedFile map(path);

    const std::size_t n = map.size();

    std::vector< std::vector< std::size_t > > c2;

    std::size_t off = 0;

    while (off < n && map[off] == 'c')
    {
        off += 9;

        std::vector< std::size_t > rs;

        while (off < n && map[off] == 'r')
        {
            rs.push_back(off);
            off += le64(map.data() + off + 1) + 1;
        }

        c2.push_back(std::move(rs));
    }

    colls = std::move(c2);
    file = std::move(map);
}
}

