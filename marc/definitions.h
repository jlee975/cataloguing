#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <string>
#include <string_view>

namespace marc
{
struct field_descriptor
{
    std::string code;
    std::string description;
};

const field_descriptor& get_field_descriptor(const std::string_view&);
}

#endif // DEFINITIONS_H
