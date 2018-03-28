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

void write(std::ostream& os, const std::string_view& s)
{
    char buf[8];
    const std::size_t n = s.length();
    le64(buf, n);
    os.write(buf, 8);
    os.write(s.data(), n);
}

std::string read(std::istream& is)
{
    char buf[8];
    is.read(buf, 8);
    const std::size_t x = le64(buf);
    char* p = new char[x];
    is.read(p, x);
    std::string t(p, x);
    delete[] p;
    return t;
}

}

namespace marc
{
void Database::insert(Collection c)
{
    collections.push_back(std::move(c));
}

std::size_t Database::size(std::size_t i) const
{
    return i < collections.size() ? collections[i].size() : 0;
}

std::string Database::label(std::size_t c, std::size_t r) const
{
    return c < collections.size() ? collections[c].label(r) : std::string();
}

Record Database::get_record(std::size_t c, std::size_t r) const
{
    return c < collections.size() ? collections[c].record(r) : Record();
}

void Database::save(const std::string & file) const
{
    std::ofstream out(file);

    if (!out.is_open())
        return;

    char buf[8];

    for (const auto& c : collections)
    {
        out.write("c", 1);
        const auto startc = out.tellp();
        out.write(buf, 8); // reserve 8 bytes for the size of the collection

        for (std::size_t ir = 0, nr = c.size(); ir < nr; ++ir)
        {
            out.write("r", 1);
            const auto startr = out.tellp();
            out.write(buf, 8);

            const Record& r = c.record(ir);

            // Write Leaders
            for (std::size_t il = 0, nl = r.num_leaders(); il < nl; ++il)
            {
                out.write("l",1);
                const Leader& l = r.get_leader(il);
                write(out, l.get_content());
            }

            for (std::size_t ic = 0, nc = r.num_controlfields(); ic < nc; ++ic)
            {
                out.write("f", 1);
                const ControlField& cf = r.get_controlfield(ic);

                write(out, cf.get_tag().to_string());
                write(out, cf.get_content());
            }

            for (std::size_t id = 0, nd = r.num_datafields(); id < nd; ++id)
            {
                out.write("d", 1);
                const DataField& df = r.get_datafield(id);
                write(out, df.get_tag().to_string());
                write(out, to_string(df.get_indicator1()));
                write(out, to_string(df.get_indicator2()));

                for (std::size_t is = 0, ns = df.num_subfields(); is < ns; ++is)
                {
                    out.write("s", 1);
                    const SubField& sf = df.get_subfield(is);
                    write(out, to_string(sf.get_code()));
                    write(out, sf.get_content());
                }
            }

            const auto endr = out.tellp();
            le64(buf, endr - startr);
            out.seekp(startr);
            out.write(buf, 8);
            out.seekp(endr);
        }

        const auto endc = out.tellp();
        le64(buf, endc - startc);
        out.seekp(startc);
        out.write(buf, 8);
        out.seekp(endc);
    }
}

void Database::load(const std::string & file)
{
    std::ifstream in(file);

    if (!in.is_open())
        return;

    char buf[8];

    std::vector< Collection > c2;

    while (in.peek() == 'c')
    {
        Collection coll;

        in.get();
        in.read(buf, 8);

        while (in.peek() == 'r')
        {
            in.get();
            Record rec;

            in.read(buf, 8);
            while (in.peek() == 'l')
            {
                in.get();
                Leader l;
                l.set_content(read(in));
                rec.insert(std::move(l));
            }

            while (in.peek() == 'f')
            {
                in.get();
                ControlField f;
                f.set_tag(read(in));
                f.set_content(read(in));
                rec.insert(std::move(f));
            }

            while (in.peek() == 'd')
            {
                in.get();
                DataField d;

                d.set_tag(read(in));
                d.set_indicator1(read(in));
                d.set_indicator2(read(in));

                while (in.peek() == 's')
                {
                    in.get();
                    SubField s;
                    s.set_code(read(in));
                    s.set_content(read(in));
                    d.insert(std::move(s));
                }

                rec.insert(std::move(d));
            }

            coll.insert(std::move(rec));
        }
        c2.push_back(std::move(coll));
    }

    c2.swap(collections);
}
}

