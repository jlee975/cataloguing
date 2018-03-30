#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <string>
#include <string_view>
#include <vector>

#include "indicator_type.h"
#include "subfield_code.h"

namespace marc
{

struct indicator_descriptor
{
    struct info
    {
        indicator_type type;
        std::string description;
    };

    const info& find(indicator_type) const;
    std::string friendly(indicator_type) const;

    std::string description;

    /// @todo flatmap
    std::vector< info > options_;
};

struct subfield_code_descriptor
{
    subfield_code code;
    std::string description;
    bool repeatable;
};

struct field_descriptor
{
    const subfield_code_descriptor& find(subfield_code) const;

    std::string code;
    std::string description;
    std::string full;
    indicator_descriptor first_indicator;
    indicator_descriptor second_indicator;

    /// @todo flatmap
    std::vector< subfield_code_descriptor > subfield_codes;
};

const field_descriptor& get_field_descriptor(const std::string_view&);
}

#endif // DEFINITIONS_H
