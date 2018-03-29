#include "indicator_type.h"

namespace marc
{
indicator_type ascii_to_indicator(unsigned char value_)
{
    if (value_ == 32)
        return indicator_type::space;
    else if (48 <= value_ && value_ <= 57)
        return static_cast< indicator_type >(static_cast< unsigned char >(indicator_type::digit0) + (value_ - 48));
    else if (97 <= value_ && value_ <= 122)
        return static_cast< indicator_type >(static_cast< unsigned char >(indicator_type::a) + (value_ - 97));
    return indicator_type::invalid;
}

std::string_view to_string(indicator_type value)
{
    static const char arr[] = "\0 0123456789abcdefghijklmnopqrstuvwxyz";
    return { arr + static_cast< unsigned char >(value), 1 };
}
}
