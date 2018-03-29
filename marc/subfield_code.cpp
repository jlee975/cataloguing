#include "subfield_code.h"

namespace marc
{
subfield_code ascii_to_subfield_code(unsigned char value_)
{
    if (33 <= value_ && value_ <= 63)
        return static_cast< subfield_code >(static_cast< unsigned char >(subfield_code::exclamation) + (value_ - 33));
    else if (65 <= value_ && value_ <= 123)
        return static_cast< subfield_code >(static_cast< unsigned char >(subfield_code::A) + (value_ - 65));
    else if (125 <= value_ && value_ <= 126)
        return static_cast< subfield_code >(static_cast< unsigned char >(subfield_code::rightbrace) + (value_ - 125));
    return subfield_code::invalid;
}
std::string_view to_string(subfield_code value)
{
    static const char arr[] = "\0!\"#$%&'()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{}~";

    return { arr + static_cast< unsigned char >(value), 1 };
}
}
