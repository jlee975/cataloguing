#ifndef INDICATOR_TYPE_H
#define INDICATOR_TYPE_H

#include <string_view>

namespace marc
{
enum class indicator_type : unsigned char { invalid, space, digit0, digit1, digit2, digit3, digit4, digit5, digit6, digit7, digit8, digit9, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, num_indicators };
indicator_type ascii_to_indicator(unsigned char value_);
std::string_view to_string(indicator_type value);
}

#endif // INDICATOR_TYPE_H
