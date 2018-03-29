#ifndef MARCXML_H
#define MARCXML_H

#include <string>

namespace marc
{
class Collection;

Collection load_xml(const std::string&);
}

#endif // MARCXML_H
