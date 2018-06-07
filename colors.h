#ifndef GUT_COLORS_H
#define GUT_COLORS_H

#include "utils.h"

namespace gut {

namespace color {

// enable/disable colors in console
struct ColorInConsole_ {};
typedef StaticFlag<ColorInConsole_> ColorInConsole;

#define GUT_ENABLE_COLORINCONSOLE \
    gut::color::ColorInConsole colorInConsole_;

} // namespace color

} // namespace gut

#ifdef _WIN32

    #include "windows/colors.h"

#elif __linux__

    #include "linux/colors.h"

#endif

#endif // GUT_COLORS_H
